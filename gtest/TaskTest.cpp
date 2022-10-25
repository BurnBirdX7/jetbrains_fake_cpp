#include <gtest/gtest.h>

#include <fstream>
#include <list>
#include <string>
#include <utility>
#include <thread>
#include <chrono>

#include "Task.hpp"

using namespace std::chrono;

auto name1 = "my-task-1";
auto name2 = "my-task-2";
auto test1_doc = YAML::LoadFile("test1.yaml");
auto out = std::ofstream("output.txt");

Task defaultTask() {
    return Task{name1};
}

std::pair<Task, Task> default2Tasks()
{
    return {Task{name1}, Task{name2}};
}

TEST(TaskClass, BasicChecks) {
    auto task = defaultTask();

    ASSERT_EQ(task.name(), name1);
    ASSERT_EQ(task.cmd(), "");
    ASSERT_TRUE(task.target().empty());
    ASSERT_FALSE(task.targetExists());
    ASSERT_ANY_THROW(auto t = task.targetTime());
}

TEST(TaskClass, TargetExistance) {
    auto non_existent_target = "target1";
    auto existing_target = "target2";
    {   // Create target
        std::ofstream stream(existing_target);
        ASSERT_TRUE(stream);
        stream << "test";
    }

    auto [task1, task2] = default2Tasks();

    task1.setTarget(non_existent_target);
    ASSERT_FALSE(task1.targetExists());
    ASSERT_ANY_THROW(auto t = task1.targetTime());
    ASSERT_EQ(task1.target(), non_existent_target);

    task2.setTarget(existing_target);
    ASSERT_TRUE(task2.targetExists());
    ASSERT_NO_THROW(auto t = task2.targetTime());
    auto time = task2.targetTime().time_since_epoch().count();
    out << "Task 2 time: " << time  << ", " << "\n";
    ASSERT_EQ(task2.target(), existing_target);

    ASSERT_FALSE(task1 < task2);
    ASSERT_TRUE(task2 < task1);

    std::filesystem::remove(existing_target);
}

TEST(TaskClass, OutputOperator) {
    Task task{"task"};
    task.setCmd("g++ --version");
    auto shared_task = std::make_shared<Task>("shared task");
    shared_task->setCmd("clang --version");

    ASSERT_NO_THROW(out << task << '\n' << shared_task << "\n");
    EXPECT_FALSE(out.bad());
}

TEST(TaskClass, TaskFromYAML) {
    auto yaml_doc = YAML::LoadFile("test1.yaml");

    ASSERT_THROW(Task::fromYaml("incorrect_task1", yaml_doc), std::runtime_error);
    ASSERT_THROW(Task::fromYaml("incorrect_task2", yaml_doc), std::runtime_error);
    ASSERT_THROW(Task::fromYaml("incorrect_task3", yaml_doc), std::runtime_error);

    ASSERT_THROW(Task::fromYaml("no_run", yaml_doc), std::runtime_error);

    EXPECT_NO_THROW(Task::fromYaml("no_target", yaml_doc));
    EXPECT_NO_THROW(Task::fromYaml("no_dependencies", yaml_doc));

    std::list<std::pair<std::string, size_t>> tasks = {
            {"no_dependencies"      , 0},
            {"one_dependency_scalar", 1},
            {"one_dependency_list"  , 1},
            {"four_dependencies"    , 4}
    };

    for (auto const& [name, amount] : tasks) {
        auto [task, list] = Task::fromYaml(name, yaml_doc);
        ASSERT_EQ(list.size(), amount);
    }
}

TEST(TaskClass, NoTargetStatus) {
    auto [task, dep_list] = Task::fromYaml("no_target", test1_doc);
    ASSERT_EQ(task->status(), Task::Status::NEEDS_UPDATING);
}

TEST(TaskClass, NoDependenciesStatus) {
    {
        {   // Create and write
            std::ofstream stream{"no_dependencies.out"};
            ASSERT_TRUE(stream);
            stream << "test";
        }
        auto [task, dep_list] = Task::fromYaml("no_dependencies", test1_doc);
        ASSERT_EQ(task->status(), Task::Status::UP_TO_DATE);
        std::filesystem::remove("no_dependencies.out");
    }

    {
        auto [task, dep_list] = Task::fromYaml("no_dependencies", test1_doc);
        ASSERT_EQ(task->status(), Task::Status::NEEDS_UPDATING);
    }

}

TEST(TaskClass, EnqueuedStatus) {
    /*
     * If task was enqueued, it should have status ENQUEUED
     * And this should be reversible
     */
    auto task = defaultTask();
    auto status = task.status();

    task.setEnqueued();
    ASSERT_EQ(task.status(), Task::Status::ENQUEUED);

    task.setEnqueued(false);
    ASSERT_EQ(task.status(), status);
}

void dep_eval(Task::ptr const& t1, Task::ptr const& t2) {
    t1->evaluateFileDependency("file.txt");
    t1->evaluateTaskDependency(t2);
}

TEST(TaskClass, DependencyEvaluation) {
    /*
     * Call for evaluate[_]Dependency() methods of tasks with no dependencies should result in throw
     */

    auto yaml_doc = YAML::LoadFile("test1.yaml");

    auto [task    , _1] = Task::fromYaml("no_dependencies"      , yaml_doc);
    auto [task_d1 , _2] = Task::fromYaml("one_dependency_scalar", yaml_doc);
    auto [task_dl1, _3] = Task::fromYaml("one_dependency_list"  , yaml_doc);
    auto [task_dl4, _4] = Task::fromYaml("four_dependencies"    , yaml_doc);

    ASSERT_NO_THROW(dep_eval(task_d1, task));
    ASSERT_NO_THROW(dep_eval(task_dl1, task));
    ASSERT_NO_THROW(dep_eval(task_dl4, task));
    ASSERT_THROW(dep_eval(task, task_d1),  std::runtime_error);
}

TEST(TaskClass, TargetTime) {
    // Create targets:
    {   // Older target
        std::ofstream stream{"target1"};
        stream << "t1";
    }
    std::this_thread::sleep_for(1s);
    {   // Newer target
        std::ofstream stream{"target2"};
        stream << "t2";
    }

    // Create task
    auto [task1, task2] = default2Tasks();
    task1.setTarget("target1");
    task2.setTarget("target2");

    ASSERT_NE(task1.status(), Task::Status::NEEDS_UPDATING);
    ASSERT_NE(task2.status(), Task::Status::NEEDS_UPDATING);

    ASSERT_TRUE(task1 < task2);
    ASSERT_FALSE(task2 < task1);

    std::filesystem::remove("target1");
    std::filesystem::remove("target2");
}

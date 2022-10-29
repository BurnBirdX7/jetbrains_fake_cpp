#include <filesystem>
#include <fstream>
#include <chrono>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <ExecutionQueueBuilder.hpp>

auto const config = "test2.yaml";

YAML::Node doc = YAML::LoadFile(config);

TEST(ExecutionQueueBuilderTest, ConfigInValid) {
    ASSERT_TRUE(doc.IsDefined());
}

TEST(ExecutionQueueBuilderTest, BasicBuild) {
    auto builder = ExecutionQueueBuilder(doc);
    builder.build("exec");
    EXPECT_FALSE(builder.failed()) << builder << '\n' << doc;
    EXPECT_EQ(builder.getExecutionQueue().size(), 3) << builder;
}

TEST(ExecutionQueueBuilderTest, BuildMultipleTasks) {
    {   // compile(2) -> build -> exec(1), size shouldn't change
        auto builder = ExecutionQueueBuilder(doc);

        builder.build("exec");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3);

        builder.build("compile");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3);
    }

    {   // compile(1) -> build -> exec(2), size SHOULD change
        auto builder = ExecutionQueueBuilder(doc);

        builder.build("compile");
        EXPECT_EQ(builder.getExecutionQueue().size(), 1) << builder;

        builder.build("exec");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3) << builder;
    }
}

using exec_queue = ExecutionQueueBuilder::exec_queue;

std::optional<size_t> getIndex(exec_queue const& queue, std::string const& name) {
    size_t counter = 0;
    auto it = queue.begin();
    while (it != queue.end()) {
        if (it->get()->name() == name)
            return counter;
        ++it, ++counter;
    }

    return {};
}

TEST(ExecutionQueueBuilderTest, BuildDifferentBranches) {
    auto builder = ExecutionQueueBuilder(doc);
    auto const& queue = builder.getExecutionQueue();

    builder.build("leaf2");
    EXPECT_FALSE(builder.failed()) << builder;

    auto n = queue.size();
    builder.build("publish");
    EXPECT_FALSE(builder.failed()) << builder;
    EXPECT_EQ(queue.size(), n) << queue.size(); // Size not changed

    auto leaf_n = getIndex(queue, "leaf2");
    auto publ_n = getIndex(queue, "publish");
    ASSERT_TRUE(leaf_n.has_value());
    ASSERT_TRUE(publ_n.has_value());
    EXPECT_LT(*publ_n, *leaf_n);

    builder.build("leaf1");
    EXPECT_FALSE(builder.failed()) << builder;
    EXPECT_EQ(queue.size(), n + 1) << queue.size();

    builder.build("exec");
    EXPECT_EQ(queue.size(), n + 4) << queue.size() << '\n' << builder;
}

TEST(ExecutionQueueBuilderTest, ExecuteOkTask) {
    auto builder = ExecutionQueueBuilder(doc);
    builder.build("exec");

    size_t count = 0;

    auto executor = [&count](Task::ptr const& task) {
        // Count executions, do not print additional info...
        ++count;
        return std::system(task->cmd().c_str());
    };

    auto ret = builder.execute(executor);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(count, 3);
}

TEST(ExecutionQueueBuilderTest, ExecuteFailingTask) {
    auto builder = ExecutionQueueBuilder(doc);
    builder.build("failing_exec");

    size_t count = 0;

    auto executor = [&count](Task::ptr const& task) {
        ++count;
        return std::system(task->cmd().c_str());
    };

    auto ret = builder.execute(executor);
    EXPECT_NE(ret, 0);
    EXPECT_EQ(count, 3);
}

void emitTask(YAML::Emitter& emitter,
              std::string const& name,
              std::vector<std::string> const& dependencies)
{
    emitter << YAML::Key << name;
    emitter << YAML::Value << YAML::BeginMap;
    emitter << YAML::Key << "run" << YAML::Value << "ls";
    emitter << YAML::Key << "target" << YAML::Value << name;
    emitter << YAML::Key << "dependencies" << YAML::Value << YAML::BeginSeq;
    for (auto const& dep : dependencies) {
        emitter << dep;
    }
    emitter << YAML::EndSeq << YAML::EndMap;
}

std::pair<size_t, std::string> generateLongDoc(size_t n, std::string const& filename) {
    YAML::Emitter emitter;

    emitter << YAML::BeginMap;
    std::string prev_task = "task_0";
    emitTask(emitter, prev_task, {"test2.yaml"});
    for (size_t i = 1; i <= n; ++i) { // Generate 50 * 3
        std::string task = "task_" + std::to_string(i);
        std::string left_task = task + "_left";
        std::string right_task = task + "_right";

        emitTask(emitter, right_task, {prev_task});
        emitTask(emitter, left_task, {prev_task});
        emitTask(emitter, task, {left_task, right_task});

        prev_task = task;

        /*
         * Builds dependencies:
         *                 /---left_task<----\
         * last_task <---<|                  |<---task <---
         *                \---right_task<---/
         *
         */
    }
    emitter << YAML::EndMap;

    std::ofstream file(filename);
    file << emitter.c_str();

    return {n * 3 + 1, prev_task};
}

TEST(ExecutionQueueBuilderTest, BuildFromSuperlongFile) {
    const std::string FILENAME = "long_conf.yaml";
    const size_t TASKS = 50;
    auto [n, task] = generateLongDoc(50, FILENAME);
    auto long_doc = YAML::LoadFile(FILENAME);

    auto builder = ExecutionQueueBuilder(long_doc);

    auto begin = std::chrono::steady_clock::now();
    builder.build("task_" + std::to_string(TASKS / 2));
    builder.build(task);
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "\t\t\tExecution queue build time: " << dur.count() << " ms\n";

    ASSERT_FALSE(builder.failed());
    ASSERT_EQ(builder.getExecutionQueue().size(), n);
}

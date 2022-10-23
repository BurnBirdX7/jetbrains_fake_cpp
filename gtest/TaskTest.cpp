#include <gtest/gtest.h>
#include <fstream>
#include "Task.hpp"

auto name1 = "my-task-1";
auto name2 = "my-task-2";

Task defaultTask() {
    return Task{name1};
}

std::pair<Task, Task> default2Tasks() {
    return {Task{name1}, Task{name2}};
}

TEST(TaskTest, BasicChecks) {
    auto task = defaultTask();

    EXPECT_EQ(task.name(), name1);
    EXPECT_EQ(task.cmd(), "");
    EXPECT_EQ(task.target(), std::filesystem::path{});
    EXPECT_EQ(task.time(), std::nullopt);
}

TEST(TaskTest, Target) {
    auto non_existent_target = "target1";
    auto existing_target = "target2";
    {
        std::ofstream ofstream(existing_target);
        ofstream << "test";
    }

    auto [task1, task2] = default2Tasks();

    task1.setTarget(non_existent_target);
    EXPECT_EQ(task1.time(), std::nullopt);
    EXPECT_EQ(task1.target(), non_existent_target);

    std::cout << std::filesystem::current_path() << std::endl;

    task2.setTarget(existing_target);
    EXPECT_NE(task2.time(), std::nullopt);
    EXPECT_EQ(task2.target(), existing_target);

    EXPECT_LT(task1, task2);
    EXPECT_LT(task2, task1);
}

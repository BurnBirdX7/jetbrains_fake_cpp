
#include <filesystem>

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



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
    {   // compile -> build -> exec
        auto builder = ExecutionQueueBuilder(doc);

        builder.build("exec");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3);

        builder.build("compile");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3);
    }

    {
        auto builder = ExecutionQueueBuilder(doc);

        builder.build("compile");
        EXPECT_EQ(builder.getExecutionQueue().size(), 1) << builder;

        builder.build("exec");
        EXPECT_EQ(builder.getExecutionQueue().size(), 3) << builder;
    }

}

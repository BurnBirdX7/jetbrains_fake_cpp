#include <cstdlib>
#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

#include "ExecutionQueueBuilder.hpp"

const auto CONFIG_FILE = "./fake.yaml";

enum ReturnCodes {
    OK = 0,
    INCORRECT_CALL,
    DEPENDENCY_ERROR,
    EXECUTION_ERROR
};

int main(int argc, char** argv)
{
    if (!std::filesystem::exists(CONFIG_FILE)) {
        std::cerr << CONFIG_FILE << " does not exist!\n";
        return INCORRECT_CALL;
    }

    if (argc < 2) {
        std::filesystem::path self_path = argv[0];

        std::cerr << "Not enough arguments...\n"
            << self_path.filename().string() << " <task>\n";
        return INCORRECT_CALL;
    }

    auto node = YAML::LoadFile(CONFIG_FILE);

    auto builder = ExecutionQueueBuilder(node);
    builder.build(argv[1]);
    if (builder.failed()) {
        std::cerr << builder;
        return DEPENDENCY_ERROR;
    }

    auto executor = [](Task::ptr const& task) -> int {
        std::cout << " > " << task << std::endl; // Force output before execution of cmd to avoid
        return std::system(task->cmd().c_str());
    };

    if (builder.execute(executor)) {
        std::cerr << "Task failed...\n";
        return EXECUTION_ERROR;
    }

    std::cout << "Finished!\n";

    return OK;
}

#include <cstdlib>
#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

#include "TaskBuilder.hpp"

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

    auto builder = TaskBuilder(node);
    builder.build(argv[1]);
    if (builder.failed()) {
        std::cerr << builder;
        return DEPENDENCY_ERROR;
    }

    std::cout << "Execution:\n";
    for (auto const& task : builder.getExecutionQueue()) {
        std::cout << " > " << task << std::endl; // Force output with `endl`

        auto ret = std::system(task->cmd().c_str());
        if (ret != 0) {
            std::cerr << "Task failed!";
            return EXECUTION_ERROR;
        }
    }

    return OK;
}

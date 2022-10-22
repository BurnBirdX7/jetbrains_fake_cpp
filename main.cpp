#include <cstdlib>
#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

#include "TaskBuilder.hpp"

const auto CONFIG_FILE = "./fake.yaml";


int main(int argc, char** argv)
{
    if (!std::filesystem::exists(CONFIG_FILE)) {
        std::cerr << CONFIG_FILE << " does not exist!\n";
        return -1;
    }

    if (argc < 2) {
        std::filesystem::path self_path = argv[0];

        std::cerr << "Not enough arguments...\n"
            << self_path.filename().string() << " <task>\n";
        return -1;
    }

    auto node = YAML::LoadFile(CONFIG_FILE);

    auto builder = TaskBuilder(node);
    builder.build(argv[1]);
    if (builder.failed()) {
        std::cerr << builder;
        return -1;
    }

    std::cout << "Execution:\n";
    for (auto const& task : builder.getExecutionQueue()) {
        std::cout << "  [" << task->name() << "] cmd: " << task->run() << std::endl;

        auto ret = std::system(task->run().c_str());
        if (ret != 0) {
            std::cerr << "\nTask failed!\n";
            return -1;
        }

    }

    return 0;
}

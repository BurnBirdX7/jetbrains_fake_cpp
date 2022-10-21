#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

#include "Task.hpp"


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
    std::list<std::string> dep_stack{};

    try {
        Task task = Task::task_from_yaml(argv[1], node, dep_stack);
        std::cout << task;
    }
    catch (CyclicDependency const& error) {
        std::cerr << error.what() << "\n";
        std::cerr << "dependency stack:\n";
        std::cerr << "  * " << error.get_target_name() << '\n';
        for (auto const& dep : dep_stack) {
            std::cerr << "  - " << dep << "\n";
        }
        return -2;
    }
    catch (TargetError const& error) {
        std::cerr << error.get_target_name() << ":\t" << error.what();
        return -2;
    }
    catch (std::runtime_error const& error) {
        std::cerr << "Error occurred:\n  " << error.what() << "\n";
        return -2;
    }
    catch (...) {
        std::cerr << "Unexpected error\n";
        return -3;
    }

    return 0;
}

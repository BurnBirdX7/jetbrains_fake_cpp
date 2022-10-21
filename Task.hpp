#ifndef JETBRAINS_FAKE_CPP_TASK_HPP
#define JETBRAINS_FAKE_CPP_TASK_HPP

#include <filesystem>
#include <optional>
#include <iostream>
#include <list>

#include <yaml-cpp/yaml.h>

class Task {
public:
    using time_type = std::filesystem::file_time_type;
    using path = std::filesystem::path;
    using file_dep = std::pair<path, time_type>;
    using dep_stack = std::list<std::string>;

    explicit Task(std::string name);

    void setTarget(std::string const& target);
    void setRun(std::string const& str);
    void addDependency(std::string const& name, YAML::Node const& doc, dep_stack&);

    [[nodiscard]] bool ok() const;

    static Task task_from_yaml(std::string const& name, YAML::Node const& node, dep_stack&);

    friend std::ostream& operator<<(std::ostream& out, Task const& task);


private:
    std::string name_   = {};
    std::string run_    = {};
    path target_        = {};
    std::optional<time_type> time_ = {};
    std::list<file_dep> file_deps_ = {};
    std::list<Task> task_deps_ = {};

};


class TargetError
    : public std::runtime_error {
public:
    TargetError(std::string const& msg, std::string name);
    [[nodiscard]] std::string const& get_target_name() const;

private:
    std::string target_name_;
};

class CyclicDependency
        : public TargetError {
public:
    explicit CyclicDependency(std::string name);
};


#endif //JETBRAINS_FAKE_CPP_TASK_HPP

#ifndef JETBRAINS_FAKE_CPP_TASK_HPP
#define JETBRAINS_FAKE_CPP_TASK_HPP

#include <filesystem>
#include <optional>
#include <iostream>
#include <list>

#include <yaml-cpp/yaml.h>

class Task {
public:
    using task_ptr = std::shared_ptr<Task>;
    using time_type = std::filesystem::file_time_type;
    using path = std::filesystem::path;
    using dep_list = std::list<std::string>;

    enum class Status {
        UNKNOWN, UP_TO_DATE, NEEDS_UPDATING, ENQUEUED
    };

    explicit Task(std::string name);

    void setTarget(std::string const& target);
    void setRun(std::string const& str);

    bool checkFileDependency(std::string const& file_name);
    void checkTaskDependency(const task_ptr& task);

    [[nodiscard]] Status getStatus() const;
    void setEnqueued(bool = true);

    [[nodiscard]] std::string const& name() const;
    [[nodiscard]] std::string const& run() const;
    // TODO: Other getters

    static std::pair<task_ptr, dep_list> task_from_yaml(std::string const& name, YAML::Node const& node);
    friend bool operator<(Task const& lhs, Task const& rhs); // returns true if

private:
    std::string name_   = {};
    std::string run_    = {};
    path target_        = {};
    std::optional<time_type> time_ = {};

    bool enqueued_ = false;
    bool has_file_dependencies_ = false;
    bool has_task_dependencies_ = false;
    bool file_updated_ = false;
    bool task_updated_ = false;
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

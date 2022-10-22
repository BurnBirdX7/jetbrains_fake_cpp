#ifndef JETBRAINS_FAKE_CPP_TASK_HPP
#define JETBRAINS_FAKE_CPP_TASK_HPP

#include <filesystem>
#include <optional>
#include <iostream>
#include <list>

#include <yaml-cpp/yaml.h>

class Task {
public:
    // Definitions:
    using ptr = std::shared_ptr<Task>;
    using time_type = std::filesystem::file_time_type;
    using path = std::filesystem::path;
    using dep_list = std::list<std::string>;

    enum class Status {
        UNKNOWN, UP_TO_DATE, NEEDS_UPDATING, ENQUEUED
    };

    explicit Task(std::string name);

    // Change state:
    void setTarget(std::string const& target);
    void setCmd(std::string const& str);
    void setEnqueued(bool = true);

    bool checkFileDependency(std::string const& file_name);
    void checkTaskDependency(const ptr& task);

    [[nodiscard]] Status status() const;

    [[nodiscard]] std::string const& name() const;
    [[nodiscard]] std::string const& cmd() const;
    [[nodiscard]] path const& target() const;
    [[nodiscard]] std::optional<time_type> const& time() const;

    static std::pair<ptr, dep_list> task_from_yaml(std::string const& name, YAML::Node const& node);
    friend bool operator<(Task const& lhs, Task const& rhs);
    friend std::ostream& operator<<(std::ostream& out, ptr const& task);

private:
    // General information about the task
    std::string name_   = {};
    std::string cmd_    = {};
    path target_        = {};
    std::optional<time_type> time_ = {};

    // Information about dependencies
    bool has_file_dependencies_ = false;
    bool has_task_dependencies_ = false;
    bool file_updated_ = false;
    bool task_updated_ = false;
    bool enqueued_ = false;
};


#endif //JETBRAINS_FAKE_CPP_TASK_HPP

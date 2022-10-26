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
        UP_TO_DATE, NEEDS_UPDATING, ENQUEUED
    };

    explicit Task(std::string name);
    Task(Task const&) = default;
    Task(Task&&) = default;
    Task& operator=(Task const&) = default;
    Task& operator=(Task&&) = default;

    // Change state:
    void setTarget(std::string const& target);
    void setCmd(std::string const& str);
    void setEnqueued(bool = true);

    bool evaluateFileDependency(std::string const& file_name);
    void evaluateTaskDependency(const ptr& task);

    [[nodiscard]] Status status() const;

    [[nodiscard]] std::string const& name() const;
    [[nodiscard]] std::string const& cmd() const;
    [[nodiscard]] path const& target() const;
    [[nodiscard]] bool targetExists() const;
    [[nodiscard]] time_type const& targetTime() const; // Will throw if target doesn't exist

    static std::pair<ptr, dep_list> fromYaml(std::string const& name, YAML::Node const& node);
    friend bool operator<(Task const& lhs, Task const& rhs); // Lesser -> Older
    friend std::ostream& operator<<(std::ostream& out, Task const& task);
    friend std::ostream& operator<<(std::ostream& out, ptr const& task);

private:
    // General information about the task
    std::string name_   = {};
    std::string cmd_    = {};
    path target_        = {};
    std::optional<time_type> time_ = {};

    // Information about dependencies
    bool has_dependencies_ = false;
    bool dependency_is_updated_ = false;
    bool enqueued_ = false;
};


#endif //JETBRAINS_FAKE_CPP_TASK_HPP

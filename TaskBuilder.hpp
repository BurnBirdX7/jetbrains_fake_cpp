#ifndef JETBRAINS_FAKE_CPP_TASKBUILDER_HPP
#define JETBRAINS_FAKE_CPP_TASKBUILDER_HPP

#include <list>
#include <string>
#include <yaml-cpp/yaml.h>
#include <map>
#include "Task.hpp"

class TaskBuilder {
public:
    using error_log = std::list<std::string>;

    using dep_stack = std::list<Task::task_ptr>;
    using dep_queue = std::list<std::pair<std::string /* task/file */, Task::task_ptr /* dependant */>>;
    using task_map = std::map<std::string /*name*/, Task::task_ptr /*task*/>;

    using exec_queue = std::list<Task::task_ptr>;

    explicit TaskBuilder(YAML::Node const& doc);

    void build(const std::string& task_name);

    [[nodiscard]] bool failed() const;
    [[nodiscard]] exec_queue const& getExecutionQueue() const;

    friend std::ostream& operator<<(std::ostream& out, TaskBuilder const& builder);


private:
    bool fixStack(Task::task_ptr const& parent, std::string const& child_name);
    void addTask(std::string const& name);
    void addError(std::string const& task_name, std::string const& msg);

    YAML::Node const& doc_;
    Task::task_ptr target_;

    dep_queue queue_;
    dep_stack dependence_stack_;
    task_map constructed_tasks_;
    exec_queue execution_queue_;

    error_log errors_;
};


#endif //JETBRAINS_FAKE_CPP_TASKBUILDER_HPP

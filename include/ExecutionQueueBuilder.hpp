#ifndef JETBRAINS_FAKE_CPP_EXECUTIONQUEUEBUILDER_HPP
#define JETBRAINS_FAKE_CPP_EXECUTIONQUEUEBUILDER_HPP

#include <list>
#include <string>
#include <map>
#include <functional>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "Task.hpp"

class ExecutionQueueBuilder {
public:
    using error_log = std::list<std::string>;

    using task_ptr = Task::ptr;
    using dep_stack = std::list<task_ptr>;
    using dep_queue = std::list<std::pair<std::string /* task/file */, task_ptr /* dependant/parent */>>;
    using task_map = std::map<std::string /*name*/, task_ptr /*task*/>;
    using exec_queue = std::list<task_ptr>;

    explicit ExecutionQueueBuilder(YAML::Node const& doc);

    void build(std::string const& task_name);
    int execute(std::function<int (task_ptr)> const& executor);

    [[nodiscard]] bool failed() const;
    [[nodiscard]] exec_queue const& getExecutionQueue() const;

    friend std::ostream& operator<<(std::ostream& out, ExecutionQueueBuilder const& builder);

private:
    void unfoldStack(task_ptr const& up_to); // O(log n)
    bool stackContains(std::string const& task_name) const;
    void addTask(std::string const& name); // O(1)
    void addError(std::string const& task_name, std::string const& msg); // O(1)

    [[nodiscard]] std::optional<task_ptr> getTaskIfConstructed(std::string const& name) const;
    [[nodiscard]] bool isTaskConstructed(std::string const& name) const;

    YAML::Node const& doc_;

    dep_queue processing_queue_;
    dep_stack dependency_stack_;
    task_map constructed_tasks_;
    exec_queue execution_queue_;

    error_log errors_;
};


#endif //JETBRAINS_FAKE_CPP_EXECUTIONQUEUEBUILDER_HPP

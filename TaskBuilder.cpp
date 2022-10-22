#include "TaskBuilder.hpp"

#include <filesystem>

TaskBuilder::TaskBuilder(YAML::Node const& doc)
    : doc_(doc)
{}

void TaskBuilder::build(std::string const& task_name)
{
    task_ptr root_task{};
    Task::dep_list root_deps{};

    try {
        std::tie(root_task, root_deps) = Task::task_from_yaml(task_name, doc_);
    }
    catch (std::runtime_error const& err) {
        addError(task_name, err.what());
        return;
    }

    for (auto const& name : root_deps) {
        queue_.emplace_back(name, root_task);
    }

    constructed_tasks_[task_name] = root_task;
    dependence_stack_.push_front(root_task);

    while (!queue_.empty()) {
        // Queue operations: take first in queue.
        auto [name, parent] = queue_.front();
        queue_.pop_front();

        if (!fixStack(parent, name)) {
            addError(name, "Self-dependence");
            return; // Self-dependence stops processing
        }

        auto already_constructed = constructed_tasks_.find(name);
        if (already_constructed != constructed_tasks_.end()) {
            dependence_stack_.push_front(already_constructed->second);
            continue;
        }

        auto curr_node = doc_[name];
        if (!curr_node.IsDefined()) { // There's no such root_task, but maybe it's a file
            if (!parent->checkFileDependency(name)) { // File doesn't exist...
                addError(name, "Is not a task or a file");
            }
        }
        else { // There's a doc entry
            addTask(name);
        }

    }

    fixStack({}, {});

}

bool TaskBuilder::fixStack(task_ptr const& parent, std::string const& child_name)
{
    while(!dependence_stack_.empty() && dependence_stack_.front() != parent) {
        auto front = constructed_tasks_[dependence_stack_.front()->name()];

        if (front->status() == Task::Status::NEEDS_UPDATING) {
            execution_queue_.push_back(front);
            front->setEnqueued();
        }

        dependence_stack_.pop_front();
        if (!dependence_stack_.empty())
            dependence_stack_.front()->checkTaskDependency(front);
    }

    auto pred = [child_name] (task_ptr const& task) {
        return task->name() == child_name;
    };

    return std::find_if(dependence_stack_.begin(), dependence_stack_.end(), pred) == dependence_stack_.end();
}

bool TaskBuilder::failed() const
{
    return !errors_.empty();
}

void TaskBuilder::addTask(std::string const& name)
{
    try {

        auto [task, deps] = Task::task_from_yaml(name, doc_);

        dependence_stack_.push_front(task);
        constructed_tasks_[task->name()] = task;

        for (auto const& dep_name: deps) {
            queue_.emplace_front(dep_name, task);
        }
    }
    catch (std::runtime_error const& err) {
        addError(name, err.what());
    }
}

TaskBuilder::exec_queue const& TaskBuilder::getExecutionQueue() const
{
    return execution_queue_;
}

std::ostream& operator<<(std::ostream& out, TaskBuilder const& builder)
{
    if (!builder.failed())
        return out << "OK\n";

    out << "Error(s) occurred while building dependencies:\n";

    for (auto const& err : builder.errors_) {
        out << "  - " << err << '\n';
    }

    if (builder.dependence_stack_.empty())
        return out;

    out << "Dependency stack:\n";
    for (auto const& dep : builder.dependence_stack_) {
        out << "  - " << dep->name() << '\n';
    }

    return out;
}

void TaskBuilder::addError(std::string const& task_name, std::string const& msg)
{
    errors_.emplace_back(task_name + ": " + msg);
}


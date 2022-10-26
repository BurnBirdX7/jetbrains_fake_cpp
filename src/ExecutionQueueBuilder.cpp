#include "ExecutionQueueBuilder.hpp"

#include <algorithm>
#include <filesystem>

ExecutionQueueBuilder::ExecutionQueueBuilder(YAML::Node const& doc)
    : doc_(doc)
{}

void ExecutionQueueBuilder::build(std::string const& task_name)
{
    if (isTaskConstructed(task_name))
        return;

    task_ptr root_task{};
    Task::dep_list root_deps{};

    try {
        std::tie(root_task, root_deps) = Task::fromYaml(task_name, doc_);
    }
    catch (std::runtime_error const& err) {
        // stop if we can't process target task
        addError(task_name, err.what());
        return;
    }

    // O(n)
    for (auto const& name : root_deps) {
        processing_queue_.emplace_back(name, root_task);
    }

    constructed_tasks_[task_name] = root_task; // O(log n)
    dependency_stack_.push_front(root_task);

    while (!processing_queue_.empty()) { // O(n log n)
        auto [name, parent] = processing_queue_.front();
        processing_queue_.pop_front();

        unfoldStack(parent);
        if (stackContains(name)) { // O (log n)
            addError(name, "Self-dependency");
            return; // Self-dependence stops processing
        }

        auto already_constructed = getTaskIfConstructed(name);
        if (already_constructed) {
            dependency_stack_.push_front(*already_constructed);
            continue;
        }

        auto curr_node = doc_[name];
        if (!curr_node.IsDefined()) { // There's no such root_task, but maybe it's a file

            /*
             * We don't need to build dependency tree if we meet file dependency,
             * and we can immediately tell if we need to update parent task or not
             */

            if (!parent->evaluateFileDependency(name)) { // File doesn't exist...
                addError(name, "Is not a task or a file");
            }
        }
        else { // There's a doc entry

            /*
             * For task-dependencies we can't tell right away if the dependency is updated or needs updating
             * and [consequently] we don't know if we should update current task.
             *
             * We will know for sure when we will unfold dependency stack
             */

            addTask(name);
        }

    }

    unfoldStack({});
}

int ExecutionQueueBuilder::execute(std::function<int (task_ptr)> const& executor)
{
    for (auto const& task : execution_queue_) {
        auto ret = executor(task);
        if (ret)
            return ret;
    }

    return 0;
}

void ExecutionQueueBuilder::unfoldStack(task_ptr const& up_to)
{
    while(!dependency_stack_.empty() && dependency_stack_.front() != up_to) { // O(log n)
        auto current = dependency_stack_.front();

        if (current->status() == Task::Status::NEEDS_UPDATING) {
            /*
             * If current task on the top of dependency should be updated
             * We enqueue it for execution
             */
            execution_queue_.push_back(current);
            current->setEnqueued();
        }

        /*
         * If there's a task in stack that is "below" current task,
         * this "below" task is dependent on the current task
         */
        dependency_stack_.pop_front();
        if (!dependency_stack_.empty())
            dependency_stack_.front()->evaluateTaskDependency(current);
    }
}

bool ExecutionQueueBuilder::stackContains(std::string const& task_name) const
{
    auto pred = [&task_name] (task_ptr const& task) -> bool {
        return task_name == task->name();
    };

    auto const& first = dependency_stack_.begin();
    auto const& last = dependency_stack_.end();

    return std::find_if(first, last, pred) != last;
}

bool ExecutionQueueBuilder::failed() const
{
    return !errors_.empty();
}

void ExecutionQueueBuilder::addTask(std::string const& name)
{
    /*
     * If there's well-formed task in doc_
     * We construct it, store in constructed_stack_ and
     *    enqueue all of its dependencies for processing
     */

    try {
        auto [task, deps] = Task::fromYaml(name, doc_);

        dependency_stack_.push_front(task);
        constructed_tasks_[task->name()] = task;

        for (auto const& dep_name: deps) {
            processing_queue_.emplace_front(dep_name, task);
        }
    }
    catch (std::runtime_error const& err) {
        addError(name, err.what());
    }
}

ExecutionQueueBuilder::exec_queue const& ExecutionQueueBuilder::getExecutionQueue() const
{
    return execution_queue_;
}

std::ostream& operator<<(std::ostream& out, ExecutionQueueBuilder const& builder)
{
    if (!builder.failed()) {
        if (builder.execution_queue_.empty()) {
            return out << "Nothing to execute\n";
        }

        out << "Execution queue:\n";

        for (auto const& task : builder.execution_queue_) {
            out << "  > " << task << '\n';
        }
        return out;
    }

    out << "Error(s) occurred while building dependencies:\n";
    for (auto const& err : builder.errors_) {
        out << "  - " << err << '\n';
    }

    if (builder.dependency_stack_.empty())
        return out;

    out << "Dependency stack:\n";
    for (auto const& dep : builder.dependency_stack_) {
        out << "  - " << dep->name() << '\n';
    }

    return out;
}

void ExecutionQueueBuilder::addError(std::string const& task_name, std::string const& msg)
{
    errors_.emplace_back('"' + task_name + "\": " + msg);
}

std::optional<Task::ptr> ExecutionQueueBuilder::getTaskIfConstructed(const std::string& name) const {

    auto it = constructed_tasks_.find(name);
    if (it == constructed_tasks_.end())
        return std::nullopt;

    return it->second;
}

bool ExecutionQueueBuilder::isTaskConstructed(std::string const& name) const {
    return constructed_tasks_.find(name) != constructed_tasks_.end();
}


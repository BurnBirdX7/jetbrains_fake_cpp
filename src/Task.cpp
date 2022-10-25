#include "Task.hpp"

#include <utility>
#include <iostream>

// Useful strings
const auto DEPS_KEYWORD = "dependencies";
const auto TARGET_KEYWORD = "target";
const auto CMD_KEYWORD  = "run";


Task::Task(std::string name)
    : name_(std::move(name))
{}

void Task::setTarget(std::string const& target)
{
    target_ = target;

    if (std::filesystem::exists(target_))
        time_ = std::filesystem::last_write_time(target_);
    else
        time_ = std::nullopt;
}

void Task::setCmd(std::string const& str)
{
    cmd_ = str;
}

void Task::setEnqueued(bool b)
{
    enqueued_ = b;
}


bool Task::evaluateFileDependency(std::string const& file_name)
{
    if (!has_dependencies_)
        throw std::runtime_error("This file doesn't have any dependencies...");

    if (!std::filesystem::exists(file_name)) { // There's no such file...
        return false;
    }

    if (targetExists() &&
        targetTime() < std::filesystem::last_write_time(file_name))
    {
        dependency_is_updated_ = true;
    }

    return true;
}

void Task::evaluateTaskDependency(Task::ptr const& task)
{
    if (!has_dependencies_)
        throw std::runtime_error("This task doesn't have any dependencies...");

    if (task->status() == Status::ENQUEUED || *this < *task) {
        dependency_is_updated_ = true;
    }
}

Task::Status Task::status() const
{
    if (enqueued_)
        return Status::ENQUEUED;

    if (!targetExists() || dependency_is_updated_) // No target or there's updated dep
        return Status::NEEDS_UPDATING;

    // -> target exists && there's no updated dependency

    return Status::UP_TO_DATE;
}

std::string const& Task::name() const
{
    return name_;
}

std::string const& Task::cmd() const
{
    return cmd_;
}

std::filesystem::path const& Task::target() const
{
    return target_;
}

bool Task::targetExists() const
{
    return time_.has_value();
}

Task::time_type const& Task::targetTime() const
{
    return time_.value();
}

std::pair<Task::ptr, std::list<std::string>> Task::fromYaml(std::string const& name, YAML::Node const& doc)
{
    auto node = doc[name];
    if (!node.IsDefined())
        throw std::runtime_error("There's no task with this name");

    if (!node.IsMap())
        throw std::runtime_error("Target description should contain Map");

    // WORK WITH OUR TASK:
    auto task = std::make_shared<Task>(name);

    auto target = node[TARGET_KEYWORD];
    if (target.IsScalar())
        task->setTarget(target.as<std::string>());

    auto run_str = node[CMD_KEYWORD];
    if (!run_str.IsScalar())
        throw std::runtime_error("`run` property MUST be defined");
    task->setCmd(run_str.as<std::string>());

    auto deps = node[DEPS_KEYWORD];

    if (deps.IsScalar()) {
        task->has_dependencies_ = true;
        return {task, {deps.as<std::string>()}};
    }

    if (deps.IsSequence()) {
        task->has_dependencies_ = true;
        auto list = dep_list{};
        for (auto const& dep: deps)
            list.push_back(dep.as<std::string>());

        return {task, list};
    }

    return {task, {}};
}

bool operator<(Task const& lhs, Task const& rhs)
{
    if (!lhs.targetExists()) // lhs can't be older if it doesn't exist
        return false;

    if (!rhs.targetExists()) // rhs is going to be created - it's newer than any existing target
        return true;

    return lhs.targetTime() < rhs.targetTime(); // Compare time of two existing targets
}

std::ostream& operator<<(std::ostream& out, Task const& task)
{
    return out << "[" << task.name_ << "]: " << task.cmd_;
}

std::ostream& operator<<(std::ostream& out, Task::ptr const& task)
{
    return out << *task;
}

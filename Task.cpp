#include "Task.hpp"

#include <utility>
#include <iostream>

// Useful strings
const auto DEPS = "dependencies";
const auto TAGT = "target";
const auto RUN = "run";


/*
 * to_time_t
 * based on: https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
 */
template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP{}
                                                        + system_clock::time_point{});
    return system_clock::to_time_t(sctp);
}


Task::Task(std::string name)
    : name_(std::move(name))
{}

bool Task::checkFileDependency(std::string const& file_name)
{
    if (!std::filesystem::exists(file_name)) { // There's no such file...
        return false;
    }

    has_file_dependencies_ = true;

    if (time_.has_value() &&
        time_.value() < std::filesystem::last_write_time(file_name))
    {
        file_updated_ = true;
    }

    return true;
}


void Task::setTarget(std::string const& target)
{
    target_ = target;

    if (std::filesystem::exists(target_))
        time_ = std::filesystem::last_write_time(target_);
    else
        time_ = std::nullopt;

}

void Task::setRun(std::string const& str)
{
    run_ = str;
}

std::pair<Task::task_ptr, std::list<std::string>> Task::task_from_yaml(std::string const& name, YAML::Node const& doc)
{
    auto node = doc[name];
    if (!node.IsDefined())
        throw std::runtime_error("task_from_yaml: there's no such task");

    if (!node.IsMap())
        throw std::runtime_error("task_from_yaml: Target description should contain Map");

    // WORK WITH OUR TASK:
    auto task = std::make_shared<Task>(name);

    auto target = node[TAGT];
    if (target.IsDefined())
        task->setTarget(target.as<std::string>());

    auto run_str = node[RUN];
    if (!run_str.IsDefined())
        throw std::runtime_error("task_from_yaml: run MUST be defined");
    task->setRun(run_str.as<std::string>());

    auto deps = node[DEPS];
    auto list = dep_list{};
    if (deps.IsSequence())
        for (auto const& dep : deps)
            list.push_back(dep.as<std::string>());

    return {task, list};
}

std::string const& Task::name() const
{
    return name_;
}

Task::Status Task::getStatus() const
{
    if (enqueued_)
        return Status::ENQUEUED;

    if (!has_task_dependencies_ && !has_file_dependencies_ || !time_.has_value() || file_updated_ || task_updated_)
        return Status::NEEDS_UPDATING;

    if (has_task_dependencies_ || has_file_dependencies_)
        return Status::UP_TO_DATE;

    return Status::UNKNOWN;
}

void Task::setEnqueued(bool b)
{
    enqueued_ = b;
}


bool operator<(Task const& lhs, Task const& rhs)
{
    // If we cannot compare the two, we just assume that rhs is updated and lhs is not
    if (!lhs.time_.has_value() || !rhs.time_.has_value())
        return true;

    return lhs.time_.value() < rhs.time_.value();
}

bool Task::checkTaskDependency(Task::task_ptr const& task)
{
    // We believe that we depend on `task`
    has_task_dependencies_ = true;

    if (task->getStatus() == Status::ENQUEUED || *this < *task) {
        task_updated_ = true;
    }

}

std::string const& Task::run() const
{
    return run_;
}

TargetError::TargetError(std::string const& msg, std::string name)
    : std::runtime_error(msg)
    , target_name_(std::move(name))
{
}

std::string const& TargetError::get_target_name() const
{
    return target_name_;
}

CyclicDependency::CyclicDependency(std::string name)
    : TargetError("Cyclic dependency detected", std::move(name))
{}

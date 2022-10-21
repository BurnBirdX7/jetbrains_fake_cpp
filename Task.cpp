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

void Task::addDependency(std::string const& name, YAML::Node const& doc, dep_stack& stack)
{
    auto node = doc[name];
    if (!node.IsDefined()) {
        // Dependency is not listed in the doc
        // Check if it is a file

        if (!std::filesystem::exists(name)) { // There's no such file...
            throw TargetError("There's no such task or file", name);
        }

        file_deps_.emplace_back(name, std::filesystem::last_write_time(name));
    }
    else {
        // Dependency is listed in the doc:
        task_deps_.emplace_back(Task::task_from_yaml(name, doc, stack));
    }
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

bool Task::ok() const
{
    return !(run_.empty() || name_.empty() || target_.empty());
}

Task Task::task_from_yaml(std::string const& name, YAML::Node const& doc, dep_stack& stack)
{
    if (std::find(stack.begin(), stack.end(), name) != stack.end()) {
        throw CyclicDependency(name);
    }
    stack.push_front(name);

    auto node = doc[name];
    if (!node.IsDefined())
        throw std::runtime_error("task_from_yaml: there's no such task");

    if (!node.IsMap())
        throw std::runtime_error("task_from_yaml: Target description should contain Map");

    // WORK WITH OUR TASK:
    auto task = Task(name);

    auto target = node[TAGT];
    if (target.IsDefined())
        task.setTarget(target.as<std::string>());

    auto run_str = node[RUN];
    if (!run_str.IsDefined())
        throw std::runtime_error("task_from_yaml: run MUST be defined");
    task.setRun(run_str.as<std::string>());

    auto deps = node[DEPS];
    if (deps.IsSequence())
        for (auto const& dep : deps)
            task.addDependency(dep.as<std::string>(), doc, stack);

    stack.pop_front();
    return task;
}

std::ostream& operator<<(std::ostream& out, Task const& task)
{
    out << "* " << task.name_ << "\n";
    if (!task.target_.empty())
        out << "  target:  " << task.target_ << "\n";
    out << "  run:     " << task.run_ << "\n";

    if (task.time_.has_value()) {
        auto t = to_time_t(task.time_.value());
        out << "  updated: " << std::ctime(&t);
    }

    if (!task.task_deps_.empty()) {
        out << "  dependencies:\n";
        for (auto const& dep : task.task_deps_)
            out << "    " << dep.name_ << '\n';
    }

    return out << '\n';
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

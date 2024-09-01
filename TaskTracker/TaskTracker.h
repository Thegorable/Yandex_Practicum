#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

enum class TaskStatus {
    NEW,
    IN_PROGRESS,
    TESTING,
    DONE
};

using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return developers_info.at(person);
    }

    void AddNewTask(const string& person) {
        if (!developers_info.count(person)) {
            for (int i = 0; i < 4; i++) {
                developers_info[person][static_cast<TaskStatus>(i)] = 0;
            }
        }
        developers_info[person][TaskStatus::NEW]++;
    }

    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        TasksInfo task_updated;
        TasksInfo task_not_updated;

        if (!developers_info.count(person)) {
            return { task_updated, task_not_updated };
        }

        TasksInfo& tasks = developers_info[person];
        for (TaskStatus i = TaskStatus::NEW; i < TaskStatus::DONE; i = Next(i) ) {
            task_not_updated[i] = tasks.at(i);
        }

        int tasks_to_complete = 0;

        for (TaskStatus status = TaskStatus::NEW; status < TaskStatus::DONE; status = Next(status))
        {
            if (!task_count) {
                break;
            }
            
            tasks_to_complete = tasks[status] - task_updated[status];
            tasks_to_complete = min(tasks_to_complete, task_count);

            tasks[status] -= tasks_to_complete;
            tasks[Next(status)] += tasks_to_complete;

            task_updated[Next(status)] += tasks_to_complete;
            task_not_updated[status] -= tasks_to_complete;

            task_count -= tasks_to_complete;
        }

        return { task_updated, task_not_updated };
    }

private:
    map<string, TasksInfo> developers_info;

    static TaskStatus Next(TaskStatus task_status) {
        return static_cast<TaskStatus>(static_cast<int>(task_status) + 1);
    }
};
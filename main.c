#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"


enum task_status {
    DONE,
    PROCESSING,
    TODO
};

typedef enum task_status task_status;

struct cp_info
{
    int cost;
    int number;
};

struct processor_info
{
    struct _pe processor;
    int current_task;
    int current_cost;
};

typedef struct cp_info cp_info;
typedef struct processor_info processor_info;

int get_exit_task(void)
{
    int exit_task_no;
    exit_task_no = total_task - 1;
    return exit_task_no;
}

int* get_critical_path(void)
{
    int i, j;
    int* cp = malloc(total_task * sizeof(int));
    memset(cp, 0, total_task * sizeof(int));
    cp[get_exit_task()] = task[get_exit_task()].cost;

    for (i = get_exit_task(); i > 0; i--)
    {
        for (j = 0; j < task[i].total_pre; j++)
        {
            if (cp[task[i].pre[j]] < task[task[i].pre[j]].cost + cp[i])
                cp[task[i].pre[j]] = task[task[i].pre[j]].cost + cp[i];
        }
    }
    return cp;
}

int compare_cp_info(const void* p_a, const void* p_b)
{
    const cp_info* a = (cp_info*)p_a;
    const cp_info* b = (cp_info*)p_b;

    if (a->cost == b->cost)
        return 0;
    else if (a->cost > b->cost)
        return -1;
    else
        return 1;
}

cp_info* get_priority_list(void)
{
    int i;
    int* cp = get_critical_path();
    cp_info* priority_list = malloc(total_task * sizeof(cp_info));
    for (i = 0; i < total_task; i++)
    {
        priority_list[i].cost = cp[i];
        priority_list[i].number = i;
    }
    qsort(priority_list, total_task, sizeof(cp_info), &compare_cp_info);
    free(cp);

    return priority_list;
}

int pre_total_cost(struct _task t)
{
    int i;
    int pre_cost = 0;
    for (i = 0; i < t.total_pre; i++)
        pre_cost += task[t.pre[i]].cost;
    return pre_cost;
}

void initialize_tasks(const cp_info* priority_list, task_status* tasks_status)
{
    int task_index;
    int processor_index = 0;
    for (task_index = 1; task_index < total_task && processor_index < total_pe; task_index++)
    {
        struct _task t = task[priority_list[task_index].number];
        if (pre_total_cost(t) == 0)
        {
            pe[processor_index].task_no[0] = priority_list[task_index].number;
            pe[processor_index].task_cost[0] = t.cost;
            tasks_status[priority_list[task_index].number] = DONE;
            processor_index++;
        }
    }
}

task_status* initialize_tasks_status(void)
{
    int i;
    task_status* tasks_status = malloc(total_task * sizeof(task_status));
    tasks_status[0] = DONE;
    for(i = 1; i < total_task; i++)
        tasks_status[i] = TODO;
    return tasks_status;
}

processor_info* initialize_processors_info(void)
{
    int i;
    processor_info* info = malloc(total_pe * sizeof(processor_info));
    for (i = 0; i < total_pe; i++)
    {
        info[i].processor = pe[i];
        info[i].current_cost = pe[i].task_cost[0];
        if(pe[i].task_cost[0] == 0)
            info[i].current_task = 0;
        else
            info[i].current_task = 1;
    }
    return info;
}

int decide_processor(const processor_info* processors_info)
{
    int min_cost = -1;
    int min_cost_processor_no = 0;
    int i;
    for (i = 0; i < total_pe; i++)
    {
        int current_task = processors_info[i].current_task - 1;
        if(current_task >= 0 && pe[i].task_no[current_task] == -1)
            continue;
        if (min_cost == -1 || min_cost > processors_info[i].current_cost)
        {
            min_cost = processors_info[i].current_cost;
            min_cost_processor_no = i;
        }
    }
    return min_cost_processor_no;
}

int is_dependency_done(struct _task task_info, const task_status* tasks_status)
{
    int i;
    for (i = 0; i < task_info.total_pre; i++)
        if (tasks_status[task_info.pre[i]] != DONE)
            return 0;
    return 1;
}

int decide_task(const task_status* tasks_status, const cp_info* priority_list)
{
    int i;
    for (i = 0; i < total_task; i++)
    {
        int task_index = priority_list[i].number;
        if (tasks_status[task_index] == TODO && is_dependency_done(task[task_index], tasks_status))
            return task_index;
    }
    return -1;
}

int get_total_cost(const processor_info* processors_info)
{
    int i;
    int max_cost = processors_info[0].current_cost;
    for (i = 0; i < total_pe; i++)
        if (max_cost < processors_info[i].current_cost)
            max_cost = processors_info[i].current_cost;
    return max_cost;
}

void fix_idle_cost(int processor_index, processor_info* processors_info, int current_cost)
{
    int i;
    for(i = 0; i < total_pe; i++)
    {
        int last_task_index = processors_info[i].current_task - 1;
        if(last_task_index >= 0)
        {
            int last_task_no = pe[i].task_no[last_task_index];
            if(last_task_no == -1)
            {
                int idle_cost = current_cost - processors_info[i].current_cost;
                processors_info[i].current_cost += idle_cost;
                pe[i].task_cost[last_task_index] = idle_cost;
            }
        }
    }
}


int update_info(int task_index, int processor_index, task_status* tasks_status, processor_info* processors_info)
{
    int processor_current_task = processors_info[processor_index].current_task++;
    if (task_index == -1)
    {
        pe[processor_index].task_no[processor_current_task] = -1;
    }
    else
    {
        tasks_status[task_index] = PROCESSING;
        pe[processor_index].task_cost[processor_current_task] = task[task_index].cost;
        pe[processor_index].task_no[processor_current_task] = task[task_index].no;
        processors_info[processor_index].current_cost += task[task_index].cost;
    }
    return processors_info[processor_index].current_cost;
}

int count_working_processors(const processor_info* processors_info)
{
    int i;
    int total_working_processors = 0;
    for(i = 0; i < total_pe; i++)
    {
        int last_task = processors_info[i].current_task - 1;
        if(last_task >= 0 && pe[i].task_no[last_task] != -1)
            total_working_processors++;
    }
    return total_working_processors;
}

void update_tasks_status(int current_cost, task_status* tasks_status, const processor_info* processors_info)
{
    int i;
    for(i = 0; i < total_pe; i++)
    {
        if(processors_info[i].current_cost < current_cost || count_working_processors(processors_info) == 1)
        {
            int last_task_index = processors_info[i].current_task - 1;
            if(last_task_index >= 0)
            {
                int last_task_no = pe[i].task_no[last_task_index];
                tasks_status[last_task_no] = DONE;
            }
        }
    }
}

int all_task_done(const task_status* tasks_status)
{
    int i;
    for (i = 0; i < total_task; i++)
        if(tasks_status[i] != DONE)
            return 0;
    return 1;
}


int get_last_working_processor_cost(const processor_info* processors_info)
{
    int i;
    for(i = 0; i < total_pe; i++)
    {
        int last_task = processors_info[i].current_task - 1;
        if(last_task >= 0 && pe[i].task_no[last_task] != -1)
            return processors_info[i].current_cost;
    }
    return 0;
}

void allocate_tasks(void)
{
    task_status* tasks_status = initialize_tasks_status();
    const cp_info* priority_list = get_priority_list();
    initialize_tasks(priority_list, tasks_status);
    processor_info* processors_info = initialize_processors_info();

    while (!all_task_done(tasks_status))
    {
        int processor_index = decide_processor(processors_info);
        int task_index = decide_task(tasks_status, priority_list);
        int current_cost = update_info(task_index, processor_index, tasks_status, processors_info);
        if(task_index != -1 || count_working_processors(processors_info) == 1)
        {
            if(task_index == -1)
                current_cost = get_last_working_processor_cost(processors_info);
            fix_idle_cost(processor_index, processors_info, current_cost);
            update_tasks_status(current_cost, tasks_status, processors_info);
        }
    }
    total_cost = get_total_cost(processors_info);

    free(tasks_status);
    free((cp_info*)priority_list);
    free(processors_info);
}

void free_resources(void)
{
    int i;
    for (i = 0; i < total_task; i++)
        free(task[i].pre);
    for (i = 0; i < total_pe; i++)
    {
        free(pe[i].task_no);
        free(pe[i].task_cost);
    }

    free(task);
    free(pe);

}

int main(int argc, char* argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/

    allocate_tasks();

    /*scheduling end*/

    output(argv);

    free_resources();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

struct cp_info
{
    int cost;
    int number;
};

struct processor_info {
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

    for(i = get_exit_task(); i > 0; i--)
    {
        for(j = 0; j < task[i].total_pre; j++)
        {
            if(cp[task[i].pre[j]] < task[task[i].pre[j]].cost + cp[i])
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
    for(i = 0; i < total_task; i++)
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
    for(i = 0; i < t.total_pre; i++)
        pre_cost += task[t.pre[i]].cost;
    return pre_cost;
}

void initialize_tasks(const cp_info* priority_list, int* task_is_done)
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
            task_is_done[priority_list[task_index].number] = 1;
            processor_index++;
        }
    }
}

int* initialize_task_is_done(void)
{
    int* task_is_done = malloc(total_task * sizeof(int));
    memset(task_is_done, 0, total_task * sizeof(int));
    task_is_done[0] = 1;
    return task_is_done;
}

processor_info* initialize_processors_info(void)
{
    int i;
    processor_info* info = malloc(total_pe * sizeof(processor_info));
    for(i = 0; i < total_pe; i++)
    {
        info[i].processor = pe[i];
        info[i].current_cost = pe[i].task_cost[0];
        info[i].current_task = 0;
    }
    return info;
}

int decide_processor(const int* task_is_done, const processor_info* processors_info)
{
    int min_cost = processors_info[0].current_cost;
    int min_cost_processor_no = 0;
    int i;
    for(i = 1; i < total_pe; i++)
    {
        if (min_cost > processors_info[i].current_cost)
        {
            min_cost = processors_info[i].current_cost;
            min_cost_processor_no = i;
        }
    }
    return min_cost_processor_no;
}

int is_dependency_done(struct _task task_info, const int* task_is_done)
{
    int i;
    for(i = 0; i < task_info.total_pre; i++)
        if(!task_is_done[task_info.pre[i]])
            return 0;
    return 1;
}

int decide_task(const int* task_is_done, const cp_info* priority_list)
{
    int i;
    for(i = 0; i < total_task; i++)
    {
        int task_index = priority_list[i].number;
        if (!task_is_done[task_index] && is_dependency_done(task[task_index], task_is_done))
            return task_index;
    }
    return -1;
}

int get_total_cost(const processor_info* processors_info)
{
    int i;
    int total = 0;
    for(i = 0; i < total_pe; i++)
        total += processors_info[i].current_cost;
    return total;
}

void update_info(int task_index, int processor_index, int* task_is_done, processor_info* processors_info)
{
    int processor_current_task = processors_info[processor_index].current_task++;
    task_is_done[task_index] = 1;
    pe[processor_index].task_cost[processor_current_task] = task[task_index].cost;
    pe[processor_index].task_no[processor_current_task] = task[task_index].no;
    processors_info[processor_index].current_cost += task[task_index].cost;
}

int all_task_done(const int* task_is_done)
{
    int i;
    for(i = 0; i < total_task; i++)
        if(!task_is_done[i])
            return 0;
    return 1;
}

void allocate_tasks(void)
{
    int* task_is_done = initialize_task_is_done();
    processor_info* processors_info = initialize_processors_info();
    const cp_info* priority_list = get_priority_list();

    while(!all_task_done(task_is_done))
    {
        int processor_index = decide_processor(task_is_done, processors_info);
        int task_index = decide_task(task_is_done, priority_list);
        update_info(task_index, processor_index, task_is_done, processors_info);
    }
    total_cost = get_total_cost(processors_info);

    free(task_is_done);
    free((cp_info*)priority_list);
    free(processors_info);
}

void free_resources(void)
{
    int i;
    for(i = 0; i < total_task; i++)
        free(task[i].pre);
    for(i = 0; i < total_pe; i++)
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

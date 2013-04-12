#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

struct cp_info
{
    int cost;
    int number;
};

typedef struct cp_info cp_info;

int get_exit_task(void)
{
    int exit_task_no;
    exit_task_no = total_task - 1;
    return exit_task_no;
}

int* get_critical_path(void)
{
    int i, j;
    //allocate cp
    int* cp = malloc(total_task * sizeof(int));
    //initialize cp
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
    //for (i = 0; i < 10; i++) 
    //    printf("cp[%d]=%d\n", i, cp[i]);
    return cp;
}

int compare_cp_info(const void* p_a, const void* p_b)
{
    const cp_info* a = (cp_info*)p_a;
    const cp_info* b = (cp_info*)p_b;
    //cp_info a = *casted_p_a;
    //cp_info b = *casted_p_b;

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

    //for (i = 0; i < total_task; i++)
    //{
    //    printf("%d:%d\n", priority_list[i].number, priority_list[i].cost );
    //}
    return priority_list;
}

int pre_total_cost(struct _task t)
{
    int i;
    int pre_cost = 0;
    for(i = 0; i < t.total_pre; i++)
    {
        pre_cost += task[t.pre[i]].cost;
    }
    return pre_cost;
}

void initialize_tasks(void)
{
    cp_info* priority_list = get_priority_list();
    int task_index;
    int processor_index = 0;
    for(task_index = 1; task_index < total_task && processor_index != total_pe; task_index++)
    {
        //struct _task : type
        struct _task t = task[priority_list[task_index].number];
        if(pre_total_cost(t) == 0)
        {
            pe[processor_index].task_no[0] = priority_list[task_index].number;
            pe[processor_index].task_cost[0] = t.cost;
            processor_index++;
        }
    }
}

int decide_processor(int* pe_current_cost)
{
    int min_cost = pe_current_cost[0];
    int min_cost_processor_no = 0;
    for (int processor_index = 1; processor_index < total_pe; processor_index++)
    {
        if(min_cost > pe_current_cost[processor_index])
        {
            min_cost = pe_current_cost[processor_index];
            min_cost_processor_no = processor_index;
        }
    }
    return min_cost_processor_no;
}

int* update_pe_current_cost(int decide_processor())
{
    int processor_index;
    int* pe_current_cost = malloc(total_pe * sizeof(int));
    //initialize pe_current_cost
    for(processor_index = 0; processor_index < total_pe; processor_index++)
    {
        pe_current_cost[processor_index] = pe[processor_index].task_cost[0];
    }
    pe_current_cost[decide_processor()] += task[decide_task()].cost;
    return pe_current_cost;
}

void initialize_task_is_done(void)
{
    int i;
    int* task_is_done = malloc(total_task * sizeof(int));
    for(i = 0; i < total_task; i++)
    {
        if(i == 0)
            task_is_done[i] = 1;
        else
            memset(task_is_done, 0, total_task-1 * sizeof(int));
    }
}

int decide_task(int* task_is_done)
{
    int i,j;
    int k = 0;
    int done_tasks = 0;
    int* next_allocated_task_no = malloc(total_task * sizeof(int));

    cp_info* priority_list = get_priority_list();
    for(i = 0; i < total_task; i++)
    {
        if(task_is_done[priority_list[i].number] == 0)
        {
            for(j = 0; j < task[priority_list[i].number].total_pre; j++)
            {
                done_tasks *= task_is_done[priority_list[i].number];
                if(done_tasks == 1)
                {
                    next_allocated_task_no[k] = priority_list[i].number;
                    k++;
                }  
            }
        }
    }
    task_is_done[next_allocated_task_no[0]] = 1;
    return next_allocated_task_no[0];
}

void allocate_tasks(void)
{


}

int main(int argc, char* argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/


    /*scheduling end*/

    output(argv);

    return 0;
}

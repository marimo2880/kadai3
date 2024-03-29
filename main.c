#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

#define TRUE 1
#define FALSE 0

struct cp_info
{
    int cost;
    int number;
};

typedef struct cp_info cp_info;

int pe_allocated_task_no;
int min_cost;

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
    //check
    //for (i = 0; i < 10; i++)
    //    printf("cp[%d] = %d\n",i,cp[i]);
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

    //check
    //for (i = 0; i < total_task; i++)
    //    printf("%d : %d\n",priority_list[i].number,priority_list[i].cost );
    return priority_list;
}

int pre_total_cost(struct _task t)
{
    int i;
    int pre_cost = 0;
    for (i = 0; i < t.total_pre; i++)
    {
        pre_cost += task[t.pre[i]].cost;
    }
    return pre_cost;
}

void initialize_tasks(const cp_info* priority_list, int* task_is_done)
{
    int task_index, i;
    int processor_index = 0;
    for (task_index = 1; task_index < total_task && processor_index < total_pe; task_index++)
    {
        //struct _task : type
        struct _task t = task[priority_list[task_index].number];
        //check
        printf("pre_total_cost_of_%d = %d\n", t.no, pre_total_cost(t));
        if (pre_total_cost(t) == 0)
        {
            //check
            printf("pocessor_index = %d\n", processor_index);
            pe[processor_index].task_no[0] = priority_list[task_index].number;
            pe[processor_index].task_cost[0] = t.cost;
            task_is_done[priority_list[task_index].number] = 1;
            //check
            printf("task_is_done[%d] = %d\n", priority_list[task_index].number, task_is_done[priority_list[task_index].number]);
            processor_index++;
            //check
            printf("pe[%d].task_no[0] = %d\n", processor_index - 1, pe[processor_index - 1].task_cost[0]);
        }
    }
    //check
    for (i = 0; i < total_task; i++)
        printf("task_is_done[%d] = %d\n", i, task_is_done[i]);
}

int* initialize_task_is_done(void)
{
    int* task_is_done = malloc(total_task * sizeof(int));
    memset(task_is_done, 0, total_task * sizeof(int));
    task_is_done[0] = 1;
    //check
    //int i;
    //for(i = 0; i < total_task; i++)
    //    printf("initialize_task_is_done = %d\n",task_is_done[i]);
    return task_is_done;
}

int* initialize_pe_current_cost(void)
{
    const cp_info* priority_list = get_priority_list();
    int* task_is_done = initialize_task_is_done();
    initialize_tasks(priority_list, task_is_done);
    int processor_index;
    int* pe_current_cost = malloc(total_pe * sizeof(int));
    for (processor_index = 0; processor_index < total_pe; processor_index++)
    {
        pe_current_cost[processor_index] = pe[processor_index].task_cost[0];
        //check
        printf("pe_current_cost[%d] = %d\n", processor_index, pe_current_cost[processor_index]);
    }
    return pe_current_cost;
}

int decide_processor(int* pe_current_cost, int* task_is_done)
{
    min_cost = pe_current_cost[0];
    int min_cost_processor_no = 0;
    int processor_index;
    for (processor_index = 1; processor_index < total_pe; processor_index++)
    {
        //check
        printf("processor_index = %d\n", processor_index);
        printf("min_cost = %d\n", min_cost);
        if (min_cost > pe_current_cost[processor_index])
        {
            min_cost = pe_current_cost[processor_index];
            min_cost_processor_no = processor_index;
        }
    }
    //previous allocated task
    task_is_done[pe[min_cost_processor_no].task_no[pe_allocated_task_no - 1]] = 1;
    //check
    printf("min_cost_processor_no = %d\n", min_cost_processor_no);
    return min_cost_processor_no;
}

void debug_decide_task(int i, const int* task_is_done, const cp_info* priority_list)
{
    int k;
    printf("--------------\n");

    //check
    for (k = 0; k < total_task; k++)
        printf("task_is_done = %d\n", task_is_done[k]);
    //check
    printf("priority_list[%d].number = %d\n", i, priority_list[i].number);
}

int check_dependencies(struct _task task_info, const int* task_is_done)
{
    int i;
    for (i = 0; i < task_info.total_pre; i++)
    {
        int dependency_index = task_info.pre[i];
        if(task_is_done[dependency_index] == FALSE)
            return FALSE;
    }
    return TRUE;
}

int decide_task(int* task_is_done, const cp_info* priority_list)
{
    int i;
    int next_allocated_task_no;

    //cp_info* priority_list = get_priority_list();

    for (i = 1; i < total_task; i++)
    {
        debug_decide_task(i, task_is_done, priority_list);

        int task_index = priority_list[i].number;
        if (task_is_done[task_index] == FALSE && check_dependencies(task[task_index], task_is_done))
        {
            next_allocated_task_no = priority_list[i].number;
            //check
            printf("next_allocated_task_no =%d\n", next_allocated_task_no);
            return next_allocated_task_no;
        }
    }
    //idle status
    return -1;
}

int get_total_cost(int* pe_current_cost)
{
    int i;
    for (i = 0; i < total_pe; i++)
    {
        if (total_cost < pe_current_cost[i])
            total_cost = pe_current_cost[i];
    }
    return total_cost;
}

void allocate_tasks(void)
{
    int* task_is_done = initialize_task_is_done();
    const cp_info* priority_list = get_priority_list();
    int* pe_current_cost = initialize_pe_current_cost();

    int i;
    int previous_min_cost = 0;
    int now_min_cost;
    int previous_pe_no = 0;
    int flag = 0;
    int all_task_is_done;
    pe_allocated_task_no = 1;

    while (1)
    {
        int processor_index = decide_processor(pe_current_cost, task_is_done);
        int task_index = decide_task(task_is_done, priority_list);
        //check
        printf("d_p = %d\n", processor_index);
        printf("d_t = %d\n", task_index);

        now_min_cost = min_cost;

        //idle status
        if (flag == 1)
        {
            pe[previous_pe_no].task_cost[pe_allocated_task_no - 1] = now_min_cost - previous_min_cost;
            pe_current_cost[previous_pe_no] += now_min_cost - previous_min_cost;
            flag = 0;
        }

        if (task_index == -1) //idle status
        {
            previous_min_cost = min_cost;
            pe[processor_index].task_no[pe_allocated_task_no] = task_index;
            previous_pe_no = processor_index;
            flag = 1;
        }
        else
        {
            pe[processor_index].task_no[pe_allocated_task_no] = task[task_index].no;
            pe[processor_index].task_cost[pe_allocated_task_no] = task[task_index].cost;
            pe_current_cost[processor_index] += task[task_index].cost;
        }

        //check
        if(task_index != -1)
        {
            printf("pe[%d].task_no[%d] = %d\n", processor_index, pe_allocated_task_no, task[task_index].no);
            printf("pe[%d].task_cost[%d] = %d\n", processor_index, pe_allocated_task_no, task[task_index].cost);
        }

        pe_allocated_task_no++;

        all_task_is_done = 1;
        for (i = 0; i < total_task; i++)
        {
            all_task_is_done *= task_is_done[i];
        }
        //check
        printf("pe_allocated_task_no = %d\n", pe_allocated_task_no);
        if (all_task_is_done == 1)
        {
            get_total_cost(pe_current_cost);
            printf("total_cost = %d\n", get_total_cost(pe_current_cost));
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/

    initialize_pe_current_cost();

    /*scheduling end*/

    output(argv);

    return 0;
}
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

int* pe_current_cost;
int* task_is_done;
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

void initialize_tasks(void)
{
    cp_info* priority_list = get_priority_list();
    int task_index;
    int processor_index = 0;
    for (task_index = 1; task_index < total_task && processor_index < total_pe; task_index++)
    {
        //struct _task : type
        struct _task t = task[priority_list[task_index].number];
        //check
        //printf("%d : %d\n",t.no,pre_total_cost(t));
        if (pre_total_cost(t) == 0)
        {
            pe[processor_index].task_no[0] = priority_list[task_index].number;
            pe[processor_index].task_cost[0] = t.cost;
            task_is_done[priority_list[task_index].number] = 1;
            processor_index++;
            //check
            //printf("pocessor_index = %d\n",processor_index-1);
            //printf("%d : %d\n",pe[processor_index-1].task_no[0],pe[processor_index-1].task_cost[0]);
        }
    }
}

void initialize_task_is_done(void)
{
    int i;
    task_is_done = malloc(total_task * sizeof(int));
    for (i = 0; i < total_task; i++)
    {
        if (i == 0)
            task_is_done[i] = 1;
        else
            task_is_done[i] = 0;
        //check
        //printf("%d : %d\n",i,task_is_done[i]);
    }
}

void initialize_pe_current_cost(void)
{
    initialize_tasks();
    int processor_index;
    pe_current_cost = malloc(total_pe * sizeof(int));
    for(processor_index = 0; processor_index < total_pe; processor_index++)
    {
        pe_current_cost[processor_index] = pe[processor_index].task_cost[0];
        //check
        //printf("%d : %d\n",processor_index,pe_current_cost[processor_index]);
    }
}

int decide_processor(int* pe_current_cost)
{
    min_cost = pe_current_cost[0];
    int min_cost_processor_no = 0;
    int processor_index;
    for (processor_index = 1; processor_index < total_pe; processor_index++)
    {
        //check
        printf("processor_index = %d\n",processor_index);
        printf("min_cost = %d\n",min_cost);
        if (min_cost > pe_current_cost[processor_index])
        {
            min_cost = pe_current_cost[processor_index];
            min_cost_processor_no = processor_index;
        }
    }
    //previous allocated task
    task_is_done[pe[min_cost_processor_no].task_no[pe_allocated_task_no - 1]] = 1;
    //check
    printf("min_cost_processor_no = %d\n",min_cost_processor_no);
    return min_cost_processor_no;
}

int decide_task(int* task_is_done)
{
    int i, j,k;
    int done_tasks;
    int next_allocated_task_no;

    cp_info* priority_list = get_priority_list();

    for (i = 1; i < total_task; i++)
    {
        printf("--------------\n");
        initialize_task_is_done();
        
        //check
        for(k = 0;k < total_task; k++)
            printf("task_is_done = %d\n",task_is_done[k]);
        //check
        printf("priority_list[%d].number = %d\n",i,priority_list[i].number);

        if (task_is_done[priority_list[i].number] == 0)
        {
            for (j = 0; j < task[priority_list[i].number].total_pre; j++)
            {
                done_tasks = 1;
                done_tasks *= task_is_done[task[priority_list[i].number].pre[j]];
                //check
                //printf("done_tasks = %d\n",done_tasks);
                if (done_tasks == 1)
                {
                    next_allocated_task_no = priority_list[i].number;
                    //check
                    printf("next_allocated_task_no =%d\n",next_allocated_task_no);
                    return next_allocated_task_no;
                }
            }
        }
    }
    //idle status
    return -1;
}

int get_total_cost(int* pe_current_cost)
{
    int i;
    int max_cost = pe_current_cost[0];
    for(i = 1; i < total_pe; i++)
    {
        if(max_cost < pe_current_cost[i])
            max_cost = pe_current_cost[i];
    }
    return max_cost;
}

void allocate_tasks(void)
{
    initialize_task_is_done();
    initialize_pe_current_cost();
    
    int i;
    int previous_min_cost = 0;
    int now_min_cost;
    int previous_pe_no = 0;
    int flag = 0;
    int all_task_is_done;
    pe_allocated_task_no = 1;

    while (1)
    {
        int d_p = decide_processor(pe_current_cost);
        int d_t = decide_task(task_is_done);
        //check
        printf("d_p = %d\n",d_p);
        printf("d_t = %d\n",d_t);

        now_min_cost = min_cost;

        //idle status
        if(flag == 1)
        {
            pe[previous_pe_no].task_cost[pe_allocated_task_no-1] = now_min_cost - previous_min_cost;
            pe_current_cost[previous_pe_no] += now_min_cost - previous_min_cost;
            flag = 0;
        }

        if(d_t == -1) //idle status
        {
            previous_min_cost = min_cost;
            pe[d_p].task_no[pe_allocated_task_no] = d_t;
            previous_pe_no = d_p;
            flag = 1;
        }
        else
        {
            pe[d_p].task_no[pe_allocated_task_no] = task[d_t].no;
            pe[d_p].task_cost[pe_allocated_task_no] = task[d_t].cost;
            pe_current_cost[d_p] += task[d_t].cost;
        }

        //check
        printf("pe[%d].task_no[%d] = %d\n",d_p,pe_allocated_task_no,task[d_t].no);
        printf("pe[%d].task_cost[%d] = %d\n",d_p,pe_allocated_task_no,task[d_t].cost);

        pe_allocated_task_no++;

        for(i = 0; i < total_task; i++)
        {
            all_task_is_done = 1;
            all_task_is_done *= task_is_done[i];
        }
        //check
        printf("pe_allocated_task_no = %d\n",pe_allocated_task_no);
        if(all_task_is_done == 1)
            get_total_cost(pe_current_cost);
            printf("total_cost = %d\n",get_total_cost(pe_current_cost));
            break;
    
}

int main(int argc, char* argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/

    allocate_tasks();

    /*scheduling end*/

    output(argv);

    return 0;
}
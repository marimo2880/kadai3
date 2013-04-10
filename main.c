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
    //for (i = 0; i < 10; i++) printf("cp[%d]=%d\n", i, cp[i]);
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

void get_priority_list(void)
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

    for (i = 0; i < total_task; i++)
    {
        printf("%d:%d\n", priority_list[i].number, priority_list[i].cost );
    }
}


int main(int argc, char* argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/

    get_priority_list();

    /*scheduling end*/

    output(argv);

    return 0;
}

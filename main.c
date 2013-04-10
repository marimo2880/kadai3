#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"


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

void get_priority_list(void)
{
    
}


int main(int argc, char *argv[])
{
    check_arg(argc, argv);
    input(argc, argv);

    /*scheduling start*/

    get_exit_task();
    get_critical_path();

    /*scheduling end*/

    output(argv);

    return 0;
}

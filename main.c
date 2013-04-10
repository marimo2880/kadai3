#include "support.h"

int 
main(int argc, char *argv[])
{
    check_arg(argc, argv);
    input(argc, argv);
    
/*scheduling start*/

int get_exit_task()
{
    int exit_task_no;
    exit_task_no = total_task - 1;
    return exit_task_no;
}
   
void get_cp()
{
    int i,j;
    int cp[total_task] = {0};
    for(i = get_exit_task(); i > 0; i--)
    {
	if(i = get_exit_task()) cp[exit_task_no] = task[exit_task_no].cost;
        for(j = 0; j < task[i].total_pre; j++)
	{
	    if(cp[task[i].pre[j]] < task[task[i].pre[j]].cost + cp[i])
            cp[task[i].pre[j]] = task[task[i].pre[j]].cost + cp[i];
	}
    }

    for (i=0;i<10;i++) printf("cp[%d]=%d\n",i,cp[i]);
}
/*scheduling end*/
    
    output(argv);
    
    return 0;
}

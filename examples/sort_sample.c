#include <stdio.h>
#include <stdlib.h>

int compare(const void* p_a, const void* p_b)
{
    const int* int_p_a = (int*)p_a; // const void* -> const int*
    const int* int_p_b = (int*)p_b; // const void* -> const int*
    int a = *int_p_a; // const int* -> int
    int b = *int_p_b; // const int* -> int

    if(a == b)
        return 0;
    else if(a < b)
        return -1;
    else
        return 1;
}

int main(void)
{
    int i;
    int array[] = {0, 3, 1, -1, 723, 71};
    int size_of_array = sizeof(array) / sizeof(int); // only with arrays

    qsort(array, size_of_array, sizeof(int), &compare);
    for(i = 0; i < size_of_array; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");


    return EXIT_SUCCESS;
}

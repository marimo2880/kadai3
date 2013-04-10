#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    int age;
    char* name;
    int gender;
};

typedef struct Person Person;

Person get_person_from_stdin(void)
{
    Person new_person;
    char name[200];
    printf("Enter your name\n");
    scanf("%s", name);
    new_person.name = malloc(strlen(name) + 1);
    strcpy(new_person.name, name);
    printf("Enter your age\n");
    int age;
    scanf("%d", &age);
    new_person.age = age;
    printf("Enter your gender (0: male, 1: female)\n");
    scanf("%d", &new_person.gender);
    return new_person;
}

int main(void)
{
    Person p = get_person_from_stdin();
    printf("%s\n", p.name);
    printf("%d\n", p.age);
    printf("%s\n", p.gender == 0 ? "male" : "female");

    free(p.name);

    return EXIT_SUCCESS;
}

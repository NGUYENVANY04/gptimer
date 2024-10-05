// #include <stdio.h>
// #include <stdbool.h>
// #include <malloc.h>

// typedef struct
// {
//     int a;
//     int b;
// } test;

// typedef struct
// {
//     test *p1;
//     test *p2;
// } hold;

// test *function()
// {
//     // Dynamically allocate memory for a 'test' struct
//     test *test1 = malloc(sizeof(test));
//     if (test1 == NULL) // Check if memory allocation was successful
//     {
//         return NULL;
//     }

//     printf("Size of test1 pointer: %ld\n", sizeof(test1)); // Print size of the pointer
//     printf("Size of struct test: %ld\n", sizeof(*test1));  // Print size of the struct

//     // Assign values to 'a' and 'b'
//     test1->a = 1;
//     test1->b = 2;

//     return test1;
// }
// void print(void *arg)
// {
//     hold *test1 = (hold *)arg;
//     printf("%p dmmm", test1->p1);
// }
// int main()
// {
//     // Initialize hold1 with pointers returned from 'function()'
//     hold *hold1 = {function(), function()};

//     // Check if the allocation succeeded
//     if (hold1->p1 == NULL || hold1->p2 == NULL)
//     {
//         return 1; // Exit if any allocation failed
//     }

//     // Print values from the 'test' structs
//     printf("hold1.p1->a: %d\n", hold1->p1->a);
//     printf("hold1.p2->a: %d\n", hold1->p2->b);
//     print(hold1);
//     // Free dynamically allocated memory
// }

#include <stdio.h>

int add_numbers(int a, int b) {
    return a + b;
}

int multiply_numbers(int a, int b) {
    return a * b;
}

void print_result(const char* operation, int result) {
    printf("%s 结果: %d\n", operation, result);
}
#include <stdio.h>
#include <string.h>

int string_length(const char* str) {
    return str ? strlen(str) : 0;
}

void string_print(const char* str) {
    printf("字符串: %s\n", str ? str : "(空)");
}
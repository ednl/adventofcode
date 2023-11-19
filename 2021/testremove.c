#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static const char a[] = "abcd";
static const char b[] = "dcba";

static bool contains(const char *a, const char *b)
{
    while (*b) {
        const char *c = a;
        while (*c && *c != *b)
            c++;
        if (!*c)
            return false;
        b++;
    }
    return true;
}

int main(void)
{
    printf("%p %p\n", (void*)a, (void*)b);
    printf("%s %s\n", a, b);
    printf("a contains b: %c\n", contains(a, b) ? 'Y' : 'N');
    printf("%p %p\n", (void*)a, (void*)b);
    printf("%s %s\n", a, b);
    return 0;
}

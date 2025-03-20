#include "test.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static int tests_count;
static int failures_count;

void print_divider()
{
    printf("=============================\n");
}

void print_header(char *suite_title)
{
    printf("%s\n", suite_title);
    print_divider();
    printf("\n");
}

void print_footer()
{
    printf("\nTotal tests ran: %d\n", tests_count);
    printf("Number of assertion failures: %d\n", failures_count);
}

void tst_suite_begin(char *title)
{
    tests_count = 0;
    failures_count = 0;
    
    print_header(title);
}

void tst_suite_finish()
{
    print_footer();
}

void tst_unit(char *title, void (*tst_func)())
{
    int failures;

    failures = failures_count;

    printf("tst: %s\n", title);

    tst_func();

    if (failures < failures_count) {
        printf("FAILED \n");
    } else {
        printf("PASSED \n");
    }

    print_divider();

    ++tests_count;
}

void tst_true(bool b)
{
    if (b == false) {
        printf("assertion failure: not true\n");
        ++failures_count;
    }
}

void tst_false(bool b)
{
    if (b == true) {
        printf("assertion failure: not false\n");
        ++failures_count;
    }
}

void tst_str_equals(char *a, char *b)
{
    if (strcmp(a, b) != 0) {
        printf("assertion failure: tst_str_equals(\"%s\", \"%s\")\n", a, b);
        ++failures_count;
    }
}

void tst_int_equals(int a, int b)
{
    if (a != b) {
        printf("assertion failure: tst_int_equals(%d, %d)\n", a, b);
        ++failures_count;
    }
}


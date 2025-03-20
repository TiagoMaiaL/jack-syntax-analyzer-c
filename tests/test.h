#include <stdbool.h>

// Small library for performing assertions and running 
// automated tests.

void tst_str_equals(char *a, char *b);
void tst_int_equals(int a, int b);
void tst_true(bool b);
void tst_false(bool b);

void tst_suite_begin(char *title);
void tst_suite_finish();
void tst_unit(char *message, void (*tst_func)());


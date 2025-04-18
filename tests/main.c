#include "test-tokenizer.h"
#include "test-parser.h"
#include "test-id-table.h"

int main(int argc, char **argv)
{
    test_tokenizer();
    test_parser();
    test_id_table();
}

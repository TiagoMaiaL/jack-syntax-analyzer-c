#include "code-gen.h"
#include "file-handler.h"
#include "linked-list.h"

#define FUNC_DEC_LEN 1000

static FILE *code_file;
static char *class_name;

static void gen_subroutine_code(Parser_subroutine_dec subroutine);
static void gen_statement_code(Parser_statement statement);
static void gen_expression_code(Parser_expression expr);

static void write(const char *str);

void cg_gen_code(FILE *file, Parser_jack_syntax *ast)
{
    code_file = file;
    class_name = ast->class_dec.name;
    
    Parser_class_dec class = ast->class_dec;

    if (class.subroutines.head == NULL) {
        return;
    }

    LL_Node *node = class.subroutines.head;
    while (node != NULL) {
        gen_subroutine_code(*(Parser_subroutine_dec *)node->data);
        node = node->next;
    }
}

static void gen_subroutine_code(Parser_subroutine_dec subroutine)
{
    char vm_func[FUNC_DEC_LEN];

    sprintf(
        vm_func, 
        "function %s.%s %d", 
        class_name,
        subroutine.name, 
        subroutine.vars.count
    );
    write(vm_func);

    LL_Node *node = subroutine.statements.head;
    while (node != NULL) {
        gen_statement_code(*(Parser_statement *)node->data);
        node = node->next;
    }
}

static void gen_statement_code(Parser_statement statement)
{
    if (statement.do_statement != NULL) {
        // for each param expression
        // generate code for it, pushing them to the stack
        // use the call vm command passing the number of args

    } else if (statement.let_statement != NULL) {
        // Exec expr + pop for specific index/segment
        // If there's subscript
            // Generate code for computing index
            // Select the that segment
        // generate code to eval expr
        // pop value to the right segment

    } else if (statement.if_statement != NULL) {
        // Exec cond expr + if-goto vm command
        // Generate code 

    } else if (statement.while_statement != NULL) {
        // Exec cond expr + jmp vm commands

    } else if (statement.return_statement != NULL) {
        // return vm command
    }
}

static void gen_expression_code(Parser_expression expr)
{

}

static void write(const char *str)
{
    fh_write(str, code_file);
}


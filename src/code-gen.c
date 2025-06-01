#include "code-gen.h"
#include "file-handler.h"
#include "linked-list.h"

#define STR_BUFF_SIZE 10000

static FILE *code_file;
static char *class_name;

static void gen_subroutine_code(Parser_subroutine_dec subroutine);

static void gen_statement_code(Parser_statement statement);
static void gen_do_code(Parser_do_statement do_statement);

static void gen_expression_code(Parser_expression *expr);
static void gen_term_code(Parser_term *term);

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
    char vm_func[STR_BUFF_SIZE];

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
        gen_do_code(*statement.do_statement);

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

static void gen_do_code(Parser_do_statement do_statement)
{
    Parser_term_subroutine_call call = do_statement.subroutine_call;

    LL_Node *node = call.param_expressions.head;
    while (node != NULL) {
        gen_expression_code((Parser_expression *)node->data);
        node = node->next;
    }

    char call_command[STR_BUFF_SIZE];
    sprintf(
        call_command,
        "call %s.%s %d",
        call.instance_var_name, // TODO: get class name from instance.
        call.subroutine_name,
        call.param_expressions.count
    );

    write(call_command);
}

static void gen_expression_code(Parser_expression *expr)
{
    LL_Node *term_node = expr->terms.head;
    LL_Node *op_node = NULL;

    // 1 + 1 + 1

    while (term_node != NULL) {
        // push term
        gen_term_code((Parser_term *)term_node->data);

        term_node = term_node->next;

        if (op_node == NULL) {
            op_node = expr->operators.head;
            continue;
        } else {
            op_node = op_node->next;
        }

        // apply vm function
        gen_operator_code(*(Parser_term_operator *)op_node->data);
    }
}

static void gen_term_code(Parser_term *term)
{
    if (term->integer != NULL) {
        char const_push[STR_BUFF_SIZE];
        sprintf(
            const_push, 
            "push constant %s", 
            term->integer
        );
        write(const_push);

    } else if (term->string != NULL) {
        // TODO:

    } else if (term->keyword_value != PARSER_TERM_KEYWORD_UNDEFINED) {
        // TODO:

    } else if (term->var_usage != NULL) {
        // TODO:

    } else if (term->subroutine_call != NULL) {
        // TODO:

    } else if (term->parenthesized_expression != NULL) {
        gen_expression_code(term->parenthesized_expression);

    } else if (term->sub_term != NULL) {
        gen_term_code(term->sub_term.term);
        gen_operator_code(term->unary_op);
    }
}

static void write(const char *str)
{
    fh_write(str, code_file);
    fh_write("\n", code_file);
}


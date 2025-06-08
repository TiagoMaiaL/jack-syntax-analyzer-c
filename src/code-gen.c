#include "code-gen.h"
#include "file-handler.h"
#include "id-table.h"
#include "linked-list.h"

#define STR_BUFF_SIZE 10000

static FILE *code_file;
static char *class_name;
static char *subroutine_name;
static int label_count;
static short indent_level;

static void gen_subroutine_code(Parser_subroutine_dec subroutine);

static void gen_statement_code(Parser_statement statement);
static void gen_do_code(Parser_do_statement do_statement);
static void gen_let_code(Parser_let_statement let_statement);
static void gen_if_code(Parser_if_statement if_statement);
static void gen_while_code(Parser_while_statement while_statement);
static void gen_return_code(Parser_return_statement return_statement);

static void gen_expression_code(Parser_expression *expr);
static void gen_term_code(Parser_term *term);
static void gen_keyword_code(Parser_term_keyword_constant keyword);
static void gen_var_usage_code(Parser_term_var_usage *var_usage);
static void gen_subroutine_call_code(Parser_term_subroutine_call call);
static void gen_operator_code(Parser_term_operator operator);
static void gen_unary_operator_code(Parser_term_operator operator);

static void unique_label(char *buff);

static void write(const char *str);

void cg_gen_code(FILE *file, Parser_jack_syntax *ast)
{
    code_file = file;
    class_name = ast->class_dec.name;
    label_count = 0;
    indent_level = 0;
    
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
    indent_level = 0;
    char vm_func[STR_BUFF_SIZE];

    short vars_count = 0;
    LL_Node *node = subroutine.vars.head;
    while (node != NULL) {
        Parser_var_dec *var = (Parser_var_dec *)node->data;
        if (var != NULL && var->names.count > 1) {
            vars_count += var->names.count;
        } else {
            vars_count++;
        }

        node = node->next;
    }

    sprintf(
        vm_func, 
        "function %s.%s %d", 
        class_name,
        subroutine.name, 
        vars_count
    );
    write(vm_func);

    subroutine_name = subroutine.name;

    node = subroutine.statements.head;
    while (node != NULL) {
        indent_level = 1;
        gen_statement_code(*(Parser_statement *)node->data);
        node = node->next;
    }
}

static void gen_statement_code(Parser_statement statement)
{
    if (statement.do_statement != NULL) {
        gen_do_code(*statement.do_statement);

    } else if (statement.let_statement != NULL) {
        gen_let_code(*statement.let_statement);

    } else if (statement.if_statement != NULL) {
        gen_if_code(*statement.if_statement);

    } else if (statement.while_statement != NULL) {
        gen_while_code(*statement.while_statement);

    } else if (statement.return_statement != NULL) {
        gen_return_code(*statement.return_statement);
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

static void gen_let_code(Parser_let_statement let_statement)
{
    gen_expression_code(&let_statement.value);

    IDT_Entry *entry = idt_entry(
        let_statement.var_name, 
        subroutine_name
    );
    if (entry != NULL) {
        char pop_command[STR_BUFF_SIZE];
        sprintf(
            pop_command,
            "pop %s %d",
            idt_category_name(entry->category),
            entry->index
        );
        write(pop_command);
    }
}

static void gen_if_code(Parser_if_statement if_statement)
{
    char else_label_buff[STR_BUFF_SIZE];
    char end_label_buff[STR_BUFF_SIZE];
    unique_label(else_label_buff);
    unique_label(end_label_buff);

    // VM code for computing expression
    gen_expression_code(&if_statement.conditional);
    gen_unary_operator_code(PARSER_TERM_OP_NOT);

    // if-goto else_statement (~cond) 
    char command_buff[STR_BUFF_SIZE];
    sprintf(command_buff, "if-goto %s", else_label_buff);
    write(command_buff);

    // inside if-branch
    LL_Node *statement_node = if_statement.conditional_statements.head;
    while (statement_node != NULL) {
        gen_statement_code(*(Parser_statement *)statement_node->data);
        statement_node = statement_node->next;
    }
    
    sprintf(command_buff, "goto %s", end_label_buff);
    write(command_buff);

    // inside else-branch
    sprintf(command_buff, "label %s", else_label_buff);
    write(command_buff);

    statement_node = if_statement.else_statements.head;
    while (statement_node != NULL) {
        gen_statement_code(*(Parser_statement *)statement_node->data);
        statement_node = statement_node->next;
    }

    // end if-else marker label
    sprintf(command_buff, "label %s", end_label_buff);
    write(command_buff);
}

static void gen_while_code(Parser_while_statement while_statement)
{
    char start_buff[STR_BUFF_SIZE];
    char end_buff[STR_BUFF_SIZE];
    unique_label(start_buff);
    unique_label(end_buff);

    char vm_command_buff[STR_BUFF_SIZE];

    // Mark beginning of while
    sprintf(
        vm_command_buff,
        "label %s",
        start_buff
    );
    write(vm_command_buff);

    // Gen expression for while ~cond
    gen_expression_code(&while_statement.conditional);
    gen_unary_operator_code(PARSER_TERM_OP_NOT);

    // Goto end if ~cond
    sprintf(
        vm_command_buff,
        "if-goto %s",
        end_buff
    );
    write(vm_command_buff);

    // Statements inside while
    LL_Node *statement_node = while_statement.statements.head;
    while (statement_node != NULL) {
        gen_statement_code(*(Parser_statement *)statement_node->data);
        statement_node = statement_node->next;
    }

    // Begin next iteration
    sprintf(
        vm_command_buff,
        "goto %s",
        start_buff
    );
    write(vm_command_buff);

    // Mark end of while
    sprintf(
        vm_command_buff,
        "label %s",
        end_buff
    );
    write(vm_command_buff);
}

static void gen_return_code(Parser_return_statement return_statement)
{
    if (return_statement.has_expr) {
        gen_expression_code(&return_statement.expression);
    }
    write("return");
}

static void gen_expression_code(Parser_expression *expr)
{
    LL_Node *term_node = expr->terms.head;
    LL_Node *op_node = NULL;

    while (term_node != NULL) {
        // push term
        gen_term_code((Parser_term *)term_node->data);

        term_node = term_node->next;

        if (op_node == NULL) {
            op_node = expr->operators.head;
            continue;
        }

        // apply vm function
        gen_operator_code(*(Parser_term_operator *)op_node->data);
        op_node = op_node->next;
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
        // TODO: determine how to handle strings.

    } else if (term->keyword_value != PARSER_TERM_KEYWORD_UNDEFINED) {
        gen_keyword_code(term->keyword_value);

    } else if (term->var_usage != NULL) {
        gen_var_usage_code(term->var_usage);

    } else if (term->subroutine_call != NULL) {
        gen_subroutine_call_code(*term->subroutine_call);

    } else if (term->parenthesized_expression != NULL) {
        gen_expression_code(term->parenthesized_expression);

    } else if (term->sub_term != NULL) {
        gen_term_code(&term->sub_term->term);
        gen_unary_operator_code(term->sub_term->unary_op);
    }
}

static void gen_keyword_code(Parser_term_keyword_constant keyword)
{
    if (keyword == PARSER_TERM_KEYWORD_TRUE) {
        write("push constant 1");
        write("neg");

    } else if (keyword == PARSER_TERM_KEYWORD_FALSE) {
        write("push constant 0");

    } else if (keyword == PARSER_TERM_KEYWORD_THIS) {

    } else if (keyword == PARSER_TERM_KEYWORD_NULL) {
        write("push constant 0");
    }
}

static void gen_var_usage_code(Parser_term_var_usage *var_usage)
{
    IDT_Entry *entry = idt_entry(
        var_usage->var_name, 
        subroutine_name
    );
    if (entry != NULL) {
        char push_command[STR_BUFF_SIZE];
        sprintf(
            push_command,
            "push %s %d",
            idt_category_name(entry->category),
            entry->index
        );
        write(push_command);
    }
}

static void gen_subroutine_call_code(Parser_term_subroutine_call call)
{
    LL_Node *expression_node = call.param_expressions.head;
    while (expression_node != NULL) {
        gen_expression_code((Parser_expression *)expression_node->data);
        expression_node = expression_node->next;
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

static void gen_operator_code(Parser_term_operator operator)
{
    if (operator == PARSER_TERM_OP_ADDITION) {
        write("add");

    } else if (operator == PARSER_TERM_OP_SUBTRACTION) {
        write("sub");

    } else if (operator == PARSER_TERM_OP_MULTIPLICATION) {
        write("call Math.multiply 2");

    } else if (operator == PARSER_TERM_OP_DIVISION) {
        write("call Math.divide 2");

    } else if (operator == PARSER_TERM_OP_GREATER) {
        write("gt");

    } else if (operator == PARSER_TERM_OP_LESSER) {
        write("lt");

    } else if (operator == PARSER_TERM_OP_ASSIGN) {
        write("eq");

    } else if (operator == PARSER_TERM_OP_AND) {
        write("and");

    } else if (operator == PARSER_TERM_OP_OR) {
        write("or");
    }
}

static void gen_unary_operator_code(Parser_term_operator operator)
{
    if (operator == PARSER_TERM_OP_SUBTRACTION) {
        write("neg");

    } else if (operator == PARSER_TERM_OP_NOT) {
        write("not");
    }
}

static void unique_label(char *buff)
{
    sprintf(
        buff,
        "%s_%d",
        class_name,
        label_count
    );
    label_count++;
}

static void write(const char *str)
{
    if (indent_level > 0) {
        for (short i = 0; i < indent_level; i++) {
            fh_write("  ", code_file);
        }
    }

    fh_write(str, code_file);
    fh_write("\n", code_file);
}


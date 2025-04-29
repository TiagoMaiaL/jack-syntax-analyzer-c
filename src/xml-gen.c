#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "file-handler.h"
#include "id-table.h"
#include "xml-gen.h"

static FILE *file;

void write_class(Parser_class_dec class);
void write_class_var(Parser_class_var_dec var_dec, short level);
void write_subroutine(Parser_subroutine_dec subroutine, short level);
void write_parameter_list(LL_List params, short level);
void write_subroutine_body(Parser_subroutine_dec subroutine, short level);
void write_vars(LL_List vars, short level);

void write_statements(LL_List statements, short level);
void write_do(Parser_do_statement do_stmt, short level);
void write_let(Parser_let_statement let_stmt, short level);
void write_if(Parser_if_statement if_stmt, short level);
void write_while(Parser_while_statement while_stmt, short level);
void write_return(Parser_return_statement return_stmt, short level);

void write_expression(Parser_expression expr, short level);
void write_subroutine_call(Parser_term_subroutine_call call, short level);
void write_term(Parser_term term, short level);
void write_operator(Parser_term_operator op, short level);

void write_keyword(char *desc, short level);
void write_identifier(char *id_type, char *desc, bool is_usage, IDT_Category category, int index, short level);
void write_type(char *desc, short level);
void write_symbol(char *desc, short level);
void write_tag(char *tag_desc, bool is_closing, short level);
void write_entry(char *tag_desc, char *val_desc, short level);
void write_indentation(short level);
void write_ln();

char *var_scope_keyword(Parser_class_var_scope scope);
char *subroutine_scope_keyword(Parser_subroutine_scope scope);
bool is_type_keyword(char *type);
char *term_keyword_value(Parser_term_keyword_constant keyword);
char *term_operator_value(Parser_term_operator op);

static char *class_name = NULL;
static char *subroutine_name = NULL;

void xml_gen(FILE *file_handle, Parser_jack_syntax file_syntax)
{
    file = file_handle;
    write_class(file_syntax.class_dec);
}

void write_class(Parser_class_dec class)
{
    short level = 1;
    class_name = class.name;

    write_tag("class", false, 0); write_ln();
    write_keyword("class", level);
    write_identifier("class", class.name, false, -1, -1, level);
    write_symbol("{", level);
    
    if (class.vars.count > 0) {
        LL_Node *class_var = class.vars.head;
        while (class_var != NULL) {
            write_class_var(*(Parser_class_var_dec *)class_var->data, level);
            class_var = class_var->next;
        }
    }

    if (class.subroutines.count > 0) {
        LL_Node *subroutine = class.subroutines.head;
        while(subroutine != NULL) {
            write_subroutine(*(Parser_subroutine_dec *)subroutine->data, level);
            subroutine = subroutine->next;
        }
    }

    write_symbol("}", level);
    write_tag("class", true, 0);
}

void write_class_var(Parser_class_var_dec var_dec, short level)
{
    assert(var_dec.names.count > 0);

    write_tag("classVarDec", false, level); write_ln();
    write_keyword(var_scope_keyword(var_dec.scope), level + 1);

    if (is_type_keyword(var_dec.type_name)) {
        write_keyword(var_dec.type_name, level + 1);
    } else {
        write_type(var_dec.type_name, level + 1);
    }
    
    LL_Node *name = var_dec.names.head;
    while (name != NULL) {
        IDT_Entry *entry = idt_entry((char *)name->data, class_name);
        write_identifier(
            var_dec.scope == PARSER_VAR_STATIC ? "static" : "field",
            (char *)name->data, 
            false,
            entry->category,
            entry->index,
            level + 1
        );

        if (name->next != NULL) {
            write_symbol(",", level + 1);
        }

        name = name->next;
    }

    write_symbol(";", level + 1);
    write_tag("classVarDec", true, level); write_ln();
}

void write_subroutine(Parser_subroutine_dec subroutine, short level)
{
    subroutine_name = subroutine.name;
    write_tag("subroutineDec", false, level); write_ln();

    write_keyword(subroutine_scope_keyword(subroutine.scope), level + 1);

    if (is_type_keyword(subroutine.type_name)) {
        write_keyword(subroutine.type_name, level + 1);
    } else {
        write_type(subroutine.type_name, level + 1);
    }

    write_identifier("subroutine", subroutine.name, false, -1, -1, level + 1);

    write_symbol("(", level + 1);
    write_parameter_list(subroutine.params, level + 1);
    write_symbol(")", level + 1);

    write_subroutine_body(subroutine, level + 1);

    write_tag("subroutineDec", true, level); write_ln();
}

void write_parameter_list(LL_List params, short level)
{
    write_tag("parameterList", false, level); write_ln();

    LL_Node *param = params.head;
    while (param != NULL) {
        Parser_param val = *(Parser_param *)param->data;

        if (is_type_keyword(val.type_name)) {
            write_keyword(val.type_name, level + 1);
        } else {
            write_type(val.type_name, level + 1);
        }

        IDT_Entry *entry = idt_entry(val.name, subroutine_name);
        write_identifier(
            "argument",
            val.name, 
            false,
            entry->category,
            entry->index,
            level + 1
        );
        
        if (param->next != NULL) {
            write_symbol(",", level + 1);
        }

        param = param->next;
    }

    write_tag("parameterList", true, level); write_ln();
}

void write_subroutine_body(Parser_subroutine_dec subroutine, short level)
{
    write_tag("subroutineBody", false, level); write_ln();
    write_symbol("{", level + 1);

    write_vars(subroutine.vars, level + 1);
    write_statements(subroutine.statements, level + 1);

    write_symbol("}", level + 1);
    write_tag("subroutineBody", true, level); write_ln();
}

void write_vars(LL_List vars, short level)
{
    if (vars.count == 0) { 
        return;
    }

    LL_Node *var = vars.head;
    while (var != NULL) {
        Parser_var_dec val = *(Parser_var_dec *)var->data;

        write_tag("varDec", false, level); write_ln();
        write_keyword("var", level + 1);

        if (is_type_keyword(val.type_name)) {
            write_keyword(val.type_name, level + 1);
        } else {
            write_type(val.type_name, level + 1);
        }

        LL_Node *name = val.names.head;
        while (name != NULL) {
            IDT_Entry *entry = idt_entry((char *)name->data, subroutine_name);
            write_identifier(
                "var",
                (char *)name->data, 
                false,
                entry->category,
                entry->index,
                level + 1
            );

            if (name->next != NULL) {
                write_symbol(",", level + 1);
            }

            name = name->next;
        }

        write_symbol(";", level + 1);

        write_tag("varDec", true, level); write_ln();

        var = var->next;
    }
}

void write_statements(LL_List statements, short level)
{
    if (statements.count == 0) { 
        return;
    }

    write_tag("statements", false, level); write_ln();

    LL_Node *statement_node = statements.head;
    while (statement_node != NULL) {
        Parser_statement statement = *(Parser_statement *)statement_node->data;

        if (statement.do_statement != NULL) {
            write_do(*statement.do_statement, level + 1);

        } else if (statement.let_statement != NULL) {
            write_let(*statement.let_statement, level + 1);

        } else if (statement.if_statement != NULL) {
            write_if(*statement.if_statement, level + 1);

        } else if (statement.while_statement != NULL) {
            write_while(*statement.while_statement, level + 1);

        } else if (statement.return_statement != NULL) {
            write_return(*statement.return_statement, level + 1);

        } else {
            assert(false);
        }

        statement_node = statement_node->next;
    }

    write_tag("statements", true, level); write_ln();
}

void write_do(Parser_do_statement do_stmt, short level)
{
    write_tag("doStatement", false, level); write_ln();
    write_keyword("do", level + 1);
    write_subroutine_call(do_stmt.subroutine_call, level + 1);
    write_symbol(";", level + 1);
    write_tag("doStatement", true, level); write_ln();
}

void write_let(Parser_let_statement let_stmt, short level)
{
    write_tag("letStatement", false, level); write_ln();
    write_keyword("let", level + 1);

    IDT_Entry *entry = idt_entry(let_stmt.var_name, subroutine_name);
    write_identifier(
        "var",
        let_stmt.var_name, 
        true,
        entry->category,
        entry->index,
        level + 1
    );

    if (let_stmt.has_subscript) {
        write_symbol("[", level + 1);
        write_expression(let_stmt.subscript, level + 1);
        write_symbol("]", level + 1);
    }
    write_symbol("=", level + 1);
    write_expression(let_stmt.value, level + 1);
    write_symbol(";", level + 1);
    write_tag("letStatement", true, level); write_ln();
}

void write_if(Parser_if_statement if_stmt, short level)
{
    write_tag("ifStatement", false, level); write_ln();

    write_keyword("if", level + 1);
    write_symbol("(", level + 1);
    write_expression(if_stmt.conditional, level + 1);
    write_symbol(")", level + 1);

    write_symbol("{", level + 1);
    write_statements(if_stmt.conditional_statements, level + 1);
    write_symbol("}", level + 1);

    if (if_stmt.has_else) {
        write_keyword("else", level + 1);
        write_symbol("{", level + 1);
        write_statements(if_stmt.else_statements, level + 1);
        write_symbol("}", level + 1);
    }

    write_tag("ifStatement", true, level); write_ln();
}

void write_while(Parser_while_statement while_stmt, short level)
{
    write_tag("whileStatement", false, level); write_ln();

    write_keyword("while", level + 1);
    write_symbol("(", level + 1);
    write_expression(while_stmt.conditional, level + 1);
    write_symbol(")", level + 1);

    write_symbol("{", level + 1);
    write_statements(while_stmt.statements, level + 1);
    write_symbol("}", level + 1);
   
    write_tag("whileStatement", true, level); write_ln();
}

void write_return(Parser_return_statement return_stmt, short level)
{
    write_tag("returnStatement", false, level); write_ln();
    write_keyword("return", level + 1);
    if (return_stmt.has_expr) {
        write_expression(return_stmt.expression, level + 1);
    }
    write_symbol(";", level + 1);
    write_tag("returnStatement", true, level); write_ln();
}

void write_expression(Parser_expression expr, short level)
{
    if (expr.terms.count == 0) {
        return;
    }

    write_tag("expression", false, level); write_ln();
    
    LL_Node *term = expr.terms.head;
    LL_Node *op = expr.operators.head;
    while (term != NULL) {
        write_term(*(Parser_term *)term->data, level + 1);

        if (op != NULL) {
            write_operator(*(Parser_term_operator *)op->data, level + 1);
            op = op->next;
        }

        term = term->next;
    }

    write_tag("expression", true, level); write_ln();
}

void write_term(Parser_term term, short level)
{
    write_tag("term", false, level); write_ln();

    if (term.integer != NULL) {
        write_entry("integerConstant", term.integer, level + 1);
    }

    if (term.string != NULL) {
        // remove "" from term.
        char *str = term.string + 1;
        char ch;

        while ((ch = *str) != '"') {
            str += 1;
        }
        *str = '\0';

        write_entry("stringConstant", term.string + 1, level + 1);
    }

    if (term.keyword_value != PARSER_TERM_KEYWORD_UNDEFINED) {
        write_keyword(term_keyword_value(term.keyword_value), level + 1);
    }

    if (term.var_usage != NULL) {
        IDT_Entry *entry = idt_entry(term.var_usage->var_name, subroutine_name);
        write_identifier(
            "var",
            term.var_usage->var_name,
            true,
            entry == NULL ? -1 : entry->category,
            entry == NULL ? -1 : entry->index,
            level + 1
        );

        if (term.var_usage->subscript != NULL) {
            write_symbol("[", level + 1);
            write_expression(*term.var_usage->subscript, level + 1);
            write_symbol("]", level + 1);
        }
    }

    if (term.subroutine_call != NULL) {
        write_subroutine_call(*term.subroutine_call, level + 1);
    }

    if (term.parenthesized_expression != NULL) {
        write_symbol("(", level + 1);
        write_expression(*term.parenthesized_expression, level + 1);
        write_symbol(")", level + 1);
    }

    if (term.sub_term != NULL) {
        write_operator(term.sub_term->unary_op, level + 1);
        write_term(term.sub_term->term, level + 1);
    }

    write_tag("term", true, level); write_ln();
}

void write_operator(Parser_term_operator op, short level)
{
    write_symbol(term_operator_value(op), level);
}

void write_subroutine_call(Parser_term_subroutine_call call, short level)
{
    if (call.instance_var_name != NULL) {
        write_entry("identifier", call.instance_var_name, level);
        write_symbol(".", level);
    }

    write_identifier(
        "subroutine",
        call.subroutine_name,
        true,
        -1,
        -1,
        level + 1
    );
    write_symbol("(", level);

    write_tag("expressionList", false, level); write_ln();

    LL_Node *param = call.param_expressions.head;
    while(param != NULL) {
        write_expression(*(Parser_expression *)param->data, level + 1);
        if (param->next != NULL) {
            write_symbol(",", level + 1);
        }
        param = param->next;
    }

    write_tag("expressionList", true, level); write_ln();

    write_symbol(")", level);
}

void write_keyword(char *desc, short level)
{
    write_entry("keyword", desc, level);
}

void write_identifier(char *id_type, char *desc, bool is_usage, IDT_Category category, int index, short level)
{
    char buff[200];
    char *category_desc = NULL;
    
    if (category == IDT_STATIC) {
        category_desc = "static";

    } else if (category == IDT_FIELD) {
        category_desc = "field";

    } else if (category == IDT_LOCAL) {
        category_desc = "local";

    } else if (category == IDT_PARAM) {
        category_desc = "argument";

    } else {
        category_desc = "";
    }

    sprintf(
        buff, 
        "type = %s, id = %s, is being used: %d, category = %s, index = %d",
        id_type,
        desc,
        is_usage,
        category_desc,
        index
    );

    write_entry("identifier", buff, level);
}

void write_type(char *desc, short level)
{
    write_entry("identifier", desc, level);
}

void write_symbol(char *desc, short level)
{
    write_entry("symbol", desc, level);
}

void write_entry(char *tag_desc, char *val_desc, short level)
{
    write_tag(tag_desc, false, level); fh_write(" ", file);
    fh_write(val_desc, file); 
    fh_write(" ", file); write_tag(tag_desc, true, 0);
    write_ln();
}

void write_tag(char *tag_desc, bool is_closing, short level)
{
    write_indentation(level);
    fh_write("<", file); 
    if (is_closing) {
        fh_write("/", file);
    }
    fh_write(tag_desc, file);
    fh_write(">", file);
}

void write_indentation(short level)
{
    assert(level >= 0);
    short whitespaces = level * 2;
    for (short i = 0; i < whitespaces; i++) {
        fh_write(" ", file);
    }
}

void write_ln() {
    fh_write("\n", file);
}

char *var_scope_keyword(Parser_class_var_scope scope)
{
    if (scope == PARSER_VAR_STATIC) {
        return "static";

    } else {
        return "field";
    }
}

char *subroutine_scope_keyword(Parser_subroutine_scope scope)
{
    if (scope == PARSER_FUNC_STATIC) {
        return "function";

    } else if (scope == PARSER_FUNC_CONSTRUCTOR) {
        return "constructor";

    } else {
        return "method";
    }
}

bool is_type_keyword(char *type)
{
    return strcmp(type, "int") == 0     ||
           strcmp(type, "char") == 0    ||
           strcmp(type, "boolean") == 0 ||
           strcmp(type, "void") == 0;
}

char *term_keyword_value(Parser_term_keyword_constant keyword)
{
    if (keyword == PARSER_TERM_KEYWORD_TRUE) {
        return "true";

    } else if (keyword == PARSER_TERM_KEYWORD_FALSE) {
        return "false";

    } else if (keyword == PARSER_TERM_KEYWORD_NULL) {
        return "null";

    } else {
        return "this";
    }
}

char *term_operator_value(Parser_term_operator op)
{
    if (op == PARSER_TERM_OP_ADDITION) {
        return "+";

    } else if (op == PARSER_TERM_OP_SUBTRACTION) {
        return "-";

    } else if (op == PARSER_TERM_OP_MULTIPLICATION) {
        return "*";

    } else if (op == PARSER_TERM_OP_DIVISION) {
        return "/";

    } else if (op == PARSER_TERM_OP_AND) {
        return "&amp;";

    } else if (op == PARSER_TERM_OP_OR) {
        return "|";

    } else if (op == PARSER_TERM_OP_LESSER) {
        return "&lt;";

    } else if (op == PARSER_TERM_OP_GREATER) {
        return "&gt;";

    } else if (op == PARSER_TERM_OP_ASSIGN) {
        return "=";

    } else {
        return "~";
    } 
}

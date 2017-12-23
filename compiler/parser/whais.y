/* WHAIS grammar file */

%pure-parser
%verbose

/* The buffer to be parsed */
%parse-param {struct ParserState *state}
%lex-param {struct ParserState *state}
%{
#define YYMAXDEPTH (256 * 1024)
#define YYSTYPE struct SemValue*

struct SemValue;
struct ParserState;

#include "../semantics/vardecl.h"
#include "../semantics/expression.h"
#include "../semantics/procdecl.h"
#include "../semantics/brlo_stmts.h"

int yylex(YYSTYPE *lvalp, struct ParserState *state);
void yyerror(struct ParserState *state,  const char *msg);

#define CHK_SEM_ERROR  { if (state->abortError != FALSE) YYABORT; }

%}

%token ARRAY
%token BOOL
%token BREAK
%token CHAR
%token CONTINUE
%token DATE
%token DATETIME
%token DO
%token ELSE
%token END
%token ENDPROC
%token ENDSYNC
%token EXTERN
%token FIELD
%token FOR
%token HIRESTIME
%token IF
%token INT8
%token INT16
%token INT32
%token INT64
%token PROCEDURE
%token REAL
%token RETURN
%token RICHREAL
%token SYNC
%token TABLE
%token TEXT
%token UINT8
%token UINT16
%token UINT32
%token UINT64
%token UNDEFINED
%token UNTIL
%token VAR
%token WHILE
%token WHAIS_NULL
%token W_FALSE
%token W_TRUE

%token IDENTIFIER

%token WHAIS_INTEGER
%token WHAIS_REAL
%token WHAIS_TEXT
%token WHAIS_CHAR
%token WHAIS_TIME

//Operator precedence and associativity

%right '=' SADD SSUB SMUL SDIV SMOD SAND SXOR SOR '?' ':' '$'
%left  EQ NE
%left  AND '&' OR '|' XOR '^'
%left  '<' '>' LE GE
%left  '+' '-'
%left  '*' '/' '%'
%right NOT '!' '~' 
%right '@'
// %right INC DEC

%right FSELECT 
%left  '[' ']'  '.'
%left  '(' ')'
%right UMINUS


/////////////
%%

program: global_block_statement
;

global_block_statement: /* empty */
                      | var_decl_stmt global_block_statement
                      | proc_decl_stmt global_block_statement
                      | extern_proc_decl_stmt global_block_statement
                      | EXTERN
                            { state->externDecl = TRUE; } 
                        var_decl_stmt
                            { state->externDecl = FALSE; }
                        global_block_statement
;

var_decl_stmt: VAR id_list type_spec ';'
                  { $$ = add_list_declaration (state, $2, $3); CHK_SEM_ERROR; }
             | VAR id_list RETURN ';'
                  { $$ = add_list_declaration (state, $2, NULL); CHK_SEM_ERROR; }
;

id_list: IDENTIFIER  
            { $$ = add_id_to_list(NULL, $1); }
       | id_list ',' IDENTIFIER
               { $$ = add_id_to_list($1, $3); }
;

type_spec: basic_type_spec
            { $$ = $1; }
         | array_type_spec
             { $$ = $1; }
         | field_type_spec
            { $$ = $1; } 
         | table_type_spec
             { $$ = $1; }
;

basic_type_spec: BOOL
                    { $$ = create_type_spec(state, T_BOOL); CHK_SEM_ERROR; }
               | CHAR
                    { $$ = create_type_spec(state, T_CHAR); CHK_SEM_ERROR; }
               | DATE
                       { $$ = create_type_spec(state, T_DATE); CHK_SEM_ERROR; }
               | DATETIME
                       { $$ = create_type_spec(state, T_DATETIME); CHK_SEM_ERROR;}
               | HIRESTIME
                       { $$ = create_type_spec(state, T_HIRESTIME);
                        CHK_SEM_ERROR;
                       }
               | INT8
                       { $$ = create_type_spec(state, T_INT8); CHK_SEM_ERROR; }
               | INT16
                       { $$ = create_type_spec(state, T_INT16); CHK_SEM_ERROR; }
               | INT32
                       { $$ = create_type_spec(state, T_INT32); CHK_SEM_ERROR; }
               | INT64
                       { $$ = create_type_spec(state, T_INT64);  CHK_SEM_ERROR; }
               | REAL
                       { $$ = create_type_spec(state, T_REAL); CHK_SEM_ERROR; }
               | RICHREAL
                       { $$ = create_type_spec(state, T_RICHREAL); CHK_SEM_ERROR;}
               | TEXT 
                       { $$ = create_type_spec(state, T_TEXT); CHK_SEM_ERROR; }
               | UINT8
                       { $$ = create_type_spec(state, T_UINT8);  CHK_SEM_ERROR; }
               | UINT16
                       { $$ = create_type_spec(state, T_UINT16); CHK_SEM_ERROR; }
               | UINT32
                       { $$ = create_type_spec(state, T_UINT32);  CHK_SEM_ERROR; }
               | UINT64
                       { $$ = create_type_spec(state, T_UINT64);  CHK_SEM_ERROR; }
               | UNDEFINED
                       { $$ = create_type_spec(state, T_UNDETERMINED);  CHK_SEM_ERROR; }
;

array_type_spec: array_of_clause ARRAY
                    {    $$ = $1; MARK_ARRAY (($$)->val.u_tspec.type); }
;

array_of_clause: /* empty */
                    { $$ = create_type_spec(state, T_UNDETERMINED); 
                      CHK_SEM_ERROR;
                    }
               | basic_type_spec
                    { $$ = $1; }
;

field_type_spec: field_of_clause FIELD
                    {   $$ = $1; MARK_FIELD (($$)->val.u_tspec.type); }
;

field_of_clause: /* empty */
                    { $$ = create_type_spec(state, T_UNDETERMINED); 
                      CHK_SEM_ERROR;
                    }
               | basic_type_spec
                    { $$ = $1; }
               | array_type_spec
                    { $$ = $1; }
;

table_type_spec: TABLE cont_clause
                 { MARK_TABLE ($2->val.u_tspec.type); $$ = $2; }
;

cont_clause: /* empty */
             {
                /* set the type spec later */
                $$ = create_type_spec(state, 0);
                CHK_SEM_ERROR;
                $$->val.u_tspec.extra = NULL;
             }
             | '(' container_type_decl ')'
                 {
                 /* set the type spec later */
                $$ = create_type_spec(state, 0);
                CHK_SEM_ERROR;
                $$->val.u_tspec.extra = $2;                     
                }
;

container_type_decl: IDENTIFIER basic_type_spec ',' container_type_decl
                        {
                            MARK_TABLE_FIELD ($2->val.u_tspec.type);
                            $$ = add_field_declaration(state,
                                $1, $2, (struct DeclaredVar *)$4);
                            CHK_SEM_ERROR;
                        } 
                   | IDENTIFIER array_type_spec ',' container_type_decl
                        {
                            MARK_TABLE_FIELD ($2->val.u_tspec.type);
                            $$ = add_field_declaration(state,
                                $1, $2, (struct DeclaredVar *)$4);
                            CHK_SEM_ERROR;
                        }
                   | IDENTIFIER basic_type_spec
                        {
                            MARK_TABLE_FIELD ($2->val.u_tspec.type);
                            $$ = add_field_declaration(state,
                                $1, $2, NULL);
                            CHK_SEM_ERROR;
                        }
                   | IDENTIFIER array_type_spec
                        {
                            MARK_TABLE_FIELD ($2->val.u_tspec.type);
                            $$ = add_field_declaration(state,
                                $1, $2, NULL);
                            CHK_SEM_ERROR;
                        }
;

proc_decl_stmt: PROCEDURE IDENTIFIER 
                    {
                        install_proc_decl(state, $2);
                        CHK_SEM_ERROR;
                    }
                '(' procedure_parameter_decl ')'
                RETURN type_spec
                    {
                        install_proc_args(state, $5);
                        CHK_SEM_ERROR;
                        set_proc_rettype(state, $8);
                        CHK_SEM_ERROR;
                    }
                DO local_block_statement
                ENDPROC
                    {
                        finish_proc_decl(state);
                        $$ = NULL;
                        CHK_SEM_ERROR;
                    }
;

extern_proc_decl_stmt: EXTERN PROCEDURE IDENTIFIER 
                            {
                                state->externDecl = TRUE;
                                install_proc_decl(state, $3);
                                state->externDecl = FALSE;
                                CHK_SEM_ERROR;
                            }
                       '(' procedure_parameter_decl ')'
                       RETURN type_spec ';'
                            {
                                install_proc_args(state, $6);
                                CHK_SEM_ERROR;
                                set_proc_rettype(state, $9);
                                CHK_SEM_ERROR;
                                state->externDecl = TRUE;
                                finish_proc_decl(state);
                                CHK_SEM_ERROR;
                                state->externDecl = FALSE;
                                $$ = NULL;
                            }
;

local_block_statement: /* empty */
                     | var_decl_stmt local_block_statement
                     | one_statement local_block_statement
                     | until_stmt local_block_statement
                     | syncronize_stmt local_block_statement
;

auto_decl_stmt: IDENTIFIER '='  exp ';'
                  { $$ = add_auto_declaration (state, $1, $3); CHK_SEM_ERROR; }
;

copy_table_rows_fields_stmt: exp '.' '{' id_list '}'  '[' exp ']' '='  exp '.'  '{' id_list  '}' '[' exp ']' ';'
                     | exp '.' '{' id_list  '}' '[' exp ']' '='  exp '.'  '*'  '[' exp ']' ';'
                     | exp '.' '*'  '[' exp ']' '='  exp '.'  '*'  '[' exp ']' ';'
;

one_statement: return_stmt
             | auto_decl_stmt
             | copy_table_rows_fields_stmt
             | exp_stmt 
             | if_stmt 
             | while_stmt 
             | for_stmt 
             | break_stmt 
             | continue_stmt 
;

return_stmt: RETURN exp ';'
                {
                   check_for_dead_statement (state);
                   $$ = translate_return_exp(state, $2); 
                   CHK_SEM_ERROR;
                }
;

procedure_parameter_decl: /* empty */
                            { $$ = NULL; }
                        | list_of_paramaters_decl
                            { $$ = $1; }
;

list_of_paramaters_decl: IDENTIFIER type_spec
                           {
                                $$ =  add_proc_param_decl(NULL, $1, $2);
                                CHK_SEM_ERROR;
                           }
                       | IDENTIFIER  type_spec ',' list_of_paramaters_decl
                           {
                                $$ = add_proc_param_decl($4, $1, $2);
                                CHK_SEM_ERROR;
                           }
;

exp_stmt : exp ';'
        {
           check_for_dead_statement (state);
           $$ = translate_exp(state, $1, TRUE); 
           CHK_SEM_ERROR;
        }
;

exp : const_exp
        {
            $$ = $1;
        }
    | IDENTIFIER %prec '='
        {
            $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
            CHK_SEM_ERROR;
        }
     | '@' IDENTIFIER
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_OFFSET);
            CHK_SEM_ERROR;
        }
     | '!' exp
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_NOT);
            CHK_SEM_ERROR;
        }
     | NOT exp
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_NOT);
            CHK_SEM_ERROR;
        }
     | '~' exp
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_NOT);
            CHK_SEM_ERROR;
        }
    | '-' exp %prec UMINUS
    | exp '+' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_ADD);
            CHK_SEM_ERROR;
        }
    | exp '-' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SUB);
            CHK_SEM_ERROR;
        }
    | exp '*' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_MUL);
            CHK_SEM_ERROR;
        }
    | exp '/' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_DIV);
            CHK_SEM_ERROR;
        }
    | exp '%' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_MOD);
            CHK_SEM_ERROR;
        }
    | exp '<' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_LT);
            CHK_SEM_ERROR;
        }
    | exp LE  exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_LE);
            CHK_SEM_ERROR;
        }
    | exp '>' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_GT);
            CHK_SEM_ERROR;
        }
    | exp GE  exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_GE);
            CHK_SEM_ERROR;
        }
    | exp EQ  exp 
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_EQ);
            CHK_SEM_ERROR;
        }
    | exp NE  exp 
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_NE);
            CHK_SEM_ERROR;
        }
    | exp AND exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_AND);
            CHK_SEM_ERROR;
        }
    | exp '&' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_AND);
            CHK_SEM_ERROR;
        }
    | exp OR  exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_OR);
            CHK_SEM_ERROR;
        }
    | exp '|'  exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_OR);
            CHK_SEM_ERROR;
        }
    | exp XOR exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_XOR);
            CHK_SEM_ERROR;
        }
    | exp '^' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_XOR);
            CHK_SEM_ERROR;
        }
    | '(' exp ')'
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_GROUP);
            CHK_SEM_ERROR;
        }
    | exp '[' exp ']'
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_INDEX);
            CHK_SEM_ERROR;
        }
    | exp '.' IDENTIFIER %prec FSELECT
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_FIELD);
            CHK_SEM_ERROR;
        }
     | exp '.'  IDENTIFIER '[' exp ']'
        {
            $$ = create_exp_link(state, $1, $5, $3, OP_TABVAL); 
            CHK_SEM_ERROR;
        }
    | exp '=' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_ATTR);
            CHK_SEM_ERROR;
        }
    | exp SADD exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SADD);
            CHK_SEM_ERROR;
        }
    | exp SSUB exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SSUB);
            CHK_SEM_ERROR;
        }
    | exp SMUL exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SMUL);
            CHK_SEM_ERROR;
        }
    | exp SDIV exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SDIV);
            CHK_SEM_ERROR;
        }
    | exp SMOD exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SMOD);
            CHK_SEM_ERROR;
        }
    | exp SAND exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SAND);
            CHK_SEM_ERROR;
        }
    | exp SXOR exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SXOR);
            CHK_SEM_ERROR;
        }
    | exp SOR exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_SOR);
            CHK_SEM_ERROR;
        }
    | exp '?' exp ':' exp  
       {
            $$ = create_exp_link(state, $1, $3, $5, OP_EXP_SEL);
            CHK_SEM_ERROR;
       }
    | exp '$' exp ':' exp
       {
        $$ = create_exp_link(state, $1, $3, NULL, OP_EXP_ALT);
        CHK_SEM_ERROR;
        }
    | IDENTIFIER '(' parameters_list ')'
        {
            /* procedure call */
            $$ = create_exp_link(state, $1, $3, NULL, OP_CALL);
            CHK_SEM_ERROR;
        }
;

const_exp: WHAIS_INTEGER
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHAIS_REAL
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHAIS_TEXT
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHAIS_CHAR
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHAIS_TIME
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHAIS_NULL
            {
                /* null operation and NULL operands means
                   the NULL value */
                $$ = create_exp_link(state, NULL, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | W_TRUE
            {
                $1 = alloc_bool_sem_value(state, TRUE);
                CHK_SEM_ERROR;
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR; 
            }
         | W_FALSE
            {
                $1 = alloc_bool_sem_value(state, FALSE);
                CHK_SEM_ERROR;
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR; 
            }
;

parameters_list: /* empty */
                    {
                        $$ = NULL;
                    }
               | not_empty_paramter_list
                    {
                        $$ = $1;
                    }
;

not_empty_paramter_list: exp
                            {
                                $$ = create_arg_link(state, $1, NULL);
                                CHK_SEM_ERROR;
                            }
                       | exp ',' parameters_list
                            {
                                $$ = create_arg_link(state, $1, $3);
                                CHK_SEM_ERROR;
                            }
;

if_stmt : IF '(' exp ')' DO 
              {
                check_for_dead_statement (state);
                begin_if_stmt(state, $3, BT_IF);
                CHK_SEM_ERROR;
              }
          local_block_statement end_of_if_statement 

        | IF '(' exp ')' 
              {
                check_for_dead_statement (state);
                begin_if_stmt(state, $3, BT_IF);
                CHK_SEM_ERROR;
              }
          one_statement possible_no_else_if_clause
;

possible_no_else_if_clause: /* empty */
                               {
                                   finalize_if_stmt(state);
                                   CHK_SEM_ERROR;
                               }
                             | else_if_clause
;

end_of_if_statement: END
                      {
                           finalize_if_stmt (state);
                           CHK_SEM_ERROR;
                      }
                   | else_if_clause
;

else_if_clause: ELSE
                  {
                        begin_else_stmt(state);
                        CHK_SEM_ERROR;
                  }
                DO local_block_statement END
                  {
                       finalize_if_stmt(state);
                       CHK_SEM_ERROR;
                  }
              | ELSE
                  {
                        begin_else_stmt(state);
                        CHK_SEM_ERROR;
                  }
                 one_statement
                  {
                       finalize_if_stmt(state);
                       CHK_SEM_ERROR;
                  }   
; 

while_stmt : WHILE '(' exp ')'
               {
                    check_for_dead_statement (state);
                    begin_while_stmt(state, $3);
                    CHK_SEM_ERROR;
               }
             DO local_block_statement END
               {
                    finalize_while_stmt(state);
                    CHK_SEM_ERROR;
               }
           | WHILE '(' exp ')'
               {
                    check_for_dead_statement (state);
                    begin_while_stmt(state, $3);
                    CHK_SEM_ERROR;
               }
             one_statement
               {
                    finalize_while_stmt(state);
                    CHK_SEM_ERROR;
               }
;

until_stmt: DO 
              {
                    check_for_dead_statement (state);
                    begin_until_stmt(state);
                    CHK_SEM_ERROR;
              }
           local_block_statement UNTIL exp ';'
              {
                    finalize_until_stmt(state, $5);
                    CHK_SEM_ERROR;
              }
;

for_stmt: FOR '(' IDENTIFIER ':' exp ')'
           {
             begin_foreach_stmt (state, $3, $5, FALSE);
           }
          one_statement
           {
             finalize_for_stmt (state);
             CHK_SEM_ERROR;
           }
        | FOR '(' IDENTIFIER ':' exp ')'
           {
             begin_foreach_stmt (state, $3, $5, FALSE);
           } 
           DO local_block_statement END
           {
             finalize_for_stmt(state);
             CHK_SEM_ERROR;
           }
        | FOR '(' '!'  IDENTIFIER ':' exp ')'
           {
             begin_foreach_stmt (state, $4, $6, TRUE);
           }
          one_statement
           {
             finalize_for_stmt (state);
             CHK_SEM_ERROR;
           }
        | FOR '(' '!' IDENTIFIER ':' exp ')'
           {
             begin_foreach_stmt (state, $4, $6, TRUE);
           } 
           DO local_block_statement END
           {
             finalize_for_stmt(state);
             CHK_SEM_ERROR;
           }
        | FOR '(' exp ';' exp ';' exp ')' 
          {
            check_for_dead_statement (state);
            begin_for_stmt (state, $3, $5, $7);
            CHK_SEM_ERROR;
          }
          one_statement
          {
            finalize_for_stmt (state);
            CHK_SEM_ERROR;
          }
        | FOR '(' exp ';' exp ';' exp ')'
          {
            check_for_dead_statement (state);
            begin_for_stmt (state, $3, $5, $7);
            CHK_SEM_ERROR;
          } 
          DO local_block_statement END
          {
            finalize_for_stmt(state);
            CHK_SEM_ERROR;
          }
; 

break_stmt: BREAK ';'
              {
                    handle_break_stmt(state);
                    CHK_SEM_ERROR;
              }
;

continue_stmt: CONTINUE ';'
                 {
                        handle_continue_stmt(state);
                        CHK_SEM_ERROR;
                 }
;

syncronize_stmt: SYNC
                   {
                       begin_sync_stmt(state);
                       CHK_SEM_ERROR;
                   }
               local_block_statement ENDSYNC
                   {
                       finalize_sync_stmt(state);
                       CHK_SEM_ERROR;
                   }
;

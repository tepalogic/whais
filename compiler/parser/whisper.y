/* WHISPER grammar file */

%pure-parser
%verbose

/* The buffer to be parsed */
%parse-param {struct ParserState *state}
%lex-param {struct ParserState *state}
%{
#define YYSTYPE struct SemValue*

struct SemValue;
struct ParserState;

#include "../semantics/vardecl.h"
#include "../semantics/expression.h"
#include "../semantics/procdecl.h"
#include "../semantics/brlo_stmts.h"

int yylex(YYSTYPE *lvalp, struct ParserState *state);
void yyerror(struct ParserState *state,  const char *msg);

#define CHK_SEM_ERROR  { if (state->err_sem != FALSE) YYABORT; }
%}

%token ARRAY
%token AS
%token BOOL
%token BREAK
%token CHARACTER
%token CONTINUE
%token DATE
%token DATETIME
%token DO
%token ELSE
%token ELSEIF
%token END
%token ENDPROC
%token ENDSYNC
%token EXTERN
%token FOREACH
%token HIRESTIME
%token IF
%token IN
%token INT8
%token INT16
%token INT32
%token INT64
%token LET
%token OF
%token PROCEDURE
%token REAL
%token RETURN
%token RICHREAL
%token SYNC
%token TABLE
%token TEXT
%token THEN
%token UNTIL
%token UNSIGNED
%token WHILE
%token WHISPER_NULL
%token W_FALSE
%token W_TRUE

%token IDENTIFIER

%token WHISPER_INTEGER
%token WHISPER_REAL
%token WHISPER_TEXT
%token WHISPER_CHARACTER
%token WHISPER_TIME

//Operator precedence and associativity


%right '='
%left  EQ NE
%left  AND OR XOR
%left  '<' '>' LE GE
%left  '+' '-'
%left  '*' '/' '%'
%right NOT
%right INC DEC

%left  '[' ']'
%left  '(' ')'


/////////////
%%

program: global_block_statement
;

global_block_statement: /* empty */
                      | return_stmt
                      | var_decl_stmt global_block_statement
                      | proc_decl_stmt global_block_statement
                      | extern_proc_decl_stmt global_block_statement
                      | EXTERN
                            { state->extern_decl = TRUE; } 
                        var_decl_stmt
                            { state->extern_decl = FALSE; }
                        global_block_statement
;

return_stmt: RETURN exp ';'
                {
                   $$ = translate_return_exp(state, $2); 
                   CHK_SEM_ERROR;
                }
;

var_decl_stmt: LET id_list AS type_spec ';'
		          { $$ = install_list_declrs(state, $2, $4); CHK_SEM_ERROR; }

id_list: IDENTIFIER  
			{ $$ = add_idlist(NULL, $1); }
       | id_list ',' IDENTIFIER
       		{ $$ = add_idlist($1, $3); }
;

type_spec: basic_type_spec
			{ $$ = $1; }
         | array_type_spec
         	{ $$ = $1; }
         | table_type_spec
         	{ $$ = $1; }
;

basic_type_spec: BOOL
					{ $$ = create_type_spec(state, T_BOOL); CHK_SEM_ERROR; }
               | CHARACTER
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
               | UNSIGNED INT8
               		{ $$ = create_type_spec(state, T_UINT8);  CHK_SEM_ERROR; }
               | UNSIGNED INT16
               		{ $$ = create_type_spec(state, T_UINT16); CHK_SEM_ERROR; }
               | UNSIGNED INT32
               		{ $$ = create_type_spec(state, T_UINT32);  CHK_SEM_ERROR; }
               | UNSIGNED INT64
               		{ $$ = create_type_spec(state, T_UINT64);  CHK_SEM_ERROR; }
;

array_type_spec: ARRAY array_of_clause
            		{	$$ = $2; ($$)->val.u_tspec.type |= T_ARRAY_MASK; }
;

array_of_clause: /* empty */
					{ $$ = create_type_spec(state, T_UNDETERMINED); 
					  CHK_SEM_ERROR;
					}
               | OF basic_type_spec
               		{ $$ = $2; }
;


table_type_spec: TABLE cont_clause
				 { $2->val.u_tspec.type = T_TABLE_MASK; $$ = $2 };
;

cont_clause: /* empty */
			 {
			    /* set the type spec later */
				$$ = create_type_spec(state, 0);
				CHK_SEM_ERROR;
				$$->val.u_tspec.extra = NULL;
			 }
             | OF '(' container_type_decl ')'
       	 	 {
		 		/* set the type spec later */
				$$ = create_type_spec(state, 0);
				CHK_SEM_ERROR;
				$$->val.u_tspec.extra = $3;         			
       		 }
;

container_type_decl: IDENTIFIER AS basic_type_spec ',' container_type_decl
                    	{
                    		$3->val.u_tspec.type |= T_FIELD_MASK;
                    		$$ = install_field_declaration(state,
                    			$1, $3, (struct DeclaredVar *)$5);
                            CHK_SEM_ERROR;
                    	} 
                   | IDENTIFIER AS array_type_spec ',' container_type_decl
                    	{
                    		$3->val.u_tspec.type |= T_FIELD_MASK;
                    		$$ = install_field_declaration(state,
                    			$1, $3, (struct DeclaredVar *)$5);
                            CHK_SEM_ERROR;
                    	}                
                   | IDENTIFIER AS basic_type_spec
                    	{
                    		$3->val.u_tspec.type |= T_FIELD_MASK;
                    		$$ = install_field_declaration(state,
                    			$1, $3, NULL);
                            CHK_SEM_ERROR;
                    	}                
                   | IDENTIFIER AS array_type_spec
                    	{
                    		$3->val.u_tspec.type |= T_FIELD_MASK;
                    		$$ = install_field_declaration(state,
                    			$1, $3, NULL);
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
                    }    
;

extern_proc_decl_stmt: EXTERN PROCEDURE IDENTIFIER 
                            {
                                state->extern_decl = TRUE;
                                install_proc_decl(state, $3);
                                state->extern_decl = FALSE;
                                CHK_SEM_ERROR;
                            }
                       '(' procedure_parameter_decl ')'
                       RETURN type_spec ';'
                            {
                                install_proc_args(state, $6);
                                CHK_SEM_ERROR;
                                set_proc_rettype(state, $9);
                                CHK_SEM_ERROR;
                                finish_proc_decl(state);
                                $$ = NULL;
                            }
;
local_block_statement: /* empty */
                     | return_stmt local_block_statement
                     | var_decl_stmt local_block_statement
                     | exp_stmt local_block_statement
                     | if_stmt local_block_statement
                     | while_stmt local_block_statement
                     | until_stmt local_block_statement
                     | foreach_stmt local_block_statement
                     | syncronize_stmt local_block_statement
                     | break_stmt local_block_statement
                     | continue_stmt local_block_statement
;


procedure_parameter_decl: /* empty */
                            { $$ = NULL; }
                        | list_of_paramaters_decl
                            { $$ = $1 }
;

list_of_paramaters_decl: IDENTIFIER AS type_spec
                           {
                                $$ =  add_prcdcl_list(NULL, $1, $3);
                           }
                       | IDENTIFIER AS type_spec ',' list_of_paramaters_decl
                           {
                                $$ = add_prcdcl_list($5, $1, $3);
                           }
;

exp_stmt : exp ';'
        {
           $$ = translate_exp(state, $1); 
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
    | INC exp 
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_INC);
            CHK_SEM_ERROR;
        }
    | DEC exp 
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_DEC);
            CHK_SEM_ERROR;
        }
    | NOT exp
        {
            $$ = create_exp_link(state, $2, NULL, NULL, OP_NOT);
            CHK_SEM_ERROR;
        }
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
    | exp OR  exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_OR);
            CHK_SEM_ERROR;
        }
    | exp XOR exp
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
    | exp '[' exp ',' IDENTIFIER ']'
        {
            $$ = create_exp_link(state, $1, $3, $5, OP_FIELD); 
            CHK_SEM_ERROR;
        }
    | exp '=' exp
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_ATTR);
            CHK_SEM_ERROR;
        }
    | exp '=' '{' parameters_list '}'
        {
            $$ = create_exp_link(state, $1, $3, NULL, OP_C_ATTR);
            CHK_SEM_ERROR;
        }
    | IDENTIFIER '(' parameters_list ')'
        {
            /* procedure call */
            $$ = create_exp_link(state, $1, $3, NULL, OP_CALL);
            CHK_SEM_ERROR;
        }
;

const_exp: WHISPER_INTEGER
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHISPER_REAL
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHISPER_TEXT
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHISPER_CHARACTER
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHISPER_TIME
            {
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | WHISPER_NULL
            {
                /* null operation and NULL operands means
                   the NULL value */
                $$ = create_exp_link(state, NULL, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR;
            }
         | W_TRUE
            {
                $1 = get_bool_sem_value(state, TRUE);
                CHK_SEM_ERROR;
                $$ = create_exp_link(state, $1, NULL, NULL, OP_NULL);
                CHK_SEM_ERROR; 
            }
         | W_FALSE
            {
                $1 = get_bool_sem_value(state, FALSE);
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

if_stmt : IF exp THEN 
              {
                begin_if_stmt(state, $2, BT_IF);
                CHK_SEM_ERROR;
              }
          local_block_statement else_if_clause 
;

else_if_clause: END
                  {
                        finalize_if_stmt(state);
                  }           
              | ELSEIF exp 
                   {
                        begin_elseif_stmt(state, $2);
                        CHK_SEM_ERROR;
                   }           
              THEN  local_block_statement else_if_clause 
              | ELSE
                  {
                        begin_else_stmt(state);
                        CHK_SEM_ERROR;
                  }
              local_block_statement END
                  {
                       finalize_if_stmt(state);
                  }   
; 

while_stmt : WHILE exp
               {
                    begin_while_stmt(state, $2);
                    CHK_SEM_ERROR;
               }
           DO local_block_statement END
               {
                    finalize_while_stmt(state);
                    CHK_SEM_ERROR;
               }
;

until_stmt: DO 
              {
                    begin_until_stmt(state);
                    CHK_SEM_ERROR;
              }
          local_block_statement UNTIL exp ';'
              {
                    finalize_until_stmt(state, $5);
                    CHK_SEM_ERROR;
              }
;

foreach_stmt: FOREACH IDENTIFIER IN '{' parameters_list '}'  DO local_block_statement END
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

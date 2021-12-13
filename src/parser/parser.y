%require "3.2"

%define api.pure
%locations
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {struct AST_TranslationUnit **parser_result}

%{
	#include "common.h"
	#include "ast.hpp"

	#ifdef __cplusplus
	extern "C" {
	#endif

	int yylex(void*, void*, yyscan_t);
	void yyerror(void*, yyscan_t, AST_TranslationUnit**, const char *str);

	#ifdef __cplusplus
	}
	#endif
%}

%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN SIZEOF

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%precedence IFX
%precedence ELSE

%start translation_unit

%union {
    int boolean;
	int enum_val;
	struct AST_Literal num_literal;
	string_id_t str;
	string_id_t ident_id;

	struct AST_Expr *expr;

	struct AST_ArgumentsList *args_list;
	struct AST_DeclSpecifiers *decl_specifiers;
	struct AST_TypeSpecifier *type_specifier;
	struct AST_StructDeclarationList *struct_decl_lst;
	struct AST_StructDeclaration *struct_decl;
	struct AST_SpecifierQualifierList *spec_qual_lst;
	struct AST_StructDeclarator *struct_delarator;
	struct AST_StructDeclaratorList *struct_delarator_lst;
	struct AST_StructOrUsionSpec *struct_or_union_spec;
	struct AST_Enumerator *enumerator;
	struct AST_EnumeratorList *enumerator_lst;
	struct AST_EnumSpecifier *enum_spec;

	struct AST_InitDeclarator *init_declarator;
	struct AST_InitDeclaratorList *init_declarator_lst;
	struct AST_Declaration *declaration;
	struct AST_DirectDeclarator *dir_declarator;
	struct AST_Pointer *pointer;
	struct AST_Declarator *declarator;
	struct AST_ParameterDeclaration *param_declaration;
	struct AST_ParameterTypeList *param_type_lst;
	struct AST_ParameterList *param_lst;
	struct AST_TypeName *type_name;
	struct AST_DirectAbstractDeclarator *dir_abstract_decl;
	struct AST_AbstractDeclarator *abstract_decl;
	struct AST_TypeQualifiers *type_quals;

	struct AST_Designator *designator;
	struct AST_InitializerList *initializer_lst;
	struct AST_Initializer *initializer;

	struct AST_Statement *stmt;
	struct AST_LabeledStmt *label_stmt;
	struct AST_BlockItemList *block_item_lst;
	struct AST_CompoundStmt *compound_stmt;
	struct AST_ExprStmt *expr_stmt;
	struct AST_SelectionStmt *select_stmt;
	struct AST_IterationStmt *iter_stmt;
	struct AST_JumpStmt *jump_stmt;

	struct AST_FunctionDef *func_def;
	struct AST_TranslationUnit *trans_unit;
}

%token <ident_id> IDENTIFIER
%token <ident_id> TYPE_NAME
%token <num_literal> CONSTANT
%token <str> STRING_LITERAL

%type <expr> primary_expr
%type <expr> postfix_expr
%type <args_list> arg_expr_lst
%type <expr> unary_expr
%type <enum_val> unary_operator
%type <expr> cast_expr
%type <expr> mult_expr
%type <expr> add_expr
%type <expr> shift_expr
%type <expr> rel_expr
%type <expr> eq_expr
%type <expr> and_expr
%type <expr> xor_expr
%type <expr> or_expr
%type <expr> log_and_expr
%type <expr> log_or_expr
%type <expr> ternary_expr
%type <expr> assign_expr
%type <enum_val> assign_op
%type <expr> expr
%type <expr> const_expr

%type <decl_specifiers> decl_specs
%type <enum_val> storage_class_specifier
%type <type_specifier> type_specifier
%type <enum_val> type_qualifier
%type <enum_val> function_specifier
%type <struct_or_union_spec> unistruct_spec
%type <boolean> unistruct
%type <struct_decl_lst> struct_declaration_list
%type <struct_decl> struct_declaration
%type <spec_qual_lst> spec_qual_lst
%type <struct_delarator_lst> struct_declarator_list
%type <struct_delarator> struct_declarator
%type <enumerator> enumerator
%type <enumerator_lst> enumerator_list
%type <enum_spec> enum_specifier

%type <declaration> declaration
%type <init_declarator_lst> init_declarator_list
%type <init_declarator> init_declarator
%type <declarator> declarator
%type <pointer> pointer
%type <dir_declarator> dir_declarator
%type <type_quals> type_qual_lst
%type <param_type_lst> param_type_lst
%type <param_lst> param_lst
%type <param_declaration> parameter_declaration
%type <type_name> type_name
%type <abstract_decl> abstr_declarator;
%type <dir_abstract_decl> dir_abstr_declarator;

%type <initializer> initializer
%type <initializer_lst> init_lst
%type <designator> designator

%type <stmt> stmt
%type <label_stmt> label_stmt
%type <block_item_lst> block_item_list
%type <compound_stmt> compound_stmt
%type <expr_stmt> expr_stmt
%type <select_stmt> select_stmt
%type <iter_stmt> loop_stmt
%type <jump_stmt> jmp_stmt

%type <func_def> func_def
%type <trans_unit> translation_unit

%%

	/* Expressions */

primary_expr
	: IDENTIFIER							{ $$ = AST_Primary::get_ident($1); }
	| CONSTANT								{ $$ = AST_Primary::get_const($1); }
	| STRING_LITERAL						{ $$ = AST_Primary::get_str($1); }
	| '(' expr ')'							{ $$ = AST_Primary::get_expr($2); }
	;

postfix_expr
	: primary_expr							{ $$ = $1; }
	| postfix_expr '[' expr ']'				{ $$ = AST_Postfix::get_arr($1, $3); }
	| postfix_expr '(' ')'					{ $$ = AST_Postfix::get_call($1, nullptr); }
	| postfix_expr '(' arg_expr_lst ')'		{ $$ = AST_Postfix::get_call($1, $3); }
	| postfix_expr '.' IDENTIFIER			{ $$ = AST_Postfix::get_accesor($1, $3, false); }
	| postfix_expr PTR_OP IDENTIFIER		{ $$ = AST_Postfix::get_accesor($1, $3, true); }
	| postfix_expr INC_OP					{ $$ = AST_Postfix::get_incdec($1, false); }
	| postfix_expr DEC_OP					{ $$ = AST_Postfix::get_incdec($1, true); }
	;

arg_expr_lst
	: assign_expr							{ $$ = (new AST_ArgumentsList())->append($1); }
	| arg_expr_lst ',' assign_expr			{ $$ = $1->append($3); };
	;

unary_expr
	: postfix_expr							{ $$ = $1; }
	| INC_OP unary_expr						{ $$ = new AST_Unop(AST_Unop::PRE_INC, $2); }
	| DEC_OP unary_expr						{ $$ = new AST_Unop(AST_Unop::PRE_DEC, $2); }
	| unary_operator cast_expr				{ $$ = new AST_Unop(AST_Unop::OpType($1), $2); }
	| SIZEOF unary_expr						{ $$ = new AST_Unop(AST_Unop::SIZEOF_OP, $2); }
	| SIZEOF '(' type_name ')'				{ $$ = new AST_Unop(AST_Unop::SIZEOF_OP, $3); }
	;

unary_operator
	: '&'									{ $$ = AST_Unop::ADDR_OF; }
	| '*'									{ $$ = AST_Unop::DEREF; }
	| '+'									{ $$ = AST_Unop::UN_PLUS; }
	| '-'									{ $$ = AST_Unop::UN_MINUS; }
	| '~'									{ $$ = AST_Unop::UN_NEG; }
	| '!'									{ $$ = AST_Unop::UN_NOT; }
	;

cast_expr
	: unary_expr							{ $$ = $1; }
	| '(' type_name ')' cast_expr			{ $$ = new AST_Cast($2, $4); }
	;

mult_expr
	: cast_expr								{ $$ = $1; }
	| mult_expr '*' cast_expr				{ $$ = new AST_Binop(AST_Binop::MUL, $1, $3); }
	| mult_expr '/' cast_expr				{ $$ = new AST_Binop(AST_Binop::DIV, $1, $3); }
	| mult_expr '%' cast_expr				{ $$ = new AST_Binop(AST_Binop::REM, $1, $3); }
	;

add_expr
	: mult_expr								{ $$ = $1; }
	| add_expr '+' mult_expr				{ $$ = new AST_Binop(AST_Binop::ADD, $1, $3); }
	| add_expr '-' mult_expr				{ $$ = new AST_Binop(AST_Binop::SUB, $1, $3); }
	;

shift_expr
	: add_expr								{ $$ = $1; }
	| shift_expr LEFT_OP add_expr			{ $$ = new AST_Binop(AST_Binop::SHL, $1, $3); }
	| shift_expr RIGHT_OP add_expr			{ $$ = new AST_Binop(AST_Binop::SHR, $1, $3); }
	;

rel_expr
	: shift_expr							{ $$ = $1; }
	| rel_expr '<' shift_expr				{ $$ = new AST_Binop(AST_Binop::LT, $1, $3); }
	| rel_expr '>' shift_expr				{ $$ = new AST_Binop(AST_Binop::GT, $1, $3); }
	| rel_expr LE_OP shift_expr				{ $$ = new AST_Binop(AST_Binop::LE, $1, $3); }
	| rel_expr GE_OP shift_expr				{ $$ = new AST_Binop(AST_Binop::GE, $1, $3); }
	;

eq_expr
	: rel_expr								{ $$ = $1; }
	| eq_expr EQ_OP rel_expr				{ $$ = new AST_Binop(AST_Binop::EQ, $1, $3); }
	| eq_expr NE_OP rel_expr				{ $$ = new AST_Binop(AST_Binop::NE, $1, $3); }
	;

and_expr
	: eq_expr								{ $$ = $1; }
	| and_expr '&' eq_expr					{ $$ = new AST_Binop(AST_Binop::BIT_AND, $1, $3); }
	;

xor_expr
	: and_expr								{ $$ = $1; }
	| xor_expr '^' and_expr					{ $$ = new AST_Binop(AST_Binop::BIT_XOR, $1, $3); }
	;

or_expr
	: xor_expr								{ $$ = $1; }
	| or_expr '|' xor_expr					{ $$ = new AST_Binop(AST_Binop::BIT_OR, $1, $3); }
	;

log_and_expr
	: or_expr								{ $$ = $1; }
	| log_and_expr AND_OP or_expr			{ $$ = new AST_Binop(AST_Binop::LOG_AND, $1, $3); }
	;

log_or_expr
	: log_and_expr							{ $$ = $1; }
	| log_or_expr OR_OP log_and_expr		{ $$ = new AST_Binop(AST_Binop::LOG_OR, $1, $3); }
	;

ternary_expr
	: log_or_expr								{ $$ = $1; }
	| log_or_expr '?' expr ':' ternary_expr		{ $$ = new AST_Ternary($1, $3, $5); }
	;

assign_expr
	: ternary_expr								{ $$ = $1; }
	| unary_expr assign_op assign_expr			{ $$ = new AST_Assignment(AST_Assignment::OpType($2), $1, $3); }
	;

assign_op
	: '='										{ $$ = AST_Assignment::DIRECT; }
	| MUL_ASSIGN								{ $$ = AST_Assignment::MUL; }
	| DIV_ASSIGN								{ $$ = AST_Assignment::DIV; }
	| MOD_ASSIGN								{ $$ = AST_Assignment::REM; }
	| ADD_ASSIGN								{ $$ = AST_Assignment::ADD; }
	| SUB_ASSIGN								{ $$ = AST_Assignment::SUB; }
	| LEFT_ASSIGN								{ $$ = AST_Assignment::SHL; }
	| RIGHT_ASSIGN								{ $$ = AST_Assignment::SHR; }
	| AND_ASSIGN								{ $$ = AST_Assignment::AND; }
	| XOR_ASSIGN								{ $$ = AST_Assignment::XOR; }
	| OR_ASSIGN									{ $$ = AST_Assignment::OR; }
	;

expr
	: assign_expr								{ $$ = $1; }
	| expr ',' assign_expr						{ $$ = dynamic_cast<AST_CommaExpression*>($1)->append($3); }
	;

const_expr
	: ternary_expr								{ $$ = $1; }
	;

	/* Specifiers */

decl_specs
	: storage_class_specifier				{ $$ = (new AST_DeclSpecifiers())->update_storage($1); }
	| storage_class_specifier decl_specs	{ $$ = $2->update_storage($1); }
	| type_specifier						{ $$ = (new AST_DeclSpecifiers())->update_type_spec($1); }
	| type_specifier decl_specs				{ $$ = $2->update_type_spec($1); }
	| type_qualifier						{ $$ = (new AST_DeclSpecifiers())->update_type_qual($1); }
	| type_qualifier decl_specs				{ $$ = $2->update_type_qual($1); }
	| function_specifier					{ $$ = (new AST_DeclSpecifiers())->update_func_qual($1); }
	| function_specifier decl_specs			{ $$ = $2->update_func_qual($1); }
	;

storage_class_specifier
	: TYPEDEF								{ $$ = AST_DeclSpecifiers::ST_TYPEDEF; }
	| EXTERN								{ $$ = AST_DeclSpecifiers::ST_EXTERN; }
	| STATIC								{ $$ = AST_DeclSpecifiers::ST_STATIC; }
	| AUTO									{ $$ = AST_DeclSpecifiers::ST_AUTO; }
	| REGISTER								{ $$ = AST_DeclSpecifiers::ST_REGISTER; }
	;

type_specifier
	: VOID									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_VOID); }
	| CHAR									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_CHAR); }
	| SHORT									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_SHORT); }
	| INT									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_INT); }
	| LONG									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_LONG); }
	| FLOAT									{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_FLOAT); }
	| DOUBLE								{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_DOUBLE); }
	| SIGNED								{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_SIGNED); }
	| UNSIGNED								{ $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_UNSIGNED); }
	| unistruct_spec						{ $$ = new AST_TypeSpecifier($1); }
	| enum_specifier						{ $$ = new AST_TypeSpecifier($1); }
	| TYPE_NAME								{ $$ = new AST_TypeSpecifier(get_def_type($1)); }
	;

type_qualifier
	: CONST									{ $$ = AST_TypeQualifiers::Q_CONST; }
	| RESTRICT								{ $$ = AST_TypeQualifiers::Q_RESTRICT; }
	| VOLATILE								{ $$ = AST_TypeQualifiers::Q_VOLATILE; }
	;

function_specifier
	: INLINE								{ $$ = AST_DeclSpecifiers::Q_INLINE; }
	;

unistruct_spec
	: unistruct IDENTIFIER '{' struct_declaration_list '}'	{ $$ = new AST_StructOrUsionSpec($1, $2, $4); }
	| unistruct '{' struct_declaration_list '}'				{ $$ = new AST_StructOrUsionSpec($1, NO_IDENT_ID, $3); }
	| unistruct IDENTIFIER									{ $$ = new AST_StructOrUsionSpec($1, $2, nullptr); }
	;

unistruct
	: STRUCT											{ $$ = 0; }
	| UNION												{ $$ = 1; }
	;

struct_declaration_list
	: struct_declaration								{ $$ = new AST_StructDeclarationList($1); }
	| struct_declaration_list struct_declaration		{ $$ = $1->append($2); }
	;

struct_declaration
	: spec_qual_lst struct_declarator_list ';'			{ $$ = new AST_StructDeclaration($1, $2); }
	;

spec_qual_lst
	: type_specifier spec_qual_lst				{ $$ = $2->append_spec($1); }
	| type_specifier							{ $$ = (new AST_SpecifierQualifierList($1)); }
	| type_qualifier spec_qual_lst				{ $$ = $2->append_qual(AST_TypeQualifiers::QualType($1)); }
	| type_qualifier							{ $$ = new AST_SpecifierQualifierList(AST_TypeQualifiers::QualType($1)); }
	;

struct_declarator_list
	: struct_declarator									{ $$ = new AST_StructDeclaratorList($1); }
	| struct_declarator_list ',' struct_declarator		{ $$ = $1->append($3); }
	;

struct_declarator
	: declarator										{ $$ = new AST_StructDeclarator($1, nullptr); }
	| ':' const_expr									{ $$ = new AST_StructDeclarator(nullptr, $2); }
	| declarator ':' const_expr							{ $$ = new AST_StructDeclarator($1, $3); }
	;

enum_specifier
	: ENUM '{' enumerator_list '}'						{ $$ = new AST_EnumSpecifier(NO_IDENT_ID, $3); }
	| ENUM IDENTIFIER '{' enumerator_list '}'			{ $$ = new AST_EnumSpecifier($2, $4); }
	| ENUM '{' enumerator_list ',' '}'					{ $$ = new AST_EnumSpecifier(NO_IDENT_ID, $3); }
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'		{ $$ = new AST_EnumSpecifier($2, $4); }
	| ENUM IDENTIFIER									{ $$ = new AST_EnumSpecifier($2, nullptr); }
	;

enumerator_list
	: enumerator										{ $$ = new AST_EnumeratorList($1); }
	| enumerator_list ',' enumerator					{ $$ = $1->append($3); }
	;

enumerator
	: IDENTIFIER										{ $$ = new AST_Enumerator($1, nullptr); }
	| IDENTIFIER '=' const_expr							{ $$ = new AST_Enumerator($1, $3); }
	;


	/* Declarations */

declaration
	: decl_specs ';'									{ $$ = new AST_Declaration($1, nullptr); }
	| decl_specs init_declarator_list ';'				{ $$ = new AST_Declaration($1, $2); }
	;

init_declarator_list
	: init_declarator									{ $$ = new AST_InitDeclaratorList($1); }
	| init_declarator_list ',' init_declarator			{ $$ = $1->append($3); }
	;

init_declarator
	: declarator										{ $$ = new AST_InitDeclarator($1, nullptr); }
	| declarator '=' initializer						{ $$ = new AST_InitDeclarator($1, $3); }
	;

declarator
	: pointer dir_declarator							{ $$ = new AST_Declarator($2, $1); }
	| dir_declarator									{ $$ = new AST_Declarator($1, nullptr); }
	;

pointer
	: '*'							{ $$ = new AST_Pointer(nullptr, nullptr); }
	| '*' type_qual_lst				{ $$ = new AST_Pointer($2, nullptr); }
	| '*' pointer					{ $$ = new AST_Pointer(nullptr, $2); }
	| '*' type_qual_lst pointer		{ $$ = new AST_Pointer($2, $3); }
	;

dir_declarator
	: IDENTIFIER											{ $$ = AST_DirectDeclarator::get_ident($1); }
	| '(' declarator ')'									{ $$ = AST_DirectDeclarator::get_nested($2); }
	| dir_declarator '[' type_qual_lst assign_expr ']'		{ $$ = AST_DirectDeclarator::get_arr($1, $3, $4); }
	| dir_declarator '[' type_qual_lst ']'					{ $$ = AST_DirectDeclarator::get_arr($1, $3, nullptr); }
	| dir_declarator '[' assign_expr ']'					{ $$ = AST_DirectDeclarator::get_arr($1, nullptr, $3); }
	| dir_declarator '[' ']'								{ $$ = AST_DirectDeclarator::get_arr($1, nullptr, nullptr); }
	| dir_declarator '(' param_type_lst ')'					{ $$ = AST_DirectDeclarator::get_func($1, $3); }
	| dir_declarator '(' ')'								{ $$ = AST_DirectDeclarator::get_func($1, nullptr); }
	;

type_qual_lst
	: type_qualifier								{ $$ = new AST_TypeQualifiers(AST_TypeQualifiers::QualType($1)); }
	| type_qual_lst type_qualifier					{ $$ = $1->update(AST_TypeQualifiers::QualType($2)); }
	;

param_type_lst
	: param_lst										{ $$ = new AST_ParameterTypeList($1, false); }
	| param_lst ',' ELLIPSIS						{ $$ = new AST_ParameterTypeList($1, true); }
	;

param_lst
	: parameter_declaration							{ $$ = new AST_ParameterList($1); }
	| param_lst ',' parameter_declaration			{ $$ = $1->append($3); }
	;

parameter_declaration
	: decl_specs declarator							{ $$ = new AST_ParameterDeclaration($1, $2); }
	| decl_specs									{ $$ = new AST_ParameterDeclaration($1, nullptr); }
	;

type_name
	: spec_qual_lst									{ $$ = new AST_TypeName($1, nullptr); }
	| spec_qual_lst abstr_declarator				{ $$ = new AST_TypeName($1, $2); }
	;

abstr_declarator
	: dir_abstr_declarator							{  $$ = new AST_AbstractDeclarator($1, nullptr); }
	| pointer dir_abstr_declarator					{  $$ = new AST_AbstractDeclarator($2, $1); }
	;
	/* 	pointer   {  $$ = new AST_AbstractDeclarator(nullptr, $1); } */

dir_abstr_declarator
	: '(' abstr_declarator ')'						{ $$ = AST_DirectAbstractDeclarator::get_nested($2); }
	| dir_abstr_declarator '[' ']'					{ $$ = AST_DirectAbstractDeclarator::get_arr($1, nullptr); }
	| dir_abstr_declarator '[' assign_expr ']'		{ $$ = AST_DirectAbstractDeclarator::get_arr($1, $3); }
	| dir_abstr_declarator '(' ')'					{ $$ = AST_DirectAbstractDeclarator::get_func($1, nullptr); }
	| dir_abstr_declarator '(' param_type_lst ')'	{ $$ = AST_DirectAbstractDeclarator::get_func($1, $3); }
	;


	/* Initializers */

initializer
	: assign_expr									{ $$ = new AST_Initializer($1); }
	| '{' init_lst '}'								{ $$ = new AST_Initializer($2); }
	| '{' init_lst ',' '}'							{ $$ = new AST_Initializer($2); }
	;

init_lst
	: initializer									{ $$ = new AST_InitializerList($1, nullptr); }
	| designator '=' initializer					{ $$ = new AST_InitializerList($3, $1); }
	| init_lst ',' initializer						{ $$ = $1->append($3, nullptr); }
	| init_lst ',' designator '=' initializer		{ $$ = $1->append($5, $3); }
	;

designator
	: '[' const_expr ']'							{ $$ = new AST_Designator($2); }
	| '.' IDENTIFIER								{ $$ = new AST_Designator($2); }
	;


	/* Statements */

stmt
	: label_stmt							{ $$ = $1; }
	| compound_stmt							{ $$ = $1; }
	| expr_stmt								{ $$ = $1; }
	| select_stmt							{ $$ = $1; }
	| loop_stmt								{ $$ = $1; }
	| jmp_stmt								{ $$ = $1; }
	;

label_stmt
	: IDENTIFIER ':' stmt					{ $$ = new AST_LabeledStmt($1, $3, AST_LabeledStmt::SIMPL); }
	| CASE const_expr ':' stmt				{ $$ = new AST_LabeledStmt($2, $4, AST_LabeledStmt::SW_CASE); }
	| DEFAULT ':' stmt						{ $$ = new AST_LabeledStmt(nullptr, $3, AST_LabeledStmt::SW_DEFAULT); }
	;

compound_stmt
	: '{' '}'								{ $$ = new AST_CompoundStmt(new AST_BlockItemList()); }
	| '{' block_item_list '}'				{ $$ = new AST_CompoundStmt($2); }
	;

block_item_list
	: declaration							{ $$ = (new AST_BlockItemList())->append($1); }
	| stmt									{ $$ = (new AST_BlockItemList())->append($1); }
	| block_item_list declaration			{ $$ = $1->append($2); }
	| block_item_list stmt					{ $$ = $1->append($2); }
	;

expr_stmt
	: ';'									{ $$ = new AST_ExprStmt(nullptr); }
	| expr ';'								{ $$ = new AST_ExprStmt($1); }
	;

select_stmt
	: IF '(' expr ')' stmt %prec IFX		{ $$ = AST_SelectionStmt::get_if($3, $5, nullptr); }
	| IF '(' expr ')' stmt ELSE stmt		{ $$ = AST_SelectionStmt::get_if($3, $5, $7); }
	| SWITCH '(' expr ')' stmt				{ $$ = AST_SelectionStmt::get_switch($3, $5); }
	;

loop_stmt
	: WHILE '(' expr ')' stmt							{ $$ = AST_IterationStmt::get_while($5, $3, false); }
	| DO stmt WHILE '(' expr ')' ';'					{ $$ = AST_IterationStmt::get_while($2, $5, true); }
	| FOR '(' expr_stmt expr_stmt ')' stmt				{ $$ = AST_IterationStmt::get_for($6, $3, $4, nullptr); }
	| FOR '(' expr_stmt expr_stmt expr ')' stmt			{ $$ = AST_IterationStmt::get_for($7, $3, $4, $5); }
	| FOR '(' declaration expr_stmt ')' stmt			{ $$ = AST_IterationStmt::get_for($6, $3, $4, nullptr); }
	| FOR '(' declaration expr_stmt expr ')' stmt		{ $$ = AST_IterationStmt::get_for($7, $3, $4, $5); }
	;

jmp_stmt
	: GOTO IDENTIFIER ';'								{ $$ = new AST_JumpStmt(AST_JumpStmt::J_GOTO, $2); }
	| CONTINUE ';'										{ $$ = new AST_JumpStmt(AST_JumpStmt::J_CONTINUE); }
	| BREAK ';'											{ $$ = new AST_JumpStmt(AST_JumpStmt::J_BREAK); }
	| RETURN ';'										{ $$ = new AST_JumpStmt(AST_JumpStmt::J_RET); }
	| RETURN expr ';'									{ $$ = new AST_JumpStmt(AST_JumpStmt::J_RET, $2); }
	;


	/* Top-level elements */

translation_unit
	: func_def							{ $$ = *parser_result = (new AST_TranslationUnit())->append($1); }
	| declaration						{ $$ = *parser_result = (new AST_TranslationUnit())->append($1); check_typedef($1); }
	| translation_unit func_def			{ $$ = $1->append($2); }
	| translation_unit declaration		{ $$ = $1->append($2); check_typedef($2); }
	;

func_def
	: decl_specs declarator compound_stmt				{ $$ = new AST_FunctionDef($1, $2, $3); }
	;


%%

void yyerror(void *loc, yyscan_t, AST_TranslationUnit **root, const char *str) {
    // fprintf(stderr, "error: %s\n",str);
    // printf("%s\n", (*root)->getTreeNode()->printHor().c_str());

    (void)root;
    YYLTYPE *mloc = reinterpret_cast<YYLTYPE*>(loc);
    fprintf(stderr, "error (%d:%d): %s\n", mloc->first_line, mloc->first_column, str);
}

AST_TranslationUnit* parse_program(std::string const &str) {
	yyscan_t scanner;
	lex_extra_t extra;
	init_scanner(str.c_str(), &scanner, &extra);
	AST_TranslationUnit *res;
	yyparse(scanner, &res);
	destroy_scanner(scanner);
	return res;
}

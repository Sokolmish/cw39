%require "3.2"

%define api.pure
%locations
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {struct AST_TranslationUnit **parser_result}
%parse-param {const struct LinesWarpMap *warps}

%code requires {
    struct LinesWarpMap;
}

%{
    #include "lexer.h"
    #include "parser.hpp"
    #include "warps.hpp"

    #ifdef __cplusplus
    extern "C" {
    #endif

    int yylex(void*, void*, yyscan_t);
    void yyerror(void*, yyscan_t, AST_TranslationUnit**, const LinesWarpMap *warps, const char *str);

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

%start trans_unit

%union {
    int boolean;
    int enum_val;
    struct AST_Literal num_literal;
    string_id_t str;
    string_id_t ident_id;

    struct AST_Expr *expr;

    struct AST_ArgumentsList *args_list;
    struct AST_StringsSeq *str_seq;

    struct AST_DeclSpecifiers *decl_specifiers;
    struct AST_TypeSpecifier *type_specifier;
    struct AST_StructDeclarationList *struct_decl_lst;
    struct AST_StructDeclaration *struct_decl;
    struct AST_SpecsQualsList *spec_qual_lst;
    struct AST_StructDeclarator *struct_delarator;
    struct AST_StructDeclaratorList *struct_delarator_lst;
    struct AST_UStructSpec *struct_or_union_spec;
    struct AST_Enumerator *enumerator;
    struct AST_EnumeratorList *enumerator_lst;
    struct AST_EnumSpecifier *enum_spec;

    struct AST_InitDeclarator *init_decltor;
    struct AST_InitDeclaratorList *init_decltor_lst;
    struct AST_Declaration *declaration;
    struct AST_DirDeclarator *dir_decltor;
    struct AST_Pointer *pointer;
    struct AST_Declarator *declarator;
    struct AST_ParameterDeclaration *param_declaration;
    struct AST_ParameterTypeList *param_type_lst;
    struct AST_ParameterList *param_lst;
    struct AST_TypeName *type_name;
    struct AST_DirAbstrDeclarator *dir_abstract_decl;
    struct AST_AbstrDeclarator *abstract_decl;
    struct AST_TypeQuals *type_quals;

    struct AST_Designator *designator;
    struct AST_InitializerList *initializer_lst;
    struct AST_Initializer *initializer;

    struct AST_Stmt *stmt;
    struct AST_LabeledStmt *label_stmt;
    struct AST_BlockItemList *block_item_lst;
    struct AST_CompoundStmt *compound_stmt;
    struct AST_ExprStmt *expr_stmt;
    struct AST_SelectionStmt *select_stmt;
    struct AST_IterStmt *iter_stmt;
    struct AST_JumpStmt *jump_stmt;

    struct AST_FunctionDef *func_def;
    struct AST_TranslationUnit *trans_unit;
}

%token <ident_id> IDENTIFIER
%token <ident_id> TYPE_NAME
%token <num_literal> CONSTANT
%token <str> STRING_LITERAL

%type <str_seq> strings_seq
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
%type <expr> land_expr
%type <expr> lor_expr
%type <expr> ternary_expr
%type <expr> assign_expr
%type <enum_val> assign_op
%type <expr> expr
%type <expr> const_expr

%type <decl_specifiers> decl_specs
%type <enum_val> storage_class_specifier
%type <type_specifier> type_specifier
%type <enum_val> type_qual
%type <enum_val> function_specifier
%type <struct_or_union_spec> ustruct_spec
%type <boolean> ustruct
%type <struct_decl_lst> struct_decls_lst
%type <struct_decl> struct_declaration
%type <spec_qual_lst> spec_qual_lst
%type <struct_delarator_lst> struct_declarator_list
%type <struct_delarator> struct_declarator
%type <enumerator> enumerator
%type <enumerator_lst> enumerator_list
%type <enum_spec> enum_specifier

%type <declaration> declaration
%type <init_decltor_lst> init_decltor_list
%type <init_decltor> init_decltor
%type <declarator> declarator
%type <pointer> pointer
%type <dir_decltor> dir_decltor
%type <type_quals> type_qual_lst
%type <param_type_lst> param_type_lst
%type <param_lst> param_lst
%type <param_declaration> parameter_declaration
%type <type_name> type_name
%type <abstract_decl> abstr_decltor;
%type <dir_abstract_decl> dir_abstr_decltor;

%type <initializer> initializer
%type <initializer_lst> init_lst
%type <designator> designator

%type <stmt> stmt
%type <label_stmt> label_stmt
%type <block_item_lst> block_items
%type <compound_stmt> compound_stmt
%type <expr_stmt> expr_stmt
%type <select_stmt> select_stmt
%type <iter_stmt> loop_stmt
%type <jump_stmt> jmp_stmt

%type <func_def> func_def
%type <trans_unit> trans_unit

%%

    /* Expressions */

primary_expr
    : IDENTIFIER                                { $$ = AST_Primary::makeIdent($1); SL($$, @$); }
    | CONSTANT                                  { $$ = AST_Primary::makeConst($1); SL($$, @$); }
    | strings_seq                               { $$ = AST_Primary::makeStr($1); SL($$, @$); }
    | '(' expr ')'                              { $$ = AST_Primary::makeExpr($2); SL($$, @$); }
    | '(' type_name ')' '{' init_lst '}'        { $$ = AST_Primary::makeCompound($2, $5); SL($$, @$); }
    | '(' type_name ')' '{' init_lst ',' '}'    { $$ = AST_Primary::makeCompound($2, $5); SL($$, @$); }
    ;

strings_seq
    : STRING_LITERAL                            { $$ = (new AST_StringsSeq())->append($1); SL($$, @$); }
    | strings_seq STRING_LITERAL                { $$ = $1->append($2); }
    ;

postfix_expr
    : primary_expr                              { $$ = $1; }
    | postfix_expr '[' expr ']'                 { $$ = AST_Postfix::makeArr($1, $3); SL($$, @$); }
    | postfix_expr '(' ')'                      { $$ = AST_Postfix::makeCall($1, nullptr); SL($$, @$); }
    | postfix_expr '(' arg_expr_lst ')'         { $$ = AST_Postfix::makeCall($1, $3); SL($$, @$); }
    | postfix_expr '.' IDENTIFIER               { $$ = AST_Postfix::makeAccesor($1, $3, false); SL($$, @$); }
    | postfix_expr PTR_OP IDENTIFIER            { $$ = AST_Postfix::makeAccesor($1, $3, true); SL($$, @$); }
    | postfix_expr INC_OP                       { $$ = AST_Postfix::makeIncdec($1, false); SL($$, @$); }
    | postfix_expr DEC_OP                       { $$ = AST_Postfix::makeIncdec($1, true); SL($$, @$); }
    ;

arg_expr_lst
    : assign_expr                               { $$ = (new AST_ArgumentsList())->append($1); SL($$, @$); }
    | arg_expr_lst ',' assign_expr              { $$ = $1->append($3); };
    ;

unary_expr
    : postfix_expr                              { $$ = $1; }
    | INC_OP unary_expr                         { $$ = new AST_Unop(AST_Unop::PRE_INC, $2); SL($$, @$); }
    | DEC_OP unary_expr                         { $$ = new AST_Unop(AST_Unop::PRE_DEC, $2); SL($$, @$); }
    | unary_operator cast_expr                  { $$ = new AST_Unop(AST_Unop::OpType($1), $2); SL($$, @$); }
    | SIZEOF unary_expr                         { $$ = new AST_Unop(AST_Unop::SIZEOF_OP, $2); SL($$, @$); }
    | SIZEOF '(' type_name ')'                  { $$ = new AST_Unop(AST_Unop::SIZEOF_OP, $3); SL($$, @$); }
    ;

unary_operator
    : '&'                                       { $$ = AST_Unop::ADDR_OF; }
    | '*'                                       { $$ = AST_Unop::DEREF; }
    | '+'                                       { $$ = AST_Unop::UN_PLUS; }
    | '-'                                       { $$ = AST_Unop::UN_MINUS; }
    | '~'                                       { $$ = AST_Unop::UN_NEG; }
    | '!'                                       { $$ = AST_Unop::UN_NOT; }
    ;

cast_expr
    : unary_expr                                { $$ = $1; }
    | '(' type_name ')' cast_expr               { $$ = new AST_Cast($2, $4); SL($$, @$); }
    ;

mult_expr
    : cast_expr                                 { $$ = $1; }
    | mult_expr '*' cast_expr                   { $$ = new AST_Binop(AST_Binop::MUL, $1, $3); SL($$, @$); }
    | mult_expr '/' cast_expr                   { $$ = new AST_Binop(AST_Binop::DIV, $1, $3); SL($$, @$); }
    | mult_expr '%' cast_expr                   { $$ = new AST_Binop(AST_Binop::REM, $1, $3); SL($$, @$); }
    ;

add_expr
    : mult_expr                                 { $$ = $1; }
    | add_expr '+' mult_expr                    { $$ = new AST_Binop(AST_Binop::ADD, $1, $3); SL($$, @$); }
    | add_expr '-' mult_expr                    { $$ = new AST_Binop(AST_Binop::SUB, $1, $3); SL($$, @$); }
    ;

shift_expr
    : add_expr                                  { $$ = $1; }
    | shift_expr LEFT_OP add_expr               { $$ = new AST_Binop(AST_Binop::SHL, $1, $3); SL($$, @$); }
    | shift_expr RIGHT_OP add_expr              { $$ = new AST_Binop(AST_Binop::SHR, $1, $3); SL($$, @$); }
    ;

rel_expr
    : shift_expr                                { $$ = $1; }
    | rel_expr '<' shift_expr                   { $$ = new AST_Binop(AST_Binop::LT, $1, $3); SL($$, @$); }
    | rel_expr '>' shift_expr                   { $$ = new AST_Binop(AST_Binop::GT, $1, $3); SL($$, @$); }
    | rel_expr LE_OP shift_expr                 { $$ = new AST_Binop(AST_Binop::LE, $1, $3); SL($$, @$); }
    | rel_expr GE_OP shift_expr                 { $$ = new AST_Binop(AST_Binop::GE, $1, $3); SL($$, @$); }
    ;

eq_expr
    : rel_expr                                  { $$ = $1; }
    | eq_expr EQ_OP rel_expr                    { $$ = new AST_Binop(AST_Binop::EQ, $1, $3); SL($$, @$); }
    | eq_expr NE_OP rel_expr                    { $$ = new AST_Binop(AST_Binop::NE, $1, $3); SL($$, @$); }
    ;

and_expr
    : eq_expr                                   { $$ = $1; }
    | and_expr '&' eq_expr                      { $$ = new AST_Binop(AST_Binop::BIT_AND, $1, $3); SL($$, @$); }
    ;

xor_expr
    : and_expr                                  { $$ = $1; }
    | xor_expr '^' and_expr                     { $$ = new AST_Binop(AST_Binop::BIT_XOR, $1, $3); SL($$, @$); }
    ;

or_expr
    : xor_expr                                  { $$ = $1; }
    | or_expr '|' xor_expr                      { $$ = new AST_Binop(AST_Binop::BIT_OR, $1, $3); SL($$, @$); }
    ;

land_expr
    : or_expr                                   { $$ = $1; }
    | land_expr AND_OP or_expr                  { $$ = new AST_Binop(AST_Binop::LOG_AND, $1, $3); SL($$, @$); }
    ;

lor_expr
    : land_expr                                 { $$ = $1; }
    | lor_expr OR_OP land_expr                  { $$ = new AST_Binop(AST_Binop::LOG_OR, $1, $3); SL($$, @$); }
    ;

ternary_expr
    : lor_expr                                  { $$ = $1; }
    | lor_expr '?' expr ':' ternary_expr        { $$ = new AST_Ternary($1, $3, $5); SL($$, @$); }
    ;

assign_expr
    : ternary_expr                              { $$ = $1; }
    | unary_expr assign_op assign_expr          { $$ = new AST_Assignment(AST_Assignment::OpType($2), $1, $3); SL($$, @$); }
    ;

assign_op
    : '='                                       { $$ = AST_Assignment::DIRECT; }
    | MUL_ASSIGN                                { $$ = AST_Assignment::MUL; }
    | DIV_ASSIGN                                { $$ = AST_Assignment::DIV; }
    | MOD_ASSIGN                                { $$ = AST_Assignment::REM; }
    | ADD_ASSIGN                                { $$ = AST_Assignment::ADD; }
    | SUB_ASSIGN                                { $$ = AST_Assignment::SUB; }
    | LEFT_ASSIGN                               { $$ = AST_Assignment::SHL; }
    | RIGHT_ASSIGN                              { $$ = AST_Assignment::SHR; }
    | AND_ASSIGN                                { $$ = AST_Assignment::AND; }
    | XOR_ASSIGN                                { $$ = AST_Assignment::XOR; }
    | OR_ASSIGN                                 { $$ = AST_Assignment::OR; }
    ;

expr
    : assign_expr                               { $$ = $1; /* TODO: fixme */ }
    | expr ',' assign_expr                      { $$ = dynamic_cast<AST_CommaExpression*>($1)->append($3); }
    ;

const_expr
    : ternary_expr                              { $$ = $1; }
    ;

    /* Specifiers */

decl_specs
    : storage_class_specifier                   { $$ = (new AST_DeclSpecifiers())->update_storage($1); SL($$, @$); }
    | storage_class_specifier decl_specs        { $$ = $2->update_storage($1); }
    | type_specifier                            { $$ = (new AST_DeclSpecifiers())->update_type_spec($1); SL($$, @$); }
    | type_specifier decl_specs                 { $$ = $2->update_type_spec($1); }
    | type_qual                                 { $$ = (new AST_DeclSpecifiers())->update_type_qual($1); SL($$, @$); }
    | type_qual decl_specs                      { $$ = $2->update_type_qual($1); }
    | function_specifier                        { $$ = (new AST_DeclSpecifiers())->update_func_qual($1); SL($$, @$); }
    | function_specifier decl_specs             { $$ = $2->update_func_qual($1); }
    ;

storage_class_specifier
    : TYPEDEF                   { $$ = AST_DeclSpecifiers::ST_TYPEDEF; }
    | EXTERN                    { $$ = AST_DeclSpecifiers::ST_EXTERN; }
    | STATIC                    { $$ = AST_DeclSpecifiers::ST_STATIC; }
    | AUTO                      { $$ = AST_DeclSpecifiers::ST_AUTO; }
    | REGISTER                  { $$ = AST_DeclSpecifiers::ST_REGISTER; }
    ;

type_specifier
    : VOID                      { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_VOID); }
    | CHAR                      { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_CHAR); }
    | SHORT                     { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_SHORT); }
    | INT                       { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_INT); }
    | LONG                      { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_LONG); }
    | FLOAT                     { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_FLOAT); }
    | DOUBLE                    { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_DOUBLE); }
    | SIGNED                    { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_SIGNED); }
    | UNSIGNED                  { $$ = new AST_TypeSpecifier(AST_TypeSpecifier::T_UNSIGNED); }
    | ustruct_spec              { $$ = new AST_TypeSpecifier($1); }
    | enum_specifier            { $$ = new AST_TypeSpecifier($1); }
    | TYPE_NAME                 { $$ = new AST_TypeSpecifier(get_def_type($1)); }
    ;

type_qual
    : CONST                     { $$ = AST_TypeQuals::Q_CONST; }
    | RESTRICT                  { $$ = AST_TypeQuals::Q_RESTRICT; }
    | VOLATILE                  { $$ = AST_TypeQuals::Q_VOLATILE; }
    ;

function_specifier
    : INLINE                    { $$ = AST_DeclSpecifiers::Q_INLINE; }
    ;

ustruct_spec
    : ustruct IDENTIFIER '{' struct_decls_lst '}'   { $$ = new AST_UStructSpec($1, $2, $4); SL($$, @$); }
    | ustruct '{' struct_decls_lst '}'              { $$ = new AST_UStructSpec($1, NO_IDENT_ID, $3); SL($$, @$); }
    | ustruct IDENTIFIER                            { $$ = new AST_UStructSpec($1, $2, nullptr); SL($$, @$); }
    ;

ustruct
    : STRUCT                                        { $$ = 0; }
    | UNION                                         { $$ = 1; }
    ;

struct_decls_lst
    : struct_declaration                            { $$ = new AST_StructDeclarationList($1); SL($$, @$); }
    | struct_decls_lst struct_declaration           { $$ = $1->append($2); }
    ;

struct_declaration
    : spec_qual_lst struct_declarator_list ';'      { $$ = new AST_StructDeclaration($1, $2); SL($$, @$); }
    ;

spec_qual_lst
    : type_specifier spec_qual_lst      { $$ = $2->append_spec($1); }
    | type_specifier                    { $$ = new AST_SpecsQualsList($1); SL($$, @$); }
    | type_qual spec_qual_lst           { $$ = $2->append_qual(AST_TypeQuals::QualType($1)); }
    | type_qual                         { $$ = new AST_SpecsQualsList(AST_TypeQuals::QualType($1)); SL($$, @$); }
    ;

struct_declarator_list
    : struct_declarator                                 { $$ = new AST_StructDeclaratorList($1); SL($$, @$); }
    | struct_declarator_list ',' struct_declarator      { $$ = $1->append($3); }
    ;

struct_declarator
    : declarator                                        { $$ = new AST_StructDeclarator($1, nullptr); SL($$, @$); }
    | ':' const_expr                                    { $$ = new AST_StructDeclarator(nullptr, $2); SL($$, @$); }
    | declarator ':' const_expr                         { $$ = new AST_StructDeclarator($1, $3); SL($$, @$); }
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'                      { $$ = new AST_EnumSpecifier(NO_IDENT_ID, $3); SL($$, @$); }
    | ENUM IDENTIFIER '{' enumerator_list '}'           { $$ = new AST_EnumSpecifier($2, $4); SL($$, @$); }
    | ENUM '{' enumerator_list ',' '}'                  { $$ = new AST_EnumSpecifier(NO_IDENT_ID, $3); SL($$, @$); }
    | ENUM IDENTIFIER '{' enumerator_list ',' '}'       { $$ = new AST_EnumSpecifier($2, $4); SL($$, @$); }
    | ENUM IDENTIFIER                                   { $$ = new AST_EnumSpecifier($2, nullptr); SL($$, @$); }
    ;

enumerator_list
    : enumerator                                        { $$ = new AST_EnumeratorList($1); SL($$, @$); }
    | enumerator_list ',' enumerator                    { $$ = $1->append($3); }
    ;

enumerator
    : IDENTIFIER                                        { $$ = new AST_Enumerator($1, nullptr); SL($$, @$); }
    | IDENTIFIER '=' const_expr                         { $$ = new AST_Enumerator($1, $3); SL($$, @$); }
    ;


    /* Declarations */

declaration
    : decl_specs ';'                                    { $$ = new AST_Declaration($1, nullptr); SL($$, @$); }
    | decl_specs init_decltor_list ';'                  { $$ = new AST_Declaration($1, $2); SL($$, @$); }
    ;

init_decltor_list
    : init_decltor                                      { $$ = new AST_InitDeclaratorList($1); SL($$, @$); }
    | init_decltor_list ',' init_decltor                { $$ = $1->append($3); }
    ;

init_decltor
    : declarator                                        { $$ = new AST_InitDeclarator($1, nullptr); SL($$, @$); }
    | declarator '=' initializer                        { $$ = new AST_InitDeclarator($1, $3); SL($$, @$); }
    ;

declarator
    : pointer dir_decltor               { $$ = new AST_Declarator($2, $1); SL($$, @$); }
    | dir_decltor                       { $$ = new AST_Declarator($1, nullptr); SL($$, @$); }
    ;

pointer
    : '*'                               { $$ = new AST_Pointer(nullptr, nullptr); SL($$, @$); }
    | '*' type_qual_lst                 { $$ = new AST_Pointer($2, nullptr); SL($$, @$); }
    | '*' pointer                       { $$ = new AST_Pointer(nullptr, $2); SL($$, @$); }
    | '*' type_qual_lst pointer         { $$ = new AST_Pointer($2, $3); SL($$, @$); }
    ;

dir_decltor
    : IDENTIFIER                                        { $$ = AST_DirDeclarator::makeIdent($1); SL($$, @$); }
    | '(' declarator ')'                                { $$ = AST_DirDeclarator::makeNested($2); SL($$, @$); }
    | dir_decltor '[' type_qual_lst assign_expr ']'     { $$ = AST_DirDeclarator::makeArr($1, $3, $4); SL($$, @$); }
    | dir_decltor '[' type_qual_lst ']'                 { $$ = AST_DirDeclarator::makeArr($1, $3, nullptr); SL($$, @$); }
    | dir_decltor '[' assign_expr ']'                   { $$ = AST_DirDeclarator::makeArr($1, nullptr, $3); SL($$, @$); }
    | dir_decltor '[' ']'                               { $$ = AST_DirDeclarator::makeArr($1, nullptr, nullptr); SL($$, @$); }
    | dir_decltor '(' param_type_lst ')'                { $$ = AST_DirDeclarator::makeFunc($1, $3); SL($$, @$); }
    | dir_decltor '(' ')'                               { $$ = AST_DirDeclarator::makeFunc($1, nullptr); SL($$, @$); }
    ;

type_qual_lst
    : type_qual                     { $$ = new AST_TypeQuals(AST_TypeQuals::QualType($1)); SL($$, @$); }
    | type_qual_lst type_qual       { $$ = $1->update(AST_TypeQuals::QualType($2)); }
    ;

param_type_lst
    : param_lst                     { $$ = new AST_ParameterTypeList($1, false); SL($$, @$); }
    | param_lst ',' ELLIPSIS        { $$ = new AST_ParameterTypeList($1, true); SL($$, @$); }
    ;

param_lst
    : parameter_declaration                     { $$ = new AST_ParameterList($1); SL($$, @$); }
    | param_lst ',' parameter_declaration       { $$ = $1->append($3); }
    ;

parameter_declaration
    : decl_specs declarator                     { $$ = new AST_ParameterDeclaration($1, $2); SL($$, @$); }
    | decl_specs                                { $$ = new AST_ParameterDeclaration($1, nullptr); SL($$, @$); }
    ;

type_name
    : spec_qual_lst                             { $$ = new AST_TypeName($1, nullptr); SL($$, @$); }
    | spec_qual_lst abstr_decltor               { $$ = new AST_TypeName($1, $2); SL($$, @$); }
    ;

abstr_decltor
    : dir_abstr_decltor                         { $$ = new AST_AbstrDeclarator($1, nullptr); SL($$, @$); }
    | pointer dir_abstr_decltor                 { $$ = new AST_AbstrDeclarator($2, $1); SL($$, @$); }
    | pointer                                   { $$ = new AST_AbstrDeclarator(nullptr, $1); SL($$, @$); }
    ;
    /*     pointer   {  $$ = new AST_AbstrDeclarator(nullptr, $1); } */

dir_abstr_decltor
    : '(' abstr_decltor ')'                         { $$ = AST_DirAbstrDeclarator::makeNested($2); SL($$, @$); }
    | dir_abstr_decltor '[' ']'                     { $$ = AST_DirAbstrDeclarator::makeArr($1, nullptr); SL($$, @$); }
    | dir_abstr_decltor '[' assign_expr ']'         { $$ = AST_DirAbstrDeclarator::makeArr($1, $3); SL($$, @$); }
    | dir_abstr_decltor '(' ')'                     { $$ = AST_DirAbstrDeclarator::makeFunc($1, nullptr); SL($$, @$); }
    | dir_abstr_decltor '(' param_type_lst ')'      { $$ = AST_DirAbstrDeclarator::makeFunc($1, $3); SL($$, @$); }
    ;


    /* Initializers */

initializer
    : assign_expr                                   { $$ = new AST_Initializer($1); SL($$, @$); }
    | '{' init_lst '}'                              { $$ = new AST_Initializer($2); SL($$, @$); }
    | '{' init_lst ',' '}'                          { $$ = new AST_Initializer($2); SL($$, @$); }
    ;

init_lst
    : initializer                                   { $$ = new AST_InitializerList($1, nullptr); SL($$, @$); }
    | designator '=' initializer                    { $$ = new AST_InitializerList($3, $1); SL($$, @$); }
    | init_lst ',' initializer                      { $$ = $1->append($3, nullptr); }
    | init_lst ',' designator '=' initializer       { $$ = $1->append($5, $3); }
    ;

designator
    : '[' const_expr ']'                            { $$ = new AST_Designator($2); SL($$, @$); }
    | '.' IDENTIFIER                                { $$ = new AST_Designator($2); SL($$, @$); }
    ;


    /* Statements */

stmt
    : label_stmt                    { $$ = $1; }
    | compound_stmt                 { $$ = $1; }
    | expr_stmt                     { $$ = $1; }
    | select_stmt                   { $$ = $1; }
    | loop_stmt                     { $$ = $1; }
    | jmp_stmt                      { $$ = $1; }
    ;

label_stmt
    : IDENTIFIER ':' stmt           { $$ = new AST_LabeledStmt($1, $3, AST_LabeledStmt::SIMPL); SL($$, @$); }
    | CASE const_expr ':' stmt      { $$ = new AST_LabeledStmt($2, $4, AST_LabeledStmt::SW_CASE); SL($$, @$); }
    | DEFAULT ':' stmt              { $$ = new AST_LabeledStmt(nullptr, $3, AST_LabeledStmt::SW_DEFAULT); SL($$, @$); }
    ;

compound_stmt
    : '{' '}'                       { $$ = new AST_CompoundStmt(new AST_BlockItemList()); SL($$, @$); }
    | '{' block_items '}'           { $$ = new AST_CompoundStmt($2); SL($$, @$); }
    ;

block_items
    : declaration                   { $$ = (new AST_BlockItemList())->append($1); SL($$, @$); }
    | stmt                          { $$ = (new AST_BlockItemList())->append($1); SL($$, @$); }
    | block_items declaration       { $$ = $1->append($2); }
    | block_items stmt              { $$ = $1->append($2); }
    ;

expr_stmt
    : ';'                           { $$ = new AST_ExprStmt(nullptr); SL($$, @$); }
    | expr ';'                      { $$ = new AST_ExprStmt($1); SL($$, @$); }
    ;

select_stmt
    : IF '(' expr ')' stmt %prec IFX        { $$ = AST_SelectionStmt::get_if($3, $5, nullptr); SL($$, @$); }
    | IF '(' expr ')' stmt ELSE stmt        { $$ = AST_SelectionStmt::get_if($3, $5, $7); SL($$, @$); }
    | SWITCH '(' expr ')' stmt              { $$ = AST_SelectionStmt::get_switch($3, $5); SL($$, @$); }
    ;

loop_stmt
    : WHILE '(' expr ')' stmt                       { $$ = AST_IterStmt::makeWhile($5, $3, false); SL($$, @$); }
    | DO stmt WHILE '(' expr ')' ';'                { $$ = AST_IterStmt::makeWhile($2, $5, true); SL($$, @$); }
    | FOR '(' expr_stmt expr_stmt ')' stmt          { $$ = AST_IterStmt::makeFor($6, $3, $4, nullptr); SL($$, @$); }
    | FOR '(' expr_stmt expr_stmt expr ')' stmt     { $$ = AST_IterStmt::makeFor($7, $3, $4, $5); SL($$, @$); }
    | FOR '(' declaration expr_stmt ')' stmt        { $$ = AST_IterStmt::makeFor($6, $3, $4, nullptr); SL($$, @$); }
    | FOR '(' declaration expr_stmt expr ')' stmt   { $$ = AST_IterStmt::makeFor($7, $3, $4, $5); SL($$, @$); }
    ;

jmp_stmt
    : GOTO IDENTIFIER ';'       { $$ = new AST_JumpStmt(AST_JumpStmt::J_GOTO, $2); SL($$, @$); }
    | CONTINUE ';'              { $$ = new AST_JumpStmt(AST_JumpStmt::J_CONTINUE); SL($$, @$); }
    | BREAK ';'                 { $$ = new AST_JumpStmt(AST_JumpStmt::J_BREAK); SL($$, @$); }
    | RETURN ';'                { $$ = new AST_JumpStmt(AST_JumpStmt::J_RET); SL($$, @$); }
    | RETURN expr ';'           { $$ = new AST_JumpStmt(AST_JumpStmt::J_RET, $2); SL($$, @$); }
    ;


    /* Top-level elements */

trans_unit
    : func_def                      { $$ = *parser_result = (new AST_TranslationUnit())->append($1); SL($$, @$); }
    | declaration                   {
                                        $$ = *parser_result = (new AST_TranslationUnit())->append($1);
                                        check_typedef($1);
                                        SL($$, @$);
                                    }
    | trans_unit func_def           { $$ = $1->append($2); }
    | trans_unit declaration        { $$ = $1->append($2); check_typedef($2); }
    ;

func_def
    : decl_specs declarator compound_stmt       { $$ = new AST_FunctionDef($1, $2, $3); SL($$, @$); }
    ;


%%

AST_TranslationUnit* CoreParser::parse_program(std::string const &str,
                                               CoreParserState *state,
                                               const LinesWarpMap *warps) {
    yyscan_t scanner;
    lex_extra_t extra;
    extra.state = state;
    extra.warps = warps;
    init_scanner(str.c_str(), &scanner, &extra);
    ast_set_pstate_ptr(state);
    AST_TranslationUnit *res;
    int isFailure = yyparse(scanner, &res, warps);
    destroy_scanner(scanner);
    ast_set_pstate_ptr(nullptr);
    if (isFailure) {
        fprintf(stderr, "Parsing failed\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

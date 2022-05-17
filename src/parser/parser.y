%skeleton "lalr1.cc"
%require "3.8.1"
%header

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include "ast.hpp"
    class CoreDriver;
    using yyscan_t = void*;
}

%param { yyscan_t scanner }
%param { CoreDriver &drv }

%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
    #include "core_driver.hpp"
    #define yylex yycore_lex
    #include "lexer.hpp"
}

%define api.token.prefix {CORE_TOK_}

%token
    SEMICOLON   ";"
    LBRACE      "{"
    RBRACE      "}"
    COMMA       ","
    COLON       ":"
    ASSIGN      "="
    LPAREN      "("
    RPAREN      ")"
    LBRACKET    "["
    RBRACKET    "]"
    DOT         "."
    AMPERSAND   "&"
    EXCMARK     "!"
    TILDA       "~"
    MINUS       "-"
    PLUS        "+"
    ASTERISK    "*"
    SLASH       "/"
    PERCENT     "%"
    LANGLE      "<"
    RANGLE      ">"
    CIRCUM      "^"
    VBAR        "|"
    QUESTMARK   "?"

%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN SIZEOF

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token EXT_PURE EXT_FCONST EXT_WEAK
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%precedence IFX
%precedence ELSE

%start trans_unit

%token <string_id_t> IDENTIFIER
%token <string_id_t> TYPE_NAME
%token <AST_Literal> CONSTANT
%token <string_id_t> STRING_LITERAL

%type <AST_StringsSeq *> strings_seq
%type <AST_Expr *> primary_expr
%type <AST_Expr *> postfix_expr
%type <AST_ArgumentsList *> arg_expr_lst
%type <AST_Expr *> unary_expr
%type <short> unary_operator
%type <AST_Expr *> cast_expr
%type <AST_Expr *> mult_expr
%type <AST_Expr *> add_expr
%type <AST_Expr *> shift_expr
%type <AST_Expr *> rel_expr
%type <AST_Expr *> eq_expr
%type <AST_Expr *> and_expr
%type <AST_Expr *> xor_expr
%type <AST_Expr *> or_expr
%type <AST_Expr *> land_expr
%type <AST_Expr *> lor_expr
%type <AST_Expr *> ternary_expr
%type <AST_Expr *> assign_expr
%type <short> assign_op
%type <AST_Expr *> expr
%type <AST_Expr *> const_expr

%type <string_id_t> general_ident
%type <AST_DeclSpecifiers *> decl_specs
%type <short> storage_class_specifier
%type <AST_TypeSpecifier *> type_specifier
%type <short> type_qual
%type <short> function_specifier
%type <AST_UStructSpec *> ustruct_spec
%type <short> ustruct
%type <AST_StructDeclarationList *> struct_decls_lst
%type <AST_StructDeclaration *> struct_declaration
%type <AST_SpecsQualsList *> spec_qual_lst
%type <AST_StructDeclaratorList *> struct_declarator_list
%type <AST_StructDeclarator *> struct_declarator
%type <AST_Enumerator *> enumerator
%type <AST_EnumeratorList *> enumerator_list
%type <AST_EnumSpecifier *> enum_specifier

%type <AST_Declaration *> declaration
%type <AST_InitDeclaratorList *> init_decltor_list
%type <AST_InitDeclarator *> init_decltor
%type <AST_Declarator *> declarator
%type <AST_Pointer *> pointer
%type <AST_DirDeclarator *> dir_decltor
%type <AST_TypeQuals *> type_qual_lst
%type <AST_ParameterTypeList *> param_type_lst
%type <AST_ParameterList *> param_lst
%type <AST_ParameterDeclaration *> parameter_declaration
%type <AST_TypeName *> type_name
%type <AST_AbstrDeclarator *> abstr_decltor;
%type <AST_DirAbstrDeclarator *> dir_abstr_decltor;

%type <AST_Initializer *> initializer
%type <AST_InitializerList *> init_lst
%type <AST_Designator *> designator

%type <AST_Stmt *> stmt
%type <AST_LabeledStmt *> label_stmt
%type <AST_BlockItemList *> block_items
%type <AST_CompoundStmt *> compound_stmt
%type <AST_ExprStmt *> expr_stmt
%type <AST_SelectionStmt *> select_stmt
%type <AST_IterStmt *> loop_stmt
%type <AST_JumpStmt *> jmp_stmt

%type <AST_FunctionDef *> func_def
%type <AST_TranslationUnit *> trans_unit

%%

    /* Expressions */

primary_expr
    : IDENTIFIER                                { $$ = drv.ast->mkPrimIdent($1); SL($$, @$); }
    | CONSTANT                                  { $$ = drv.ast->mkPrimConst($1); SL($$, @$); }
    | strings_seq                               { $$ = drv.ast->mkPrimStr($1); SL($$, @$); }
    | "(" expr ")"                              { $$ = drv.ast->mkPrimExpr($2); SL($$, @$); }
    | "(" type_name ")" "{" init_lst "}"        { $$ = drv.ast->mkPrimCompound($2, $5); SL($$, @$); }
    | "(" type_name ")" "{" init_lst "," "}"    { $$ = drv.ast->mkPrimCompound($2, $5); SL($$, @$); }
    ;

strings_seq
    : STRING_LITERAL                            { $$ = drv.ast->mkStringsSeq()->append($1); SL($$, @$); }
    | strings_seq STRING_LITERAL                { $$ = $1->append($2); }
    ;

postfix_expr
    : primary_expr                              { $$ = $1; }
    | postfix_expr "[" expr "]"                 { $$ = drv.ast->mkPostfArr($1, $3); SL($$, @$); }
    | postfix_expr "(" ")"                      { $$ = drv.ast->mkPostfCall($1, nullptr); SL($$, @$); }
    | postfix_expr "(" arg_expr_lst ")"         { $$ = drv.ast->mkPostfCall($1, $3); SL($$, @$); }
    | postfix_expr "." IDENTIFIER               { $$ = drv.ast->mkPostfAccesor($1, $3, false); SL($$, @$); }
    | postfix_expr PTR_OP IDENTIFIER            { $$ = drv.ast->mkPostfAccesor($1, $3, true); SL($$, @$); }
    | postfix_expr INC_OP                       { $$ = drv.ast->mkPostfIncdec($1, false); SL($$, @$); }
    | postfix_expr DEC_OP                       { $$ = drv.ast->mkPostfIncdec($1, true); SL($$, @$); }
    ;

arg_expr_lst
    : assign_expr                               { $$ = drv.ast->mkArgsLst()->append($1); SL($$, @$); }
    | arg_expr_lst "," assign_expr              { $$ = $1->append($3); };
    ;

unary_expr
    : postfix_expr                              { $$ = $1; }
    | INC_OP unary_expr                         { $$ = drv.ast->mkUnop(AST_Unop::PRE_INC, $2); SL($$, @$); }
    | DEC_OP unary_expr                         { $$ = drv.ast->mkUnop(AST_Unop::PRE_DEC, $2); SL($$, @$); }
    | unary_operator cast_expr                  { $$ = drv.ast->mkUnop(AST_Unop::OpType($1), $2); SL($$, @$); }
    | SIZEOF unary_expr                         { $$ = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, $2); SL($$, @$); }
    | SIZEOF "(" type_name ")"                  { $$ = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, $3); SL($$, @$); }
    ;

unary_operator
    : "&"                                       { $$ = AST_Unop::ADDR_OF; }
    | "*"                                       { $$ = AST_Unop::DEREF; }
    | "+"                                       { $$ = AST_Unop::UN_PLUS; }
    | "-"                                       { $$ = AST_Unop::UN_MINUS; }
    | "~"                                       { $$ = AST_Unop::UN_NEG; }
    | "!"                                       { $$ = AST_Unop::UN_NOT; }
    ;

cast_expr
    : unary_expr                                { $$ = $1; }
    | "(" type_name ")" cast_expr               { $$ = drv.ast->mkCastop($2, $4); SL($$, @$); }
    ;

mult_expr
    : cast_expr                                 { $$ = $1; }
    | mult_expr "*" cast_expr                   { $$ = drv.ast->mkBinop(AST_Binop::MUL, $1, $3); SL($$, @$); }
    | mult_expr "/" cast_expr                   { $$ = drv.ast->mkBinop(AST_Binop::DIV, $1, $3); SL($$, @$); }
    | mult_expr "%" cast_expr                   { $$ = drv.ast->mkBinop(AST_Binop::REM, $1, $3); SL($$, @$); }
    ;

add_expr
    : mult_expr                                 { $$ = $1; }
    | add_expr "+" mult_expr                    { $$ = drv.ast->mkBinop(AST_Binop::ADD, $1, $3); SL($$, @$); }
    | add_expr "-" mult_expr                    { $$ = drv.ast->mkBinop(AST_Binop::SUB, $1, $3); SL($$, @$); }
    ;

shift_expr
    : add_expr                                  { $$ = $1; }
    | shift_expr LEFT_OP add_expr               { $$ = drv.ast->mkBinop(AST_Binop::SHL, $1, $3); SL($$, @$); }
    | shift_expr RIGHT_OP add_expr              { $$ = drv.ast->mkBinop(AST_Binop::SHR, $1, $3); SL($$, @$); }
    ;

rel_expr
    : shift_expr                                { $$ = $1; }
    | rel_expr "<" shift_expr                   { $$ = drv.ast->mkBinop(AST_Binop::LT, $1, $3); SL($$, @$); }
    | rel_expr ">" shift_expr                   { $$ = drv.ast->mkBinop(AST_Binop::GT, $1, $3); SL($$, @$); }
    | rel_expr LE_OP shift_expr                 { $$ = drv.ast->mkBinop(AST_Binop::LE, $1, $3); SL($$, @$); }
    | rel_expr GE_OP shift_expr                 { $$ = drv.ast->mkBinop(AST_Binop::GE, $1, $3); SL($$, @$); }
    ;

eq_expr
    : rel_expr                                  { $$ = $1; }
    | eq_expr EQ_OP rel_expr                    { $$ = drv.ast->mkBinop(AST_Binop::EQ, $1, $3); SL($$, @$); }
    | eq_expr NE_OP rel_expr                    { $$ = drv.ast->mkBinop(AST_Binop::NE, $1, $3); SL($$, @$); }
    ;

and_expr
    : eq_expr                                   { $$ = $1; }
    | and_expr "&" eq_expr                      { $$ = drv.ast->mkBinop(AST_Binop::BIT_AND, $1, $3); SL($$, @$); }
    ;

xor_expr
    : and_expr                                  { $$ = $1; }
    | xor_expr "^" and_expr                     { $$ = drv.ast->mkBinop(AST_Binop::BIT_XOR, $1, $3); SL($$, @$); }
    ;

or_expr
    : xor_expr                                  { $$ = $1; }
    | or_expr "|" xor_expr                      { $$ = drv.ast->mkBinop(AST_Binop::BIT_OR, $1, $3); SL($$, @$); }
    ;

land_expr
    : or_expr                                   { $$ = $1; }
    | land_expr AND_OP or_expr                  { $$ = drv.ast->mkBinop(AST_Binop::LOG_AND, $1, $3); SL($$, @$); }
    ;

lor_expr
    : land_expr                                 { $$ = $1; }
    | lor_expr OR_OP land_expr                  { $$ = drv.ast->mkBinop(AST_Binop::LOG_OR, $1, $3); SL($$, @$); }
    ;

ternary_expr
    : lor_expr                                  { $$ = $1; }
    | lor_expr "?" expr ":" ternary_expr        { $$ = drv.ast->mkTernary($1, $3, $5); SL($$, @$); }
    ;

assign_expr
    : ternary_expr                              { $$ = $1; }
    | unary_expr assign_op assign_expr          { $$ = drv.ast->mkAssign(AST_Assignment::OpType($2), $1, $3); SL($$, @$); }
    ;

assign_op
    : "="                                       { $$ = AST_Assignment::DIRECT; }
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
    | expr "," assign_expr                      { $$ = dynamic_cast<AST_CommaExpression*>($1)->append($3); }
    ;

const_expr
    : ternary_expr                              { $$ = $1; }
    ;

    /* Specifiers */

decl_specs
    : storage_class_specifier                   { $$ = drv.ast->mkDeclSpecs()->update_storage($1); SL($$, @$); }
    | storage_class_specifier decl_specs        { $$ = $2->update_storage($1); }
    | type_specifier                            { $$ = drv.ast->mkDeclSpecs()->update_type_spec($1); SL($$, @$); }
    | type_specifier decl_specs                 { $$ = $2->update_type_spec($1); }
    | type_qual                                 { $$ = drv.ast->mkDeclSpecs()->update_type_qual($1); SL($$, @$); }
    | type_qual decl_specs                      { $$ = $2->update_type_qual($1); }
    | function_specifier                        { $$ = drv.ast->mkDeclSpecs()->update_func_qual($1); SL($$, @$); }
    | function_specifier decl_specs             { $$ = $2->update_func_qual($1); }
    ;

storage_class_specifier
    : TYPEDEF                   { $$ = AST_DeclSpecifiers::ST_TYPEDEF; }
    | EXTERN                    { $$ = AST_DeclSpecifiers::ST_EXTERN; }
    | STATIC                    { $$ = AST_DeclSpecifiers::ST_STATIC; }
    | AUTO                      { $$ = AST_DeclSpecifiers::ST_AUTO; }
    | REGISTER                  { $$ = AST_DeclSpecifiers::ST_REGISTER; }
    | EXT_WEAK                  { $$ = AST_DeclSpecifiers::ST_WEAK; }
    ;

type_specifier
    : VOID                      { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_VOID); SL($$, @$); }
    | CHAR                      { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_CHAR); SL($$, @$); }
    | SHORT                     { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SHORT); SL($$, @$); }
    | INT                       { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_INT); SL($$, @$); }
    | LONG                      { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_LONG); SL($$, @$); }
    | FLOAT                     { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_FLOAT); SL($$, @$); }
    | DOUBLE                    { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_DOUBLE); SL($$, @$); }
    | SIGNED                    { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SIGNED); SL($$, @$); }
    | UNSIGNED                  { $$ = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_UNSIGNED); SL($$, @$); }
    | ustruct_spec              { $$ = drv.ast->mkTypeSpec($1); SL($$, @$); }
    | enum_specifier            { $$ = drv.ast->mkTypeSpec($1); SL($$, @$); }
    | TYPE_NAME                 { $$ = drv.getDefinedType($1); SL($$, @$); }
    ;

type_qual
    : CONST                     { $$ = AST_TypeQuals::Q_CONST; }
    | RESTRICT                  { $$ = AST_TypeQuals::Q_RESTRICT; }
    | VOLATILE                  { $$ = AST_TypeQuals::Q_VOLATILE; }
    ;

function_specifier
    : INLINE                    { $$ = AST_DeclSpecifiers::Q_INLINE; }
    | EXT_PURE                  { $$ = AST_DeclSpecifiers::Q_PURE; }
    | EXT_FCONST                { $$ = AST_DeclSpecifiers::Q_FCONST; }
    ;

ustruct_spec
    : ustruct general_ident "{" struct_decls_lst "}"   { $$ = drv.ast->mkUstructSpec($1, $2, $4); SL($$, @$); }
    | ustruct "{" struct_decls_lst "}"                 { $$ = drv.ast->mkUstructSpec($1, NO_IDENT_ID, $3); SL($$, @$); }
    | ustruct general_ident                            { $$ = drv.ast->mkUstructSpec($1, $2, nullptr); SL($$, @$); }
    ;

general_ident
    : IDENTIFIER
    | TYPE_NAME

ustruct
    : STRUCT                                        { $$ = 0; }
    | UNION                                         { $$ = 1; }
    ;

struct_decls_lst
    : struct_declaration                            { $$ = drv.ast->mkStructDeclarationLst($1); SL($$, @$); }
    | struct_decls_lst struct_declaration           { $$ = $1->append($2); }
    ;

struct_declaration
    : spec_qual_lst struct_declarator_list ";"      { $$ = drv.ast->mkStructDeclaration($1, $2); SL($$, @$); }
    ;

spec_qual_lst
    : type_specifier spec_qual_lst      { $$ = $2->append_spec($1); }
    | type_specifier                    { $$ = drv.ast->mkSpecQualLst($1); SL($$, @$); }
    | type_qual spec_qual_lst           { $$ = $2->append_qual(AST_TypeQuals::QualType($1)); }
    | type_qual                         { $$ = drv.ast->mkSpecQualLst(AST_TypeQuals::QualType($1)); SL($$, @$); }
    ;

struct_declarator_list
    : struct_declarator                                 { $$ = drv.ast->mkStructDeclaratorLst($1); SL($$, @$); }
    | struct_declarator_list "," struct_declarator      { $$ = $1->append($3); }
    ;

struct_declarator
    : declarator                                        { $$ = drv.ast->mkStructDeclarator($1, nullptr); SL($$, @$); }
    | ":" const_expr                                    { $$ = drv.ast->mkStructDeclarator(nullptr, $2); SL($$, @$); }
    | declarator ":" const_expr                         { $$ = drv.ast->mkStructDeclarator($1, $3); SL($$, @$); }
    ;

enum_specifier
    : ENUM "{" enumerator_list "}"                      { $$ = drv.ast->mkEnumSpec(NO_IDENT_ID, $3); SL($$, @$); }
    | ENUM IDENTIFIER "{" enumerator_list "}"           { $$ = drv.ast->mkEnumSpec($2, $4); SL($$, @$); }
    | ENUM "{" enumerator_list "," "}"                  { $$ = drv.ast->mkEnumSpec(NO_IDENT_ID, $3); SL($$, @$); }
    | ENUM IDENTIFIER "{" enumerator_list "," "}"       { $$ = drv.ast->mkEnumSpec($2, $4); SL($$, @$); }
    | ENUM IDENTIFIER                                   { $$ = drv.ast->mkEnumSpec($2, nullptr); SL($$, @$); }
    ;

enumerator_list
    : enumerator                                        { $$ = drv.ast->mkEnumLst($1); SL($$, @$); }
    | enumerator_list "," enumerator                    { $$ = $1->append($3); }
    ;

enumerator
    : IDENTIFIER                                        { $$ = drv.ast->mkEnumer($1, nullptr); SL($$, @$); }
    | IDENTIFIER "=" const_expr                         { $$ = drv.ast->mkEnumer($1, $3); SL($$, @$); }
    ;


    /* Declarations */

declaration
    : decl_specs ";"                                    { $$ = drv.parseDeclaration($1, nullptr); SL($$, @$); }
    | decl_specs init_decltor_list ";"                  { $$ = drv.parseDeclaration($1, $2); SL($$, @$); }
    ;

init_decltor_list
    : init_decltor                                      { $$ = drv.ast->mkInitDeclaratorLst($1); SL($$, @$); }
    | init_decltor_list "," init_decltor                { $$ = $1->append($3); }
    ;

init_decltor
    : declarator                                        { $$ = drv.ast->mkInitDeclarator($1, nullptr); SL($$, @$); }
    | declarator "=" initializer                        { $$ = drv.ast->mkInitDeclarator($1, $3); SL($$, @$); }
    ;

declarator
    : pointer dir_decltor               { $$ = drv.ast->mkDeclarator($2, $1); SL($$, @$); }
    | dir_decltor                       { $$ = drv.ast->mkDeclarator($1, nullptr); SL($$, @$); }
    ;

pointer
    : "*"                               { $$ = drv.ast->mkPointer(nullptr, nullptr); SL($$, @$); }
    | "*" type_qual_lst                 { $$ = drv.ast->mkPointer($2, nullptr); SL($$, @$); }
    | "*" pointer                       { $$ = drv.ast->mkPointer(nullptr, $2); SL($$, @$); }
    | "*" type_qual_lst pointer         { $$ = drv.ast->mkPointer($2, $3); SL($$, @$); }
    ;

dir_decltor
    : IDENTIFIER                                        { $$ = drv.ast->mkDirDeclIdent($1); SL($$, @$); }
    | "(" declarator ")"                                { $$ = drv.ast->mkDirDeclNested($2); SL($$, @$); }
    | dir_decltor "[" type_qual_lst assign_expr "]"     { $$ = drv.ast->mkDirDeclArr($1, $3, $4, false); SL($$, @$); }
    | dir_decltor "[" STATIC type_qual_lst assign_expr "]"     { $$ = drv.ast->mkDirDeclArr($1, $4, $5, true); SL($$, @$); }
    | dir_decltor "[" type_qual_lst STATIC assign_expr "]"     { $$ = drv.ast->mkDirDeclArr($1, $3, $5, true); SL($$, @$); }
    | dir_decltor "[" type_qual_lst "]"                 { $$ = drv.ast->mkDirDeclArr($1, $3, nullptr, false); SL($$, @$); }
    | dir_decltor "[" assign_expr "]"                   { $$ = drv.ast->mkDirDeclArr($1, nullptr, $3, false); SL($$, @$); }
    | dir_decltor "[" STATIC assign_expr "]"                   { $$ = drv.ast->mkDirDeclArr($1, nullptr, $4, true); SL($$, @$); }
    | dir_decltor "[" "]"                               { $$ = drv.ast->mkDirDeclArr($1, nullptr, nullptr, false); SL($$, @$); }
    | dir_decltor "(" param_type_lst ")"                { $$ = drv.ast->mkDirDeclFunc($1, $3); SL($$, @$); }
    | dir_decltor "(" ")"                               { $$ = drv.ast->mkDirDeclFunc($1, nullptr); SL($$, @$); }
    ;

type_qual_lst
    : type_qual                     { $$ = drv.ast->mkTypeQuals(AST_TypeQuals::QualType($1)); SL($$, @$); }
    | type_qual_lst type_qual       { $$ = $1->update(AST_TypeQuals::QualType($2)); }
    ;

param_type_lst
    : param_lst                     { $$ = drv.ast->mkParamTypeLst($1, false); SL($$, @$); }
    | param_lst "," ELLIPSIS        { $$ = drv.ast->mkParamTypeLst($1, true); SL($$, @$); }
    ;

param_lst
    : parameter_declaration                     { $$ = drv.ast->mkParamLst($1); SL($$, @$); }
    | param_lst "," parameter_declaration       { $$ = $1->append($3); }
    ;

parameter_declaration
    : decl_specs declarator                     { $$ = drv.ast->mkParamDecl($1, $2); SL($$, @$); }
    | decl_specs                                { $$ = drv.ast->mkParamDecl($1, nullptr); SL($$, @$); }
    ;

type_name
    : spec_qual_lst                             { $$ = drv.ast->mkTypeName($1, nullptr); SL($$, @$); }
    | spec_qual_lst abstr_decltor               { $$ = drv.ast->mkTypeName($1, $2); SL($$, @$); }
    ;

abstr_decltor
    : dir_abstr_decltor                         { $$ = drv.ast->mkAbstrDeclarator($1, nullptr); SL($$, @$); }
    | pointer dir_abstr_decltor                 { $$ = drv.ast->mkAbstrDeclarator($2, $1); SL($$, @$); }
    | pointer                                   { $$ = drv.ast->mkAbstrDeclarator(nullptr, $1); SL($$, @$); }
    ;

dir_abstr_decltor
    : "(" abstr_decltor ")"                         { $$ = drv.ast->mkDirAbstrDeclNested($2); SL($$, @$); }
    | dir_abstr_decltor "[" "]"                     { $$ = drv.ast->mkDirAbstrDeclArr($1, nullptr); SL($$, @$); }
    | dir_abstr_decltor "[" assign_expr "]"         { $$ = drv.ast->mkDirAbstrDeclArr($1, $3); SL($$, @$); }
    | dir_abstr_decltor "(" ")"                     { $$ = drv.ast->mkDirAbstrDeclFunc($1, nullptr); SL($$, @$); }
    | dir_abstr_decltor "(" param_type_lst ")"      { $$ = drv.ast->mkDirAbstrDeclFunc($1, $3); SL($$, @$); }
    ;


    /* Initializers */

initializer
    : assign_expr                                   { $$ = drv.ast->mkInitializer($1); SL($$, @$); }
    | "{" init_lst "}"                              { $$ = drv.ast->mkInitializer($2); SL($$, @$); }
    | "{" init_lst "," "}"                          { $$ = drv.ast->mkInitializer($2); SL($$, @$); }
    ;

init_lst
    : initializer                                   { $$ = drv.ast->mkInitializerLst($1, nullptr); SL($$, @$); }
    | designator "=" initializer                    { $$ = drv.ast->mkInitializerLst($3, $1); SL($$, @$); }
    | init_lst "," initializer                      { $$ = $1->append($3, nullptr); }
    | init_lst "," designator "=" initializer       { $$ = $1->append($5, $3); }
    ;

designator
    : "[" const_expr "]"                            { $$ = drv.ast->mkDesignator($2); SL($$, @$); }
    | "." IDENTIFIER                                { $$ = drv.ast->mkDesignator($2); SL($$, @$); }
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
    : IDENTIFIER ":" stmt           { $$ = drv.ast->mkLabelStmt($1, $3, AST_LabeledStmt::SIMPL); SL($$, @$); }
    | CASE const_expr ":" stmt      { $$ = drv.ast->mkLabelStmt($2, $4, AST_LabeledStmt::SW_CASE); SL($$, @$); }
    | DEFAULT ":" stmt              { $$ = drv.ast->mkLabelStmt(nullptr, $3, AST_LabeledStmt::SW_DEFAULT); SL($$, @$); }
    ;

compound_stmt
    : "{" "}"                       { $$ = drv.ast->mkCompoundStmt(new AST_BlockItemList()); SL($$, @$); }
    | "{" block_items "}"           { $$ = drv.ast->mkCompoundStmt($2); SL($$, @$); }
    ;

block_items
    : declaration                   { $$ = drv.ast->mkBlockItemLst()->append($1); SL($$, @$); }
    | stmt                          { $$ = drv.ast->mkBlockItemLst()->append($1); SL($$, @$); }
    | block_items declaration       { $$ = $1->append($2); }
    | block_items stmt              { $$ = $1->append($2); }
    ;

expr_stmt
    : ";"                           { $$ = drv.ast->mkExprStmt(nullptr); SL($$, @$); }
    | expr ";"                      { $$ = drv.ast->mkExprStmt($1); SL($$, @$); }
    ;

select_stmt
    : IF "(" expr ")" stmt %prec IFX        { $$ = drv.ast->mkIfStmt($3, $5, nullptr); SL($$, @$); }
    | IF "(" expr ")" stmt ELSE stmt        { $$ = drv.ast->mkIfStmt($3, $5, $7); SL($$, @$); }
    | SWITCH "(" expr ")" stmt              { $$ = drv.ast->mkSwitchStmt($3, $5); SL($$, @$); }
    ;

loop_stmt
    : WHILE "(" expr ")" stmt                       { $$ = drv.ast->makeWhileStmt($5, $3, false); SL($$, @$); }
    | DO stmt WHILE "(" expr ")" ";"                { $$ = drv.ast->makeWhileStmt($2, $5, true); SL($$, @$); }
    | FOR "(" expr_stmt expr_stmt ")" stmt          { $$ = drv.ast->makeForStmt($6, $3, $4, nullptr); SL($$, @$); }
    | FOR "(" expr_stmt expr_stmt expr ")" stmt     { $$ = drv.ast->makeForStmt($7, $3, $4, $5); SL($$, @$); }
    | FOR "(" declaration expr_stmt ")" stmt        { $$ = drv.ast->makeForStmt($6, $3, $4, nullptr); SL($$, @$); }
    | FOR "(" declaration expr_stmt expr ")" stmt   { $$ = drv.ast->makeForStmt($7, $3, $4, $5); SL($$, @$); }
    ;

jmp_stmt
    : GOTO IDENTIFIER ";"       { $$ = drv.ast->mkJumpStmt(AST_JumpStmt::J_GOTO, $2); SL($$, @$); }
    | CONTINUE ";"              { $$ = drv.ast->mkJumpStmt(AST_JumpStmt::J_CONTINUE); SL($$, @$); }
    | BREAK ";"                 { $$ = drv.ast->mkJumpStmt(AST_JumpStmt::J_BREAK); SL($$, @$); }
    | RETURN ";"                { $$ = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET); SL($$, @$); }
    | RETURN expr ";"           { $$ = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET, $2); SL($$, @$); }
    ;


    /* Top-level elements */

trans_unit
    : func_def                      { $$ = drv.ast->mkTransUnit()->append($1); SL($$, @$); }
    | declaration                   { $$ = drv.ast->mkTransUnit()->append($1); SL($$, @$); }
    | trans_unit func_def           { $$ = $1->append($2); }
    | trans_unit declaration        { $$ = $1->append($2); }
    ;

func_def
    : decl_specs declarator compound_stmt       { $$ = drv.ast->mkFunDef($1, $2, $3); SL($$, @$); }
    ;

%%

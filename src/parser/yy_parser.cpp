// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "yy_parser.hpp"


// Unqualified %code blocks.
#line 24 "parser.y"

    #include "core_driver.hpp"
    #define yylex yycore_lex

    // TODO: fix this
    #define SL(node, loc) do { (node)->setLoc((loc)); } while (0)

#line 54 "yy_parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 146 "yy_parser.cpp"

  /// Build a parser object.
  parser::parser (yyscan_t scanner_yyarg, CoreDriver &drv_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false),
      scanner (scanner_yyarg),
      drv (drv_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.YY_MOVE_OR_COPY< AST_AbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.YY_MOVE_OR_COPY< AST_ArgumentsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block_items: // block_items
        value.YY_MOVE_OR_COPY< AST_BlockItemList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.YY_MOVE_OR_COPY< AST_CompoundStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.YY_MOVE_OR_COPY< AST_DeclSpecifiers * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.YY_MOVE_OR_COPY< AST_Declaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.YY_MOVE_OR_COPY< AST_Declarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_designator: // designator
        value.YY_MOVE_OR_COPY< AST_Designator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.YY_MOVE_OR_COPY< AST_DirAbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.YY_MOVE_OR_COPY< AST_DirDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.YY_MOVE_OR_COPY< AST_EnumSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.YY_MOVE_OR_COPY< AST_Enumerator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.YY_MOVE_OR_COPY< AST_EnumeratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_primary_expr: // primary_expr
      case symbol_kind::S_postfix_expr: // postfix_expr
      case symbol_kind::S_unary_expr: // unary_expr
      case symbol_kind::S_cast_expr: // cast_expr
      case symbol_kind::S_mult_expr: // mult_expr
      case symbol_kind::S_add_expr: // add_expr
      case symbol_kind::S_shift_expr: // shift_expr
      case symbol_kind::S_rel_expr: // rel_expr
      case symbol_kind::S_eq_expr: // eq_expr
      case symbol_kind::S_and_expr: // and_expr
      case symbol_kind::S_xor_expr: // xor_expr
      case symbol_kind::S_or_expr: // or_expr
      case symbol_kind::S_land_expr: // land_expr
      case symbol_kind::S_lor_expr: // lor_expr
      case symbol_kind::S_ternary_expr: // ternary_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_const_expr: // const_expr
        value.YY_MOVE_OR_COPY< AST_Expr * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.YY_MOVE_OR_COPY< AST_ExprStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.YY_MOVE_OR_COPY< AST_FunctionDef * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.YY_MOVE_OR_COPY< AST_InitDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.YY_MOVE_OR_COPY< AST_InitDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.YY_MOVE_OR_COPY< AST_Initializer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.YY_MOVE_OR_COPY< AST_InitializerList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.YY_MOVE_OR_COPY< AST_IterStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.YY_MOVE_OR_COPY< AST_JumpStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.YY_MOVE_OR_COPY< AST_LabeledStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.YY_MOVE_OR_COPY< AST_Literal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.YY_MOVE_OR_COPY< AST_ParameterDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.YY_MOVE_OR_COPY< AST_ParameterList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.YY_MOVE_OR_COPY< AST_ParameterTypeList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.YY_MOVE_OR_COPY< AST_Pointer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.YY_MOVE_OR_COPY< AST_SelectionStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.YY_MOVE_OR_COPY< AST_SpecsQualsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
        value.YY_MOVE_OR_COPY< AST_Stmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.YY_MOVE_OR_COPY< AST_StringsSeq * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.YY_MOVE_OR_COPY< AST_StructDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.YY_MOVE_OR_COPY< AST_StructDeclarationList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.YY_MOVE_OR_COPY< AST_StructDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.YY_MOVE_OR_COPY< AST_StructDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.YY_MOVE_OR_COPY< AST_TranslationUnit * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.YY_MOVE_OR_COPY< AST_TypeName * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.YY_MOVE_OR_COPY< AST_TypeQuals * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.YY_MOVE_OR_COPY< AST_TypeSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.YY_MOVE_OR_COPY< AST_UStructSpec * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.YY_MOVE_OR_COPY< short > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_general_ident: // general_ident
        value.YY_MOVE_OR_COPY< string_id_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.move< AST_AbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.move< AST_ArgumentsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block_items: // block_items
        value.move< AST_BlockItemList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.move< AST_CompoundStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.move< AST_DeclSpecifiers * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< AST_Declaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< AST_Declarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_designator: // designator
        value.move< AST_Designator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.move< AST_DirAbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.move< AST_DirDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< AST_EnumSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< AST_Enumerator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< AST_EnumeratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_primary_expr: // primary_expr
      case symbol_kind::S_postfix_expr: // postfix_expr
      case symbol_kind::S_unary_expr: // unary_expr
      case symbol_kind::S_cast_expr: // cast_expr
      case symbol_kind::S_mult_expr: // mult_expr
      case symbol_kind::S_add_expr: // add_expr
      case symbol_kind::S_shift_expr: // shift_expr
      case symbol_kind::S_rel_expr: // rel_expr
      case symbol_kind::S_eq_expr: // eq_expr
      case symbol_kind::S_and_expr: // and_expr
      case symbol_kind::S_xor_expr: // xor_expr
      case symbol_kind::S_or_expr: // or_expr
      case symbol_kind::S_land_expr: // land_expr
      case symbol_kind::S_lor_expr: // lor_expr
      case symbol_kind::S_ternary_expr: // ternary_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_const_expr: // const_expr
        value.move< AST_Expr * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.move< AST_ExprStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< AST_FunctionDef * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.move< AST_InitDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.move< AST_InitDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< AST_Initializer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.move< AST_InitializerList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< AST_IterStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.move< AST_JumpStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.move< AST_LabeledStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.move< AST_Literal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< AST_ParameterDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.move< AST_ParameterList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.move< AST_ParameterTypeList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< AST_Pointer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.move< AST_SelectionStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.move< AST_SpecsQualsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
        value.move< AST_Stmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.move< AST_StringsSeq * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< AST_StructDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.move< AST_StructDeclarationList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< AST_StructDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< AST_StructDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.move< AST_TranslationUnit * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< AST_TypeName * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.move< AST_TypeQuals * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< AST_TypeSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.move< AST_UStructSpec * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.move< short > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_general_ident: // general_ident
        value.move< string_id_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.copy< AST_AbstrDeclarator * > (that.value);
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.copy< AST_ArgumentsList * > (that.value);
        break;

      case symbol_kind::S_block_items: // block_items
        value.copy< AST_BlockItemList * > (that.value);
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.copy< AST_CompoundStmt * > (that.value);
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.copy< AST_DeclSpecifiers * > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.copy< AST_Declaration * > (that.value);
        break;

      case symbol_kind::S_declarator: // declarator
        value.copy< AST_Declarator * > (that.value);
        break;

      case symbol_kind::S_designator: // designator
        value.copy< AST_Designator * > (that.value);
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.copy< AST_DirAbstrDeclarator * > (that.value);
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.copy< AST_DirDeclarator * > (that.value);
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.copy< AST_EnumSpecifier * > (that.value);
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.copy< AST_Enumerator * > (that.value);
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.copy< AST_EnumeratorList * > (that.value);
        break;

      case symbol_kind::S_primary_expr: // primary_expr
      case symbol_kind::S_postfix_expr: // postfix_expr
      case symbol_kind::S_unary_expr: // unary_expr
      case symbol_kind::S_cast_expr: // cast_expr
      case symbol_kind::S_mult_expr: // mult_expr
      case symbol_kind::S_add_expr: // add_expr
      case symbol_kind::S_shift_expr: // shift_expr
      case symbol_kind::S_rel_expr: // rel_expr
      case symbol_kind::S_eq_expr: // eq_expr
      case symbol_kind::S_and_expr: // and_expr
      case symbol_kind::S_xor_expr: // xor_expr
      case symbol_kind::S_or_expr: // or_expr
      case symbol_kind::S_land_expr: // land_expr
      case symbol_kind::S_lor_expr: // lor_expr
      case symbol_kind::S_ternary_expr: // ternary_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_const_expr: // const_expr
        value.copy< AST_Expr * > (that.value);
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.copy< AST_ExprStmt * > (that.value);
        break;

      case symbol_kind::S_func_def: // func_def
        value.copy< AST_FunctionDef * > (that.value);
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.copy< AST_InitDeclarator * > (that.value);
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.copy< AST_InitDeclaratorList * > (that.value);
        break;

      case symbol_kind::S_initializer: // initializer
        value.copy< AST_Initializer * > (that.value);
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.copy< AST_InitializerList * > (that.value);
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.copy< AST_IterStmt * > (that.value);
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.copy< AST_JumpStmt * > (that.value);
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.copy< AST_LabeledStmt * > (that.value);
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.copy< AST_Literal > (that.value);
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.copy< AST_ParameterDeclaration * > (that.value);
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.copy< AST_ParameterList * > (that.value);
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.copy< AST_ParameterTypeList * > (that.value);
        break;

      case symbol_kind::S_pointer: // pointer
        value.copy< AST_Pointer * > (that.value);
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.copy< AST_SelectionStmt * > (that.value);
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.copy< AST_SpecsQualsList * > (that.value);
        break;

      case symbol_kind::S_stmt: // stmt
        value.copy< AST_Stmt * > (that.value);
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.copy< AST_StringsSeq * > (that.value);
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.copy< AST_StructDeclaration * > (that.value);
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.copy< AST_StructDeclarationList * > (that.value);
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.copy< AST_StructDeclarator * > (that.value);
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.copy< AST_StructDeclaratorList * > (that.value);
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.copy< AST_TranslationUnit * > (that.value);
        break;

      case symbol_kind::S_type_name: // type_name
        value.copy< AST_TypeName * > (that.value);
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.copy< AST_TypeQuals * > (that.value);
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.copy< AST_TypeSpecifier * > (that.value);
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.copy< AST_UStructSpec * > (that.value);
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.copy< short > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_general_ident: // general_ident
        value.copy< string_id_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.move< AST_AbstrDeclarator * > (that.value);
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.move< AST_ArgumentsList * > (that.value);
        break;

      case symbol_kind::S_block_items: // block_items
        value.move< AST_BlockItemList * > (that.value);
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.move< AST_CompoundStmt * > (that.value);
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.move< AST_DeclSpecifiers * > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< AST_Declaration * > (that.value);
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< AST_Declarator * > (that.value);
        break;

      case symbol_kind::S_designator: // designator
        value.move< AST_Designator * > (that.value);
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.move< AST_DirAbstrDeclarator * > (that.value);
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.move< AST_DirDeclarator * > (that.value);
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< AST_EnumSpecifier * > (that.value);
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< AST_Enumerator * > (that.value);
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< AST_EnumeratorList * > (that.value);
        break;

      case symbol_kind::S_primary_expr: // primary_expr
      case symbol_kind::S_postfix_expr: // postfix_expr
      case symbol_kind::S_unary_expr: // unary_expr
      case symbol_kind::S_cast_expr: // cast_expr
      case symbol_kind::S_mult_expr: // mult_expr
      case symbol_kind::S_add_expr: // add_expr
      case symbol_kind::S_shift_expr: // shift_expr
      case symbol_kind::S_rel_expr: // rel_expr
      case symbol_kind::S_eq_expr: // eq_expr
      case symbol_kind::S_and_expr: // and_expr
      case symbol_kind::S_xor_expr: // xor_expr
      case symbol_kind::S_or_expr: // or_expr
      case symbol_kind::S_land_expr: // land_expr
      case symbol_kind::S_lor_expr: // lor_expr
      case symbol_kind::S_ternary_expr: // ternary_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_const_expr: // const_expr
        value.move< AST_Expr * > (that.value);
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.move< AST_ExprStmt * > (that.value);
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< AST_FunctionDef * > (that.value);
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.move< AST_InitDeclarator * > (that.value);
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.move< AST_InitDeclaratorList * > (that.value);
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< AST_Initializer * > (that.value);
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.move< AST_InitializerList * > (that.value);
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< AST_IterStmt * > (that.value);
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.move< AST_JumpStmt * > (that.value);
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.move< AST_LabeledStmt * > (that.value);
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.move< AST_Literal > (that.value);
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< AST_ParameterDeclaration * > (that.value);
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.move< AST_ParameterList * > (that.value);
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.move< AST_ParameterTypeList * > (that.value);
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< AST_Pointer * > (that.value);
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.move< AST_SelectionStmt * > (that.value);
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.move< AST_SpecsQualsList * > (that.value);
        break;

      case symbol_kind::S_stmt: // stmt
        value.move< AST_Stmt * > (that.value);
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.move< AST_StringsSeq * > (that.value);
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< AST_StructDeclaration * > (that.value);
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.move< AST_StructDeclarationList * > (that.value);
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< AST_StructDeclarator * > (that.value);
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< AST_StructDeclaratorList * > (that.value);
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.move< AST_TranslationUnit * > (that.value);
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< AST_TypeName * > (that.value);
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.move< AST_TypeQuals * > (that.value);
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< AST_TypeSpecifier * > (that.value);
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.move< AST_UStructSpec * > (that.value);
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.move< short > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_general_ident: // general_ident
        value.move< string_id_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, drv));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        yylhs.value.emplace< AST_AbstrDeclarator * > ();
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        yylhs.value.emplace< AST_ArgumentsList * > ();
        break;

      case symbol_kind::S_block_items: // block_items
        yylhs.value.emplace< AST_BlockItemList * > ();
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        yylhs.value.emplace< AST_CompoundStmt * > ();
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        yylhs.value.emplace< AST_DeclSpecifiers * > ();
        break;

      case symbol_kind::S_declaration: // declaration
        yylhs.value.emplace< AST_Declaration * > ();
        break;

      case symbol_kind::S_declarator: // declarator
        yylhs.value.emplace< AST_Declarator * > ();
        break;

      case symbol_kind::S_designator: // designator
        yylhs.value.emplace< AST_Designator * > ();
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        yylhs.value.emplace< AST_DirAbstrDeclarator * > ();
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        yylhs.value.emplace< AST_DirDeclarator * > ();
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        yylhs.value.emplace< AST_EnumSpecifier * > ();
        break;

      case symbol_kind::S_enumerator: // enumerator
        yylhs.value.emplace< AST_Enumerator * > ();
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        yylhs.value.emplace< AST_EnumeratorList * > ();
        break;

      case symbol_kind::S_primary_expr: // primary_expr
      case symbol_kind::S_postfix_expr: // postfix_expr
      case symbol_kind::S_unary_expr: // unary_expr
      case symbol_kind::S_cast_expr: // cast_expr
      case symbol_kind::S_mult_expr: // mult_expr
      case symbol_kind::S_add_expr: // add_expr
      case symbol_kind::S_shift_expr: // shift_expr
      case symbol_kind::S_rel_expr: // rel_expr
      case symbol_kind::S_eq_expr: // eq_expr
      case symbol_kind::S_and_expr: // and_expr
      case symbol_kind::S_xor_expr: // xor_expr
      case symbol_kind::S_or_expr: // or_expr
      case symbol_kind::S_land_expr: // land_expr
      case symbol_kind::S_lor_expr: // lor_expr
      case symbol_kind::S_ternary_expr: // ternary_expr
      case symbol_kind::S_assign_expr: // assign_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_const_expr: // const_expr
        yylhs.value.emplace< AST_Expr * > ();
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        yylhs.value.emplace< AST_ExprStmt * > ();
        break;

      case symbol_kind::S_func_def: // func_def
        yylhs.value.emplace< AST_FunctionDef * > ();
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        yylhs.value.emplace< AST_InitDeclarator * > ();
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        yylhs.value.emplace< AST_InitDeclaratorList * > ();
        break;

      case symbol_kind::S_initializer: // initializer
        yylhs.value.emplace< AST_Initializer * > ();
        break;

      case symbol_kind::S_init_lst: // init_lst
        yylhs.value.emplace< AST_InitializerList * > ();
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        yylhs.value.emplace< AST_IterStmt * > ();
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        yylhs.value.emplace< AST_JumpStmt * > ();
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        yylhs.value.emplace< AST_LabeledStmt * > ();
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        yylhs.value.emplace< AST_Literal > ();
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        yylhs.value.emplace< AST_ParameterDeclaration * > ();
        break;

      case symbol_kind::S_param_lst: // param_lst
        yylhs.value.emplace< AST_ParameterList * > ();
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        yylhs.value.emplace< AST_ParameterTypeList * > ();
        break;

      case symbol_kind::S_pointer: // pointer
        yylhs.value.emplace< AST_Pointer * > ();
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        yylhs.value.emplace< AST_SelectionStmt * > ();
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        yylhs.value.emplace< AST_SpecsQualsList * > ();
        break;

      case symbol_kind::S_stmt: // stmt
        yylhs.value.emplace< AST_Stmt * > ();
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        yylhs.value.emplace< AST_StringsSeq * > ();
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        yylhs.value.emplace< AST_StructDeclaration * > ();
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        yylhs.value.emplace< AST_StructDeclarationList * > ();
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        yylhs.value.emplace< AST_StructDeclarator * > ();
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        yylhs.value.emplace< AST_StructDeclaratorList * > ();
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        yylhs.value.emplace< AST_TranslationUnit * > ();
        break;

      case symbol_kind::S_type_name: // type_name
        yylhs.value.emplace< AST_TypeName * > ();
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        yylhs.value.emplace< AST_TypeQuals * > ();
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        yylhs.value.emplace< AST_TypeSpecifier * > ();
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        yylhs.value.emplace< AST_UStructSpec * > ();
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        yylhs.value.emplace< short > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
      case symbol_kind::S_general_ident: // general_ident
        yylhs.value.emplace< string_id_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // primary_expr: IDENTIFIER
#line 156 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimIdent(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1537 "yy_parser.cpp"
    break;

  case 3: // primary_expr: CONSTANT
#line 157 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimConst(yystack_[0].value.as < AST_Literal > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1543 "yy_parser.cpp"
    break;

  case 4: // primary_expr: strings_seq
#line 158 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimStr(yystack_[0].value.as < AST_StringsSeq * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1549 "yy_parser.cpp"
    break;

  case 5: // primary_expr: "(" expr ")"
#line 159 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimExpr(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1555 "yy_parser.cpp"
    break;

  case 6: // primary_expr: "(" type_name ")" "{" init_lst "}"
#line 160 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimCompound(yystack_[4].value.as < AST_TypeName * > (), yystack_[1].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1561 "yy_parser.cpp"
    break;

  case 7: // primary_expr: "(" type_name ")" "{" init_lst "," "}"
#line 161 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimCompound(yystack_[5].value.as < AST_TypeName * > (), yystack_[2].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1567 "yy_parser.cpp"
    break;

  case 8: // strings_seq: STRING_LITERAL
#line 165 "parser.y"
                                                { yylhs.value.as < AST_StringsSeq * > () = drv.ast->mkStringsSeq()->append(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_StringsSeq * > (), yylhs.location); }
#line 1573 "yy_parser.cpp"
    break;

  case 9: // strings_seq: strings_seq STRING_LITERAL
#line 166 "parser.y"
                                                { yylhs.value.as < AST_StringsSeq * > () = yystack_[1].value.as < AST_StringsSeq * > ()->append(yystack_[0].value.as < string_id_t > ()); }
#line 1579 "yy_parser.cpp"
    break;

  case 10: // postfix_expr: primary_expr
#line 170 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1585 "yy_parser.cpp"
    break;

  case 11: // postfix_expr: postfix_expr "[" expr "]"
#line 171 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfArr(yystack_[3].value.as < AST_Expr * > (), yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1591 "yy_parser.cpp"
    break;

  case 12: // postfix_expr: postfix_expr "(" ")"
#line 172 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfCall(yystack_[2].value.as < AST_Expr * > (), nullptr); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1597 "yy_parser.cpp"
    break;

  case 13: // postfix_expr: postfix_expr "(" arg_expr_lst ")"
#line 173 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfCall(yystack_[3].value.as < AST_Expr * > (), yystack_[1].value.as < AST_ArgumentsList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1603 "yy_parser.cpp"
    break;

  case 14: // postfix_expr: postfix_expr "." IDENTIFIER
#line 174 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfAccesor(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < string_id_t > (), false); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1609 "yy_parser.cpp"
    break;

  case 15: // postfix_expr: postfix_expr PTR_OP IDENTIFIER
#line 175 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfAccesor(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < string_id_t > (), true); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1615 "yy_parser.cpp"
    break;

  case 16: // postfix_expr: postfix_expr INC_OP
#line 176 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfIncdec(yystack_[1].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1621 "yy_parser.cpp"
    break;

  case 17: // postfix_expr: postfix_expr DEC_OP
#line 177 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfIncdec(yystack_[1].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1627 "yy_parser.cpp"
    break;

  case 18: // arg_expr_lst: assign_expr
#line 181 "parser.y"
                                                { yylhs.value.as < AST_ArgumentsList * > () = drv.ast->mkArgsLst()->append(yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_ArgumentsList * > (), yylhs.location); }
#line 1633 "yy_parser.cpp"
    break;

  case 19: // arg_expr_lst: arg_expr_lst "," assign_expr
#line 182 "parser.y"
                                                { yylhs.value.as < AST_ArgumentsList * > () = yystack_[2].value.as < AST_ArgumentsList * > ()->append(yystack_[0].value.as < AST_Expr * > ()); }
#line 1639 "yy_parser.cpp"
    break;

  case 20: // unary_expr: postfix_expr
#line 186 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1645 "yy_parser.cpp"
    break;

  case 21: // unary_expr: INC_OP unary_expr
#line 187 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::PRE_INC, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1651 "yy_parser.cpp"
    break;

  case 22: // unary_expr: DEC_OP unary_expr
#line 188 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::PRE_DEC, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1657 "yy_parser.cpp"
    break;

  case 23: // unary_expr: unary_operator cast_expr
#line 189 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::OpType(yystack_[1].value.as < short > ()), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1663 "yy_parser.cpp"
    break;

  case 24: // unary_expr: SIZEOF unary_expr
#line 190 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1669 "yy_parser.cpp"
    break;

  case 25: // unary_expr: SIZEOF "(" type_name ")"
#line 191 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, yystack_[1].value.as < AST_TypeName * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1675 "yy_parser.cpp"
    break;

  case 26: // unary_operator: "&"
#line 195 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::ADDR_OF; }
#line 1681 "yy_parser.cpp"
    break;

  case 27: // unary_operator: "*"
#line 196 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::DEREF; }
#line 1687 "yy_parser.cpp"
    break;

  case 28: // unary_operator: "+"
#line 197 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_PLUS; }
#line 1693 "yy_parser.cpp"
    break;

  case 29: // unary_operator: "-"
#line 198 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_MINUS; }
#line 1699 "yy_parser.cpp"
    break;

  case 30: // unary_operator: "~"
#line 199 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_NEG; }
#line 1705 "yy_parser.cpp"
    break;

  case 31: // unary_operator: "!"
#line 200 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_NOT; }
#line 1711 "yy_parser.cpp"
    break;

  case 32: // cast_expr: unary_expr
#line 204 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1717 "yy_parser.cpp"
    break;

  case 33: // cast_expr: "(" type_name ")" cast_expr
#line 205 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkCastop(yystack_[2].value.as < AST_TypeName * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1723 "yy_parser.cpp"
    break;

  case 34: // mult_expr: cast_expr
#line 209 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1729 "yy_parser.cpp"
    break;

  case 35: // mult_expr: mult_expr "*" cast_expr
#line 210 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::MUL, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1735 "yy_parser.cpp"
    break;

  case 36: // mult_expr: mult_expr "/" cast_expr
#line 211 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::DIV, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1741 "yy_parser.cpp"
    break;

  case 37: // mult_expr: mult_expr "%" cast_expr
#line 212 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::REM, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1747 "yy_parser.cpp"
    break;

  case 38: // add_expr: mult_expr
#line 216 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1753 "yy_parser.cpp"
    break;

  case 39: // add_expr: add_expr "+" mult_expr
#line 217 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::ADD, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1759 "yy_parser.cpp"
    break;

  case 40: // add_expr: add_expr "-" mult_expr
#line 218 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SUB, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1765 "yy_parser.cpp"
    break;

  case 41: // shift_expr: add_expr
#line 222 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1771 "yy_parser.cpp"
    break;

  case 42: // shift_expr: shift_expr LEFT_OP add_expr
#line 223 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SHL, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1777 "yy_parser.cpp"
    break;

  case 43: // shift_expr: shift_expr RIGHT_OP add_expr
#line 224 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SHR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1783 "yy_parser.cpp"
    break;

  case 44: // rel_expr: shift_expr
#line 228 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1789 "yy_parser.cpp"
    break;

  case 45: // rel_expr: rel_expr "<" shift_expr
#line 229 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LT, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1795 "yy_parser.cpp"
    break;

  case 46: // rel_expr: rel_expr ">" shift_expr
#line 230 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::GT, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1801 "yy_parser.cpp"
    break;

  case 47: // rel_expr: rel_expr LE_OP shift_expr
#line 231 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1807 "yy_parser.cpp"
    break;

  case 48: // rel_expr: rel_expr GE_OP shift_expr
#line 232 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::GE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1813 "yy_parser.cpp"
    break;

  case 49: // eq_expr: rel_expr
#line 236 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1819 "yy_parser.cpp"
    break;

  case 50: // eq_expr: eq_expr EQ_OP rel_expr
#line 237 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::EQ, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1825 "yy_parser.cpp"
    break;

  case 51: // eq_expr: eq_expr NE_OP rel_expr
#line 238 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::NE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1831 "yy_parser.cpp"
    break;

  case 52: // and_expr: eq_expr
#line 242 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1837 "yy_parser.cpp"
    break;

  case 53: // and_expr: and_expr "&" eq_expr
#line 243 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_AND, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1843 "yy_parser.cpp"
    break;

  case 54: // xor_expr: and_expr
#line 247 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1849 "yy_parser.cpp"
    break;

  case 55: // xor_expr: xor_expr "^" and_expr
#line 248 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_XOR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1855 "yy_parser.cpp"
    break;

  case 56: // or_expr: xor_expr
#line 252 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1861 "yy_parser.cpp"
    break;

  case 57: // or_expr: or_expr "|" xor_expr
#line 253 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_OR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1867 "yy_parser.cpp"
    break;

  case 58: // land_expr: or_expr
#line 257 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1873 "yy_parser.cpp"
    break;

  case 59: // land_expr: land_expr AND_OP or_expr
#line 258 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LOG_AND, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1879 "yy_parser.cpp"
    break;

  case 60: // lor_expr: land_expr
#line 262 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1885 "yy_parser.cpp"
    break;

  case 61: // lor_expr: lor_expr OR_OP land_expr
#line 263 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LOG_OR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1891 "yy_parser.cpp"
    break;

  case 62: // ternary_expr: lor_expr
#line 267 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1897 "yy_parser.cpp"
    break;

  case 63: // ternary_expr: lor_expr "?" expr ":" ternary_expr
#line 268 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkTernary(yystack_[4].value.as < AST_Expr * > (), yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1903 "yy_parser.cpp"
    break;

  case 64: // assign_expr: ternary_expr
#line 272 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1909 "yy_parser.cpp"
    break;

  case 65: // assign_expr: unary_expr assign_op assign_expr
#line 273 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkAssign(AST_Assignment::OpType(yystack_[1].value.as < short > ()), yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1915 "yy_parser.cpp"
    break;

  case 66: // assign_op: "="
#line 277 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::DIRECT; }
#line 1921 "yy_parser.cpp"
    break;

  case 67: // assign_op: MUL_ASSIGN
#line 278 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::MUL; }
#line 1927 "yy_parser.cpp"
    break;

  case 68: // assign_op: DIV_ASSIGN
#line 279 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::DIV; }
#line 1933 "yy_parser.cpp"
    break;

  case 69: // assign_op: MOD_ASSIGN
#line 280 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::REM; }
#line 1939 "yy_parser.cpp"
    break;

  case 70: // assign_op: ADD_ASSIGN
#line 281 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::ADD; }
#line 1945 "yy_parser.cpp"
    break;

  case 71: // assign_op: SUB_ASSIGN
#line 282 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SUB; }
#line 1951 "yy_parser.cpp"
    break;

  case 72: // assign_op: LEFT_ASSIGN
#line 283 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SHL; }
#line 1957 "yy_parser.cpp"
    break;

  case 73: // assign_op: RIGHT_ASSIGN
#line 284 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SHR; }
#line 1963 "yy_parser.cpp"
    break;

  case 74: // assign_op: AND_ASSIGN
#line 285 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::AND; }
#line 1969 "yy_parser.cpp"
    break;

  case 75: // assign_op: XOR_ASSIGN
#line 286 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::XOR; }
#line 1975 "yy_parser.cpp"
    break;

  case 76: // assign_op: OR_ASSIGN
#line 287 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::OR; }
#line 1981 "yy_parser.cpp"
    break;

  case 77: // expr: assign_expr
#line 291 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1987 "yy_parser.cpp"
    break;

  case 78: // expr: expr "," assign_expr
#line 292 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkCommaExpr(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1993 "yy_parser.cpp"
    break;

  case 79: // const_expr: ternary_expr
#line 296 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1999 "yy_parser.cpp"
    break;

  case 80: // decl_specs: storage_class_specifier
#line 302 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_storage(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2005 "yy_parser.cpp"
    break;

  case 81: // decl_specs: storage_class_specifier decl_specs
#line 303 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_storage(yystack_[1].value.as < short > ()); }
#line 2011 "yy_parser.cpp"
    break;

  case 82: // decl_specs: type_specifier
#line 304 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_type_spec(yystack_[0].value.as < AST_TypeSpecifier * > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2017 "yy_parser.cpp"
    break;

  case 83: // decl_specs: type_specifier decl_specs
#line 305 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_type_spec(yystack_[1].value.as < AST_TypeSpecifier * > ()); }
#line 2023 "yy_parser.cpp"
    break;

  case 84: // decl_specs: type_qual
#line 306 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_type_qual(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2029 "yy_parser.cpp"
    break;

  case 85: // decl_specs: type_qual decl_specs
#line 307 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_type_qual(yystack_[1].value.as < short > ()); }
#line 2035 "yy_parser.cpp"
    break;

  case 86: // decl_specs: function_specifier
#line 308 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_func_qual(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2041 "yy_parser.cpp"
    break;

  case 87: // decl_specs: function_specifier decl_specs
#line 309 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_func_qual(yystack_[1].value.as < short > ()); }
#line 2047 "yy_parser.cpp"
    break;

  case 88: // storage_class_specifier: TYPEDEF
#line 313 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_TYPEDEF; }
#line 2053 "yy_parser.cpp"
    break;

  case 89: // storage_class_specifier: EXTERN
#line 314 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_EXTERN; }
#line 2059 "yy_parser.cpp"
    break;

  case 90: // storage_class_specifier: STATIC
#line 315 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_STATIC; }
#line 2065 "yy_parser.cpp"
    break;

  case 91: // storage_class_specifier: AUTO
#line 316 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_AUTO; }
#line 2071 "yy_parser.cpp"
    break;

  case 92: // storage_class_specifier: REGISTER
#line 317 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_REGISTER; }
#line 2077 "yy_parser.cpp"
    break;

  case 93: // storage_class_specifier: EXT_WEAK
#line 318 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_WEAK; }
#line 2083 "yy_parser.cpp"
    break;

  case 94: // type_specifier: VOID
#line 322 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_VOID); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2089 "yy_parser.cpp"
    break;

  case 95: // type_specifier: CHAR
#line 323 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_CHAR); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2095 "yy_parser.cpp"
    break;

  case 96: // type_specifier: SHORT
#line 324 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SHORT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2101 "yy_parser.cpp"
    break;

  case 97: // type_specifier: INT
#line 325 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_INT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2107 "yy_parser.cpp"
    break;

  case 98: // type_specifier: LONG
#line 326 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_LONG); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2113 "yy_parser.cpp"
    break;

  case 99: // type_specifier: FLOAT
#line 327 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_FLOAT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2119 "yy_parser.cpp"
    break;

  case 100: // type_specifier: DOUBLE
#line 328 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_DOUBLE); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2125 "yy_parser.cpp"
    break;

  case 101: // type_specifier: SIGNED
#line 329 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SIGNED); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2131 "yy_parser.cpp"
    break;

  case 102: // type_specifier: UNSIGNED
#line 330 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_UNSIGNED); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2137 "yy_parser.cpp"
    break;

  case 103: // type_specifier: ustruct_spec
#line 331 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(yystack_[0].value.as < AST_UStructSpec * > ()); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2143 "yy_parser.cpp"
    break;

  case 104: // type_specifier: enum_specifier
#line 332 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(yystack_[0].value.as < AST_EnumSpecifier * > ()); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2149 "yy_parser.cpp"
    break;

  case 105: // type_specifier: TYPE_NAME
#line 333 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.getDefinedType(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2155 "yy_parser.cpp"
    break;

  case 106: // type_qual: CONST
#line 337 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_CONST; }
#line 2161 "yy_parser.cpp"
    break;

  case 107: // type_qual: RESTRICT
#line 338 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_RESTRICT; }
#line 2167 "yy_parser.cpp"
    break;

  case 108: // type_qual: VOLATILE
#line 339 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_VOLATILE; }
#line 2173 "yy_parser.cpp"
    break;

  case 109: // function_specifier: INLINE
#line 343 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::Q_INLINE; }
#line 2179 "yy_parser.cpp"
    break;

  case 110: // function_specifier: EXT_PURE
#line 344 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::Q_PURE; }
#line 2185 "yy_parser.cpp"
    break;

  case 111: // function_specifier: EXT_FCONST
#line 345 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::Q_FCONST; }
#line 2191 "yy_parser.cpp"
    break;

  case 112: // ustruct_spec: ustruct general_ident "{" struct_decls_lst "}"
#line 349 "parser.y"
                                                       { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[4].value.as < short > (), yystack_[3].value.as < string_id_t > (), yystack_[1].value.as < AST_StructDeclarationList * > ()); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2197 "yy_parser.cpp"
    break;

  case 113: // ustruct_spec: ustruct "{" struct_decls_lst "}"
#line 350 "parser.y"
                                                       { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[3].value.as < short > (), NO_IDENT_ID, yystack_[1].value.as < AST_StructDeclarationList * > ()); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2203 "yy_parser.cpp"
    break;

  case 114: // ustruct_spec: ustruct general_ident
#line 351 "parser.y"
                                                       { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[1].value.as < short > (), yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2209 "yy_parser.cpp"
    break;

  case 115: // general_ident: IDENTIFIER
#line 355 "parser.y"
      { yylhs.value.as < string_id_t > () = yystack_[0].value.as < string_id_t > (); }
#line 2215 "yy_parser.cpp"
    break;

  case 116: // general_ident: TYPE_NAME
#line 356 "parser.y"
      { yylhs.value.as < string_id_t > () = yystack_[0].value.as < string_id_t > (); }
#line 2221 "yy_parser.cpp"
    break;

  case 117: // ustruct: STRUCT
#line 359 "parser.y"
                                                    { yylhs.value.as < short > () = 0; }
#line 2227 "yy_parser.cpp"
    break;

  case 118: // ustruct: UNION
#line 360 "parser.y"
                                                    { yylhs.value.as < short > () = 1; }
#line 2233 "yy_parser.cpp"
    break;

  case 119: // struct_decls_lst: struct_declaration
#line 364 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclarationList * > () = drv.ast->mkStructDeclarationLst(yystack_[0].value.as < AST_StructDeclaration * > ()); SL(yylhs.value.as < AST_StructDeclarationList * > (), yylhs.location); }
#line 2239 "yy_parser.cpp"
    break;

  case 120: // struct_decls_lst: struct_decls_lst struct_declaration
#line 365 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclarationList * > () = yystack_[1].value.as < AST_StructDeclarationList * > ()->append(yystack_[0].value.as < AST_StructDeclaration * > ()); }
#line 2245 "yy_parser.cpp"
    break;

  case 121: // struct_declaration: spec_qual_lst struct_declarator_list ";"
#line 369 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclaration * > () = drv.ast->mkStructDeclaration(yystack_[2].value.as < AST_SpecsQualsList * > (), yystack_[1].value.as < AST_StructDeclaratorList * > ()); SL(yylhs.value.as < AST_StructDeclaration * > (), yylhs.location); }
#line 2251 "yy_parser.cpp"
    break;

  case 122: // spec_qual_lst: type_specifier spec_qual_lst
#line 373 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = yystack_[0].value.as < AST_SpecsQualsList * > ()->append_spec(yystack_[1].value.as < AST_TypeSpecifier * > ()); }
#line 2257 "yy_parser.cpp"
    break;

  case 123: // spec_qual_lst: type_specifier
#line 374 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = drv.ast->mkSpecQualLst(yystack_[0].value.as < AST_TypeSpecifier * > ()); SL(yylhs.value.as < AST_SpecsQualsList * > (), yylhs.location); }
#line 2263 "yy_parser.cpp"
    break;

  case 124: // spec_qual_lst: type_qual spec_qual_lst
#line 375 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = yystack_[0].value.as < AST_SpecsQualsList * > ()->append_qual(AST_TypeQuals::QualType(yystack_[1].value.as < short > ())); }
#line 2269 "yy_parser.cpp"
    break;

  case 125: // spec_qual_lst: type_qual
#line 376 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = drv.ast->mkSpecQualLst(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); SL(yylhs.value.as < AST_SpecsQualsList * > (), yylhs.location); }
#line 2275 "yy_parser.cpp"
    break;

  case 126: // struct_declarator_list: struct_declarator
#line 380 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclaratorList * > () = drv.ast->mkStructDeclaratorLst(yystack_[0].value.as < AST_StructDeclarator * > ()); SL(yylhs.value.as < AST_StructDeclaratorList * > (), yylhs.location); }
#line 2281 "yy_parser.cpp"
    break;

  case 127: // struct_declarator_list: struct_declarator_list "," struct_declarator
#line 381 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclaratorList * > () = yystack_[2].value.as < AST_StructDeclaratorList * > ()->append(yystack_[0].value.as < AST_StructDeclarator * > ()); }
#line 2287 "yy_parser.cpp"
    break;

  case 128: // struct_declarator: declarator
#line 385 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(yystack_[0].value.as < AST_Declarator * > (), nullptr); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2293 "yy_parser.cpp"
    break;

  case 129: // struct_declarator: ":" const_expr
#line 386 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(nullptr, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2299 "yy_parser.cpp"
    break;

  case 130: // struct_declarator: declarator ":" const_expr
#line 387 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(yystack_[2].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2305 "yy_parser.cpp"
    break;

  case 131: // enum_specifier: ENUM "{" enumerator_list "}"
#line 391 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(NO_IDENT_ID, yystack_[1].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2311 "yy_parser.cpp"
    break;

  case 132: // enum_specifier: ENUM IDENTIFIER "{" enumerator_list "}"
#line 392 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[3].value.as < string_id_t > (), yystack_[1].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2317 "yy_parser.cpp"
    break;

  case 133: // enum_specifier: ENUM "{" enumerator_list "," "}"
#line 393 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(NO_IDENT_ID, yystack_[2].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2323 "yy_parser.cpp"
    break;

  case 134: // enum_specifier: ENUM IDENTIFIER "{" enumerator_list "," "}"
#line 394 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[4].value.as < string_id_t > (), yystack_[2].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2329 "yy_parser.cpp"
    break;

  case 135: // enum_specifier: ENUM IDENTIFIER
#line 395 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2335 "yy_parser.cpp"
    break;

  case 136: // enumerator_list: enumerator
#line 399 "parser.y"
                                                        { yylhs.value.as < AST_EnumeratorList * > () = drv.ast->mkEnumLst(yystack_[0].value.as < AST_Enumerator * > ()); SL(yylhs.value.as < AST_EnumeratorList * > (), yylhs.location); }
#line 2341 "yy_parser.cpp"
    break;

  case 137: // enumerator_list: enumerator_list "," enumerator
#line 400 "parser.y"
                                                        { yylhs.value.as < AST_EnumeratorList * > () = yystack_[2].value.as < AST_EnumeratorList * > ()->append(yystack_[0].value.as < AST_Enumerator * > ()); }
#line 2347 "yy_parser.cpp"
    break;

  case 138: // enumerator: IDENTIFIER
#line 404 "parser.y"
                                                        { yylhs.value.as < AST_Enumerator * > () = drv.ast->mkEnumer(yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_Enumerator * > (), yylhs.location); }
#line 2353 "yy_parser.cpp"
    break;

  case 139: // enumerator: IDENTIFIER "=" const_expr
#line 405 "parser.y"
                                                        { yylhs.value.as < AST_Enumerator * > () = drv.ast->mkEnumer(yystack_[2].value.as < string_id_t > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Enumerator * > (), yylhs.location); }
#line 2359 "yy_parser.cpp"
    break;

  case 140: // declaration: decl_specs ";"
#line 412 "parser.y"
                                                        { yylhs.value.as < AST_Declaration * > () = drv.parseDeclaration(yystack_[1].value.as < AST_DeclSpecifiers * > (), nullptr); SL(yylhs.value.as < AST_Declaration * > (), yylhs.location); }
#line 2365 "yy_parser.cpp"
    break;

  case 141: // declaration: decl_specs init_decltor_list ";"
#line 413 "parser.y"
                                                        { yylhs.value.as < AST_Declaration * > () = drv.parseDeclaration(yystack_[2].value.as < AST_DeclSpecifiers * > (), yystack_[1].value.as < AST_InitDeclaratorList * > ()); SL(yylhs.value.as < AST_Declaration * > (), yylhs.location); }
#line 2371 "yy_parser.cpp"
    break;

  case 142: // init_decltor_list: init_decltor
#line 417 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclaratorList * > () = drv.ast->mkInitDeclaratorLst(yystack_[0].value.as < AST_InitDeclarator * > ()); SL(yylhs.value.as < AST_InitDeclaratorList * > (), yylhs.location); }
#line 2377 "yy_parser.cpp"
    break;

  case 143: // init_decltor_list: init_decltor_list "," init_decltor
#line 418 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclaratorList * > () = yystack_[2].value.as < AST_InitDeclaratorList * > ()->append(yystack_[0].value.as < AST_InitDeclarator * > ()); }
#line 2383 "yy_parser.cpp"
    break;

  case 144: // init_decltor: declarator
#line 422 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclarator * > () = drv.ast->mkInitDeclarator(yystack_[0].value.as < AST_Declarator * > (), nullptr); SL(yylhs.value.as < AST_InitDeclarator * > (), yylhs.location); }
#line 2389 "yy_parser.cpp"
    break;

  case 145: // init_decltor: declarator "=" initializer
#line 423 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclarator * > () = drv.ast->mkInitDeclarator(yystack_[2].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_Initializer * > ()); SL(yylhs.value.as < AST_InitDeclarator * > (), yylhs.location); }
#line 2395 "yy_parser.cpp"
    break;

  case 146: // declarator: pointer dir_decltor
#line 427 "parser.y"
                                        { yylhs.value.as < AST_Declarator * > () = drv.ast->mkDeclarator(yystack_[0].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Declarator * > (), yylhs.location); }
#line 2401 "yy_parser.cpp"
    break;

  case 147: // declarator: dir_decltor
#line 428 "parser.y"
                                        { yylhs.value.as < AST_Declarator * > () = drv.ast->mkDeclarator(yystack_[0].value.as < AST_DirDeclarator * > (), nullptr); SL(yylhs.value.as < AST_Declarator * > (), yylhs.location); }
#line 2407 "yy_parser.cpp"
    break;

  case 148: // pointer: "*"
#line 432 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(nullptr, nullptr); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2413 "yy_parser.cpp"
    break;

  case 149: // pointer: "*" type_qual_lst
#line 433 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(yystack_[0].value.as < AST_TypeQuals * > (), nullptr); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2419 "yy_parser.cpp"
    break;

  case 150: // pointer: "*" pointer
#line 434 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(nullptr, yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2425 "yy_parser.cpp"
    break;

  case 151: // pointer: "*" type_qual_lst pointer
#line 435 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(yystack_[1].value.as < AST_TypeQuals * > (), yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2431 "yy_parser.cpp"
    break;

  case 152: // dir_decltor: IDENTIFIER
#line 439 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclIdent(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2437 "yy_parser.cpp"
    break;

  case 153: // dir_decltor: "(" declarator ")"
#line 440 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclNested(yystack_[1].value.as < AST_Declarator * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2443 "yy_parser.cpp"
    break;

  case 154: // dir_decltor: dir_decltor "[" type_qual_lst assign_expr "]"
#line 441 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[4].value.as < AST_DirDeclarator * > (), yystack_[2].value.as < AST_TypeQuals * > (), yystack_[1].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2449 "yy_parser.cpp"
    break;

  case 155: // dir_decltor: dir_decltor "[" STATIC type_qual_lst assign_expr "]"
#line 442 "parser.y"
                                                               { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[5].value.as < AST_DirDeclarator * > (), yystack_[2].value.as < AST_TypeQuals * > (), yystack_[1].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2455 "yy_parser.cpp"
    break;

  case 156: // dir_decltor: dir_decltor "[" type_qual_lst STATIC assign_expr "]"
#line 443 "parser.y"
                                                               { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[5].value.as < AST_DirDeclarator * > (), yystack_[3].value.as < AST_TypeQuals * > (), yystack_[1].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2461 "yy_parser.cpp"
    break;

  case 157: // dir_decltor: dir_decltor "[" type_qual_lst "]"
#line 444 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[3].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_TypeQuals * > (), nullptr, false); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2467 "yy_parser.cpp"
    break;

  case 158: // dir_decltor: dir_decltor "[" assign_expr "]"
#line 445 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[3].value.as < AST_DirDeclarator * > (), nullptr, yystack_[1].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2473 "yy_parser.cpp"
    break;

  case 159: // dir_decltor: dir_decltor "[" STATIC assign_expr "]"
#line 446 "parser.y"
                                                               { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[4].value.as < AST_DirDeclarator * > (), nullptr, yystack_[1].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2479 "yy_parser.cpp"
    break;

  case 160: // dir_decltor: dir_decltor "[" "]"
#line 447 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[2].value.as < AST_DirDeclarator * > (), nullptr, nullptr, false); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2485 "yy_parser.cpp"
    break;

  case 161: // dir_decltor: dir_decltor "(" param_type_lst ")"
#line 448 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclFunc(yystack_[3].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_ParameterTypeList * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2491 "yy_parser.cpp"
    break;

  case 162: // dir_decltor: dir_decltor "(" ")"
#line 449 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclFunc(yystack_[2].value.as < AST_DirDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2497 "yy_parser.cpp"
    break;

  case 163: // type_qual_lst: type_qual
#line 453 "parser.y"
                                    { yylhs.value.as < AST_TypeQuals * > () = drv.ast->mkTypeQuals(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); SL(yylhs.value.as < AST_TypeQuals * > (), yylhs.location); }
#line 2503 "yy_parser.cpp"
    break;

  case 164: // type_qual_lst: type_qual_lst type_qual
#line 454 "parser.y"
                                    { yylhs.value.as < AST_TypeQuals * > () = yystack_[1].value.as < AST_TypeQuals * > ()->update(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); }
#line 2509 "yy_parser.cpp"
    break;

  case 165: // param_type_lst: param_lst
#line 458 "parser.y"
                                    { yylhs.value.as < AST_ParameterTypeList * > () = drv.ast->mkParamTypeLst(yystack_[0].value.as < AST_ParameterList * > (), false); SL(yylhs.value.as < AST_ParameterTypeList * > (), yylhs.location); }
#line 2515 "yy_parser.cpp"
    break;

  case 166: // param_type_lst: param_lst "," ELLIPSIS
#line 459 "parser.y"
                                    { yylhs.value.as < AST_ParameterTypeList * > () = drv.ast->mkParamTypeLst(yystack_[2].value.as < AST_ParameterList * > (), true); SL(yylhs.value.as < AST_ParameterTypeList * > (), yylhs.location); }
#line 2521 "yy_parser.cpp"
    break;

  case 167: // param_lst: parameter_declaration
#line 463 "parser.y"
                                                { yylhs.value.as < AST_ParameterList * > () = drv.ast->mkParamLst(yystack_[0].value.as < AST_ParameterDeclaration * > ()); SL(yylhs.value.as < AST_ParameterList * > (), yylhs.location); }
#line 2527 "yy_parser.cpp"
    break;

  case 168: // param_lst: param_lst "," parameter_declaration
#line 464 "parser.y"
                                                { yylhs.value.as < AST_ParameterList * > () = yystack_[2].value.as < AST_ParameterList * > ()->append(yystack_[0].value.as < AST_ParameterDeclaration * > ()); }
#line 2533 "yy_parser.cpp"
    break;

  case 169: // parameter_declaration: decl_specs declarator
#line 468 "parser.y"
                                                { yylhs.value.as < AST_ParameterDeclaration * > () = drv.ast->mkParamDecl(yystack_[1].value.as < AST_DeclSpecifiers * > (), yystack_[0].value.as < AST_Declarator * > ()); SL(yylhs.value.as < AST_ParameterDeclaration * > (), yylhs.location); }
#line 2539 "yy_parser.cpp"
    break;

  case 170: // parameter_declaration: decl_specs
#line 469 "parser.y"
                                                { yylhs.value.as < AST_ParameterDeclaration * > () = drv.ast->mkParamDecl(yystack_[0].value.as < AST_DeclSpecifiers * > (), nullptr); SL(yylhs.value.as < AST_ParameterDeclaration * > (), yylhs.location); }
#line 2545 "yy_parser.cpp"
    break;

  case 171: // type_name: spec_qual_lst
#line 473 "parser.y"
                                                { yylhs.value.as < AST_TypeName * > () = drv.ast->mkTypeName(yystack_[0].value.as < AST_SpecsQualsList * > (), nullptr); SL(yylhs.value.as < AST_TypeName * > (), yylhs.location); }
#line 2551 "yy_parser.cpp"
    break;

  case 172: // type_name: spec_qual_lst abstr_decltor
#line 474 "parser.y"
                                                { yylhs.value.as < AST_TypeName * > () = drv.ast->mkTypeName(yystack_[1].value.as < AST_SpecsQualsList * > (), yystack_[0].value.as < AST_AbstrDeclarator * > ()); SL(yylhs.value.as < AST_TypeName * > (), yylhs.location); }
#line 2557 "yy_parser.cpp"
    break;

  case 173: // abstr_decltor: dir_abstr_decltor
#line 478 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(yystack_[0].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2563 "yy_parser.cpp"
    break;

  case 174: // abstr_decltor: pointer dir_abstr_decltor
#line 479 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(yystack_[0].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2569 "yy_parser.cpp"
    break;

  case 175: // abstr_decltor: pointer
#line 480 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(nullptr, yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2575 "yy_parser.cpp"
    break;

  case 176: // dir_abstr_decltor: "(" abstr_decltor ")"
#line 484 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclNested(yystack_[1].value.as < AST_AbstrDeclarator * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2581 "yy_parser.cpp"
    break;

  case 177: // dir_abstr_decltor: dir_abstr_decltor "[" "]"
#line 485 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclArr(yystack_[2].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2587 "yy_parser.cpp"
    break;

  case 178: // dir_abstr_decltor: dir_abstr_decltor "[" assign_expr "]"
#line 486 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclArr(yystack_[3].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2593 "yy_parser.cpp"
    break;

  case 179: // dir_abstr_decltor: dir_abstr_decltor "(" ")"
#line 487 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclFunc(yystack_[2].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2599 "yy_parser.cpp"
    break;

  case 180: // dir_abstr_decltor: dir_abstr_decltor "(" param_type_lst ")"
#line 488 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclFunc(yystack_[3].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_ParameterTypeList * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2605 "yy_parser.cpp"
    break;

  case 181: // initializer: assign_expr
#line 495 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2611 "yy_parser.cpp"
    break;

  case 182: // initializer: "{" init_lst "}"
#line 496 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[1].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2617 "yy_parser.cpp"
    break;

  case 183: // initializer: "{" init_lst "," "}"
#line 497 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[2].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2623 "yy_parser.cpp"
    break;

  case 184: // init_lst: initializer
#line 501 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = drv.ast->mkInitializerLst(yystack_[0].value.as < AST_Initializer * > (), nullptr); SL(yylhs.value.as < AST_InitializerList * > (), yylhs.location); }
#line 2629 "yy_parser.cpp"
    break;

  case 185: // init_lst: designator "=" initializer
#line 502 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = drv.ast->mkInitializerLst(yystack_[0].value.as < AST_Initializer * > (), yystack_[2].value.as < AST_Designator * > ()); SL(yylhs.value.as < AST_InitializerList * > (), yylhs.location); }
#line 2635 "yy_parser.cpp"
    break;

  case 186: // init_lst: init_lst "," initializer
#line 503 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = yystack_[2].value.as < AST_InitializerList * > ()->append(yystack_[0].value.as < AST_Initializer * > (), nullptr); }
#line 2641 "yy_parser.cpp"
    break;

  case 187: // init_lst: init_lst "," designator "=" initializer
#line 504 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = yystack_[4].value.as < AST_InitializerList * > ()->append(yystack_[0].value.as < AST_Initializer * > (), yystack_[2].value.as < AST_Designator * > ()); }
#line 2647 "yy_parser.cpp"
    break;

  case 188: // designator: "[" const_expr "]"
#line 508 "parser.y"
                                                    { yylhs.value.as < AST_Designator * > () = drv.ast->mkDesignator(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Designator * > (), yylhs.location); }
#line 2653 "yy_parser.cpp"
    break;

  case 189: // designator: "." IDENTIFIER
#line 509 "parser.y"
                                                    { yylhs.value.as < AST_Designator * > () = drv.ast->mkDesignator(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_Designator * > (), yylhs.location); }
#line 2659 "yy_parser.cpp"
    break;

  case 190: // stmt: label_stmt
#line 516 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_LabeledStmt * > (); }
#line 2665 "yy_parser.cpp"
    break;

  case 191: // stmt: compound_stmt
#line 517 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_CompoundStmt * > (); }
#line 2671 "yy_parser.cpp"
    break;

  case 192: // stmt: expr_stmt
#line 518 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_ExprStmt * > (); }
#line 2677 "yy_parser.cpp"
    break;

  case 193: // stmt: select_stmt
#line 519 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_SelectionStmt * > (); }
#line 2683 "yy_parser.cpp"
    break;

  case 194: // stmt: loop_stmt
#line 520 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_IterStmt * > (); }
#line 2689 "yy_parser.cpp"
    break;

  case 195: // stmt: jmp_stmt
#line 521 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_JumpStmt * > (); }
#line 2695 "yy_parser.cpp"
    break;

  case 196: // label_stmt: IDENTIFIER ":" stmt
#line 525 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(yystack_[2].value.as < string_id_t > (), yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SIMPL); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2701 "yy_parser.cpp"
    break;

  case 197: // label_stmt: CASE const_expr ":" stmt
#line 526 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SW_CASE); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2707 "yy_parser.cpp"
    break;

  case 198: // label_stmt: DEFAULT ":" stmt
#line 527 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(nullptr, yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SW_DEFAULT); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2713 "yy_parser.cpp"
    break;

  case 199: // compound_stmt: "{" "}"
#line 531 "parser.y"
                                    { yylhs.value.as < AST_CompoundStmt * > () = drv.ast->mkCompoundStmt(new AST_BlockItemList()); SL(yylhs.value.as < AST_CompoundStmt * > (), yylhs.location); }
#line 2719 "yy_parser.cpp"
    break;

  case 200: // compound_stmt: "{" block_items "}"
#line 532 "parser.y"
                                    { yylhs.value.as < AST_CompoundStmt * > () = drv.ast->mkCompoundStmt(yystack_[1].value.as < AST_BlockItemList * > ()); SL(yylhs.value.as < AST_CompoundStmt * > (), yylhs.location); }
#line 2725 "yy_parser.cpp"
    break;

  case 201: // block_items: declaration
#line 536 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = drv.ast->mkBlockItemLst()->append(yystack_[0].value.as < AST_Declaration * > ()); SL(yylhs.value.as < AST_BlockItemList * > (), yylhs.location); }
#line 2731 "yy_parser.cpp"
    break;

  case 202: // block_items: stmt
#line 537 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = drv.ast->mkBlockItemLst()->append(yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_BlockItemList * > (), yylhs.location); }
#line 2737 "yy_parser.cpp"
    break;

  case 203: // block_items: block_items declaration
#line 538 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = yystack_[1].value.as < AST_BlockItemList * > ()->append(yystack_[0].value.as < AST_Declaration * > ()); }
#line 2743 "yy_parser.cpp"
    break;

  case 204: // block_items: block_items stmt
#line 539 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = yystack_[1].value.as < AST_BlockItemList * > ()->append(yystack_[0].value.as < AST_Stmt * > ()); }
#line 2749 "yy_parser.cpp"
    break;

  case 205: // expr_stmt: ";"
#line 543 "parser.y"
                                    { yylhs.value.as < AST_ExprStmt * > () = drv.ast->mkExprStmt(nullptr); SL(yylhs.value.as < AST_ExprStmt * > (), yylhs.location); }
#line 2755 "yy_parser.cpp"
    break;

  case 206: // expr_stmt: expr ";"
#line 544 "parser.y"
                                    { yylhs.value.as < AST_ExprStmt * > () = drv.ast->mkExprStmt(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_ExprStmt * > (), yylhs.location); }
#line 2761 "yy_parser.cpp"
    break;

  case 207: // select_stmt: IF "(" expr ")" stmt
#line 548 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkIfStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > (), nullptr); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2767 "yy_parser.cpp"
    break;

  case 208: // select_stmt: IF "(" expr ")" stmt ELSE stmt
#line 549 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkIfStmt(yystack_[4].value.as < AST_Expr * > (), yystack_[2].value.as < AST_Stmt * > (), yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2773 "yy_parser.cpp"
    break;

  case 209: // select_stmt: SWITCH "(" expr ")" stmt
#line 550 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkSwitchStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2779 "yy_parser.cpp"
    break;

  case 210: // loop_stmt: WHILE "(" expr ")" stmt
#line 554 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeWhileStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[2].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2785 "yy_parser.cpp"
    break;

  case 211: // loop_stmt: DO stmt WHILE "(" expr ")" ";"
#line 555 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeWhileStmt(yystack_[5].value.as < AST_Stmt * > (), yystack_[2].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2791 "yy_parser.cpp"
    break;

  case 212: // loop_stmt: FOR "(" expr_stmt expr_stmt ")" stmt
#line 556 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_ExprStmt * > (), nullptr); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2797 "yy_parser.cpp"
    break;

  case 213: // loop_stmt: FOR "(" expr_stmt expr_stmt expr ")" stmt
#line 557 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[4].value.as < AST_ExprStmt * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2803 "yy_parser.cpp"
    break;

  case 214: // loop_stmt: FOR "(" declaration expr_stmt ")" stmt
#line 558 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[3].value.as < AST_Declaration * > (), yystack_[2].value.as < AST_ExprStmt * > (), nullptr); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2809 "yy_parser.cpp"
    break;

  case 215: // loop_stmt: FOR "(" declaration expr_stmt expr ")" stmt
#line 559 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[4].value.as < AST_Declaration * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2815 "yy_parser.cpp"
    break;

  case 216: // jmp_stmt: GOTO IDENTIFIER ";"
#line 563 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_GOTO, yystack_[1].value.as < string_id_t > ()); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2821 "yy_parser.cpp"
    break;

  case 217: // jmp_stmt: CONTINUE ";"
#line 564 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_CONTINUE); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2827 "yy_parser.cpp"
    break;

  case 218: // jmp_stmt: BREAK ";"
#line 565 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_BREAK); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2833 "yy_parser.cpp"
    break;

  case 219: // jmp_stmt: RETURN ";"
#line 566 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2839 "yy_parser.cpp"
    break;

  case 220: // jmp_stmt: RETURN expr ";"
#line 567 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET, yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2845 "yy_parser.cpp"
    break;

  case 221: // trans_unit: func_def
#line 574 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = drv.ast->mkTransUnit()->append(yystack_[0].value.as < AST_FunctionDef * > ()); SL(yylhs.value.as < AST_TranslationUnit * > (), yylhs.location); }
#line 2851 "yy_parser.cpp"
    break;

  case 222: // trans_unit: declaration
#line 575 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = drv.ast->mkTransUnit()->append(yystack_[0].value.as < AST_Declaration * > ()); SL(yylhs.value.as < AST_TranslationUnit * > (), yylhs.location); }
#line 2857 "yy_parser.cpp"
    break;

  case 223: // trans_unit: trans_unit func_def
#line 576 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = yystack_[1].value.as < AST_TranslationUnit * > ()->append(yystack_[0].value.as < AST_FunctionDef * > ()); }
#line 2863 "yy_parser.cpp"
    break;

  case 224: // trans_unit: trans_unit declaration
#line 577 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = yystack_[1].value.as < AST_TranslationUnit * > ()->append(yystack_[0].value.as < AST_Declaration * > ()); }
#line 2869 "yy_parser.cpp"
    break;

  case 225: // func_def: decl_specs declarator compound_stmt
#line 581 "parser.y"
                                                { yylhs.value.as < AST_FunctionDef * > () = drv.ast->mkFunDef(yystack_[2].value.as < AST_DeclSpecifiers * > (), yystack_[1].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_CompoundStmt * > ()); SL(yylhs.value.as < AST_FunctionDef * > (), yylhs.location); }
#line 2875 "yy_parser.cpp"
    break;


#line 2879 "yy_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      yy_lac_discard_ ("error recovery");
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", ";", "{", "}", ",", ":", "=",
  "(", ")", "[", "]", ".", "&", "!", "~", "-", "+", "*", "/", "%", "<",
  ">", "^", "|", "?", "PTR_OP", "INC_OP", "DEC_OP", "LEFT_OP", "RIGHT_OP",
  "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP", "MUL_ASSIGN",
  "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN", "SUB_ASSIGN", "LEFT_ASSIGN",
  "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN", "SIZEOF",
  "TYPEDEF", "EXTERN", "STATIC", "AUTO", "REGISTER", "INLINE", "RESTRICT",
  "EXT_PURE", "EXT_FCONST", "EXT_WEAK", "CHAR", "SHORT", "INT", "LONG",
  "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "CONST", "VOLATILE", "VOID",
  "STRUCT", "UNION", "ENUM", "ELLIPSIS", "CASE", "DEFAULT", "IF", "SWITCH",
  "WHILE", "DO", "FOR", "GOTO", "CONTINUE", "BREAK", "RETURN", "IFX",
  "ELSE", "IDENTIFIER", "TYPE_NAME", "CONSTANT", "STRING_LITERAL",
  "$accept", "primary_expr", "strings_seq", "postfix_expr", "arg_expr_lst",
  "unary_expr", "unary_operator", "cast_expr", "mult_expr", "add_expr",
  "shift_expr", "rel_expr", "eq_expr", "and_expr", "xor_expr", "or_expr",
  "land_expr", "lor_expr", "ternary_expr", "assign_expr", "assign_op",
  "expr", "const_expr", "decl_specs", "storage_class_specifier",
  "type_specifier", "type_qual", "function_specifier", "ustruct_spec",
  "general_ident", "ustruct", "struct_decls_lst", "struct_declaration",
  "spec_qual_lst", "struct_declarator_list", "struct_declarator",
  "enum_specifier", "enumerator_list", "enumerator", "declaration",
  "init_decltor_list", "init_decltor", "declarator", "pointer",
  "dir_decltor", "type_qual_lst", "param_type_lst", "param_lst",
  "parameter_declaration", "type_name", "abstr_decltor",
  "dir_abstr_decltor", "initializer", "init_lst", "designator", "stmt",
  "label_stmt", "compound_stmt", "block_items", "expr_stmt", "select_stmt",
  "loop_stmt", "jmp_stmt", "trans_unit", "func_def", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

#if YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  bool
  parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
  }


  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
         In the first two cases, it might appear that the current syntax
         error should have been detected in the previous state when
         yy_lac_check was invoked.  However, at that time, there might
         have been a different syntax error that discarded a different
         initial context during error recovery, leaving behind the
         current lookahead.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short parser::yypact_ninf_ = -287;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
    1323,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,    15,  -287,    30,  1323,  1323,  1323,
    1323,  -287,    10,  -287,  -287,   272,  -287,   -62,    76,  -287,
      28,    17,  -287,    57,  -287,    71,    42,   154,  -287,  -287,
    -287,  -287,  1353,  -287,  -287,   108,  -287,  -287,  -287,    37,
      47,  -287,   -62,    51,  -287,  -287,    17,  -287,    28,   347,
     762,  -287,   154,  1180,   886,  1353,  1353,   214,  -287,    58,
    1353,  1090,  -287,    19,   135,  -287,  -287,  -287,  -287,   156,
    -287,  -287,   861,  -287,  -287,  -287,  -287,  -287,  -287,  1117,
    1117,  1138,  1090,   210,   212,   217,   229,   594,   235,   161,
     252,   268,   611,   285,  -287,  -287,  -287,   204,   234,   130,
    1090,  -287,   113,   140,   -13,    59,   170,   282,   275,   276,
     264,    -5,  -287,  -287,    84,    30,  -287,  -287,  -287,  -287,
     435,  -287,  -287,  -287,  -287,   741,  -287,  -287,  -287,  -287,
      28,   293,   298,  -287,  -287,   968,   294,   943,  -287,  -287,
    -287,  -287,  1090,   149,  -287,   300,   795,  -287,  -287,  -287,
    -287,  -287,  -287,    41,   115,    35,   299,   861,  -287,  -287,
     861,  -287,   301,   594,  1090,  1090,  1090,   232,   517,   308,
    -287,  -287,  -287,   175,   594,  -287,   990,  1090,   225,   226,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  1090,  -287,  1090,  1090,  1090,  1090,  1090,
    1090,  1090,  1090,  1090,  1090,  1090,  1090,  1090,  1090,  1090,
    1090,  1090,  1090,  1090,  -287,  1090,  -287,  -287,  -287,  1090,
     227,  -287,   208,   307,  -287,  -287,  1283,   304,   968,  -287,
    -287,  1090,   305,  -287,  -287,    58,  1090,  -287,  -287,  -287,
      35,   309,  -287,   213,   798,   335,   336,   594,  -287,   136,
     143,   144,   310,   632,   632,  -287,  -287,  -287,  -287,   197,
    -287,   124,  -287,  -287,  -287,  -287,  -287,  -287,   113,   113,
     140,   140,   -13,   -13,   -13,   -13,    59,    59,   170,   282,
     275,   276,   251,   264,  -287,   337,  -287,  -287,   698,   762,
    -287,  -287,  -287,   341,   342,  -287,  -287,  -287,   338,   213,
    1243,  1025,   741,  -287,   343,   343,  -287,   594,   594,   594,
    1090,  1053,  1074,  1090,  -287,  -287,  1090,  -287,  -287,  -287,
     349,  -287,  -287,  -287,  -287,  -287,   345,  -287,   346,   254,
     273,  -287,  -287,   200,   594,   205,   594,   206,  -287,  -287,
     762,  -287,  -287,  -287,   719,   594,   364,  -287,   594,  -287,
     594,  -287,  -287,  -287,  -287,  -287,  -287
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,    88,    89,    90,    91,    92,   109,   107,   110,   111,
      93,    95,    96,    97,    98,   101,   102,    99,   100,   106,
     108,    94,   117,   118,     0,   105,     0,    80,    82,    84,
      86,   103,     0,   104,   222,     0,   221,     0,   135,   140,
       0,   148,   152,     0,   142,   144,     0,   147,    81,    83,
      85,    87,     0,   115,   116,   114,     1,   224,   223,   138,
       0,   136,     0,     0,   163,   150,   149,   141,     0,     0,
       0,   225,   146,     0,     0,   123,   125,     0,   119,     0,
       0,     0,   131,     0,     0,   153,   164,   151,   143,   144,
     205,   199,     0,    26,    31,    30,    29,    28,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     3,     8,    10,     4,    20,    32,
       0,    34,    38,    41,    44,    49,    52,    54,    56,    58,
      60,    62,    64,    77,     0,     0,   201,   202,   190,   191,
       0,   192,   193,   194,   195,     0,     2,   181,   145,   162,
     170,     0,   165,   167,   160,     0,     0,     0,   122,   124,
     113,   120,     0,     0,   126,   128,     0,    32,    79,   139,
     133,   137,   132,     0,     0,   171,     0,     0,    21,    22,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
     217,   218,   219,     0,     0,     9,     0,     0,     0,     0,
      16,    17,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   206,     0,   200,   203,   204,     0,
       0,   184,     0,     0,   169,   161,     0,     0,     0,   158,
     157,     0,     0,   129,   121,     0,     0,   112,   134,     5,
       0,   175,   172,   173,     0,     0,     0,     0,   198,     0,
       0,     0,     0,     0,     0,   216,   220,   196,    12,     0,
      18,     0,    14,    15,    65,    35,    36,    37,    40,    39,
      42,    43,    45,    46,    47,    48,    50,    51,    53,    55,
      57,    59,     0,    61,    78,     0,   189,   182,     0,     0,
     166,   168,   159,     0,     0,   154,   127,   130,     0,   174,
       0,     0,     0,    33,     0,    25,   197,     0,     0,     0,
       0,     0,     0,     0,    13,    11,     0,   188,   183,   186,
       0,   185,   155,   156,   176,   179,     0,   177,     0,     0,
     207,   209,   210,     0,     0,     0,     0,     0,    19,    63,
       0,   180,   178,     6,     0,     0,     0,   214,     0,   212,
       0,   187,     7,   208,   211,   215,   213
  };

  const short
  parser::yypgoto_[] =
  {
    -287,  -287,  -287,  -287,  -287,   -31,  -287,  -108,    48,    67,
      12,    63,   141,   139,   142,   146,   137,  -287,   -38,   -69,
    -287,   -81,   -96,     0,  -287,    43,   -18,  -287,  -287,  -287,
    -287,   291,   -73,   -66,  -287,   118,  -287,   312,   -63,   -27,
    -287,   311,   -24,   -28,   332,   -33,    60,  -287,   138,     3,
     121,   122,   -67,    64,  -286,  -100,  -287,   340,  -287,  -173,
    -287,  -287,  -287,  -287,   352
  };

  const short
  parser::yydefgoto_[] =
  {
       0,   116,   117,   118,   279,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     213,   134,   169,   135,    27,    28,    29,    30,    31,    55,
      32,    77,    78,    79,   163,   164,    33,    60,    61,    34,
      43,    44,    89,    46,    47,    66,   151,   152,   153,   176,
     262,   263,   241,   242,   243,   137,   138,   139,   140,   141,
     142,   143,   144,    35,    36
  };

  const short
  parser::yytable_[] =
  {
      26,   147,    45,   148,   161,   156,   182,   187,    57,   158,
     159,   174,   214,    65,    52,   274,    63,   220,   221,    37,
     171,   232,   340,    64,   170,    59,   175,    48,    49,    50,
      51,   193,   233,    39,    76,    26,    41,    40,    87,    40,
     238,   157,   136,   168,   260,    81,   258,    41,    86,    41,
     167,    40,    82,    83,    41,   165,    64,    76,    76,    76,
      67,    85,    76,    68,   168,   162,   253,    40,   178,   179,
     181,   167,     7,   150,    76,    69,   147,    41,   340,    70,
      62,   222,   223,   268,    19,    20,   247,   234,   252,   167,
     235,   224,   225,   161,   277,    75,   174,    53,    54,   174,
     331,   332,    38,   269,   270,   271,    59,   285,   286,   287,
     171,   175,    80,   237,   175,    42,   281,    42,    75,    75,
      75,   235,   248,    75,   168,   259,   244,   280,    59,    42,
     235,   167,   215,   216,   217,    75,   335,    64,   202,    86,
     172,   173,   235,   305,   284,    42,   327,   261,    76,   235,
     235,   302,   254,   328,   329,   255,   323,   218,   219,    76,
     317,   273,    76,    73,    70,    74,   304,   326,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   276,   313,
     265,   235,   314,   266,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   168,   167,   333,   226,   227,   235,   334,   167,    75,
     366,   235,   235,   307,   308,   368,   370,   183,   168,   160,
      75,   184,   320,    75,   321,   167,   185,   350,   351,   352,
      86,   165,   261,   167,   292,   293,   294,   295,   186,   147,
     147,   339,   341,   196,   188,   197,   150,   198,   189,   353,
     355,   357,   348,   147,   367,   190,   369,   235,   336,   363,
     364,   199,   200,   201,   358,   373,   288,   289,   375,     7,
     376,   191,    56,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,   290,   291,   296,
     297,   147,   194,   371,   195,   147,   228,   339,   359,   229,
     231,   230,    25,   245,   246,   167,   249,   256,   267,   264,
     272,   275,   282,   283,   306,   309,   312,   315,   260,   330,
     150,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,   324,   325,   322,   344,   337,
      90,    69,    91,   342,   343,   361,    92,   360,   362,   365,
      25,    93,    94,    95,    96,    97,    98,   374,   299,   298,
     303,   166,   300,   316,    84,    99,   100,   301,    72,    88,
     346,   318,     0,   319,   311,    71,   349,    58,     0,     0,
       0,     0,     0,     0,     0,   101,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       0,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,     0,     0,   113,    25,   114,   115,    90,    69,
     236,     0,     0,     0,    92,     0,     0,     0,     0,    93,
      94,    95,    96,    97,    98,     0,     0,     0,     0,     0,
       0,     0,     0,    99,   100,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   101,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,     0,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
      90,     0,   113,    25,   114,   115,    92,     0,     0,     0,
       0,    93,    94,    95,    96,    97,    98,     0,     0,     0,
       0,     0,     0,     0,     0,    99,   100,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   101,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       0,     0,     0,     0,     0,     0,     0,    90,    69,     0,
       0,     0,     0,    92,   146,    25,   114,   115,    93,    94,
      95,    96,    97,    98,   192,     0,     0,     0,     0,     0,
      92,     0,    99,   100,     0,    93,    94,    95,    96,    97,
      98,     0,     0,     0,     0,    90,     0,     0,     0,    99,
     100,    92,   101,     0,     0,     0,    93,    94,    95,    96,
      97,    98,     0,     0,     0,     0,     0,     0,     0,   101,
      99,   100,     0,     0,     0,     0,     0,     0,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,     0,
     101,   113,     0,   114,   115,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,     0,
     114,   115,   145,   338,     0,     0,     0,    92,     0,   239,
       0,   240,    93,    94,    95,    96,    97,    98,     0,   146,
       0,   114,   115,   145,   372,     0,    99,   100,    92,     0,
     239,     0,   240,    93,    94,    95,    96,    97,    98,     0,
       0,     0,     0,     0,     0,   145,   101,    99,   100,     0,
      92,     0,   239,     0,   240,    93,    94,    95,    96,    97,
      98,     0,     0,     0,     0,     0,   145,   101,     0,    99,
     100,    92,     0,     0,     0,     0,    93,    94,    95,    96,
      97,    98,     0,     0,     0,   146,     0,   114,   115,   101,
      99,   100,     0,     0,     0,     0,     0,     0,     0,     0,
     257,     0,   322,     0,     0,     0,   146,    92,   114,   115,
     101,     0,    93,    94,    95,    96,    97,    98,     0,     0,
       0,     0,     0,     0,     0,     0,    99,   100,   146,     0,
     114,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   101,     0,     0,   146,
       7,   114,   115,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,     0,     0,
      92,     0,     0,     0,     0,    93,    94,    95,    96,    97,
      98,     0,     0,    25,     0,   146,     0,   114,   115,    99,
     100,     0,     0,     0,     0,    92,     0,     0,   154,     0,
      93,    94,    95,    96,    97,    98,     0,     0,     0,   101,
       0,     0,     0,     0,    99,   100,     7,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,   101,     0,     0,   155,     0,     0,
       0,     7,     0,     0,     0,     0,     0,     0,   146,    25,
     114,   115,    92,    19,    20,   250,     0,    93,    94,    95,
      96,    97,    98,     0,     0,     0,     0,     0,     0,     0,
       0,    99,   100,   146,     0,   114,   115,    92,     0,     0,
       0,     0,    93,    94,    95,    96,    97,    98,     0,     0,
       0,   101,     0,     0,   251,     0,    99,   100,     7,    92,
     278,     0,     0,     0,    93,    94,    95,    96,    97,    98,
      19,    20,     0,     0,     0,     0,   101,     0,    99,   100,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
     146,     0,   114,   115,    92,    19,    20,   347,   101,    93,
      94,    95,    96,    97,    98,     0,     0,     0,     0,     0,
       0,     0,     0,    99,   100,   146,     0,   114,   115,     0,
       0,     0,    92,   354,     0,     0,     0,    93,    94,    95,
      96,    97,    98,   101,     0,     0,     0,   146,     0,   114,
     115,    99,   100,    92,   356,     0,     0,     0,    93,    94,
      95,    96,    97,    98,     0,     0,     0,     0,     0,    92,
       0,   101,    99,   100,    93,    94,    95,    96,    97,    98,
       0,     0,   146,     0,   114,   115,     0,     0,    99,   100,
       0,     0,   101,     0,     0,     0,   177,     0,     0,     0,
       0,    93,    94,    95,    96,    97,    98,     0,   101,     0,
     146,     0,   114,   115,     0,    99,   100,   180,     0,     0,
       0,     0,    93,    94,    95,    96,    97,    98,     0,     0,
       0,   146,     0,   114,   115,   101,    99,   100,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   146,     0,   114,
     115,     0,     0,     0,     0,     0,   101,     0,     0,     0,
     149,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   146,     0,   114,   115,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   146,     0,   114,   115,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,   345,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,   310,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     7,     0,
       0,    25,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25
  };

  const short
  parser::yycheck_[] =
  {
       0,    70,    26,    70,    77,    74,   102,   107,    35,    75,
      76,    92,   120,    41,     4,   188,    40,    30,    31,     4,
      83,    26,   308,    41,     5,    87,    92,    27,    28,    29,
      30,   112,    37,     3,    52,    35,    19,     9,    66,     9,
     140,    74,    69,    81,     9,     8,     5,    19,    66,    19,
      81,     9,     5,     6,    19,    79,    74,    75,    76,    77,
       3,    10,    80,     6,   102,     7,   162,     9,    99,   100,
     101,   102,    55,    73,    92,     4,   145,    19,   364,     8,
       4,    22,    23,   183,    67,    68,   155,     3,   157,   120,
       6,    32,    33,   166,   194,    52,   177,    87,    88,   180,
     273,   274,    87,   184,   185,   186,    87,   215,   216,   217,
     173,   177,     4,   140,   180,    87,   197,    87,    75,    76,
      77,     6,   155,    80,   162,    10,   150,   196,    87,    87,
       6,   162,    19,    20,    21,    92,    12,   155,     8,   157,
       5,     6,     6,   239,   213,    87,    10,   175,   166,     6,
       6,   232,     3,    10,    10,     6,   264,    17,    18,   177,
     256,   188,   180,     9,     8,    11,   235,   267,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,     3,   248,
     177,     6,   251,   180,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   239,   233,     6,    34,    35,     6,    10,   239,   166,
      10,     6,     6,     5,     6,    10,    10,     7,   256,     5,
     177,     9,     9,   180,    11,   256,     9,   327,   328,   329,
     248,   255,   260,   264,   222,   223,   224,   225,     9,   308,
     309,   308,   309,     9,     9,    11,   246,    13,    87,   330,
     331,   332,   321,   322,   354,     3,   356,     6,     7,     5,
       6,    27,    28,    29,   333,   365,   218,   219,   368,    55,
     370,     3,     0,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,   220,   221,   226,
     227,   360,     7,   360,    90,   364,    14,   364,   336,    24,
      36,    25,    88,    10,     6,   336,    12,     7,     7,    10,
      78,     3,    87,    87,    87,     8,    12,    12,     9,     9,
     320,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    10,    10,     4,    10,    12,
       3,     4,     5,    12,    12,    10,     9,     8,    12,    86,
      88,    14,    15,    16,    17,    18,    19,     3,   229,   228,
     233,    80,   230,   255,    62,    28,    29,   231,    46,    68,
     320,   260,    -1,   261,   246,    45,   322,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      -1,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    -1,    -1,    87,    88,    89,    90,     3,     4,
       5,    -1,    -1,    -1,     9,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
       3,    -1,    87,    88,    89,    90,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    -1,     9,    87,    88,    89,    90,    14,    15,
      16,    17,    18,    19,     3,    -1,    -1,    -1,    -1,    -1,
       9,    -1,    28,    29,    -1,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,    28,
      29,     9,    48,    -1,    -1,    -1,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    -1,
      48,    87,    -1,    89,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,
      89,    90,     4,     5,    -1,    -1,    -1,     9,    -1,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    -1,    87,
      -1,    89,    90,     4,     5,    -1,    28,    29,     9,    -1,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,     4,    48,    28,    29,    -1,
       9,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,     4,    48,    -1,    28,
      29,     9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    87,    -1,    89,    90,    48,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       5,    -1,     4,    -1,    -1,    -1,    87,     9,    89,    90,
      48,    -1,    14,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    87,    -1,
      89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    87,
      55,    89,    90,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    -1,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    -1,    -1,    88,    -1,    87,    -1,    89,    90,    28,
      29,    -1,    -1,    -1,    -1,     9,    -1,    -1,    12,    -1,
      14,    15,    16,    17,    18,    19,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    28,    29,    55,    -1,    -1,    -1,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    48,    -1,    -1,    51,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      89,    90,     9,    67,    68,    12,    -1,    14,    15,    16,
      17,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    28,    29,    87,    -1,    89,    90,     9,    -1,    -1,
      -1,    -1,    14,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    48,    -1,    -1,    51,    -1,    28,    29,    55,     9,
      10,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      67,    68,    -1,    -1,    -1,    -1,    48,    -1,    28,    29,
      -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    -1,    89,    90,     9,    67,    68,    12,    48,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    87,    -1,    89,    90,    -1,
      -1,    -1,     9,    10,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    48,    -1,    -1,    -1,    87,    -1,    89,
      90,    28,    29,     9,    10,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,     9,
      -1,    48,    28,    29,    14,    15,    16,    17,    18,    19,
      -1,    -1,    87,    -1,    89,    90,    -1,    -1,    28,    29,
      -1,    -1,    48,    -1,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    -1,    48,    -1,
      87,    -1,    89,    90,    -1,    28,    29,     9,    -1,    -1,
      -1,    -1,    14,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    87,    -1,    89,    90,    48,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,    89,
      90,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    -1,    89,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    -1,    89,    90,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    88,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    88,   114,   115,   116,   117,
     118,   119,   121,   127,   130,   154,   155,     4,    87,     3,
       9,    19,    87,   131,   132,   133,   134,   135,   114,   114,
     114,   114,     4,    87,    88,   120,     0,   130,   155,    87,
     128,   129,     4,   133,   117,   134,   136,     3,     6,     4,
       8,   148,   135,     9,    11,   116,   117,   122,   123,   124,
       4,     8,     5,     6,   128,    10,   117,   134,   132,   133,
       3,     5,     9,    14,    15,    16,    17,    18,    19,    28,
      29,    48,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    87,    89,    90,    92,    93,    94,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   112,   114,   130,   146,   147,   148,
     149,   150,   151,   152,   153,     4,    87,   110,   143,    10,
     114,   137,   138,   139,    12,    51,   110,   136,   124,   124,
       5,   123,     7,   125,   126,   133,   122,    96,   109,   113,
       5,   129,     5,     6,   112,   124,   140,     9,    96,    96,
       9,    96,   113,     7,     9,     9,     9,   146,     9,    87,
       3,     3,     3,   112,     7,    90,     9,    11,    13,    27,
      28,    29,     8,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   111,    98,    19,    20,    21,    17,    18,
      30,    31,    22,    23,    32,    33,    34,    35,    14,    24,
      25,    36,    26,    37,     3,     6,     5,   130,   146,    11,
      13,   143,   144,   145,   133,    10,     6,   110,   136,    12,
      12,    51,   110,   113,     3,     6,     7,     5,     5,    10,
       9,   134,   141,   142,    10,   140,   140,     7,   146,   112,
     112,   112,    78,   130,   150,     3,     3,   146,    10,    95,
     110,   112,    87,    87,   110,    98,    98,    98,    99,    99,
     100,   100,   101,   101,   101,   101,   102,   102,   103,   104,
     105,   106,   112,   107,   110,   113,    87,     5,     6,     8,
      73,   139,    12,   110,   110,    12,   126,   113,   141,   142,
       9,    11,     4,    98,    10,    10,   146,    10,    10,    10,
       9,   150,   150,     6,    10,    12,     7,    12,     5,   143,
     145,   143,    12,    12,    10,    10,   137,    12,   110,   144,
     146,   146,   146,   112,    10,   112,    10,   112,   110,   109,
       8,    10,    12,     5,     6,    86,    10,   146,    10,   146,
      10,   143,     5,   146,     3,   146,   146
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    91,    92,    92,    92,    92,    92,    92,    93,    93,
      94,    94,    94,    94,    94,    94,    94,    94,    95,    95,
      96,    96,    96,    96,    96,    96,    97,    97,    97,    97,
      97,    97,    98,    98,    99,    99,    99,    99,   100,   100,
     100,   101,   101,   101,   102,   102,   102,   102,   102,   103,
     103,   103,   104,   104,   105,   105,   106,   106,   107,   107,
     108,   108,   109,   109,   110,   110,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   112,   112,   113,
     114,   114,   114,   114,   114,   114,   114,   114,   115,   115,
     115,   115,   115,   115,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   117,   117,   117,   118,
     118,   118,   119,   119,   119,   120,   120,   121,   121,   122,
     122,   123,   124,   124,   124,   124,   125,   125,   126,   126,
     126,   127,   127,   127,   127,   127,   128,   128,   129,   129,
     130,   130,   131,   131,   132,   132,   133,   133,   134,   134,
     134,   134,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   136,   136,   137,   137,   138,   138,   139,
     139,   140,   140,   141,   141,   141,   142,   142,   142,   142,
     142,   143,   143,   143,   144,   144,   144,   144,   145,   145,
     146,   146,   146,   146,   146,   146,   147,   147,   147,   148,
     148,   149,   149,   149,   149,   150,   150,   151,   151,   151,
     152,   152,   152,   152,   152,   152,   153,   153,   153,   153,
     153,   154,   154,   154,   154,   155
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     3,     6,     7,     1,     2,
       1,     4,     3,     4,     3,     3,     2,     2,     1,     3,
       1,     2,     2,     2,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     4,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     5,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     2,     1,     2,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     4,     2,     1,     1,     1,     1,     1,
       2,     3,     2,     1,     2,     1,     1,     3,     1,     2,
       3,     4,     5,     5,     6,     2,     1,     3,     1,     3,
       2,     3,     1,     3,     1,     3,     2,     1,     1,     2,
       2,     3,     1,     3,     5,     6,     6,     4,     4,     5,
       3,     4,     3,     1,     2,     1,     3,     1,     3,     2,
       1,     1,     2,     1,     2,     1,     3,     3,     4,     3,
       4,     1,     3,     4,     1,     3,     3,     5,     3,     2,
       1,     1,     1,     1,     1,     1,     3,     4,     3,     2,
       3,     1,     1,     2,     2,     1,     2,     5,     7,     5,
       5,     7,     6,     7,     6,     7,     3,     2,     2,     2,
       3,     1,     1,     2,     2,     3
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   156,   156,   157,   158,   159,   160,   161,   165,   166,
     170,   171,   172,   173,   174,   175,   176,   177,   181,   182,
     186,   187,   188,   189,   190,   191,   195,   196,   197,   198,
     199,   200,   204,   205,   209,   210,   211,   212,   216,   217,
     218,   222,   223,   224,   228,   229,   230,   231,   232,   236,
     237,   238,   242,   243,   247,   248,   252,   253,   257,   258,
     262,   263,   267,   268,   272,   273,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   291,   292,   296,
     302,   303,   304,   305,   306,   307,   308,   309,   313,   314,
     315,   316,   317,   318,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   337,   338,   339,   343,
     344,   345,   349,   350,   351,   355,   356,   359,   360,   364,
     365,   369,   373,   374,   375,   376,   380,   381,   385,   386,
     387,   391,   392,   393,   394,   395,   399,   400,   404,   405,
     412,   413,   417,   418,   422,   423,   427,   428,   432,   433,
     434,   435,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,   449,   453,   454,   458,   459,   463,   464,   468,
     469,   473,   474,   478,   479,   480,   484,   485,   486,   487,
     488,   495,   496,   497,   501,   502,   503,   504,   508,   509,
     516,   517,   518,   519,   520,   521,   525,   526,   527,   531,
     532,   536,   537,   538,   539,   543,   544,   548,   549,   550,
     554,   555,   556,   557,   558,   559,   563,   564,   565,   566,
     567,   574,   575,   576,   577,   581
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 3932 "yy_parser.cpp"

#line 584 "parser.y"


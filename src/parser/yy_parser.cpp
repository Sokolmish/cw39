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
    #include "lexer.hpp"

#line 52 "yy_parser.cpp"


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
#line 144 "yy_parser.cpp"

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
#line 152 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimIdent(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1530 "yy_parser.cpp"
    break;

  case 3: // primary_expr: CONSTANT
#line 153 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimConst(yystack_[0].value.as < AST_Literal > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1536 "yy_parser.cpp"
    break;

  case 4: // primary_expr: strings_seq
#line 154 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimStr(yystack_[0].value.as < AST_StringsSeq * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1542 "yy_parser.cpp"
    break;

  case 5: // primary_expr: "(" expr ")"
#line 155 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimExpr(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1548 "yy_parser.cpp"
    break;

  case 6: // primary_expr: "(" type_name ")" "{" init_lst "}"
#line 156 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimCompound(yystack_[4].value.as < AST_TypeName * > (), yystack_[1].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1554 "yy_parser.cpp"
    break;

  case 7: // primary_expr: "(" type_name ")" "{" init_lst "," "}"
#line 157 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPrimCompound(yystack_[5].value.as < AST_TypeName * > (), yystack_[2].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1560 "yy_parser.cpp"
    break;

  case 8: // strings_seq: STRING_LITERAL
#line 161 "parser.y"
                                                { yylhs.value.as < AST_StringsSeq * > () = drv.ast->mkStringsSeq()->append(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_StringsSeq * > (), yylhs.location); }
#line 1566 "yy_parser.cpp"
    break;

  case 9: // strings_seq: strings_seq STRING_LITERAL
#line 162 "parser.y"
                                                { yylhs.value.as < AST_StringsSeq * > () = yystack_[1].value.as < AST_StringsSeq * > ()->append(yystack_[0].value.as < string_id_t > ()); }
#line 1572 "yy_parser.cpp"
    break;

  case 10: // postfix_expr: primary_expr
#line 166 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1578 "yy_parser.cpp"
    break;

  case 11: // postfix_expr: postfix_expr "[" expr "]"
#line 167 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfArr(yystack_[3].value.as < AST_Expr * > (), yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1584 "yy_parser.cpp"
    break;

  case 12: // postfix_expr: postfix_expr "(" ")"
#line 168 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfCall(yystack_[2].value.as < AST_Expr * > (), nullptr); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1590 "yy_parser.cpp"
    break;

  case 13: // postfix_expr: postfix_expr "(" arg_expr_lst ")"
#line 169 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfCall(yystack_[3].value.as < AST_Expr * > (), yystack_[1].value.as < AST_ArgumentsList * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1596 "yy_parser.cpp"
    break;

  case 14: // postfix_expr: postfix_expr "." IDENTIFIER
#line 170 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfAccesor(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < string_id_t > (), false); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1602 "yy_parser.cpp"
    break;

  case 15: // postfix_expr: postfix_expr PTR_OP IDENTIFIER
#line 171 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfAccesor(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < string_id_t > (), true); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1608 "yy_parser.cpp"
    break;

  case 16: // postfix_expr: postfix_expr INC_OP
#line 172 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfIncdec(yystack_[1].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1614 "yy_parser.cpp"
    break;

  case 17: // postfix_expr: postfix_expr DEC_OP
#line 173 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkPostfIncdec(yystack_[1].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1620 "yy_parser.cpp"
    break;

  case 18: // arg_expr_lst: assign_expr
#line 177 "parser.y"
                                                { yylhs.value.as < AST_ArgumentsList * > () = drv.ast->mkArgsLst()->append(yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_ArgumentsList * > (), yylhs.location); }
#line 1626 "yy_parser.cpp"
    break;

  case 19: // arg_expr_lst: arg_expr_lst "," assign_expr
#line 178 "parser.y"
                                                { yylhs.value.as < AST_ArgumentsList * > () = yystack_[2].value.as < AST_ArgumentsList * > ()->append(yystack_[0].value.as < AST_Expr * > ()); }
#line 1632 "yy_parser.cpp"
    break;

  case 20: // unary_expr: postfix_expr
#line 182 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1638 "yy_parser.cpp"
    break;

  case 21: // unary_expr: INC_OP unary_expr
#line 183 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::PRE_INC, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1644 "yy_parser.cpp"
    break;

  case 22: // unary_expr: DEC_OP unary_expr
#line 184 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::PRE_DEC, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1650 "yy_parser.cpp"
    break;

  case 23: // unary_expr: unary_operator cast_expr
#line 185 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::OpType(yystack_[1].value.as < short > ()), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1656 "yy_parser.cpp"
    break;

  case 24: // unary_expr: SIZEOF unary_expr
#line 186 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1662 "yy_parser.cpp"
    break;

  case 25: // unary_expr: SIZEOF "(" type_name ")"
#line 187 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkUnop(AST_Unop::SIZEOF_OP, yystack_[1].value.as < AST_TypeName * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1668 "yy_parser.cpp"
    break;

  case 26: // unary_operator: "&"
#line 191 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::ADDR_OF; }
#line 1674 "yy_parser.cpp"
    break;

  case 27: // unary_operator: "*"
#line 192 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::DEREF; }
#line 1680 "yy_parser.cpp"
    break;

  case 28: // unary_operator: "+"
#line 193 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_PLUS; }
#line 1686 "yy_parser.cpp"
    break;

  case 29: // unary_operator: "-"
#line 194 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_MINUS; }
#line 1692 "yy_parser.cpp"
    break;

  case 30: // unary_operator: "~"
#line 195 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_NEG; }
#line 1698 "yy_parser.cpp"
    break;

  case 31: // unary_operator: "!"
#line 196 "parser.y"
                                                { yylhs.value.as < short > () = AST_Unop::UN_NOT; }
#line 1704 "yy_parser.cpp"
    break;

  case 32: // cast_expr: unary_expr
#line 200 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1710 "yy_parser.cpp"
    break;

  case 33: // cast_expr: "(" type_name ")" cast_expr
#line 201 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkCastop(yystack_[2].value.as < AST_TypeName * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1716 "yy_parser.cpp"
    break;

  case 34: // mult_expr: cast_expr
#line 205 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1722 "yy_parser.cpp"
    break;

  case 35: // mult_expr: mult_expr "*" cast_expr
#line 206 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::MUL, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1728 "yy_parser.cpp"
    break;

  case 36: // mult_expr: mult_expr "/" cast_expr
#line 207 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::DIV, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1734 "yy_parser.cpp"
    break;

  case 37: // mult_expr: mult_expr "%" cast_expr
#line 208 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::REM, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1740 "yy_parser.cpp"
    break;

  case 38: // add_expr: mult_expr
#line 212 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1746 "yy_parser.cpp"
    break;

  case 39: // add_expr: add_expr "+" mult_expr
#line 213 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::ADD, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1752 "yy_parser.cpp"
    break;

  case 40: // add_expr: add_expr "-" mult_expr
#line 214 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SUB, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1758 "yy_parser.cpp"
    break;

  case 41: // shift_expr: add_expr
#line 218 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1764 "yy_parser.cpp"
    break;

  case 42: // shift_expr: shift_expr LEFT_OP add_expr
#line 219 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SHL, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1770 "yy_parser.cpp"
    break;

  case 43: // shift_expr: shift_expr RIGHT_OP add_expr
#line 220 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::SHR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1776 "yy_parser.cpp"
    break;

  case 44: // rel_expr: shift_expr
#line 224 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1782 "yy_parser.cpp"
    break;

  case 45: // rel_expr: rel_expr "<" shift_expr
#line 225 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LT, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1788 "yy_parser.cpp"
    break;

  case 46: // rel_expr: rel_expr ">" shift_expr
#line 226 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::GT, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1794 "yy_parser.cpp"
    break;

  case 47: // rel_expr: rel_expr LE_OP shift_expr
#line 227 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1800 "yy_parser.cpp"
    break;

  case 48: // rel_expr: rel_expr GE_OP shift_expr
#line 228 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::GE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1806 "yy_parser.cpp"
    break;

  case 49: // eq_expr: rel_expr
#line 232 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1812 "yy_parser.cpp"
    break;

  case 50: // eq_expr: eq_expr EQ_OP rel_expr
#line 233 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::EQ, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1818 "yy_parser.cpp"
    break;

  case 51: // eq_expr: eq_expr NE_OP rel_expr
#line 234 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::NE, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1824 "yy_parser.cpp"
    break;

  case 52: // and_expr: eq_expr
#line 238 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1830 "yy_parser.cpp"
    break;

  case 53: // and_expr: and_expr "&" eq_expr
#line 239 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_AND, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1836 "yy_parser.cpp"
    break;

  case 54: // xor_expr: and_expr
#line 243 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1842 "yy_parser.cpp"
    break;

  case 55: // xor_expr: xor_expr "^" and_expr
#line 244 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_XOR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1848 "yy_parser.cpp"
    break;

  case 56: // or_expr: xor_expr
#line 248 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1854 "yy_parser.cpp"
    break;

  case 57: // or_expr: or_expr "|" xor_expr
#line 249 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::BIT_OR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1860 "yy_parser.cpp"
    break;

  case 58: // land_expr: or_expr
#line 253 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1866 "yy_parser.cpp"
    break;

  case 59: // land_expr: land_expr AND_OP or_expr
#line 254 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LOG_AND, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1872 "yy_parser.cpp"
    break;

  case 60: // lor_expr: land_expr
#line 258 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1878 "yy_parser.cpp"
    break;

  case 61: // lor_expr: lor_expr OR_OP land_expr
#line 259 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkBinop(AST_Binop::LOG_OR, yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1884 "yy_parser.cpp"
    break;

  case 62: // ternary_expr: lor_expr
#line 263 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1890 "yy_parser.cpp"
    break;

  case 63: // ternary_expr: lor_expr "?" expr ":" ternary_expr
#line 264 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkTernary(yystack_[4].value.as < AST_Expr * > (), yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1896 "yy_parser.cpp"
    break;

  case 64: // assign_expr: ternary_expr
#line 268 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1902 "yy_parser.cpp"
    break;

  case 65: // assign_expr: unary_expr assign_op assign_expr
#line 269 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = drv.ast->mkAssign(AST_Assignment::OpType(yystack_[1].value.as < short > ()), yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Expr * > (), yylhs.location); }
#line 1908 "yy_parser.cpp"
    break;

  case 66: // assign_op: "="
#line 273 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::DIRECT; }
#line 1914 "yy_parser.cpp"
    break;

  case 67: // assign_op: MUL_ASSIGN
#line 274 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::MUL; }
#line 1920 "yy_parser.cpp"
    break;

  case 68: // assign_op: DIV_ASSIGN
#line 275 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::DIV; }
#line 1926 "yy_parser.cpp"
    break;

  case 69: // assign_op: MOD_ASSIGN
#line 276 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::REM; }
#line 1932 "yy_parser.cpp"
    break;

  case 70: // assign_op: ADD_ASSIGN
#line 277 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::ADD; }
#line 1938 "yy_parser.cpp"
    break;

  case 71: // assign_op: SUB_ASSIGN
#line 278 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SUB; }
#line 1944 "yy_parser.cpp"
    break;

  case 72: // assign_op: LEFT_ASSIGN
#line 279 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SHL; }
#line 1950 "yy_parser.cpp"
    break;

  case 73: // assign_op: RIGHT_ASSIGN
#line 280 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::SHR; }
#line 1956 "yy_parser.cpp"
    break;

  case 74: // assign_op: AND_ASSIGN
#line 281 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::AND; }
#line 1962 "yy_parser.cpp"
    break;

  case 75: // assign_op: XOR_ASSIGN
#line 282 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::XOR; }
#line 1968 "yy_parser.cpp"
    break;

  case 76: // assign_op: OR_ASSIGN
#line 283 "parser.y"
                                                { yylhs.value.as < short > () = AST_Assignment::OR; }
#line 1974 "yy_parser.cpp"
    break;

  case 77: // expr: assign_expr
#line 287 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); /* TODO: fixme */ }
#line 1980 "yy_parser.cpp"
    break;

  case 78: // expr: expr "," assign_expr
#line 288 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = dynamic_cast<AST_CommaExpression*>(yystack_[2].value.as < AST_Expr * > ())->append(yystack_[0].value.as < AST_Expr * > ()); }
#line 1986 "yy_parser.cpp"
    break;

  case 79: // const_expr: ternary_expr
#line 292 "parser.y"
                                                { yylhs.value.as < AST_Expr * > () = yystack_[0].value.as < AST_Expr * > (); }
#line 1992 "yy_parser.cpp"
    break;

  case 80: // decl_specs: storage_class_specifier
#line 298 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_storage(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 1998 "yy_parser.cpp"
    break;

  case 81: // decl_specs: storage_class_specifier decl_specs
#line 299 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_storage(yystack_[1].value.as < short > ()); }
#line 2004 "yy_parser.cpp"
    break;

  case 82: // decl_specs: type_specifier
#line 300 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_type_spec(yystack_[0].value.as < AST_TypeSpecifier * > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2010 "yy_parser.cpp"
    break;

  case 83: // decl_specs: type_specifier decl_specs
#line 301 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_type_spec(yystack_[1].value.as < AST_TypeSpecifier * > ()); }
#line 2016 "yy_parser.cpp"
    break;

  case 84: // decl_specs: type_qual
#line 302 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_type_qual(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2022 "yy_parser.cpp"
    break;

  case 85: // decl_specs: type_qual decl_specs
#line 303 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_type_qual(yystack_[1].value.as < short > ()); }
#line 2028 "yy_parser.cpp"
    break;

  case 86: // decl_specs: function_specifier
#line 304 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = drv.ast->mkDeclSpecs()->update_func_qual(yystack_[0].value.as < short > ()); SL(yylhs.value.as < AST_DeclSpecifiers * > (), yylhs.location); }
#line 2034 "yy_parser.cpp"
    break;

  case 87: // decl_specs: function_specifier decl_specs
#line 305 "parser.y"
                                                { yylhs.value.as < AST_DeclSpecifiers * > () = yystack_[0].value.as < AST_DeclSpecifiers * > ()->update_func_qual(yystack_[1].value.as < short > ()); }
#line 2040 "yy_parser.cpp"
    break;

  case 88: // storage_class_specifier: TYPEDEF
#line 309 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_TYPEDEF; }
#line 2046 "yy_parser.cpp"
    break;

  case 89: // storage_class_specifier: EXTERN
#line 310 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_EXTERN; }
#line 2052 "yy_parser.cpp"
    break;

  case 90: // storage_class_specifier: STATIC
#line 311 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_STATIC; }
#line 2058 "yy_parser.cpp"
    break;

  case 91: // storage_class_specifier: AUTO
#line 312 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_AUTO; }
#line 2064 "yy_parser.cpp"
    break;

  case 92: // storage_class_specifier: REGISTER
#line 313 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::ST_REGISTER; }
#line 2070 "yy_parser.cpp"
    break;

  case 93: // type_specifier: VOID
#line 317 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_VOID); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2076 "yy_parser.cpp"
    break;

  case 94: // type_specifier: CHAR
#line 318 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_CHAR); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2082 "yy_parser.cpp"
    break;

  case 95: // type_specifier: SHORT
#line 319 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SHORT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2088 "yy_parser.cpp"
    break;

  case 96: // type_specifier: INT
#line 320 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_INT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2094 "yy_parser.cpp"
    break;

  case 97: // type_specifier: LONG
#line 321 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_LONG); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2100 "yy_parser.cpp"
    break;

  case 98: // type_specifier: FLOAT
#line 322 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_FLOAT); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2106 "yy_parser.cpp"
    break;

  case 99: // type_specifier: DOUBLE
#line 323 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_DOUBLE); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2112 "yy_parser.cpp"
    break;

  case 100: // type_specifier: SIGNED
#line 324 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_SIGNED); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2118 "yy_parser.cpp"
    break;

  case 101: // type_specifier: UNSIGNED
#line 325 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(AST_TypeSpecifier::T_UNSIGNED); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2124 "yy_parser.cpp"
    break;

  case 102: // type_specifier: ustruct_spec
#line 326 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(yystack_[0].value.as < AST_UStructSpec * > ()); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2130 "yy_parser.cpp"
    break;

  case 103: // type_specifier: enum_specifier
#line 327 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(yystack_[0].value.as < AST_EnumSpecifier * > ()); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2136 "yy_parser.cpp"
    break;

  case 104: // type_specifier: TYPE_NAME
#line 328 "parser.y"
                                { yylhs.value.as < AST_TypeSpecifier * > () = drv.ast->mkTypeSpec(get_def_type(yystack_[0].value.as < string_id_t > ())); SL(yylhs.value.as < AST_TypeSpecifier * > (), yylhs.location); }
#line 2142 "yy_parser.cpp"
    break;

  case 105: // type_qual: CONST
#line 332 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_CONST; }
#line 2148 "yy_parser.cpp"
    break;

  case 106: // type_qual: RESTRICT
#line 333 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_RESTRICT; }
#line 2154 "yy_parser.cpp"
    break;

  case 107: // type_qual: VOLATILE
#line 334 "parser.y"
                                { yylhs.value.as < short > () = AST_TypeQuals::Q_VOLATILE; }
#line 2160 "yy_parser.cpp"
    break;

  case 108: // function_specifier: INLINE
#line 338 "parser.y"
                                { yylhs.value.as < short > () = AST_DeclSpecifiers::Q_INLINE; }
#line 2166 "yy_parser.cpp"
    break;

  case 109: // ustruct_spec: ustruct IDENTIFIER "{" struct_decls_lst "}"
#line 342 "parser.y"
                                                    { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[4].value.as < short > (), yystack_[3].value.as < string_id_t > (), yystack_[1].value.as < AST_StructDeclarationList * > ()); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2172 "yy_parser.cpp"
    break;

  case 110: // ustruct_spec: ustruct "{" struct_decls_lst "}"
#line 343 "parser.y"
                                                    { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[3].value.as < short > (), NO_IDENT_ID, yystack_[1].value.as < AST_StructDeclarationList * > ()); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2178 "yy_parser.cpp"
    break;

  case 111: // ustruct_spec: ustruct IDENTIFIER
#line 344 "parser.y"
                                                    { yylhs.value.as < AST_UStructSpec * > () = drv.ast->mkUstructSpec(yystack_[1].value.as < short > (), yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_UStructSpec * > (), yylhs.location); }
#line 2184 "yy_parser.cpp"
    break;

  case 112: // ustruct: STRUCT
#line 348 "parser.y"
                                                    { yylhs.value.as < short > () = 0; }
#line 2190 "yy_parser.cpp"
    break;

  case 113: // ustruct: UNION
#line 349 "parser.y"
                                                    { yylhs.value.as < short > () = 1; }
#line 2196 "yy_parser.cpp"
    break;

  case 114: // struct_decls_lst: struct_declaration
#line 353 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclarationList * > () = drv.ast->mkStructDeclarationLst(yystack_[0].value.as < AST_StructDeclaration * > ()); SL(yylhs.value.as < AST_StructDeclarationList * > (), yylhs.location); }
#line 2202 "yy_parser.cpp"
    break;

  case 115: // struct_decls_lst: struct_decls_lst struct_declaration
#line 354 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclarationList * > () = yystack_[1].value.as < AST_StructDeclarationList * > ()->append(yystack_[0].value.as < AST_StructDeclaration * > ()); }
#line 2208 "yy_parser.cpp"
    break;

  case 116: // struct_declaration: spec_qual_lst struct_declarator_list ";"
#line 358 "parser.y"
                                                    { yylhs.value.as < AST_StructDeclaration * > () = drv.ast->mkStructDeclaration(yystack_[2].value.as < AST_SpecsQualsList * > (), yystack_[1].value.as < AST_StructDeclaratorList * > ()); SL(yylhs.value.as < AST_StructDeclaration * > (), yylhs.location); }
#line 2214 "yy_parser.cpp"
    break;

  case 117: // spec_qual_lst: type_specifier spec_qual_lst
#line 362 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = yystack_[0].value.as < AST_SpecsQualsList * > ()->append_spec(yystack_[1].value.as < AST_TypeSpecifier * > ()); }
#line 2220 "yy_parser.cpp"
    break;

  case 118: // spec_qual_lst: type_specifier
#line 363 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = drv.ast->mkSpecQualLst(yystack_[0].value.as < AST_TypeSpecifier * > ()); SL(yylhs.value.as < AST_SpecsQualsList * > (), yylhs.location); }
#line 2226 "yy_parser.cpp"
    break;

  case 119: // spec_qual_lst: type_qual spec_qual_lst
#line 364 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = yystack_[0].value.as < AST_SpecsQualsList * > ()->append_qual(AST_TypeQuals::QualType(yystack_[1].value.as < short > ())); }
#line 2232 "yy_parser.cpp"
    break;

  case 120: // spec_qual_lst: type_qual
#line 365 "parser.y"
                                        { yylhs.value.as < AST_SpecsQualsList * > () = drv.ast->mkSpecQualLst(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); SL(yylhs.value.as < AST_SpecsQualsList * > (), yylhs.location); }
#line 2238 "yy_parser.cpp"
    break;

  case 121: // struct_declarator_list: struct_declarator
#line 369 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclaratorList * > () = drv.ast->mkStructDeclaratorLst(yystack_[0].value.as < AST_StructDeclarator * > ()); SL(yylhs.value.as < AST_StructDeclaratorList * > (), yylhs.location); }
#line 2244 "yy_parser.cpp"
    break;

  case 122: // struct_declarator_list: struct_declarator_list "," struct_declarator
#line 370 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclaratorList * > () = yystack_[2].value.as < AST_StructDeclaratorList * > ()->append(yystack_[0].value.as < AST_StructDeclarator * > ()); }
#line 2250 "yy_parser.cpp"
    break;

  case 123: // struct_declarator: declarator
#line 374 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(yystack_[0].value.as < AST_Declarator * > (), nullptr); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2256 "yy_parser.cpp"
    break;

  case 124: // struct_declarator: ":" const_expr
#line 375 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(nullptr, yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2262 "yy_parser.cpp"
    break;

  case 125: // struct_declarator: declarator ":" const_expr
#line 376 "parser.y"
                                                        { yylhs.value.as < AST_StructDeclarator * > () = drv.ast->mkStructDeclarator(yystack_[2].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_StructDeclarator * > (), yylhs.location); }
#line 2268 "yy_parser.cpp"
    break;

  case 126: // enum_specifier: ENUM "{" enumerator_list "}"
#line 380 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(NO_IDENT_ID, yystack_[1].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2274 "yy_parser.cpp"
    break;

  case 127: // enum_specifier: ENUM IDENTIFIER "{" enumerator_list "}"
#line 381 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[3].value.as < string_id_t > (), yystack_[1].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2280 "yy_parser.cpp"
    break;

  case 128: // enum_specifier: ENUM "{" enumerator_list "," "}"
#line 382 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(NO_IDENT_ID, yystack_[2].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2286 "yy_parser.cpp"
    break;

  case 129: // enum_specifier: ENUM IDENTIFIER "{" enumerator_list "," "}"
#line 383 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[4].value.as < string_id_t > (), yystack_[2].value.as < AST_EnumeratorList * > ()); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2292 "yy_parser.cpp"
    break;

  case 130: // enum_specifier: ENUM IDENTIFIER
#line 384 "parser.y"
                                                        { yylhs.value.as < AST_EnumSpecifier * > () = drv.ast->mkEnumSpec(yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_EnumSpecifier * > (), yylhs.location); }
#line 2298 "yy_parser.cpp"
    break;

  case 131: // enumerator_list: enumerator
#line 388 "parser.y"
                                                        { yylhs.value.as < AST_EnumeratorList * > () = drv.ast->mkEnumLst(yystack_[0].value.as < AST_Enumerator * > ()); SL(yylhs.value.as < AST_EnumeratorList * > (), yylhs.location); }
#line 2304 "yy_parser.cpp"
    break;

  case 132: // enumerator_list: enumerator_list "," enumerator
#line 389 "parser.y"
                                                        { yylhs.value.as < AST_EnumeratorList * > () = yystack_[2].value.as < AST_EnumeratorList * > ()->append(yystack_[0].value.as < AST_Enumerator * > ()); }
#line 2310 "yy_parser.cpp"
    break;

  case 133: // enumerator: IDENTIFIER
#line 393 "parser.y"
                                                        { yylhs.value.as < AST_Enumerator * > () = drv.ast->mkEnumer(yystack_[0].value.as < string_id_t > (), nullptr); SL(yylhs.value.as < AST_Enumerator * > (), yylhs.location); }
#line 2316 "yy_parser.cpp"
    break;

  case 134: // enumerator: IDENTIFIER "=" const_expr
#line 394 "parser.y"
                                                        { yylhs.value.as < AST_Enumerator * > () = drv.ast->mkEnumer(yystack_[2].value.as < string_id_t > (), yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Enumerator * > (), yylhs.location); }
#line 2322 "yy_parser.cpp"
    break;

  case 135: // declaration: decl_specs ";"
#line 401 "parser.y"
                                                        { yylhs.value.as < AST_Declaration * > () = drv.ast->mkDeclaration(yystack_[1].value.as < AST_DeclSpecifiers * > (), nullptr); SL(yylhs.value.as < AST_Declaration * > (), yylhs.location); }
#line 2328 "yy_parser.cpp"
    break;

  case 136: // declaration: decl_specs init_decltor_list ";"
#line 402 "parser.y"
                                                        { yylhs.value.as < AST_Declaration * > () = drv.ast->mkDeclaration(yystack_[2].value.as < AST_DeclSpecifiers * > (), yystack_[1].value.as < AST_InitDeclaratorList * > ()); SL(yylhs.value.as < AST_Declaration * > (), yylhs.location); }
#line 2334 "yy_parser.cpp"
    break;

  case 137: // init_decltor_list: init_decltor
#line 406 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclaratorList * > () = drv.ast->mkInitDeclaratorLst(yystack_[0].value.as < AST_InitDeclarator * > ()); SL(yylhs.value.as < AST_InitDeclaratorList * > (), yylhs.location); }
#line 2340 "yy_parser.cpp"
    break;

  case 138: // init_decltor_list: init_decltor_list "," init_decltor
#line 407 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclaratorList * > () = yystack_[2].value.as < AST_InitDeclaratorList * > ()->append(yystack_[0].value.as < AST_InitDeclarator * > ()); }
#line 2346 "yy_parser.cpp"
    break;

  case 139: // init_decltor: declarator
#line 411 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclarator * > () = drv.ast->mkInitDeclarator(yystack_[0].value.as < AST_Declarator * > (), nullptr); SL(yylhs.value.as < AST_InitDeclarator * > (), yylhs.location); }
#line 2352 "yy_parser.cpp"
    break;

  case 140: // init_decltor: declarator "=" initializer
#line 412 "parser.y"
                                                        { yylhs.value.as < AST_InitDeclarator * > () = drv.ast->mkInitDeclarator(yystack_[2].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_Initializer * > ()); SL(yylhs.value.as < AST_InitDeclarator * > (), yylhs.location); }
#line 2358 "yy_parser.cpp"
    break;

  case 141: // declarator: pointer dir_decltor
#line 416 "parser.y"
                                        { yylhs.value.as < AST_Declarator * > () = drv.ast->mkDeclarator(yystack_[0].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Declarator * > (), yylhs.location); }
#line 2364 "yy_parser.cpp"
    break;

  case 142: // declarator: dir_decltor
#line 417 "parser.y"
                                        { yylhs.value.as < AST_Declarator * > () = drv.ast->mkDeclarator(yystack_[0].value.as < AST_DirDeclarator * > (), nullptr); SL(yylhs.value.as < AST_Declarator * > (), yylhs.location); }
#line 2370 "yy_parser.cpp"
    break;

  case 143: // pointer: "*"
#line 421 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(nullptr, nullptr); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2376 "yy_parser.cpp"
    break;

  case 144: // pointer: "*" type_qual_lst
#line 422 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(yystack_[0].value.as < AST_TypeQuals * > (), nullptr); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2382 "yy_parser.cpp"
    break;

  case 145: // pointer: "*" pointer
#line 423 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(nullptr, yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2388 "yy_parser.cpp"
    break;

  case 146: // pointer: "*" type_qual_lst pointer
#line 424 "parser.y"
                                        { yylhs.value.as < AST_Pointer * > () = drv.ast->mkPointer(yystack_[1].value.as < AST_TypeQuals * > (), yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_Pointer * > (), yylhs.location); }
#line 2394 "yy_parser.cpp"
    break;

  case 147: // dir_decltor: IDENTIFIER
#line 428 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclIdent(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2400 "yy_parser.cpp"
    break;

  case 148: // dir_decltor: "(" declarator ")"
#line 429 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclNested(yystack_[1].value.as < AST_Declarator * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2406 "yy_parser.cpp"
    break;

  case 149: // dir_decltor: dir_decltor "[" type_qual_lst assign_expr "]"
#line 430 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[4].value.as < AST_DirDeclarator * > (), yystack_[2].value.as < AST_TypeQuals * > (), yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2412 "yy_parser.cpp"
    break;

  case 150: // dir_decltor: dir_decltor "[" type_qual_lst "]"
#line 431 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[3].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_TypeQuals * > (), nullptr); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2418 "yy_parser.cpp"
    break;

  case 151: // dir_decltor: dir_decltor "[" assign_expr "]"
#line 432 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[3].value.as < AST_DirDeclarator * > (), nullptr, yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2424 "yy_parser.cpp"
    break;

  case 152: // dir_decltor: dir_decltor "[" "]"
#line 433 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclArr(yystack_[2].value.as < AST_DirDeclarator * > (), nullptr, nullptr); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2430 "yy_parser.cpp"
    break;

  case 153: // dir_decltor: dir_decltor "(" param_type_lst ")"
#line 434 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclFunc(yystack_[3].value.as < AST_DirDeclarator * > (), yystack_[1].value.as < AST_ParameterTypeList * > ()); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2436 "yy_parser.cpp"
    break;

  case 154: // dir_decltor: dir_decltor "(" ")"
#line 435 "parser.y"
                                                        { yylhs.value.as < AST_DirDeclarator * > () = drv.ast->mkDirDeclFunc(yystack_[2].value.as < AST_DirDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirDeclarator * > (), yylhs.location); }
#line 2442 "yy_parser.cpp"
    break;

  case 155: // type_qual_lst: type_qual
#line 439 "parser.y"
                                    { yylhs.value.as < AST_TypeQuals * > () = drv.ast->mkTypeQuals(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); SL(yylhs.value.as < AST_TypeQuals * > (), yylhs.location); }
#line 2448 "yy_parser.cpp"
    break;

  case 156: // type_qual_lst: type_qual_lst type_qual
#line 440 "parser.y"
                                    { yylhs.value.as < AST_TypeQuals * > () = yystack_[1].value.as < AST_TypeQuals * > ()->update(AST_TypeQuals::QualType(yystack_[0].value.as < short > ())); }
#line 2454 "yy_parser.cpp"
    break;

  case 157: // param_type_lst: param_lst
#line 444 "parser.y"
                                    { yylhs.value.as < AST_ParameterTypeList * > () = drv.ast->mkParamTypeLst(yystack_[0].value.as < AST_ParameterList * > (), false); SL(yylhs.value.as < AST_ParameterTypeList * > (), yylhs.location); }
#line 2460 "yy_parser.cpp"
    break;

  case 158: // param_type_lst: param_lst "," ELLIPSIS
#line 445 "parser.y"
                                    { yylhs.value.as < AST_ParameterTypeList * > () = drv.ast->mkParamTypeLst(yystack_[2].value.as < AST_ParameterList * > (), true); SL(yylhs.value.as < AST_ParameterTypeList * > (), yylhs.location); }
#line 2466 "yy_parser.cpp"
    break;

  case 159: // param_lst: parameter_declaration
#line 449 "parser.y"
                                                { yylhs.value.as < AST_ParameterList * > () = drv.ast->mkParamLst(yystack_[0].value.as < AST_ParameterDeclaration * > ()); SL(yylhs.value.as < AST_ParameterList * > (), yylhs.location); }
#line 2472 "yy_parser.cpp"
    break;

  case 160: // param_lst: param_lst "," parameter_declaration
#line 450 "parser.y"
                                                { yylhs.value.as < AST_ParameterList * > () = yystack_[2].value.as < AST_ParameterList * > ()->append(yystack_[0].value.as < AST_ParameterDeclaration * > ()); }
#line 2478 "yy_parser.cpp"
    break;

  case 161: // parameter_declaration: decl_specs declarator
#line 454 "parser.y"
                                                { yylhs.value.as < AST_ParameterDeclaration * > () = drv.ast->mkParamDecl(yystack_[1].value.as < AST_DeclSpecifiers * > (), yystack_[0].value.as < AST_Declarator * > ()); SL(yylhs.value.as < AST_ParameterDeclaration * > (), yylhs.location); }
#line 2484 "yy_parser.cpp"
    break;

  case 162: // parameter_declaration: decl_specs
#line 455 "parser.y"
                                                { yylhs.value.as < AST_ParameterDeclaration * > () = drv.ast->mkParamDecl(yystack_[0].value.as < AST_DeclSpecifiers * > (), nullptr); SL(yylhs.value.as < AST_ParameterDeclaration * > (), yylhs.location); }
#line 2490 "yy_parser.cpp"
    break;

  case 163: // type_name: spec_qual_lst
#line 459 "parser.y"
                                                { yylhs.value.as < AST_TypeName * > () = drv.ast->mkTypeName(yystack_[0].value.as < AST_SpecsQualsList * > (), nullptr); SL(yylhs.value.as < AST_TypeName * > (), yylhs.location); }
#line 2496 "yy_parser.cpp"
    break;

  case 164: // type_name: spec_qual_lst abstr_decltor
#line 460 "parser.y"
                                                { yylhs.value.as < AST_TypeName * > () = drv.ast->mkTypeName(yystack_[1].value.as < AST_SpecsQualsList * > (), yystack_[0].value.as < AST_AbstrDeclarator * > ()); SL(yylhs.value.as < AST_TypeName * > (), yylhs.location); }
#line 2502 "yy_parser.cpp"
    break;

  case 165: // abstr_decltor: dir_abstr_decltor
#line 464 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(yystack_[0].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2508 "yy_parser.cpp"
    break;

  case 166: // abstr_decltor: pointer dir_abstr_decltor
#line 465 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(yystack_[0].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2514 "yy_parser.cpp"
    break;

  case 167: // abstr_decltor: pointer
#line 466 "parser.y"
                                                { yylhs.value.as < AST_AbstrDeclarator * > () = drv.ast->mkAbstrDeclarator(nullptr, yystack_[0].value.as < AST_Pointer * > ()); SL(yylhs.value.as < AST_AbstrDeclarator * > (), yylhs.location); }
#line 2520 "yy_parser.cpp"
    break;

  case 168: // dir_abstr_decltor: "(" abstr_decltor ")"
#line 470 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclNested(yystack_[1].value.as < AST_AbstrDeclarator * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2526 "yy_parser.cpp"
    break;

  case 169: // dir_abstr_decltor: dir_abstr_decltor "[" "]"
#line 471 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclArr(yystack_[2].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2532 "yy_parser.cpp"
    break;

  case 170: // dir_abstr_decltor: dir_abstr_decltor "[" assign_expr "]"
#line 472 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclArr(yystack_[3].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2538 "yy_parser.cpp"
    break;

  case 171: // dir_abstr_decltor: dir_abstr_decltor "(" ")"
#line 473 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclFunc(yystack_[2].value.as < AST_DirAbstrDeclarator * > (), nullptr); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2544 "yy_parser.cpp"
    break;

  case 172: // dir_abstr_decltor: dir_abstr_decltor "(" param_type_lst ")"
#line 474 "parser.y"
                                                    { yylhs.value.as < AST_DirAbstrDeclarator * > () = drv.ast->mkDirAbstrDeclFunc(yystack_[3].value.as < AST_DirAbstrDeclarator * > (), yystack_[1].value.as < AST_ParameterTypeList * > ()); SL(yylhs.value.as < AST_DirAbstrDeclarator * > (), yylhs.location); }
#line 2550 "yy_parser.cpp"
    break;

  case 173: // initializer: assign_expr
#line 481 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[0].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2556 "yy_parser.cpp"
    break;

  case 174: // initializer: "{" init_lst "}"
#line 482 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[1].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2562 "yy_parser.cpp"
    break;

  case 175: // initializer: "{" init_lst "," "}"
#line 483 "parser.y"
                                                    { yylhs.value.as < AST_Initializer * > () = drv.ast->mkInitializer(yystack_[2].value.as < AST_InitializerList * > ()); SL(yylhs.value.as < AST_Initializer * > (), yylhs.location); }
#line 2568 "yy_parser.cpp"
    break;

  case 176: // init_lst: initializer
#line 487 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = drv.ast->mkInitializerLst(yystack_[0].value.as < AST_Initializer * > (), nullptr); SL(yylhs.value.as < AST_InitializerList * > (), yylhs.location); }
#line 2574 "yy_parser.cpp"
    break;

  case 177: // init_lst: designator "=" initializer
#line 488 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = drv.ast->mkInitializerLst(yystack_[0].value.as < AST_Initializer * > (), yystack_[2].value.as < AST_Designator * > ()); SL(yylhs.value.as < AST_InitializerList * > (), yylhs.location); }
#line 2580 "yy_parser.cpp"
    break;

  case 178: // init_lst: init_lst "," initializer
#line 489 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = yystack_[2].value.as < AST_InitializerList * > ()->append(yystack_[0].value.as < AST_Initializer * > (), nullptr); }
#line 2586 "yy_parser.cpp"
    break;

  case 179: // init_lst: init_lst "," designator "=" initializer
#line 490 "parser.y"
                                                    { yylhs.value.as < AST_InitializerList * > () = yystack_[4].value.as < AST_InitializerList * > ()->append(yystack_[0].value.as < AST_Initializer * > (), yystack_[2].value.as < AST_Designator * > ()); }
#line 2592 "yy_parser.cpp"
    break;

  case 180: // designator: "[" const_expr "]"
#line 494 "parser.y"
                                                    { yylhs.value.as < AST_Designator * > () = drv.ast->mkDesignator(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_Designator * > (), yylhs.location); }
#line 2598 "yy_parser.cpp"
    break;

  case 181: // designator: "." IDENTIFIER
#line 495 "parser.y"
                                                    { yylhs.value.as < AST_Designator * > () = drv.ast->mkDesignator(yystack_[0].value.as < string_id_t > ()); SL(yylhs.value.as < AST_Designator * > (), yylhs.location); }
#line 2604 "yy_parser.cpp"
    break;

  case 182: // stmt: label_stmt
#line 502 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_LabeledStmt * > (); }
#line 2610 "yy_parser.cpp"
    break;

  case 183: // stmt: compound_stmt
#line 503 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_CompoundStmt * > (); }
#line 2616 "yy_parser.cpp"
    break;

  case 184: // stmt: expr_stmt
#line 504 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_ExprStmt * > (); }
#line 2622 "yy_parser.cpp"
    break;

  case 185: // stmt: select_stmt
#line 505 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_SelectionStmt * > (); }
#line 2628 "yy_parser.cpp"
    break;

  case 186: // stmt: loop_stmt
#line 506 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_IterStmt * > (); }
#line 2634 "yy_parser.cpp"
    break;

  case 187: // stmt: jmp_stmt
#line 507 "parser.y"
                                    { yylhs.value.as < AST_Stmt * > () = yystack_[0].value.as < AST_JumpStmt * > (); }
#line 2640 "yy_parser.cpp"
    break;

  case 188: // label_stmt: IDENTIFIER ":" stmt
#line 511 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(yystack_[2].value.as < string_id_t > (), yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SIMPL); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2646 "yy_parser.cpp"
    break;

  case 189: // label_stmt: CASE const_expr ":" stmt
#line 512 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SW_CASE); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2652 "yy_parser.cpp"
    break;

  case 190: // label_stmt: DEFAULT ":" stmt
#line 513 "parser.y"
                                    { yylhs.value.as < AST_LabeledStmt * > () = drv.ast->mkLabelStmt(nullptr, yystack_[0].value.as < AST_Stmt * > (), AST_LabeledStmt::SW_DEFAULT); SL(yylhs.value.as < AST_LabeledStmt * > (), yylhs.location); }
#line 2658 "yy_parser.cpp"
    break;

  case 191: // compound_stmt: "{" "}"
#line 517 "parser.y"
                                    { yylhs.value.as < AST_CompoundStmt * > () = drv.ast->mkCompoundStmt(new AST_BlockItemList()); SL(yylhs.value.as < AST_CompoundStmt * > (), yylhs.location); }
#line 2664 "yy_parser.cpp"
    break;

  case 192: // compound_stmt: "{" block_items "}"
#line 518 "parser.y"
                                    { yylhs.value.as < AST_CompoundStmt * > () = drv.ast->mkCompoundStmt(yystack_[1].value.as < AST_BlockItemList * > ()); SL(yylhs.value.as < AST_CompoundStmt * > (), yylhs.location); }
#line 2670 "yy_parser.cpp"
    break;

  case 193: // block_items: declaration
#line 522 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = drv.ast->mkBlockItemLst()->append(yystack_[0].value.as < AST_Declaration * > ()); SL(yylhs.value.as < AST_BlockItemList * > (), yylhs.location); }
#line 2676 "yy_parser.cpp"
    break;

  case 194: // block_items: stmt
#line 523 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = drv.ast->mkBlockItemLst()->append(yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_BlockItemList * > (), yylhs.location); }
#line 2682 "yy_parser.cpp"
    break;

  case 195: // block_items: block_items declaration
#line 524 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = yystack_[1].value.as < AST_BlockItemList * > ()->append(yystack_[0].value.as < AST_Declaration * > ()); }
#line 2688 "yy_parser.cpp"
    break;

  case 196: // block_items: block_items stmt
#line 525 "parser.y"
                                    { yylhs.value.as < AST_BlockItemList * > () = yystack_[1].value.as < AST_BlockItemList * > ()->append(yystack_[0].value.as < AST_Stmt * > ()); }
#line 2694 "yy_parser.cpp"
    break;

  case 197: // expr_stmt: ";"
#line 529 "parser.y"
                                    { yylhs.value.as < AST_ExprStmt * > () = drv.ast->mkExprStmt(nullptr); SL(yylhs.value.as < AST_ExprStmt * > (), yylhs.location); }
#line 2700 "yy_parser.cpp"
    break;

  case 198: // expr_stmt: expr ";"
#line 530 "parser.y"
                                    { yylhs.value.as < AST_ExprStmt * > () = drv.ast->mkExprStmt(yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_ExprStmt * > (), yylhs.location); }
#line 2706 "yy_parser.cpp"
    break;

  case 199: // select_stmt: IF "(" expr ")" stmt
#line 534 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkIfStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > (), nullptr); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2712 "yy_parser.cpp"
    break;

  case 200: // select_stmt: IF "(" expr ")" stmt ELSE stmt
#line 535 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkIfStmt(yystack_[4].value.as < AST_Expr * > (), yystack_[2].value.as < AST_Stmt * > (), yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2718 "yy_parser.cpp"
    break;

  case 201: // select_stmt: SWITCH "(" expr ")" stmt
#line 536 "parser.y"
                                            { yylhs.value.as < AST_SelectionStmt * > () = drv.ast->mkSwitchStmt(yystack_[2].value.as < AST_Expr * > (), yystack_[0].value.as < AST_Stmt * > ()); SL(yylhs.value.as < AST_SelectionStmt * > (), yylhs.location); }
#line 2724 "yy_parser.cpp"
    break;

  case 202: // loop_stmt: WHILE "(" expr ")" stmt
#line 540 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeWhileStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[2].value.as < AST_Expr * > (), false); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2730 "yy_parser.cpp"
    break;

  case 203: // loop_stmt: DO stmt WHILE "(" expr ")" ";"
#line 541 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeWhileStmt(yystack_[5].value.as < AST_Stmt * > (), yystack_[2].value.as < AST_Expr * > (), true); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2736 "yy_parser.cpp"
    break;

  case 204: // loop_stmt: FOR "(" expr_stmt expr_stmt ")" stmt
#line 542 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_ExprStmt * > (), nullptr); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2742 "yy_parser.cpp"
    break;

  case 205: // loop_stmt: FOR "(" expr_stmt expr_stmt expr ")" stmt
#line 543 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[4].value.as < AST_ExprStmt * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2748 "yy_parser.cpp"
    break;

  case 206: // loop_stmt: FOR "(" declaration expr_stmt ")" stmt
#line 544 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[3].value.as < AST_Declaration * > (), yystack_[2].value.as < AST_ExprStmt * > (), nullptr); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2754 "yy_parser.cpp"
    break;

  case 207: // loop_stmt: FOR "(" declaration expr_stmt expr ")" stmt
#line 545 "parser.y"
                                                    { yylhs.value.as < AST_IterStmt * > () = drv.ast->makeForStmt(yystack_[0].value.as < AST_Stmt * > (), yystack_[4].value.as < AST_Declaration * > (), yystack_[3].value.as < AST_ExprStmt * > (), yystack_[2].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_IterStmt * > (), yylhs.location); }
#line 2760 "yy_parser.cpp"
    break;

  case 208: // jmp_stmt: GOTO IDENTIFIER ";"
#line 549 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_GOTO, yystack_[1].value.as < string_id_t > ()); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2766 "yy_parser.cpp"
    break;

  case 209: // jmp_stmt: CONTINUE ";"
#line 550 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_CONTINUE); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2772 "yy_parser.cpp"
    break;

  case 210: // jmp_stmt: BREAK ";"
#line 551 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_BREAK); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2778 "yy_parser.cpp"
    break;

  case 211: // jmp_stmt: RETURN ";"
#line 552 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2784 "yy_parser.cpp"
    break;

  case 212: // jmp_stmt: RETURN expr ";"
#line 553 "parser.y"
                                { yylhs.value.as < AST_JumpStmt * > () = drv.ast->mkJumpStmt(AST_JumpStmt::J_RET, yystack_[1].value.as < AST_Expr * > ()); SL(yylhs.value.as < AST_JumpStmt * > (), yylhs.location); }
#line 2790 "yy_parser.cpp"
    break;

  case 213: // trans_unit: func_def
#line 560 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = drv.ast->mkTransUnit()->append(yystack_[0].value.as < AST_FunctionDef * > ()); SL(yylhs.value.as < AST_TranslationUnit * > (), yylhs.location); }
#line 2796 "yy_parser.cpp"
    break;

  case 214: // trans_unit: declaration
#line 561 "parser.y"
                                    {
                                        yylhs.value.as < AST_TranslationUnit * > () = drv.ast->mkTransUnit()->append(yystack_[0].value.as < AST_Declaration * > ());
                                        check_typedef(yystack_[0].value.as < AST_Declaration * > ());
                                        SL(yylhs.value.as < AST_TranslationUnit * > (), yylhs.location);
                                    }
#line 2806 "yy_parser.cpp"
    break;

  case 215: // trans_unit: trans_unit func_def
#line 566 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = yystack_[1].value.as < AST_TranslationUnit * > ()->append(yystack_[0].value.as < AST_FunctionDef * > ()); }
#line 2812 "yy_parser.cpp"
    break;

  case 216: // trans_unit: trans_unit declaration
#line 567 "parser.y"
                                    { yylhs.value.as < AST_TranslationUnit * > () = yystack_[1].value.as < AST_TranslationUnit * > ()->append(yystack_[0].value.as < AST_Declaration * > ()); check_typedef(yystack_[0].value.as < AST_Declaration * > ()); }
#line 2818 "yy_parser.cpp"
    break;

  case 217: // func_def: decl_specs declarator compound_stmt
#line 571 "parser.y"
                                                { yylhs.value.as < AST_FunctionDef * > () = drv.ast->mkFunDef(yystack_[2].value.as < AST_DeclSpecifiers * > (), yystack_[1].value.as < AST_Declarator * > (), yystack_[0].value.as < AST_CompoundStmt * > ()); SL(yylhs.value.as < AST_FunctionDef * > (), yylhs.location); }
#line 2824 "yy_parser.cpp"
    break;


#line 2828 "yy_parser.cpp"

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
  "CHAR", "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE",
  "CONST", "VOLATILE", "VOID", "STRUCT", "UNION", "ENUM", "ELLIPSIS",
  "CASE", "DEFAULT", "IF", "SWITCH", "WHILE", "DO", "FOR", "GOTO",
  "CONTINUE", "BREAK", "RETURN", "IFX", "ELSE", "IDENTIFIER", "TYPE_NAME",
  "CONSTANT", "STRING_LITERAL", "$accept", "primary_expr", "strings_seq",
  "postfix_expr", "arg_expr_lst", "unary_expr", "unary_operator",
  "cast_expr", "mult_expr", "add_expr", "shift_expr", "rel_expr",
  "eq_expr", "and_expr", "xor_expr", "or_expr", "land_expr", "lor_expr",
  "ternary_expr", "assign_expr", "assign_op", "expr", "const_expr",
  "decl_specs", "storage_class_specifier", "type_specifier", "type_qual",
  "function_specifier", "ustruct_spec", "ustruct", "struct_decls_lst",
  "struct_declaration", "spec_qual_lst", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "declaration", "init_decltor_list", "init_decltor", "declarator",
  "pointer", "dir_decltor", "type_qual_lst", "param_type_lst", "param_lst",
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


  const short parser::yypact_ninf_ = -281;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
    1245,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,    17,  -281,    37,  1245,  1245,  1245,  1245,  -281,    19,
    -281,  -281,   260,  -281,   -67,    38,  -281,    39,    16,  -281,
      80,  -281,    74,    20,   204,  -281,  -281,  -281,  -281,  1260,
      46,  -281,  -281,  -281,    14,   101,  -281,   -67,    49,  -281,
    -281,    16,  -281,    39,   332,   179,  -281,   204,  1111,   865,
    1260,  1260,   764,  -281,    36,  1260,  1030,  -281,     8,   119,
    -281,  -281,  -281,  -281,    58,  -281,  -281,   841,  -281,  -281,
    -281,  -281,  -281,  -281,  1051,  1051,  1072,  1030,    84,   178,
     214,   217,   570,   219,   146,   228,   231,   587,   232,  -281,
    -281,  -281,   156,   118,   253,  1030,  -281,   190,   123,   125,
      41,   128,   240,   235,   230,   226,    10,  -281,  -281,   111,
      37,  -281,  -281,  -281,  -281,   417,  -281,  -281,  -281,  -281,
     668,  -281,  -281,  -281,  -281,    39,   257,   262,  -281,  -281,
     258,   925,  -281,  -281,  -281,  -281,  1030,   113,  -281,   264,
     779,  -281,  -281,  -281,  -281,  -281,  -281,    11,    78,    25,
     259,   841,  -281,  -281,   841,  -281,   270,   570,  1030,  1030,
    1030,   208,   496,   276,  -281,  -281,  -281,   148,   570,  -281,
     946,  1030,   196,   201,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  1030,  -281,  1030,
    1030,  1030,  1030,  1030,  1030,  1030,  1030,  1030,  1030,  1030,
    1030,  1030,  1030,  1030,  1030,  1030,  1030,  1030,  -281,  1030,
    -281,  -281,  -281,  1030,   218,  -281,   186,   279,  -281,  -281,
    1208,  -281,  -281,   277,  -281,  -281,    36,  1030,  -281,  -281,
    -281,    25,   292,  -281,   205,   731,   293,   294,   570,  -281,
      79,   124,   126,   296,   605,   605,  -281,  -281,  -281,  -281,
     127,  -281,    63,  -281,  -281,  -281,  -281,  -281,  -281,   190,
     190,   123,   123,   125,   125,   125,   125,    41,    41,   128,
     240,   235,   230,   199,   226,  -281,   295,  -281,  -281,   689,
     179,  -281,  -281,  -281,  -281,  -281,   320,   205,  1171,   967,
     668,  -281,   302,   302,  -281,   570,   570,   570,  1030,   988,
    1009,  1030,  -281,  -281,  1030,  -281,  -281,  -281,   323,  -281,
    -281,  -281,   322,  -281,   321,   216,   251,  -281,  -281,   129,
     570,   142,   570,   143,  -281,  -281,   179,  -281,  -281,  -281,
     710,   570,   335,  -281,   570,  -281,   570,  -281,  -281,  -281,
    -281,  -281,  -281
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,    88,    89,    90,    91,    92,   108,   106,    94,    95,
      96,    97,   100,   101,    98,    99,   105,   107,    93,   112,
     113,     0,   104,     0,    80,    82,    84,    86,   102,     0,
     103,   214,     0,   213,     0,   130,   135,     0,   143,   147,
       0,   137,   139,     0,   142,    81,    83,    85,    87,     0,
     111,     1,   216,   215,   133,     0,   131,     0,     0,   155,
     145,   144,   136,     0,     0,     0,   217,   141,     0,     0,
     118,   120,     0,   114,     0,     0,     0,   126,     0,     0,
     148,   156,   146,   138,   139,   197,   191,     0,    26,    31,
      30,    29,    28,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     3,
       8,    10,     4,    20,    32,     0,    34,    38,    41,    44,
      49,    52,    54,    56,    58,    60,    62,    64,    77,     0,
       0,   193,   194,   182,   183,     0,   184,   185,   186,   187,
       0,     2,   173,   140,   154,   162,     0,   157,   159,   152,
       0,     0,   117,   119,   110,   115,     0,     0,   121,   123,
       0,    32,    79,   134,   128,   132,   127,     0,     0,   163,
       0,     0,    21,    22,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,   209,   210,   211,     0,     0,     9,
       0,     0,     0,     0,    16,    17,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   198,     0,
     192,   195,   196,     0,     0,   176,     0,     0,   161,   153,
       0,   151,   150,     0,   124,   116,     0,     0,   109,   129,
       5,     0,   167,   164,   165,     0,     0,     0,     0,   190,
       0,     0,     0,     0,     0,     0,   208,   212,   188,    12,
       0,    18,     0,    14,    15,    65,    35,    36,    37,    40,
      39,    42,    43,    45,    46,    47,    48,    50,    51,    53,
      55,    57,    59,     0,    61,    78,     0,   181,   174,     0,
       0,   158,   160,   149,   122,   125,     0,   166,     0,     0,
       0,    33,     0,    25,   189,     0,     0,     0,     0,     0,
       0,     0,    13,    11,     0,   180,   175,   178,     0,   177,
     168,   171,     0,   169,     0,     0,   199,   201,   202,     0,
       0,     0,     0,     0,    19,    63,     0,   172,   170,     6,
       0,     0,     0,   206,     0,   204,     0,   179,     7,   200,
     203,   207,   205
  };

  const short
  parser::yypgoto_[] =
  {
    -281,  -281,  -281,  -281,  -281,   -43,  -281,  -113,    12,    18,
     -16,    27,   117,   120,   116,   130,   115,  -281,   -48,   -64,
    -281,   -69,   -89,     0,  -281,   -10,   203,  -281,  -281,  -281,
     269,   -60,   -56,  -281,   106,  -281,   299,   -68,   -23,  -281,
     290,   -17,   -31,   314,   289,    51,  -281,   122,    15,   114,
     112,   -62,    56,  -280,   -98,  -281,   325,  -281,  -171,  -281,
    -281,  -281,  -281,   336
  };

  const short
  parser::yydefgoto_[] =
  {
       0,   111,   112,   113,   270,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     207,   129,   163,   130,    24,    25,    26,    27,    28,    29,
      72,    73,    74,   157,   158,    30,    55,    56,    31,    40,
      41,    84,    43,    44,    61,   146,   147,   148,   170,   253,
     254,   235,   236,   237,   132,   133,   134,   135,   136,   137,
     138,   139,    32,    33
  };

  const short
  parser::yytable_[] =
  {
      23,   142,   208,   143,   181,   150,    42,    60,   176,    52,
     165,   265,   155,   164,   152,   153,   249,    54,   168,   328,
      58,    34,    76,    49,    45,    46,    47,    48,   162,    37,
      82,   169,    23,   161,   251,    38,   226,   232,   187,    70,
      36,   131,    57,   156,    38,    37,    37,   227,    37,   162,
      75,   172,   173,   175,   161,    38,    38,   159,    38,    80,
      70,    70,    70,   216,   217,    70,    65,   244,   145,   229,
     328,     7,   161,   218,   219,   323,   142,    70,    64,   259,
      16,    17,    65,    62,   229,   229,    63,   243,   250,   315,
     268,   177,    54,   319,   320,    54,   276,   277,   278,   165,
     155,    35,   168,    50,    39,   168,    77,    78,   162,   260,
     261,   262,   231,   161,   228,   169,   245,   229,   169,   246,
      39,    39,   272,    39,   166,   167,   271,   190,   238,   191,
     229,   192,   229,   321,   316,   229,   317,   322,   252,   352,
     212,   213,   311,   275,   296,   193,   194,   195,   229,   229,
      70,   267,   354,   356,   229,   214,   215,   293,   305,   264,
     314,    70,   220,   221,    70,   295,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   140,   161,   162,   256,   178,    87,   257,
     161,   298,   299,    88,    89,    90,    91,    92,    93,   162,
     283,   284,   285,   286,   161,   229,   324,    94,    95,   209,
     210,   211,   161,    68,   308,    69,   309,   336,   337,   338,
     252,   349,   350,   179,   279,   280,   180,    96,   182,   159,
     183,   184,   281,   282,   185,   142,   142,   327,   329,   188,
     145,    59,   353,   189,   355,   334,   142,   287,   288,   339,
     341,   343,    71,   359,   222,   224,   361,   344,   362,   223,
      51,   196,   225,   141,    81,   109,   110,   239,   240,   255,
     241,   247,    59,    71,    71,    71,   345,   258,    71,   266,
     273,   161,   142,   263,   357,   274,   142,   300,   327,   303,
      71,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   251,   297,   312,   313,   318,   310,   325,   145,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
     330,   346,   347,   348,   351,    85,    64,    86,   360,   289,
     291,    87,   294,   290,   160,    22,    88,    89,    90,    91,
      92,    93,   304,    83,    81,   292,    79,    67,   151,   332,
      94,    95,   302,    71,   307,   306,   335,    66,    53,     0,
       0,     0,     0,     0,    71,     0,     0,    71,     0,     0,
      96,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     0,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,     0,   108,    22,   109,   110,
      85,    64,   230,     0,     0,     0,    87,     0,     0,     0,
       0,    88,    89,    90,    91,    92,    93,     0,     0,     0,
       0,     0,     0,     0,     0,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    96,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,     0,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,    85,
       0,   108,    22,   109,   110,    87,     0,     0,     0,     0,
      88,    89,    90,    91,    92,    93,     0,     0,     0,     0,
       0,     0,     0,     0,    94,    95,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    96,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     0,     0,     0,     0,
       0,     0,     0,    85,    64,     0,     0,     0,     0,    87,
     141,    22,   109,   110,    88,    89,    90,    91,    92,    93,
     186,     0,     0,     0,     0,     0,    87,     0,    94,    95,
       0,    88,    89,    90,    91,    92,    93,     0,    85,     0,
       0,     0,     0,     0,    87,    94,    95,     0,    96,    88,
      89,    90,    91,    92,    93,     0,     0,     0,     0,     0,
       0,     0,     0,    94,    95,    96,     0,     0,     0,     0,
       0,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,     0,    96,   108,     0,   109,   110,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,   140,   109,   110,     0,     0,    87,     0,   233,
       0,   234,    88,    89,    90,    91,    92,    93,     0,   141,
       0,   109,   110,   140,   326,     0,    94,    95,    87,     0,
     233,     0,   234,    88,    89,    90,    91,    92,    93,     0,
       0,     0,     0,     0,   140,   358,    96,    94,    95,    87,
       0,   233,     0,   234,    88,    89,    90,    91,    92,    93,
       0,     0,     0,     0,     0,   310,     0,    96,    94,    95,
      87,     0,     0,     0,     0,    88,    89,    90,    91,    92,
      93,     0,   141,     0,   109,   110,     0,     0,    96,    94,
      95,     0,     0,     0,     0,     0,     0,     0,     0,   154,
       0,     0,     0,   141,     0,   109,   110,     0,     0,    96,
       0,     0,     0,     0,   248,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   141,     0,   109,   110,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   141,     0,   109,   110,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      87,     0,     0,     0,     0,    88,    89,    90,    91,    92,
      93,     0,     0,     0,    22,     0,     0,     0,     0,    94,
      95,     0,     0,     0,    87,     0,     0,   149,     0,    88,
      89,    90,    91,    92,    93,     0,     0,     0,     0,    96,
       0,     0,     0,    94,    95,     0,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,     0,     0,    96,     0,     0,     0,     0,     0,     0,
       7,     0,     0,     0,     0,   141,    22,   109,   110,    16,
      17,     0,     0,     0,    87,     0,     0,   242,     0,    88,
      89,    90,    91,    92,    93,     0,     0,     0,     0,   141,
       0,   109,   110,    94,    95,    87,   269,     0,     0,     0,
      88,    89,    90,    91,    92,    93,     0,     0,     0,     0,
       0,     0,     0,    96,    94,    95,    87,     0,     0,   333,
       7,    88,    89,    90,    91,    92,    93,     0,     0,    16,
      17,     0,     0,     0,    96,    94,    95,    87,   340,     0,
       0,     0,    88,    89,    90,    91,    92,    93,     0,   141,
       0,   109,   110,     0,     0,    96,    94,    95,    87,   342,
       0,     0,     0,    88,    89,    90,    91,    92,    93,     0,
     141,     0,   109,   110,     0,     0,    96,    94,    95,    87,
       0,     0,     0,     0,    88,    89,    90,    91,    92,    93,
       0,   141,     0,   109,   110,     0,     0,    96,    94,    95,
     171,     0,     0,     0,     0,    88,    89,    90,    91,    92,
      93,     0,   141,     0,   109,   110,     0,     0,    96,    94,
      95,   174,     0,     0,     0,     0,    88,    89,    90,    91,
      92,    93,     0,   141,     0,   109,   110,     0,     0,    96,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   141,     0,   109,   110,     0,     0,
      96,   144,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   141,     0,   109,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   141,     0,   109,   110,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,   331,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22
  };

  const short
  parser::yycheck_[] =
  {
       0,    65,   115,    65,   102,    69,    23,    38,    97,    32,
      78,   182,    72,     5,    70,    71,     5,    84,    87,   299,
      37,     4,     8,     4,    24,    25,    26,    27,    76,     9,
      61,    87,    32,    76,     9,    19,    26,   135,   107,    49,
       3,    64,     4,     7,    19,     9,     9,    37,     9,    97,
       4,    94,    95,    96,    97,    19,    19,    74,    19,    10,
      70,    71,    72,    22,    23,    75,     8,   156,    68,     6,
     350,    55,   115,    32,    33,    12,   140,    87,     4,   177,
      64,    65,     8,     3,     6,     6,     6,   151,    10,    10,
     188,     7,    84,   264,   265,    84,   209,   210,   211,   167,
     160,    84,   171,    84,    84,   174,     5,     6,   156,   178,
     179,   180,   135,   156,     3,   171,     3,     6,   174,     6,
      84,    84,   191,    84,     5,     6,   190,     9,   145,    11,
       6,    13,     6,     6,    10,     6,    10,    10,   169,    10,
      17,    18,   255,   207,   233,    27,    28,    29,     6,     6,
     160,     3,    10,    10,     6,    30,    31,   226,   247,   182,
     258,   171,    34,    35,   174,   229,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,     4,   227,   233,   171,     9,     9,   174,
     233,     5,     6,    14,    15,    16,    17,    18,    19,   247,
     216,   217,   218,   219,   247,     6,     7,    28,    29,    19,
      20,    21,   255,     9,     9,    11,    11,   315,   316,   317,
     251,     5,     6,     9,   212,   213,     9,    48,     9,   246,
      84,     3,   214,   215,     3,   299,   300,   299,   300,     7,
     240,    38,   340,    87,   342,   309,   310,   220,   221,   318,
     319,   320,    49,   351,    14,    25,   354,   321,   356,    24,
       0,     8,    36,    84,    61,    86,    87,    10,     6,    10,
      12,     7,    69,    70,    71,    72,   324,     7,    75,     3,
      84,   324,   346,    75,   346,    84,   350,     8,   350,    12,
      87,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,     9,    84,    10,    10,     9,     4,    12,   308,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      10,     8,    10,    12,    83,     3,     4,     5,     3,   222,
     224,     9,   227,   223,    75,    85,    14,    15,    16,    17,
      18,    19,   246,    63,   151,   225,    57,    43,    69,   308,
      28,    29,   240,   160,   252,   251,   310,    42,    32,    -1,
      -1,    -1,    -1,    -1,   171,    -1,    -1,   174,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    -1,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    84,    85,    86,    87,
       3,     4,     5,    -1,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,     3,
      -1,    84,    85,    86,    87,     9,    -1,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,     9,
      84,    85,    86,    87,    14,    15,    16,    17,    18,    19,
       3,    -1,    -1,    -1,    -1,    -1,     9,    -1,    28,    29,
      -1,    14,    15,    16,    17,    18,    19,    -1,     3,    -1,
      -1,    -1,    -1,    -1,     9,    28,    29,    -1,    48,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    48,    -1,    -1,    -1,    -1,
      -1,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    48,    84,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,     4,    86,    87,    -1,    -1,     9,    -1,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    -1,    84,
      -1,    86,    87,     4,     5,    -1,    28,    29,     9,    -1,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    -1,    -1,     4,     5,    48,    28,    29,     9,
      -1,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    -1,    -1,     4,    -1,    48,    28,    29,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    -1,    84,    -1,    86,    87,    -1,    -1,    48,    28,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,
      -1,    -1,    -1,    84,    -1,    86,    87,    -1,    -1,    48,
      -1,    -1,    -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    86,    87,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    85,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    85,    -1,    -1,    -1,    -1,    28,
      29,    -1,    -1,    -1,     9,    -1,    -1,    12,    -1,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    28,    29,    -1,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    64,
      65,    -1,    -1,    -1,     9,    -1,    -1,    12,    -1,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    84,
      -1,    86,    87,    28,    29,     9,    10,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    28,    29,     9,    -1,    -1,    12,
      55,    14,    15,    16,    17,    18,    19,    -1,    -1,    64,
      65,    -1,    -1,    -1,    48,    28,    29,     9,    10,    -1,
      -1,    -1,    14,    15,    16,    17,    18,    19,    -1,    84,
      -1,    86,    87,    -1,    -1,    48,    28,    29,     9,    10,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    -1,
      84,    -1,    86,    87,    -1,    -1,    48,    28,    29,     9,
      -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      -1,    84,    -1,    86,    87,    -1,    -1,    48,    28,    29,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    -1,    84,    -1,    86,    87,    -1,    -1,    48,    28,
      29,     9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,
      18,    19,    -1,    84,    -1,    86,    87,    -1,    -1,    48,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    86,    87,    -1,    -1,
      48,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    86,    87,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    85,   111,   112,   113,   114,   115,   116,   117,
     123,   126,   150,   151,     4,    84,     3,     9,    19,    84,
     127,   128,   129,   130,   131,   111,   111,   111,   111,     4,
      84,     0,   126,   151,    84,   124,   125,     4,   129,   114,
     130,   132,     3,     6,     4,     8,   144,   131,     9,    11,
     113,   114,   118,   119,   120,     4,     8,     5,     6,   124,
      10,   114,   130,   128,   129,     3,     5,     9,    14,    15,
      16,    17,    18,    19,    28,    29,    48,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    84,    86,
      87,    89,    90,    91,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   109,
     111,   126,   142,   143,   144,   145,   146,   147,   148,   149,
       4,    84,   107,   139,    10,   111,   133,   134,   135,    12,
     107,   132,   120,   120,     5,   119,     7,   121,   122,   129,
     118,    93,   106,   110,     5,   125,     5,     6,   109,   120,
     136,     9,    93,    93,     9,    93,   110,     7,     9,     9,
       9,   142,     9,    84,     3,     3,     3,   109,     7,    87,
       9,    11,    13,    27,    28,    29,     8,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,   108,    95,    19,
      20,    21,    17,    18,    30,    31,    22,    23,    32,    33,
      34,    35,    14,    24,    25,    36,    26,    37,     3,     6,
       5,   126,   142,    11,    13,   139,   140,   141,   129,    10,
       6,    12,    12,   107,   110,     3,     6,     7,     5,     5,
      10,     9,   130,   137,   138,    10,   136,   136,     7,   142,
     109,   109,   109,    75,   126,   146,     3,     3,   142,    10,
      92,   107,   109,    84,    84,   107,    95,    95,    95,    96,
      96,    97,    97,    98,    98,    98,    98,    99,    99,   100,
     101,   102,   103,   109,   104,   107,   110,    84,     5,     6,
       8,    70,   135,    12,   122,   110,   137,   138,     9,    11,
       4,    95,    10,    10,   142,    10,    10,    10,     9,   146,
     146,     6,    10,    12,     7,    12,     5,   139,   141,   139,
      10,    10,   133,    12,   107,   140,   142,   142,   142,   109,
      10,   109,    10,   109,   107,   106,     8,    10,    12,     5,
       6,    83,    10,   142,    10,   142,    10,   139,     5,   142,
       3,   142,   142
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    88,    89,    89,    89,    89,    89,    89,    90,    90,
      91,    91,    91,    91,    91,    91,    91,    91,    92,    92,
      93,    93,    93,    93,    93,    93,    94,    94,    94,    94,
      94,    94,    95,    95,    96,    96,    96,    96,    97,    97,
      97,    98,    98,    98,    99,    99,    99,    99,    99,   100,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   109,   109,   110,
     111,   111,   111,   111,   111,   111,   111,   111,   112,   112,
     112,   112,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   114,   114,   114,   115,   116,
     116,   116,   117,   117,   118,   118,   119,   120,   120,   120,
     120,   121,   121,   122,   122,   122,   123,   123,   123,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   127,   128,
     128,   129,   129,   130,   130,   130,   130,   131,   131,   131,
     131,   131,   131,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   137,   137,   137,   138,   138,
     138,   138,   138,   139,   139,   139,   140,   140,   140,   140,
     141,   141,   142,   142,   142,   142,   142,   142,   143,   143,
     143,   144,   144,   145,   145,   145,   145,   146,   146,   147,
     147,   147,   148,   148,   148,   148,   148,   148,   149,   149,
     149,   149,   149,   150,   150,   150,   150,   151
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
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       4,     2,     1,     1,     1,     2,     3,     2,     1,     2,
       1,     1,     3,     1,     2,     3,     4,     5,     5,     6,
       2,     1,     3,     1,     3,     2,     3,     1,     3,     1,
       3,     2,     1,     1,     2,     2,     3,     1,     3,     5,
       4,     4,     3,     4,     3,     1,     2,     1,     3,     1,
       3,     2,     1,     1,     2,     1,     2,     1,     3,     3,
       4,     3,     4,     1,     3,     4,     1,     3,     3,     5,
       3,     2,     1,     1,     1,     1,     1,     1,     3,     4,
       3,     2,     3,     1,     1,     2,     2,     1,     2,     5,
       7,     5,     5,     7,     6,     7,     6,     7,     3,     2,
       2,     2,     3,     1,     1,     2,     2,     3
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   152,   152,   153,   154,   155,   156,   157,   161,   162,
     166,   167,   168,   169,   170,   171,   172,   173,   177,   178,
     182,   183,   184,   185,   186,   187,   191,   192,   193,   194,
     195,   196,   200,   201,   205,   206,   207,   208,   212,   213,
     214,   218,   219,   220,   224,   225,   226,   227,   228,   232,
     233,   234,   238,   239,   243,   244,   248,   249,   253,   254,
     258,   259,   263,   264,   268,   269,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   287,   288,   292,
     298,   299,   300,   301,   302,   303,   304,   305,   309,   310,
     311,   312,   313,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   332,   333,   334,   338,   342,
     343,   344,   348,   349,   353,   354,   358,   362,   363,   364,
     365,   369,   370,   374,   375,   376,   380,   381,   382,   383,
     384,   388,   389,   393,   394,   401,   402,   406,   407,   411,
     412,   416,   417,   421,   422,   423,   424,   428,   429,   430,
     431,   432,   433,   434,   435,   439,   440,   444,   445,   449,
     450,   454,   455,   459,   460,   464,   465,   466,   470,   471,
     472,   473,   474,   481,   482,   483,   487,   488,   489,   490,
     494,   495,   502,   503,   504,   505,   506,   507,   511,   512,
     513,   517,   518,   522,   523,   524,   525,   529,   530,   534,
     535,   536,   540,   541,   542,   543,   544,   545,   549,   550,
     551,   552,   553,   560,   561,   566,   567,   571
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
#line 3854 "yy_parser.cpp"

#line 574 "parser.y"


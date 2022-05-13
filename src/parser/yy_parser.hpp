// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file yy_parser.hpp
 ** Define the yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_YY_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 9 "parser.y"

    #include "ast.hpp"
    class CoreDriver;
    using yyscan_t = void*;

#line 55 "yy_parser.hpp"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "yy_location.hpp"
#include <typeinfo>
#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

namespace yy {
#line 195 "yy_parser.hpp"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {
      YY_ASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // abstr_decltor
      char dummy1[sizeof (AST_AbstrDeclarator *)];

      // arg_expr_lst
      char dummy2[sizeof (AST_ArgumentsList *)];

      // block_items
      char dummy3[sizeof (AST_BlockItemList *)];

      // compound_stmt
      char dummy4[sizeof (AST_CompoundStmt *)];

      // decl_specs
      char dummy5[sizeof (AST_DeclSpecifiers *)];

      // declaration
      char dummy6[sizeof (AST_Declaration *)];

      // declarator
      char dummy7[sizeof (AST_Declarator *)];

      // designator
      char dummy8[sizeof (AST_Designator *)];

      // dir_abstr_decltor
      char dummy9[sizeof (AST_DirAbstrDeclarator *)];

      // dir_decltor
      char dummy10[sizeof (AST_DirDeclarator *)];

      // enum_specifier
      char dummy11[sizeof (AST_EnumSpecifier *)];

      // enumerator
      char dummy12[sizeof (AST_Enumerator *)];

      // enumerator_list
      char dummy13[sizeof (AST_EnumeratorList *)];

      // primary_expr
      // postfix_expr
      // unary_expr
      // cast_expr
      // mult_expr
      // add_expr
      // shift_expr
      // rel_expr
      // eq_expr
      // and_expr
      // xor_expr
      // or_expr
      // land_expr
      // lor_expr
      // ternary_expr
      // assign_expr
      // expr
      // const_expr
      char dummy14[sizeof (AST_Expr *)];

      // expr_stmt
      char dummy15[sizeof (AST_ExprStmt *)];

      // func_def
      char dummy16[sizeof (AST_FunctionDef *)];

      // init_decltor
      char dummy17[sizeof (AST_InitDeclarator *)];

      // init_decltor_list
      char dummy18[sizeof (AST_InitDeclaratorList *)];

      // initializer
      char dummy19[sizeof (AST_Initializer *)];

      // init_lst
      char dummy20[sizeof (AST_InitializerList *)];

      // loop_stmt
      char dummy21[sizeof (AST_IterStmt *)];

      // jmp_stmt
      char dummy22[sizeof (AST_JumpStmt *)];

      // label_stmt
      char dummy23[sizeof (AST_LabeledStmt *)];

      // CONSTANT
      char dummy24[sizeof (AST_Literal)];

      // parameter_declaration
      char dummy25[sizeof (AST_ParameterDeclaration *)];

      // param_lst
      char dummy26[sizeof (AST_ParameterList *)];

      // param_type_lst
      char dummy27[sizeof (AST_ParameterTypeList *)];

      // pointer
      char dummy28[sizeof (AST_Pointer *)];

      // select_stmt
      char dummy29[sizeof (AST_SelectionStmt *)];

      // spec_qual_lst
      char dummy30[sizeof (AST_SpecsQualsList *)];

      // stmt
      char dummy31[sizeof (AST_Stmt *)];

      // strings_seq
      char dummy32[sizeof (AST_StringsSeq *)];

      // struct_declaration
      char dummy33[sizeof (AST_StructDeclaration *)];

      // struct_decls_lst
      char dummy34[sizeof (AST_StructDeclarationList *)];

      // struct_declarator
      char dummy35[sizeof (AST_StructDeclarator *)];

      // struct_declarator_list
      char dummy36[sizeof (AST_StructDeclaratorList *)];

      // trans_unit
      char dummy37[sizeof (AST_TranslationUnit *)];

      // type_name
      char dummy38[sizeof (AST_TypeName *)];

      // type_qual_lst
      char dummy39[sizeof (AST_TypeQuals *)];

      // type_specifier
      char dummy40[sizeof (AST_TypeSpecifier *)];

      // ustruct_spec
      char dummy41[sizeof (AST_UStructSpec *)];

      // unary_operator
      // assign_op
      // storage_class_specifier
      // type_qual
      // function_specifier
      // ustruct
      char dummy42[sizeof (short)];

      // IDENTIFIER
      // TYPE_NAME
      // STRING_LITERAL
      char dummy43[sizeof (string_id_t)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        CORE_TOK_YYEMPTY = -2,
    CORE_TOK_YYEOF = 0,            // "end of file"
    CORE_TOK_YYerror = 256,        // error
    CORE_TOK_YYUNDEF = 257,        // "invalid token"
    CORE_TOK_SEMICOLON = 258,      // ";"
    CORE_TOK_LBRACE = 259,         // "{"
    CORE_TOK_RBRACE = 260,         // "}"
    CORE_TOK_COMMA = 261,          // ","
    CORE_TOK_COLON = 262,          // ":"
    CORE_TOK_ASSIGN = 263,         // "="
    CORE_TOK_LPAREN = 264,         // "("
    CORE_TOK_RPAREN = 265,         // ")"
    CORE_TOK_LBRACKET = 266,       // "["
    CORE_TOK_RBRACKET = 267,       // "]"
    CORE_TOK_DOT = 268,            // "."
    CORE_TOK_AMPERSAND = 269,      // "&"
    CORE_TOK_EXCMARK = 270,        // "!"
    CORE_TOK_TILDA = 271,          // "~"
    CORE_TOK_MINUS = 272,          // "-"
    CORE_TOK_PLUS = 273,           // "+"
    CORE_TOK_ASTERISK = 274,       // "*"
    CORE_TOK_SLASH = 275,          // "/"
    CORE_TOK_PERCENT = 276,        // "%"
    CORE_TOK_LANGLE = 277,         // "<"
    CORE_TOK_RANGLE = 278,         // ">"
    CORE_TOK_CIRCUM = 279,         // "^"
    CORE_TOK_VBAR = 280,           // "|"
    CORE_TOK_QUESTMARK = 281,      // "?"
    CORE_TOK_PTR_OP = 282,         // PTR_OP
    CORE_TOK_INC_OP = 283,         // INC_OP
    CORE_TOK_DEC_OP = 284,         // DEC_OP
    CORE_TOK_LEFT_OP = 285,        // LEFT_OP
    CORE_TOK_RIGHT_OP = 286,       // RIGHT_OP
    CORE_TOK_LE_OP = 287,          // LE_OP
    CORE_TOK_GE_OP = 288,          // GE_OP
    CORE_TOK_EQ_OP = 289,          // EQ_OP
    CORE_TOK_NE_OP = 290,          // NE_OP
    CORE_TOK_AND_OP = 291,         // AND_OP
    CORE_TOK_OR_OP = 292,          // OR_OP
    CORE_TOK_MUL_ASSIGN = 293,     // MUL_ASSIGN
    CORE_TOK_DIV_ASSIGN = 294,     // DIV_ASSIGN
    CORE_TOK_MOD_ASSIGN = 295,     // MOD_ASSIGN
    CORE_TOK_ADD_ASSIGN = 296,     // ADD_ASSIGN
    CORE_TOK_SUB_ASSIGN = 297,     // SUB_ASSIGN
    CORE_TOK_LEFT_ASSIGN = 298,    // LEFT_ASSIGN
    CORE_TOK_RIGHT_ASSIGN = 299,   // RIGHT_ASSIGN
    CORE_TOK_AND_ASSIGN = 300,     // AND_ASSIGN
    CORE_TOK_XOR_ASSIGN = 301,     // XOR_ASSIGN
    CORE_TOK_OR_ASSIGN = 302,      // OR_ASSIGN
    CORE_TOK_SIZEOF = 303,         // SIZEOF
    CORE_TOK_TYPEDEF = 304,        // TYPEDEF
    CORE_TOK_EXTERN = 305,         // EXTERN
    CORE_TOK_STATIC = 306,         // STATIC
    CORE_TOK_AUTO = 307,           // AUTO
    CORE_TOK_REGISTER = 308,       // REGISTER
    CORE_TOK_INLINE = 309,         // INLINE
    CORE_TOK_RESTRICT = 310,       // RESTRICT
    CORE_TOK_EXT_PURE = 311,       // EXT_PURE
    CORE_TOK_EXT_FCONST = 312,     // EXT_FCONST
    CORE_TOK_CHAR = 313,           // CHAR
    CORE_TOK_SHORT = 314,          // SHORT
    CORE_TOK_INT = 315,            // INT
    CORE_TOK_LONG = 316,           // LONG
    CORE_TOK_SIGNED = 317,         // SIGNED
    CORE_TOK_UNSIGNED = 318,       // UNSIGNED
    CORE_TOK_FLOAT = 319,          // FLOAT
    CORE_TOK_DOUBLE = 320,         // DOUBLE
    CORE_TOK_CONST = 321,          // CONST
    CORE_TOK_VOLATILE = 322,       // VOLATILE
    CORE_TOK_VOID = 323,           // VOID
    CORE_TOK_STRUCT = 324,         // STRUCT
    CORE_TOK_UNION = 325,          // UNION
    CORE_TOK_ENUM = 326,           // ENUM
    CORE_TOK_ELLIPSIS = 327,       // ELLIPSIS
    CORE_TOK_CASE = 328,           // CASE
    CORE_TOK_DEFAULT = 329,        // DEFAULT
    CORE_TOK_IF = 330,             // IF
    CORE_TOK_SWITCH = 331,         // SWITCH
    CORE_TOK_WHILE = 332,          // WHILE
    CORE_TOK_DO = 333,             // DO
    CORE_TOK_FOR = 334,            // FOR
    CORE_TOK_GOTO = 335,           // GOTO
    CORE_TOK_CONTINUE = 336,       // CONTINUE
    CORE_TOK_BREAK = 337,          // BREAK
    CORE_TOK_RETURN = 338,         // RETURN
    CORE_TOK_IFX = 339,            // IFX
    CORE_TOK_ELSE = 340,           // ELSE
    CORE_TOK_IDENTIFIER = 341,     // IDENTIFIER
    CORE_TOK_TYPE_NAME = 342,      // TYPE_NAME
    CORE_TOK_CONSTANT = 343,       // CONSTANT
    CORE_TOK_STRING_LITERAL = 344  // STRING_LITERAL
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 90, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_SEMICOLON = 3,                         // ";"
        S_LBRACE = 4,                            // "{"
        S_RBRACE = 5,                            // "}"
        S_COMMA = 6,                             // ","
        S_COLON = 7,                             // ":"
        S_ASSIGN = 8,                            // "="
        S_LPAREN = 9,                            // "("
        S_RPAREN = 10,                           // ")"
        S_LBRACKET = 11,                         // "["
        S_RBRACKET = 12,                         // "]"
        S_DOT = 13,                              // "."
        S_AMPERSAND = 14,                        // "&"
        S_EXCMARK = 15,                          // "!"
        S_TILDA = 16,                            // "~"
        S_MINUS = 17,                            // "-"
        S_PLUS = 18,                             // "+"
        S_ASTERISK = 19,                         // "*"
        S_SLASH = 20,                            // "/"
        S_PERCENT = 21,                          // "%"
        S_LANGLE = 22,                           // "<"
        S_RANGLE = 23,                           // ">"
        S_CIRCUM = 24,                           // "^"
        S_VBAR = 25,                             // "|"
        S_QUESTMARK = 26,                        // "?"
        S_PTR_OP = 27,                           // PTR_OP
        S_INC_OP = 28,                           // INC_OP
        S_DEC_OP = 29,                           // DEC_OP
        S_LEFT_OP = 30,                          // LEFT_OP
        S_RIGHT_OP = 31,                         // RIGHT_OP
        S_LE_OP = 32,                            // LE_OP
        S_GE_OP = 33,                            // GE_OP
        S_EQ_OP = 34,                            // EQ_OP
        S_NE_OP = 35,                            // NE_OP
        S_AND_OP = 36,                           // AND_OP
        S_OR_OP = 37,                            // OR_OP
        S_MUL_ASSIGN = 38,                       // MUL_ASSIGN
        S_DIV_ASSIGN = 39,                       // DIV_ASSIGN
        S_MOD_ASSIGN = 40,                       // MOD_ASSIGN
        S_ADD_ASSIGN = 41,                       // ADD_ASSIGN
        S_SUB_ASSIGN = 42,                       // SUB_ASSIGN
        S_LEFT_ASSIGN = 43,                      // LEFT_ASSIGN
        S_RIGHT_ASSIGN = 44,                     // RIGHT_ASSIGN
        S_AND_ASSIGN = 45,                       // AND_ASSIGN
        S_XOR_ASSIGN = 46,                       // XOR_ASSIGN
        S_OR_ASSIGN = 47,                        // OR_ASSIGN
        S_SIZEOF = 48,                           // SIZEOF
        S_TYPEDEF = 49,                          // TYPEDEF
        S_EXTERN = 50,                           // EXTERN
        S_STATIC = 51,                           // STATIC
        S_AUTO = 52,                             // AUTO
        S_REGISTER = 53,                         // REGISTER
        S_INLINE = 54,                           // INLINE
        S_RESTRICT = 55,                         // RESTRICT
        S_EXT_PURE = 56,                         // EXT_PURE
        S_EXT_FCONST = 57,                       // EXT_FCONST
        S_CHAR = 58,                             // CHAR
        S_SHORT = 59,                            // SHORT
        S_INT = 60,                              // INT
        S_LONG = 61,                             // LONG
        S_SIGNED = 62,                           // SIGNED
        S_UNSIGNED = 63,                         // UNSIGNED
        S_FLOAT = 64,                            // FLOAT
        S_DOUBLE = 65,                           // DOUBLE
        S_CONST = 66,                            // CONST
        S_VOLATILE = 67,                         // VOLATILE
        S_VOID = 68,                             // VOID
        S_STRUCT = 69,                           // STRUCT
        S_UNION = 70,                            // UNION
        S_ENUM = 71,                             // ENUM
        S_ELLIPSIS = 72,                         // ELLIPSIS
        S_CASE = 73,                             // CASE
        S_DEFAULT = 74,                          // DEFAULT
        S_IF = 75,                               // IF
        S_SWITCH = 76,                           // SWITCH
        S_WHILE = 77,                            // WHILE
        S_DO = 78,                               // DO
        S_FOR = 79,                              // FOR
        S_GOTO = 80,                             // GOTO
        S_CONTINUE = 81,                         // CONTINUE
        S_BREAK = 82,                            // BREAK
        S_RETURN = 83,                           // RETURN
        S_IFX = 84,                              // IFX
        S_ELSE = 85,                             // ELSE
        S_IDENTIFIER = 86,                       // IDENTIFIER
        S_TYPE_NAME = 87,                        // TYPE_NAME
        S_CONSTANT = 88,                         // CONSTANT
        S_STRING_LITERAL = 89,                   // STRING_LITERAL
        S_YYACCEPT = 90,                         // $accept
        S_primary_expr = 91,                     // primary_expr
        S_strings_seq = 92,                      // strings_seq
        S_postfix_expr = 93,                     // postfix_expr
        S_arg_expr_lst = 94,                     // arg_expr_lst
        S_unary_expr = 95,                       // unary_expr
        S_unary_operator = 96,                   // unary_operator
        S_cast_expr = 97,                        // cast_expr
        S_mult_expr = 98,                        // mult_expr
        S_add_expr = 99,                         // add_expr
        S_shift_expr = 100,                      // shift_expr
        S_rel_expr = 101,                        // rel_expr
        S_eq_expr = 102,                         // eq_expr
        S_and_expr = 103,                        // and_expr
        S_xor_expr = 104,                        // xor_expr
        S_or_expr = 105,                         // or_expr
        S_land_expr = 106,                       // land_expr
        S_lor_expr = 107,                        // lor_expr
        S_ternary_expr = 108,                    // ternary_expr
        S_assign_expr = 109,                     // assign_expr
        S_assign_op = 110,                       // assign_op
        S_expr = 111,                            // expr
        S_const_expr = 112,                      // const_expr
        S_decl_specs = 113,                      // decl_specs
        S_storage_class_specifier = 114,         // storage_class_specifier
        S_type_specifier = 115,                  // type_specifier
        S_type_qual = 116,                       // type_qual
        S_function_specifier = 117,              // function_specifier
        S_ustruct_spec = 118,                    // ustruct_spec
        S_ustruct = 119,                         // ustruct
        S_struct_decls_lst = 120,                // struct_decls_lst
        S_struct_declaration = 121,              // struct_declaration
        S_spec_qual_lst = 122,                   // spec_qual_lst
        S_struct_declarator_list = 123,          // struct_declarator_list
        S_struct_declarator = 124,               // struct_declarator
        S_enum_specifier = 125,                  // enum_specifier
        S_enumerator_list = 126,                 // enumerator_list
        S_enumerator = 127,                      // enumerator
        S_declaration = 128,                     // declaration
        S_init_decltor_list = 129,               // init_decltor_list
        S_init_decltor = 130,                    // init_decltor
        S_declarator = 131,                      // declarator
        S_pointer = 132,                         // pointer
        S_dir_decltor = 133,                     // dir_decltor
        S_type_qual_lst = 134,                   // type_qual_lst
        S_param_type_lst = 135,                  // param_type_lst
        S_param_lst = 136,                       // param_lst
        S_parameter_declaration = 137,           // parameter_declaration
        S_type_name = 138,                       // type_name
        S_abstr_decltor = 139,                   // abstr_decltor
        S_dir_abstr_decltor = 140,               // dir_abstr_decltor
        S_initializer = 141,                     // initializer
        S_init_lst = 142,                        // init_lst
        S_designator = 143,                      // designator
        S_stmt = 144,                            // stmt
        S_label_stmt = 145,                      // label_stmt
        S_compound_stmt = 146,                   // compound_stmt
        S_block_items = 147,                     // block_items
        S_expr_stmt = 148,                       // expr_stmt
        S_select_stmt = 149,                     // select_stmt
        S_loop_stmt = 150,                       // loop_stmt
        S_jmp_stmt = 151,                        // jmp_stmt
        S_trans_unit = 152,                      // trans_unit
        S_func_def = 153                         // func_def
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.move< AST_AbstrDeclarator * > (std::move (that.value));
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.move< AST_ArgumentsList * > (std::move (that.value));
        break;

      case symbol_kind::S_block_items: // block_items
        value.move< AST_BlockItemList * > (std::move (that.value));
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.move< AST_CompoundStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.move< AST_DeclSpecifiers * > (std::move (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< AST_Declaration * > (std::move (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< AST_Declarator * > (std::move (that.value));
        break;

      case symbol_kind::S_designator: // designator
        value.move< AST_Designator * > (std::move (that.value));
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.move< AST_DirAbstrDeclarator * > (std::move (that.value));
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.move< AST_DirDeclarator * > (std::move (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< AST_EnumSpecifier * > (std::move (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< AST_Enumerator * > (std::move (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< AST_EnumeratorList * > (std::move (that.value));
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
        value.move< AST_Expr * > (std::move (that.value));
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.move< AST_ExprStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< AST_FunctionDef * > (std::move (that.value));
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.move< AST_InitDeclarator * > (std::move (that.value));
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.move< AST_InitDeclaratorList * > (std::move (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< AST_Initializer * > (std::move (that.value));
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.move< AST_InitializerList * > (std::move (that.value));
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< AST_IterStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.move< AST_JumpStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.move< AST_LabeledStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.move< AST_Literal > (std::move (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< AST_ParameterDeclaration * > (std::move (that.value));
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.move< AST_ParameterList * > (std::move (that.value));
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.move< AST_ParameterTypeList * > (std::move (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< AST_Pointer * > (std::move (that.value));
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.move< AST_SelectionStmt * > (std::move (that.value));
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.move< AST_SpecsQualsList * > (std::move (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
        value.move< AST_Stmt * > (std::move (that.value));
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.move< AST_StringsSeq * > (std::move (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< AST_StructDeclaration * > (std::move (that.value));
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.move< AST_StructDeclarationList * > (std::move (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< AST_StructDeclarator * > (std::move (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< AST_StructDeclaratorList * > (std::move (that.value));
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.move< AST_TranslationUnit * > (std::move (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< AST_TypeName * > (std::move (that.value));
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.move< AST_TypeQuals * > (std::move (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< AST_TypeSpecifier * > (std::move (that.value));
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.move< AST_UStructSpec * > (std::move (that.value));
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.move< short > (std::move (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.move< string_id_t > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_AbstrDeclarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_AbstrDeclarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_ArgumentsList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_ArgumentsList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_BlockItemList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_BlockItemList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_CompoundStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_CompoundStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_DeclSpecifiers *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_DeclSpecifiers *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Declaration *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Declaration *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Declarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Declarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Designator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Designator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_DirAbstrDeclarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_DirAbstrDeclarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_DirDeclarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_DirDeclarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_EnumSpecifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_EnumSpecifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Enumerator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Enumerator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_EnumeratorList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_EnumeratorList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Expr *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Expr *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_ExprStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_ExprStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_FunctionDef *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_FunctionDef *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_InitDeclarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_InitDeclarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_InitDeclaratorList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_InitDeclaratorList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Initializer *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Initializer *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_InitializerList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_InitializerList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_IterStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_IterStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_JumpStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_JumpStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_LabeledStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_LabeledStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Literal&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Literal& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_ParameterDeclaration *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_ParameterDeclaration *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_ParameterList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_ParameterList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_ParameterTypeList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_ParameterTypeList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Pointer *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Pointer *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_SelectionStmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_SelectionStmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_SpecsQualsList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_SpecsQualsList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_Stmt *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_Stmt *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_StringsSeq *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_StringsSeq *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_StructDeclaration *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_StructDeclaration *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_StructDeclarationList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_StructDeclarationList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_StructDeclarator *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_StructDeclarator *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_StructDeclaratorList *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_StructDeclaratorList *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_TranslationUnit *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_TranslationUnit *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_TypeName *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_TypeName *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_TypeQuals *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_TypeQuals *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_TypeSpecifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_TypeSpecifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AST_UStructSpec *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AST_UStructSpec *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, short&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const short& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, string_id_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const string_id_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.template destroy< AST_AbstrDeclarator * > ();
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.template destroy< AST_ArgumentsList * > ();
        break;

      case symbol_kind::S_block_items: // block_items
        value.template destroy< AST_BlockItemList * > ();
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.template destroy< AST_CompoundStmt * > ();
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.template destroy< AST_DeclSpecifiers * > ();
        break;

      case symbol_kind::S_declaration: // declaration
        value.template destroy< AST_Declaration * > ();
        break;

      case symbol_kind::S_declarator: // declarator
        value.template destroy< AST_Declarator * > ();
        break;

      case symbol_kind::S_designator: // designator
        value.template destroy< AST_Designator * > ();
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.template destroy< AST_DirAbstrDeclarator * > ();
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.template destroy< AST_DirDeclarator * > ();
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.template destroy< AST_EnumSpecifier * > ();
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.template destroy< AST_Enumerator * > ();
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.template destroy< AST_EnumeratorList * > ();
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
        value.template destroy< AST_Expr * > ();
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.template destroy< AST_ExprStmt * > ();
        break;

      case symbol_kind::S_func_def: // func_def
        value.template destroy< AST_FunctionDef * > ();
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.template destroy< AST_InitDeclarator * > ();
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.template destroy< AST_InitDeclaratorList * > ();
        break;

      case symbol_kind::S_initializer: // initializer
        value.template destroy< AST_Initializer * > ();
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.template destroy< AST_InitializerList * > ();
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.template destroy< AST_IterStmt * > ();
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.template destroy< AST_JumpStmt * > ();
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.template destroy< AST_LabeledStmt * > ();
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.template destroy< AST_Literal > ();
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.template destroy< AST_ParameterDeclaration * > ();
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.template destroy< AST_ParameterList * > ();
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.template destroy< AST_ParameterTypeList * > ();
        break;

      case symbol_kind::S_pointer: // pointer
        value.template destroy< AST_Pointer * > ();
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.template destroy< AST_SelectionStmt * > ();
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.template destroy< AST_SpecsQualsList * > ();
        break;

      case symbol_kind::S_stmt: // stmt
        value.template destroy< AST_Stmt * > ();
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.template destroy< AST_StringsSeq * > ();
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.template destroy< AST_StructDeclaration * > ();
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.template destroy< AST_StructDeclarationList * > ();
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.template destroy< AST_StructDeclarator * > ();
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.template destroy< AST_StructDeclaratorList * > ();
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.template destroy< AST_TranslationUnit * > ();
        break;

      case symbol_kind::S_type_name: // type_name
        value.template destroy< AST_TypeName * > ();
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.template destroy< AST_TypeQuals * > ();
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.template destroy< AST_TypeSpecifier * > ();
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.template destroy< AST_UStructSpec * > ();
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.template destroy< short > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.template destroy< string_id_t > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::CORE_TOK_YYEOF
                   || (token::CORE_TOK_YYerror <= tok && tok <= token::CORE_TOK_ELSE));
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, AST_Literal v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const AST_Literal& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::CORE_TOK_CONSTANT);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, string_id_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const string_id_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT ((token::CORE_TOK_IDENTIFIER <= tok && tok <= token::CORE_TOK_TYPE_NAME)
                   || tok == token::CORE_TOK_STRING_LITERAL);
#endif
      }
    };

    /// Build a parser object.
    parser (yyscan_t scanner_yyarg, CoreDriver &drv_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::CORE_TOK_YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::CORE_TOK_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::CORE_TOK_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::CORE_TOK_SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LBRACE (location_type l)
      {
        return symbol_type (token::CORE_TOK_LBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_LBRACE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RBRACE (location_type l)
      {
        return symbol_type (token::CORE_TOK_RBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_RBRACE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::CORE_TOK_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (location_type l)
      {
        return symbol_type (token::CORE_TOK_COLON, std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_COLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LPAREN (location_type l)
      {
        return symbol_type (token::CORE_TOK_LPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_LPAREN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RPAREN (location_type l)
      {
        return symbol_type (token::CORE_TOK_RPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_RPAREN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LBRACKET (location_type l)
      {
        return symbol_type (token::CORE_TOK_LBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_LBRACKET (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RBRACKET (location_type l)
      {
        return symbol_type (token::CORE_TOK_RBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_RBRACKET (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOT (location_type l)
      {
        return symbol_type (token::CORE_TOK_DOT, std::move (l));
      }
#else
      static
      symbol_type
      make_DOT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AMPERSAND (location_type l)
      {
        return symbol_type (token::CORE_TOK_AMPERSAND, std::move (l));
      }
#else
      static
      symbol_type
      make_AMPERSAND (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_AMPERSAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXCMARK (location_type l)
      {
        return symbol_type (token::CORE_TOK_EXCMARK, std::move (l));
      }
#else
      static
      symbol_type
      make_EXCMARK (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_EXCMARK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TILDA (location_type l)
      {
        return symbol_type (token::CORE_TOK_TILDA, std::move (l));
      }
#else
      static
      symbol_type
      make_TILDA (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_TILDA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (location_type l)
      {
        return symbol_type (token::CORE_TOK_MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::CORE_TOK_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASTERISK (location_type l)
      {
        return symbol_type (token::CORE_TOK_ASTERISK, std::move (l));
      }
#else
      static
      symbol_type
      make_ASTERISK (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ASTERISK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SLASH (location_type l)
      {
        return symbol_type (token::CORE_TOK_SLASH, std::move (l));
      }
#else
      static
      symbol_type
      make_SLASH (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SLASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PERCENT (location_type l)
      {
        return symbol_type (token::CORE_TOK_PERCENT, std::move (l));
      }
#else
      static
      symbol_type
      make_PERCENT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_PERCENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LANGLE (location_type l)
      {
        return symbol_type (token::CORE_TOK_LANGLE, std::move (l));
      }
#else
      static
      symbol_type
      make_LANGLE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LANGLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RANGLE (location_type l)
      {
        return symbol_type (token::CORE_TOK_RANGLE, std::move (l));
      }
#else
      static
      symbol_type
      make_RANGLE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RANGLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CIRCUM (location_type l)
      {
        return symbol_type (token::CORE_TOK_CIRCUM, std::move (l));
      }
#else
      static
      symbol_type
      make_CIRCUM (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CIRCUM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VBAR (location_type l)
      {
        return symbol_type (token::CORE_TOK_VBAR, std::move (l));
      }
#else
      static
      symbol_type
      make_VBAR (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_VBAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_QUESTMARK (location_type l)
      {
        return symbol_type (token::CORE_TOK_QUESTMARK, std::move (l));
      }
#else
      static
      symbol_type
      make_QUESTMARK (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_QUESTMARK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PTR_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_PTR_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_PTR_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_PTR_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INC_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_INC_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_INC_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_INC_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEC_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_DEC_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_DEC_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DEC_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_LEFT_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LEFT_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_RIGHT_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RIGHT_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LE_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_LE_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_LE_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LE_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GE_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_GE_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_GE_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_GE_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQ_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_EQ_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_EQ_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_EQ_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NE_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_NE_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_NE_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_NE_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_AND_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_AND_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_AND_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR_OP (location_type l)
      {
        return symbol_type (token::CORE_TOK_OR_OP, std::move (l));
      }
#else
      static
      symbol_type
      make_OR_OP (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_OR_OP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MUL_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_MUL_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_MUL_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_MUL_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIV_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_DIV_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_DIV_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DIV_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MOD_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_MOD_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_MOD_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_MOD_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_ADD_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ADD_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SUB_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_SUB_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_SUB_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SUB_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_LEFT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LEFT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_RIGHT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RIGHT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_AND_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_AND_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_AND_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_XOR_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_XOR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_XOR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_XOR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR_ASSIGN (location_type l)
      {
        return symbol_type (token::CORE_TOK_OR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_OR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_OR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIZEOF (location_type l)
      {
        return symbol_type (token::CORE_TOK_SIZEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_SIZEOF (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SIZEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TYPEDEF (location_type l)
      {
        return symbol_type (token::CORE_TOK_TYPEDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_TYPEDEF (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_TYPEDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXTERN (location_type l)
      {
        return symbol_type (token::CORE_TOK_EXTERN, std::move (l));
      }
#else
      static
      symbol_type
      make_EXTERN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_EXTERN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STATIC (location_type l)
      {
        return symbol_type (token::CORE_TOK_STATIC, std::move (l));
      }
#else
      static
      symbol_type
      make_STATIC (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_STATIC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTO (location_type l)
      {
        return symbol_type (token::CORE_TOK_AUTO, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTO (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_AUTO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REGISTER (location_type l)
      {
        return symbol_type (token::CORE_TOK_REGISTER, std::move (l));
      }
#else
      static
      symbol_type
      make_REGISTER (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_REGISTER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INLINE (location_type l)
      {
        return symbol_type (token::CORE_TOK_INLINE, std::move (l));
      }
#else
      static
      symbol_type
      make_INLINE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_INLINE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RESTRICT (location_type l)
      {
        return symbol_type (token::CORE_TOK_RESTRICT, std::move (l));
      }
#else
      static
      symbol_type
      make_RESTRICT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RESTRICT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXT_PURE (location_type l)
      {
        return symbol_type (token::CORE_TOK_EXT_PURE, std::move (l));
      }
#else
      static
      symbol_type
      make_EXT_PURE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_EXT_PURE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXT_FCONST (location_type l)
      {
        return symbol_type (token::CORE_TOK_EXT_FCONST, std::move (l));
      }
#else
      static
      symbol_type
      make_EXT_FCONST (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_EXT_FCONST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CHAR (location_type l)
      {
        return symbol_type (token::CORE_TOK_CHAR, std::move (l));
      }
#else
      static
      symbol_type
      make_CHAR (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CHAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SHORT (location_type l)
      {
        return symbol_type (token::CORE_TOK_SHORT, std::move (l));
      }
#else
      static
      symbol_type
      make_SHORT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SHORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INT (location_type l)
      {
        return symbol_type (token::CORE_TOK_INT, std::move (l));
      }
#else
      static
      symbol_type
      make_INT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_INT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LONG (location_type l)
      {
        return symbol_type (token::CORE_TOK_LONG, std::move (l));
      }
#else
      static
      symbol_type
      make_LONG (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_LONG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIGNED (location_type l)
      {
        return symbol_type (token::CORE_TOK_SIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_SIGNED (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNSIGNED (location_type l)
      {
        return symbol_type (token::CORE_TOK_UNSIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_UNSIGNED (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_UNSIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT (location_type l)
      {
        return symbol_type (token::CORE_TOK_FLOAT, std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_FLOAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOUBLE (location_type l)
      {
        return symbol_type (token::CORE_TOK_DOUBLE, std::move (l));
      }
#else
      static
      symbol_type
      make_DOUBLE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DOUBLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST (location_type l)
      {
        return symbol_type (token::CORE_TOK_CONST, std::move (l));
      }
#else
      static
      symbol_type
      make_CONST (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CONST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VOLATILE (location_type l)
      {
        return symbol_type (token::CORE_TOK_VOLATILE, std::move (l));
      }
#else
      static
      symbol_type
      make_VOLATILE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_VOLATILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VOID (location_type l)
      {
        return symbol_type (token::CORE_TOK_VOID, std::move (l));
      }
#else
      static
      symbol_type
      make_VOID (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_VOID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRUCT (location_type l)
      {
        return symbol_type (token::CORE_TOK_STRUCT, std::move (l));
      }
#else
      static
      symbol_type
      make_STRUCT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_STRUCT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNION (location_type l)
      {
        return symbol_type (token::CORE_TOK_UNION, std::move (l));
      }
#else
      static
      symbol_type
      make_UNION (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_UNION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENUM (location_type l)
      {
        return symbol_type (token::CORE_TOK_ENUM, std::move (l));
      }
#else
      static
      symbol_type
      make_ENUM (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ENUM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELLIPSIS (location_type l)
      {
        return symbol_type (token::CORE_TOK_ELLIPSIS, std::move (l));
      }
#else
      static
      symbol_type
      make_ELLIPSIS (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ELLIPSIS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CASE (location_type l)
      {
        return symbol_type (token::CORE_TOK_CASE, std::move (l));
      }
#else
      static
      symbol_type
      make_CASE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CASE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEFAULT (location_type l)
      {
        return symbol_type (token::CORE_TOK_DEFAULT, std::move (l));
      }
#else
      static
      symbol_type
      make_DEFAULT (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DEFAULT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::CORE_TOK_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SWITCH (location_type l)
      {
        return symbol_type (token::CORE_TOK_SWITCH, std::move (l));
      }
#else
      static
      symbol_type
      make_SWITCH (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_SWITCH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHILE (location_type l)
      {
        return symbol_type (token::CORE_TOK_WHILE, std::move (l));
      }
#else
      static
      symbol_type
      make_WHILE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_WHILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DO (location_type l)
      {
        return symbol_type (token::CORE_TOK_DO, std::move (l));
      }
#else
      static
      symbol_type
      make_DO (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_DO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FOR (location_type l)
      {
        return symbol_type (token::CORE_TOK_FOR, std::move (l));
      }
#else
      static
      symbol_type
      make_FOR (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_FOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GOTO (location_type l)
      {
        return symbol_type (token::CORE_TOK_GOTO, std::move (l));
      }
#else
      static
      symbol_type
      make_GOTO (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_GOTO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONTINUE (location_type l)
      {
        return symbol_type (token::CORE_TOK_CONTINUE, std::move (l));
      }
#else
      static
      symbol_type
      make_CONTINUE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CONTINUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BREAK (location_type l)
      {
        return symbol_type (token::CORE_TOK_BREAK, std::move (l));
      }
#else
      static
      symbol_type
      make_BREAK (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_BREAK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RETURN (location_type l)
      {
        return symbol_type (token::CORE_TOK_RETURN, std::move (l));
      }
#else
      static
      symbol_type
      make_RETURN (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_RETURN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFX (location_type l)
      {
        return symbol_type (token::CORE_TOK_IFX, std::move (l));
      }
#else
      static
      symbol_type
      make_IFX (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_IFX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELSE (location_type l)
      {
        return symbol_type (token::CORE_TOK_ELSE, std::move (l));
      }
#else
      static
      symbol_type
      make_ELSE (const location_type& l)
      {
        return symbol_type (token::CORE_TOK_ELSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENTIFIER (string_id_t v, location_type l)
      {
        return symbol_type (token::CORE_TOK_IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENTIFIER (const string_id_t& v, const location_type& l)
      {
        return symbol_type (token::CORE_TOK_IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TYPE_NAME (string_id_t v, location_type l)
      {
        return symbol_type (token::CORE_TOK_TYPE_NAME, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TYPE_NAME (const string_id_t& v, const location_type& l)
      {
        return symbol_type (token::CORE_TOK_TYPE_NAME, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONSTANT (AST_Literal v, location_type l)
      {
        return symbol_type (token::CORE_TOK_CONSTANT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONSTANT (const AST_Literal& v, const location_type& l)
      {
        return symbol_type (token::CORE_TOK_CONSTANT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING_LITERAL (string_id_t v, location_type l)
      {
        return symbol_type (token::CORE_TOK_STRING_LITERAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING_LITERAL (const string_id_t& v, const location_type& l)
      {
        return symbol_type (token::CORE_TOK_STRING_LITERAL, v, l);
      }
#endif


    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif

    /// Check the lookahead yytoken.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_check_ (symbol_kind_type yytoken) const;
    /// Establish the initial context if no initial context currently exists.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_establish_ (symbol_kind_type yytoken);
    /// Discard any previous initial lookahead context because of event.
    /// \param event  the event which caused the lookahead to be discarded.
    ///               Only used for debbuging output.
    void yy_lac_discard_ (const char* event);

    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const unsigned char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const unsigned char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const unsigned char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;
    /// The stack for LAC.
    /// Logically, the yy_lac_stack's lifetime is confined to the function
    /// yy_lac_check_. We just store it as a member of this class to hold
    /// on to the memory and to avoid frequent reallocations.
    /// Since yy_lac_check_ is const, this member must be mutable.
    mutable std::vector<state_type> yylac_stack_;
    /// Whether an initial LAC context was established.
    bool yy_lac_established_;


    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 1420,     ///< Last index in yytable_.
      yynnts_ = 64,  ///< Number of nonterminal symbols.
      yyfinal_ = 53 ///< Termination state number.
    };


    // User arguments.
    yyscan_t scanner;
    CoreDriver &drv;

  };

  inline
  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89
    };
    // Last valid token kind.
    const int code_max = 344;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.copy< AST_AbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.copy< AST_ArgumentsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block_items: // block_items
        value.copy< AST_BlockItemList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.copy< AST_CompoundStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.copy< AST_DeclSpecifiers * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.copy< AST_Declaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.copy< AST_Declarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_designator: // designator
        value.copy< AST_Designator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.copy< AST_DirAbstrDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.copy< AST_DirDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.copy< AST_EnumSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.copy< AST_Enumerator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.copy< AST_EnumeratorList * > (YY_MOVE (that.value));
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
        value.copy< AST_Expr * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.copy< AST_ExprStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.copy< AST_FunctionDef * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.copy< AST_InitDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.copy< AST_InitDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.copy< AST_Initializer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.copy< AST_InitializerList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.copy< AST_IterStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.copy< AST_JumpStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.copy< AST_LabeledStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.copy< AST_Literal > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.copy< AST_ParameterDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.copy< AST_ParameterList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.copy< AST_ParameterTypeList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.copy< AST_Pointer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.copy< AST_SelectionStmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.copy< AST_SpecsQualsList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
        value.copy< AST_Stmt * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.copy< AST_StringsSeq * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.copy< AST_StructDeclaration * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.copy< AST_StructDeclarationList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.copy< AST_StructDeclarator * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.copy< AST_StructDeclaratorList * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.copy< AST_TranslationUnit * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.copy< AST_TypeName * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.copy< AST_TypeQuals * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.copy< AST_TypeSpecifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.copy< AST_UStructSpec * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.copy< short > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.copy< string_id_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_abstr_decltor: // abstr_decltor
        value.move< AST_AbstrDeclarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_arg_expr_lst: // arg_expr_lst
        value.move< AST_ArgumentsList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_block_items: // block_items
        value.move< AST_BlockItemList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_compound_stmt: // compound_stmt
        value.move< AST_CompoundStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_decl_specs: // decl_specs
        value.move< AST_DeclSpecifiers * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< AST_Declaration * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< AST_Declarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_designator: // designator
        value.move< AST_Designator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_dir_abstr_decltor: // dir_abstr_decltor
        value.move< AST_DirAbstrDeclarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_dir_decltor: // dir_decltor
        value.move< AST_DirDeclarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< AST_EnumSpecifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< AST_Enumerator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< AST_EnumeratorList * > (YY_MOVE (s.value));
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
        value.move< AST_Expr * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr_stmt: // expr_stmt
        value.move< AST_ExprStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_func_def: // func_def
        value.move< AST_FunctionDef * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_init_decltor: // init_decltor
        value.move< AST_InitDeclarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_init_decltor_list: // init_decltor_list
        value.move< AST_InitDeclaratorList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< AST_Initializer * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_init_lst: // init_lst
        value.move< AST_InitializerList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_loop_stmt: // loop_stmt
        value.move< AST_IterStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_jmp_stmt: // jmp_stmt
        value.move< AST_JumpStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_label_stmt: // label_stmt
        value.move< AST_LabeledStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_CONSTANT: // CONSTANT
        value.move< AST_Literal > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< AST_ParameterDeclaration * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_param_lst: // param_lst
        value.move< AST_ParameterList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_param_type_lst: // param_type_lst
        value.move< AST_ParameterTypeList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< AST_Pointer * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_select_stmt: // select_stmt
        value.move< AST_SelectionStmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_spec_qual_lst: // spec_qual_lst
        value.move< AST_SpecsQualsList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_stmt: // stmt
        value.move< AST_Stmt * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_strings_seq: // strings_seq
        value.move< AST_StringsSeq * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< AST_StructDeclaration * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_struct_decls_lst: // struct_decls_lst
        value.move< AST_StructDeclarationList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< AST_StructDeclarator * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< AST_StructDeclaratorList * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_trans_unit: // trans_unit
        value.move< AST_TranslationUnit * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< AST_TypeName * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_qual_lst: // type_qual_lst
        value.move< AST_TypeQuals * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< AST_TypeSpecifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ustruct_spec: // ustruct_spec
        value.move< AST_UStructSpec * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assign_op: // assign_op
      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
      case symbol_kind::S_type_qual: // type_qual
      case symbol_kind::S_function_specifier: // function_specifier
      case symbol_kind::S_ustruct: // ustruct
        value.move< short > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_TYPE_NAME: // TYPE_NAME
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.move< string_id_t > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


} // yy
#line 4389 "yy_parser.hpp"




#endif // !YY_YY_YY_PARSER_HPP_INCLUDED

/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         cssyyparse
#define yylex           cssyylex
#define yyerror         cssyyerror
#define yylval          cssyylval
#define yychar          cssyychar
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "Source/WebCore/css/CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSMediaRule.h"
#include "CSSParser.h"
#include "CSSParserMode.h"
#include "CSSPrimitiveValue.h"
#include "CSSPropertyNames.h"
#include "CSSSelector.h"
#include "CSSSelectorList.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "MediaQueryExp.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <wtf/FastMalloc.h>
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

// FIXME: Replace with %parse-param { CSSParser* parser } once we can depend on bison 2.x
#define YYPARSE_PARAM parser
#define YYLEX_PARAM parser



/* Line 268 of yacc.c  */
#line 141 "./DerivedSources/WebCore/CSSGrammar.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 0,
     LOWEST_PREC = 258,
     UNIMPORTANT_TOK = 259,
     WHITESPACE = 260,
     SGML_CD = 261,
     INCLUDES = 262,
     DASHMATCH = 263,
     BEGINSWITH = 264,
     ENDSWITH = 265,
     CONTAINS = 266,
     STRING = 267,
     IDENT = 268,
     NTH = 269,
     HEX = 270,
     IDSEL = 271,
     IMPORT_SYM = 272,
     PAGE_SYM = 273,
     MEDIA_SYM = 274,
     FONT_FACE_SYM = 275,
     CHARSET_SYM = 276,
     NAMESPACE_SYM = 277,
     WEBKIT_RULE_SYM = 278,
     WEBKIT_DECLS_SYM = 279,
     WEBKIT_KEYFRAME_RULE_SYM = 280,
     WEBKIT_KEYFRAMES_SYM = 281,
     WEBKIT_VALUE_SYM = 282,
     WEBKIT_MEDIAQUERY_SYM = 283,
     WEBKIT_SELECTOR_SYM = 284,
     WEBKIT_REGION_RULE_SYM = 285,
     TOPLEFTCORNER_SYM = 286,
     TOPLEFT_SYM = 287,
     TOPCENTER_SYM = 288,
     TOPRIGHT_SYM = 289,
     TOPRIGHTCORNER_SYM = 290,
     BOTTOMLEFTCORNER_SYM = 291,
     BOTTOMLEFT_SYM = 292,
     BOTTOMCENTER_SYM = 293,
     BOTTOMRIGHT_SYM = 294,
     BOTTOMRIGHTCORNER_SYM = 295,
     LEFTTOP_SYM = 296,
     LEFTMIDDLE_SYM = 297,
     LEFTBOTTOM_SYM = 298,
     RIGHTTOP_SYM = 299,
     RIGHTMIDDLE_SYM = 300,
     RIGHTBOTTOM_SYM = 301,
     ATKEYWORD = 302,
     IMPORTANT_SYM = 303,
     MEDIA_ONLY = 304,
     MEDIA_NOT = 305,
     MEDIA_AND = 306,
     REMS = 307,
     QEMS = 308,
     EMS = 309,
     EXS = 310,
     PXS = 311,
     CMS = 312,
     MMS = 313,
     INS = 314,
     PTS = 315,
     PCS = 316,
     DEGS = 317,
     RADS = 318,
     GRADS = 319,
     TURNS = 320,
     MSECS = 321,
     SECS = 322,
     HERTZ = 323,
     KHERTZ = 324,
     DIMEN = 325,
     INVALIDDIMEN = 326,
     PERCENTAGE = 327,
     FLOATTOKEN = 328,
     INTEGER = 329,
     VW = 330,
     VH = 331,
     VMIN = 332,
     URI = 333,
     FUNCTION = 334,
     ANYFUNCTION = 335,
     NOTFUNCTION = 336,
     CALCFUNCTION = 337,
     MINFUNCTION = 338,
     MAXFUNCTION = 339,
     UNICODERANGE = 340
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 64 "Source/WebCore/css/CSSGrammar.y"

    bool boolean;
    char character;
    int integer;
    double number;
    CSSParserString string;

    CSSRule* rule;
    Vector<RefPtr<CSSRule> >* ruleList;
    CSSParserSelector* selector;
    Vector<OwnPtr<CSSParserSelector> >* selectorList;
    CSSSelector::MarginBoxType marginBox;
    CSSSelector::Relation relation;
    MediaQuerySet* mediaList;
    MediaQuery* mediaQuery;
    MediaQuery::Restrictor mediaQueryRestrictor;
    MediaQueryExp* mediaQueryExp;
    CSSParserValue value;
    CSSParserValueList* valueList;
    Vector<OwnPtr<MediaQueryExp> >* mediaQueryExpList;
    StyleKeyframe* keyframe;
    WebKitCSSKeyframesRule* keyframesRule;
    float val;



/* Line 293 of yacc.c  */
#line 290 "./DerivedSources/WebCore/CSSGrammar.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 343 of yacc.c  */
#line 89 "Source/WebCore/css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 343 of yacc.c  */
#line 317 "./DerivedSources/WebCore/CSSGrammar.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1757

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  96
/* YYNRULES -- Number of rules.  */
#define YYNRULES  298
/* YYNRULES -- Number of states.  */
#define YYNSTATES  560

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   104,     2,   105,     2,     2,
      94,    95,    20,    97,    96,   100,    18,   103,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,    93,
       2,   102,    99,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,   101,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    91,    21,    92,    98,     2,     2,     2,
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
      15,    16,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,    11,    14,    17,    20,    23,    26,
      33,    40,    46,    52,    58,    64,    65,    68,    69,    72,
      75,    76,    78,    80,    82,    88,    92,    96,   102,   103,
     107,   110,   112,   114,   116,   118,   120,   122,   124,   126,
     128,   130,   132,   133,   137,   139,   141,   143,   145,   147,
     149,   151,   153,   155,   157,   164,   171,   178,   182,   186,
     193,   200,   204,   208,   209,   212,   214,   216,   219,   220,
     225,   233,   235,   241,   242,   246,   247,   249,   251,   253,
     258,   259,   261,   263,   268,   271,   279,   286,   289,   298,
     300,   302,   303,   307,   314,   316,   322,   324,   326,   335,
     339,   343,   345,   348,   350,   351,   353,   358,   359,   367,
     369,   371,   373,   375,   377,   379,   381,   383,   385,   387,
     389,   391,   393,   395,   397,   399,   407,   411,   415,   417,
     425,   428,   431,   434,   436,   437,   439,   441,   443,   444,
     445,   452,   454,   459,   462,   465,   467,   469,   472,   476,
     479,   481,   484,   487,   489,   492,   494,   497,   501,   504,
     506,   512,   515,   517,   519,   521,   524,   527,   529,   531,
     533,   535,   537,   540,   543,   548,   557,   563,   573,   575,
     577,   579,   581,   583,   585,   587,   589,   592,   595,   599,
     606,   613,   621,   628,   635,   637,   640,   642,   646,   648,
     651,   654,   658,   662,   667,   671,   677,   682,   687,   694,
     700,   703,   710,   717,   720,   724,   729,   732,   735,   738,
     739,   741,   745,   748,   752,   755,   758,   761,   762,   764,
     767,   770,   773,   776,   780,   783,   786,   789,   792,   794,
     796,   798,   801,   804,   807,   810,   813,   816,   819,   822,
     825,   828,   831,   834,   837,   840,   843,   846,   849,   852,
     855,   858,   861,   864,   867,   870,   873,   879,   884,   889,
     893,   895,   898,   901,   904,   907,   910,   917,   920,   924,
     928,   930,   933,   935,   940,   946,   950,   952,   954,   960,
     964,   966,   969,   973,   977,   980,   986,   990,   992
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     107,     0,    -1,   114,   116,   115,   120,    -1,   108,   114,
      -1,   110,   114,    -1,   111,   114,    -1,   112,   114,    -1,
     113,   114,    -1,   109,   114,    -1,    28,    91,   114,   121,
     114,    92,    -1,    30,    91,   114,   144,   114,    92,    -1,
      29,    91,   159,   179,    92,    -1,    32,    91,   114,   184,
      92,    -1,    33,     5,   114,   136,    92,    -1,    34,    91,
     114,   163,    92,    -1,    -1,   114,     5,    -1,    -1,   115,
       6,    -1,   115,     5,    -1,    -1,   118,    -1,    92,    -1,
       0,    -1,    26,   114,    12,   114,    93,    -1,    26,     1,
     200,    -1,    26,     1,    93,    -1,    26,   114,    12,   114,
      93,    -1,    -1,   120,   122,   115,    -1,   160,   162,    -1,
     139,    -1,   147,    -1,   153,    -1,   141,    -1,   127,    -1,
     126,    -1,   155,    -1,   121,    -1,   119,    -1,   199,    -1,
     198,    -1,    -1,   123,   125,   115,    -1,   162,    -1,   147,
      -1,   153,    -1,   141,    -1,   124,    -1,   199,    -1,   198,
      -1,   127,    -1,   126,    -1,   139,    -1,    22,   114,   129,
     114,   137,    93,    -1,    22,   114,   129,   114,   137,     0,
      -1,    22,   114,   129,   114,   137,   200,    -1,    22,     1,
      93,    -1,    22,     1,   200,    -1,    27,   114,   128,   129,
     114,    93,    -1,    27,   114,   128,   129,   114,   200,    -1,
      27,     1,   200,    -1,    27,     1,    93,    -1,    -1,    13,
     114,    -1,    12,    -1,    83,    -1,    13,   114,    -1,    -1,
      17,   114,   184,   114,    -1,    94,   114,   130,   114,   131,
      95,   114,    -1,   132,    -1,   133,   114,    56,   114,   132,
      -1,    -1,    56,   114,   133,    -1,    -1,    54,    -1,    55,
      -1,   133,    -1,   135,   114,   140,   134,    -1,    -1,   138,
      -1,   136,    -1,   138,    96,   114,   136,    -1,   138,     1,
      -1,    24,   114,   138,    91,   114,   123,   197,    -1,    24,
     114,    91,   114,   123,   197,    -1,    13,   114,    -1,    31,
     114,   142,   114,    91,   114,   143,    92,    -1,    13,    -1,
      12,    -1,    -1,   143,   144,   114,    -1,   145,   114,    91,
     114,   179,    92,    -1,   146,    -1,   145,   114,    96,   114,
     146,    -1,    77,    -1,    13,    -1,    23,   114,   148,   114,
      91,   114,   149,   117,    -1,    23,     1,   200,    -1,    23,
       1,    93,    -1,    13,    -1,    13,   177,    -1,   177,    -1,
      -1,   179,    -1,   149,   150,   114,   179,    -1,    -1,   152,
     151,   114,    91,   114,   179,   117,    -1,    36,    -1,    37,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    25,
     114,    91,   114,   179,    92,   114,    -1,    25,     1,   200,
      -1,    25,     1,    93,    -1,   163,    -1,    35,     5,   154,
      91,   114,   123,   197,    -1,    97,   114,    -1,    98,   114,
      -1,    99,   114,    -1,   158,    -1,    -1,   100,    -1,    97,
      -1,   114,    -1,    -1,    -1,   163,   161,    91,   159,   179,
     117,    -1,   165,    -1,   163,    96,   114,   165,    -1,   163,
       1,    -1,   165,     5,    -1,   167,    -1,   164,    -1,   164,
     167,    -1,   165,   156,   167,    -1,   165,     1,    -1,    21,
      -1,    20,    21,    -1,    13,    21,    -1,   169,    -1,   169,
     170,    -1,   170,    -1,   166,   169,    -1,   166,   169,   170,
      -1,   166,   170,    -1,   167,    -1,   168,   114,    96,   114,
     167,    -1,   168,     1,    -1,    13,    -1,    20,    -1,   171,
      -1,   170,   171,    -1,   170,     1,    -1,    16,    -1,    15,
      -1,   172,    -1,   174,    -1,   178,    -1,    18,    13,    -1,
      13,   114,    -1,    19,   114,   173,   101,    -1,    19,   114,
     173,   175,   114,   176,   114,   101,    -1,    19,   114,   166,
     173,   101,    -1,    19,   114,   166,   173,   175,   114,   176,
     114,   101,    -1,   102,    -1,     7,    -1,     8,    -1,     9,
      -1,    10,    -1,    11,    -1,    13,    -1,    12,    -1,    17,
      13,    -1,    17,    13,    -1,    17,    17,    13,    -1,    17,
      85,   114,   168,   114,    95,    -1,    17,    84,   114,    14,
     114,    95,    -1,    17,    84,   114,   157,    79,   114,    95,
      -1,    17,    84,   114,    13,   114,    95,    -1,    17,    86,
     114,   167,   114,    95,    -1,   181,    -1,   180,   181,    -1,
     180,    -1,     1,   201,     1,    -1,     1,    -1,   180,     1,
      -1,   180,   201,    -1,   181,    93,   114,    -1,   181,   201,
     114,    -1,   181,   201,    93,   114,    -1,     1,    93,   114,
      -1,     1,   201,     1,    93,   114,    -1,   180,   181,    93,
     114,    -1,   180,     1,    93,   114,    -1,   180,     1,   201,
       1,    93,   114,    -1,   182,    17,   114,   184,   183,    -1,
     182,     1,    -1,   182,    17,   114,     1,   184,   183,    -1,
     182,    17,   114,   184,   183,     1,    -1,    53,   114,    -1,
     182,    17,   114,    -1,   182,    17,   114,     1,    -1,   182,
     200,    -1,    13,   114,    -1,    53,   114,    -1,    -1,   186,
      -1,   184,   185,   186,    -1,   184,   201,    -1,   184,   201,
       1,    -1,   184,     1,    -1,   103,   114,    -1,    96,   114,
      -1,    -1,   187,    -1,   158,   187,    -1,    12,   114,    -1,
      13,   114,    -1,    75,   114,    -1,   158,    75,   114,    -1,
      83,   114,    -1,    90,   114,    -1,    15,   114,    -1,   104,
     114,    -1,   188,    -1,   194,    -1,   196,    -1,   105,   114,
      -1,    79,   114,    -1,    78,   114,    -1,    77,   114,    -1,
      61,   114,    -1,    62,   114,    -1,    63,   114,    -1,    64,
     114,    -1,    65,   114,    -1,    66,   114,    -1,    67,   114,
      -1,    68,   114,    -1,    69,   114,    -1,    70,   114,    -1,
      71,   114,    -1,    72,   114,    -1,    73,   114,    -1,    74,
     114,    -1,    59,   114,    -1,    58,   114,    -1,    60,   114,
      -1,    57,   114,    -1,    80,   114,    -1,    81,   114,    -1,
      82,   114,    -1,    84,   114,   184,    95,   114,    -1,    84,
     114,   184,     0,    -1,    84,   114,    95,   114,    -1,    84,
     114,     1,    -1,   187,    -1,   158,   187,    -1,    97,     5,
      -1,   100,     5,    -1,    20,   114,    -1,   103,   114,    -1,
      94,   114,   192,   114,    95,   114,    -1,   189,   114,    -1,
     192,   190,   189,    -1,   192,   190,   191,    -1,   191,    -1,
     192,     1,    -1,   192,    -1,   193,    96,   114,   192,    -1,
      87,   114,   192,    95,   114,    -1,    87,   114,     1,    -1,
      88,    -1,    89,    -1,   195,   114,   193,    95,   114,    -1,
     195,   114,     1,    -1,   117,    -1,     1,   117,    -1,    52,
       1,   200,    -1,    52,     1,    93,    -1,     1,   200,    -1,
      91,     1,   201,     1,   117,    -1,    91,     1,   117,    -1,
     200,    -1,   201,     1,   200,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   300,   300,   301,   302,   303,   304,   305,   306,   310,
     316,   322,   328,   341,   348,   358,   359,   362,   364,   365,
     368,   370,   375,   376,   380,   386,   388,   393,   399,   401,
     409,   412,   413,   414,   415,   416,   417,   418,   422,   425,
     426,   427,   431,   432,   443,   444,   445,   446,   450,   451,
     452,   453,   454,   455,   460,   463,   466,   469,   472,   478,
     482,   485,   488,   494,   495,   499,   500,   504,   510,   513,
     519,   526,   531,   538,   541,   547,   550,   553,   559,   564,
     572,   575,   579,   585,   593,   599,   602,   608,   614,   621,
     622,   626,   627,   635,   641,   646,   655,   656,   669,   681,
     684,   690,   696,   704,   709,   717,   718,   722,   722,   730,
     733,   736,   739,   742,   745,   748,   751,   754,   757,   760,
     763,   766,   769,   772,   775,   781,   785,   788,   794,   805,
     814,   815,   816,   820,   821,   825,   826,   830,   837,   844,
     851,   858,   867,   876,   882,   888,   891,   895,   909,   922,
     928,   929,   930,   934,   939,   944,   949,   959,   964,   972,
     980,   988,   994,  1002,  1010,  1013,  1019,  1025,  1033,  1045,
    1046,  1047,  1051,  1062,  1073,  1078,  1084,  1092,  1104,  1107,
    1110,  1113,  1116,  1119,  1125,  1126,  1130,  1141,  1150,  1163,
    1178,  1189,  1200,  1219,  1238,  1241,  1246,  1249,  1252,  1255,
    1258,  1264,  1269,  1272,  1275,  1280,  1283,  1290,  1295,  1303,
    1320,  1324,  1333,  1340,  1345,  1352,  1359,  1366,  1372,  1373,
    1377,  1382,  1396,  1399,  1402,  1408,  1411,  1414,  1420,  1421,
    1422,  1423,  1429,  1430,  1431,  1432,  1433,  1434,  1436,  1439,
    1442,  1445,  1451,  1452,  1453,  1454,  1455,  1456,  1457,  1458,
    1459,  1460,  1461,  1462,  1463,  1464,  1465,  1466,  1467,  1468,
    1469,  1470,  1471,  1479,  1480,  1481,  1485,  1494,  1503,  1513,
    1525,  1526,  1530,  1533,  1536,  1539,  1545,  1560,  1565,  1579,
    1591,  1592,  1598,  1601,  1616,  1625,  1632,  1635,  1641,  1650,
    1658,  1661,  1667,  1670,  1676,  1694,  1697,  1703,  1704
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'",
  "'.'", "'['", "'*'", "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM",
  "FONT_FACE_SYM", "CHARSET_SYM", "NAMESPACE_SYM", "WEBKIT_RULE_SYM",
  "WEBKIT_DECLS_SYM", "WEBKIT_KEYFRAME_RULE_SYM", "WEBKIT_KEYFRAMES_SYM",
  "WEBKIT_VALUE_SYM", "WEBKIT_MEDIAQUERY_SYM", "WEBKIT_SELECTOR_SYM",
  "WEBKIT_REGION_RULE_SYM", "TOPLEFTCORNER_SYM", "TOPLEFT_SYM",
  "TOPCENTER_SYM", "TOPRIGHT_SYM", "TOPRIGHTCORNER_SYM",
  "BOTTOMLEFTCORNER_SYM", "BOTTOMLEFT_SYM", "BOTTOMCENTER_SYM",
  "BOTTOMRIGHT_SYM", "BOTTOMRIGHTCORNER_SYM", "LEFTTOP_SYM",
  "LEFTMIDDLE_SYM", "LEFTBOTTOM_SYM", "RIGHTTOP_SYM", "RIGHTMIDDLE_SYM",
  "RIGHTBOTTOM_SYM", "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY",
  "MEDIA_NOT", "MEDIA_AND", "REMS", "QEMS", "EMS", "EXS", "PXS", "CMS",
  "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS", "MSECS",
  "SECS", "HERTZ", "KHERTZ", "DIMEN", "INVALIDDIMEN", "PERCENTAGE",
  "FLOATTOKEN", "INTEGER", "VW", "VH", "VMIN", "URI", "FUNCTION",
  "ANYFUNCTION", "NOTFUNCTION", "CALCFUNCTION", "MINFUNCTION",
  "MAXFUNCTION", "UNICODERANGE", "'{'", "'}'", "';'", "'('", "')'", "','",
  "'+'", "'~'", "'>'", "'-'", "']'", "'='", "'/'", "'#'", "'%'", "$accept",
  "stylesheet", "webkit_rule", "webkit_keyframe_rule", "webkit_decls",
  "webkit_value", "webkit_mediaquery", "webkit_selector", "maybe_space",
  "maybe_sgml", "maybe_charset", "closing_brace", "charset",
  "ignored_charset", "rule_list", "valid_rule", "rule", "block_rule_list",
  "block_valid_rule", "block_rule", "import", "namespace",
  "maybe_ns_prefix", "string_or_uri", "media_feature", "maybe_media_value",
  "media_query_exp", "media_query_exp_list",
  "maybe_and_media_query_exp_list", "maybe_media_restrictor",
  "media_query", "maybe_media_list", "media_list", "media", "medium",
  "keyframes", "keyframe_name", "keyframes_rule", "keyframe_rule",
  "key_list", "key", "page", "page_selector", "declarations_and_margins",
  "margin_box", "$@1", "margin_sym", "font_face", "region_selector",
  "region", "combinator", "maybe_unary_operator", "unary_operator",
  "maybe_space_before_declaration", "before_ruleset",
  "before_rule_opening_brace", "ruleset", "selector_list",
  "selector_with_trailing_whitespace", "selector", "namespace_selector",
  "simple_selector", "simple_selector_list", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo_page", "pseudo", "declaration_list",
  "decl_list", "declaration", "property", "prio", "expr", "operator",
  "term", "unary_term", "function", "calc_func_term", "calc_func_operator",
  "calc_func_paren_expr", "calc_func_expr", "calc_func_expr_list",
  "calc_function", "min_or_max", "min_or_max_function", "save_block",
  "invalid_at", "invalid_rule", "invalid_block", "invalid_block_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    58,    46,    91,
      42,   124,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   123,   125,    59,    40,    41,    44,    43,   126,    62,
      45,    93,    61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   106,   107,   107,   107,   107,   107,   107,   107,   108,
     109,   110,   111,   112,   113,   114,   114,   115,   115,   115,
     116,   116,   117,   117,   118,   118,   118,   119,   120,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   122,   122,
     122,   122,   123,   123,   124,   124,   124,   124,   125,   125,
     125,   125,   125,   125,   126,   126,   126,   126,   126,   127,
     127,   127,   127,   128,   128,   129,   129,   130,   131,   131,
     132,   133,   133,   134,   134,   135,   135,   135,   136,   136,
     137,   137,   138,   138,   138,   139,   139,   140,   141,   142,
     142,   143,   143,   144,   145,   145,   146,   146,   147,   147,
     147,   148,   148,   148,   148,   149,   149,   151,   150,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   153,   153,   153,   154,   155,
     156,   156,   156,   157,   157,   158,   158,   159,   160,   161,
     162,   163,   163,   163,   164,   165,   165,   165,   165,   165,
     166,   166,   166,   167,   167,   167,   167,   167,   167,   168,
     168,   168,   169,   169,   170,   170,   170,   171,   171,   171,
     171,   171,   172,   173,   174,   174,   174,   174,   175,   175,
     175,   175,   175,   175,   176,   176,   177,   178,   178,   178,
     178,   178,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   180,   180,   180,   180,   180,   180,   180,   180,   181,
     181,   181,   181,   181,   181,   181,   181,   182,   183,   183,
     184,   184,   184,   184,   184,   185,   185,   185,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   188,   188,   188,   188,
     189,   189,   190,   190,   190,   190,   191,   192,   192,   192,
     192,   192,   193,   193,   194,   194,   195,   195,   196,   196,
     197,   197,   198,   198,   199,   200,   200,   201,   201
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     2,     2,     2,     2,     2,     6,
       6,     5,     5,     5,     5,     0,     2,     0,     2,     2,
       0,     1,     1,     1,     5,     3,     3,     5,     0,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     6,     6,     3,     3,     6,
       6,     3,     3,     0,     2,     1,     1,     2,     0,     4,
       7,     1,     5,     0,     3,     0,     1,     1,     1,     4,
       0,     1,     1,     4,     2,     7,     6,     2,     8,     1,
       1,     0,     3,     6,     1,     5,     1,     1,     8,     3,
       3,     1,     2,     1,     0,     1,     4,     0,     7,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     7,     3,     3,     1,     7,
       2,     2,     2,     1,     0,     1,     1,     1,     0,     0,
       6,     1,     4,     2,     2,     1,     1,     2,     3,     2,
       1,     2,     2,     1,     2,     1,     2,     3,     2,     1,
       5,     2,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     4,     8,     5,     9,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     6,
       6,     7,     6,     6,     1,     2,     1,     3,     1,     2,
       2,     3,     3,     4,     3,     5,     4,     4,     6,     5,
       2,     6,     6,     2,     3,     4,     2,     2,     2,     0,
       1,     3,     2,     3,     2,     2,     2,     0,     1,     2,
       2,     2,     2,     3,     2,     2,     2,     2,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     5,     4,     4,     3,
       1,     2,     2,     2,     2,     2,     6,     2,     3,     3,
       1,     2,     1,     4,     5,     3,     1,     1,     5,     3,
       1,     2,     3,     3,     2,     5,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      15,     0,     0,     0,     0,     0,     0,     0,    15,    15,
      15,    15,    15,    15,    20,    15,    15,    15,    15,    15,
      15,     1,     3,     8,     4,     5,     6,     7,    16,     0,
      17,    21,   138,   137,     0,     0,     0,    75,     0,     0,
       0,    28,     0,     0,    15,     0,     0,    15,     0,    15,
      36,    35,    31,    34,    32,    33,    37,     0,   198,    15,
      15,     0,     0,   194,     0,    97,    96,    15,    15,    94,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      15,   286,   287,    15,   136,   135,    15,    15,     0,     0,
     220,   228,   238,   239,    15,   240,    76,    77,    15,    71,
      78,    15,     0,   162,   168,   167,     0,     0,    15,   163,
     150,     0,   146,     0,     0,   145,   153,     0,   164,   169,
     170,   171,     0,    26,    25,    15,    19,    18,     0,     0,
       0,     0,   104,    75,     0,     0,     0,    63,     0,     0,
       0,    30,     0,    15,   297,     0,   217,   213,    11,   199,
     195,     0,    15,     0,   210,    15,   216,     0,     0,   230,
     231,   236,   262,   260,   259,   261,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     232,   244,   243,   242,   263,   264,   265,   234,     0,     0,
     235,   237,   241,    15,   229,   224,    12,    15,    15,     0,
       0,     0,     0,     0,     0,    13,   152,   187,     0,    15,
      15,    15,   172,     0,   151,   143,    14,    15,   147,   149,
     144,    15,    15,    15,     0,   162,   163,   156,     0,     0,
     166,   165,     0,     0,     0,    15,     0,    39,    38,    17,
      41,    40,    57,    58,    65,    66,    15,   100,    99,   101,
       0,    15,   103,    15,    82,     0,   127,   126,    15,    62,
      61,    15,     0,    90,    89,    15,     0,     0,     9,     0,
     204,   197,    15,     0,    15,     0,   201,    15,   202,     0,
      10,    15,    15,   269,    15,     0,   285,    15,     0,   270,
      15,   280,     0,   233,   226,   225,   221,   223,   289,     0,
       0,    15,    15,    15,    15,    73,   188,   134,     0,     0,
      15,     0,     0,     0,     0,   130,   131,   132,   148,     0,
      23,    22,   296,     0,    24,   294,     0,     0,    29,    80,
     102,   186,     0,    42,    84,    15,    15,     0,    64,    15,
       0,    15,    15,    15,   298,   207,     0,   206,   203,   215,
       0,     0,     0,   268,   267,    15,     0,   271,   277,   281,
      15,    15,     0,     0,    15,     0,    15,    15,    67,    68,
       0,    87,    15,    79,    15,    15,     0,   133,   159,     0,
      15,   173,    15,     0,   179,   180,   181,   182,   183,   174,
     178,    15,     0,     0,    15,   293,   292,     0,     0,    15,
       0,    42,    75,     0,     0,    15,    42,     0,   205,    15,
       0,    15,     0,     0,    95,   266,     0,   274,   284,   272,
     273,   275,   278,   279,   288,     0,    15,     0,    72,     0,
       0,     0,    15,   161,     0,     0,   176,    15,     0,   295,
       0,    55,    54,    56,     0,     0,   290,    48,    17,    52,
      51,    53,    47,    45,    46,    44,    86,    50,    49,     0,
      83,    15,    59,    60,    91,     0,     0,   208,   211,   218,
     212,    93,     0,     0,     0,    15,    74,   192,   190,     0,
     189,    15,   193,     0,   185,   184,    15,    27,     0,   105,
     291,    43,    85,   125,     0,   129,   140,    15,     0,    70,
     191,     0,    15,     0,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      98,    15,   107,    88,    15,   276,    69,   160,     0,   175,
       0,    15,    92,   177,   106,     0,    15,     0,     0,   108
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,    12,    13,    33,    41,
      30,   466,    31,   257,   148,    49,   259,   420,   467,   468,
     469,   470,   282,   266,   322,   447,   119,   120,   393,   121,
     274,   417,   275,   471,   325,   472,   285,   514,    67,    68,
      69,   473,   271,   508,   541,   551,   542,   474,   286,    56,
     244,   396,   108,    34,    57,   289,   475,   162,   132,   133,
     134,   135,   399,   136,   137,   138,   139,   333,   140,   411,
     506,   272,   141,    61,    62,    63,    64,   432,   109,   219,
     110,   111,   112,   310,   385,   311,   312,   320,   113,   114,
     115,   476,   477,   478,   164,   220
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -324
static const yytype_int16 yypact[] =
{
     520,   -50,    70,    74,    85,   206,   199,   316,  -324,  -324,
    -324,  -324,  -324,  -324,    27,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,   339,   339,   339,   339,   339,   339,  -324,   408,
    -324,  -324,   619,   339,   292,   312,  1178,   278,   494,   -37,
      46,   314,   359,   271,  -324,   134,   334,  -324,   343,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,   716,   -36,  -324,
    -324,   262,  1555,   142,   205,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  1649,  1003,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
     125,  -324,   266,   347,  -324,  -324,   405,   360,  -324,   358,
    -324,   100,   716,   336,   725,  -324,   422,   831,  -324,  -324,
    -324,  -324,   397,  -324,  -324,  -324,  -324,  -324,  1609,   196,
     364,   319,   365,   258,   330,   148,   333,   154,   451,   716,
     178,  -324,    51,  -324,  -324,   400,   339,   339,  -324,   368,
     369,  1096,  -324,  1479,  -324,  -324,  -324,   216,   114,   339,
     339,   339,   339,   339,   339,   339,   339,   339,   339,   339,
     339,   339,   339,   339,   339,   339,   339,   339,   339,   339,
     339,   339,   339,   339,   339,   339,   339,   339,  1097,  1227,
     339,   339,   339,  -324,  -324,  -324,  -324,  -324,  -324,  1386,
     523,  1271,   372,   182,   378,  -324,  -324,  -324,   445,  -324,
    -324,  -324,  -324,   526,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,   716,  -324,  -324,   422,   840,   939,
    -324,  -324,   102,   207,   396,  -324,   481,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,   466,
     484,  -324,  -324,  -324,  -324,   165,  -324,  -324,  -324,  -324,
    -324,  -324,   105,  -324,  -324,  -324,   403,   184,  -324,   411,
     339,   374,  -324,   502,  -324,   396,   339,  -324,   339,   715,
    -324,  -324,  -324,  -324,  -324,   809,  -324,  -324,  1675,  -324,
    -324,  -324,   150,   339,   339,   339,  -324,   396,  -324,   104,
     311,  -324,  -324,  -324,  -324,   448,  -324,   177,   494,   494,
     347,   358,   495,    54,   494,   339,   339,   339,  -324,  1024,
    -324,  -324,  -324,   515,  -324,  -324,   391,   395,   314,   361,
    -324,  -324,   169,   339,  -324,  -324,  -324,   392,   339,  -324,
     174,  -324,  -324,  -324,  -324,   339,   407,   339,   339,  1386,
     621,   392,   312,   339,  -324,  -324,  1315,  -324,   339,  -324,
    -324,  -324,   514,   516,  -324,  1605,  -324,  -324,   339,   298,
      31,   339,  -324,  -324,  -324,  -324,   441,  -324,  -324,   163,
    -324,   339,  -324,   101,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,   352,   102,  -324,  -324,  -324,    69,    38,  -324,
     453,   339,   278,   433,   249,  -324,   339,   292,   339,  -324,
     621,  -324,  1497,   435,  -324,   339,   149,   339,   339,  -324,
    -324,   339,  -324,  -324,   339,  1315,  -324,   434,  -324,    31,
     173,   211,  -324,  -324,   203,   214,  -324,  -324,   480,  -324,
     243,  -324,  -324,  -324,   392,   102,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,   453,
    -324,  -324,  -324,  -324,   339,   453,    66,   339,  -324,   339,
    -324,  -324,   219,   231,  1178,  -324,   125,  -324,  -324,   223,
    -324,  -324,  -324,   480,  -324,  -324,  -324,  -324,  1390,  -324,
    -324,   314,  -324,   339,   273,  -324,  -324,  -324,   906,   339,
    -324,   494,  -324,    35,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,   339,   339,  -324,    45,  -324,
     392,  -324,   339,  -324,  -324,   204,  -324,   392,    66,  -324
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,     0,  -253,
    -324,  -229,  -324,  -324,  -324,   384,  -324,   -83,  -324,  -324,
     -25,   -10,  -324,   251,  -324,  -324,   144,    91,  -324,  -324,
     -34,  -324,   193,    -8,  -324,    -5,  -324,  -324,    29,  -324,
     183,    -4,  -324,  -324,  -324,  -324,  -324,     1,  -324,  -324,
    -324,  -324,  -205,   195,  -324,  -324,   501,   -33,  -324,   241,
     344,  -131,  -324,   465,   -99,  -116,  -324,   247,  -324,   222,
     106,   348,  -324,  -323,  -324,   546,  -324,   200,  -183,  -324,
     410,   -94,  -324,   246,  -324,   250,  -219,  -324,  -324,  -324,
    -324,  -290,   489,   490,   113,   -32
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -284
static const yytype_int16 yytable[] =
{
      14,   238,   319,   122,   308,   131,   348,    50,    22,    23,
      24,    25,    26,    27,   214,    32,   308,    35,    36,    37,
      38,   251,    51,   342,    52,   305,   165,    53,    54,    40,
     171,   173,    28,    55,   423,   248,    28,   249,   -81,   354,
      28,    15,   150,   152,   153,   155,   157,   158,   433,   160,
      28,    28,   235,    29,   142,   142,   143,   163,   145,   166,
     167,   404,   405,   406,   407,   408,   340,   177,   178,   461,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   235,   340,   210,   486,   379,   211,   212,   404,   405,
     406,   407,   408,   338,   221,   309,   370,   264,   222,    28,
     223,   224,   397,    50,   380,   118,   287,   309,   233,   -81,
     -15,   -81,   251,   251,   356,   154,   549,   293,    51,   -15,
      52,   509,  -139,    53,    54,   253,   553,   237,   339,    55,
     379,   379,   144,    28,   -15,   409,   410,   436,   341,    28,
     142,    16,   462,   290,   453,    17,   354,   281,   -15,   380,
     380,   308,   296,   298,    28,   299,    18,   176,    28,    28,
     308,   -15,    28,    28,   459,   235,   430,    28,   265,   512,
     394,   395,   236,   142,   341,   515,   237,   398,   400,  -282,
    -282,   382,   456,   410,   383,   301,   174,   384,    28,    28,
     302,    19,    28,   313,   377,   511,    28,   314,   315,    28,
     343,    28,   175,   251,    28,   -15,   493,   554,    28,   327,
     328,   329,   379,   142,   558,   172,   510,   334,   323,   278,
     308,   335,   336,   337,   -15,   381,   382,   382,    28,   383,
     383,   380,   384,   384,    28,   346,   355,   516,   -15,   -15,
     419,   356,   263,    28,   268,   425,   349,   277,   497,   280,
     288,   352,   151,   353,   104,  -128,   -15,   105,   357,   540,
     237,   358,   309,    28,   -15,   360,    65,   142,   -15,   262,
      20,   309,   365,    58,   367,   556,   142,   368,   500,   501,
     344,   371,   372,    28,   373,    59,   498,   376,   300,   502,
     378,   518,   116,   117,   517,   446,    21,    28,   520,   146,
     147,   388,   389,   390,   391,    65,  -283,  -283,   382,   559,
     401,   383,   116,   117,   384,   156,   507,   239,   479,   -15,
     142,   240,   482,   485,    28,    60,   -15,   -15,   159,   273,
      66,   309,   118,   239,   168,   421,   422,   240,   225,   424,
     149,   426,   -15,   428,   -15,   543,    28,   345,   226,    28,
      28,   -15,   118,   232,   -75,   435,   264,    28,   269,   234,
     437,   438,   270,    28,   441,   321,   444,   445,   480,    66,
     547,   324,   449,    58,   450,   451,    28,    28,   252,   454,
     455,   291,   401,   414,   364,    59,   386,   387,   364,    39,
     142,   458,   267,   -15,   460,   116,   117,   -15,   227,   464,
     -15,   142,   228,   276,   142,   484,   279,  -141,  -141,   487,
     364,   489,  -141,   241,   242,   243,   492,   124,   125,   126,
     127,   128,   -15,  -142,  -142,    60,   494,   265,  -142,   241,
     242,   243,   499,   340,   465,   118,    28,   503,   326,   142,
     416,   292,   294,   283,   284,   142,   123,   363,   124,   125,
     126,   127,   128,   129,   130,    42,    43,    44,    45,   364,
      46,   513,   347,   270,    47,    28,   142,   142,   415,   229,
     230,   231,   504,   505,   361,   519,   223,   351,   142,    28,
     429,   521,   362,   366,   392,   256,   523,   123,   402,   124,
     125,   126,   127,   128,   129,   130,   413,   545,   546,   439,
     452,   440,   548,  -222,   317,   481,   364,   491,  -222,   495,
     463,    28,   258,   359,   448,  -222,  -222,   483,  -222,   330,
     496,   550,   418,   544,   552,   341,   331,   130,     1,     2,
       3,   555,     4,     5,     6,   434,   557,   427,   161,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,   412,  -222,   332,   345,   403,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,   247,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,   170,   522,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,   350,  -222,  -222,
    -222,  -219,   215,  -222,    28,   457,  -222,  -222,  -222,   316,
     488,   442,     0,  -227,  -227,   443,  -227,   260,   261,     0,
       0,    42,    43,    44,    45,     0,    46,     0,     0,     0,
      47,     0,     0,     0,    48,     0,     0,  -219,  -219,  -219,
    -219,  -219,  -219,  -219,  -219,  -219,  -219,  -219,  -219,  -219,
    -219,  -219,  -219,     0,   431,     0,     0,     0,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,     0,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,     0,     0,  -227,  -227,
    -227,  -227,   142,  -219,  -219,  -214,   369,   217,  -227,     0,
      28,  -227,     0,     0,   218,  -227,  -227,    70,    71,   123,
      72,   124,   125,   126,   127,   128,   129,   130,   245,     0,
     124,   125,   126,   127,   128,   246,     0,     0,     0,     0,
       0,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,
    -214,  -214,  -214,  -214,  -214,  -214,  -214,     0,     0,     0,
       0,     0,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,     0,    92,    93,    94,    95,    96,    97,    98,    99,
       0,     0,   100,   101,   102,   103,  -214,  -214,  -214,   374,
     215,     0,   104,     0,     0,   105,     0,     0,     0,   106,
     107,  -227,  -227,     0,  -227,     0,     0,     0,     0,     0,
       0,     0,   250,     0,     0,     0,  -155,     0,     0,     0,
       0,   250,     0,     0,     0,  -158,   124,   125,   126,   127,
     128,     0,     0,     0,     0,   124,   125,   126,   127,   128,
       0,     0,     0,     0,     0,     0,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,     0,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,     0,     0,  -227,  -227,  -227,  -227,
     142,     0,     0,     0,   375,   217,  -227,   215,     0,  -227,
       0,   -15,   218,  -227,  -227,     0,     0,     0,  -227,  -227,
       0,  -227,  -155,  -155,     0,     0,  -155,  -155,  -155,  -155,
    -155,  -158,  -158,     0,     0,  -158,  -158,  -158,  -158,  -158,
     250,     0,     0,     0,  -154,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   124,   125,   126,   127,   128,     0,
       0,     0,     0,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,     0,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,     0,     0,  -227,  -227,  -227,  -227,   142,     0,     0,
       0,   -15,   217,  -227,   215,     0,  -227,     0,     0,   218,
    -227,  -227,     0,     0,     0,  -227,  -227,     0,  -227,     0,
       0,     0,     0,     0,     0,   250,     0,     0,     0,  -157,
    -154,  -154,     0,     0,  -154,  -154,  -154,  -154,  -154,   124,
     125,   126,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,     0,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,     0,     0,
    -227,  -227,  -227,  -227,   142,   216,  -200,   295,   303,   217,
    -227,     0,    28,  -227,     0,     0,   218,  -227,  -227,    70,
      71,     0,    72,     0,     0,  -157,  -157,     0,     0,  -157,
    -157,  -157,  -157,  -157,     0,     0,     0,     0,     0,     0,
       0,     0,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,     0,     0,
       0,     0,     0,     0,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,     0,    92,    93,    94,    95,    96,    97,
      98,    99,     0,    28,   100,   101,   102,   103,  -200,     0,
      70,    71,   304,    72,   104,     0,     0,   105,     0,     0,
       0,   106,   107,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   306,     0,
       0,     0,    28,     0,     0,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,     0,    92,    93,    94,    95,    96,
      97,    98,    99,     0,     0,   100,   101,   102,   103,     0,
       0,     0,   318,     0,     0,   104,    28,     0,   105,     0,
       0,     0,   106,   107,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,    92,    93,    94,    95,    96,    97,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,   307,     0,     0,   104,     0,     0,   105,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,     0,     0,    92,    93,
      94,    95,    96,    97,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   307,     0,     0,   104,     0,
       0,   105,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
     340,     0,    92,    93,    94,    95,    96,    97,    70,    71,
       0,    72,     0,     0,     0,     0,     0,     0,     0,   307,
       0,     0,   104,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     538,   539,     0,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,     0,    92,    93,    94,    95,    96,    97,    98,
      99,     0,     0,   100,   101,   102,   103,     0,     0,   -15,
     295,     0,   341,   104,   -15,     0,   105,     0,     0,     0,
     106,   107,   -15,     0,     0,     0,     0,  -209,   490,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   -15,   -15,   -15,   -15,   -15,
     -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,
     -15,     0,   -15,  -209,  -209,  -209,  -209,  -209,  -209,  -209,
    -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,     0,
       0,     0,     0,     0,     0,  -196,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,     0,
     -15,   -15,   297,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -209,  -209,
    -209,  -196,  -196,  -196,  -196,  -196,  -196,  -196,  -196,  -196,
    -196,  -196,  -196,  -196,  -196,  -196,  -196,     0,    60,    -2,
     254,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -138,     0,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,    42,    43,    44,    45,   255,    46,     0,     0,     0,
      47,     0,     0,     0,    48,     0,   142,  -196,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   256,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
       0,     0,    92,    93,    94,    95,    96,    97,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   307,
       0,     0,   104,     0,     0,   105,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,   213,     0,    92,    93,    94,    95,
      96,    97,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
       0,     0,    92,    93,    94,    95,    96,    97
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-324))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,   132,   221,    37,   209,    38,   259,    32,     8,     9,
      10,    11,    12,    13,   108,    15,   221,    17,    18,    19,
      20,   137,    32,   252,    32,   208,    58,    32,    32,    29,
      62,    63,     5,    32,   357,   134,     5,   136,     0,     1,
       5,    91,    42,    43,    44,    45,    46,    47,   371,    49,
       5,     5,     1,    26,    91,    91,    93,    93,    12,    59,
      60,     7,     8,     9,    10,    11,     0,    67,    68,     0,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,     1,     0,   103,   427,     1,   106,   107,     7,     8,
       9,    10,    11,   244,   114,   209,   299,    12,   118,     5,
     120,   121,   327,   148,    20,    94,   159,   221,   128,    91,
       5,    93,   248,   249,    96,     1,   101,   169,   148,     5,
     148,   464,    91,   148,   148,   145,   101,    96,   247,   148,
       1,     1,    39,     5,     5,   101,   102,   376,    92,     5,
      91,    91,    93,   163,     1,    91,     1,    13,     5,    20,
      20,   376,   172,   173,     5,   175,    91,    64,     5,     5,
     385,    56,     5,     5,   413,     1,   369,     5,    83,   479,
      13,    14,    92,    91,    92,   485,    96,   328,   329,    95,
      96,    97,   101,   102,   100,    91,     1,   103,     5,     5,
      96,     5,     5,   213,   308,   468,     5,   217,   218,     5,
     252,     5,    17,   339,     5,    91,   445,   550,     5,   229,
     230,   231,     1,    91,   557,    93,   465,   237,    56,    91,
     445,   241,   242,   243,    95,    95,    97,    97,     5,   100,
     100,    20,   103,   103,     5,   255,    91,   486,    95,    96,
      91,    96,   149,     5,   151,    91,   266,   154,    95,   156,
      92,   271,     1,   273,    97,    91,     5,   100,   278,   508,
      96,   281,   376,     5,    13,   285,    13,    91,    17,    93,
      91,   385,   292,     1,   294,    91,    91,   297,    95,    96,
      93,   301,   302,     5,   304,    13,    95,   307,    92,    95,
     310,   494,    54,    55,    95,    17,     0,     5,    95,     5,
       6,   321,   322,   323,   324,    13,    95,    96,    97,   558,
     330,   100,    54,    55,   103,     1,    93,     1,   421,     5,
      91,     5,    93,   426,     5,    53,    12,    13,     5,    91,
      77,   445,    94,     1,    92,   355,   356,     5,    92,   359,
       1,   361,    91,   363,     5,    92,     5,   254,    21,     5,
       5,    12,    94,    13,    13,   375,    12,     5,    13,    21,
     380,   381,    17,     5,   384,    13,   386,   387,   422,    77,
     521,    13,   392,     1,   394,   395,     5,     5,     1,   399,
     400,     1,   402,    12,   291,    13,    95,    96,   295,     1,
      91,   411,    93,     5,   414,    54,    55,    83,    13,   419,
      12,    91,    17,    93,    91,   425,    93,    91,    92,   429,
     317,   431,    96,    97,    98,    99,   436,    15,    16,    17,
      18,    19,    83,    91,    92,    53,   446,    83,    96,    97,
      98,    99,   452,     0,     1,    94,     5,   457,    13,    91,
     347,    93,    93,    12,    13,    91,    13,    93,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,   366,
      27,   481,     1,    17,    31,     5,    91,    91,    93,    84,
      85,    86,    12,    13,    91,   495,   496,    13,    91,     5,
      93,   501,    91,     1,    56,    52,   506,    13,    13,    15,
      16,    17,    18,    19,    20,    21,     1,   517,   518,     5,
      79,     5,   522,     0,     1,    92,   413,    92,     5,    95,
     417,     5,   148,   282,   390,    12,    13,   424,    15,    13,
     449,   541,   349,   514,   544,    92,    20,    21,    28,    29,
      30,   551,    32,    33,    34,   372,   556,   362,    57,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,   334,    53,   233,   465,   332,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,   134,
      77,    78,    79,    80,    81,    82,    83,    84,    62,   503,
      87,    88,    89,    90,    91,    92,    93,   269,    95,    96,
      97,     0,     1,   100,     5,   403,   103,   104,   105,   219,
     430,   385,    -1,    12,    13,   385,    15,   148,   148,    -1,
      -1,    22,    23,    24,    25,    -1,    27,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    35,    -1,    -1,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    53,    -1,    -1,    -1,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    -1,    77,    78,
      79,    80,    81,    82,    83,    84,    -1,    -1,    87,    88,
      89,    90,    91,    92,    93,     0,     1,    96,    97,    -1,
       5,   100,    -1,    -1,   103,   104,   105,    12,    13,    13,
      15,    15,    16,    17,    18,    19,    20,    21,    13,    -1,
      15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    -1,    -1,    -1,
      -1,    -1,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    -1,    77,    78,    79,    80,    81,    82,    83,    84,
      -1,    -1,    87,    88,    89,    90,    91,    92,    93,     0,
       1,    -1,    97,    -1,    -1,   100,    -1,    -1,    -1,   104,
     105,    12,    13,    -1,    15,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,    -1,    -1,     5,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    -1,     5,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    -1,    77,    78,    79,    80,
      81,    82,    83,    84,    -1,    -1,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    95,    96,    97,     1,    -1,   100,
      -1,     5,   103,   104,   105,    -1,    -1,    -1,    12,    13,
      -1,    15,    91,    92,    -1,    -1,    95,    96,    97,    98,
      99,    91,    92,    -1,    -1,    95,    96,    97,    98,    99,
       1,    -1,    -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    -1,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    -1,    77,    78,    79,    80,    81,    82,    83,
      84,    -1,    -1,    87,    88,    89,    90,    91,    -1,    -1,
      -1,    95,    96,    97,     1,    -1,   100,    -1,    -1,   103,
     104,   105,    -1,    -1,    -1,    12,    13,    -1,    15,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,     5,
      91,    92,    -1,    -1,    95,    96,    97,    98,    99,    15,
      16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    -1,
      77,    78,    79,    80,    81,    82,    83,    84,    -1,    -1,
      87,    88,    89,    90,    91,    92,     0,     1,     1,    96,
      97,    -1,     5,   100,    -1,    -1,   103,   104,   105,    12,
      13,    -1,    15,    -1,    -1,    91,    92,    -1,    -1,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    -1,    77,    78,    79,    80,    81,    82,
      83,    84,    -1,     5,    87,    88,    89,    90,    92,    -1,
      12,    13,    95,    15,    97,    -1,    -1,   100,    -1,    -1,
      -1,   104,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,
      -1,    -1,     5,    -1,    -1,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    -1,    77,    78,    79,    80,    81,
      82,    83,    84,    -1,    -1,    87,    88,    89,    90,    -1,
      -1,    -1,     1,    -1,    -1,    97,     5,    -1,   100,    -1,
      -1,    -1,   104,   105,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,    77,    78,    79,    80,    81,    82,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       5,    94,    -1,    -1,    97,    -1,    -1,   100,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    -1,    -1,    77,    78,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    97,    -1,
      -1,   100,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
       0,    -1,    77,    78,    79,    80,    81,    82,    12,    13,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    97,    -1,    -1,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    -1,    77,    78,    79,    80,    81,    82,    83,
      84,    -1,    -1,    87,    88,    89,    90,    -1,    -1,     0,
       1,    -1,    92,    97,     5,    -1,   100,    -1,    -1,    -1,
     104,   105,    13,    -1,    -1,    -1,    -1,     0,     1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    -1,    53,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    -1,    -1,    -1,     0,     1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,    92,
      93,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    -1,    53,     0,
       1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    13,    -1,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    35,    -1,    91,    92,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    77,    78,    79,    80,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    97,    -1,    -1,   100,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    -1,    77,    78,    79,    80,
      81,    82,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    77,    78,    79,    80,    81,    82
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    28,    29,    30,    32,    33,    34,   107,   108,   109,
     110,   111,   112,   113,   114,    91,    91,    91,    91,     5,
      91,     0,   114,   114,   114,   114,   114,   114,     5,    26,
     116,   118,   114,   114,   159,   114,   114,   114,   114,     1,
     114,   115,    22,    23,    24,    25,    27,    31,    35,   121,
     126,   127,   139,   141,   147,   153,   155,   160,     1,    13,
      53,   179,   180,   181,   182,    13,    77,   144,   145,   146,
      12,    13,    15,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    77,    78,    79,    80,    81,    82,    83,    84,
      87,    88,    89,    90,    97,   100,   104,   105,   158,   184,
     186,   187,   188,   194,   195,   196,    54,    55,    94,   132,
     133,   135,   136,    13,    15,    16,    17,    18,    19,    20,
      21,   163,   164,   165,   166,   167,   169,   170,   171,   172,
     174,   178,    91,    93,   200,    12,     5,     6,   120,     1,
     114,     1,   114,   114,     1,   114,     1,   114,   114,     5,
     114,   162,   163,    93,   200,   201,   114,   114,    92,     1,
     181,   201,    93,   201,     1,    17,   200,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,    75,   187,     1,    92,    96,   103,   185,
     201,   114,   114,   114,   114,    92,    21,    13,    17,    84,
      85,    86,    13,   114,    21,     1,    92,    96,   167,     1,
       5,    97,    98,    99,   156,    13,    20,   169,   170,   170,
       1,   171,     1,   114,     1,    26,    52,   119,   121,   122,
     198,   199,    93,   200,    12,    83,   129,    93,   200,    13,
      17,   148,   177,    91,   136,   138,    93,   200,    91,    93,
     200,    13,   128,    12,    13,   142,   154,   163,    92,   161,
     114,     1,    93,   201,    93,     1,   114,    93,   114,   114,
      92,    91,    96,     1,    95,   184,     1,    94,   158,   187,
     189,   191,   192,   114,   114,   114,   186,     1,     1,   192,
     193,    13,   130,    56,    13,   140,    13,   114,   114,   114,
      13,    20,   166,   173,   114,   114,   114,   114,   167,   170,
       0,    92,   117,   201,    93,   200,   114,     1,   115,   114,
     177,    13,   114,   114,     1,    91,    96,   114,   114,   129,
     114,    91,    91,    93,   200,   114,     1,   114,   114,     1,
     184,   114,   114,   114,     0,    95,   114,   187,   114,     1,
      20,    95,    97,   100,   103,   190,    95,    96,   114,   114,
     114,   114,    56,   134,    13,    14,   157,   158,   167,   168,
     167,   114,    13,   173,     7,     8,     9,    10,    11,   101,
     102,   175,   165,     1,    12,    93,   200,   137,   138,    91,
     123,   114,   114,   179,   114,    91,   114,   159,   114,    93,
     184,    53,   183,   179,   146,   114,   192,   114,   114,     5,
       5,   114,   189,   191,   114,   114,    17,   131,   132,   114,
     114,   114,    79,     1,   114,   114,   101,   175,   114,   117,
     114,     0,    93,   200,   114,     1,   117,   124,   125,   126,
     127,   139,   141,   147,   153,   162,   197,   198,   199,   123,
     136,    92,    93,   200,   114,   123,   179,   114,   183,   114,
       1,    92,   114,   192,   114,    95,   133,    95,    95,   114,
      95,    96,    95,   114,    12,    13,   176,    93,   149,   179,
     117,   115,   197,   114,   143,   197,   117,    95,   184,   114,
      95,   114,   176,   114,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
     117,   150,   152,    92,   144,   114,   114,   167,   114,   101,
     114,   151,   114,   101,   179,   114,    91,   114,   179,   117
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 9:

/* Line 1806 of yacc.c  */
#line 310 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 316 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframe);
    }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 322 "Source/WebCore/css/CSSGrammar.y"
    {
        /* can be empty */
    }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 328 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_parsedProperties.size();
            if (!p->parseValue(p->m_id, p->m_important))
                p->rollbackLastProperties(p->m_parsedProperties.size() - oldParsedProperties);
            p->m_valueList = nullptr;
        }
    }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 341 "Source/WebCore/css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 348 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 370 "Source/WebCore/css/CSSGrammar.y"
    {
  }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 380 "Source/WebCore/css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if (p->m_styleSheet)
         p->m_styleSheet->parserSetEncodingFromCharsetRule((yyvsp[(3) - (5)].string));
     (yyval.rule) = 0;
  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 386 "Source/WebCore/css/CSSGrammar.y"
    {
  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 388 "Source/WebCore/css/CSSGrammar.y"
    {
  }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 393 "Source/WebCore/css/CSSGrammar.y"
    {
        // Ignore any @charset rule not at the beginning of the style sheet.
        (yyval.rule) = 0;
    }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 401 "Source/WebCore/css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->parserAppendRule((yyvsp[(2) - (3)].rule));
 }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 409 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(2) - (2)].rule);
    }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 422 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_hadSyntacticallyValidCSSRule = true;
    }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 431 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.ruleList) = 0; }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 432 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if ((yyvsp[(2) - (3)].rule)) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
          (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
      }
  }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 460 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 463 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 466 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 469 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 472 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 478 "Source/WebCore/css/CSSGrammar.y"
    {
    static_cast<CSSParser*>(parser)->addNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
    (yyval.rule) = 0;
}
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 482 "Source/WebCore/css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
}
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 485 "Source/WebCore/css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
}
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 488 "Source/WebCore/css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
}
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 494 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.string).characters = 0; }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 495 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 504 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 510 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 513 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 519 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 526 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 531 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 538 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 541 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 547 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 550 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 553 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 559 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 564 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 572 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaQuerySet();
     }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 579 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaQuerySet();
        (yyval.mediaList)->addMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
        p->updateLastMediaLine((yyval.mediaList));
    }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 585 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.mediaList)->addMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
            p->updateLastMediaLine((yyval.mediaList));
        }
    }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 593 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 599 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 602 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 608 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 614 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 626 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 627 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframe))
            (yyval.keyframesRule)->parserAppendKeyframe((yyvsp[(2) - (3)].keyframe));
    }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 635 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.keyframe) = static_cast<CSSParser*>(parser)->createKeyframe((yyvsp[(1) - (6)].valueList));
    }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 641 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 646 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 655 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 656 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (equalIgnoringCase("from", str.characters, str.length))
            (yyval.value).fValue = 0;
        else if (equalIgnoringCase("to", str.characters, str.length))
            (yyval.value).fValue = 100;
        else
            YYERROR;
    }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 670 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(3) - (8)].selector))
            (yyval.rule) = p->createPageRule(p->sinkFloatingSelector((yyvsp[(3) - (8)].selector)));
        else {
            // Clear properties in the invalid @page rule.
            p->clearProperties();
            // Also clear margin at-rules here once we fully implement margin at-rules parsing.
            (yyval.rule) = 0;
        }
    }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 681 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 684 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 690 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setTag(QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace));
        (yyval.selector)->setForPage();
    }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 696 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            (yyval.selector)->setTag(QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace));
            (yyval.selector)->setForPage();
        }
    }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 704 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        if ((yyval.selector))
            (yyval.selector)->setForPage();
    }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 709 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setForPage();
    }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 722 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->startDeclarationsForMarginBox();
    }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 724 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMarginAtRule((yyvsp[(1) - (7)].marginBox));
    }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 730 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftCornerMarginBox;
    }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 733 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftMarginBox;
    }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 736 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopCenterMarginBox;
    }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 739 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightMarginBox;
    }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 742 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightCornerMarginBox;
    }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 745 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftCornerMarginBox;
    }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 748 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftMarginBox;
    }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 751 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomCenterMarginBox;
    }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 754 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightMarginBox;
    }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 757 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightCornerMarginBox;
    }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 760 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftTopMarginBox;
    }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 763 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftMiddleMarginBox;
    }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 766 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftBottomMarginBox;
    }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 769 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightTopMarginBox;
    }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 772 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightMiddleMarginBox;
    }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 775 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightBottomMarginBox;
    }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 782 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 785 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 788 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 794 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selectorList)) {
            static_cast<CSSParser*>(parser)->setReusableRegionSelectorVector((yyvsp[(1) - (1)].selectorList));
            (yyval.selectorList) = static_cast<CSSParser*>(parser)->reusableRegionSelectorVector();
        }
        else
            (yyval.selectorList) = 0;
    }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 805 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(3) - (7)].selectorList))
            (yyval.rule) = static_cast<CSSParser*>(parser)->createRegionRule((yyvsp[(3) - (7)].selectorList), (yyvsp[(6) - (7)].ruleList));
        else
            (yyval.rule) = 0;
    }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 814 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 815 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 816 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 820 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].integer); }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 821 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 825 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.integer) = -1; }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 826 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 830 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
    }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 837 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markSelectorListStart();
    }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 844 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markSelectorListEnd();
    }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 851 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.rule) = p->createStyleRule((yyvsp[(1) - (6)].selectorList));
    }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 858 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->reusableSelectorVector();
            (yyval.selectorList)->shrink(0);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
            p->updateLastSelectorLineAndPosition();
        }
    }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 867 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
            p->updateLastSelectorLineAndPosition();
        } else
            (yyval.selectorList) = 0;
    }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 876 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 882 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 888 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 892 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 896 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if (!(yyvsp[(1) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSParserSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->setRelation(CSSSelector::Descendant);
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (2)].selector)));
        }
    }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 909 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if (!(yyvsp[(1) - (3)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSParserSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->setRelation((yyvsp[(2) - (3)].relation));
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
        }
    }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 922 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 928 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 929 "Source/WebCore/css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 930 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 934 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setTag(QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace));
    }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 939 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector))
            static_cast<CSSParser*>(parser)->updateSpecifiersWithElementName(nullAtom, (yyvsp[(1) - (2)].string), (yyval.selector));
    }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 944 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        if ((yyval.selector))
            static_cast<CSSParser*>(parser)->updateSpecifiersWithElementName(nullAtom, starAtom, (yyval.selector));
    }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 949 "Source/WebCore/css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        if (p->m_styleSheet)
            (yyval.selector)->setTag(QualifiedName(namespacePrefix, (yyvsp[(2) - (2)].string),
                                      p->m_styleSheet->determineNamespace(namespacePrefix)));
        else // FIXME: Shouldn't this case be an error?
            (yyval.selector)->setTag(QualifiedName(nullAtom, (yyvsp[(2) - (2)].string), p->m_defaultNamespace));
    }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 959 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if ((yyval.selector))
            static_cast<CSSParser*>(parser)->updateSpecifiersWithElementName((yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].string), (yyval.selector));
    }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 964 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector))
            static_cast<CSSParser*>(parser)->updateSpecifiersWithElementName((yyvsp[(1) - (2)].string), starAtom, (yyval.selector));
    }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 972 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->createFloatingSelectorVector();
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
        } else
            (yyval.selectorList) = 0;
    }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 980 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (5)].selectorList) && (yyvsp[(5) - (5)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (5)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(5) - (5)].selector)));
        } else
            (yyval.selectorList) = 0;
    }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 988 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 994 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->findDocument();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 1002 "Source/WebCore/css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 1010 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 1013 "Source/WebCore/css/CSSGrammar.y"
    {
        if (!(yyvsp[(2) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyvsp[(1) - (2)].selector))
            (yyval.selector) = static_cast<CSSParser*>(parser)->updateSpecifiers((yyvsp[(1) - (2)].selector), (yyvsp[(2) - (2)].selector));
    }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 1019 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 1025 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::Id);
        if (p->m_cssParserMode == CSSQuirksMode || p->m_cssParserMode == SVGAttributeMode)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
    }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 1033 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).characters[0] >= '0' && (yyvsp[(1) - (1)].string).characters[0] <= '9') {
            (yyval.selector) = 0;
        } else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->setMatch(CSSSelector::Id);
            if (p->m_cssParserMode == CSSQuirksMode || p->m_cssParserMode == SVGAttributeMode)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
        }
    }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 1051 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::Class);
        if (p->m_cssParserMode == CSSQuirksMode || p->m_cssParserMode == SVGAttributeMode)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
    }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 1062 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->findDocument();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 1073 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 1078 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->setMatch((CSSSelector::Match)(yyvsp[(4) - (8)].integer));
        (yyval.selector)->setValue((yyvsp[(6) - (8)].string));
    }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 1084 "Source/WebCore/css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 1092 "Source/WebCore/css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (9)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (9)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->setMatch((CSSSelector::Match)(yyvsp[(5) - (9)].integer));
        (yyval.selector)->setValue((yyvsp[(7) - (9)].string));
    }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 1104 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 1107 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 1110 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 1113 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 1116 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 1119 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 1130 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PagePseudoClass);
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
    }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 1141 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
    }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 1150 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoElement);
        (yyvsp[(3) - (3)].string).lower();
        (yyval.selector)->setValue((yyvsp[(3) - (3)].string));
        // FIXME: This call is needed to force selector to compute the pseudoType early enough.
        (yyval.selector)->pseudoType();
    }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 1163 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (6)].selectorList)) {
            CSSParser *p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->setMatch(CSSSelector::PseudoClass);
            (yyval.selector)->adoptSelectorVector(*p->sinkFloatingSelectorVector((yyvsp[(4) - (6)].selectorList)));
            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
            CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
            if (type != CSSSelector::PseudoAny)
                (yyval.selector) = 0;
        } else
            (yyval.selector) = 0;
    }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 1178 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
    }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 1189 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument(String::number((yyvsp[(4) - (7)].integer) * (yyvsp[(5) - (7)].number)));
        (yyval.selector)->setValue((yyvsp[(2) - (7)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
    }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 1200 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        (yyvsp[(2) - (6)].string).lower();
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (!isValidNthToken((yyvsp[(4) - (6)].string)))
                (yyval.selector) = 0;
        }
    }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 1219 "Source/WebCore/css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector) || !(yyvsp[(4) - (6)].selector)->isSimple())
            (yyval.selector) = 0;
        else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->setMatch(CSSSelector::PseudoClass);

            Vector<OwnPtr<CSSParserSelector> > selectorVector;
            selectorVector.append(p->sinkFloatingSelector((yyvsp[(4) - (6)].selector)));
            (yyval.selector)->adoptSelectorVector(selectorVector);

            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        }
    }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 1238 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 1241 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 1246 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 1249 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 1252 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 1255 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 1258 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 1264 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1269 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 1272 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 1275 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
        (yyval.boolean) = false;
    }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 1280 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 1283 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 1290 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 1295 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyStart();
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 1303 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        bool isPropertyParsed = false;
        if ((yyvsp[(1) - (5)].integer) && (yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_parsedProperties.size();
            (yyval.boolean) = p->parseValue((yyvsp[(1) - (5)].integer), (yyvsp[(5) - (5)].boolean));
            if (!(yyval.boolean))
                p->rollbackLastProperties(p->m_parsedProperties.size() - oldParsedProperties);
            else
                isPropertyParsed = true;
            p->m_valueList = nullptr;
        }
        p->markPropertyEnd((yyvsp[(5) - (5)].boolean), isPropertyParsed);
    }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1320 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 1324 "Source/WebCore/css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 1333 "Source/WebCore/css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1340 "Source/WebCore/css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    }
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 1345 "Source/WebCore/css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1352 "Source/WebCore/css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->markPropertyEnd(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1359 "Source/WebCore/css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1366 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1372 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.boolean) = true; }
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 1373 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1377 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    }
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 1382 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if ((yyval.valueList)) {
            if ((yyvsp[(2) - (3)].character)) {
                CSSParserValue v;
                v.id = 0;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[(2) - (3)].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        }
    }
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 1396 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1399 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1402 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1408 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1411 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1414 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1420 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1421 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1422 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 1423 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1429 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1430 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1431 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 1432 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 1433 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 1434 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 1436 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 1439 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 1442 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 1445 "Source/WebCore/css/CSSGrammar.y"
    { /* Handle width: %; */
      (yyval.value).id = 0; (yyval.value).unit = 0;
  }
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 1451 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 1452 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; }
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 1453 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 1454 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 1455 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 1456 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; }
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 1457 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 1458 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 1459 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 1460 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 1461 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 1462 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 1463 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 1464 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 1465 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 1466 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 1467 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 1468 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 1469 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 1470 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 1471 "Source/WebCore/css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).fValue = (yyvsp[(1) - (2)].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      CSSParser* p = static_cast<CSSParser*>(parser);
      if (Document* doc = p->findDocument())
          doc->setUsesRemUnits(true);
  }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 1479 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VW; }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 1480 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VH; }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 1481 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_VMIN; }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 1485 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 1494 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (4)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (4)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 1503 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (4)].string);
        CSSParserValueList* valueList = p->createFloatingValueList();
        f->args = p->sinkFloatingValueList(valueList);
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 1513 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (3)].string);
        f->args = nullptr;
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 1525 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 1526 "Source/WebCore/css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 1530 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = '+';
    }
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 1533 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = '-';
    }
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 1536 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = '*';
    }
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 1539 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    }
    break;

  case 276:

/* Line 1806 of yacc.c  */
#line 1545 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(3) - (6)].valueList)) {
            (yyval.valueList) = (yyvsp[(3) - (6)].valueList);
            CSSParserValue v;
            v.id = 0;
            v.unit = CSSParserValue::Operator;
            v.iValue = '(';
            (yyval.valueList)->insertValueAt(0, v);
            v.iValue = ')';
            (yyval.valueList)->addValue(v);
        } else
            (yyval.valueList) = 0;
    }
    break;

  case 277:

/* Line 1806 of yacc.c  */
#line 1560 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (2)].value)));
    }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 1565 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(1) - (3)].valueList) && (yyvsp[(2) - (3)].character)) {
            (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
            CSSParserValue v;
            v.id = 0;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[(2) - (3)].character);
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        } else
            (yyval.valueList) = 0;

    }
    break;

  case 279:

/* Line 1806 of yacc.c  */
#line 1579 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (3)].valueList) && (yyvsp[(2) - (3)].character) && (yyvsp[(3) - (3)].valueList)) {
            (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
            CSSParserValue v;
            v.id = 0;
            v.unit = CSSParserValue::Operator;
            v.iValue = (yyvsp[(2) - (3)].character);
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->extend(*((yyvsp[(3) - (3)].valueList)));
        } else 
            (yyval.valueList) = 0;
    }
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 1592 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 1598 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (1)].valueList);
    }
    break;

  case 283:

/* Line 1806 of yacc.c  */
#line 1601 "Source/WebCore/css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].valueList) && (yyvsp[(4) - (4)].valueList)) {
            (yyval.valueList) = (yyvsp[(1) - (4)].valueList);
            CSSParserValue v;
            v.id = 0;
            v.unit = CSSParserValue::Operator;
            v.iValue = ',';
            (yyval.valueList)->addValue(v);
            (yyval.valueList)->extend(*((yyvsp[(4) - (4)].valueList)));
        } else
            (yyval.valueList) = 0;
    }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 1616 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 1625 "Source/WebCore/css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 1632 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (1)].string);
    }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 1635 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (1)].string);
    }
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 1641 "Source/WebCore/css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 1650 "Source/WebCore/css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 1658 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 1661 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 1667 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 293:

/* Line 1806 of yacc.c  */
#line 1670 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 294:

/* Line 1806 of yacc.c  */
#line 1676 "Source/WebCore/css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 295:

/* Line 1806 of yacc.c  */
#line 1694 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    }
    break;

  case 296:

/* Line 1806 of yacc.c  */
#line 1697 "Source/WebCore/css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    }
    break;



/* Line 1806 of yacc.c  */
#line 4849 "./DerivedSources/WebCore/CSSGrammar.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1707 "Source/WebCore/css/CSSGrammar.y"



#ifndef CSSGrammar_h
#define CSSGrammar_h
/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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

/* Line 2068 of yacc.c  */
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



/* Line 2068 of yacc.c  */
#line 163 "./DerivedSources/WebCore/CSSGrammar.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




#endif

/*
 * Copyright (C) 2016, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

%{
#include <string.h>
#include <stdlib.h>

#include "aidl_language.h"
#include "parser.h"
#include "aidl_language_y.h"

#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%option noyywrap
%option nounput
%option noinput
%option reentrant
%option bison-bridge
%option bison-locations

%x LONG_COMMENT
%s CONST_MODE

identifier  [_a-zA-Z][_a-zA-Z0-9]*
whitespace  ([ \t\r]+)
intvalue    [0-9]+[lL]?
hexvalue    0[x|X][0-9a-fA-F]+[lL]?
floatvalue  [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?f?

 /* LONG_COMMENT keeps the original state (INITIAL or CONST_MODE) */
 int save_state;

%%
%{
  /* This happens at every call to yylex (every time we receive one token) */
  std::string extra_text;
  yylloc->step();
%}

\/\*                  { extra_text += yytext; save_state = YY_START; BEGIN(LONG_COMMENT); }
<LONG_COMMENT>\*+\/   { extra_text += yytext; yylloc->step(); BEGIN(save_state);  }
<LONG_COMMENT>\*+     { extra_text += yytext; }
<LONG_COMMENT>\n+     { extra_text += yytext; yylloc->lines(yyleng); }
<LONG_COMMENT>[^*\n]+ { extra_text += yytext; }

\"[^\"]*\"            { yylval->token = new AidlToken(yytext, extra_text);
                        return yy::parser::token::C_STR; }

\/\/.*                { extra_text += yytext; extra_text += "\n"; }

\n+                   { yylloc->lines(yyleng); yylloc->step(); }
{whitespace}          {}
<<EOF>>               { yyterminate(); }

    /* symbols */
"("                   { return('('); }
")"                   { return(')'); }
"<"                   { return('<'); }
">"                   { return('>'); }
"{"                   { return('{'); }
"}"                   { return('}'); }
"["                   { return('['); }
"]"                   { return(']'); }
":"                   { return(':'); }
";"                   { BEGIN(INITIAL); return(';'); }
","                   { return(','); }
"."                   { return('.'); }
"="                   { BEGIN(CONST_MODE); return('='); }
  /*
   Limit availability of operators for "const_expr" to avoid unintended conflicts
   in other parts of the grammar rules.
   e.g. ">>" bothers when parsing "type_args"("Foo<Bar<Baz>>").
  */
<CONST_MODE>{
  "+"                   { return('+'); }
  "-"                   { return('-'); }
  "*"                   { return('*'); }
  "/"                   { return('/'); }
  "%"                   { return('%'); }
  "&"                   { return('&'); }
  "|"                   { return('|'); }
  "^"                   { return('^'); }
  "<<"                  { return(yy::parser::token::LSHIFT); }
  ">>"                  { return(yy::parser::token::RSHIFT); }
  "&&"                  { return(yy::parser::token::LOGICAL_AND); }
  "||"                  { return(yy::parser::token::LOGICAL_OR);  }
  "!"                   { return('!'); }
  "~"                   { return('~'); }
  "<="                  { return(yy::parser::token::LEQ); }
  ">="                  { return(yy::parser::token::GEQ); }
  "=="                  { return(yy::parser::token::EQUALITY); }
  "!="                  { return(yy::parser::token::NEQ); }
}
    /* annotations */
@{identifier}         { yylval->token = new AidlToken(yytext + 1, extra_text);
                        return yy::parser::token::ANNOTATION;
                      }

    /* keywords */
parcelable            { yylval->token = new AidlToken("parcelable", extra_text);
                        return yy::parser::token::PARCELABLE;
                      }
import                { return yy::parser::token::IMPORT; }
package               { return yy::parser::token::PACKAGE; }
in                    { return yy::parser::token::IN; }
out                   { return yy::parser::token::OUT; }
inout                 { return yy::parser::token::INOUT; }
cpp_header            { yylval->token = new AidlToken("cpp_header", extra_text);
                        return yy::parser::token::CPP_HEADER; }
const                 { yylval->token = new AidlToken("const", extra_text);
                        return yy::parser::token::CONST; }
true                  { return yy::parser::token::TRUE_LITERAL; }
false                 { return yy::parser::token::FALSE_LITERAL; }

interface             { yylval->token = new AidlToken("interface", extra_text);
                        return yy::parser::token::INTERFACE;
                      }
oneway                { yylval->token = new AidlToken("oneway", extra_text);
                        return yy::parser::token::ONEWAY;
                      }
enum                  { yylval->token = new AidlToken("enum", extra_text);
                        return yy::parser::token::ENUM;
                      }
union                 { yylval->token = new AidlToken("union", extra_text);
                        return yy::parser::token::UNION;
                      }

    /* scalars */
{identifier}          { yylval->token = new AidlToken(yytext, extra_text);
                        return yy::parser::token::IDENTIFIER;
                      }
'.'                   { yylval->character = yytext[1];
                        return yy::parser::token::CHARVALUE;
                      }
{intvalue}            { yylval->token = new AidlToken(yytext, extra_text);
                        return yy::parser::token::INTVALUE; }
{floatvalue}          { yylval->token = new AidlToken(yytext, extra_text);
                        return yy::parser::token::FLOATVALUE; }
{hexvalue}            { yylval->token = new AidlToken(yytext, extra_text);
                        return yy::parser::token::HEXVALUE; }

  /* lexical error! */
.                     { return yy::parser::token::UNKNOWN; }

%%

// comment and whitespace handling
// ================================================

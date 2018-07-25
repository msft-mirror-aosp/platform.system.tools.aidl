%{
#include "aidl_language.h"
#include "aidl_language_y.h"
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(yy::parser::semantic_type *, yy::parser::location_type *, void *);

#define lex_scanner ps->Scanner()

%}

%parse-param { Parser* ps }
%lex-param { void *lex_scanner }

%pure-parser
%glr-parser
%skeleton "glr.cc"

%expect-rr 0

%error-verbose

%union {
    AidlToken* token;
    int integer;
    std::string *str;
    AidlAnnotation* annotation;
    std::set<std::unique_ptr<AidlAnnotation>>* annotation_list;
    AidlTypeSpecifier* type;
    AidlTypeSpecifier* unannotated_type;
    AidlArgument* arg;
    AidlArgument::Direction direction;
    AidlConstantValue* constant_value;
    std::vector<std::unique_ptr<AidlArgument>>* arg_list;
    AidlVariableDeclaration* variable;
    std::vector<std::unique_ptr<AidlVariableDeclaration>>* variable_list;
    AidlMethod* method;
    AidlMember* constant;
    std::vector<std::unique_ptr<AidlMember>>* interface_members;
    AidlQualifiedName* qname;
    AidlInterface* interface;
    AidlParcelable* parcelable;
    AidlDefinedType* declaration;
    AidlDocument* declaration_list;
    std::vector<std::unique_ptr<AidlTypeSpecifier>>* type_args;
}

%token<token> ANNOTATION "annotation"
%token<token> C_STR "string literal"
%token<token> HEXVALUE "hex literal"
%token<token> IDENTIFIER "identifier"
%token<token> INTERFACE "interface"
%token<token> ONEWAY "oneway"
%token<integer> INTVALUE "int literal"

%token '(' ')' ',' '=' '[' ']' '<' '>' '.' '{' '}' ';'
%token CONST "const"
%token UNKNOWN "unrecognized character"
%token CPP_HEADER "cpp_header"
%token IMPORT "import"
%token IN "in"
%token INOUT "inout"
%token OUT "out"
%token PACKAGE "package"
%token PARCELABLE "parcelable"

%type<declaration_list> decls
%type<declaration> decl
%type<variable_list> variable_decls
%type<variable> variable_decl
%type<interface_members> interface_members
%type<declaration> unannotated_decl
%type<interface> interface_decl
%type<parcelable> parcelable_decl
%type<method> method_decl
%type<constant> constant_decl
%type<annotation> annotation
%type<annotation_list>annotation_list
%type<type> type
%type<unannotated_type> unannotated_type
%type<arg_list> arg_list
%type<arg> arg
%type<direction> direction
%type<type_args> type_args
%type<qname> qualified_name
%type<constant_value> constant_value

%type<token> identifier error
%%
document
 : package imports decls
  { ps->SetDocument($3); }
 ;

/* A couple of tokens that are keywords elsewhere are identifiers when
 * occurring in the identifier position. Therefore identifier is a
 * non-terminal, which is either an IDENTIFIER token, or one of the
 * aforementioned keyword tokens.
 */
identifier
 : IDENTIFIER
  { $$ = $1; }
 | CPP_HEADER
  { $$ = new AidlToken("cpp_header", ""); }
 ;

package
 : {}
 | PACKAGE qualified_name ';'
  { ps->SetPackage($2); };

imports
 : {}
 | import imports {};

import
 : IMPORT qualified_name ';'
  { ps->AddImport($2, @1.begin.line); };

qualified_name
 : identifier {
    $$ = new AidlQualifiedName($1->GetText(), $1->GetComments());
    delete $1;
  }
 | qualified_name '.' identifier
  { $$ = $1;
    $$->AddTerm($3->GetText());
    delete $3;
  };

decls
 : /* empty */
  { $$ = new AidlDocument(); }
 | decls decl {
   $$ = $1;
   $$->AddDefinedType($2);
  }
 ;

decl
 : annotation_list unannotated_decl
   {
    $$ = $2;

    bool is_unstructured_parcelable =
      $$->AsParcelable() != nullptr && $$->AsStructuredParcelable() == nullptr;

    if (is_unstructured_parcelable && !$1->empty()) {
      std::cerr << ps->FileName() << ":" << @1 << ": unstructured parcelables cannot be annotated"
                << std::endl;
      ps->AddError();
    }

    $$->Annotate(std::move(*$1));
    delete $1;
   }
 ;

unannotated_decl
 : parcelable_decl
  { $$ = $1; }
 | interface_decl
  { $$ = $1; }
 ;

parcelable_decl
 : PARCELABLE qualified_name ';' {
    $$ = new AidlParcelable($2, @2.begin.line, ps->Package());
    ps->GetTypenames().AddDefinedType($$);
  }
 | PARCELABLE qualified_name CPP_HEADER C_STR ';' {
    $$ = new AidlParcelable($2, @2.begin.line, ps->Package(), $4->GetText());
    ps->GetTypenames().AddDefinedType($$);
  }
 | PARCELABLE identifier '{' variable_decls '}' {
    AidlQualifiedName* name = new AidlQualifiedName($2->GetText(), $2->GetComments());
    $$ = new AidlStructuredParcelable(name, @2.begin.line, ps->Package(), $4);
    ps->GetTypenames().AddDefinedType($$);
 }
 | PARCELABLE error ';' {
    ps->AddError();
    $$ = NULL;
  };

variable_decls
 : /* empty */ {
    $$ = new std::vector<std::unique_ptr<AidlVariableDeclaration>>; }
 | variable_decls variable_decl {
    $$ = $1;
    if ($2 != nullptr) {
      $$->push_back(std::unique_ptr<AidlVariableDeclaration>($2));
    }
 };

variable_decl
 : type identifier ';' {
   $$ = new AidlVariableDeclaration($1, $2->GetText(), @2.begin.line);
 }
 | type identifier '=' constant_value ';' {
   $$ = new AidlVariableDeclaration($1, $2->GetText(), @2.begin.line, $4);
 }
 | error ';' {
   ps->AddError();
   $$ = nullptr;
 }

interface_decl
 : INTERFACE identifier '{' interface_members '}' {
    $$ = new AidlInterface($2->GetText(), @1.begin.line, $1->GetComments(),
                           false, $4, ps->Package());
    ps->GetTypenames().AddDefinedType($$);
    delete $1;
    delete $2;
  }
 | ONEWAY INTERFACE identifier '{' interface_members '}' {
    $$ = new AidlInterface($3->GetText(), @2.begin.line, $1->GetComments(),
                           true, $5, ps->Package());
    ps->GetTypenames().AddDefinedType($$);
    delete $1;
    delete $2;
    delete $3;
  }
 | INTERFACE error '{' interface_members '}' {
    ps->AddError();
    $$ = nullptr;
    delete $1;
    delete $2;
    delete $4;
  };

interface_members
 :
  { $$ = new std::vector<std::unique_ptr<AidlMember>>(); }
 | interface_members method_decl
  { $1->push_back(std::unique_ptr<AidlMember>($2)); }
 | interface_members constant_decl
  { $1->push_back(std::unique_ptr<AidlMember>($2)); }
 | interface_members error ';' {
    ps->AddError();
    $$ = $1;
  };

constant_value
 : INTVALUE { $$ = AidlConstantValue::LiteralInt($1); }
 | HEXVALUE {
    $$ = AidlConstantValue::ParseHex($1->GetText(), @1.begin.line);
    delete $1;
  }
 | C_STR {
    $$ = AidlConstantValue::ParseString($1->GetText(), @1.begin.line);
    delete $1;
  }
 ;

constant_decl
 : CONST type identifier '=' constant_value ';' {
    $$ = new AidlConstantDeclaration($2, $3->GetText(), $5, @3.begin.line);
    delete $3;
   }
 ;

method_decl
 : type identifier '(' arg_list ')' ';' {
    $$ = new AidlMethod(false, $1, $2->GetText(), $4, @2.begin.line,
                        $1->GetComments());
    delete $2;
  }
 | ONEWAY type identifier '(' arg_list ')' ';' {
    $$ = new AidlMethod(true, $2, $3->GetText(), $5, @3.begin.line,
                        $1->GetComments());
    delete $1;
    delete $3;
  }
 | type identifier '(' arg_list ')' '=' INTVALUE ';' {
    $$ = new AidlMethod(false, $1, $2->GetText(), $4, @2.begin.line,
                        $1->GetComments(), $7);
    delete $2;
  }
 | ONEWAY type identifier '(' arg_list ')' '=' INTVALUE ';' {
    $$ = new AidlMethod(true, $2, $3->GetText(), $5, @3.begin.line,
                        $1->GetComments(), $8);
    delete $1;
    delete $3;
  };

arg_list
 :
  { $$ = new std::vector<std::unique_ptr<AidlArgument>>(); }
 | arg {
    $$ = new std::vector<std::unique_ptr<AidlArgument>>();
    $$->push_back(std::unique_ptr<AidlArgument>($1));
  }
 | arg_list ',' arg {
    $$ = $1;
    $$->push_back(std::unique_ptr<AidlArgument>($3));
  };

arg
 : direction type identifier {
    $$ = new AidlArgument($1, $2, $3->GetText(), @3.begin.line);
    delete $3;
  }
 | type identifier {
    $$ = new AidlArgument($1, $2->GetText(), @2.begin.line);
    delete $2;
  }
 | error {
    ps->AddError();
  };

unannotated_type
 : qualified_name {
    $$ = new AidlTypeSpecifier($1->GetDotName(), false, nullptr, @1.begin.line,
                               $1->GetComments());
    ps->DeferResolution($$);
    delete $1;
  }
 | qualified_name '[' ']' {
    $$ = new AidlTypeSpecifier($1->GetDotName(), true, nullptr, @1.begin.line,
                               $1->GetComments());
    ps->DeferResolution($$);
    delete $1;
  }
 | qualified_name '<' type_args '>' {
    $$ = new AidlTypeSpecifier($1->GetDotName(), false, $3, @1.begin.line,
                               $1->GetComments());
    ps->DeferResolution($$);
    delete $1;
  };

type
 : annotation_list unannotated_type {
    $$ = $2;
    $2->Annotate(std::move(*$1));
    delete $1;
  };

type_args
 : unannotated_type {
    $$ = new std::vector<std::unique_ptr<AidlTypeSpecifier>>();
    $$->emplace_back($1);
  }
 | type_args ',' unannotated_type {
    $1->emplace_back($3);
  };

annotation_list
 :
  { $$ = new std::set<unique_ptr<AidlAnnotation>>(); }
 | annotation_list annotation
  {
    if ($2 != nullptr) {
      $1->insert(std::unique_ptr<AidlAnnotation>($2));
    }
  };

annotation
 : ANNOTATION
  {
    std::string error;
    AidlAnnotation* annot = new AidlAnnotation($1->GetText(), error);
    if (error != "") {
      std::cerr << ps->FileName() << ":" << @1 << ": ";
      std::cerr << error << std::endl;
      ps->AddError();
      $$ = nullptr;
    } else {
      $$ = annot;
    }
  };

direction
 : IN
  { $$ = AidlArgument::IN_DIR; }
 | OUT
  { $$ = AidlArgument::OUT_DIR; }
 | INOUT
  { $$ = AidlArgument::INOUT_DIR; };

%%

#include <ctype.h>
#include <stdio.h>

void yy::parser::error(const yy::parser::location_type& l,
                       const std::string& errstr) {
  std::cerr << ps->FileName() << ":" << l << ": " << errstr << std::endl;
}

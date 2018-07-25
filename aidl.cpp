/*
 * Copyright (C) 2015, The Android Open Source Project
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

#include "aidl.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#endif

#include <android-base/strings.h>

#include "aidl_language.h"
#include "generate_cpp.h"
#include "generate_java.h"
#include "import_resolver.h"
#include "logging.h"
#include "options.h"
#include "os.h"
#include "type_cpp.h"
#include "type_java.h"
#include "type_namespace.h"

#ifndef O_BINARY
#  define O_BINARY  0
#endif

using android::base::Join;
using android::base::Split;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

namespace android {
namespace aidl {
namespace {

// The following are gotten as the offset from the allowable id's between
// android.os.IBinder.FIRST_CALL_TRANSACTION=1 and
// android.os.IBinder.LAST_CALL_TRANSACTION=16777215
const int kMinUserSetMethodId = 0;
const int kMaxUserSetMethodId = 16777214;

bool check_filename(const std::string& filename,
                    const std::string& package,
                    const std::string& name,
                    unsigned line) {
    const char* p;
    string expected;
    string fn;
    size_t len;
    bool valid = false;

    if (!IoDelegate::GetAbsolutePath(filename, &fn)) {
      return false;
    }

    if (!package.empty()) {
        expected = package;
        expected += '.';
    }

    len = expected.length();
    for (size_t i=0; i<len; i++) {
        if (expected[i] == '.') {
            expected[i] = OS_PATH_SEPARATOR;
        }
    }

    expected.append(name, 0, name.find('.'));

    expected += ".aidl";

    len = fn.length();
    valid = (len >= expected.length());

    if (valid) {
        p = fn.c_str() + (len - expected.length());

#ifdef _WIN32
        if (OS_PATH_SEPARATOR != '/') {
            // Input filename under cygwin most likely has / separators
            // whereas the expected string uses \\ separators. Adjust
            // them accordingly.
          for (char *c = const_cast<char *>(p); *c; ++c) {
                if (*c == '/') *c = OS_PATH_SEPARATOR;
            }
        }
#endif

        // aidl assumes case-insensitivity on Mac Os and Windows.
#if defined(__linux__)
        valid = (expected == p);
#else
        valid = !strcasecmp(expected.c_str(), p);
#endif
    }

    if (!valid) {
        fprintf(stderr, "%s:%d %s should be declared in a file called %s.\n",
                filename.c_str(), line, name.c_str(), expected.c_str());
    }

    return valid;
}

bool check_filenames(const std::string& filename, const AidlDocument& doc) {
  bool success = true;

  for (const auto& item : doc.GetDefinedTypes()) {
    success &= check_filename(filename, item->GetPackage(), item->GetName(),
                              item->GetLine());
  }

  return success;
}

bool gather_types(const std::string& filename, const AidlDocument& doc, TypeNamespace* types) {
  bool success = true;

  for (const auto& item : doc.GetDefinedTypes()) {
    const AidlInterface* interface = item->AsInterface();
    if (interface != nullptr) {
      success &= types->AddBinderType(*interface, filename);
      continue;
    }

    const AidlParcelable* parcelable = item->AsParcelable();
    if (parcelable != nullptr) {
      success &= types->AddParcelableType(*parcelable, filename);
      continue;
    }

    CHECK(false) << "aidl internal error: unrecognized type";
  }

  return success;
}

int check_types(const string& filename, const AidlStructuredParcelable* parcel,
                TypeNamespace* types) {
  int err = 0;
  for (const auto& v : parcel->GetFields()) {
    if (!v->CheckValid()) {
      err = 1;
    }

    if (!types->MaybeAddContainerType(v->GetType())) {
      err = 1;  // return type is invalid
    }

    const ValidatableType* type = types->GetReturnType(v->GetType(), filename, *parcel);
    if (!type) {
      err = 1;
    }

    v->GetMutableType()->SetLanguageType(type);
  }

  return err;
}

int check_types(const string& filename,
                const AidlInterface* c,
                TypeNamespace* types) {
  int err = 0;

  if (c->IsUtf8() && c->IsUtf8InCpp()) {
    cerr << filename << ":" << c->GetLine()
         << "Interface cannot be marked as both @utf8 and @utf8InCpp";
    err = 1;
  }

  // Has to be a pointer due to deleting copy constructor. No idea why.
  map<string, const AidlMethod*> method_names;
  for (const auto& m : c->GetMethods()) {
    bool oneway = m->IsOneway() || c->IsOneway();

    if (!types->MaybeAddContainerType(m->GetType())) {
      err = 1;  // return type is invalid
    }

    const ValidatableType* return_type =
        types->GetReturnType(m->GetType(), filename, *c);

    if (!return_type) {
      err = 1;
    }

    m->GetMutableType()->SetLanguageType(return_type);

    if (oneway && m->GetType().GetName() != "void") {
        cerr << filename << ":" << m->GetLine()
            << " oneway method '" << m->GetName() << "' cannot return a value"
            << endl;
        err = 1;
    }

    int index = 1;
    for (const auto& arg : m->GetArguments()) {
      if (!types->MaybeAddContainerType(arg->GetType())) {
        err = 1;
      }

      const ValidatableType* arg_type =
          types->GetArgType(*arg, index, filename, *c);

      if (!arg_type) {
        err = 1;
      }

      arg->GetMutableType()->SetLanguageType(arg_type);

      if (oneway && arg->IsOut()) {
        cerr << filename << ":" << m->GetLine()
            << " oneway method '" << m->GetName()
            << "' cannot have out parameters" << endl;
        err = 1;
      }
    }

    auto it = method_names.find(m->GetName());
    // prevent duplicate methods
    if (it == method_names.end()) {
      method_names[m->GetName()] = m.get();
    } else {
      cerr << filename << ":" << m->GetLine()
           << " attempt to redefine method " << m->GetName() << "," << endl
           << filename << ":" << it->second->GetLine()
           << "    previously defined here." << endl;
      err = 1;
    }
  }
  return err;
}

void write_common_dep_file(const string& output_file,
                           const vector<string>& aidl_sources,
                           CodeWriter* writer,
                           const bool ninja) {
  // Encode that the output file depends on aidl input files.
  writer->Write("%s : \\\n", output_file.c_str());
  writer->Write("  %s", Join(aidl_sources, " \\\n  ").c_str());
  writer->Write("\n");

  if (!ninja) {
    writer->Write("\n");
    // Output "<input_aidl_file>: " so make won't fail if the input .aidl file
    // has been deleted, moved or renamed in incremental build.
    for (const auto& src : aidl_sources) {
      writer->Write("%s :\n", src.c_str());
    }
  }
}

bool write_java_dep_file(const JavaOptions& options,
                         const vector<unique_ptr<AidlImport>>& imports,
                         const IoDelegate& io_delegate,
                         const string& output_file_name) {
  string dep_file_name = options.DependencyFilePath();
  if (dep_file_name.empty()) {
    return true;  // nothing to do
  }
  CodeWriterPtr writer = io_delegate.GetCodeWriter(dep_file_name);
  if (!writer) {
    LOG(ERROR) << "Could not open dependency file: " << dep_file_name;
    return false;
  }

  vector<string> source_aidl = {options.input_file_name_};
  for (const auto& import : imports) {
    if (!import->GetFilename().empty()) {
      source_aidl.push_back(import->GetFilename());
    }
  }

  write_common_dep_file(output_file_name, source_aidl, writer.get(),
                        options.DependencyFileNinja());

  return true;
}

bool write_cpp_dep_file(const CppOptions& options, const AidlDefinedType& defined_type,
                        const vector<unique_ptr<AidlImport>>& imports,
                        const IoDelegate& io_delegate) {
  using ::android::aidl::cpp::HeaderFile;
  using ::android::aidl::cpp::ClassNames;

  string dep_file_name = options.DependencyFilePath();
  if (dep_file_name.empty()) {
    return true;  // nothing to do
  }
  CodeWriterPtr writer = io_delegate.GetCodeWriter(dep_file_name);
  if (!writer) {
    LOG(ERROR) << "Could not open dependency file: " << dep_file_name;
    return false;
  }

  vector<string> source_aidl = {options.InputFileName()};
  for (const auto& import : imports) {
    if (!import->GetFilename().empty()) {
      source_aidl.push_back(import->GetFilename());
    }
  }

  write_common_dep_file(options.OutputCppFilePath(), source_aidl, writer.get(),
                        options.DependencyFileNinja());

  if (!options.DependencyFileNinja()) {
    vector<string> headers;
    for (ClassNames c : {ClassNames::CLIENT,
                         ClassNames::SERVER,
                         ClassNames::INTERFACE}) {
      headers.push_back(options.OutputHeaderDir() + '/' +
                        HeaderFile(defined_type, c, false /* use_os_sep */));
    }

    writer->Write("\n");

    // Generated headers also depend on the source aidl files.
    writer->Write("%s : \\\n    %s\n", Join(headers, " \\\n    ").c_str(),
                  Join(source_aidl, " \\\n    ").c_str());
  }

  return true;
}

string generate_outputFileName(const JavaOptions& options, const AidlDefinedType& defined_type) {
  const string& name = defined_type.GetName();
  string package = defined_type.GetPackage();
  string result;

  // create the path to the destination folder based on the
  // defined_type package name
  result = options.output_base_folder_;
  result += OS_PATH_SEPARATOR;

  string packageStr = package;
  size_t len = packageStr.length();
  for (size_t i = 0; i < len; i++) {
    if (packageStr[i] == '.') {
      packageStr[i] = OS_PATH_SEPARATOR;
    }
  }

  result += packageStr;

  // add the filename by replacing the .aidl extension to .java
  result += OS_PATH_SEPARATOR;
  result.append(name, 0, name.find('.'));
  result += ".java";

  return result;
}

int check_and_assign_method_ids(const char * filename,
                                const std::vector<std::unique_ptr<AidlMethod>>& items) {
    // Check whether there are any methods with manually assigned id's and any that are not.
    // Either all method id's must be manually assigned or all of them must not.
    // Also, check for duplicates of user set id's and that the id's are within the proper bounds.
    set<int> usedIds;
    bool hasUnassignedIds = false;
    bool hasAssignedIds = false;
    for (const auto& item : items) {
        if (item->HasId()) {
            hasAssignedIds = true;
            // Ensure that the user set id is not duplicated.
            if (usedIds.find(item->GetId()) != usedIds.end()) {
                // We found a duplicate id, so throw an error.
                fprintf(stderr,
                        "%s:%d Found duplicate method id (%d) for method: %s\n",
                        filename, item->GetLine(),
                        item->GetId(), item->GetName().c_str());
                return 1;
            }
            // Ensure that the user set id is within the appropriate limits
            if (item->GetId() < kMinUserSetMethodId ||
                    item->GetId() > kMaxUserSetMethodId) {
                fprintf(stderr, "%s:%d Found out of bounds id (%d) for method: %s\n",
                        filename, item->GetLine(),
                        item->GetId(), item->GetName().c_str());
                fprintf(stderr, "    Value for id must be between %d and %d inclusive.\n",
                        kMinUserSetMethodId, kMaxUserSetMethodId);
                return 1;
            }
            usedIds.insert(item->GetId());
        } else {
            hasUnassignedIds = true;
        }
        if (hasAssignedIds && hasUnassignedIds) {
            fprintf(stderr,
                    "%s: You must either assign id's to all methods or to none of them.\n",
                    filename);
            return 1;
        }
    }

    // In the case that all methods have unassigned id's, set a unique id for them.
    if (hasUnassignedIds) {
        int newId = 0;
        for (const auto& item : items) {
            item->SetId(newId++);
        }
    }

    // success
    return 0;
}

bool validate_constants(const AidlInterface& interface) {
  bool success = true;
  set<string> names;
  for (const std::unique_ptr<AidlConstantDeclaration>& constant :
       interface.GetConstantDeclarations()) {
    if (names.count(constant->GetName()) > 0) {
      LOG(ERROR) << "Found duplicate constant name '" << constant->GetName() << "'";
      success = false;
    }
    names.insert(constant->GetName());
    success = success && constant->CheckValid();
  }
  return success;
}

// TODO: Remove this in favor of using the YACC parser b/25479378
bool ParsePreprocessedLine(const string& line, string* decl,
                           vector<string>* package, string* class_name) {
  // erase all trailing whitespace and semicolons
  const size_t end = line.find_last_not_of(" ;\t");
  if (end == string::npos) {
    return false;
  }
  if (line.rfind(';', end) != string::npos) {
    return false;
  }

  decl->clear();
  string type;
  vector<string> pieces = Split(line.substr(0, end + 1), " \t");
  for (const string& piece : pieces) {
    if (piece.empty()) {
      continue;
    }
    if (decl->empty()) {
      *decl = std::move(piece);
    } else if (type.empty()) {
      type = std::move(piece);
    } else {
      return false;
    }
  }

  // Note that this logic is absolutely wrong.  Given a parcelable
  // org.some.Foo.Bar, the class name is Foo.Bar, but this code will claim that
  // the class is just Bar.  However, this was the way it was done in the past.
  //
  // See b/17415692
  size_t dot_pos = type.rfind('.');
  if (dot_pos != string::npos) {
    *class_name = type.substr(dot_pos + 1);
    *package = Split(type.substr(0, dot_pos), ".");
  } else {
    *class_name = type;
    package->clear();
  }

  return true;
}

}  // namespace

namespace internals {

bool parse_preprocessed_file(const IoDelegate& io_delegate, const string& filename,
                             TypeNamespace* types, AidlTypenames& typenames) {
  bool success = true;
  unique_ptr<LineReader> line_reader = io_delegate.GetLineReader(filename);
  if (!line_reader) {
    LOG(ERROR) << "cannot open preprocessed file: " << filename;
    success = false;
    return success;
  }

  string line;
  unsigned lineno = 1;
  for ( ; line_reader->ReadLine(&line); ++lineno) {
    if (line.empty() || line.compare(0, 2, "//") == 0) {
      // skip comments and empty lines
      continue;
    }

    string decl;
    vector<string> package;
    string class_name;
    if (!ParsePreprocessedLine(line, &decl, &package, &class_name)) {
      success = false;
      break;
    }

    if (decl == "parcelable") {
      AidlParcelable* doc =
          new AidlParcelable(new AidlQualifiedName(class_name, ""), lineno, package);
      types->AddParcelableType(*doc, filename);
      typenames.AddPreprocessedType(unique_ptr<AidlParcelable>(doc));
    } else if (decl == "structured_parcelable") {
      auto temp = new std::vector<std::unique_ptr<AidlVariableDeclaration>>();
      AidlStructuredParcelable* doc = new AidlStructuredParcelable(
          new AidlQualifiedName(class_name, ""), lineno, package, temp);
      types->AddParcelableType(*doc, filename);
      typenames.AddPreprocessedType(unique_ptr<AidlStructuredParcelable>(doc));
    } else if (decl == "interface") {
      auto temp = new std::vector<std::unique_ptr<AidlMember>>();
      AidlInterface* doc = new AidlInterface(class_name, lineno, "", false, temp, package);
      types->AddBinderType(*doc, filename);
      typenames.AddPreprocessedType(unique_ptr<AidlInterface>(doc));
    } else {
      success = false;
      break;
    }
  }
  if (!success) {
    LOG(ERROR) << filename << ':' << lineno
               << " malformed preprocessed file line: '" << line << "'";
  }

  return success;
}

AidlError load_and_validate_aidl(const std::vector<std::string>& preprocessed_files,
                                 const ImportResolver& import_resolver,
                                 const std::string& input_file_name, const bool generate_traces,
                                 const IoDelegate& io_delegate, TypeNamespace* types,
                                 std::unique_ptr<AidlDefinedType>* returned_type,
                                 std::vector<std::unique_ptr<AidlImport>>* returned_imports) {
  AidlError err = AidlError::OK;

  AidlTypenames typenames;

  // import the preprocessed file
  for (const string& s : preprocessed_files) {
    if (!parse_preprocessed_file(io_delegate, s, types, typenames)) {
      err = AidlError::BAD_PRE_PROCESSED_FILE;
    }
  }
  if (err != AidlError::OK) {
    return err;
  }

  // parse the input file
  Parser p{io_delegate, &typenames};
  if (!p.ParseFile(input_file_name)) {
    return AidlError::PARSE_ERROR;
  }

  AidlDocument* parsed_doc = p.GetDocument();

  if (parsed_doc->GetDefinedTypes().empty()) {
    LOG(ERROR) << "Cannot generate file without any definitions.";
    return AidlError::BAD_TYPE;
  }

  bool has_only_unstructured_parcelables = true;
  for (const auto& defined_type : parsed_doc->GetDefinedTypes()) {
    if (defined_type->AsStructuredParcelable() != nullptr ||
        defined_type->AsInterface() != nullptr) {
      has_only_unstructured_parcelables = false;
      break;
    }
  }
  if (has_only_unstructured_parcelables) {
    LOG(ERROR) << "Refusing to generate code with unstructured parcelables.";
    return AidlError::FOUND_PARCELABLE;
  }

  if (parsed_doc->GetDefinedTypes().size() > 1) {
    LOG(ERROR) << "Exactly one structured type is required to be defined.";
    return AidlError::BAD_TYPE;
  }

  unique_ptr<AidlDefinedType> defined_type(parsed_doc->ReleaseDefinedType());
  AidlInterface* interface = defined_type->AsInterface();
  AidlStructuredParcelable* parcelable = defined_type->AsStructuredParcelable();

  CHECK(interface != nullptr || parcelable != nullptr);

  if (!check_filename(input_file_name.c_str(), defined_type->GetPackage(), defined_type->GetName(),
                      defined_type->GetLine()) ||
      !types->IsValidPackage(defined_type->GetPackage())) {
    LOG(ERROR) << "Invalid package declaration '" << defined_type->GetPackage() << "'";
    return AidlError::BAD_PACKAGE;
  }

  // parse the imports of the input file
  for (auto& import : p.GetImports()) {
    if (types->HasImportType(*import)) {
      // There are places in the Android tree where an import doesn't resolve,
      // but we'll pick the type up through the preprocessed types.
      // This seems like an error, but legacy support demands we support it...
      continue;
    }
    string import_path = import_resolver.FindImportFile(import->GetNeededClass());
    if (import_path.empty()) {
      cerr << import->GetFileFrom() << ":" << import->GetLine()
           << ": couldn't find import for class "
           << import->GetNeededClass() << endl;
      err = AidlError::BAD_IMPORT;
      continue;
    }
    import->SetFilename(import_path);

    Parser p{io_delegate, &typenames};
    if (!p.ParseFile(import->GetFilename())) {
      cerr << "error while parsing import for class "
           << import->GetNeededClass() << endl;
      err = AidlError::BAD_IMPORT;
      continue;
    }

    std::unique_ptr<AidlDocument> document(p.ReleaseDocument());
    if (!check_filenames(import->GetFilename(), *document)) err = AidlError::BAD_IMPORT;
    import->SetAidlDocument(std::move(document));
  }
  if (err != AidlError::OK) {
    return err;
  }

  if (!p.Resolve()) {
    return AidlError::BAD_TYPE;
  }

  if (interface) {
    // gather the types that have been declared
    if (!types->AddBinderType(*interface, input_file_name)) {
      err = AidlError::BAD_TYPE;
    }
    interface->SetGenerateTraces(generate_traces);
  }

  if (parcelable) {
    if (!types->AddParcelableType(*parcelable, input_file_name)) {
      err = AidlError::BAD_TYPE;
    }
  }

  defined_type->SetLanguageType(types->GetDefinedType(*defined_type));

  for (const auto& import : p.GetImports()) {
    // If we skipped an unresolved import above (see comment there) we'll have
    // an empty bucket here.
    const AidlDocument* doc = import->GetAidlDocument();
    if (doc == nullptr) {
      continue;
    }

    if (!gather_types(import->GetFilename(), *doc, types)) {
      err = AidlError::BAD_TYPE;
    }
  }

  // check the referenced types in parsed_doc to make sure we've imported them
  if (interface && check_types(input_file_name, interface, types) != 0) {
    err = AidlError::BAD_TYPE;
  }
  if (parcelable && check_types(input_file_name, parcelable, types) != 0) {
    err = AidlError::BAD_TYPE;
  }
  if (err != AidlError::OK) {
    return err;
  }

  // assign method ids and validate.
  if (interface &&
      check_and_assign_method_ids(input_file_name.c_str(), interface->GetMethods()) != 0) {
    return AidlError::BAD_METHOD_ID;
  }
  if (interface && !validate_constants(*interface)) {
    return AidlError::BAD_CONSTANTS;
  }

  *returned_type = std::move(defined_type);

  if (returned_imports)
    p.ReleaseImports(returned_imports);

  return AidlError::OK;
}

} // namespace internals

int compile_aidl_to_cpp(const CppOptions& options,
                        const IoDelegate& io_delegate) {
  unique_ptr<AidlDefinedType> defined_type;
  std::vector<std::unique_ptr<AidlImport>> imports;
  unique_ptr<cpp::TypeNamespace> types(new cpp::TypeNamespace());
  types->Init();
  ImportResolver import_resolver{io_delegate, options.ImportPaths(), {}};
  AidlError err = internals::load_and_validate_aidl(
      options.preprocessed_files_, import_resolver, options.InputFileName(),
      options.ShouldGenTraces(), io_delegate, types.get(), &defined_type, &imports);
  if (err != AidlError::OK) {
    return 1;
  }

  CHECK(defined_type != nullptr);

  if (!write_cpp_dep_file(options, *defined_type, imports, io_delegate)) {
    return 1;
  }

  return (cpp::GenerateCpp(options, *types, *defined_type, io_delegate)) ? 0 : 1;
}

int compile_aidl_to_java(const JavaOptions& options,
                         const IoDelegate& io_delegate) {
  unique_ptr<AidlDefinedType> defined_type;
  std::vector<std::unique_ptr<AidlImport>> imports;
  unique_ptr<java::JavaTypeNamespace> types(new java::JavaTypeNamespace());
  types->Init();
  ImportResolver import_resolver{io_delegate, options.import_paths_, {}};
  AidlError aidl_err = internals::load_and_validate_aidl(
      options.preprocessed_files_, import_resolver, options.input_file_name_, options.gen_traces_,
      io_delegate, types.get(), &defined_type, &imports);
  if (aidl_err == AidlError::FOUND_PARCELABLE && !options.fail_on_parcelable_) {
    // We aborted code generation because this file contains parcelables.
    // However, we were not told to complain if we find parcelables.
    // Just generate a dep file and exit quietly.  The dep file is for a legacy
    // use case by the SDK.
    write_java_dep_file(options, imports, io_delegate, "");
    return 0;
  }
  if (aidl_err != AidlError::OK) {
    return 1;
  }

  CHECK(defined_type != nullptr);

  string output_file_name = options.output_file_name_;
  // if needed, generate the output file name from the base folder
  if (output_file_name.empty() && !options.output_base_folder_.empty()) {
    output_file_name = generate_outputFileName(options, *defined_type);
  }

  // make sure the folders of the output file all exists
  if (!io_delegate.CreatePathForFile(output_file_name)) {
    return 1;
  }

  if (!write_java_dep_file(options, imports, io_delegate, output_file_name)) {
    return 1;
  }

  return generate_java(output_file_name, options.input_file_name_.c_str(), defined_type.get(),
                       types.get(), io_delegate, options);
}

bool preprocess_aidl(const JavaOptions& options,
                     const IoDelegate& io_delegate) {
  unique_ptr<CodeWriter> writer =
      io_delegate.GetCodeWriter(options.output_file_name_);

  for (const auto& file : options.input_file_names_) {
    AidlTypenames typenames;
    Parser p{io_delegate, &typenames};
    if (!p.ParseFile(file))
      return false;
    AidlDocument* doc = p.GetDocument();
    string line;

    for (const auto& defined_type : doc->GetDefinedTypes()) {
      if (!writer->Write("%s %s;\n", defined_type->GetPreprocessDeclarationName().c_str(),
                         defined_type->GetCanonicalName().c_str())) {
        return false;
      }
    }
  }

  return writer->Close();
}

bool dump_api(const JavaOptions& options, const IoDelegate& io_delegate) {
  ImportResolver import_resolver{io_delegate, options.import_paths_, options.input_file_names_};

  map<string, vector<unique_ptr<AidlDefinedType>>> types_by_package;
  for (const auto& file : options.input_file_names_) {
    unique_ptr<java::JavaTypeNamespace> types(new java::JavaTypeNamespace());
    types->Init();
    unique_ptr<AidlDefinedType> t;
    if (internals::load_and_validate_aidl(options.preprocessed_files_, import_resolver, file,
                                          options.gen_traces_, io_delegate, types.get(), &t,
                                          nullptr) == AidlError::OK) {
      // group them by package name
      string package = t->GetPackage();
      types_by_package[package].emplace_back(std::move(t));
    } else {
      return false;
    }
  }

  // sort types within a package by their name. packages are already sorted.
  for (auto it = types_by_package.begin(); it != types_by_package.end(); it++) {
    auto& list = it->second;
    std::sort(list.begin(), list.end(), [](const auto& lhs, const auto& rhs) {
      return lhs->GetName().compare(rhs->GetName());
    });
  }

  // print
  unique_ptr<CodeWriter> writer = io_delegate.GetCodeWriter(options.output_file_name_);
  for (auto it = types_by_package.begin(); it != types_by_package.end(); it++) {
    writer->Write("package %s {\n", it->first.c_str());
    writer->Indent();
    for (const auto& type : it->second) {
      type->Write(writer.get());
      writer->Write("\n");
    }
    writer->Dedent();
    writer->Write("}\n");
  }

  return writer->Close();
}

}  // namespace android
}  // namespace aidl

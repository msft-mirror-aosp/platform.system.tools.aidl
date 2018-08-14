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

#include "generate_java.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <sstream>

#include <android-base/stringprintf.h>

#include "aidl_to_java.h"
#include "code_writer.h"
#include "type_java.h"

using std::unique_ptr;
using ::android::aidl::java::Variable;
using std::string;
using android::base::StringPrintf;

namespace android {
namespace aidl {
namespace java {

bool generate_java_interface(const string& filename, const string& original_src,
                             const AidlInterface* iface, JavaTypeNamespace* types,
                             const IoDelegate& io_delegate, const Options& options) {
  Class* cl = generate_binder_interface_class(iface, types, options);

  Document* document =
      new Document("" /* no comment */, iface->GetPackage(), original_src, unique_ptr<Class>(cl));

  CodeWriterPtr code_writer = io_delegate.GetCodeWriter(filename);
  document->Write(code_writer.get());

  return true;
}

bool generate_java_parcel(const std::string& filename, const std::string& original_src,
                          const AidlStructuredParcelable* parcel, JavaTypeNamespace* types,
                          const IoDelegate& io_delegate, const Options& options) {
  Class* cl = generate_parcel_class(parcel, types, options);

  Document* document =
      new Document("" /* no comment */, parcel->GetPackage(), original_src, unique_ptr<Class>(cl));

  CodeWriterPtr code_writer = io_delegate.GetCodeWriter(filename);
  document->Write(code_writer.get());

  return true;
}

bool generate_java(const std::string& filename, const std::string& original_src,
                   const AidlDefinedType* defined_type, JavaTypeNamespace* types,
                   const IoDelegate& io_delegate, const Options& options) {
  const AidlStructuredParcelable* parcelable = defined_type->AsStructuredParcelable();
  if (parcelable != nullptr) {
    return generate_java_parcel(filename, original_src, parcelable, types, io_delegate, options);
  }

  const AidlInterface* interface = defined_type->AsInterface();
  if (interface != nullptr) {
    return generate_java_interface(filename, original_src, interface, types, io_delegate, options);
  }

  CHECK(false) << "Unrecognized type sent for cpp generation.";
  return false;
}

android::aidl::java::Class* generate_parcel_class(const AidlStructuredParcelable* parcel,
                                                  java::JavaTypeNamespace* types,
                                                  const Options& /*options*/) {
  const ParcelType* parcelType = parcel->GetLanguageType<ParcelType>();

  Class* parcel_class = new Class;
  parcel_class->comment = parcel->GetComments();
  parcel_class->modifiers = PUBLIC;
  parcel_class->what = Class::CLASS;
  parcel_class->type = parcelType;

  for (const auto& variable : parcel->GetFields()) {
    const Type* type = variable->GetType().GetLanguageType<Type>();

    std::ostringstream out;
    out << "public " << type->JavaType() << (variable->GetType().IsArray() ? "[]" : "") << " "
        << variable->GetName();
    if (variable->GetDefaultValue()) {
      out << " = " << variable->ValueString();
    }
    out << ";\n";
    parcel_class->elements.push_back(new LiteralClassElement(out.str()));
  }

  std::ostringstream out;
  out << "public static final android.os.Parcelable.Creator<" << parcel->GetName() << "> CREATOR = "
      << "new android.os.Parcelable.Creator<" << parcel->GetName() << ">() {\n";
  out << "  public " << parcel->GetName()
      << " createFromParcel(android.os.Parcel _aidl_source) {\n";
  out << "    " << parcel->GetName() << " _aidl_out = new " << parcel->GetName() << "();\n";
  out << "    _aidl_out.readFromParcel(_aidl_source);\n";
  out << "    return _aidl_out;\n";
  out << "  }\n";
  out << "  public " << parcel->GetName() << "[] newArray(int _aidl_size) {\n";
  out << "    return new " << parcel->GetName() << "[_aidl_size];\n";
  out << "  }\n";
  out << "};\n";
  parcel_class->elements.push_back(new LiteralClassElement(out.str()));

  Variable* flag_variable = new Variable(new Type(types, "int", 0, false), "_aidl_flag");
  Variable* parcel_variable =
      new Variable(new Type(types, "android.os.Parcel", 0, false), "_aidl_parcel");

  Method* write_method = new Method;
  write_method->modifiers = PUBLIC;
  write_method->returnType = new Type(types, "void", 0, false);
  write_method->name = "writeToParcel";
  write_method->parameters.push_back(parcel_variable);
  write_method->parameters.push_back(flag_variable);
  write_method->statements = new StatementBlock();
  for (const auto& field : parcel->GetFields()) {
    string code;
    CodeWriterPtr writer = CodeWriter::ForString(&code);
    CodeGeneratorContext context{
        .writer = *(writer.get()),
        .typenames = types->typenames_,
        .type = field->GetType(),
        .var = field->GetName(),
        .parcel = parcel_variable->name,
        .is_return_value = false,
    };
    WriteToParcelFor(context);
    writer->Close();
    write_method->statements->Add(new LiteralStatement(code));
  }
  parcel_class->elements.push_back(write_method);

  Method* read_method = new Method;
  read_method->modifiers = PUBLIC;
  read_method->returnType = new Type(types, "void", 0, false);
  read_method->name = "readFromParcel";
  read_method->parameters.push_back(parcel_variable);
  read_method->statements = new StatementBlock();

  // keep this across different fields in order to create the classloader
  // at most once.
  bool is_classloader_created = false;
  for (const auto& field : parcel->GetFields()) {
    string code;
    CodeWriterPtr writer = CodeWriter::ForString(&code);
    CodeGeneratorContext context{
        .writer = *(writer.get()),
        .typenames = types->typenames_,
        .type = field->GetType(),
        .var = field->GetName(),
        .parcel = parcel_variable->name,
        .is_classloader_created = &is_classloader_created,
    };
    CreateFromParcelFor(context);
    writer->Close();
    read_method->statements->Add(new LiteralStatement(code));
  }
  parcel_class->elements.push_back(read_method);

  return parcel_class;
}

}  // namespace java
}  // namespace android
}  // namespace aidl

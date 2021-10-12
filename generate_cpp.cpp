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

#include "generate_cpp.h"
#include "aidl.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <random>
#include <set>
#include <string>

#include <android-base/format.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include "aidl_language.h"
#include "aidl_to_cpp.h"
#include "ast_cpp.h"
#include "code_writer.h"
#include "logging.h"
#include "os.h"

using android::base::Join;
using android::base::StringPrintf;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

namespace android {
namespace aidl {
namespace cpp {
namespace internals {
namespace {

const char kAndroidStatusVarName[] = "_aidl_ret_status";
const char kCodeVarName[] = "_aidl_code";
const char kFlagsVarName[] = "_aidl_flags";
const char kDataVarName[] = "_aidl_data";
const char kErrorLabel[] = "_aidl_error";
const char kImplVarName[] = "_aidl_impl";
const char kParcelVarName[] = "_aidl_parcel";
const char kReplyVarName[] = "_aidl_reply";
const char kReturnVarName[] = "_aidl_return";
const char kStatusVarName[] = "_aidl_status";
const char kTraceVarName[] = "_aidl_trace";
const char kAndroidParcelLiteral[] = "::android::Parcel";
const char kAndroidStatusLiteral[] = "::android::status_t";
const char kAndroidStatusOk[] = "::android::OK";
const char kAndroidStatusBadValue[] = "::android::BAD_VALUE";
const char kBinderStatusLiteral[] = "::android::binder::Status";
const char kIBinderHeader[] = "binder/IBinder.h";
const char kIInterfaceHeader[] = "binder/IInterface.h";
const char kParcelHeader[] = "binder/Parcel.h";
const char kStabilityHeader[] = "binder/Stability.h";
const char kStatusHeader[] = "binder/Status.h";
const char kString16Header[] = "utils/String16.h";
const char kTraceHeader[] = "utils/Trace.h";
const char kStrongPointerHeader[] = "utils/StrongPointer.h";
const char kAndroidBaseMacrosHeader[] = "android-base/macros.h";

unique_ptr<AstNode> BreakOnStatusNotOk() {
  IfStatement* ret = new IfStatement(new Comparison(
      new LiteralExpression(kAndroidStatusVarName), "!=",
      new LiteralExpression(kAndroidStatusOk)));
  ret->OnTrue()->AddLiteral("break");
  return unique_ptr<AstNode>(ret);
}

unique_ptr<AstNode> GotoErrorOnBadStatus() {
  IfStatement* ret = new IfStatement(new Comparison(
      new LiteralExpression(kAndroidStatusVarName), "!=",
      new LiteralExpression(kAndroidStatusOk)));
  ret->OnTrue()->AddLiteral(StringPrintf("goto %s", kErrorLabel));
  return unique_ptr<AstNode>(ret);
}

ArgList BuildArgList(const AidlTypenames& typenames, const AidlMethod& method, bool for_declaration,
                     bool type_name_only = false) {
  // Build up the argument list for the server method call.
  vector<string> method_arguments;
  for (const unique_ptr<AidlArgument>& a : method.GetArguments()) {
    string literal;
    // b/144943748: CppNameOf FileDescriptor is unique_fd. Don't pass it by
    // const reference but by value to make it easier for the user to keep
    // it beyond the scope of the call. unique_fd is a thin wrapper for an
    // int (fd) so passing by value is not expensive.
    const bool nonCopyable = IsNonCopyableType(a->GetType(), typenames);
    if (for_declaration) {
      // Method declarations need typenames, pointers to out params, and variable
      // names that match the .aidl specification.
      literal = CppNameOf(a->GetType(), typenames);

      if (a->IsOut()) {
        literal = literal + "*";
      } else {
        const auto definedType = typenames.TryGetDefinedType(a->GetType().GetName());

        const bool isEnum = definedType && definedType->AsEnumDeclaration() != nullptr;
        const bool isPrimitive = AidlTypenames::IsPrimitiveTypename(a->GetType().GetName());

        // We pass in parameters that are not primitives by const reference.
        // Arrays of primitives are not primitives.
        if (!(isPrimitive || isEnum || nonCopyable) || a->GetType().IsArray()) {
          literal = "const " + literal + "&";
        }
      }
      if (!type_name_only) {
        literal += " " + a->GetName();
      }
    } else {
      std::string varName = BuildVarName(*a);
      if (a->IsOut()) {
        literal = "&" + varName;
      } else if (nonCopyable) {
        literal = "std::move(" + varName + ")";
      } else {
        literal = varName;
      }
    }
    method_arguments.push_back(literal);
  }

  if (method.GetType().GetName() != "void") {
    string literal;
    if (for_declaration) {
      literal = CppNameOf(method.GetType(), typenames) + "*";
      if (!type_name_only) {
        literal += " " + string(kReturnVarName);
      }
    } else {
      literal = string{"&"} + kReturnVarName;
    }
    method_arguments.push_back(literal);
  }

  return ArgList(method_arguments);
}

unique_ptr<Declaration> BuildMethodDecl(const AidlMethod& method, const AidlTypenames& typenames,
                                        bool for_interface) {
  uint32_t modifiers = 0;
  if (for_interface) {
    modifiers |= MethodDecl::IS_VIRTUAL;
    modifiers |= MethodDecl::IS_PURE_VIRTUAL;
  } else {
    modifiers |= MethodDecl::IS_OVERRIDE;
  }

  const string attribute = GetDeprecatedAttribute(method);
  return unique_ptr<Declaration>{new MethodDecl{
      kBinderStatusLiteral, method.GetName(),
      BuildArgList(typenames, method, true /* for method decl */), modifiers, attribute}};
}

unique_ptr<Declaration> BuildMetaMethodDecl(const AidlMethod& method, const AidlTypenames&,
                                            const Options& options, bool for_interface) {
  AIDL_FATAL_IF(method.IsUserDefined(), method);
  if (method.GetName() == kGetInterfaceVersion && options.Version()) {
    std::ostringstream code;
    if (for_interface) {
      code << "virtual ";
    }
    code << "int32_t " << kGetInterfaceVersion << "()";
    if (for_interface) {
      code << " = 0;\n";
    } else {
      code << " override;\n";
    }
    return unique_ptr<Declaration>(new LiteralDecl(code.str()));
  }
  if (method.GetName() == kGetInterfaceHash && !options.Hash().empty()) {
    std::ostringstream code;
    if (for_interface) {
      code << "virtual ";
    }
    code << "std::string " << kGetInterfaceHash << "()";
    if (for_interface) {
      code << " = 0;\n";
    } else {
      code << " override;\n";
    }
    return unique_ptr<Declaration>(new LiteralDecl(code.str()));
  }
  return nullptr;
}

std::vector<unique_ptr<Declaration>> NestInNamespaces(vector<unique_ptr<Declaration>> decls,
                                                      const vector<string>& package) {
  auto it = package.crbegin();  // Iterate over the namespaces inner to outer
  for (; it != package.crend(); ++it) {
    vector<unique_ptr<Declaration>> inner;
    inner.emplace_back(unique_ptr<Declaration>{new CppNamespace{*it, std::move(decls)}});

    decls = std::move(inner);
  }
  return decls;
}

unique_ptr<Declaration> DefineClientTransaction(const AidlTypenames& typenames,
                                                const AidlInterface& interface,
                                                const AidlMethod& method, const Options& options) {
  const string i_name = ClassName(interface, ClassNames::INTERFACE);
  const string bp_name = ClassName(interface, ClassNames::CLIENT);
  unique_ptr<MethodImpl> ret{
      new MethodImpl{kBinderStatusLiteral,
                     bp_name,
                     method.GetName(),
                     {},
                     ArgList{BuildArgList(typenames, method, true /* for method decl */)}}};
  StatementBlock* b = ret->GetStatementBlock();

  // Declare parcels to hold our query and the response.
  b->AddLiteral(StringPrintf("%s %s", kAndroidParcelLiteral, kDataVarName));
  if (interface.IsSensitiveData()) {
    b->AddLiteral(StringPrintf("%s.markSensitive()", kDataVarName));
  }
  b->AddLiteral(StringPrintf("%s.markForBinder(remoteStrong())", kDataVarName));

  // Even if we're oneway, the transact method still takes a parcel.
  b->AddLiteral(StringPrintf("%s %s", kAndroidParcelLiteral, kReplyVarName));

  // Declare the status_t variable we need for error handling.
  b->AddLiteral(StringPrintf("%s %s = %s", kAndroidStatusLiteral,
                             kAndroidStatusVarName,
                             kAndroidStatusOk));
  // We unconditionally return a Status object.
  b->AddLiteral(StringPrintf("%s %s", kBinderStatusLiteral, kStatusVarName));

  if (options.GenTraces()) {
    b->AddLiteral(
        StringPrintf("::android::ScopedTrace %s(ATRACE_TAG_AIDL, \"AIDL::cpp::%s::%s::cppClient\")",
                     kTraceVarName, interface.GetName().c_str(), method.GetName().c_str()));
  }

  if (options.GenLog()) {
    b->AddLiteral(GenLogBeforeExecute(bp_name, method, false /* isServer */, false /* isNdk */),
                  false /* no semicolon */);
  }

  // Add the name of the interface we're hoping to call.
  b->AddStatement(new Assignment(
      kAndroidStatusVarName,
      new MethodCall(StringPrintf("%s.writeInterfaceToken",
                                  kDataVarName),
                     "getInterfaceDescriptor()")));
  b->AddStatement(GotoErrorOnBadStatus());

  for (const auto& a: method.GetArguments()) {
    const string var_name = ((a->IsOut()) ? "*" : "") + a->GetName();

    if (a->IsIn()) {
      // Serialization looks roughly like:
      //     _aidl_ret_status = _aidl_data.WriteInt32(in_param_name);
      //     if (_aidl_ret_status != ::android::OK) { goto error; }
      const string& method = ParcelWriteMethodOf(a->GetType(), typenames);
      b->AddStatement(
          new Assignment(kAndroidStatusVarName,
                         new MethodCall(StringPrintf("%s.%s", kDataVarName, method.c_str()),
                                        ParcelWriteCastOf(a->GetType(), typenames, var_name))));
      b->AddStatement(GotoErrorOnBadStatus());
    } else if (a->IsOut() && a->GetType().IsArray()) {
      // Special case, the length of the out array is written into the parcel.
      //     _aidl_ret_status = _aidl_data.writeVectorSize(&out_param_name);
      //     if (_aidl_ret_status != ::android::OK) { goto error; }
      b->AddStatement(new Assignment(
          kAndroidStatusVarName,
          new MethodCall(StringPrintf("%s.writeVectorSize", kDataVarName), var_name)));
      b->AddStatement(GotoErrorOnBadStatus());
    }
  }

  // Invoke the transaction on the remote binder and confirm status.
  string transaction_code = GetTransactionIdFor(interface, method);

  vector<string> args = {transaction_code, kDataVarName,
                         StringPrintf("&%s", kReplyVarName)};

  std::vector<std::string> flags;
  if (method.IsOneway()) flags.push_back("::android::IBinder::FLAG_ONEWAY");
  if (interface.IsSensitiveData()) flags.push_back("::android::IBinder::FLAG_CLEAR_BUF");

  args.push_back(flags.empty() ? "0" : Join(flags, " | "));

  b->AddStatement(new Assignment(
      kAndroidStatusVarName,
      new MethodCall("remote()->transact",
                     ArgList(args))));

  // If the method is not implemented in the remote side, try to call the
  // default implementation, if provided.
  vector<string> arg_names;
  for (const auto& a : method.GetArguments()) {
    if (IsNonCopyableType(a->GetType(), typenames)) {
      arg_names.emplace_back(StringPrintf("std::move(%s)", a->GetName().c_str()));
    } else {
      arg_names.emplace_back(a->GetName());
    }
  }
  if (method.GetType().GetName() != "void") {
    arg_names.emplace_back(kReturnVarName);
  }
  b->AddLiteral(StringPrintf("if (UNLIKELY(_aidl_ret_status == ::android::UNKNOWN_TRANSACTION && "
                             "%s::getDefaultImpl())) {\n"
                             "   return %s::getDefaultImpl()->%s(%s);\n"
                             "}\n",
                             i_name.c_str(), i_name.c_str(), method.GetName().c_str(),
                             Join(arg_names, ", ").c_str()),
                false /* no semicolon */);

  b->AddStatement(GotoErrorOnBadStatus());

  if (!method.IsOneway()) {
    // Strip off the exception header and fail if we see a remote exception.
    // _aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
    // if (_aidl_ret_status != ::android::OK) { goto error; }
    // if (!_aidl_status.isOk()) { return _aidl_ret_status; }
    b->AddStatement(new Assignment(
        kAndroidStatusVarName,
        StringPrintf("%s.readFromParcel(%s)", kStatusVarName, kReplyVarName)));
    b->AddStatement(GotoErrorOnBadStatus());
    IfStatement* exception_check = new IfStatement(
        new LiteralExpression(StringPrintf("!%s.isOk()", kStatusVarName)));
    b->AddStatement(exception_check);
    exception_check->OnTrue()->AddLiteral(
        StringPrintf("return %s", kStatusVarName));
  }

  // Type checking should guarantee that nothing below emits code until "return
  // status" if we are a oneway method, so no more fear of accessing reply.

  // If the method is expected to return something, read it first by convention.
  if (method.GetType().GetName() != "void") {
    const string& method_call = ParcelReadMethodOf(method.GetType(), typenames);
    b->AddStatement(new Assignment(
        kAndroidStatusVarName,
        new MethodCall(StringPrintf("%s.%s", kReplyVarName, method_call.c_str()),
                       ParcelReadCastOf(method.GetType(), typenames, kReturnVarName))));
    b->AddStatement(GotoErrorOnBadStatus());
  }

  for (const AidlArgument* a : method.GetOutArguments()) {
    // Deserialization looks roughly like:
    //     _aidl_ret_status = _aidl_reply.ReadInt32(out_param_name);
    //     if (_aidl_status != ::android::OK) { goto _aidl_error; }
    string method = ParcelReadMethodOf(a->GetType(), typenames);

    b->AddStatement(
        new Assignment(kAndroidStatusVarName,
                       new MethodCall(StringPrintf("%s.%s", kReplyVarName, method.c_str()),
                                      ParcelReadCastOf(a->GetType(), typenames, a->GetName()))));
    b->AddStatement(GotoErrorOnBadStatus());
  }

  // If we've gotten to here, one of two things is true:
  //   1) We've read some bad status_t
  //   2) We've only read status_t == OK and there was no exception in the
  //      response.
  // In both cases, we're free to set Status from the status_t and return.
  b->AddLiteral(StringPrintf("%s:\n", kErrorLabel), false /* no semicolon */);
  b->AddLiteral(
      StringPrintf("%s.setFromStatusT(%s)", kStatusVarName,
                   kAndroidStatusVarName));

  if (options.GenLog()) {
    b->AddLiteral(GenLogAfterExecute(bp_name, interface, method, kStatusVarName, kReturnVarName,
                                     false /* isServer */, false /* isNdk */),
                  false /* no semicolon */);
  }

  b->AddLiteral(StringPrintf("return %s", kStatusVarName));

  return unique_ptr<Declaration>(ret.release());
}

unique_ptr<Declaration> DefineClientMetaTransaction(const AidlTypenames& /* typenames */,
                                                    const AidlInterface& interface,
                                                    const AidlMethod& method,
                                                    const Options& options) {
  AIDL_FATAL_IF(method.IsUserDefined(), method);
  if (method.GetName() == kGetInterfaceVersion && options.Version() > 0) {
    const string iface = ClassName(interface, ClassNames::INTERFACE);
    const string proxy = ClassName(interface, ClassNames::CLIENT);
    // Note: race condition can happen here, but no locking is required
    // because 1) writing an interger is atomic and 2) this transaction
    // will always return the same value, i.e., competing threads will
    // give write the same value to cached_version_.
    std::ostringstream code;
    code << "int32_t " << proxy << "::" << kGetInterfaceVersion << "() {\n"
         << "  if (cached_version_ == -1) {\n"
         << "    ::android::Parcel data;\n"
         << "    ::android::Parcel reply;\n"
         << "    data.writeInterfaceToken(getInterfaceDescriptor());\n"
         << "    ::android::status_t err = remote()->transact(" << GetTransactionIdFor(interface, method)
         << ", data, &reply);\n"
         << "    if (err == ::android::OK) {\n"
         << "      ::android::binder::Status _aidl_status;\n"
         << "      err = _aidl_status.readFromParcel(reply);\n"
         << "      if (err == ::android::OK && _aidl_status.isOk()) {\n"
         << "        cached_version_ = reply.readInt32();\n"
         << "      }\n"
         << "    }\n"
         << "  }\n"
         << "  return cached_version_;\n"
         << "}\n";
    return unique_ptr<Declaration>(new LiteralDecl(code.str()));
  }
  if (method.GetName() == kGetInterfaceHash && !options.Hash().empty()) {
    const string iface = ClassName(interface, ClassNames::INTERFACE);
    const string proxy = ClassName(interface, ClassNames::CLIENT);
    std::ostringstream code;
    code << "std::string " << proxy << "::" << kGetInterfaceHash << "() {\n"
         << "  std::lock_guard<std::mutex> lockGuard(cached_hash_mutex_);\n"
         << "  if (cached_hash_ == \"-1\") {\n"
         << "    ::android::Parcel data;\n"
         << "    ::android::Parcel reply;\n"
         << "    data.writeInterfaceToken(getInterfaceDescriptor());\n"
         << "    ::android::status_t err = remote()->transact(" << GetTransactionIdFor(interface, method)
         << ", data, &reply);\n"
         << "    if (err == ::android::OK) {\n"
         << "      ::android::binder::Status _aidl_status;\n"
         << "      err = _aidl_status.readFromParcel(reply);\n"
         << "      if (err == ::android::OK && _aidl_status.isOk()) {\n"
         << "        reply.readUtf8FromUtf16(&cached_hash_);\n"
         << "      }\n"
         << "    }\n"
         << "  }\n"
         << "  return cached_hash_;\n"
         << "}\n";
    return unique_ptr<Declaration>(new LiteralDecl(code.str()));
  }
  return nullptr;
}

}  // namespace

unique_ptr<Document> BuildClientSource(const AidlTypenames& typenames,
                                       const AidlInterface& interface, const Options& options) {
  vector<string> include_list = {
      HeaderFile(interface, ClassNames::CLIENT, false),
      HeaderFile(interface, ClassNames::SERVER, false), // for TRANSACTION_* consts
      kParcelHeader,
      kAndroidBaseMacrosHeader
  };
  if (options.GenLog()) {
    include_list.emplace_back("chrono");
    include_list.emplace_back("functional");
  }
  vector<unique_ptr<Declaration>> file_decls;

  // The constructor just passes the IBinder instance up to the super
  // class.
  const string i_name = ClassName(interface, ClassNames::INTERFACE);
  const string bp_name = ClassName(interface, ClassNames::CLIENT);
  file_decls.push_back(unique_ptr<Declaration>{new ConstructorImpl{
      bp_name,
      ArgList{StringPrintf("const ::android::sp<::android::IBinder>& %s", kImplVarName)},
      {"BpInterface<" + i_name + ">(" + kImplVarName + ")"}}});

  if (options.GenLog()) {
    string code;
    CodeWriterPtr writer = CodeWriter::ForString(&code);
    (*writer) << "std::function<void(const " + bp_name + "::TransactionLog&)> " << bp_name
              << "::logFunc;\n";
    writer->Close();
    file_decls.push_back(unique_ptr<Declaration>(new LiteralDecl(code)));
  }

  // Clients define a method per transaction.
  for (const auto& method : interface.GetMethods()) {
    unique_ptr<Declaration> m;
    if (method->IsUserDefined()) {
      m = DefineClientTransaction(typenames, interface, *method, options);
    } else {
      m = DefineClientMetaTransaction(typenames, interface, *method, options);
    }
    if (!m) { return nullptr; }
    file_decls.push_back(std::move(m));
  }
  return unique_ptr<Document>{new CppSource{
      include_list,
      NestInNamespaces(std::move(file_decls), interface.GetSplitPackage())}};
}

namespace {

void GenerateConstantDefinitions(CodeWriter& out, const AidlDefinedType& type,
                                 const AidlTypenames& typenames, const string& template_decl,
                                 const string& q_name) {
  for (const auto& constant : type.GetConstantDeclarations()) {
    const AidlConstantValue& value = constant->GetValue();
    if (value.GetType() != AidlConstantValue::Type::STRING) continue;

    std::string cpp_type = CppNameOf(constant->GetType(), typenames);
    out << template_decl;
    out << "const " << cpp_type << "& " << q_name << "::" << constant->GetName() << "() {\n";
    out << "  static const " << cpp_type << " value("
        << constant->ValueString(ConstantValueDecorator) << ");\n";
    out << "  return value;\n";
    out << "}\n";
  }
}

void GenerateConstantDeclarations(CodeWriter& out, const AidlDefinedType& type,
                                  const AidlTypenames& typenames) {
  for (const auto& constant : type.GetConstantDeclarations()) {
    const AidlTypeSpecifier& type = constant->GetType();
    const auto cpp_type = CppNameOf(type, typenames);
    if (type.Signature() == "String") {
      out << "static const " << cpp_type << "& " << constant->GetName() << "()";
      cpp::GenerateDeprecated(out, *constant);
      out << ";\n";
    } else {
      out << "enum : " << cpp_type << " { " << constant->GetName();
      cpp::GenerateDeprecated(out, *constant);
      out << " = " << constant->ValueString(ConstantValueDecorator) << " };\n";
    }
  }
}

bool HandleServerTransaction(const AidlTypenames& typenames, const AidlInterface& interface,
                             const AidlMethod& method, const Options& options, StatementBlock* b) {
  // Declare all the parameters now.  In the common case, we expect no errors
  // in serialization.
  for (const unique_ptr<AidlArgument>& a : method.GetArguments()) {
    b->AddLiteral(StringPrintf("%s %s", CppNameOf(a->GetType(), typenames).c_str(),
                               BuildVarName(*a).c_str()));
  }

  // Declare a variable to hold the return value.
  if (method.GetType().GetName() != "void") {
    string type = CppNameOf(method.GetType(), typenames);
    b->AddLiteral(StringPrintf("%s %s", type.c_str(), kReturnVarName));
  }

  // Check that the client is calling the correct interface.
  IfStatement* interface_check = new IfStatement(
      new MethodCall(StringPrintf("%s.checkInterface",
                                  kDataVarName), "this"),
      true /* invert the check */);
  b->AddStatement(interface_check);
  interface_check->OnTrue()->AddStatement(
      new Assignment(kAndroidStatusVarName, "::android::BAD_TYPE"));
  interface_check->OnTrue()->AddLiteral("break");

  if (options.GenTraces()) {
    b->AddLiteral(
        StringPrintf("::android::ScopedTrace %s(ATRACE_TAG_AIDL, \"AIDL::cpp::%s::%s::cppServer\")",
                     kTraceVarName, interface.GetName().c_str(), method.GetName().c_str()));
  }

  // Deserialize each "in" parameter to the transaction.
  for (const auto& a: method.GetArguments()) {
    // Deserialization looks roughly like:
    //     _aidl_ret_status = _aidl_data.ReadInt32(&in_param_name);
    //     if (_aidl_ret_status != ::android::OK) { break; }
    const string& var_name = "&" + BuildVarName(*a);
    if (a->IsIn()) {
      const string& readMethod = ParcelReadMethodOf(a->GetType(), typenames);
      b->AddStatement(
          new Assignment{kAndroidStatusVarName,
                         new MethodCall{string(kDataVarName) + "." + readMethod,
                                        ParcelReadCastOf(a->GetType(), typenames, var_name)}});
      b->AddStatement(BreakOnStatusNotOk());
    } else if (a->IsOut() && a->GetType().IsArray()) {
      // Special case, the length of the out array is written into the parcel.
      //     _aidl_ret_status = _aidl_data.resizeOutVector(&out_param_name);
      //     if (_aidl_ret_status != ::android::OK) { break; }
      b->AddStatement(
          new Assignment{kAndroidStatusVarName,
                         new MethodCall{string(kDataVarName) + ".resizeOutVector", var_name}});
      b->AddStatement(BreakOnStatusNotOk());
    }
  }

  const string bn_name = ClassName(interface, ClassNames::SERVER);
  if (options.GenLog()) {
    b->AddLiteral(GenLogBeforeExecute(bn_name, method, true /* isServer */, false /* isNdk */),
                  false);
  }
  // Call the actual method.  This is implemented by the subclass.
  vector<unique_ptr<AstNode>> status_args;
  status_args.emplace_back(new MethodCall(
      method.GetName(), BuildArgList(typenames, method, false /* not for method decl */)));
  b->AddStatement(new Statement(new MethodCall(
      StringPrintf("%s %s", kBinderStatusLiteral, kStatusVarName),
      ArgList(std::move(status_args)))));

  if (options.GenLog()) {
    b->AddLiteral(GenLogAfterExecute(bn_name, interface, method, kStatusVarName, kReturnVarName,
                                     true /* isServer */, false /* isNdk */),
                  false);
  }

  // Write exceptions during transaction handling to parcel.
  if (!method.IsOneway()) {
    b->AddStatement(new Assignment(
        kAndroidStatusVarName,
        StringPrintf("%s.writeToParcel(%s)", kStatusVarName, kReplyVarName)));
    b->AddStatement(BreakOnStatusNotOk());
    IfStatement* exception_check = new IfStatement(
        new LiteralExpression(StringPrintf("!%s.isOk()", kStatusVarName)));
    b->AddStatement(exception_check);
    exception_check->OnTrue()->AddLiteral("break");
  }

  // If we have a return value, write it first.
  if (method.GetType().GetName() != "void") {
    string writeMethod =
        string(kReplyVarName) + "->" + ParcelWriteMethodOf(method.GetType(), typenames);
    b->AddStatement(new Assignment(
        kAndroidStatusVarName,
        new MethodCall(writeMethod,
                       ParcelWriteCastOf(method.GetType(), typenames, kReturnVarName))));
    b->AddStatement(BreakOnStatusNotOk());
  }
  // Write each out parameter to the reply parcel.
  for (const AidlArgument* a : method.GetOutArguments()) {
    // Serialization looks roughly like:
    //     _aidl_ret_status = data.WriteInt32(out_param_name);
    //     if (_aidl_ret_status != ::android::OK) { break; }
    const string& writeMethod = ParcelWriteMethodOf(a->GetType(), typenames);
    b->AddStatement(new Assignment(
        kAndroidStatusVarName,
        new MethodCall(string(kReplyVarName) + "->" + writeMethod,
                       ParcelWriteCastOf(a->GetType(), typenames, BuildVarName(*a)))));
    b->AddStatement(BreakOnStatusNotOk());
  }

  return true;
}

bool HandleServerMetaTransaction(const AidlTypenames&, const AidlInterface& interface,
                                 const AidlMethod& method, const Options& options,
                                 StatementBlock* b) {
  AIDL_FATAL_IF(method.IsUserDefined(), method);

  if (method.GetName() == kGetInterfaceVersion && options.Version() > 0) {
    std::ostringstream code;
    code << "_aidl_data.checkInterface(this);\n"
         << "_aidl_reply->writeNoException();\n"
         << "_aidl_reply->writeInt32(" << ClassName(interface, ClassNames::INTERFACE)
         << "::VERSION)";
    b->AddLiteral(code.str());
    return true;
  }
  if (method.GetName() == kGetInterfaceHash && !options.Hash().empty()) {
    std::ostringstream code;
    code << "_aidl_data.checkInterface(this);\n"
         << "_aidl_reply->writeNoException();\n"
         << "_aidl_reply->writeUtf8AsUtf16(" << ClassName(interface, ClassNames::INTERFACE)
         << "::HASH)";
    b->AddLiteral(code.str());
    return true;
  }
  return false;
}

}  // namespace

unique_ptr<Document> BuildServerSource(const AidlTypenames& typenames,
                                       const AidlInterface& interface, const Options& options) {
  const string bn_name = ClassName(interface, ClassNames::SERVER);
  vector<string> include_list{
      HeaderFile(interface, ClassNames::SERVER, false),
      kParcelHeader,
      kStabilityHeader,
  };
  if (options.GenLog()) {
    include_list.emplace_back("chrono");
    include_list.emplace_back("functional");
  }

  unique_ptr<ConstructorImpl> constructor{new ConstructorImpl{bn_name, ArgList{}, {}}};

  if (interface.IsVintfStability()) {
    constructor->GetStatementBlock()->AddLiteral("::android::internal::Stability::markVintf(this)");
  } else {
    constructor->GetStatementBlock()->AddLiteral(
        "::android::internal::Stability::markCompilationUnit(this)");
  }

  unique_ptr<MethodImpl> on_transact{
      new MethodImpl{kAndroidStatusLiteral,
                     bn_name,
                     "onTransact",
                     {},
                     ArgList{{StringPrintf("uint32_t %s", kCodeVarName),
                              StringPrintf("const %s& %s", kAndroidParcelLiteral, kDataVarName),
                              StringPrintf("%s* %s", kAndroidParcelLiteral, kReplyVarName),
                              StringPrintf("uint32_t %s", kFlagsVarName)}}}};

  // Declare the status_t variable
  on_transact->GetStatementBlock()->AddLiteral(
      StringPrintf("%s %s = %s", kAndroidStatusLiteral, kAndroidStatusVarName,
                   kAndroidStatusOk));

  // Add the all important switch statement, but retain a pointer to it.
  SwitchStatement* s = new SwitchStatement{kCodeVarName};
  on_transact->GetStatementBlock()->AddStatement(s);

  // The switch statement has a case statement for each transaction code.
  for (const auto& method : interface.GetMethods()) {
    StatementBlock* b = s->AddCase(GetTransactionIdFor(interface, *method));
    if (!b) { return nullptr; }

    bool success = false;
    if (method->IsUserDefined()) {
      success = HandleServerTransaction(typenames, interface, *method, options, b);
    } else {
      success = HandleServerMetaTransaction(typenames, interface, *method, options, b);
    }
    if (!success) {
      return nullptr;
    }
  }

  // The switch statement has a default case which defers to the super class.
  // The superclass handles a few pre-defined transactions.
  StatementBlock* b = s->AddCase("");
  b->AddLiteral(StringPrintf(
                "%s = ::android::BBinder::onTransact(%s, %s, "
                "%s, %s)", kAndroidStatusVarName, kCodeVarName,
                kDataVarName, kReplyVarName, kFlagsVarName));

  // If we saw a null reference, we can map that to an appropriate exception.
  IfStatement* null_check = new IfStatement(
      new LiteralExpression(string(kAndroidStatusVarName) +
                            " == ::android::UNEXPECTED_NULL"));
  on_transact->GetStatementBlock()->AddStatement(null_check);
  null_check->OnTrue()->AddStatement(new Assignment(
      kAndroidStatusVarName,
      StringPrintf("%s::fromExceptionCode(%s::EX_NULL_POINTER)"
                   ".writeToParcel(%s)",
                   kBinderStatusLiteral, kBinderStatusLiteral,
                   kReplyVarName)));

  // Finally, the server's onTransact method just returns a status code.
  on_transact->GetStatementBlock()->AddLiteral(
      StringPrintf("return %s", kAndroidStatusVarName));
  vector<unique_ptr<Declaration>> decls;
  decls.push_back(std::move(constructor));

  bool deprecated = interface.IsDeprecated() ||
                    std::any_of(interface.GetMethods().begin(), interface.GetMethods().end(),
                                [](const auto& m) { return m->IsDeprecated(); });
  if (deprecated) {
    decls.emplace_back(
        new LiteralDecl("#pragma clang diagnostic push\n"
                        "#pragma clang diagnostic ignored \"-Wdeprecated\"\n"));
  }
  decls.push_back(std::move(on_transact));
  if (deprecated) {
    decls.emplace_back(new LiteralDecl("#pragma clang diagnostic pop\n"));
  }

  if (options.Version() > 0) {
    std::ostringstream code;
    code << "int32_t " << bn_name << "::" << kGetInterfaceVersion << "() {\n"
         << "  return " << ClassName(interface, ClassNames::INTERFACE) << "::VERSION;\n"
         << "}\n";
    decls.emplace_back(new LiteralDecl(code.str()));
  }
  if (!options.Hash().empty()) {
    std::ostringstream code;
    code << "std::string " << bn_name << "::" << kGetInterfaceHash << "() {\n"
         << "  return " << ClassName(interface, ClassNames::INTERFACE) << "::HASH;\n"
         << "}\n";
    decls.emplace_back(new LiteralDecl(code.str()));
  }

  if (options.GenLog()) {
    string code;
    CodeWriterPtr writer = CodeWriter::ForString(&code);
    (*writer) << "std::function<void(const " + bn_name + "::TransactionLog&)> " << bn_name
              << "::logFunc;\n";
    writer->Close();
    decls.push_back(unique_ptr<Declaration>(new LiteralDecl(code)));
  }
  return unique_ptr<Document>{
      new CppSource{include_list, NestInNamespaces(std::move(decls), interface.GetSplitPackage())}};
}

void GenerateInterfaceSource(CodeWriter& out, const AidlInterface& interface,
                             const AidlTypenames& typenames, const Options&) {
  out << "#include <" << HeaderFile(interface, ClassNames::RAW, false) << ">\n";
  out << "#include <" << HeaderFile(interface, ClassNames::CLIENT, false) << ">\n";

  EnterNamespace(out, interface);

  out << fmt::format("DO_NOT_DIRECTLY_USE_ME_IMPLEMENT_META_INTERFACE({}, \"{}\")\n",
                     ClassName(interface, ClassNames::BASE), interface.GetDescriptor());

  GenerateConstantDefinitions(out, interface, typenames, /*template_decl=*/"",
                              ClassName(interface, ClassNames::INTERFACE));

  LeaveNamespace(out, interface);
}

void GenerateClientHeader(CodeWriter& out, const AidlInterface& interface,
                          const AidlTypenames& typenames, const Options& options) {
  const string bp_name = ClassName(interface, ClassNames::CLIENT);
  const string iface = ClassName(interface, ClassNames::INTERFACE);

  out << "#pragma once\n\n";
  out << "#include <" << kIBinderHeader << ">\n";
  out << "#include <" << kIInterfaceHeader << ">\n";
  out << "#include <utils/Errors.h>\n";
  out << "#include <" << HeaderFile(interface, ClassNames::RAW, false) << ">\n";
  if (options.GenLog()) {
    out << "#include <functional>\n";  // for std::function
    out << "#include <android/binder_to_string.h>\n";
  }
  out << "\n";
  EnterNamespace(out, interface);
  out << "class";
  cpp::GenerateDeprecated(out, interface);
  out << " " << bp_name << " : public ::android::BpInterface<" << iface << "> {\n";
  out << "public:\n";
  out.Indent();
  out << "explicit " << bp_name << "(const ::android::sp<::android::IBinder>& " << kImplVarName
      << ");\n";
  out << "virtual ~" << bp_name << "() = default;\n";

  for (const auto& method : interface.GetMethods()) {
    if (method->IsUserDefined()) {
      BuildMethodDecl(*method, typenames, false)->Write(&out);
    } else {
      BuildMetaMethodDecl(*method, typenames, options, false)->Write(&out);
    }
  }

  if (options.GenLog()) {
    out << kTransactionLogStruct;
    out << "static std::function<void(const TransactionLog&)> logFunc;\n";
  }
  out.Dedent();

  if (options.Version() > 0 || !options.Hash().empty()) {
    out << "private:\n";
    out.Indent();
    if (options.Version() > 0) {
      out << "int32_t cached_version_ = -1;\n";
    }
    if (!options.Hash().empty()) {
      out << "std::string cached_hash_ = \"-1\";\n";
      out << "std::mutex cached_hash_mutex_;\n";
    }
    out.Dedent();
  }

  out << "};  // class " << bp_name << "\n";
  LeaveNamespace(out, interface);
}

void GenerateServerHeader(CodeWriter& out, const AidlInterface& interface,
                          const AidlTypenames& /* typenames */, const Options& options) {
  const string bn_name = ClassName(interface, ClassNames::SERVER);
  const string iface = ClassName(interface, ClassNames::INTERFACE);

  out << "#pragma once\n\n";
  out << "#include <binder/IInterface.h>\n";
  out << "#include <" << HeaderFile(interface, ClassNames::RAW, false) << ">\n";
  if (options.GenLog()) {
    out << "#include <functional>\n";  // for std::function
    out << "#include <android/binder_to_string.h>\n";
  }
  out << "\n";
  EnterNamespace(out, interface);
  out << "class";
  cpp::GenerateDeprecated(out, interface);
  out << " " << bn_name << " : public "
      << "::android::BnInterface<" << iface << "> {\n";
  out << "public:\n";
  out.Indent();
  for (const auto& method : interface.GetMethods()) {
    out << "static constexpr uint32_t TRANSACTION_" << method->GetName() << " = "
        << "::android::IBinder::FIRST_CALL_TRANSACTION + " << std::to_string(method->GetId())
        << ";\n";
  }
  out << "explicit " << bn_name << "();\n";
  out << fmt::format("{} onTransact(uint32_t {}, const {}& {}, {}* {}, uint32_t {}) override;\n",
                     kAndroidStatusLiteral, kCodeVarName, kAndroidParcelLiteral, kDataVarName,
                     kAndroidParcelLiteral, kReplyVarName, kFlagsVarName);
  if (options.Version() > 0) {
    out << "int32_t " << kGetInterfaceVersion << "() final;\n";
  }
  if (!options.Hash().empty()) {
    out << "std::string " << kGetInterfaceHash << "();\n";
  }
  if (options.GenLog()) {
    out << kTransactionLogStruct;
    out << "static std::function<void(const TransactionLog&)> logFunc;\n";
  }
  out.Dedent();
  out << "};  // class " << bn_name << "\n";
  LeaveNamespace(out, interface);
}

void GenerateClassDecl(CodeWriter& out, const AidlDefinedType& defined_type,
                       const AidlTypenames& typenames, const Options& options);

void GenerateInterfaceClassDecl(CodeWriter& out, const AidlInterface& interface,
                                const AidlTypenames& typenames, const Options& options) {
  const string i_name = ClassName(interface, ClassNames::INTERFACE);

  out << "class";
  GenerateDeprecated(out, interface);
  out << " " << i_name << " : public ::android::IInterface {\n";
  out << "public:\n";
  out.Indent();
  out << "DECLARE_META_INTERFACE(" << ClassName(interface, ClassNames::BASE) << ")\n";
  if (options.Version() > 0) {
    out << "const int32_t VERSION = " << std::to_string(options.Version()) << ";\n";
  }
  if (!options.Hash().empty()) {
    out << "const std::string HASH = \"" << options.Hash() << "\";\n";
  }
  // TODO(b/201376182) consider re-ordering nested type decls.
  for (const auto& nested : interface.GetNestedTypes()) {
    GenerateClassDecl(out, *nested, typenames, options);
  }
  GenerateConstantDeclarations(out, interface, typenames);
  for (const auto& method : interface.GetMethods()) {
    if (method->IsUserDefined()) {
      // Each method gets an enum entry and pure virtual declaration.
      BuildMethodDecl(*method, typenames, true)->Write(&out);
    } else {
      BuildMetaMethodDecl(*method, typenames, options, true)->Write(&out);
    }
  }
  out.Dedent();
  out << "};  // class " << i_name << "\n";
  out << "\n";

  // Implement the default impl class.
  const string default_impl = ClassName(interface, ClassNames::DEFAULT_IMPL);
  out << "class";
  GenerateDeprecated(out, interface);
  out << " " << default_impl << " : public " << i_name << " {\n";
  out << "public:\n";
  out.Indent();
  // onAsBinder returns nullptr as this interface is not associated with a
  // real binder.
  out << "::android::IBinder* onAsBinder() override {\n"
      << "  return nullptr;\n"
      << "}\n";

  // Each interface method by default returns UNKNOWN_TRANSACTION with is
  // the same status that is returned by transact() when the method is
  // not implemented in the server side. In other words, these default
  // methods do nothing; they only exist to aid making a real default
  // impl class without having to override all methods in an interface.
  for (const auto& method : interface.GetMethods()) {
    if (method->IsUserDefined()) {
      out << "::android::binder::Status " << method->GetName()
          << BuildArgList(typenames, *method, true, true).ToString() << " override";
      GenerateDeprecated(out, *method);
      out << " {\n"
          << "  return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);\n"
          << "}\n";
    } else if (method->GetName() == kGetInterfaceVersion && options.Version() > 0) {
      out << "int32_t " << kGetInterfaceVersion << "() override {\n"
          << "  return 0;\n"
          << "}\n";
    } else if (method->GetName() == kGetInterfaceHash && !options.Hash().empty()) {
      out << "std::string " << kGetInterfaceHash << "() override {\n"
          << "  return \"\";\n"
          << "}\n";
    }
  }
  out.Dedent();
  out << "};  // class " << default_impl << "\n";
}

string GetInitializer(const AidlTypenames& typenames, const AidlVariableDeclaration& variable) {
  string cppType = CppNameOf(variable.GetType(), typenames);
  return cppType + "(" + variable.ValueString(ConstantValueDecorator) + ")";
}

void GenerateReadFromParcel(CodeWriter& out, const AidlStructuredParcelable& parcel,
                            const AidlTypenames& typenames) {
  out << "::android::status_t _aidl_ret_status = ::android::OK;\n";
  out << "[[maybe_unused]] size_t _aidl_start_pos = " << kParcelVarName << "->dataPosition();\n";
  out << "int32_t _aidl_parcelable_raw_size = " << kParcelVarName << "->readInt32();\n";
  out << "if (_aidl_parcelable_raw_size < 0) return ::android::BAD_VALUE;\n";
  out << "[[maybe_unused]] size_t _aidl_parcelable_size = "
      << "static_cast<size_t>(_aidl_parcelable_raw_size);\n";
  out << "if (_aidl_start_pos > SIZE_MAX - _aidl_parcelable_size) return ::android::BAD_VALUE;\n";
  for (const auto& variable : parcel.GetFields()) {
    string method = ParcelReadMethodOf(variable->GetType(), typenames);
    string arg = ParcelReadCastOf(variable->GetType(), typenames, "&" + variable->GetName());
    out << "if (" << kParcelVarName
        << "->dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) {\n";
    out << "  " << kParcelVarName
        << "->setDataPosition(_aidl_start_pos + _aidl_parcelable_size);\n";
    out << "  return _aidl_ret_status;\n";
    out << "}\n";
    out << "_aidl_ret_status = " << kParcelVarName << "->" << method << "(" << arg << ");\n";
    out << "if (((_aidl_ret_status) != (::android::OK))) {\n";
    out << "  return _aidl_ret_status;\n";
    out << "}\n";
  }
  out << "" << kParcelVarName << "->setDataPosition(_aidl_start_pos + _aidl_parcelable_size);\n";
  out << "return _aidl_ret_status;\n";
}

void GenerateWriteToParcel(CodeWriter& out, const AidlStructuredParcelable& parcel,
                           const AidlTypenames& typenames) {
  out << "::android::status_t _aidl_ret_status = ::android::OK;\n";
  out << "auto _aidl_start_pos = " << kParcelVarName << "->dataPosition();\n";
  out << kParcelVarName << "->writeInt32(0);\n";
  for (const auto& variable : parcel.GetFields()) {
    string method = ParcelWriteMethodOf(variable->GetType(), typenames);
    string arg = ParcelWriteCastOf(variable->GetType(), typenames, variable->GetName());
    out << "_aidl_ret_status = " << kParcelVarName << "->" << method << "(" << arg << ");\n";
    out << "if (((_aidl_ret_status) != (::android::OK))) {\n";
    out << "  return _aidl_ret_status;\n";
    out << "}\n";
  }
  out << "auto _aidl_end_pos = " << kParcelVarName << "->dataPosition();\n";
  out << kParcelVarName << "->setDataPosition(_aidl_start_pos);\n";
  out << kParcelVarName << "->writeInt32(_aidl_end_pos - _aidl_start_pos);\n";
  out << kParcelVarName << "->setDataPosition(_aidl_end_pos);\n";
  out << "return _aidl_ret_status;\n";
}

ParcelWriterContext GetParcelWriterContext(const AidlTypenames& typenames) {
  return ParcelWriterContext{
      .status_type = kAndroidStatusLiteral,
      .status_ok = kAndroidStatusOk,
      .status_bad = kAndroidStatusBadValue,
      .read_func =
          [&](CodeWriter& out, const string& var, const AidlTypeSpecifier& type) {
            out << fmt::format("{}->{}({})", kParcelVarName, ParcelReadMethodOf(type, typenames),
                               ParcelReadCastOf(type, typenames, "&" + var));
          },
      .write_func =
          [&](CodeWriter& out, const string& value, const AidlTypeSpecifier& type) {
            out << fmt::format("{}->{}({})", kParcelVarName, ParcelWriteMethodOf(type, typenames),
                               ParcelWriteCastOf(type, typenames, value));
          },
  };
}

void GenerateReadFromParcel(CodeWriter& out, const AidlUnionDecl& decl,
                            const AidlTypenames& typenames) {
  UnionWriter uw{decl, typenames, &CppNameOf, &ConstantValueDecorator};
  uw.ReadFromParcel(out, GetParcelWriterContext(typenames));
}

void GenerateWriteToParcel(CodeWriter& out, const AidlUnionDecl& decl,
                           const AidlTypenames& typenames) {
  UnionWriter uw{decl, typenames, &CppNameOf, &ConstantValueDecorator};
  uw.WriteToParcel(out, GetParcelWriterContext(typenames));
}

void GenerateParcelFields(CodeWriter& out, const AidlStructuredParcelable& decl,
                          const AidlTypenames& typenames) {
  for (const auto& variable : decl.GetFields()) {
    std::string cppType = CppNameOf(variable->GetType(), typenames);
    out << cppType;
    GenerateDeprecated(out, *variable);
    out << " " << variable->GetName();
    if (variable->GetDefaultValue()) {
      out << " = " << GetInitializer(typenames, *variable);
    } else if (variable->GetType().GetName() == "ParcelableHolder") {
      if (decl.IsVintfStability()) {
        out << " { ::android::Parcelable::Stability::STABILITY_VINTF }";
      } else {
        out << " { ::android::Parcelable::Stability::STABILITY_LOCAL }";
      }
    }
    out << ";\n";
  }
}

void GenerateParcelFields(CodeWriter& out, const AidlUnionDecl& decl,
                          const AidlTypenames& typenames) {
  UnionWriter uw{decl, typenames, &CppNameOf, &ConstantValueDecorator};
  uw.PublicFields(out);
}

template <typename ParcelableType>
void GenerateParcelClassDecl(CodeWriter& out, const ParcelableType& parcel,
                             const AidlTypenames& typenames, const Options& options) {
  const string clazz = parcel.GetName();

  out << TemplateDecl(parcel);
  out << "class";
  GenerateDeprecated(out, parcel);
  out << " " << clazz << " : public ::android::Parcelable {\n";
  out << "public:\n";
  out.Indent();

  GenerateParcelableComparisonOperators(out, parcel);
  // TODO(b/201376182) consider re-ordering nested type decls.
  for (const auto& nested : parcel.GetNestedTypes()) {
    GenerateClassDecl(out, *nested, typenames, options);
  }

  GenerateParcelFields(out, parcel, typenames);

  GenerateConstantDeclarations(out, parcel, typenames);

  if (parcel.IsVintfStability()) {
    out << "::android::Parcelable::Stability getStability() const override { return "
           "::android::Parcelable::Stability::STABILITY_VINTF; }\n";
  }

  out << kAndroidStatusLiteral << " readFromParcel(const ::android::Parcel* _aidl_parcel) final;\n";
  out << kAndroidStatusLiteral << " writeToParcel(::android::Parcel* _aidl_parcel) const final;\n";

  const string canonical_name = parcel.GetCanonicalName();
  out << "static const ::android::String16& getParcelableDescriptor() {\n"
      << "  static const ::android::StaticString16 DESCIPTOR (u\"" << canonical_name << "\");\n"
      << "  return DESCIPTOR;\n"
      << "}\n";

  GenerateToString(out, parcel);

  out.Dedent();

  if (auto decl = AidlCast<AidlUnionDecl>(parcel); decl) {
    out << "private:\n";
    out.Indent();
    UnionWriter uw{*decl, typenames, &CppNameOf, &ConstantValueDecorator};
    uw.PrivateFields(out);
    out.Dedent();
  }

  out << "};  // class " << clazz << "\n";
}

template <typename T>
void GenerateParcelSource(CodeWriter& out, const T& parcel, const AidlTypenames& typenames,
                          const Options&) {
  string q_name = GetQualifiedName(parcel);
  if (parcel.IsGeneric()) {
    q_name += "<" + Join(parcel.GetTypeParameters(), ",") + ">";
  }

  out << "#include <" << CppHeaderForType(parcel) << ">\n\n";

  EnterNamespace(out, parcel);
  GenerateConstantDefinitions(out, parcel, typenames, TemplateDecl(parcel), q_name);

  out << TemplateDecl(parcel);
  out << "::android::status_t " << q_name << "::readFromParcel(const ::android::Parcel* "
      << kParcelVarName << ") {\n";
  out.Indent();
  GenerateReadFromParcel(out, parcel, typenames);
  out.Dedent();
  out << "}\n";

  out << TemplateDecl(parcel);
  out << "::android::status_t " << q_name << "::writeToParcel(::android::Parcel* " << kParcelVarName
      << ") const {\n";
  out.Indent();
  GenerateWriteToParcel(out, parcel, typenames);
  out.Dedent();
  out << "}\n";
  LeaveNamespace(out, parcel);
}

void GenerateEnumClassDecl(CodeWriter& out, const AidlEnumDeclaration& enum_decl,
                           const AidlTypenames& typenames) {
  cpp::GenerateEnumClassDecl(out, enum_decl, CppNameOf(enum_decl.GetBackingType(), typenames),
                             ConstantValueDecorator);
}

void GenerateClassDecl(CodeWriter& out, const AidlDefinedType& defined_type,
                       const AidlTypenames& typenames, const Options& options) {
  if (auto iface = AidlCast<AidlInterface>(defined_type); iface) {
    GenerateInterfaceClassDecl(out, *iface, typenames, options);
  } else if (auto parcelable = AidlCast<AidlStructuredParcelable>(defined_type); parcelable) {
    GenerateParcelClassDecl(out, *parcelable, typenames, options);
  } else if (auto union_decl = AidlCast<AidlUnionDecl>(defined_type); union_decl) {
    GenerateParcelClassDecl(out, *union_decl, typenames, options);
  } else if (auto enum_decl = AidlCast<AidlEnumDeclaration>(defined_type); enum_decl) {
    GenerateEnumClassDecl(out, *enum_decl, typenames);
  } else {
    AIDL_FATAL(defined_type) << "Unrecognized type sent for CPP generation.";
  }
}

}  // namespace internals

using namespace internals;

// Ensures that output_file is  <out_dir>/<packagename>/<typename>.cpp
bool ValidateOutputFilePath(const string& output_file, const Options& options,
                            const AidlDefinedType& defined_type) {
  const auto& out_dir =
      !options.OutputDir().empty() ? options.OutputDir() : options.OutputHeaderDir();
  if (output_file.empty() || !android::base::StartsWith(output_file, out_dir)) {
    // If output_file is not set (which happens in the unit tests) or is outside of out_dir, we can
    // help but accepting it, because the path is what the user has requested.
    return true;
  }

  string canonical_name = defined_type.GetCanonicalName();
  std::replace(canonical_name.begin(), canonical_name.end(), '.', OS_PATH_SEPARATOR);
  const string expected = out_dir + canonical_name + ".cpp";
  if (expected != output_file) {
    AIDL_ERROR(defined_type) << "Output file is expected to be at " << expected << ", but is "
                             << output_file << ".\n If this is an Android platform "
                             << "build, consider providing the input AIDL files using a filegroup "
                             << "with `path:\"<base>\"` so that the AIDL files are located at "
                             << "<base>/<packagename>/<typename>.aidl.";
    return false;
  }
  return true;
}

// Collect all includes for the type's header. Nested types are visited as well via VisitTopDown.
void GenerateHeaderIncludes(CodeWriter& out, const AidlDefinedType& defined_type,
                            const AidlTypenames& typenames, const Options& options) {
  struct Visitor : AidlVisitor {
    const AidlTypenames& typenames;
    const Options& options;
    std::set<std::string> includes;
    Visitor(const AidlTypenames& typenames, const Options& options)
        : typenames(typenames), options(options) {}

    // Collect includes for each type reference including built-in type
    void Visit(const AidlTypeSpecifier& type) override {
      cpp::AddHeaders(type, typenames, &includes);
    }

    // Collect implementation-specific includes for each type definition
    void Visit(const AidlInterface&) override {
      includes.insert(kIBinderHeader);        // IBinder
      includes.insert(kIInterfaceHeader);     // IInterface
      includes.insert(kStatusHeader);         // Status
      includes.insert(kStrongPointerHeader);  // sp<>

      if (options.GenTraces()) {
        includes.insert(kTraceHeader);
      }
    }

    void Visit(const AidlStructuredParcelable&) override {
      AddParcelableCommonHeaders();
      includes.insert("tuple");  // std::tie in comparison operators
    }

    void Visit(const AidlUnionDecl&) override {
      AddParcelableCommonHeaders();
      includes.insert(std::begin(UnionWriter::headers), std::end(UnionWriter::headers));
    }

    void Visit(const AidlEnumDeclaration&) override {
      includes.insert("array");           // used in enum_values
      includes.insert("binder/Enums.h");  // provides enum_range
      includes.insert("string");          // toString() returns std::string
    }

    void AddParcelableCommonHeaders() {
      includes.insert(kParcelHeader);                 // Parcel in readFromParcel/writeToParcel
      includes.insert(kStatusHeader);                 // Status
      includes.insert(kString16Header);               // String16 in getParcelableDescriptor
      includes.insert("android/binder_to_string.h");  // toString()
    }
  } v(typenames, options);
  VisitTopDown(v, defined_type);

  for (const auto& path : v.includes) {
    out << "#include <" << path << ">\n";
  }
  out << "\n";
  if (v.includes.count("cassert")) {
    // TODO(b/31559095) bionic on host should define __assert2
    out << "#ifndef __BIONIC__\n#define __assert2(a,b,c,d) ((void)0)\n#endif\n\n";
  }
}

// Generic parcelables and enum utilities should be defined in header.
void GenerateHeaderDefinitions(CodeWriter& out, const AidlDefinedType& defined_type,
                               const AidlTypenames& typenames, const Options& options) {
  struct Visitor : AidlVisitor {
    CodeWriter& out;
    const AidlTypenames& typenames;
    const Options& options;
    Visitor(CodeWriter& out, const AidlTypenames& typenames, const Options& options)
        : out(out), typenames(typenames), options(options) {}

    void Visit(const AidlEnumDeclaration& enum_decl) override {
      const auto backing_type = CppNameOf(enum_decl.GetBackingType(), typenames);
      EnterNamespace(out, enum_decl);
      out << GenerateEnumToString(enum_decl, backing_type);
      LeaveNamespace(out, enum_decl);

      out << "namespace android {\n";
      out << "namespace internal {\n";
      out << GenerateEnumValues(enum_decl, {""});
      out << "}  // namespace internal\n";
      out << "}  // namespace android\n";
    }

    void Visit(const AidlStructuredParcelable& parcelable) override {
      if (parcelable.IsGeneric()) {
        GenerateParcelSource(out, parcelable, typenames, options);
      }
    }

    void Visit(const AidlUnionDecl& union_decl) override {
      if (union_decl.IsGeneric()) {
        GenerateParcelSource(out, union_decl, typenames, options);
      }
    }

  } v(out, typenames, options);
  VisitTopDown(v, defined_type);
}

void GenerateHeader(CodeWriter& out, const AidlDefinedType& defined_type,
                    const AidlTypenames& typenames, const Options& options) {
  if (auto parcelable = AidlCast<AidlParcelable>(defined_type); parcelable) {
    out << "#error TODO(b/111362593) parcelables do not have headers";
    return;
  }
  out << "#pragma once\n\n";
  GenerateHeaderIncludes(out, defined_type, typenames, options);
  EnterNamespace(out, defined_type);
  // Each class decl contains its own nested types' class decls
  GenerateClassDecl(out, defined_type, typenames, options);
  LeaveNamespace(out, defined_type);
  GenerateHeaderDefinitions(out, defined_type, typenames, options);
}

void GenerateClientHeader(CodeWriter& out, const AidlDefinedType& defined_type,
                          const AidlTypenames& typenames, const Options& options) {
  if (auto iface = AidlCast<AidlInterface>(defined_type); iface) {
    GenerateClientHeader(out, *iface, typenames, options);
  } else if (auto parcelable = AidlCast<AidlStructuredParcelable>(defined_type); parcelable) {
    out << "#error TODO(b/111362593) parcelables do not have bp classes";
  } else if (auto union_decl = AidlCast<AidlUnionDecl>(defined_type); union_decl) {
    out << "#error TODO(b/111362593) parcelables do not have bp classes";
  } else if (auto enum_decl = AidlCast<AidlEnumDeclaration>(defined_type); enum_decl) {
    out << "#error TODO(b/111362593) enums do not have bp classes";
  } else if (auto parcelable = AidlCast<AidlParcelable>(defined_type); parcelable) {
    out << "#error TODO(b/111362593) parcelables do not have bp classes";
  } else {
    AIDL_FATAL(defined_type) << "Unrecognized type sent for CPP generation.";
  }
}

void GenerateServerHeader(CodeWriter& out, const AidlDefinedType& defined_type,
                          const AidlTypenames& typenames, const Options& options) {
  if (auto iface = AidlCast<AidlInterface>(defined_type); iface) {
    GenerateServerHeader(out, *iface, typenames, options);
  } else if (auto parcelable = AidlCast<AidlStructuredParcelable>(defined_type); parcelable) {
    out << "#error TODO(b/111362593) parcelables do not have bn classes";
  } else if (auto union_decl = AidlCast<AidlUnionDecl>(defined_type); union_decl) {
    out << "#error TODO(b/111362593) parcelables do not have bn classes";
  } else if (auto enum_decl = AidlCast<AidlEnumDeclaration>(defined_type); enum_decl) {
    out << "#error TODO(b/111362593) enums do not have bn classes";
  } else if (auto parcelable = AidlCast<AidlParcelable>(defined_type); parcelable) {
    out << "#error TODO(b/111362593) parcelables do not have bn classes";
  } else {
    AIDL_FATAL(defined_type) << "Unrecognized type sent for CPP generation.";
  }
}

void GenerateSource(CodeWriter& out, const AidlDefinedType& defined_type,
                    const AidlTypenames& typenames, const Options& options) {
  struct Visitor : AidlVisitor {
    CodeWriter& out;
    const AidlTypenames& typenames;
    const Options& options;
    Visitor(CodeWriter& out, const AidlTypenames& typenames, const Options& options)
        : out(out), typenames(typenames), options(options) {}

    void Visit(const AidlInterface& interface) override {
      GenerateInterfaceSource(out, interface, typenames, options);
      BuildClientSource(typenames, interface, options)->Write(&out);
      BuildServerSource(typenames, interface, options)->Write(&out);
    }

    void Visit(const AidlStructuredParcelable& parcelable) override {
      if (!parcelable.IsGeneric()) {
        GenerateParcelSource(out, parcelable, typenames, options);
      } else {
        out << "\n";
      }
    }

    void Visit(const AidlUnionDecl& union_decl) override {
      if (!union_decl.IsGeneric()) {
        GenerateParcelSource(out, union_decl, typenames, options);
      } else {
        out << "\n";
      }
    }

    void Visit(const AidlEnumDeclaration& enum_decl) override {
      if (!enum_decl.GetParentType()) {
        out << "// This file is intentionally left blank as placeholder for enum declaration.\n";
      }
    }

    void Visit(const AidlParcelable& parcelable) override {
      AIDL_FATAL_IF(parcelable.GetParentType(), parcelable)
          << "Unstructured parcelable can't be nested.";
      out << "// This file is intentionally left blank as placeholder for parcel declaration.\n";
    }
  } v(out, typenames, options);
  VisitTopDown(v, defined_type);
}

bool GenerateCpp(const string& output_file, const Options& options, const AidlTypenames& typenames,
                 const AidlDefinedType& defined_type, const IoDelegate& io_delegate) {
  if (!ValidateOutputFilePath(output_file, options, defined_type)) {
    return false;
  }

  using GenFn = void (*)(CodeWriter & out, const AidlDefinedType& defined_type,
                         const AidlTypenames& typenames, const Options& options);
  // Wrap Generate* function to handle CodeWriter for a file.
  auto gen = [&](auto file, GenFn fn) {
    unique_ptr<CodeWriter> writer(io_delegate.GetCodeWriter(file));
    fn(*writer, defined_type, typenames, options);
    AIDL_FATAL_IF(!writer->Close(), defined_type) << "I/O Error!";
    return true;
  };

  return gen(options.OutputHeaderDir() + HeaderFile(defined_type, ClassNames::RAW),
             &GenerateHeader) &&
         gen(options.OutputHeaderDir() + HeaderFile(defined_type, ClassNames::CLIENT),
             &GenerateClientHeader) &&
         gen(options.OutputHeaderDir() + HeaderFile(defined_type, ClassNames::SERVER),
             &GenerateServerHeader) &&
         gen(output_file, &GenerateSource);
}

}  // namespace cpp
}  // namespace aidl
}  // namespace android

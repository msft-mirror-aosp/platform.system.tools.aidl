/*
 * Copyright (C) 2018, The Android Open Source Project
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

#include "aidl_to_java.h"
#include "aidl_language.h"
#include "aidl_typenames.h"
#include "logging.h"

#include <android-base/strings.h>

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace android {
namespace aidl {
namespace java {

using android::base::Join;

using std::function;
using std::map;
using std::string;
using std::vector;

std::string ConstantValueDecorator(const AidlTypeSpecifier& type, const std::string& raw_value) {
  if (type.IsArray() && !raw_value.empty() && raw_value[0] == '{') {
    return raw_value;
  }
  if (type.GetName() == "long") {
    return raw_value + "L";
  }
  if (auto defined_type = type.GetDefinedType(); defined_type) {
    auto enum_type = defined_type->AsEnumDeclaration();
    AIDL_FATAL_IF(!enum_type, type) << "Invalid type for \"" << raw_value << "\"";
    return type.GetName() + "." + raw_value.substr(raw_value.find_last_of('.') + 1);
  }
  return raw_value;
};

const string& JavaNameOf(const AidlTypeSpecifier& aidl, const AidlTypenames& typenames,
                         bool instantiable = false, bool boxing = false) {
  AIDL_FATAL_IF(!aidl.IsResolved(), aidl) << aidl.ToString();

  if (instantiable) {
    // An instantiable type is used in only out type(not even inout type),
    // And instantiable type has to be either the type in List, Map, ParcelFileDescriptor or
    // user-defined type.

    static map<string, string> instantiable_m = {
        {"List", "java.util.ArrayList"},
        {"Map", "java.util.HashMap"},
        {"ParcelFileDescriptor", "android.os.ParcelFileDescriptor"},
    };
    const string& aidl_name = aidl.GetName();

    if (instantiable_m.find(aidl_name) != instantiable_m.end()) {
      return instantiable_m[aidl_name];
    }
  }

  // map from AIDL built-in type name to the corresponding Java type name
  static map<string, string> m = {
      {"void", "void"},
      {"boolean", "boolean"},
      {"byte", "byte"},
      {"char", "char"},
      {"int", "int"},
      {"long", "long"},
      {"float", "float"},
      {"double", "double"},
      {"String", "java.lang.String"},
      {"List", "java.util.List"},
      {"Map", "java.util.Map"},
      {"IBinder", "android.os.IBinder"},
      {"FileDescriptor", "java.io.FileDescriptor"},
      {"CharSequence", "java.lang.CharSequence"},
      {"ParcelFileDescriptor", "android.os.ParcelFileDescriptor"},
      {"ParcelableHolder", "android.os.ParcelableHolder"},
  };

  // map from primitive types to the corresponding boxing types
  static map<string, string> boxing_types = {
      {"void", "Void"},   {"boolean", "Boolean"}, {"byte", "Byte"},   {"char", "Character"},
      {"int", "Integer"}, {"long", "Long"},       {"float", "Float"}, {"double", "Double"},
  };

  // Enums in Java are represented by their backing type when
  // referenced in parcelables, methods, etc.
  if (const AidlEnumDeclaration* enum_decl = typenames.GetEnumDeclaration(aidl);
      enum_decl != nullptr) {
    const string& backing_type_name = enum_decl->GetBackingType().GetName();
    AIDL_FATAL_IF(m.find(backing_type_name) == m.end(), enum_decl);
    AIDL_FATAL_IF(!AidlTypenames::IsBuiltinTypename(backing_type_name), enum_decl);
    if (boxing) {
      return boxing_types[backing_type_name];
    } else {
      return m[backing_type_name];
    }
  }

  const string& aidl_name = aidl.GetName();
  if (boxing && AidlTypenames::IsPrimitiveTypename(aidl_name)) {
    // Every primitive type must have the corresponding boxing type
    AIDL_FATAL_IF(boxing_types.find(aidl_name) == m.end(), aidl);
    return boxing_types[aidl_name];
  }
  if (m.find(aidl_name) != m.end()) {
    AIDL_FATAL_IF(!AidlTypenames::IsBuiltinTypename(aidl_name), aidl);
    return m[aidl_name];
  } else {
    // 'foo.bar.IFoo' in AIDL maps to 'foo.bar.IFoo' in Java
    return aidl_name;
  }
}

namespace {
string JavaSignatureOfInternal(
    const AidlTypeSpecifier& aidl, const AidlTypenames& typenames, bool instantiable,
    bool omit_array, bool boxing = false /* boxing can be true only if it is a type parameter */) {
  string ret = JavaNameOf(aidl, typenames, instantiable, boxing && !aidl.IsArray());
  if (aidl.IsGeneric()) {
    vector<string> arg_names;
    for (const auto& ta : aidl.GetTypeParameters()) {
      arg_names.emplace_back(
          JavaSignatureOfInternal(*ta, typenames, false, false, true /* boxing */));
    }
    ret += "<" + Join(arg_names, ",") + ">";
  }
  if (aidl.IsArray() && !omit_array) {
    ret += "[]";
  }
  return ret;
}

// Returns the name of the backing type for the specified type. Note: this
// returns type names as used in AIDL, not a Java signature.
// For enums, this is the enum's backing type.
// For all other types, this is the type itself.
string AidlBackingTypeName(const AidlTypeSpecifier& type, const AidlTypenames& typenames) {
  string type_name;
  if (const AidlEnumDeclaration* enum_decl = typenames.GetEnumDeclaration(type);
      enum_decl != nullptr) {
    type_name = enum_decl->GetBackingType().GetName();
  } else {
    type_name = type.GetName();
  }
  if (type.IsArray()) {
    type_name += "[]";
  }
  return type_name;
}

}  // namespace

string JavaSignatureOf(const AidlTypeSpecifier& aidl, const AidlTypenames& typenames) {
  return JavaSignatureOfInternal(aidl, typenames, false, false);
}

string InstantiableJavaSignatureOf(const AidlTypeSpecifier& aidl, const AidlTypenames& typenames) {
  return JavaSignatureOfInternal(aidl, typenames, true, true);
}

string DefaultJavaValueOf(const AidlTypeSpecifier& aidl, const AidlTypenames& typenames) {
  static map<string, string> m = {
      {"boolean", "false"}, {"byte", "0"},     {"char", R"('\u0000')"}, {"int", "0"},
      {"long", "0L"},       {"float", "0.0f"}, {"double", "0.0d"},
  };

  const string name = AidlBackingTypeName(aidl, typenames);
  AIDL_FATAL_IF(name == "void", aidl);

  if (!aidl.IsArray() && m.find(name) != m.end()) {
    AIDL_FATAL_IF(!AidlTypenames::IsBuiltinTypename(name), aidl);
    return m[name];
  } else {
    return "null";
  }
}

static string GetFlagFor(const CodeGeneratorContext& c) {
  if (c.is_return_value) {
    return "android.os.Parcelable.PARCELABLE_WRITE_RETURN_VALUE";
  } else {
    return "0";
  }
}

typedef void (*ParcelHelperGenerator)(CodeWriter&, const Options&);

static void GenerateTypedObjectHelper(CodeWriter& out, const Options&) {
  // Note that the name is inconsistent here because Parcel.java defines readTypedObject as if it
  // "creates" a new value from a parcel. "in-place" read function is not necessary because
  // user-defined parcelable defines its readFromParcel.
  out << R"(static private <T> T readTypedObject(
    android.os.Parcel parcel,
    android.os.Parcelable.Creator<T> c) {
  if (parcel.readInt() != 0) {
      return c.createFromParcel(parcel);
  } else {
      return null;
  }
}
static private <T extends android.os.Parcelable> void writeTypedObject(
    android.os.Parcel parcel, T value, int parcelableFlags) {
  if (value != null) {
    parcel.writeInt(1);
    value.writeToParcel(parcel, parcelableFlags);
  } else {
    parcel.writeInt(0);
  }
}
)";
}

void GenerateParcelHelpers(CodeWriter& out, const AidlDefinedType& defined_type,
                           const Options& options) {
  // root-level type contains all necessary helpers
  if (defined_type.GetParentType()) {
    return;
  }
  // visits method parameters and parcelable fields to collect types which
  // requires read/write/create helpers.
  struct Visitor : AidlVisitor {
    const Options& options;
    set<ParcelHelperGenerator> helpers;
    Visitor(const Options& options) : options(options) {}
    void Visit(const AidlTypeSpecifier& type) override {
      auto name = type.GetName();
      if (auto defined_type = type.GetDefinedType(); defined_type) {
        if (defined_type->AsParcelable() != nullptr && !type.IsArray()) {
          // TypedObjects are supported since 23. So we don't need helpers.
          if (options.GetMinSdkVersion() < 23u) {
            helpers.insert(&GenerateTypedObjectHelper);
          }
        }
      } else {
        // There's parcelable-like built-in types as well.
        if (name == "ParcelFileDescriptor" || name == "CharSequence") {
          if (!type.IsArray()) {
            // TypedObjects are supported since 23. So we don't need helpers.
            if (options.GetMinSdkVersion() < 23u) {
              helpers.insert(&GenerateTypedObjectHelper);
            }
          }
        }
      }
    }
  } v{options};

  VisitTopDown(v, defined_type);
  if (!v.helpers.empty()) {
    // Nested class (_Parcel) is used to contain static helper methods because some targets are
    // still using Java 7 which doesn't allow interfaces to have static methods.
    // Helpers shouldn't bother API checks, but in case where AIDL types are not marked `@hide`
    // explicitly marks the helper class as @hide.
    out << "/** @hide */\n";
    out << "static class _Parcel {\n";
    out.Indent();
    for (const auto& helper : v.helpers) {
      helper(out, options);
    }
    out.Dedent();
    out << "}\n";
  }
}

void WriteToParcelFor(const CodeGeneratorContext& c) {
  static map<string, function<void(const CodeGeneratorContext&)>> method_map{
      {"boolean",
       [](const CodeGeneratorContext& c) {
         if (c.min_sdk_version >= 29u) {
           c.writer << c.parcel << ".writeBoolean(" << c.var << ");\n";
         } else {
           c.writer << c.parcel << ".writeInt(((" << c.var << ")?(1):(0)));\n";
         }
       }},
      {"boolean[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeBooleanArray(" << c.var << ");\n";
       }},
      {"byte",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeByte(" << c.var << ");\n";
       }},
      {"byte[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeByteArray(" << c.var << ");\n";
       }},
      {"char",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeInt(((int)" << c.var << "));\n";
       }},
      {"char[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeCharArray(" << c.var << ");\n";
       }},
      {"int",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeInt(" << c.var << ");\n";
       }},
      {"int[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeIntArray(" << c.var << ");\n";
       }},
      {"long",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeLong(" << c.var << ");\n";
       }},
      {"long[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeLongArray(" << c.var << ");\n";
       }},
      {"float",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeFloat(" << c.var << ");\n";
       }},
      {"float[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeFloatArray(" << c.var << ");\n";
       }},
      {"double",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeDouble(" << c.var << ");\n";
       }},
      {"double[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeDoubleArray(" << c.var << ");\n";
       }},
      {"String",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeString(" << c.var << ");\n";
       }},
      {"String[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeStringArray(" << c.var << ");\n";
       }},
      {"List",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           AIDL_FATAL_IF(c.type.GetTypeParameters().size() != 1, c.type);
           const auto& element_type = *c.type.GetTypeParameters().at(0);
           const auto& element_type_name = element_type.GetName();
           if (element_type_name == "String") {
             c.writer << c.parcel << ".writeStringList(" << c.var << ");\n";
           } else if (element_type_name == "IBinder") {
             c.writer << c.parcel << ".writeBinderList(" << c.var << ");\n";
           } else if (c.typenames.IsParcelable(element_type_name)) {
             c.writer << c.parcel << ".writeTypedList(" << c.var << ");\n";
           } else if (c.typenames.GetInterface(element_type)) {
             c.writer << c.parcel << ".writeInterfaceList(" << c.var << ");\n";
           } else {
             AIDL_FATAL(c.type) << "write: NOT IMPLEMENTED for " << element_type_name;
           }
         } else {
           c.writer << c.parcel << ".writeList(" << c.var << ");\n";
         }
       }},
      {"Map",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           c.writer << "if (" << c.var << " == null) {\n";
           c.writer.Indent();
           c.writer << c.parcel << ".writeInt(-1);\n";
           c.writer.Dedent();
           c.writer << "} else {\n";
           c.writer.Indent();
           c.writer << c.parcel << ".writeInt(" << c.var << ".size());\n";
           c.writer << c.var << ".forEach((k, v) -> {\n";
           c.writer.Indent();
           c.writer << c.parcel << ".writeString(k);\n";

           CodeGeneratorContext value_context{
               c.writer,
               c.typenames,
               *c.type.GetTypeParameters()[1].get(),
               c.parcel,
               "v",
               c.min_sdk_version,
               c.is_return_value,
               c.is_classloader_created,
               c.filename,
           };
           WriteToParcelFor(value_context);
           c.writer.Dedent();
           c.writer << "});\n";

           c.writer.Dedent();
           c.writer << "}\n";
         } else {
           c.writer << c.parcel << ".writeMap(" << c.var << ");\n";
         }
       }},
      {"IBinder",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeStrongBinder(" << c.var << ");\n";
       }},
      {"IBinder[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeBinderArray(" << c.var << ");\n";
       }},
      {"FileDescriptor",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeRawFileDescriptor(" << c.var << ");\n";
       }},
      {"FileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeRawFileDescriptorArray(" << c.var << ");\n";
       }},
      {"ParcelFileDescriptor",
       [](const CodeGeneratorContext& c) {
         if (c.min_sdk_version >= 23u) {
           c.writer << c.parcel << ".writeTypedObject(" << c.var << ", " << GetFlagFor(c) << ");\n";
         } else {
           c.writer << "_Parcel.writeTypedObject(" << c.parcel << ", " << c.var << ", "
                    << GetFlagFor(c) << ");\n";
         }
       }},
      {"ParcelFileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeTypedArray(" << c.var << ", " << GetFlagFor(c) << ");\n";
       }},
      {"CharSequence",
       [](const CodeGeneratorContext& c) {
         // TextUtils.writeToParcel does not accept null. So, we need to handle
         // the case here.
         c.writer << "if (" << c.var << "!=null) {\n";
         c.writer.Indent();
         c.writer << c.parcel << ".writeInt(1);\n";
         c.writer << "android.text.TextUtils.writeToParcel(" << c.var << ", " << c.parcel << ", "
                  << GetFlagFor(c) << ");\n";
         c.writer.Dedent();
         c.writer << "}\n";
         c.writer << "else {\n";
         c.writer.Indent();
         c.writer << c.parcel << ".writeInt(0);\n";
         c.writer.Dedent();
         c.writer << "}\n";
       }},
      {"ParcelableHolder",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".writeTypedObject(" << c.var << ", 0);\n";
       }},
  };
  const string type_name = AidlBackingTypeName(c.type, c.typenames);
  const auto found = method_map.find(type_name);
  if (found != method_map.end()) {
    found->second(c);
  } else {
    const AidlDefinedType* t = c.typenames.TryGetDefinedType(c.type.GetName());
    AIDL_FATAL_IF(t == nullptr, c.type) << "Unknown type: " << c.type.GetName();
    if (t->AsInterface() != nullptr) {
      if (c.type.IsArray()) {
        c.writer << c.parcel << ".writeInterfaceArray(" << c.var << ");\n";
      } else {
        c.writer << c.parcel << ".writeStrongInterface(" << c.var << ");\n";
      }
    } else if (t->AsParcelable() != nullptr) {
      if (c.type.IsArray()) {
        c.writer << c.parcel << ".writeTypedArray(" << c.var << ", " << GetFlagFor(c) << ");\n";
      } else {
        if (c.min_sdk_version >= 23u) {
          c.writer << c.parcel << ".writeTypedObject(" << c.var << ", " << GetFlagFor(c) << ");\n";
        } else {
          c.writer << "_Parcel.writeTypedObject(" << c.parcel << ", " << c.var << ", "
                   << GetFlagFor(c) << ");\n";
        }
      }
    }
  }
}

// Ensures that a variable is initialized to refer to the classloader
// of the current object and returns the name of the variable.
static string EnsureAndGetClassloader(CodeGeneratorContext& c) {
  AIDL_FATAL_IF(c.is_classloader_created == nullptr, AIDL_LOCATION_HERE);
  if (!*(c.is_classloader_created)) {
    c.writer << "java.lang.ClassLoader cl = "
             << "(java.lang.ClassLoader)this.getClass().getClassLoader();\n";
    *(c.is_classloader_created) = true;
  }
  return "cl";
}

bool CreateFromParcelFor(const CodeGeneratorContext& c) {
  static map<string, function<void(const CodeGeneratorContext&)>> method_map{
      {"boolean",
       [](const CodeGeneratorContext& c) {
         if (c.min_sdk_version >= 29u) {
           c.writer << c.var << " = " << c.parcel << ".readBoolean();\n";
         } else {
           c.writer << c.var << " = (0!=" << c.parcel << ".readInt());\n";
         }
       }},
      {"boolean[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createBooleanArray();\n";
       }},
      {"byte",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readByte();\n";
       }},
      {"byte[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createByteArray();\n";
       }},
      {"char",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = (char)" << c.parcel << ".readInt();\n";
       }},
      {"char[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createCharArray();\n";
       }},
      {"int",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readInt();\n";
       }},
      {"int[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createIntArray();\n";
       }},
      {"long",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readLong();\n";
       }},
      {"long[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createLongArray();\n";
       }},
      {"float",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readFloat();\n";
       }},
      {"float[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createFloatArray();\n";
       }},
      {"double",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readDouble();\n";
       }},
      {"double[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createDoubleArray();\n";
       }},
      {"String",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readString();\n";
       }},
      {"String[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createStringArray();\n";
       }},
      {"List",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           AIDL_FATAL_IF(c.type.GetTypeParameters().size() != 1, c.type);
           const auto& element_type = *c.type.GetTypeParameters().at(0);
           const auto& element_type_name = element_type.GetName();
           if (element_type_name == "String") {
             c.writer << c.var << " = " << c.parcel << ".createStringArrayList();\n";
           } else if (element_type_name == "IBinder") {
             c.writer << c.var << " = " << c.parcel << ".createBinderArrayList();\n";
           } else if (c.typenames.IsParcelable(element_type_name)) {
             c.writer << c.var << " = " << c.parcel << ".createTypedArrayList("
                      << JavaNameOf(element_type, c.typenames) << ".CREATOR);\n";
           } else if (c.typenames.GetInterface(element_type)) {
             auto as_interface = element_type_name + ".Stub::asInterface";
             c.writer << c.var << " = " << c.parcel << ".createInterfaceArrayList(" << as_interface
                      << ");\n";
           } else {
             AIDL_FATAL(c.type) << "create: NOT IMPLEMENTED for " << element_type_name;
           }
         } else {
           const string classloader = EnsureAndGetClassloader(const_cast<CodeGeneratorContext&>(c));
           c.writer << c.var << " = " << c.parcel << ".readArrayList(" << classloader << ");\n";
         }
       }},
      {"Map",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           c.writer << "{\n";
           c.writer.Indent();
           c.writer << "int N = " << c.parcel << ".readInt();\n";
           c.writer << c.var << " = N < 0 ? null : new java.util.HashMap<>();\n";

           auto creator = JavaNameOf(*(c.type.GetTypeParameters().at(1)), c.typenames) + ".CREATOR";
           c.writer << "java.util.stream.IntStream.range(0, N).forEach(i -> {\n";
           c.writer.Indent();
           c.writer << "String k = " << c.parcel << ".readString();\n";
           c.writer << JavaSignatureOf(*(c.type.GetTypeParameters().at(1)), c.typenames) << " v;\n";
           CodeGeneratorContext value_context{
               c.writer,
               c.typenames,
               *c.type.GetTypeParameters()[1].get(),
               c.parcel,
               "v",
               c.min_sdk_version,
               c.is_return_value,
               c.is_classloader_created,
               c.filename,
           };
           CreateFromParcelFor(value_context);
           c.writer << c.var << ".put(k, v);\n";

           c.writer.Dedent();
           c.writer << "});\n";

           c.writer.Dedent();
           c.writer << "}\n";
         } else {
           const string classloader = EnsureAndGetClassloader(const_cast<CodeGeneratorContext&>(c));
           c.writer << c.var << " = " << c.parcel << ".readHashMap(" << classloader << ");\n";
         }
       }},
      {"IBinder",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readStrongBinder();\n";
       }},
      {"IBinder[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createBinderArray();\n";
       }},
      {"FileDescriptor",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".readRawFileDescriptor();\n";
       }},
      {"FileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel << ".createRawFileDescriptorArray();\n";
       }},
      {"ParcelFileDescriptor",
       [](const CodeGeneratorContext& c) {
         if (c.min_sdk_version >= 23u) {
           c.writer << c.var << " = " << c.parcel
                    << ".readTypedObject(android.os.ParcelFileDescriptor.CREATOR);\n";
         } else {
           c.writer << c.var << " = _Parcel.readTypedObject(" << c.parcel
                    << ", android.os.ParcelFileDescriptor.CREATOR);\n";
         }
       }},
      {"ParcelFileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.var << " = " << c.parcel
                  << ".createTypedArray(android.os.ParcelFileDescriptor.CREATOR);\n";
       }},
      {"CharSequence",
       [](const CodeGeneratorContext& c) {
         if (c.min_sdk_version >= 23u) {
           c.writer << c.var << " = " << c.parcel
                    << ".readTypedObject(android.text.TextUtils.CHAR_SEQUENCE_CREATOR);\n";
         } else {
           c.writer << c.var << " = _Parcel.readTypedObject(" << c.parcel
                    << ", android.text.TextUtils.CHAR_SEQUENCE_CREATOR);\n";
         }
       }},
      {"ParcelableHolder",
       [](const CodeGeneratorContext& c) {
         c.writer << "if ((0!=" << c.parcel << ".readInt())) {\n";
         c.writer.Indent();
         c.writer << c.var << ".readFromParcel(" << c.parcel << ");\n";
         c.writer.Dedent();
         c.writer << "}\n";
       }},
  };
  const auto found = method_map.find(AidlBackingTypeName(c.type, c.typenames));
  if (found != method_map.end()) {
    found->second(c);
  } else {
    const AidlDefinedType* t = c.typenames.TryGetDefinedType(c.type.GetName());
    AIDL_FATAL_IF(t == nullptr, c.type) << "Unknown type: " << c.type.GetName();
    if (t->AsInterface() != nullptr) {
      auto name = c.type.GetName();
      if (c.type.IsArray()) {
        auto new_array = name + "[]::new";
        auto as_interface = name + ".Stub::asInterface";
        c.writer << c.var << " = " << c.parcel << ".createInterfaceArray(" << new_array << ", "
                 << as_interface << ");\n";
      } else {
        c.writer << c.var << " = " << name << ".Stub.asInterface(" << c.parcel
                 << ".readStrongBinder());\n";
      }
    } else if (t->AsParcelable() != nullptr) {
      if (c.type.IsArray()) {
        c.writer << c.var << " = " << c.parcel << ".createTypedArray("
                 << JavaNameOf(c.type, c.typenames) << ".CREATOR);\n";
      } else {
        if (c.min_sdk_version >= 23u) {
          c.writer << c.var << " = " << c.parcel << ".readTypedObject(" << c.type.GetName()
                   << ".CREATOR);\n";
        } else {
          c.writer << c.var << " = _Parcel.readTypedObject(" << c.parcel << ", " << c.type.GetName()
                   << ".CREATOR);\n";
        }
      }
    }
  }
  return true;
}

bool ReadFromParcelFor(const CodeGeneratorContext& c) {
  static map<string, function<void(const CodeGeneratorContext&)>> method_map{
      {"boolean[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readBooleanArray(" << c.var << ");\n";
       }},
      {"byte[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readByteArray(" << c.var << ");\n";
       }},
      {"char[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readCharArray(" << c.var << ");\n";
       }},
      {"int[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readIntArray(" << c.var << ");\n";
       }},
      {"long[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readLongArray(" << c.var << ");\n";
       }},
      {"float[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readFloatArray(" << c.var << ");\n";
       }},
      {"double[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readDoubleArray(" << c.var << ");\n";
       }},
      {"String[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readStringArray(" << c.var << ");\n";
       }},
      {"List",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           AIDL_FATAL_IF(c.type.GetTypeParameters().size() != 1, c.type);
           const auto& element_type = *c.type.GetTypeParameters().at(0);
           const auto& element_type_name = element_type.GetName();
           if (element_type_name == "String") {
             c.writer << c.parcel << ".readStringList(" << c.var << ");\n";
           } else if (element_type_name == "IBinder") {
             c.writer << c.parcel << ".readBinderList(" << c.var << ");\n";
           } else if (c.typenames.IsParcelable(element_type_name)) {
             c.writer << c.parcel << ".readTypedList(" << c.var << ", "
                      << JavaNameOf(*(c.type.GetTypeParameters().at(0)), c.typenames)
                      << ".CREATOR);\n";
           } else if (c.typenames.GetInterface(element_type)) {
             auto as_interface = element_type_name + ".Stub::asInterface";
             c.writer << c.parcel << ".readInterfaceList(" << c.var << ", " << as_interface
                      << ");\n";
           } else {
             AIDL_FATAL(c.type) << "read: NOT IMPLEMENTED for " << element_type_name;
           }
         } else {
           const string classloader = EnsureAndGetClassloader(const_cast<CodeGeneratorContext&>(c));
           c.writer << c.parcel << ".readList(" << c.var << ", " << classloader << ");\n";
         }
       }},
      {"Map",
       [](const CodeGeneratorContext& c) {
         if (c.type.IsGeneric()) {
           c.writer << "if (" << c.var << " != null) " << c.var << ".clear();\n";
           c.writer << "java.util.stream.IntStream.range(0, " << c.parcel
                    << ".readInt()).forEach(i -> {\n";
           c.writer.Indent();
           c.writer << "String k = " << c.parcel << ".readString();\n";
           c.writer << JavaSignatureOf(*(c.type.GetTypeParameters().at(1)), c.typenames) << " v;\n";
           CodeGeneratorContext value_context{
               c.writer,
               c.typenames,
               *c.type.GetTypeParameters()[1].get(),
               c.parcel,
               "v",
               c.min_sdk_version,
               c.is_return_value,
               c.is_classloader_created,
               c.filename,
           };
           CreateFromParcelFor(value_context);
           c.writer << c.var << ".put(k, v);\n";

           c.writer.Dedent();
           c.writer << "});\n";
         } else {
           const string classloader = EnsureAndGetClassloader(const_cast<CodeGeneratorContext&>(c));
           c.writer << c.var << " = " << c.parcel << ".readHashMap(" << classloader << ");\n";
         }
       }},
      {"IBinder[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readBinderArray(" << c.var << ");\n";
       }},
      {"FileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readRawFileDescriptorArray(" << c.var << ");\n";
       }},
      {"ParcelFileDescriptor",
       [](const CodeGeneratorContext& c) {
         c.writer << "if ((0!=" << c.parcel << ".readInt())) {\n";
         c.writer.Indent();
         c.writer << c.var << " = " << "android.os.ParcelFileDescriptor.CREATOR.createFromParcel(" << c.parcel << ");\n";
         c.writer.Dedent();
         c.writer << "}\n";
       }},
      {"ParcelFileDescriptor[]",
       [](const CodeGeneratorContext& c) {
         c.writer << c.parcel << ".readTypedArray(" << c.var
                  << ", android.os.ParcelFileDescriptor.CREATOR);\n";
       }},
  };
  const auto& found = method_map.find(AidlBackingTypeName(c.type, c.typenames));
  if (found != method_map.end()) {
    found->second(c);
  } else {
    const AidlDefinedType* t = c.typenames.TryGetDefinedType(c.type.GetName());
    AIDL_FATAL_IF(t == nullptr, c.type) << "Unknown type: " << c.type.GetName();
    if (t->AsParcelable() != nullptr || t->AsUnionDeclaration() != nullptr) {
      if (c.type.IsArray()) {
        c.writer << c.parcel << ".readTypedArray(" << c.var << ", " << c.type.GetName()
                 << ".CREATOR);\n";
      } else {
        c.writer << "if ((0!=" << c.parcel << ".readInt())) {\n";
        c.writer.Indent();
        c.writer << c.var << ".readFromParcel(" << c.parcel << ");\n";
        c.writer.Dedent();
        c.writer << "}\n";
      }
    } else if (t->AsInterface()) {
      AIDL_FATAL_IF(!c.type.IsArray(), c.type) << "readFromParcel(interface) doesn't make sense.";
      auto as_interface = c.type.GetName() + ".Stub::asInterface";
      c.writer << c.parcel << ".readInterfaceArray(" << c.var << ", " << as_interface << ");\n";
    }
  }
  return true;
}

void ToStringFor(const CodeGeneratorContext& c) {
  if (c.type.IsArray()) {
    c.writer << "java.util.Arrays.toString(" << c.var << ")";
    return;
  }

  const std::string name = c.type.GetName();

  if (AidlTypenames::IsPrimitiveTypename(name)) {
    c.writer << c.var;
    return;
  }

  const AidlDefinedType* t = c.typenames.TryGetDefinedType(name);
  if (t != nullptr && t->AsEnumDeclaration()) {
    c.writer << c.var;
    return;
  }

  // FileDescriptor doesn't have a good toString() impl.
  if (name == "FileDescriptor") {
    c.writer << c.var << " == null ? \"null\" : ";
    c.writer << c.var << ".getInt$()";
    return;
  }

  // Rest of the built-in types have reasonable toString() impls.
  if (AidlTypenames::IsBuiltinTypename(name)) {
    c.writer << "java.util.Objects.toString(" << c.var << ")";
    return;
  }

  // For user-defined types, we also use toString() that we are generating here, but just make sure
  // that they are actually user-defined types.
  AIDL_FATAL_IF(t == nullptr, c.type) << "Unknown type";
  if (t->AsInterface() != nullptr || t->AsParcelable() != nullptr) {
    c.writer << "java.util.Objects.toString(" << c.var << ")";
    return;
  }

  AIDL_FATAL(AIDL_LOCATION_HERE) << "Unhandled typename: " << name;
}

}  // namespace java
}  // namespace aidl
}  // namespace android

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

#include <android-base/format.h>
#include <android-base/stringprintf.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "aidl_checkapi.h"
#include "aidl_language.h"
#include "aidl_to_cpp.h"
#include "aidl_to_java.h"
#include "logging.h"
#include "options.h"
#include "tests/fake_io_delegate.h"

using android::aidl::internals::parse_preprocessed_file;
using android::aidl::test::FakeIoDelegate;
using android::base::StringPrintf;
using std::map;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;
using testing::TestParamInfo;
using testing::internal::CaptureStderr;
using testing::internal::GetCapturedStderr;

namespace android {
namespace aidl {
namespace {

const char kExpectedDepFileContents[] =
R"(place/for/output/p/IFoo.java : \
  p/IFoo.aidl

p/IFoo.aidl :
)";

const char kExpectedNinjaDepFileContents[] =
R"(place/for/output/p/IFoo.java : \
  p/IFoo.aidl
)";

const char kExpectedParcelableDeclarationDepFileContents[] =
    R"( : \
  p/Foo.aidl

p/Foo.aidl :
)";

const char kExpectedStructuredParcelableDepFileContents[] =
    R"(place/for/output/p/Foo.java : \
  p/Foo.aidl

p/Foo.aidl :
)";

const char kExpectedJavaParcelableOutputContents[] =
    R"(/*
 * This file is auto-generated.  DO NOT MODIFY.
 */
@android.annotation.Hide
public class Rect implements android.os.Parcelable
{
  // Comment

  @android.annotation.Hide
  public int x = 5;

  @android.annotation.Hide
  @android.compat.annotation.UnsupportedAppUsage(expectedSignature = "dummy", implicitMember = "dummy", maxTargetSdk = 28, publicAlternatives = "dummy", trackingBug = 42L, overrideSourcePosition="Rect.aidl:7:1:10:14")
  public int y = 0;

  public android.os.ParcelFileDescriptor fd;

  public java.util.List<android.os.ParcelFileDescriptor> fds;
  public static final android.os.Parcelable.Creator<Rect> CREATOR = new android.os.Parcelable.Creator<Rect>() {
    @Override
    public Rect createFromParcel(android.os.Parcel _aidl_source) {
      Rect _aidl_out = new Rect();
      _aidl_out.readFromParcel(_aidl_source);
      return _aidl_out;
    }
    @Override
    public Rect[] newArray(int _aidl_size) {
      return new Rect[_aidl_size];
    }
  };
  @Override public final void writeToParcel(android.os.Parcel _aidl_parcel, int _aidl_flag)
  {
    int _aidl_start_pos = _aidl_parcel.dataPosition();
    _aidl_parcel.writeInt(0);
    _aidl_parcel.writeInt(x);
    _aidl_parcel.writeInt(y);
    if ((fd!=null)) {
      _aidl_parcel.writeInt(1);
      fd.writeToParcel(_aidl_parcel, 0);
    }
    else {
      _aidl_parcel.writeInt(0);
    }
    _aidl_parcel.writeTypedList(fds);
    int _aidl_end_pos = _aidl_parcel.dataPosition();
    _aidl_parcel.setDataPosition(_aidl_start_pos);
    _aidl_parcel.writeInt(_aidl_end_pos - _aidl_start_pos);
    _aidl_parcel.setDataPosition(_aidl_end_pos);
  }
  public final void readFromParcel(android.os.Parcel _aidl_parcel)
  {
    int _aidl_start_pos = _aidl_parcel.dataPosition();
    int _aidl_parcelable_size = _aidl_parcel.readInt();
    try {
      if (_aidl_parcelable_size < 0) return;
      x = _aidl_parcel.readInt();
      if (_aidl_parcel.dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) return;
      y = _aidl_parcel.readInt();
      if (_aidl_parcel.dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) return;
      if ((0!=_aidl_parcel.readInt())) {
        fd = android.os.ParcelFileDescriptor.CREATOR.createFromParcel(_aidl_parcel);
      }
      else {
        fd = null;
      }
      if (_aidl_parcel.dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) return;
      fds = _aidl_parcel.createTypedArrayList(android.os.ParcelFileDescriptor.CREATOR);
      if (_aidl_parcel.dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) return;
    } finally {
      if (_aidl_start_pos > (Integer.MAX_VALUE - _aidl_parcelable_size)) {
        throw new android.os.BadParcelableException("Overflow in the size of parcelable");
      }
      _aidl_parcel.setDataPosition(_aidl_start_pos + _aidl_parcelable_size);
    }
  }
  @Override
  public int describeContents() {
    int _mask = 0;
    _mask |= describeContents(fd);
    _mask |= describeContents(fds);
    return _mask;
  }
  private int describeContents(Object _v) {
    if (_v == null) return 0;
    if (_v instanceof java.util.Collection) {
      int _mask = 0;
      for (Object o : (java.util.Collection) _v) {
        _mask |= describeContents(o);
      }
      return _mask;
    }
    if (_v instanceof android.os.Parcelable) {
      return ((android.os.Parcelable) _v).describeContents();
    }
    return 0;
  }
}
)";

// clang-format off
const char kExpectedCppHeaderOutput[] =
    R"(#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <optional>
#include <utils/String16.h>
#include <utils/StrongPointer.h>
#include <vector>

class IFoo : public ::android::IInterface {
public:
  DECLARE_META_INTERFACE(Foo)
  virtual ::android::binder::Status foo(::std::optional<::std::vector<::std::optional<::android::String16>>>* _aidl_return) = 0;
};  // class IFoo
class IFooDefault : public IFoo {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status foo(::std::optional<::std::vector<::std::optional<::android::String16>>>*) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IFooDefault
)";
// clang-format on

}  // namespace

class AidlTest : public ::testing::TestWithParam<Options::Language> {
 protected:
  AidlDefinedType* Parse(const string& path, const string& contents, AidlTypenames& typenames_,
                         Options::Language lang, AidlError* error = nullptr,
                         const vector<string> additional_arguments = {}) {
    io_delegate_.SetFileContents(path, contents);
    vector<string> args;
    args.emplace_back("aidl");
    args.emplace_back("--lang=" + Options::LanguageToString(lang));
    for (const string& s : additional_arguments) {
      args.emplace_back(s);
    }
    for (const string& f : preprocessed_files_) {
      args.emplace_back("--preprocessed=" + f);
    }
    for (const string& i : import_paths_) {
      args.emplace_back("--include=" + i);
    }
    args.emplace_back(path);
    Options options = Options::From(args);
    vector<string> imported_files;
    ImportResolver import_resolver{io_delegate_, path, import_paths_, {}};
    AidlError actual_error = ::android::aidl::internals::load_and_validate_aidl(
        path, options, io_delegate_, &typenames_, &imported_files);

    if (error != nullptr) {
      *error = actual_error;
    }

    if (actual_error != AidlError::OK) {
      return nullptr;
    }

    const auto& defined_types = typenames_.MainDocument().DefinedTypes();
    EXPECT_EQ(1ul, defined_types.size());

    return defined_types.front().get();
  }

  Options::Language GetLanguage() { return GetParam(); }

  FakeIoDelegate io_delegate_;
  vector<string> preprocessed_files_;
  set<string> import_paths_;
  AidlTypenames typenames_;
};

// Instantiate the AidlTest parameterized suite, calling all of the TEST_P
// tests with each of the supported languages as a parameter.
INSTANTIATE_TEST_SUITE_P(AidlTestSuite, AidlTest,
                         testing::Values(Options::Language::CPP, Options::Language::JAVA,
                                         Options::Language::NDK, Options::Language::RUST),
                         [](const testing::TestParamInfo<Options::Language>& info) {
                           return Options::LanguageToString(info.param);
                         });

TEST_P(AidlTest, AcceptMissingPackage) {
  EXPECT_NE(nullptr, Parse("IFoo.aidl", "interface IFoo { }", typenames_, GetLanguage()));
}

TEST_P(AidlTest, EndsInSingleLineComment) {
  EXPECT_NE(nullptr, Parse("IFoo.aidl", "interface IFoo { } // foo", typenames_, GetLanguage()));
}

TEST_P(AidlTest, InterfaceRequiresCorrectPath) {
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.11-21: IBar should be declared in a file called a/IBar.aidl\n";
  const std::string file_contents = "package a; interface IBar {}";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", file_contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr()) << file_contents;
}

TEST_P(AidlTest, ParcelableRequiresCorrectPath) {
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.11-21: Bar should be declared in a file called a/Bar.aidl\n";
  const std::string file_contents = "package a; interface Bar {}";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", file_contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr()) << file_contents;
}

TEST_P(AidlTest, UnstructuredParcelableRequiresCorrectPath) {
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.22-26: Bar should be declared in a file called a/Bar.aidl\n";
  const std::string file_contents = "package a; parcelable Bar cpp_header \"anything.h\";";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", file_contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr()) << file_contents;
}

TEST_P(AidlTest, EnumRequiresCorrectPath) {
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.16-20: Bar should be declared in a file called a/Bar.aidl\n";
  const std::string file_contents = "package a; enum Bar { A, }";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", file_contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr()) << file_contents;
}

TEST_P(AidlTest, RejectsArraysOfBinders) {
  import_paths_.emplace("");
  io_delegate_.SetFileContents("bar/IBar.aidl",
                               "package bar; interface IBar {}");
  const string path = "foo/IFoo.aidl";
  const string contents =
      "package foo;\n"
      "import bar.IBar;\n"
      "interface IFoo { void f(in IBar[] input); }";
  const string expected_stderr = "ERROR: foo/IFoo.aidl:3.27-32: Binder type cannot be an array\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse(path, contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, SupportOnlyOutParameters) {
  const string interface_list = "package a; interface IBar { void f(out List<String> bar); }";
  EXPECT_NE(nullptr, Parse("a/IBar.aidl", interface_list, typenames_, GetLanguage()));
}

TEST_P(AidlTest, RejectOutParametersForIBinder) {
  const string interface_ibinder = "package a; interface IBaz { void f(out IBinder bar); }";
  const string expected_ibinder_stderr =
      "ERROR: a/IBaz.aidl:1.47-51: 'bar' can't be an out parameter because IBinder can only be an "
      "in parameter.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IBaz.aidl", interface_ibinder, typenames_, GetLanguage()));
  EXPECT_EQ(expected_ibinder_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsOutParametersInOnewayInterface) {
  const string oneway_interface = "package a; oneway interface IBar { void f(out int bar); }";
  const string expected_stderr =
      "ERROR: a/IBar.aidl:1.40-42: oneway method 'f' cannot have out parameters\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IBar.aidl", oneway_interface, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsOutParametersInOnewayMethod) {
  const string oneway_method = "package a; interface IBar { oneway void f(out int bar); }";
  const string expected_stderr =
      "ERROR: a/IBar.aidl:1.40-42: oneway method 'f' cannot have out parameters\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IBar.aidl", oneway_method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsOnewayNonVoidReturn) {
  const string oneway_method = "package a; interface IFoo { oneway int f(); }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.39-41: oneway method 'f' cannot return a value\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsNullablePrimitive) {
  const string oneway_method = "package a; interface IFoo { @nullable int f(); }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.38-42: Primitive type cannot get nullable annotation\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, AcceptNullableList) {
  const string oneway_method = "package a; interface IFoo { @nullable List<String> f(); }";
  const string expected_stderr = "";
  CaptureStderr();
  EXPECT_NE(nullptr, Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsDuplicatedArgumentNames) {
  const string method = "package a; interface IFoo { void f(int a, int a); }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.33-35: method 'f' has duplicate argument name 'a'\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsDuplicatedFieldNames) {
  const string method = "package a; parcelable Foo { int a; String a; }";
  const string expected_stderr = "ERROR: a/Foo.aidl:1.42-44: 'Foo' has duplicate field name 'a'\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsRepeatedAnnotations) {
  const string method = R"(@Hide @Hide parcelable Foo {})";
  const string expected_stderr =
      "ERROR: Foo.aidl:1.23-27: 'Hide' is repeated, but not allowed. Previous location: "
      "Foo.aidl:1.1-6\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("Foo.aidl", method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsDuplicatedAnnotationParams) {
  const string method = "package a; interface IFoo { @UnsupportedAppUsage(foo=1, foo=2)void f(); }";
  const string expected_stderr = "ERROR: a/IFoo.aidl:1.56-62: Trying to redefine parameter foo.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectUnsupportedInterfaceAnnotations) {
  AidlError error;
  const string method = "package a; @nullable interface IFoo { int f(); }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.21-31: 'nullable' is not a supported annotation for this node. "
      "It must be one of: Hide, UnsupportedAppUsage, VintfStability, SensitiveData, "
      "JavaPassthrough, Descriptor\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, RejectUnsupportedTypeAnnotations) {
  AidlError error;
  const string method = "package a; interface IFoo { @JavaOnlyStableParcelable int f(); }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.54-58: 'JavaOnlyStableParcelable' is not a supported annotation "
      "for this node. It must be one of: Hide, UnsupportedAppUsage, nullable, utf8InCpp, JavaPassthrough\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, RejectUnsupportedParcelableAnnotations) {
  AidlError error;
  const string method = "package a; @nullable parcelable IFoo cpp_header \"IFoo.h\";";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.32-37: 'nullable' is not a supported annotation for this node. "
      "It must be one of: Hide, JavaOnlyStableParcelable, UnsupportedAppUsage, VintfStability, "
      "JavaPassthrough, JavaOnlyImmutable\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, RejectUnsupportedParcelableDefineAnnotations) {
  AidlError error;
  const string method = "package a; @nullable parcelable IFoo { String a; String b; }";
  const string expected_stderr =
      "ERROR: a/IFoo.aidl:1.32-37: 'nullable' is not a supported annotation for this node. "
      "It must be one of: Hide, UnsupportedAppUsage, VintfStability, JavaPassthrough, JavaDerive, "
      "JavaOnlyImmutable, FixedSize, RustDerive\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", method, typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, ParsesNonNullableAnnotation) {
  auto parse_result =
      Parse("a/IFoo.aidl", "package a; interface IFoo { String f(); }", typenames_, GetLanguage());
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_FALSE(interface->GetMethods().empty());
  EXPECT_FALSE(interface->GetMethods()[0]->GetType().IsNullable());
}

TEST_P(AidlTest, ParsesNullableAnnotation) {
  auto parse_result = Parse("a/IFoo.aidl", "package a; interface IFoo { @nullable String f(); }",
                            typenames_, GetLanguage());
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_FALSE(interface->GetMethods().empty());
  EXPECT_TRUE(interface->GetMethods()[0]->GetType().IsNullable());
}

TEST_P(AidlTest, ParsesNonUtf8Annotations) {
  auto parse_result =
      Parse("a/IFoo.aidl", "package a; interface IFoo { String f(); }", typenames_, GetLanguage());
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_FALSE(interface->GetMethods().empty());
  EXPECT_FALSE(interface->GetMethods()[0]->GetType().IsUtf8InCpp());
}

TEST_P(AidlTest, ParsesUtf8Annotations) {
  auto parse_result = Parse("a/IFoo.aidl", "package a; interface IFoo { @utf8InCpp String f(); }",
                            typenames_, GetLanguage());
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_FALSE(interface->GetMethods().empty());
  EXPECT_TRUE(interface->GetMethods()[0]->GetType().IsUtf8InCpp());
}

TEST_P(AidlTest, VintfRequiresStructuredAndStability) {
  AidlError error;
  const string expected_stderr =
      "ERROR: IFoo.aidl:1.16-26: Must compile @VintfStability type w/ aidl_interface 'stability: "
      "\"vintf\"'\n"
      "ERROR: IFoo.aidl:1.16-26: Must compile @VintfStability type w/ aidl_interface "
      "--structured\n";
  CaptureStderr();
  ASSERT_EQ(nullptr, Parse("IFoo.aidl", "@VintfStability interface IFoo {}", typenames_,
                           GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  ASSERT_EQ(AidlError::NOT_STRUCTURED, error);
}

TEST_P(AidlTest, VintfRequiresStructured) {
  AidlError error;
  const string expected_stderr =
      "ERROR: IFoo.aidl:1.16-26: Must compile @VintfStability type w/ aidl_interface "
      "--structured\n";
  CaptureStderr();
  ASSERT_EQ(nullptr, Parse("IFoo.aidl", "@VintfStability interface IFoo {}", typenames_,
                           GetLanguage(), &error, {"--stability", "vintf"}));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  ASSERT_EQ(AidlError::NOT_STRUCTURED, error);
}

TEST_P(AidlTest, VintfRequiresSpecifiedStability) {
  AidlError error;
  const string expected_stderr =
      "ERROR: IFoo.aidl:1.16-26: Must compile @VintfStability type w/ aidl_interface 'stability: "
      "\"vintf\"'\n";
  CaptureStderr();
  ASSERT_EQ(nullptr, Parse("IFoo.aidl", "@VintfStability interface IFoo {}", typenames_,
                           GetLanguage(), &error, {"--structured"}));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  ASSERT_EQ(AidlError::NOT_STRUCTURED, error);
}

TEST_P(AidlTest, ParsesStabilityAnnotations) {
  AidlError error;
  auto parse_result = Parse("IFoo.aidl", "@VintfStability interface IFoo {}", typenames_,
                            GetLanguage(), &error, {"--structured", "--stability", "vintf"});
  ASSERT_EQ(AidlError::OK, error);
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_TRUE(interface->IsVintfStability());
}

TEST_F(AidlTest, ParsesJavaOnlyStableParcelable) {
  Options java_options = Options::From("aidl -o out --structured a/Foo.aidl");
  Options cpp_options = Options::From("aidl --lang=cpp -o out -h out/include a/Foo.aidl");
  Options cpp_structured_options =
      Options::From("aidl --lang=cpp --structured -o out -h out/include a/Foo.aidl");
  Options rust_options = Options::From("aidl --lang=rust -o out --structured a/Foo.aidl");
  io_delegate_.SetFileContents(
      "a/Foo.aidl",
      StringPrintf("package a; @JavaOnlyStableParcelable parcelable Foo cpp_header \"Foo.h\" ;"));

  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cpp_options, io_delegate_));
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.48-52: Cannot declared parcelable in a --structured interface. "
      "Parcelable must be defined in AIDL directly.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(cpp_structured_options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(rust_options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ParcelableSupportJavaDeriveToString) {
  io_delegate_.SetFileContents("a/Foo.aidl", R"(package a;
    @JavaDerive(toString=true) parcelable Foo { int a; float b; })");
  Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));

  string java_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &java_out));
  EXPECT_THAT(java_out, testing::HasSubstr("public String toString() {"));

  // Other backends shouldn't be bothered
  Options cpp_options = Options::From("aidl --lang=cpp -o out -h out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cpp_options, io_delegate_));

  Options ndk_options = Options::From("aidl --lang=ndk -o out -h out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(ndk_options, io_delegate_));
}

TEST_F(AidlTest, UnionSupportJavaDeriveToString) {
  io_delegate_.SetFileContents("a/Foo.aidl", R"(package a;
    @JavaDerive(toString=true) union Foo { int a; int[] b; })");
  CaptureStderr();
  Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());

  const string expected_to_string_method = R"--(
  @Override
  public String toString() {
    switch (_tag) {
    case a: return "a.Foo.a(" + (getA()) + ")";
    case b: return "a.Foo.b(" + (java.util.Arrays.toString(getB())) + ")";
    }
    throw new IllegalStateException("unknown field: " + _tag);
  }
)--";

  string java_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &java_out));
  EXPECT_THAT(java_out, testing::HasSubstr(expected_to_string_method));
}

TEST_F(AidlTest, ParcelableSupportJavaDeriveEquals) {
  io_delegate_.SetFileContents("a/Foo.aidl", R"(package a;
    @JavaDerive(equals=true) parcelable Foo { int a; float b; })");
  CaptureStderr();
  Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());

  const std::string expected = R"--(
  @Override
  public boolean equals(Object other) {
    if (this == other) return true;
    if (other == null) return false;
    if (!(other instanceof Foo)) return false;
    Foo that = (Foo)other;
    if (!java.util.Objects.deepEquals(a, that.a)) return false;
    if (!java.util.Objects.deepEquals(b, that.b)) return false;
    return true;
  }

  @Override
  public int hashCode() {
    return java.util.Arrays.deepHashCode(java.util.Arrays.asList(a, b).toArray());
  }
)--";

  string java_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &java_out));
  EXPECT_THAT(java_out, testing::HasSubstr(expected));
}

TEST_F(AidlTest, UnionSupportJavaDeriveEquals) {
  io_delegate_.SetFileContents("a/Foo.aidl", R"(package a;
    @JavaDerive(equals=true) union Foo { int a; int[] b; })");
  CaptureStderr();
  Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());

  const std::string expected = R"--(
  @Override
  public boolean equals(Object other) {
    if (this == other) return true;
    if (other == null) return false;
    if (!(other instanceof Foo)) return false;
    Foo that = (Foo)other;
    if (_tag != that._tag) return false;
    if (!java.util.Objects.deepEquals(_value, that._value)) return false;
    return true;
  }

  @Override
  public int hashCode() {
    return java.util.Arrays.deepHashCode(java.util.Arrays.asList(_tag, _value).toArray());
  }
)--";

  string java_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &java_out));
  EXPECT_THAT(java_out, testing::HasSubstr(expected));
}

TEST_F(AidlTest, RejectsJavaDeriveAnnotation) {
  {
    io_delegate_.SetFileContents("a/Foo.aidl",
                                 "package a; @JavaDerive(blah=true) parcelable Foo{}");
    Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
    const std::string expected_stderr =
        "ERROR: a/Foo.aidl:1.11-34: Parameter blah not supported for annotation JavaDerive.";
    EXPECT_THAT(GetCapturedStderr(), testing::HasSubstr(expected_stderr));
  }

  {
    io_delegate_.SetFileContents("a/IFoo.aidl", "package a; @JavaDerive interface IFoo{}");
    Options java_options = Options::From("aidl --lang=java -o out a/IFoo.aidl");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
    const std::string expected_stderr =
        "ERROR: a/IFoo.aidl:1.23-33: 'JavaDerive' is not a supported annotation for this node. "
        "It must be one of: Hide, UnsupportedAppUsage, VintfStability, SensitiveData, "
        "JavaPassthrough, Descriptor\n";
    EXPECT_EQ(expected_stderr, GetCapturedStderr());
  }

  {
    io_delegate_.SetFileContents("a/IFoo.aidl", "package a; @JavaDerive enum IFoo { A=1, }");
    Options java_options = Options::From("aidl --lang=java -o out a/IFoo.aidl");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
    const std::string expected_stderr =
        "ERROR: a/IFoo.aidl:1.28-33: 'JavaDerive' is not a supported annotation for this node. "
        "It must be one of: Backing, Hide, VintfStability, JavaPassthrough\n";
    EXPECT_EQ(expected_stderr, GetCapturedStderr());
  }
}

TEST_P(AidlTest, ParseDescriptorAnnotation) {
  AidlError error;
  auto parse_result = Parse("IFoo.aidl", R"(@Descriptor(value="IBar") interface IFoo{})",
                            typenames_, GetLanguage(), &error, {"--structured"});
  ASSERT_EQ(AidlError::OK, error);
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  ASSERT_EQ("IBar", interface->GetDescriptor());
}

TEST_P(AidlTest, AcceptsOnewayMethod) {
  const string oneway_method = "package a; interface IFoo { oneway void f(int a); }";
  EXPECT_NE(nullptr, Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage()));
}

TEST_P(AidlTest, AcceptsOnewayInterface) {
  const string oneway_interface = "package a; oneway interface IBar { void f(int a); }";
  EXPECT_NE(nullptr, Parse("a/IBar.aidl", oneway_interface, typenames_, GetLanguage()));
}

TEST_P(AidlTest, AcceptsAnnotatedOnewayMethod) {
  const string oneway_method =
      "package a; interface IFoo { @UnsupportedAppUsage oneway void f(int a); }";
  EXPECT_NE(nullptr, Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage()));
}

TEST_P(AidlTest, AnnotationsInMultiplePlaces) {
  const string oneway_method =
      "package a; interface IFoo { @UnsupportedAppUsage oneway @Hide void f(int a); }";
  const AidlDefinedType* defined = Parse("a/IFoo.aidl", oneway_method, typenames_, GetLanguage());
  ASSERT_NE(nullptr, defined);
  const AidlInterface* iface = defined->AsInterface();
  ASSERT_NE(nullptr, iface);

  const auto& methods = iface->GetMethods();
  ASSERT_EQ(1u, methods.size());
  const auto& method = methods[0];
  const AidlTypeSpecifier& ret_type = method->GetType();

  // TODO(b/151102494): these annotations should be on the method
  ASSERT_NE(nullptr, ret_type.UnsupportedAppUsage());
  ASSERT_TRUE(ret_type.IsHide());
}

TEST_P(AidlTest, WritesComments) {
  string foo_interface =
      "package a; /* foo */ interface IFoo {"
      "  /* i */ int i();"
      "  /* j */ @nullable String j();"
      "  /* k */ @UnsupportedAppUsage oneway void k(int a); }";

  auto parse_result = Parse("a/IFoo.aidl", foo_interface, typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);
  EXPECT_EQ("/* foo */", parse_result->GetComments());

  const AidlInterface* interface = parse_result->AsInterface();
  EXPECT_EQ("/* i */", interface->GetMethods()[0]->GetComments());
  EXPECT_EQ("/* j */", interface->GetMethods()[1]->GetComments());
  EXPECT_EQ("/* k */", interface->GetMethods()[2]->GetComments());
}

TEST_P(AidlTest, CppHeaderCanBeIdentifierAsWell) {
  io_delegate_.SetFileContents("p/cpp_header.aidl",
                               R"(package p;
         parcelable cpp_header cpp_header "bar/header";)");
  import_paths_.emplace("");
  const string input_path = "p/IFoo.aidl";
  const string input = R"(package p;
                          import p.cpp_header;
                          interface IFoo {
                            // get bar
                            cpp_header get();
                          })";

  auto parse_result = Parse(input_path, input, typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  EXPECT_EQ("// get bar\n", interface->GetMethods()[0]->GetComments());
}

TEST_F(AidlTest, ParsesPreprocessedFile) {
  string simple_content = "parcelable a.Foo;\ninterface b.IBar;";
  io_delegate_.SetFileContents("path", simple_content);
  EXPECT_FALSE(typenames_.ResolveTypename("a.Foo").is_resolved);
  EXPECT_TRUE(parse_preprocessed_file(io_delegate_, "path", &typenames_));
  EXPECT_TRUE(typenames_.ResolveTypename("a.Foo").is_resolved);
  EXPECT_TRUE(typenames_.ResolveTypename("b.IBar").is_resolved);
}

TEST_F(AidlTest, ParsesPreprocessedFileWithWhitespace) {
  string simple_content = "parcelable    a.Foo;\n  interface b.IBar  ;\t";
  io_delegate_.SetFileContents("path", simple_content);

  EXPECT_FALSE(typenames_.ResolveTypename("a.Foo").is_resolved);
  EXPECT_TRUE(parse_preprocessed_file(io_delegate_, "path", &typenames_));
  EXPECT_TRUE(typenames_.ResolveTypename("a.Foo").is_resolved);
  EXPECT_TRUE(typenames_.ResolveTypename("b.IBar").is_resolved);
}

TEST_P(AidlTest, PreferImportToPreprocessed) {
  io_delegate_.SetFileContents("preprocessed", "interface another.IBar;");
  io_delegate_.SetFileContents("one/IBar.aidl", "package one; "
                                                "interface IBar {}");
  preprocessed_files_.push_back("preprocessed");
  import_paths_.emplace("");
  auto parse_result = Parse("p/IFoo.aidl", "package p; import one.IBar; interface IFoo {}",
                            typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);

  // We expect to know about both kinds of IBar
  EXPECT_TRUE(typenames_.ResolveTypename("one.IBar").is_resolved);
  EXPECT_TRUE(typenames_.ResolveTypename("another.IBar").is_resolved);
  // But if we request just "IBar" we should get our imported one.
  AidlTypeSpecifier ambiguous_type(AIDL_LOCATION_HERE, "IBar", false, nullptr, "");
  ambiguous_type.Resolve(typenames_);
  EXPECT_EQ("one.IBar", ambiguous_type.GetName());
}

// Special case of PreferImportToPreprocessed. Imported type should be preferred
// even when the preprocessed file already has the same type.
TEST_P(AidlTest, B147918827) {
  io_delegate_.SetFileContents("preprocessed", "interface another.IBar;\ninterface one.IBar;");
  io_delegate_.SetFileContents("one/IBar.aidl",
                               "package one; "
                               "interface IBar {}");
  preprocessed_files_.push_back("preprocessed");
  import_paths_.emplace("");
  auto parse_result = Parse("p/IFoo.aidl", "package p; import one.IBar; interface IFoo {}",
                            typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);

  // We expect to know about both kinds of IBar
  EXPECT_TRUE(typenames_.ResolveTypename("one.IBar").is_resolved);
  EXPECT_TRUE(typenames_.ResolveTypename("another.IBar").is_resolved);
  // But if we request just "IBar" we should get our imported one.
  AidlTypeSpecifier ambiguous_type(AIDL_LOCATION_HERE, "IBar", false, nullptr, "");
  ambiguous_type.Resolve(typenames_);
  EXPECT_EQ("one.IBar", ambiguous_type.GetName());
}

TEST_F(AidlTest, WritePreprocessedFile) {
  io_delegate_.SetFileContents("p/Outer.aidl",
                               "package p; parcelable Outer.Inner;");
  io_delegate_.SetFileContents("one/IBar.aidl", "package one; import p.Outer;"
                                                "interface IBar {}");

  vector<string> args {
    "aidl",
    "--preprocess",
    "preprocessed",
    "p/Outer.aidl",
    "one/IBar.aidl"};
  Options options = Options::From(args);
  EXPECT_TRUE(::android::aidl::preprocess_aidl(options, io_delegate_));

  string output;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("preprocessed", &output));
  EXPECT_EQ("parcelable p.Outer.Inner;\ninterface one.IBar;\n", output);
}

TEST_F(AidlTest, JavaParcelableOutput) {
  io_delegate_.SetFileContents(
      "Rect.aidl",
      "@Hide\n"
      "parcelable Rect {\n"
      "  // Comment\n"
      "  @Hide\n"
      "  int x=5;\n"
      "  @Hide\n"
      "  @UnsupportedAppUsage(maxTargetSdk = 28, trackingBug = 42, implicitMember = \"dummy\", "
      "expectedSignature = \"dummy\", publicAlternatives = \"d\" \n + \"u\" + \n \"m\" \n + \"m\" "
      "+ \"y\")\n"
      "  int y;\n"
      "  ParcelFileDescriptor fd;\n"
      "  List<ParcelFileDescriptor> fds;\n"
      "}");

  vector<string> args{"aidl", "Rect.aidl"};
  Options options = Options::From(args);
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  string output;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("Rect.java", &output));
  EXPECT_EQ(kExpectedJavaParcelableOutputContents, output);
}

TEST_F(AidlTest, CppHeaderIncludes) {
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  @nullable List<String> foo();\n"
                               "}");

  vector<string> args{"aidl", "--lang=cpp", "IFoo.aidl"};
  Options options = Options::From(args);
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  std::vector<std::string> empty;
  EXPECT_EQ(empty, io_delegate_.ListFiles(""));
  EXPECT_EQ(empty, io_delegate_.ListFiles("/"));
  EXPECT_EQ(empty, io_delegate_.ListFiles("out"));

  // Make sure the optional and String16.h includes are added
  string output;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("IFoo.h", &output));
  EXPECT_EQ(kExpectedCppHeaderOutput, output);
}

TEST_P(AidlTest, RequireOuterClass) {
  const string expected_stderr = "ERROR: p/IFoo.aidl:1.54-60: Failed to resolve 'Inner'\n";
  io_delegate_.SetFileContents("p/Outer.aidl",
                               "package p; parcelable Outer.Inner;");
  import_paths_.emplace("");
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           "package p; import p.Outer; interface IFoo { void f(in Inner c); }",
                           typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, ParseCompoundParcelableFromPreprocess) {
  io_delegate_.SetFileContents("preprocessed",
                               "parcelable p.Outer.Inner;");
  preprocessed_files_.push_back("preprocessed");
  auto parse_result = Parse("p/IFoo.aidl", "package p; interface IFoo { void f(in Inner c); }",
                            typenames_, GetLanguage());
  // TODO(wiley): This should actually return nullptr because we require
  //              the outer class name.  However, for legacy reasons,
  //              this behavior must be maintained.  b/17415692
  EXPECT_NE(nullptr, parse_result);
}

TEST_F(AidlTest, FailOnParcelable) {
  const string expected_foo_stderr =
      "ERROR: p/IFoo.aidl:1.22-27: Refusing to generate code with unstructured parcelables. "
      "Declared parcelables should be in their own file and/or cannot be used with --structured "
      "interfaces.\n";
  io_delegate_.SetFileContents("p/IFoo.aidl", "package p; parcelable IFoo;");

  // By default, we shouldn't fail on parcelable.
  Options options1 = Options::From("aidl p/IFoo.aidl");
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options1, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());

  // -b considers this an error
  Options options2 = Options::From("aidl -b p/IFoo.aidl");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options2, io_delegate_));
  EXPECT_EQ(expected_foo_stderr, GetCapturedStderr());

  const string expected_bar_stderr =
      "ERROR: p/IBar.aidl:1.22-26: Refusing to generate code with unstructured parcelables. "
      "Declared parcelables should be in their own file and/or cannot be used with --structured "
      "interfaces.\n";
  io_delegate_.SetFileContents("p/IBar.aidl", "package p; parcelable Foo; interface IBar{}");

  // With '-b' option, a parcelable and an interface should fail.
  Options options3 = Options::From("aidl p/IBar.aidl");
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options3, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
  Options options4 = Options::From("aidl -b p/IBar.aidl");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options4, io_delegate_));
  EXPECT_EQ(expected_bar_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, StructuredFailOnUnstructuredParcelable) {
  const string expected_stderr =
      "ERROR: ./o/WhoKnowsWhat.aidl:1.22-35: o.WhoKnowsWhat is not structured, but this is a "
      "structured interface.\n";
  io_delegate_.SetFileContents("o/WhoKnowsWhat.aidl", "package o; parcelable WhoKnowsWhat;");
  import_paths_.emplace("");
  AidlError error;
  CaptureStderr();
  EXPECT_EQ(
      nullptr,
      Parse("p/IFoo.aidl",
            "package p; import o.WhoKnowsWhat; interface IFoo { void f(in WhoKnowsWhat thisIs); }",
            typenames_, GetLanguage(), &error, {"--structured"}));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::NOT_STRUCTURED, error);
}

TEST_P(AidlTest, FailOnDuplicateConstantNames) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:4.34-45: Found duplicate constant name 'DUPLICATED'\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                      interface IFoo {
                        const String DUPLICATED = "d";
                        const int DUPLICATED = 1;
                      }
                   )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, FailOnTooBigConstant) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.48-52: Invalid type specifier for an int32 literal: byte\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                      interface IFoo {
                        const byte type2small = 256;
                      }
                   )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_F(AidlTest, BoolConstantsEvaluatesToIntegers) {
  io_delegate_.SetFileContents("a/Foo.aidl", "package a; parcelable Foo { const int y = true; }");
  CaptureStderr();
  auto options = Options::From("aidl --lang java -o out a/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &code));
  EXPECT_THAT(code, testing::HasSubstr("public static final int y = 1;"));
}

TEST_P(AidlTest, FailOnManyDefinedTypes) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.33-38: You must declare only one type per file.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                      interface IFoo {}
                      parcelable IBar {}
                      parcelable StructuredParcelable {}
                      interface IBaz {}
                  )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  // Parse success is important for clear error handling even if the cases aren't
  // actually supported in code generation.
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, FailOnNoDefinedTypes) {
  AidlError error;
  const string expected_stderr = "ERROR: p/IFoo.aidl:1.11-11: syntax error, unexpected $end\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl", R"(package p;)", typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnEmptyListWithComma) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/Foo.aidl:1.45-47: syntax error, unexpected ',', expecting '}'\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/Foo.aidl", R"(package p; parcelable Foo { uint64_t[] a = { , }; })",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnMalformedConstHexValue) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.50-71: Could not parse hexvalue: 0xffffffffffffffffff\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                      interface IFoo {
                        const int BAD_HEX_VALUE = 0xffffffffffffffffff;
                      }
                   )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnMalformedQualifiedNameAsIdentifier) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.25-26: syntax error, unexpected ';', expecting identifier or "
      "cpp_header (which can also be used as an identifier)\n";
  CaptureStderr();
  // Notice the trailing dot(.) in the name, which isn't a correct name
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl", R"(package p; parcelable A.; )", typenames_,
                           GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnMalformedQualifiedNameAsPackage) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.11-12: syntax error, unexpected ';', expecting identifier or "
      "cpp_header (which can also be used as an identifier)\n";
  CaptureStderr();
  // Notice the trailing dot(.) in the package name
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl", R"(package p.; parcelable A; )", typenames_,
                           GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, ParsePositiveConstHexValue) {
  AidlError error;
  auto parse_result = Parse("p/IFoo.aidl",
                            R"(package p;
              interface IFoo {
                const int POSITIVE_HEX_VALUE = 0xf5;
              }
           )",
                            typenames_, GetLanguage(), &error);
  EXPECT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  const auto& cpp_constants = interface->GetConstantDeclarations();
  EXPECT_EQ((size_t)1, cpp_constants.size());
  EXPECT_EQ("POSITIVE_HEX_VALUE", cpp_constants[0]->GetName());
  EXPECT_TRUE(cpp_constants[0]->CheckValid(typenames_));
  EXPECT_EQ("245", cpp_constants[0]->ValueString(cpp::ConstantValueDecorator));
}

TEST_P(AidlTest, ParseNegativeConstHexValue) {
  AidlError error;
  auto parse_result = Parse("p/IFoo.aidl",
                            R"(package p;
              interface IFoo {
                const int NEGATIVE_HEX_VALUE = 0xffffffff;
              }
           )",
                            typenames_, GetLanguage(), &error);
  ASSERT_NE(nullptr, parse_result);
  const AidlInterface* interface = parse_result->AsInterface();
  ASSERT_NE(nullptr, interface);
  const auto& cpp_constants = interface->GetConstantDeclarations();
  EXPECT_EQ((size_t)1, cpp_constants.size());
  EXPECT_EQ("NEGATIVE_HEX_VALUE", cpp_constants[0]->GetName());
  EXPECT_EQ(true, cpp_constants[0]->CheckValid(typenames_));
  EXPECT_EQ("-1", cpp_constants[0]->ValueString(cpp::ConstantValueDecorator));
}

TEST_P(AidlTest, UnderstandsNestedParcelables) {
  io_delegate_.SetFileContents(
      "p/Outer.aidl",
      "package p; parcelable Outer.Inner cpp_header \"baz/header\";");
  import_paths_.emplace("");
  const string input_path = "p/IFoo.aidl";
  const string input = "package p; import p.Outer; interface IFoo"
                       " { Outer.Inner get(); }";

  auto parse_result = Parse(input_path, input, typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);

  EXPECT_TRUE(typenames_.ResolveTypename("p.Outer.Inner").is_resolved);
  // C++ uses "::" instead of "." to refer to a inner class.
  AidlTypeSpecifier nested_type(AIDL_LOCATION_HERE, "p.Outer.Inner", false, nullptr, "");
  EXPECT_EQ("::p::Outer::Inner", cpp::CppNameOf(nested_type, typenames_));
}

TEST_P(AidlTest, UnderstandsNativeParcelables) {
  io_delegate_.SetFileContents(
      "p/Bar.aidl",
      "package p; parcelable Bar cpp_header \"baz/header\";");
  import_paths_.emplace("");
  const string input_path = "p/IFoo.aidl";
  const string input = "package p; import p.Bar; interface IFoo { }";
  auto parse_result = Parse(input_path, input, typenames_, GetLanguage());
  EXPECT_NE(nullptr, parse_result);
  EXPECT_TRUE(typenames_.ResolveTypename("p.Bar").is_resolved);
  AidlTypeSpecifier native_type(AIDL_LOCATION_HERE, "p.Bar", false, nullptr, "");
  native_type.Resolve(typenames_);

  EXPECT_EQ("p.Bar", java::InstantiableJavaSignatureOf(native_type, typenames_));
  // C++ understands C++ specific stuff
  EXPECT_EQ("::p::Bar", cpp::CppNameOf(native_type, typenames_));
  set<string> headers;
  cpp::AddHeaders(native_type, typenames_, &headers);
  EXPECT_EQ(1u, headers.size());
  EXPECT_EQ(1u, headers.count("baz/header"));
}

TEST_F(AidlTest, WritesCorrectDependencyFile) {
  // While the in tree build system always gives us an output file name,
  // other android tools take advantage of our ability to infer the intended
  // file name.  This test makes sure we handle this correctly.
  vector<string> args = {
    "aidl",
    "-d dep/file/path",
    "-o place/for/output",
    "p/IFoo.aidl"};
  Options options = Options::From(args);
  io_delegate_.SetFileContents(options.InputFiles().front(), "package p; interface IFoo {}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  string actual_dep_file_contents;
  EXPECT_TRUE(io_delegate_.GetWrittenContents(options.DependencyFile(), &actual_dep_file_contents));
  EXPECT_EQ(actual_dep_file_contents, kExpectedDepFileContents);
}

TEST_F(AidlTest, WritesCorrectDependencyFileNinja) {
  // While the in tree build system always gives us an output file name,
  // other android tools take advantage of our ability to infer the intended
  // file name.  This test makes sure we handle this correctly.
  vector<string> args = {
    "aidl",
    "-d dep/file/path",
    "--ninja",
    "-o place/for/output",
    "p/IFoo.aidl"};
  Options options = Options::From(args);
  io_delegate_.SetFileContents(options.InputFiles().front(), "package p; interface IFoo {}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  string actual_dep_file_contents;
  EXPECT_TRUE(io_delegate_.GetWrittenContents(options.DependencyFile(), &actual_dep_file_contents));
  EXPECT_EQ(actual_dep_file_contents, kExpectedNinjaDepFileContents);
}

TEST_F(AidlTest, WritesTrivialDependencyFileForParcelableDeclaration) {
  // The SDK uses aidl to decide whether a .aidl file is a parcelable.  It does
  // this by calling aidl with every .aidl file it finds, then parsing the
  // generated dependency files.  Those that reference .java output files are
  // for interfaces and those that do not are parcelables.  However, for both
  // parcelables and interfaces, we *must* generate a non-empty dependency file.
  vector<string> args = {
    "aidl",
    "-o place/for/output",
    "-d dep/file/path",
    "p/Foo.aidl"};
  Options options = Options::From(args);
  io_delegate_.SetFileContents(options.InputFiles().front(), "package p; parcelable Foo;");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  string actual_dep_file_contents;
  EXPECT_TRUE(io_delegate_.GetWrittenContents(options.DependencyFile(), &actual_dep_file_contents));
  EXPECT_EQ(actual_dep_file_contents, kExpectedParcelableDeclarationDepFileContents);
}

TEST_F(AidlTest, WritesDependencyFileForStructuredParcelable) {
  vector<string> args = {
    "aidl",
    "--structured",
    "-o place/for/output",
    "-d dep/file/path",
    "p/Foo.aidl"};
  Options options = Options::From(args);
  io_delegate_.SetFileContents(options.InputFiles().front(), "package p; parcelable Foo {int a;}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  string actual_dep_file_contents;
  EXPECT_TRUE(io_delegate_.GetWrittenContents(options.DependencyFile(), &actual_dep_file_contents));
  EXPECT_EQ(actual_dep_file_contents, kExpectedStructuredParcelableDepFileContents);
}

TEST_F(AidlTest, NoJavaOutputForParcelableDeclaration) {
 vector<string> args = {
    "aidl",
    "--lang=java",
    "-o place/for/output",
    "p/Foo.aidl"};
  Options options = Options::From(args);
  io_delegate_.SetFileContents(options.InputFiles().front(), "package p; parcelable Foo;");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  string output_file_contents;
  EXPECT_FALSE(io_delegate_.GetWrittenContents(options.OutputFile(), &output_file_contents));
}

TEST_P(AidlTest, RejectsListArray) {
  const string expected_stderr = "ERROR: a/Foo.aidl:2.1-7: List[] is not supported.\n";
  const string list_array_parcelable =
      "package a; parcelable Foo {\n"
      "  List[] lists; }";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", list_array_parcelable, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectsPrimitiveListInStableAidl) {
  AidlError error;
  string expected_stderr =
      "ERROR: a/IFoo.aidl:2.7-11: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n";
  if (GetLanguage() != Options::Language::JAVA) {
    expected_stderr =
        "ERROR: a/IFoo.aidl:2.1-7: "
        "Currently, only the Java backend supports non-generic List.\n";
  }

  const string primitive_interface =
      "package a; interface IFoo {\n"
      "  List foo(); }";
  CaptureStderr();
  AidlTypenames tn1;
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", primitive_interface, tn1, GetLanguage(), &error,
                           {"--structured"}));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  string primitive_parcelable =
      "package a; parcelable IFoo {\n"
      "  List foo;}";
  CaptureStderr();
  AidlTypenames tn2;
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", primitive_parcelable, tn2, GetLanguage(), &error,
                           {"--structured"}));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, ExtensionTest) {
  CaptureStderr();
  string extendable_parcelable =
      "package a; parcelable Data {\n"
      "  ParcelableHolder extension;\n"
      "  ParcelableHolder extension2;\n"
      "}";
  if (GetLanguage() == Options::Language::RUST) {
    EXPECT_EQ(nullptr, Parse("a/Data.aidl", extendable_parcelable, typenames_, GetLanguage()));
    EXPECT_EQ(
        "ERROR: a/Data.aidl:2.1-19: The Rust backend does not support ParcelableHolder "
        "yet.\n",
        GetCapturedStderr());
  } else {
    EXPECT_NE(nullptr, Parse("a/Data.aidl", extendable_parcelable, typenames_, GetLanguage()));
    EXPECT_EQ("", GetCapturedStderr());
  }
}
TEST_P(AidlTest, ParcelableHolderAsReturnType) {
  CaptureStderr();
  string parcelableholder_return_interface =
      "package a; interface IFoo {\n"
      "  ParcelableHolder foo();\n"
      "}";
  EXPECT_EQ(nullptr,
            Parse("a/IFoo.aidl", parcelableholder_return_interface, typenames_, GetLanguage()));

  if (GetLanguage() == Options::Language::RUST) {
    EXPECT_EQ(
        "ERROR: a/IFoo.aidl:2.19-23: ParcelableHolder cannot be a return type\n"
        "ERROR: a/IFoo.aidl:2.1-19: The Rust backend does not support ParcelableHolder "
        "yet.\n",
        GetCapturedStderr());
    return;
  }
  EXPECT_EQ("ERROR: a/IFoo.aidl:2.19-23: ParcelableHolder cannot be a return type\n",
            GetCapturedStderr());
}

TEST_P(AidlTest, ParcelableHolderAsArgumentType) {
  CaptureStderr();
  string extendable_parcelable_arg_interface =
      "package a; interface IFoo {\n"
      "  void foo(in ParcelableHolder ph);\n"
      "}";
  EXPECT_EQ(nullptr,
            Parse("a/IFoo.aidl", extendable_parcelable_arg_interface, typenames_, GetLanguage()));

  if (GetLanguage() == Options::Language::RUST) {
    EXPECT_EQ(
        "ERROR: a/IFoo.aidl:2.31-34: ParcelableHolder cannot be an argument type\n"
        "ERROR: a/IFoo.aidl:2.14-31: The Rust backend does not support ParcelableHolder "
        "yet.\n",
        GetCapturedStderr());
    return;
  }
  EXPECT_EQ("ERROR: a/IFoo.aidl:2.31-34: ParcelableHolder cannot be an argument type\n",
            GetCapturedStderr());
}

TEST_P(AidlTest, RejectNullableParcelableHolderField) {
  io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo { @nullable ParcelableHolder ext; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  const string expected_stderr = "ERROR: Foo.aidl:1.27-44: ParcelableHolder cannot be nullable.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  if (GetLanguage() == Options::Language::RUST) {
    EXPECT_EQ(
        "ERROR: Foo.aidl:1.27-44: ParcelableHolder cannot be nullable.\n"
        "ERROR: Foo.aidl:1.27-44: The Rust backend does not support ParcelableHolder "
        "yet.\n",
        GetCapturedStderr());
    return;
  }
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, ParcelablesWithConstants) {
  io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo { const int BIT = 0x1 << 3; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
}

TEST_P(AidlTest, UnionWithConstants) {
  io_delegate_.SetFileContents("Foo.aidl", "union Foo { const int BIT = 0x1 << 3; int n; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
}

TEST_F(AidlTest, ApiDump) {
  io_delegate_.SetFileContents(
      "foo/bar/IFoo.aidl",
      "package foo.bar;\n"
      "import foo.bar.Data;\n"
      "// comment @hide\n"
      "interface IFoo {\n"
      "    /* @hide */\n"
      "    int foo(out int[] a, String b, boolean c, inout List<String>  d);\n"
      "    int foo2(@utf8InCpp String x, inout List<String>  y);\n"
      "    IFoo foo3(IFoo foo);\n"
      "    Data getData();\n"
      "    // @hide\n"
      "    const int A = 1;\n"
      "    const String STR = \"Hello\";\n"
      "}\n");
  io_delegate_.SetFileContents("foo/bar/Data.aidl",
                               "package foo.bar;\n"
                               "import foo.bar.IFoo;\n"
                               "/* @hide*/\n"
                               "parcelable Data {\n"
                               "   // @hide\n"
                               "   int x = 10;\n"
                               "   // @hide\n"
                               "   int y;\n"
                               "   /*@hide2*/\n"
                               "   IFoo foo;\n"
                               "   // It should be @hide property\n"
                               "   @nullable String[] c;\n"
                               "}\n");
  io_delegate_.SetFileContents("api.aidl", "");
  vector<string> args = {"aidl", "--dumpapi", "--out=dump", "--include=.",
                         "foo/bar/IFoo.aidl", "foo/bar/Data.aidl"};
  Options options = Options::From(args);
  bool result = dump_api(options, io_delegate_);
  ASSERT_TRUE(result);
  string actual;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/IFoo.aidl", &actual));
  EXPECT_EQ(actual, string(kPreamble).append(R"(package foo.bar;
/* @hide */
interface IFoo {
  /* @hide */
  int foo(out int[] a, String b, boolean c, inout List<String> d);
  int foo2(@utf8InCpp String x, inout List<String> y);
  foo.bar.IFoo foo3(foo.bar.IFoo foo);
  foo.bar.Data getData();
  /* @hide */
  const int A = 1;
  const String STR = "Hello";
}
)"));

  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/Data.aidl", &actual));
  EXPECT_EQ(actual, string(kPreamble).append(R"(package foo.bar;
/* @hide */
parcelable Data {
  /* @hide */
  int x = 10;
  /* @hide */
  int y;
  foo.bar.IFoo foo;
  /* @hide */
  @nullable String[] c;
}
)"));
}

TEST_F(AidlTest, ApiDumpWithManualIds) {
  io_delegate_.SetFileContents(
      "foo/bar/IFoo.aidl",
      "package foo.bar;\n"
      "interface IFoo {\n"
      "    int foo() = 1;\n"
      "    int bar() = 2;\n"
      "    int baz() = 10;\n"
      "}\n");

  vector<string> args = {"aidl", "--dumpapi", "-o dump", "foo/bar/IFoo.aidl"};
  Options options = Options::From(args);
  bool result = dump_api(options, io_delegate_);
  ASSERT_TRUE(result);
  string actual;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/IFoo.aidl", &actual));
  EXPECT_EQ(actual, string(kPreamble).append(R"(package foo.bar;
interface IFoo {
  int foo() = 1;
  int bar() = 2;
  int baz() = 10;
}
)"));
}

TEST_F(AidlTest, ApiDumpWithManualIdsOnlyOnSomeMethods) {
  const string expected_stderr =
      "ERROR: foo/bar/IFoo.aidl:4.8-12: You must either assign id's to all methods or to none of "
      "them.\n";
  io_delegate_.SetFileContents(
      "foo/bar/IFoo.aidl",
      "package foo.bar;\n"
      "interface IFoo {\n"
      "    int foo() = 1;\n"
      "    int bar();\n"
      "    int baz() = 10;\n"
      "}\n");

  vector<string> args = {"aidl", "--dumpapi", "-o dump", "foo/bar/IFoo.aidl"};
  Options options = Options::From(args);
  CaptureStderr();
  EXPECT_FALSE(dump_api(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ApiDumpConstWithAnnotation) {
  io_delegate_.SetFileContents("foo/bar/IFoo.aidl",
                               "package foo.bar;\n"
                               "interface IFoo {\n"
                               "    @utf8InCpp String foo();\n"
                               "    const @utf8InCpp String bar = \"bar\";\n"
                               "}\n");

  vector<string> args = {"aidl", "--dumpapi", "-o dump", "foo/bar/IFoo.aidl"};
  Options options = Options::From(args);
  CaptureStderr();
  EXPECT_TRUE(dump_api(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
  string actual;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/IFoo.aidl", &actual));
  EXPECT_EQ(string(kPreamble).append(R"(package foo.bar;
interface IFoo {
  @utf8InCpp String foo();
  const @utf8InCpp String bar = "bar";
}
)"),
            actual);
}

TEST_F(AidlTest, ApiDumpWithEnums) {
  io_delegate_.SetFileContents("foo/bar/Enum.aidl",
                               "package foo.bar;\n"
                               "enum Enum {\n"
                               "    FOO,\n"
                               "    BAR = FOO + 1,\n"
                               "}\n");

  vector<string> args = {"aidl", "--dumpapi", "-I . ", "-o dump", "foo/bar/Enum.aidl"};
  Options options = Options::From(args);
  CaptureStderr();
  EXPECT_TRUE(dump_api(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
  string actual;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/Enum.aidl", &actual));
  EXPECT_EQ(string(kPreamble).append("package foo.bar;\n"
                                     "enum Enum {\n"
                                     "  FOO = 0,\n"
                                     "  BAR = 1,\n"
                                     "}\n"),
            actual);
}

TEST_F(AidlTest, ApiDumpWithEnumDefaultValues) {
  io_delegate_.SetFileContents("foo/bar/Enum.aidl",
                               "package foo.bar;\n"
                               "enum Enum {\n"
                               "    FOO,\n"
                               "}\n");
  io_delegate_.SetFileContents("foo/bar/Foo.aidl",
                               "package foo.bar;\n"
                               "import foo.bar.Enum;\n"
                               "parcelable Foo {\n"
                               "    Enum e = Enum.FOO;\n"
                               "}\n");

  vector<string> args = {"aidl", "--dumpapi", "-I . ", "-o dump", "foo/bar/Foo.aidl"};
  Options options = Options::From(args);
  CaptureStderr();
  EXPECT_TRUE(dump_api(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
  string actual;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("dump/foo/bar/Foo.aidl", &actual));
  EXPECT_EQ(string(kPreamble).append("package foo.bar;\n"
                                     "parcelable Foo {\n"
                                     "  foo.bar.Enum e = foo.bar.Enum.FOO;\n"
                                     "}\n"),
            actual);
}

TEST_F(AidlTest, CheckNumGenericTypeSecifier) {
  const string expected_list_stderr =
      "ERROR: p/IFoo.aidl:1.37-41: List can only have one type parameter, but got: "
      "'List<String,String>'\n";
  const string expected_map_stderr =
      "ERROR: p/IFoo.aidl:1.37-40: Map must have 0 or 2 type parameters, but got 'Map<String>'\n";
  Options options = Options::From("aidl p/IFoo.aidl IFoo.java");
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "void foo(List<String, String> a);}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_list_stderr, GetCapturedStderr());

  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "void foo(Map<String> a);}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_map_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, CheckTypeParameterInMapType) {
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.28-31: The type of key in map must be String, but it is 'p.Bar'\n";
  Options options = Options::From("aidl -I p p/IFoo.aidl");
  io_delegate_.SetFileContents("p/Bar.aidl", "package p; parcelable Bar { String s; }");

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Map<String, Bar> foo();}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Map<Bar, Bar> foo();}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Map<String, String> foo();}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Map<String, ParcelFileDescriptor> foo();}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, WrongGenericType) {
  const string expected_stderr = "ERROR: p/IFoo.aidl:1.28-34: String is not a generic type.\n";
  Options options = Options::From("aidl p/IFoo.aidl IFoo.java");
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "String<String> foo(); }");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, UserDefinedUnstructuredGenericParcelableType) {
  Options optionsForParcelable = Options::From("aidl -I p p/Bar.aidl");
  io_delegate_.SetFileContents("p/Bar.aidl", "package p; parcelable Bar<T, T>;");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(optionsForParcelable, io_delegate_));
  EXPECT_EQ("ERROR: p/Bar.aidl:1.22-26: Every type parameter should be unique.\n",
            GetCapturedStderr());

  Options options = Options::From("aidl -I p p/IFoo.aidl");
  io_delegate_.SetFileContents("p/Bar.aidl", "package p; parcelable Bar;");
  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Bar<String, String> foo();}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("ERROR: p/IFoo.aidl:1.28-31: p.Bar is not a generic type.\n", GetCapturedStderr());
  io_delegate_.SetFileContents("p/Bar.aidl", "package p; parcelable Bar<T>;");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("ERROR: p/IFoo.aidl:1.28-31: p.Bar must have 1 type parameters, but got 2\n",
            GetCapturedStderr());
  io_delegate_.SetFileContents("p/Bar.aidl", "package p; parcelable Bar<T, V>;");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Bar<String, ParcelFileDescriptor> foo();}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Bar<int, long> foo();}");

  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo {"
                               "Bar<int[], long[]> foo();}");

  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, FailOnMultipleTypesInSingleFile) {
  std::vector<std::string> rawOptions{"aidl --lang=java -o out foo/bar/Foo.aidl",
                                      "aidl --lang=cpp -o out -h out/include foo/bar/Foo.aidl",
                                      "aidl --lang=rust -o out foo/bar/Foo.aidl"};
  for (const auto& rawOption : rawOptions) {
    string expected_stderr =
        "ERROR: foo/bar/Foo.aidl:3.1-10: You must declare only one type per file.\n";
    Options options = Options::From(rawOption);
    io_delegate_.SetFileContents(options.InputFiles().front(),
                                 "package foo.bar;\n"
                                 "interface IFoo1 { int foo(); }\n"
                                 "interface IFoo2 { int foo(); }\n"
                                 "parcelable Data1 { int a; int b;}\n"
                                 "parcelable Data2 { int a; int b;}\n");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
    EXPECT_EQ(expected_stderr, GetCapturedStderr());

    io_delegate_.SetFileContents(options.InputFiles().front(),
                                 "package foo.bar;\n"
                                 "interface IFoo1 { int foo(); }\n"
                                 "interface IFoo2 { int foo(); }\n");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
    EXPECT_EQ(expected_stderr, GetCapturedStderr());

    expected_stderr = "ERROR: foo/bar/Foo.aidl:3.11-17: You must declare only one type per file.\n";
    io_delegate_.SetFileContents(options.InputFiles().front(),
                                 "package foo.bar;\n"
                                 "parcelable Data1 { int a; int b;}\n"
                                 "parcelable Data2 { int a; int b;}\n");
    CaptureStderr();
    EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
    EXPECT_EQ(expected_stderr, GetCapturedStderr());
  }
}

TEST_P(AidlTest, FailParseOnEmptyFile) {
  const string contents = "";
  const string expected_stderr = "ERROR: a/IFoo.aidl:1.1-1: syntax error, unexpected $end\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/IFoo.aidl", contents, typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, MultipleInputFiles) {
  Options options = Options::From(
      "aidl --lang=java -o out -I . foo/bar/IFoo.aidl foo/bar/Data.aidl");

  io_delegate_.SetFileContents(options.InputFiles().at(0),
      "package foo.bar;\n"
      "import foo.bar.Data;\n"
      "interface IFoo { Data getData(); }\n");

  io_delegate_.SetFileContents(options.InputFiles().at(1),
        "package foo.bar;\n"
        "import foo.bar.IFoo;\n"
        "parcelable Data { IFoo foo; }\n");

  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  string content;
  for (const auto file : {
    "out/foo/bar/IFoo.java", "out/foo/bar/Data.java"}) {
    content.clear();
    EXPECT_TRUE(io_delegate_.GetWrittenContents(file, &content));
    EXPECT_FALSE(content.empty());
  }
}

TEST_F(AidlTest, MultipleInputFilesCpp) {
  Options options = Options::From("aidl --lang=cpp -o out -h out/include "
      "-I . foo/bar/IFoo.aidl foo/bar/Data.aidl");

  io_delegate_.SetFileContents(options.InputFiles().at(0),
      "package foo.bar;\n"
      "import foo.bar.Data;\n"
      "interface IFoo { Data getData(); }\n");

  io_delegate_.SetFileContents(options.InputFiles().at(1),
        "package foo.bar;\n"
        "import foo.bar.IFoo;\n"
        "parcelable Data { IFoo foo; }\n");

  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  string content;
  for (const auto file : {
    "out/foo/bar/IFoo.cpp", "out/foo/bar/Data.cpp",
    "out/include/foo/bar/IFoo.h", "out/include/foo/bar/Data.h",
    "out/include/foo/bar/BpFoo.h", "out/include/foo/bar/BpData.h",
    "out/include/foo/bar/BnFoo.h", "out/include/foo/bar/BnData.h"}) {
    content.clear();
    EXPECT_TRUE(io_delegate_.GetWrittenContents(file, &content));
    EXPECT_FALSE(content.empty());
  }
}

TEST_F(AidlTest, MultipleInputFilesRust) {
  Options options =
      Options::From("aidl --lang=rust -o out -I . foo/bar/IFoo.aidl foo/bar/Data.aidl");

  io_delegate_.SetFileContents(options.InputFiles().at(0),
                               "package foo.bar;\n"
                               "import foo.bar.Data;\n"
                               "interface IFoo { Data getData(); }\n");

  io_delegate_.SetFileContents(options.InputFiles().at(1),
                               "package foo.bar;\n"
                               "import foo.bar.IFoo;\n"
                               "parcelable Data { IFoo foo; }\n");

  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  string content;
  for (const auto file : {"out/foo/bar/IFoo.rs", "out/foo/bar/Data.rs"}) {
    content.clear();
    EXPECT_TRUE(io_delegate_.GetWrittenContents(file, &content));
    EXPECT_FALSE(content.empty());
  }
}

TEST_F(AidlTest, ConflictWithMetaTransactionGetVersion) {
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.31-51:  method getInterfaceVersion() is reserved for internal use.\n";
  Options options = Options::From("aidl --lang=java -o place/for/output p/IFoo.aidl");
  // int getInterfaceVersion() is one of the meta transactions
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "int getInterfaceVersion(); }");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ConflictWithSimilarMetaTransaction) {
  // boolean getInterfaceVersion() is not a meta transaction, but should be
  // prevented because return type is not part of a method signature
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.35-55:  method getInterfaceVersion() is reserved for internal use.\n";
  Options options = Options::From("aidl --lang=java -o place/for/output p/IFoo.aidl");
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "boolean getInterfaceVersion(); }");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ConflictWithMetaTransactionGetName) {
  // this is another reserved name
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:1.34-53:  method getTransactionName(int) is reserved for internal use.\n";
  Options options = Options::From("aidl --lang=java -o place/for/output p/IFoo.aidl");
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "String getTransactionName(int code); }");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  // this is not a meta interface method as it differs type arguments
  io_delegate_.SetFileContents(options.InputFiles().front(),
                               "package p; interface IFoo {"
                               "String getTransactionName(); }");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, DifferentOrderAnnotationsInCheckAPI) {
  Options options = Options::From("aidl --checkapi old new");
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p; interface IFoo{ @utf8InCpp @nullable String foo();}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p; interface IFoo{ @nullable @utf8InCpp String foo();}");

  EXPECT_TRUE(::android::aidl::check_api(options, io_delegate_));
}

TEST_F(AidlTest, SuccessOnIdenticalApiDumps) {
  Options options = Options::From("aidl --checkapi old new");
  io_delegate_.SetFileContents("old/p/IFoo.aidl", "package p; interface IFoo{ void foo();}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl", "package p; interface IFoo{ void foo();}");

  EXPECT_TRUE(::android::aidl::check_api(options, io_delegate_));
}

TEST_F(AidlTest, CheckApi_EnumFieldsWithDefaultValues) {
  Options options = Options::From("aidl --checkapi old new");
  const string foo_definition = "package p; parcelable Foo{ p.Enum e = p.Enum.FOO; }";
  const string enum_definition = "package p; enum Enum { FOO }";
  io_delegate_.SetFileContents("old/p/Foo.aidl", foo_definition);
  io_delegate_.SetFileContents("old/p/Enum.aidl", enum_definition);
  io_delegate_.SetFileContents("new/p/Foo.aidl", foo_definition);
  io_delegate_.SetFileContents("new/p/Enum.aidl", enum_definition);

  EXPECT_TRUE(::android::aidl::check_api(options, io_delegate_));
}

class AidlTestCompatibleChanges : public AidlTest {
 protected:
  Options options_ = Options::From("aidl --checkapi old new");
};

TEST_F(AidlTestCompatibleChanges, NewType) {
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/p/IBar.aidl",
                               "package p;"
                               "interface IBar {"
                               "  void bar();"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewMethod) {
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "  void bar();"
                               "  void baz(in List<IFoo> arg);"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewField) {
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "  int bar = 0;"
                               "  @nullable List<Data> list;"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewEnumerator) {
  io_delegate_.SetFileContents("old/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "  BAR = 2,"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, ReorderedEnumerator) {
  io_delegate_.SetFileContents("old/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "  BAR = 2,"
                               "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  BAR = 2,"
                               "  FOO = 1,"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewUnionField) {
  io_delegate_.SetFileContents("old/p/Union.aidl",
                               "package p;"
                               "union Union {"
                               "  String foo;"
                               "}");
  io_delegate_.SetFileContents("new/p/Union.aidl",
                               "package p;"
                               "union Union {"
                               "  String foo;"
                               "  int num;"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewPackage) {
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  io_delegate_.SetFileContents("new/q/IFoo.aidl",
                               "package q;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/q/Data.aidl",
                               "package q;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, ArgNameChange) {
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int a);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(int b);"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, AddedConstValue) {
  io_delegate_.SetFileContents("old/p/I.aidl",
                               "package p; interface I {"
                               "const int A = 1; }");
  io_delegate_.SetFileContents("new/p/I.aidl",
                               "package p ; interface I {"
                               "const int A = 1; const int B = 2;}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, ChangedConstValueOrder) {
  io_delegate_.SetFileContents("old/p/I.aidl",
                               "package p; interface I {"
                               "const int A = 1; const int B = 2;}");
  io_delegate_.SetFileContents("new/p/I.aidl",
                               "package p ; interface I {"
                               "const int B = 2; const int A = 1;}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, ReorderedAnnatations) {
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "@JavaPassthrough(annotation=\"Alice\")"
                               "@JavaPassthrough(annotation=\"Bob\")"
                               "parcelable Foo {}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "@JavaPassthrough(annotation=\"Bob\")"
                               "@JavaPassthrough(annotation=\"Alice\")"
                               "parcelable Foo {}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, OkayToDeprecate) {
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "parcelable Foo {}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "@JavaPassthrough(annotation=\"@Deprecated\")"
                               "parcelable Foo {}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

TEST_F(AidlTestCompatibleChanges, NewFieldOfNewType) {
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int num;"
                               "}");
  io_delegate_.SetFileContents(
      "new/p/Data.aidl",
      "package p;"
      "parcelable Data {"
      "  int num;"
      "  p.Enum e;"  // this is considered as valid since 0(enum default) is valid for "Enum" type
      "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 0,"
                               "  BAR = 1,"
                               "}");
  EXPECT_TRUE(::android::aidl::check_api(options_, io_delegate_));
}

class AidlTestIncompatibleChanges : public AidlTest {
 protected:
  Options options_ = Options::From("aidl --checkapi old new");
};

TEST_F(AidlTestIncompatibleChanges, RemovedType) {
  const string expected_stderr = "ERROR: old/p/IFoo.aidl:1.11-20: Removed type: p.IFoo\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedMethod) {
  const string expected_stderr =
      "ERROR: old/p/IFoo.aidl:1.61-65: Removed or changed method: p.IFoo.bar(String)\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, UntypedListInInterface) {
  const string expected_stderr =
      "ERROR: new/p/IFoo.aidl:1.61-65: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n"
      "ERROR: new/p/IFoo.aidl: Failed to read.\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(in List arg);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestCompatibleChanges, UntypedListInParcelable) {
  const string expected_stderr =
      "ERROR: new/p/Data.aidl:1.54-59: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n"
      "ERROR: new/p/Data.aidl: Failed to read.\n";
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "  @nullable List list;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedField) {
  const string expected_stderr =
      "ERROR: new/p/Data.aidl:1.21-26: Number of fields in p.Data is reduced from 2 to 1.\n";
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "  int bar;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, NewFieldWithNoDefault) {
  const string expected_stderr =
      "ERROR: new/p/Data.aidl:1.46-50: Field 'str' does not have a useful default in some "
      "backends. Please either provide a default value for this field or mark the field as "
      "@nullable. This value or a null value will be used automatically when an old version of "
      "this parcelable is sent to a process which understands a new version of this parcelable. In "
      "order to make sure your code continues to be backwards compatible, make sure the default or "
      "null value does not cause a semantic change to this parcelable.\n";
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int num;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int num;"
                               "  String str;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, NewFieldWithNonZeroEnum) {
  const string expected_stderr =
      "ERROR: new/p/Data.aidl:1.46-48: Field 'e' of enum 'Enum' can't be initialized as '0'. "
      "Please make sure 'Enum' has '0' as a valid value.\n";
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int num;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int num;"
                               "  p.Enum e;"
                               "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "  BAR = 2,"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedEnumerator) {
  const string expected_stderr =
      "ERROR: new/p/Enum.aidl:1.15-20: Removed enumerator from p.Enum: FOO\n";
  io_delegate_.SetFileContents("old/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "  BAR = 2,"
                               "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  BAR = 2,"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedUnionField) {
  const string expected_stderr =
      "ERROR: new/p/Union.aidl:1.16-22: Number of fields in p.Union is reduced from 2 to 1.\n";
  io_delegate_.SetFileContents("old/p/Union.aidl",
                               "package p;"
                               "union Union {"
                               "  String str;"
                               "  int num;"
                               "}");
  io_delegate_.SetFileContents("new/p/Union.aidl",
                               "package p;"
                               "union Union {"
                               "  String str;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RenamedMethod) {
  const string expected_stderr =
      "ERROR: old/p/IFoo.aidl:1.61-65: Removed or changed method: p.IFoo.bar(String)\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar2(@utf8InCpp String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RenamedType) {
  const string expected_stderr = "ERROR: old/p/IFoo.aidl:1.11-20: Removed type: p.IFoo\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo2.aidl",
                               "package p;"
                               "interface IFoo2 {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedEnumerator) {
  const string expected_stderr =
      "ERROR: new/p/Enum.aidl:1.15-20: Changed enumerator value: p.Enum::FOO from 1 to 3.\n";
  io_delegate_.SetFileContents("old/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 1,"
                               "  BAR = 2,"
                               "}");
  io_delegate_.SetFileContents("new/p/Enum.aidl",
                               "package p;"
                               "enum Enum {"
                               "  FOO = 3,"
                               "  BAR = 2,"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ReorderedMethod) {
  const string expected_stderr =
      "ERROR: new/p/IFoo.aidl:1.67-71: Transaction ID changed: p.IFoo.foo(String[]) is changed "
      "from 0 to 1.\n"
      "ERROR: new/p/IFoo.aidl:1.33-37: Transaction ID changed: p.IFoo.bar(String) is changed from "
      "1 to 0.\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void bar(@utf8InCpp String str);"
                               "  void foo(in String[] str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ReorderedField) {
  const string expected_stderr =
      "ERROR: new/p/Data.aidl:1.33-37: Reordered bar from 1 to 0.\n"
      "ERROR: new/p/Data.aidl:1.43-47: Reordered foo from 0 to 1.\n";
  io_delegate_.SetFileContents("old/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int foo;"
                               "  int bar;"
                               "}");
  io_delegate_.SetFileContents("new/p/Data.aidl",
                               "package p;"
                               "parcelable Data {"
                               "  int bar;"
                               "  int foo;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedDirectionSpecifier) {
  const string expected_stderr = "ERROR: new/p/IFoo.aidl:1.33-37: Direction changed: in to out.\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(out String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, AddedAnnotation) {
  const string expected_stderr =
      "ERROR: new/p/IFoo.aidl:1.51-58: Changed annotations: (empty) to @utf8InCpp\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in @utf8InCpp String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedAnnotation) {
  const string expected_stderr =
      "ERROR: new/p/IFoo.aidl:1.66-72: Changed annotations: @utf8InCpp to (empty)\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(@utf8InCpp String str);"
                               "}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl",
                               "package p;"
                               "interface IFoo {"
                               "  void foo(in String[] str);"
                               "  void bar(String str);"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedBackingTypeOfEnum) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.11-32: Type changed: byte to long.\n"
      "ERROR: new/p/Foo.aidl:1.36-40: Changed backing types.\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "@Backing(type=\"byte\")"
                               "enum Foo {"
                               " FOO, BAR,"
                               "}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "@Backing(type=\"long\")"
                               "enum Foo {"
                               " FOO, BAR,"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedAnnatationParams) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.55-59: Changed annotations: @JavaPassthrough(annotation=\"Alice\") "
      "to @JavaPassthrough(annotation=\"Bob\")\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "@JavaPassthrough(annotation=\"Alice\")"
                               "parcelable Foo {}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "@JavaPassthrough(annotation=\"Bob\")"
                               "parcelable Foo {}");

  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, AddedParcelableAnnotation) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.47-51: Changed annotations: (empty) to @JavaOnlyStableParcelable\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "parcelable Foo {"
                               "  int A;"
                               "}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "@JavaOnlyStableParcelable parcelable Foo {"
                               "  int A;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedParcelableAnnotation) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.21-25: Changed annotations: @JavaOnlyStableParcelable to (empty)\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p;"
                               "@JavaOnlyStableParcelable parcelable Foo {"
                               "  int A;"
                               "}");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p;"
                               "parcelable Foo {"
                               "  int A;"
                               "}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedPackage) {
  const string expected_stderr = "ERROR: old/q/IFoo.aidl:1.11-21: Removed type: q.IFoo\n";
  io_delegate_.SetFileContents("old/p/IFoo.aidl", "package p; interface IFoo{}");
  io_delegate_.SetFileContents("old/q/IFoo.aidl", "package q; interface IFoo{}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl", "package p; interface IFoo{}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedDefaultValue) {
  const string expected_stderr = "ERROR: new/p/D.aidl:1.30-32: Changed default value: 1 to 2.\n";
  io_delegate_.SetFileContents("old/p/D.aidl", "package p; parcelable D { int a = 1; }");
  io_delegate_.SetFileContents("new/p/D.aidl", "package p; parcelable D { int a = 2; }");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, RemovedConstValue) {
  const string expected_stderr =
      "ERROR: old/p/I.aidl:1.51-53: Removed constant declaration: p.I.B\n";
  io_delegate_.SetFileContents("old/p/I.aidl",
                               "package p; interface I {"
                               "const int A = 1; const int B = 2;}");
  io_delegate_.SetFileContents("new/p/I.aidl", "package p; interface I { const int A = 1; }");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, ChangedConstValue) {
  const string expected_stderr =
      "ERROR: new/p/I.aidl:1.11-21: Changed constant value: p.I.A from 1 to 2.\n";
  io_delegate_.SetFileContents("old/p/I.aidl", "package p; interface I { const int A = 1; }");
  io_delegate_.SetFileContents("new/p/I.aidl", "package p; interface I { const int A = 2; }");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, FixedSizeAddedField) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.33-37: Number of fields in p.Foo is changed from 1 to 2. "
      "This is an incompatible change for FixedSize types.\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p; @FixedSize parcelable Foo { int A = 1; }");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p; @FixedSize parcelable Foo { int A = 1; int B = 2; }");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTestIncompatibleChanges, FixedSizeRemovedField) {
  const string expected_stderr =
      "ERROR: new/p/Foo.aidl:1.33-37: Number of fields in p.Foo is reduced from 2 to 1.\n";
  io_delegate_.SetFileContents("old/p/Foo.aidl",
                               "package p; @FixedSize parcelable Foo { int A = 1; int B = 1; }");
  io_delegate_.SetFileContents("new/p/Foo.aidl",
                               "package p; @FixedSize parcelable Foo { int A = 1; }");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options_, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectNonFixedSizeFromFixedSize) {
  const string expected_stderr =
      "ERROR: Foo.aidl:1.36-38: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "a.\n"
      "ERROR: Foo.aidl:1.44-46: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "b.\n"
      "ERROR: Foo.aidl:1.55-57: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "c.\n"
      "ERROR: Foo.aidl:1.80-82: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "d.\n"
      "ERROR: Foo.aidl:1.92-94: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "e.\n"
      "ERROR: Foo.aidl:1.109-111: The @FixedSize parcelable 'Foo' has a non-fixed size field named "
      "f.\n";

  io_delegate_.SetFileContents("Foo.aidl",
                               "@FixedSize parcelable Foo { "
                               "  int[] a;"
                               "  Bar b;"
                               "  String c;"
                               "  ParcelFileDescriptor d;"
                               "  IBinder e;"
                               "  List<String> f;"
                               "  int isFixedSize;"
                               "}");
  io_delegate_.SetFileContents("Bar.aidl", "parcelable Bar { int a; }");
  Options options =
      Options::From("aidl Foo.aidl -I . --lang=" + Options::LanguageToString(GetLanguage()));

  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, AcceptFixedSizeFromFixedSize) {
  const string expected_stderr = "";

  io_delegate_.SetFileContents("Foo.aidl", "@FixedSize parcelable Foo { int a; Bar b; }");
  io_delegate_.SetFileContents("Bar.aidl", "@FixedSize parcelable Bar { Val c; }");
  io_delegate_.SetFileContents("Val.aidl", "enum Val { A, B, }");
  Options options =
      Options::From("aidl Foo.aidl -I . --lang=" + Options::LanguageToString(GetLanguage()));

  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, RejectAmbiguousImports) {
  const string expected_stderr =
      "ERROR: p/IFoo.aidl: Duplicate files found for q.IBar from:\n"
      "dir1/q/IBar.aidl\n"
      "dir2/q/IBar.aidl\n"
      "ERROR: p/IFoo.aidl: Couldn't find import for class q.IBar\n";
  Options options = Options::From("aidl --lang=java -o out -I dir1 -I dir2 p/IFoo.aidl");
  io_delegate_.SetFileContents("p/IFoo.aidl", "package p; import q.IBar; interface IFoo{}");
  io_delegate_.SetFileContents("dir1/q/IBar.aidl", "package q; interface IBar{}");
  io_delegate_.SetFileContents("dir2/q/IBar.aidl", "package q; interface IBar{}");

  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, HandleManualIdAssignments) {
  const string expected_stderr =
      "ERROR: new/p/IFoo.aidl:1.32-36: Transaction ID changed: p.IFoo.foo() is changed from 10 to "
      "11.\n";
  Options options = Options::From("aidl --checkapi old new");
  io_delegate_.SetFileContents("old/p/IFoo.aidl", "package p; interface IFoo{ void foo() = 10;}");
  io_delegate_.SetFileContents("new/p/IFoo.aidl", "package p; interface IFoo{ void foo() = 10;}");

  EXPECT_TRUE(::android::aidl::check_api(options, io_delegate_));

  io_delegate_.SetFileContents("new/p/IFoo.aidl", "package p; interface IFoo{ void foo() = 11;}");
  CaptureStderr();
  EXPECT_FALSE(::android::aidl::check_api(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ParcelFileDescriptorIsBuiltinType) {
  Options javaOptions = Options::From("aidl --lang=java -o out p/IFoo.aidl");
  Options cppOptions = Options::From("aidl --lang=cpp -h out -o out p/IFoo.aidl");
  Options rustOptions = Options::From("aidl --lang=rust -o out p/IFoo.aidl");

  // use without import
  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p; interface IFoo{ void foo(in ParcelFileDescriptor fd);}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(javaOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cppOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(rustOptions, io_delegate_));

  // use without import but with full name
  io_delegate_.SetFileContents(
      "p/IFoo.aidl",
      "package p; interface IFoo{ void foo(in android.os.ParcelFileDescriptor fd);}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(javaOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cppOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(rustOptions, io_delegate_));

  // use with import (as before)
  io_delegate_.SetFileContents("p/IFoo.aidl",
                               "package p;"
                               "import android.os.ParcelFileDescriptor;"
                               "interface IFoo{"
                               "  void foo(in ParcelFileDescriptor fd);"
                               "}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(javaOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cppOptions, io_delegate_));
  EXPECT_EQ(0, ::android::aidl::compile_aidl(rustOptions, io_delegate_));
}

TEST_F(AidlTest, ManualIds) {
  Options options = Options::From("aidl --lang=java -o out IFoo.aidl");
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  void foo() = 0;\n"
                               "  void bar() = 1;\n"
                               "}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, ManualIdsWithMetaTransactions) {
  Options options = Options::From("aidl --lang=java --version 10 -o out IFoo.aidl");
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  void foo() = 0;\n"
                               "  void bar() = 1;\n"
                               "}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, FailOnDuplicatedIds) {
  const string expected_stderr =
      "ERROR: IFoo.aidl:3.7-11: Found duplicate method id (3) for method bar\n";
  Options options = Options::From("aidl --lang=java --version 10 -o out IFoo.aidl");
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  void foo() = 3;\n"
                               "  void bar() = 3;\n"
                               "}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, FailOnOutOfRangeIds) {
  // 16777115 is kLastMetaMethodId + 1
  const string expected_stderr =
      "ERROR: IFoo.aidl:3.7-11: Found out of bounds id (16777115) for method bar. "
      "Value for id must be between 0 and 16777114 inclusive.\n";
  Options options = Options::From("aidl --lang=java --version 10 -o out IFoo.aidl");
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  void foo() = 3;\n"
                               "  void bar() = 16777115;\n"
                               "}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, FailOnPartiallyAssignedIds) {
  const string expected_stderr =
      "ERROR: IFoo.aidl:3.7-11: You must either assign id's to all methods or to none of them.\n";
  Options options = Options::From("aidl --lang=java --version 10 -o out IFoo.aidl");
  io_delegate_.SetFileContents("IFoo.aidl",
                               "interface IFoo {\n"
                               "  void foo() = 3;\n"
                               "  void bar();\n"
                               "}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, AllowDuplicatedImportPaths) {
  Options options = Options::From("aidl --lang=java -I dir -I dir IFoo.aidl");
  io_delegate_.SetFileContents("dir/IBar.aidl", "interface IBar{}");
  io_delegate_.SetFileContents("IFoo.aidl", "import IBar; interface IFoo{}");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, FailOnAmbiguousImports) {
  const string expected_stderr =
      "ERROR: IFoo.aidl: Duplicate files found for IBar from:\n"
      "dir/IBar.aidl\n"
      "dir2/IBar.aidl\n"
      "ERROR: IFoo.aidl: Couldn't find import for class IBar\n";

  Options options = Options::From("aidl --lang=java -I dir -I dir2 IFoo.aidl");
  io_delegate_.SetFileContents("dir/IBar.aidl", "interface IBar{}");
  io_delegate_.SetFileContents("dir2/IBar.aidl", "interface IBar{}");
  io_delegate_.SetFileContents("IFoo.aidl", "import IBar; interface IFoo{}");
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, UnusedImportDoesNotContributeInclude) {
  io_delegate_.SetFileContents("a/b/IFoo.aidl",
                               "package a.b;\n"
                               "import a.b.IBar;\n"
                               "import a.b.IQux;\n"
                               "interface IFoo { IQux foo(); }\n");
  io_delegate_.SetFileContents("a/b/IBar.aidl", "package a.b; interface IBar { void foo(); }");
  io_delegate_.SetFileContents("a/b/IQux.aidl", "package a.b; interface IQux { void foo(); }");

  Options options = Options::From("aidl --lang=ndk a/b/IFoo.aidl -I . -o out -h out/include");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));

  string output;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/include/aidl/a/b/IFoo.h", &output));
  // IBar was imported but wasn't used. include is not expected.
  EXPECT_THAT(output, Not(testing::HasSubstr("#include <aidl/a/b/IBar.h>")));
  // IBar was imported and used. include is expected.
  EXPECT_THAT(output, (testing::HasSubstr("#include <aidl/a/b/IQux.h>")));
}

TEST_F(AidlTest, ParseJavaPassthroughAnnotation) {
  io_delegate_.SetFileContents("a/IFoo.aidl", R"--(package a;
    import a.MyEnum;
    @JavaPassthrough(annotation="@com.android.Alice(arg=com.android.Alice.Value.A)")
    @JavaPassthrough(annotation="@com.android.AliceTwo")
    interface IFoo {
        @JavaPassthrough(annotation="@com.android.Bob")
        void foo(@JavaPassthrough(annotation="@com.android.Cat") int x, MyEnum y);
        const @JavaPassthrough(annotation="@com.android.David") int A = 3;
    })--");
  // JavaPassthrough should work with other types as well (e.g. enum)
  io_delegate_.SetFileContents("a/MyEnum.aidl", R"--(package a;
    @JavaPassthrough(annotation="@com.android.Alice(arg=com.android.Alice.Value.A)")
    @JavaPassthrough(annotation="@com.android.AliceTwo")
    @Backing(type="byte")
    enum MyEnum {
      a, b, c
    })--");

  Options java_options = Options::From("aidl -I . --lang=java -o out a/IFoo.aidl a/MyEnum.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));

  string java_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/IFoo.java", &java_out));
  // type-decl-level annotations with newline at the end
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.Alice(arg=com.android.Alice.Value.A)\n"));
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.AliceTwo\n"));
  // member-decl-level annotations with newline at the end
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.Bob\n"));
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.David\n"));
  // inline annotations with space at the end
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.Cat "));

  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/MyEnum.java", &java_out));
  // type-decl-level annotations with newline at the end
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.Alice(arg=com.android.Alice.Value.A)\n"));
  EXPECT_THAT(java_out, testing::HasSubstr("@com.android.AliceTwo\n"));

  // Other backends shouldn't be bothered
  Options cpp_options =
      Options::From("aidl -I . --lang=cpp -o out -h out a/IFoo.aidl a/MyEnum.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cpp_options, io_delegate_));

  Options ndk_options =
      Options::From("aidl -I . --lang=ndk -o out -h out a/IFoo.aidl a/MyEnum.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(ndk_options, io_delegate_));

  Options rust_options = Options::From("aidl -I . --lang=rust -o out a/IFoo.aidl a/MyEnum.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(rust_options, io_delegate_));
}

TEST_F(AidlTest, ParseRustDerive) {
  io_delegate_.SetFileContents("a/Foo.aidl", R"(package a;
    @RustDerive(Clone=true, Copy=false)
    parcelable Foo {
        int a;
    })");

  Options rust_options = Options::From("aidl --lang=rust -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(rust_options, io_delegate_));

  string rust_out;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.rs", &rust_out));
  EXPECT_THAT(rust_out, testing::HasSubstr("#[derive(Debug, Clone)]"));

  // Other backends shouldn't be bothered
  Options cpp_options = Options::From("aidl --lang=cpp -o out -h out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(cpp_options, io_delegate_));

  Options ndk_options = Options::From("aidl --lang=ndk -o out -h out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(ndk_options, io_delegate_));

  Options java_options = Options::From("aidl --lang=java -o out a/Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(java_options, io_delegate_));
}

class AidlOutputPathTest : public AidlTest {
 protected:
  void SetUp() override {
    AidlTest::SetUp();
    io_delegate_.SetFileContents("sub/dir/foo/bar/IFoo.aidl", "package foo.bar; interface IFoo {}");
  }

  void Test(const Options& options, const std::string expected_output_path) {
    EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
    // check the existence
    EXPECT_TRUE(io_delegate_.GetWrittenContents(expected_output_path, nullptr));
  }
};

TEST_F(AidlOutputPathTest, OutDirWithNoOutputFile) {
  // <out_dir> / <package_name> / <type_name>.java
  Test(Options::From("aidl -o out sub/dir/foo/bar/IFoo.aidl"), "out/foo/bar/IFoo.java");
}

TEST_F(AidlOutputPathTest, OutDirWithOutputFile) {
  // when output file is explicitly set, it is always respected. -o option is
  // ignored.
  Test(Options::From("aidl -o out sub/dir/foo/bar/IFoo.aidl output/IFoo.java"), "output/IFoo.java");
}

TEST_F(AidlOutputPathTest, NoOutDirWithOutputFile) {
  Test(Options::From("aidl -o out sub/dir/foo/bar/IFoo.aidl output/IFoo.java"), "output/IFoo.java");
}

TEST_F(AidlOutputPathTest, NoOutDirWithNoOutputFile) {
  // output is the same as the input file except for the suffix
  Test(Options::From("aidl sub/dir/foo/bar/IFoo.aidl"), "sub/dir/foo/bar/IFoo.java");
}

TEST_P(AidlTest, FailOnOutOfBoundsInt32MaxConstInt) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.58-69: Invalid type specifier for an int64 literal: int\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                              interface IFoo {
                                const int int32_max_oob = 2147483650;
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, FailOnOutOfBoundsInt32MinConstInt) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.58-60: Invalid type specifier for an int64 literal: int\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                              interface IFoo {
                                const int int32_min_oob = -2147483650;
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, FailOnOutOfBoundsInt64MaxConstInt) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.59-86: Could not parse integer: 21474836509999999999999999\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                              interface IFoo {
                                const long int64_max_oob = 21474836509999999999999999;
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnOutOfBoundsInt64MinConstInt) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/IFoo.aidl:3.61-87: Could not parse integer: 21474836509999999999999999\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/IFoo.aidl",
                           R"(package p;
                              interface IFoo {
                                const long int64_min_oob = -21474836509999999999999999;
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::PARSE_ERROR, error);
}

TEST_P(AidlTest, FailOnOutOfBoundsAutofilledEnum) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/TestEnum.aidl:5.1-36: Invalid type specifier for an int32 literal: byte\n"
      "ERROR: p/TestEnum.aidl:5.1-36: Enumerator type differs from enum backing type.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/TestEnum.aidl",
                           R"(package p;
                              @Backing(type="byte")
                              enum TestEnum {
                                FOO = 127,
                                BAR,
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, UnsupportedBackingAnnotationParam) {
  AidlError error;
  const string expected_stderr =
      "ERROR: p/TestEnum.aidl:2.1-51: Parameter foo not supported for annotation Backing. It must "
      "be one of: type\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("p/TestEnum.aidl",
                           R"(package p;
                              @Backing(foo="byte")
                              enum TestEnum {
                                FOO = 1,
                                BAR,
                              }
                             )",
                           typenames_, GetLanguage(), &error));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
  EXPECT_EQ(AidlError::BAD_TYPE, error);
}

TEST_P(AidlTest, BackingAnnotationRequireTypeParameter) {
  const string expected_stderr = "ERROR: Enum.aidl:1.1-9: Missing 'type' on @Backing.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("Enum.aidl", "@Backing enum Enum { FOO }", typenames_, GetLanguage()));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, SupportJavaOnlyImmutableAnnotation) {
  io_delegate_.SetFileContents("Foo.aidl",
                               "@JavaOnlyImmutable parcelable Foo { int a; Bar b; List<Bar> c; "
                               "Map<String, Baz> d; Bar[] e; }");
  io_delegate_.SetFileContents("Bar.aidl", "@JavaOnlyImmutable parcelable Bar { String a; }");
  io_delegate_.SetFileContents("Baz.aidl",
                               "@JavaOnlyImmutable @JavaOnlyStableParcelable parcelable Baz;");
  Options options = Options::From("aidl --lang=java -I . Foo.aidl");
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
}

TEST_F(AidlTest, RejectMutableParcelableFromJavaOnlyImmutableParcelable) {
  io_delegate_.SetFileContents("Foo.aidl", "@JavaOnlyImmutable parcelable Foo { Bar bar; }");
  io_delegate_.SetFileContents("Bar.aidl", "parcelable Bar { String a; }");
  string expected_error =
      "ERROR: Foo.aidl:1.40-44: The @JavaOnlyImmutable 'Foo' has a non-immutable field "
      "named 'bar'.\n";
  CaptureStderr();
  Options options = Options::From("aidl --lang=java Foo.aidl -I .");
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_error, GetCapturedStderr());
}

TEST_F(AidlTest, RejectMutableParcelableFromJavaOnlyImmutableUnion) {
  io_delegate_.SetFileContents("Foo.aidl", "@JavaOnlyImmutable union Foo { Bar bar; }");
  io_delegate_.SetFileContents("Bar.aidl", "parcelable Bar { String a; }");
  string expected_error =
      "ERROR: Foo.aidl:1.35-39: The @JavaOnlyImmutable 'Foo' has a non-immutable field "
      "named 'bar'.\n";
  CaptureStderr();
  Options options = Options::From("aidl --lang=java Foo.aidl -I .");
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_error, GetCapturedStderr());
}

TEST_F(AidlTest, ImmutableParcelableCannotBeInOut) {
  io_delegate_.SetFileContents("Foo.aidl", "@JavaOnlyImmutable parcelable Foo { int a; }");
  io_delegate_.SetFileContents("IBar.aidl", "interface IBar { void my(inout Foo foo); }");
  string expected_error =
      "ERROR: IBar.aidl:1.35-39: 'foo' can't be an inout parameter because @JavaOnlyImmutable can "
      "only be an in parameter.\n";
  CaptureStderr();
  Options options = Options::From("aidl --lang=java IBar.aidl -I .");
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_error, GetCapturedStderr());
}

TEST_F(AidlTest, ImmutableParcelableCannotBeOut) {
  io_delegate_.SetFileContents("Foo.aidl", "@JavaOnlyImmutable parcelable Foo { int a; }");
  io_delegate_.SetFileContents("IBar.aidl", "interface IBar { void my(out Foo foo); }");
  string expected_error =
      "ERROR: IBar.aidl:1.33-37: 'foo' can't be an out parameter because @JavaOnlyImmutable can "
      "only be an in parameter.\n";
  CaptureStderr();
  Options options = Options::From("aidl --lang=java IBar.aidl -I .");
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_error, GetCapturedStderr());
}

TEST_F(AidlTest, ImmutableParcelableFieldNameRestriction) {
  io_delegate_.SetFileContents("Foo.aidl", "@JavaOnlyImmutable parcelable Foo { int a; int A; }");
  Options options = Options::From("aidl --lang=java Foo.aidl");
  const string expected_stderr =
      "ERROR: Foo.aidl:1.47-49: 'Foo' has duplicate field name 'A' after capitalizing the first "
      "letter\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

const char kUnionExampleExpectedOutputCppHeader[] = R"(#pragma once

#include <a/ByteEnum.h>
#include <binder/Parcel.h>
#include <binder/ParcelFileDescriptor.h>
#include <binder/Status.h>
#include <cassert>
#include <codecvt>
#include <cstdint>
#include <locale>
#include <sstream>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#ifndef __BIONIC__
#define __assert2(a,b,c,d) ((void)0)
#endif

namespace a {

class Foo : public ::android::Parcelable {
public:
  inline bool operator!=(const Foo& rhs) const {
    return _value != rhs._value;
  }
  inline bool operator<(const Foo& rhs) const {
    return _value < rhs._value;
  }
  inline bool operator<=(const Foo& rhs) const {
    return _value <= rhs._value;
  }
  inline bool operator==(const Foo& rhs) const {
    return _value == rhs._value;
  }
  inline bool operator>(const Foo& rhs) const {
    return _value > rhs._value;
  }
  inline bool operator>=(const Foo& rhs) const {
    return _value >= rhs._value;
  }

  enum Tag : int32_t {
    ns = 0,  // int[] ns;
    e,  // a.ByteEnum e;
    pfd,  // ParcelFileDescriptor pfd;
  };

  template<typename _Tp>
  static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, Foo>;

  Foo() : _value(std::in_place_index<ns>, ::std::vector<int32_t>({42})) { }
  Foo(const Foo&) = default;
  Foo(Foo&&) = default;
  Foo& operator=(const Foo&) = default;
  Foo& operator=(Foo&&) = default;

  template <typename _Tp, typename = std::enable_if_t<_not_self<_Tp>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Foo(_Tp&& _arg)
      : _value(std::forward<_Tp>(_arg)) {}

  template <typename... _Tp>
  constexpr explicit Foo(_Tp&&... _args)
      : _value(std::forward<_Tp>(_args)...) {}

  template <Tag _tag, typename... _Tp>
  static Foo make(_Tp&&... _args) {
    return Foo(std::in_place_index<_tag>, std::forward<_Tp>(_args)...);
  }

  template <Tag _tag, typename _Tp, typename... _Up>
  static Foo make(std::initializer_list<_Tp> _il, _Up&&... _args) {
    return Foo(std::in_place_index<_tag>, std::move(_il), std::forward<_Up>(_args)...);
  }

  Tag getTag() const {
    return static_cast<Tag>(_value.index());
  }

  template <Tag _tag>
  const auto& get() const {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag>
  auto& get() {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag, typename... _Tp>
  void set(_Tp&&... _args) {
    _value.emplace<_tag>(std::forward<_Tp>(_args)...);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) override final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const override final;
  static const std::string& getParcelableDescriptor() {
    static const std::string DESCIPTOR = "a.Foo";
    return DESCIPTOR;
  }
  template <typename _T> class _has_toString {
    template <typename _U> static std::true_type __has_toString(decltype(&_U::toString));
    template <typename _U> static std::false_type __has_toString(...);
    public: enum { value = decltype(__has_toString<_T>(nullptr))::value };
  };
  template <typename _T> inline static std::string _call_toString(const _T& t) {
    if constexpr (_has_toString<_T>::value) return t.toString();
    return "{no toString() implemented}";
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "Foo{";
    switch (getTag()) {
    case ns: os << "ns: " << [&](){ std::ostringstream o; o << "["; bool first = true; for (const auto& v: get<ns>()) { (void)v; if (first) first = false; else o << ", "; o << std::to_string(v); }; o << "]"; return o.str(); }(); break;
    case e: os << "e: " << a::toString(get<e>()); break;
    case pfd: os << "pfd: " << ""; break;
    }
    os << "}";
    return os.str();
  }
private:
  std::variant<::std::vector<int32_t>, ::a::ByteEnum, ::android::os::ParcelFileDescriptor> _value;
};  // class Foo

}  // namespace a
)";

const char kUnionExampleExpectedOutputCppSource[] = R"(#include <a/Foo.h>

namespace a {

::android::status_t Foo::readFromParcel(const ::android::Parcel* _aidl_parcel) {
  ::android::status_t _aidl_ret_status;
  int32_t _aidl_tag;
  if ((_aidl_ret_status = _aidl_parcel->readInt32(&_aidl_tag)) != ::android::OK) return _aidl_ret_status;
  switch (_aidl_tag) {
  case ns: {
    ::std::vector<int32_t> _aidl_value;
    if ((_aidl_ret_status = _aidl_parcel->readInt32Vector(&_aidl_value)) != ::android::OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<::std::vector<int32_t>>) {
      set<ns>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<ns>(std::move(_aidl_value));
    }
    return ::android::OK; }
  case e: {
    ::a::ByteEnum _aidl_value;
    if ((_aidl_ret_status = _aidl_parcel->readByte(reinterpret_cast<int8_t *>(&_aidl_value))) != ::android::OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<::a::ByteEnum>) {
      set<e>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<e>(std::move(_aidl_value));
    }
    return ::android::OK; }
  case pfd: {
    ::android::os::ParcelFileDescriptor _aidl_value;
    if ((_aidl_ret_status = _aidl_parcel->readParcelable(&_aidl_value)) != ::android::OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<::android::os::ParcelFileDescriptor>) {
      set<pfd>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<pfd>(std::move(_aidl_value));
    }
    return ::android::OK; }
  }
  return ::android::BAD_VALUE;
}

::android::status_t Foo::writeToParcel(::android::Parcel* _aidl_parcel) const {
  ::android::status_t _aidl_ret_status = _aidl_parcel->writeInt32(getTag());
  if (_aidl_ret_status != ::android::OK) return _aidl_ret_status;
  switch (getTag()) {
  case ns: return _aidl_parcel->writeInt32Vector(get<ns>());
  case e: return _aidl_parcel->writeByte(static_cast<int8_t>(get<e>()));
  case pfd: return _aidl_parcel->writeParcelable(get<pfd>());
  }
  __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "can't reach here");
}

}  // namespace a
)";

const char kUnionExampleExpectedOutputNdkHeader[] = R"(#pragma once
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <codecvt>
#include <locale>
#include <sstream>

#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT
#include <aidl/a/ByteEnum.h>

#ifndef __BIONIC__
#define __assert2(a,b,c,d) ((void)0)
#endif

namespace aidl {
namespace a {
class Foo {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  enum Tag : int32_t {
    ns = 0,  // int[] ns;
    e,  // a.ByteEnum e;
    pfd,  // ParcelFileDescriptor pfd;
  };

  template<typename _Tp>
  static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, Foo>;

  Foo() : _value(std::in_place_index<ns>, std::vector<int32_t>({42})) { }
  Foo(const Foo&) = default;
  Foo(Foo&&) = default;
  Foo& operator=(const Foo&) = default;
  Foo& operator=(Foo&&) = default;

  template <typename _Tp, typename = std::enable_if_t<_not_self<_Tp>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Foo(_Tp&& _arg)
      : _value(std::forward<_Tp>(_arg)) {}

  template <typename... _Tp>
  constexpr explicit Foo(_Tp&&... _args)
      : _value(std::forward<_Tp>(_args)...) {}

  template <Tag _tag, typename... _Tp>
  static Foo make(_Tp&&... _args) {
    return Foo(std::in_place_index<_tag>, std::forward<_Tp>(_args)...);
  }

  template <Tag _tag, typename _Tp, typename... _Up>
  static Foo make(std::initializer_list<_Tp> _il, _Up&&... _args) {
    return Foo(std::in_place_index<_tag>, std::move(_il), std::forward<_Up>(_args)...);
  }

  Tag getTag() const {
    return static_cast<Tag>(_value.index());
  }

  template <Tag _tag>
  const auto& get() const {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag>
  auto& get() {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag, typename... _Tp>
  void set(_Tp&&... _args) {
    _value.emplace<_tag>(std::forward<_Tp>(_args)...);
  }

  binder_status_t readFromParcel(const AParcel* _parcel);
  binder_status_t writeToParcel(AParcel* _parcel) const;

  inline bool operator!=(const Foo& rhs) const {
    return _value != rhs._value;
  }
  inline bool operator<(const Foo& rhs) const {
    return _value < rhs._value;
  }
  inline bool operator<=(const Foo& rhs) const {
    return _value <= rhs._value;
  }
  inline bool operator==(const Foo& rhs) const {
    return _value == rhs._value;
  }
  inline bool operator>(const Foo& rhs) const {
    return _value > rhs._value;
  }
  inline bool operator>=(const Foo& rhs) const {
    return _value >= rhs._value;
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
  template <typename _T> class _has_toString {
    template <typename _U> static std::true_type __has_toString(decltype(&_U::toString));
    template <typename _U> static std::false_type __has_toString(...);
    public: enum { value = decltype(__has_toString<_T>(nullptr))::value };
  };
  template <typename _T> inline static std::string _call_toString(const _T& t) {
    if constexpr (_has_toString<_T>::value) return t.toString();
    return "{no toString() implemented}";
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "Foo{";
    switch (getTag()) {
    case ns: os << "ns: " << [&](){ std::ostringstream o; o << "["; bool first = true; for (const auto& v: get<ns>()) { (void)v; if (first) first = false; else o << ", "; o << std::to_string(v); }; o << "]"; return o.str(); }(); break;
    case e: os << "e: " << a::toString(get<e>()); break;
    case pfd: os << "pfd: " << ""; break;
    }
    os << "}";
    return os.str();
  }
private:
  std::variant<std::vector<int32_t>, ::aidl::a::ByteEnum, ::ndk::ScopedFileDescriptor> _value;
};
}  // namespace a
}  // namespace aidl
)";

const char kUnionExampleExpectedOutputNdkSource[] = R"(#include "aidl/a/Foo.h"

#include <android/binder_parcel_utils.h>

namespace aidl {
namespace a {
const char* Foo::descriptor = "a.Foo";

binder_status_t Foo::readFromParcel(const AParcel* _parcel) {
  binder_status_t _aidl_ret_status;
  int32_t _aidl_tag;
  if ((_aidl_ret_status = AParcel_readInt32(_parcel, &_aidl_tag)) != STATUS_OK) return _aidl_ret_status;
  switch (_aidl_tag) {
  case ns: {
    std::vector<int32_t> _aidl_value;
    if ((_aidl_ret_status = ::ndk::AParcel_readVector(_parcel, &_aidl_value)) != STATUS_OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<std::vector<int32_t>>) {
      set<ns>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<ns>(std::move(_aidl_value));
    }
    return STATUS_OK; }
  case e: {
    ::aidl::a::ByteEnum _aidl_value;
    if ((_aidl_ret_status = AParcel_readByte(_parcel, reinterpret_cast<int8_t*>(&_aidl_value))) != STATUS_OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<::aidl::a::ByteEnum>) {
      set<e>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<e>(std::move(_aidl_value));
    }
    return STATUS_OK; }
  case pfd: {
    ::ndk::ScopedFileDescriptor _aidl_value;
    if ((_aidl_ret_status = ::ndk::AParcel_readRequiredParcelFileDescriptor(_parcel, &_aidl_value)) != STATUS_OK) return _aidl_ret_status;
    if constexpr (std::is_trivially_copyable_v<::ndk::ScopedFileDescriptor>) {
      set<pfd>(_aidl_value);
    } else {
      // NOLINTNEXTLINE(performance-move-const-arg)
      set<pfd>(std::move(_aidl_value));
    }
    return STATUS_OK; }
  }
  return STATUS_BAD_VALUE;
}
binder_status_t Foo::writeToParcel(AParcel* _parcel) const {
  binder_status_t _aidl_ret_status = AParcel_writeInt32(_parcel, getTag());
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;
  switch (getTag()) {
  case ns: return ::ndk::AParcel_writeVector(_parcel, get<ns>());
  case e: return AParcel_writeByte(_parcel, static_cast<int8_t>(get<e>()));
  case pfd: return ::ndk::AParcel_writeRequiredParcelFileDescriptor(_parcel, get<pfd>());
  }
  __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "can't reach here");
}

}  // namespace a
}  // namespace aidl
)";

const char kUnionExampleExpectedOutputJava[] = R"(/*
 * This file is auto-generated.  DO NOT MODIFY.
 */
package a;


public final class Foo implements android.os.Parcelable {
  // tags for union fields
  public final static int ns = 0;  // int[] ns;
  public final static int e = 1;  // a.ByteEnum e;
  public final static int pfd = 2;  // ParcelFileDescriptor pfd;

  private int _tag;
  private Object _value;

  public Foo() {
    int[] _value = {42};
    this._tag = ns;
    this._value = _value;
  }

  private Foo(android.os.Parcel _aidl_parcel) {
    readFromParcel(_aidl_parcel);
  }

  private Foo(int _tag, Object _value) {
    this._tag = _tag;
    this._value = _value;
  }

  public int getTag() {
    return _tag;
  }

  // int[] ns;

  public static Foo ns(int[] _value) {
    return new Foo(ns, _value);
  }

  public int[] getNs() {
    _assertTag(ns);
    return (int[]) _value;
  }

  public void setNs(int[] _value) {
    _set(ns, _value);
  }

  // a.ByteEnum e;

  public static Foo e(byte _value) {
    return new Foo(e, _value);
  }

  public byte getE() {
    _assertTag(e);
    return (byte) _value;
  }

  public void setE(byte _value) {
    _set(e, _value);
  }

  // ParcelFileDescriptor pfd;

  public static Foo pfd(android.os.ParcelFileDescriptor _value) {
    return new Foo(pfd, _value);
  }

  public android.os.ParcelFileDescriptor getPfd() {
    _assertTag(pfd);
    return (android.os.ParcelFileDescriptor) _value;
  }

  public void setPfd(android.os.ParcelFileDescriptor _value) {
    _set(pfd, _value);
  }

  public static final android.os.Parcelable.Creator<Foo> CREATOR = new android.os.Parcelable.Creator<Foo>() {
    @Override
    public Foo createFromParcel(android.os.Parcel _aidl_source) {
      return new Foo(_aidl_source);
    }
    @Override
    public Foo[] newArray(int _aidl_size) {
      return new Foo[_aidl_size];
    }
  };

  @Override
  public final void writeToParcel(android.os.Parcel _aidl_parcel, int _aidl_flag) {
    _aidl_parcel.writeInt(_tag);
    switch (_tag) {
    case ns:
      _aidl_parcel.writeIntArray(getNs());
      break;
    case e:
      _aidl_parcel.writeByte(getE());
      break;
    case pfd:
      if ((getPfd()!=null)) {
        _aidl_parcel.writeInt(1);
        getPfd().writeToParcel(_aidl_parcel, 0);
      }
      else {
        _aidl_parcel.writeInt(0);
      }
      break;
    }
  }

  public void readFromParcel(android.os.Parcel _aidl_parcel) {
    int _aidl_tag;
    _aidl_tag = _aidl_parcel.readInt();
    switch (_aidl_tag) {
    case ns: {
      int[] _aidl_value;
      _aidl_value = _aidl_parcel.createIntArray();
      _set(_aidl_tag, _aidl_value);
      return; }
    case e: {
      byte _aidl_value;
      _aidl_value = _aidl_parcel.readByte();
      _set(_aidl_tag, _aidl_value);
      return; }
    case pfd: {
      android.os.ParcelFileDescriptor _aidl_value;
      if ((0!=_aidl_parcel.readInt())) {
        _aidl_value = android.os.ParcelFileDescriptor.CREATOR.createFromParcel(_aidl_parcel);
      }
      else {
        _aidl_value = null;
      }
      _set(_aidl_tag, _aidl_value);
      return; }
    }
    throw new IllegalArgumentException("union: unknown tag: " + _aidl_tag);
  }

  @Override
  public int describeContents() {
    int _mask = 0;
    switch (getTag()) {
    case pfd:
      _mask |= describeContents(getPfd());
      break;
    }
    return _mask;
  }
  private int describeContents(Object _v) {
    if (_v == null) return 0;
    if (_v instanceof android.os.Parcelable) {
      return ((android.os.Parcelable) _v).describeContents();
    }
    return 0;
  }

  private void _assertTag(int tag) {
    if (getTag() != tag) {
      throw new IllegalStateException("bad access: " + _tagString(tag) + ", " + _tagString(getTag()) + " is available.");
    }
  }

  private String _tagString(int _tag) {
    switch (_tag) {
    case ns: return "ns";
    case e: return "e";
    case pfd: return "pfd";
    }
    throw new IllegalStateException("unknown field: " + _tag);
  }

  private void _set(int _tag, Object _value) {
    this._tag = _tag;
    this._value = _value;
  }
}
)";

const char kUnionExampleExpectedOutputRust[] = R"(#[derive(Debug)]
pub enum Foo {
  Ns(Vec<i32>),
  E(crate::mangled::_1_a_8_ByteEnum),
  Pfd(Option<binder::parcel::ParcelFileDescriptor>),
}
pub(crate) mod mangled { pub use super::Foo as _1_a_3_Foo; }
impl Default for Foo {
  fn default() -> Self {
    Self::Ns(vec!{42})
  }
}
impl binder::parcel::Serialize for Foo {
  fn serialize(&self, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    <Self as binder::parcel::SerializeOption>::serialize_option(Some(self), parcel)
  }
}
impl binder::parcel::SerializeArray for Foo {}
impl binder::parcel::SerializeOption for Foo {
  fn serialize_option(this: Option<&Self>, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    let this = if let Some(this) = this {
      parcel.write(&1i32)?;
      this
    } else {
      return parcel.write(&0i32);
    };
    match this {
      Self::Ns(v) => {
        parcel.write(&0i32)?;
        parcel.write(v)
      }
      Self::E(v) => {
        parcel.write(&1i32)?;
        parcel.write(v)
      }
      Self::Pfd(v) => {
        parcel.write(&2i32)?;
        let __field_ref = v.as_ref().ok_or(binder::StatusCode::UNEXPECTED_NULL)?;
        parcel.write(__field_ref)
      }
    }
  }
}
impl binder::parcel::Deserialize for Foo {
  fn deserialize(parcel: &binder::parcel::Parcel) -> binder::Result<Self> {
    <Self as binder::parcel::DeserializeOption>::deserialize_option(parcel)
       .transpose()
       .unwrap_or(Err(binder::StatusCode::UNEXPECTED_NULL))
  }
}
impl binder::parcel::DeserializeArray for Foo {}
impl binder::parcel::DeserializeOption for Foo {
  fn deserialize_option(parcel: &binder::parcel::Parcel) -> binder::Result<Option<Self>> {
    let status: i32 = parcel.read()?;
    if status == 0 { return Ok(None); }
    let tag: i32 = parcel.read()?;
    match tag {
      0 => {
        let value: Vec<i32> = parcel.read()?;
        Ok(Some(Self::Ns(value)))
      }
      1 => {
        let value: crate::mangled::_1_a_8_ByteEnum = parcel.read()?;
        Ok(Some(Self::E(value)))
      }
      2 => {
        let value: Option<binder::parcel::ParcelFileDescriptor> = Some(parcel.read()?);
        Ok(Some(Self::Pfd(value)))
      }
      _ => {
        Err(binder::StatusCode::BAD_VALUE)
      }
    }
  }
}
)";

struct AidlUnionTest : ::testing::Test {
  void SetUp() override {
    io_delegate_.SetFileContents("a/Foo.aidl", R"(
package a;
import a.ByteEnum;
union Foo {
  int[] ns = {42};
  ByteEnum  e;
  ParcelFileDescriptor pfd;
}
)");
    io_delegate_.SetFileContents("a/ByteEnum.aidl", R"(
package a;
@Backing(type="byte")
enum ByteEnum {
  a, b, c
}
)");
  }
  FakeIoDelegate io_delegate_;
  void Compile(string lang) {
    string command = "aidl --structured -I. -o out --lang=" + lang;
    if (lang == "cpp" || lang == "ndk") {
      command += " -h out";
    }
    command += " a/Foo.aidl";
    auto opts = Options::From(command);
    CaptureStderr();
    auto ret = ::android::aidl::compile_aidl(opts, io_delegate_);
    auto err = GetCapturedStderr();
    EXPECT_EQ(0, ret) << err;
  }
  void EXPECT_COMPILE_OUTPUTS(const map<string, string>& outputs) {
    for (const auto& [path, expected] : outputs) {
      string actual;
      EXPECT_TRUE(io_delegate_.GetWrittenContents(path, &actual)) << path << " not found.";
      EXPECT_EQ(expected, actual);
    }
  }
};

TEST_F(AidlUnionTest, Example_Cpp) {
  Compile("cpp");
  EXPECT_COMPILE_OUTPUTS(
      map<string, string>({{"out/a/Foo.cpp", kUnionExampleExpectedOutputCppSource},
                           {"out/a/Foo.h", kUnionExampleExpectedOutputCppHeader}}));
}

TEST_F(AidlUnionTest, Example_Ndk) {
  Compile("ndk");
  EXPECT_COMPILE_OUTPUTS(
      map<string, string>({{"out/a/Foo.cpp", kUnionExampleExpectedOutputNdkSource},
                           {"out/aidl/a/Foo.h", kUnionExampleExpectedOutputNdkHeader}}));
}

TEST_F(AidlUnionTest, Example_Java) {
  Compile("java");
  EXPECT_COMPILE_OUTPUTS(
      map<string, string>({{"out/a/Foo.java", kUnionExampleExpectedOutputJava}}));
}

TEST_F(AidlUnionTest, Example_Rust) {
  Compile("rust");
  EXPECT_COMPILE_OUTPUTS(map<string, string>({{"out/a/Foo.rs", kUnionExampleExpectedOutputRust}}));
}

TEST_P(AidlTest, UnionRejectsEmptyDecl) {
  const string method = "package a; union Foo {}";
  const string expected_stderr = "ERROR: a/Foo.aidl:1.17-21: The union 'Foo' has no fields.\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", method, typenames_, GetLanguage()));
  EXPECT_THAT(GetCapturedStderr(), testing::HasSubstr(expected_stderr));
}

TEST_P(AidlTest, UnionRejectsParcelableHolder) {
  const string method = "package a; union Foo { ParcelableHolder x; }";
  const string expected_stderr =
      "ERROR: a/Foo.aidl:1.40-42: A union can't have a member of ParcelableHolder 'x'\n";
  CaptureStderr();
  EXPECT_EQ(nullptr, Parse("a/Foo.aidl", method, typenames_, GetLanguage()));
  EXPECT_THAT(GetCapturedStderr(), testing::HasSubstr(expected_stderr));
}

TEST_P(AidlTest, UnionRejectsFirstEnumWithNoDefaults) {
  import_paths_.insert(".");
  io_delegate_.SetFileContents("a/Enum.aidl", "package a; enum Enum { FOO, BAR }");
  const string expected_err = "The union's first member should have a useful default value.";
  CaptureStderr();
  EXPECT_EQ(nullptr,
            Parse("a/Foo.aidl", "package a; union Foo { a.Enum e; }", typenames_, GetLanguage()));
  EXPECT_THAT(GetCapturedStderr(), testing::HasSubstr(expected_err));
}

TEST_P(AidlTest, GenericStructuredParcelable) {
  io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo<T, U> { int a; int A; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  const string expected_stderr = "";
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, GenericStructuredParcelableWithStringConstants_Cpp) {
  io_delegate_.SetFileContents("Foo.aidl",
                               "parcelable Foo<T, U> { int a; const String s = \"\"; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(Options::Language::CPP) +
                    " -o out -h out");
  const string expected_stderr = "";
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr(R"--(template <typename T, typename U>
const ::android::String16& Foo<T,U>::s() {
  static const ::android::String16 value(::android::String16(""));
  return value;
})--"));
}

TEST_F(AidlTest, GenericStructuredParcelableWithStringConstants_Ndk) {
  io_delegate_.SetFileContents("Foo.aidl",
                               "parcelable Foo<T, U> { int a; const String s = \"\"; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(Options::Language::NDK) +
                    " -o out -h out");
  const string expected_stderr = "";
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/aidl/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr(R"--(template <typename T, typename U>
const char* Foo<T, U>::s = "";
)--"));
}

TEST_F(AidlTest, NestedTypeArgs) {
  io_delegate_.SetFileContents("a/Bar.aidl", "package a; parcelable Bar<A> { }");
  io_delegate_.SetFileContents("a/Baz.aidl", "package a; parcelable Baz<A, B> { }");

  io_delegate_.SetFileContents("a/Foo.aidl",
                               "package a; import a.Bar; import a.Baz; parcelable Foo { "
                               "Baz<Bar<Bar<String[]>>[], Bar<String>> barss; }");
  Options options = Options::From("aidl a/Foo.aidl -I . -o out --lang=java");
  const string expected_stderr = "";
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/a/Foo.java", &code));
  EXPECT_THAT(code,
              testing::HasSubstr(
                  "a.Baz<a.Bar<a.Bar<java.lang.String[]>>[],a.Bar<java.lang.String>> barss;"));
}

TEST_F(AidlTest, DoubleArrayError) {
  io_delegate_.SetFileContents("a/Bar.aidl", "package a; parcelable Bar { String[][] a; }");

  Options options = Options::From("aidl a/Bar.aidl -I . -o out --lang=java");
  const string expected_stderr =
      "ERROR: a/Bar.aidl:1.28-37: Can only have one dimensional arrays.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, DoubleGenericError) {
  io_delegate_.SetFileContents("a/Bar.aidl",
                               "package a; parcelable Bar { List<String><String> a; }");

  Options options = Options::From("aidl a/Bar.aidl -I . -o out --lang=java");
  const string expected_stderr =
      "ERROR: a/Bar.aidl:1.28-33: Can only specify one set of type parameters.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_F(AidlTest, ArrayBeforeGenericError) {
  io_delegate_.SetFileContents("a/Bar.aidl", "package a; parcelable Bar { List[]<String> a; }");

  Options options = Options::From("aidl a/Bar.aidl -I . -o out --lang=java");
  const string expected_stderr =
      "ERROR: a/Bar.aidl:1.28-33: Must specify type parameters (<>) before array ([]).\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

struct GenericAidlTest : ::testing::Test {
  FakeIoDelegate io_delegate_;
  void Compile(string cmd) {
    io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo { Bar<Baz<Qux>> x; }");
    io_delegate_.SetFileContents("Bar.aidl", "parcelable Bar<T> {  }");
    io_delegate_.SetFileContents("Baz.aidl", "parcelable Baz<T> {  }");
    io_delegate_.SetFileContents("Qux.aidl", "parcelable Qux {  }");

    Options options = Options::From(cmd);
    CaptureStderr();
    EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
    EXPECT_EQ("", GetCapturedStderr());
  }
};

TEST_F(GenericAidlTest, ImportGenericParameterTypesCPP) {
  Compile("aidl Foo.aidl --lang=cpp -I . -o out -h out");
  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr("#include <Bar.h>"));
  EXPECT_THAT(code, testing::HasSubstr("#include <Baz.h>"));
  EXPECT_THAT(code, testing::HasSubstr("#include <Qux.h>"));
}

TEST_F(GenericAidlTest, ImportGenericParameterTypesNDK) {
  Compile("aidl Foo.aidl --lang=ndk -I . -o out -h out");
  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/aidl/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr("#include <aidl/Bar.h>"));
  EXPECT_THAT(code, testing::HasSubstr("#include <aidl/Baz.h>"));
  EXPECT_THAT(code, testing::HasSubstr("#include <aidl/Qux.h>"));
}

TEST_P(AidlTest, RejectGenericStructuredParcelabelRepeatedParam) {
  io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo<T,T> { int a; int A; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  const string expected_stderr =
      "ERROR: Foo.aidl:1.11-15: Every type parameter should be unique.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, RejectGenericStructuredParcelableField) {
  io_delegate_.SetFileContents("Foo.aidl", "parcelable Foo<T,T> { T a; int A; }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  const string expected_stderr = "ERROR: Foo.aidl:1.22-24: Failed to resolve 'T'\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expected_stderr, GetCapturedStderr());
}

TEST_P(AidlTest, LongCommentWithinConstExpression) {
  io_delegate_.SetFileContents("Foo.aidl", "enum Foo { FOO = (1 << 1) /* comment */ | 0x0 }");
  Options options =
      Options::From("aidl Foo.aidl --lang=" + Options::LanguageToString(GetLanguage()));
  CaptureStderr();
  EXPECT_EQ(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
}

TEST_F(AidlTest, RejectUntypdeListAndMapInUnion) {
  io_delegate_.SetFileContents("a/Foo.aidl", "package a; union Foo { List l; Map m; }");
  Options options = Options::From("aidl a/Foo.aidl --lang=java -o out");
  std::string expectedErr =
      "ERROR: a/Foo.aidl:1.28-30: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n"
      "ERROR: a/Foo.aidl:1.35-37: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expectedErr, GetCapturedStderr());
}

TEST_F(AidlTest, RejectUntypdeListAndMapInUnstructuredParcelable) {
  io_delegate_.SetFileContents("a/Foo.aidl", "package a; parcelable Foo { List l; Map m; }");
  Options options = Options::From("aidl a/Foo.aidl --lang=java -o out");
  std::string expectedErr =
      "ERROR: a/Foo.aidl:1.33-35: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n"
      "ERROR: a/Foo.aidl:1.40-42: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expectedErr, GetCapturedStderr());
}

TEST_F(AidlTest, RejectNestedUntypedListAndMap) {
  io_delegate_.SetFileContents("a/Bar.aidl", "package a; parcelable Bar<T>;");
  io_delegate_.SetFileContents(
      "a/Foo.aidl", "package a; import a.Bar; parcelable Foo { Bar<List> a; Bar<Map> b; }");
  Options options = Options::From("aidl a/Foo.aidl -I . --lang=java -o out");
  std::string expectedErr =
      "ERROR: a/Foo.aidl:1.52-54: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n"
      "ERROR: a/Foo.aidl:1.64-66: "
      "Encountered an untyped List or Map. The use of untyped List/Map is "
      "prohibited because it is not guaranteed that the objects in the list are recognizable in "
      "the receiving side. Consider switching to an array or a generic List/Map.\n";
  CaptureStderr();
  EXPECT_NE(0, ::android::aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ(expectedErr, GetCapturedStderr());
}

TEST_F(AidlTest, EnumWithDefaults_Java) {
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO, BAR }");
  io_delegate_.SetFileContents("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  Enum e = Enum.BAR;
})");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang java -o out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/p/Foo.java", &code));
  EXPECT_THAT(code, testing::HasSubstr("byte e = p.Enum.BAR"));
}

TEST_F(AidlTest, EnumWithDefaults_Cpp) {
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO, BAR }");
  io_delegate_.SetFileContents("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  Enum e = Enum.BAR;
})");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang cpp -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/p/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr("::p::Enum e = ::p::Enum(::p::Enum::BAR);"));
}

TEST_F(AidlTest, EnumWithDefaults_Ndk) {
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO, BAR }");
  io_delegate_.SetFileContents("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  Enum e = Enum.BAR;
})");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang ndk -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/aidl/p/Foo.h", &code));
  EXPECT_THAT(code, testing::HasSubstr("::aidl::p::Enum e = ::aidl::p::Enum::BAR;"));
}

TEST_F(AidlTest, EnumWithDefaults_Rust) {
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO, BAR }");
  io_delegate_.SetFileContents("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  int  n = 42;
  Enum e = Enum.BAR;
})");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang rust -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);

  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/p/Foo.rs", &code));
  EXPECT_THAT(code, testing::HasSubstr(R"(
  fn default() -> Self {
    Self {
      n: 42,
      e: crate::mangled::_1_p_4_Enum::BAR,
    }
  })"));
}

TEST_P(AidlTest, EnumeratorIsConstantValue_DefaultValue) {
  import_paths_.insert("a");
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO = 1, BAR = 2}");
  CaptureStderr();
  const AidlDefinedType* type = Parse("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  int e = Enum.FOO | Enum.BAR;
})",
                                      typenames_, GetLanguage());
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);
  EXPECT_TRUE(type);
  const auto& fields = type->AsStructuredParcelable()->GetFields();
  EXPECT_EQ("int e = 3", fields[0]->ToString());
}

TEST_P(AidlTest, EnumeratorIsConstantValue_CanDefineOtherEnumerator) {
  CaptureStderr();
  const AidlDefinedType* type = Parse("a/p/Foo.aidl", R"(
@Backing(type="int")
enum Foo {
      STANDARD_SHIFT = 16,
      STANDARD_BT709 = 1 << STANDARD_SHIFT,
      STANDARD_BT601_625 = 2 << STANDARD_SHIFT,
}
)",
                                      typenames_, GetLanguage());
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);
  EXPECT_TRUE(type);
  const auto& enum_type = type->AsEnumDeclaration();
  string code;
  enum_type->Dump(CodeWriter::ForString(&code).get());
  EXPECT_EQ(R"--(@Backing(type="int")
enum Foo {
  STANDARD_SHIFT = 16,
  STANDARD_BT709 = 65536,
  STANDARD_BT601_625 = 131072,
}
)--",
            code);
}

TEST_F(AidlTest, EnumDefaultShouldBeEnumerators) {
  io_delegate_.SetFileContents("a/p/Enum.aidl", "package p; enum Enum { FOO = 1, BAR = 2}");
  io_delegate_.SetFileContents("a/p/Foo.aidl", R"(
package p;
import p.Enum;
parcelable Foo {
  Enum e = Enum.FOO | Enum.BAR;
})");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang java -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("ERROR: a/p/Foo.aidl:5.11-20: Invalid value (Enum.FOO|Enum.BAR) for enum p.Enum\n",
            err);
}

TEST_P(AidlTest, DefaultWithEmptyArray) {
  io_delegate_.SetFileContents("a/p/Foo.aidl", "package p; parcelable Foo { p.Bar[] bars = {}; }");
  io_delegate_.SetFileContents("a/p/Bar.aidl", "package p; parcelable Bar { }");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang " + Options::LanguageToString(GetLanguage()) +
                               " -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);
}

TEST_P(AidlTest, RejectRefsInAnnotation) {
  io_delegate_.SetFileContents("a/p/IFoo.aidl",
                               "package p; interface IFoo {\n"
                               "  const String ANNOTATION = \"@Annotation\";\n"
                               "  @JavaPassthrough(annotation=ANNOTATION) void foo();\n"
                               "}");
  CaptureStderr();
  auto options = Options::From("aidl --lang " + Options::LanguageToString(GetLanguage()) +
                               " -o out -h out a/p/IFoo.aidl");
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ(
      "ERROR: a/p/IFoo.aidl:3.31-41: Value must be a constant expression but contains reference to "
      "ANNOTATION.\n",
      err);
}

TEST_F(AidlTest, DefaultWithEnumValues) {
  io_delegate_.SetFileContents(
      "a/p/Foo.aidl",
      "package p; import p.Bar; parcelable Foo { Bar[] bars = { Bar.FOO, Bar.FOO }; }");
  io_delegate_.SetFileContents("a/p/Bar.aidl", "package p; enum Bar { FOO, BAR }");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang ndk -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("", err);
  string code;
  EXPECT_TRUE(io_delegate_.GetWrittenContents("out/aidl/p/Foo.h", &code));
  EXPECT_THAT(
      code, testing::HasSubstr(
                "std::vector<::aidl::p::Bar> bars = {::aidl::p::Bar::FOO, ::aidl::p::Bar::FOO};"));
}

TEST_F(AidlTest, RejectsCircularReferencingEnumerators) {
  io_delegate_.SetFileContents("a/p/Foo.aidl", "package p; enum Foo { A = B, B }");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang ndk -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ(
      "ERROR: a/p/Foo.aidl:1.26-28: Found a circular reference: B -> A -> B\n"
      "ERROR: a/p/Foo.aidl:1.29-31: Found a circular reference: A -> B -> A\n",
      err);
}

TEST_F(AidlTest, RejectsCircularReferencingConsts) {
  io_delegate_.SetFileContents("a/p/Foo.aidl",
                               "package p; parcelable Foo { const int A = A + 1; }");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang ndk -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  auto err = GetCapturedStderr();
  EXPECT_EQ("ERROR: a/p/Foo.aidl:1.42-44: Found a circular reference: A -> A\n", err);
}

TEST_F(AidlTest, RecursiveReferences) {
  io_delegate_.SetFileContents("a/p/Foo.aidl",
                               "package p; parcelable Foo { const int A = p.Bar.A + 1; }");
  io_delegate_.SetFileContents("a/p/Bar.aidl",
                               "package p; parcelable Bar { const int A = p.Baz.A + 1; }");
  io_delegate_.SetFileContents("a/p/Baz.aidl", "package p; parcelable Baz { const int A = 1; }");
  CaptureStderr();
  auto options = Options::From("aidl -I a --lang ndk -o out -h out a/p/Foo.aidl");
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("", GetCapturedStderr());
}

TEST_P(AidlTest, JavaCompatibleBuiltinTypes) {
  string contents = R"(
import android.os.IBinder;
import android.os.IInterface;
interface IFoo {}
  )";
  EXPECT_NE(nullptr, Parse("IFoo.aidl", contents, typenames_, GetLanguage()));
}

TEST_P(AidlTest, WarningInterfaceName) {
  io_delegate_.SetFileContents("p/Foo.aidl", "interface Foo {}");
  auto options = Options::From("aidl --lang " + Options::LanguageToString(GetLanguage()) +
                               " -Weverything -o out -h out p/Foo.aidl");
  CaptureStderr();
  EXPECT_EQ(0, aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("WARNING: p/Foo.aidl:1.1-10: Interface names should start with I.\n",
            GetCapturedStderr());
}

TEST_P(AidlTest, ErrorInterfaceName) {
  io_delegate_.SetFileContents("p/Foo.aidl", "interface Foo {}");
  auto options = Options::From("aidl --lang " + Options::LanguageToString(GetLanguage()) +
                               " -Weverything -Werror -o out -h out p/Foo.aidl");
  CaptureStderr();
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("ERROR: p/Foo.aidl:1.1-10: Interface names should start with I.\n",
            GetCapturedStderr());
}

TEST_P(AidlTest, ErrorEnumZero) {
  io_delegate_.SetFileContents("p/Foo.aidl", "enum Foo { FOO = 1 }");
  auto options = Options::From("aidl --lang " + Options::LanguageToString(GetLanguage()) +
                               " -Weverything -Werror -o out -h out p/Foo.aidl");
  CaptureStderr();
  EXPECT_EQ(1, aidl::compile_aidl(options, io_delegate_));
  EXPECT_EQ("ERROR: p/Foo.aidl:1.11-15: The first enumerator 'FOO' should be 0, but it is 1.\n",
            GetCapturedStderr());
}

struct TypeParam {
  string kind;
  string literal;
};

const TypeParam kTypeParams[] = {
    {"primitive", "int"},   {"String", "String"},
    {"IBinder", "IBinder"}, {"ParcelFileDescriptor", "ParcelFileDescriptor"},
    {"parcelable", "Foo"},  {"enum", "a.Enum"},
    {"union", "a.Union"},   {"interface", "a.IBar"},
};

const std::map<std::string, std::string> kListSupportExpectations = {
    {"cpp_primitive", "A generic type cannot have any primitive type parameters."},
    {"java_primitive", "A generic type cannot have any primitive type parameters."},
    {"ndk_primitive", "A generic type cannot have any primitive type parameters."},
    {"rust_primitive", "A generic type cannot have any primitive type parameters."},
    {"cpp_String", ""},
    {"java_String", ""},
    {"ndk_String", ""},
    {"rust_String", ""},
    {"cpp_IBinder", ""},
    {"java_IBinder", ""},
    {"ndk_IBinder", "List<IBinder> is not supported. List in NDK doesn't support IBinder."},
    {"rust_IBinder", ""},
    {"cpp_ParcelFileDescriptor", ""},
    {"java_ParcelFileDescriptor", ""},
    {"ndk_ParcelFileDescriptor", ""},
    {"rust_ParcelFileDescriptor", ""},
    {"cpp_interface", "List<a.IBar> is not supported."},
    {"java_interface", "List<a.IBar> is not supported."},
    {"ndk_interface", "List<a.IBar> is not supported."},
    {"rust_interface", "List<a.IBar> is not supported."},
    {"cpp_parcelable", ""},
    {"java_parcelable", ""},
    {"ndk_parcelable", ""},
    {"rust_parcelable", ""},
    {"cpp_enum", "A generic type cannot have any primitive type parameters."},
    {"java_enum", "A generic type cannot have any primitive type parameters."},
    {"ndk_enum", "A generic type cannot have any primitive type parameters."},
    {"rust_enum", "A generic type cannot have any primitive type parameters."},
    {"cpp_union", ""},
    {"java_union", ""},
    {"ndk_union", ""},
    {"rust_union", ""},
};

const std::map<std::string, std::string> kArraySupportExpectations = {
    {"cpp_primitive", ""},
    {"java_primitive", ""},
    {"ndk_primitive", ""},
    {"rust_primitive", ""},
    {"cpp_String", ""},
    {"java_String", ""},
    {"ndk_String", ""},
    {"rust_String", ""},
    {"cpp_IBinder", ""},
    {"java_IBinder", ""},
    {"ndk_IBinder", "The ndk backend does not support array of IBinder"},
    {"rust_IBinder", "The rust backend does not support array of IBinder"},
    {"cpp_ParcelFileDescriptor", ""},
    {"java_ParcelFileDescriptor", ""},
    {"ndk_ParcelFileDescriptor", ""},
    {"rust_ParcelFileDescriptor", ""},
    {"cpp_interface", "Binder type cannot be an array"},
    {"java_interface", "Binder type cannot be an array"},
    {"ndk_interface", "Binder type cannot be an array"},
    {"rust_interface", "Binder type cannot be an array"},
    {"cpp_parcelable", ""},
    {"java_parcelable", ""},
    {"ndk_parcelable", ""},
    {"rust_parcelable", ""},
    {"cpp_enum", ""},
    {"java_enum", ""},
    {"ndk_enum", ""},
    {"rust_enum", ""},
    {"cpp_union", ""},
    {"java_union", ""},
    {"ndk_union", ""},
    {"rust_union", ""},
};

class AidlTypeParamTest : public testing::TestWithParam<std::tuple<Options::Language, TypeParam>> {
 public:
  void Run(const std::string& generic_type_decl,
           const std::map<std::string, std::string>& expectations) {
    const auto& param = GetParam();
    const auto& lang = Options::LanguageToString(std::get<0>(param));
    const auto& kind = std::get<1>(param).kind;

    FakeIoDelegate io;
    io.SetFileContents("a/IBar.aidl", "package a; interface IBar { }");
    io.SetFileContents("a/Enum.aidl", "package a; enum Enum { A }");
    io.SetFileContents("a/Union.aidl", "package a; union Union { int a; }");
    std::string decl = fmt::format(generic_type_decl, std::get<1>(param).literal);
    io.SetFileContents("a/Foo.aidl", "package a; parcelable Foo { " + decl + " f; }");

    const auto options =
        Options::From(fmt::format("aidl -I . --lang={} a/Foo.aidl -o out -h out", lang));
    CaptureStderr();
    compile_aidl(options, io);
    auto it = expectations.find(lang + "_" + kind);
    EXPECT_TRUE(it != expectations.end());
    const string err = GetCapturedStderr();
    if (it->second.empty()) {
      EXPECT_EQ("", err);
    } else {
      EXPECT_THAT(err, testing::HasSubstr(it->second));
    }
  }
};

INSTANTIATE_TEST_SUITE_P(
    AidlTestSuite, AidlTypeParamTest,
    testing::Combine(testing::Values(Options::Language::CPP, Options::Language::JAVA,
                                     Options::Language::NDK, Options::Language::RUST),
                     testing::ValuesIn(kTypeParams)),
    [](const testing::TestParamInfo<std::tuple<Options::Language, TypeParam>>& info) {
      return Options::LanguageToString(std::get<0>(info.param)) + "_" +
             std::get<1>(info.param).kind;
    });

TEST_P(AidlTypeParamTest, ListSupportedTypes) {
  Run("List<{}>", kListSupportExpectations);
}

TEST_P(AidlTypeParamTest, ArraySupportedTypes) {
  Run("{}[]", kArraySupportExpectations);
}

}  // namespace aidl
}  // namespace android

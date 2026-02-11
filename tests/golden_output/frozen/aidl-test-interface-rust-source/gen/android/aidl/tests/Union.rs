/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=rust -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen/android/aidl/tests/Union.rs.d -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/Union.aidl
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#![forbid(unsafe_code)]
#![cfg_attr(rustfmt, rustfmt_skip)]
use alloc::boxed::Box;
#[derive(Debug, Clone, PartialEq)]
pub enum r#Union {
  Ns(alloc::vec::Vec<i32>),
  N(i32),
  M(i32),
  S(alloc::string::String),
  Ibinder(Option<binder::SpIBinder>),
  Ss(alloc::vec::Vec<alloc::string::String>),
  Be(crate::mangled::_7_android_4_aidl_5_tests_8_ByteEnum),
}
pub const r#S1: &str = "a string constant in union";
impl Default for r#Union {
  fn default() -> Self {
    Self::Ns(alloc::vec![])
  }
}
impl binder::Parcelable for r#Union {
  fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> core::result::Result<(), binder::StatusCode> {
    match self {
      Self::Ns(v) => {
        parcel.write(&0i32)?;
        parcel.write(v)
      }
      Self::N(v) => {
        parcel.write(&1i32)?;
        parcel.write(v)
      }
      Self::M(v) => {
        parcel.write(&2i32)?;
        parcel.write(v)
      }
      Self::S(v) => {
        parcel.write(&3i32)?;
        parcel.write(v)
      }
      Self::Ibinder(v) => {
        parcel.write(&4i32)?;
        parcel.write(v)
      }
      Self::Ss(v) => {
        parcel.write(&5i32)?;
        parcel.write(v)
      }
      Self::Be(v) => {
        parcel.write(&6i32)?;
        parcel.write(v)
      }
    }
  }
  fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> core::result::Result<(), binder::StatusCode> {
    let tag: i32 = parcel.read()?;
    match tag {
      0 => {
        let value: alloc::vec::Vec<i32> = parcel.read()?;
        *self = Self::Ns(value);
        Ok(())
      }
      1 => {
        let value: i32 = parcel.read()?;
        *self = Self::N(value);
        Ok(())
      }
      2 => {
        let value: i32 = parcel.read()?;
        *self = Self::M(value);
        Ok(())
      }
      3 => {
        let value: alloc::string::String = parcel.read()?;
        *self = Self::S(value);
        Ok(())
      }
      4 => {
        let value: Option<binder::SpIBinder> = parcel.read()?;
        *self = Self::Ibinder(value);
        Ok(())
      }
      5 => {
        let value: alloc::vec::Vec<alloc::string::String> = parcel.read()?;
        *self = Self::Ss(value);
        Ok(())
      }
      6 => {
        let value: crate::mangled::_7_android_4_aidl_5_tests_8_ByteEnum = parcel.read()?;
        *self = Self::Be(value);
        Ok(())
      }
      _ => {
        Err(binder::StatusCode::BAD_VALUE)
      }
    }
  }
}
binder::impl_serialize_for_parcelable!(r#Union);
binder::impl_deserialize_for_parcelable!(r#Union);
impl binder::binder_impl::ParcelableMetadata for r#Union {
  fn get_descriptor() -> &'static str { "android.aidl.tests.Union" }
}
pub mod r#Tag {
  #![allow(non_upper_case_globals)]
  use binder::declare_binder_enum;
  declare_binder_enum! {
    #[repr(align(4))]
    r#Tag : [i32; 7] {
      r#ns = 0,
      r#n = 1,
      r#m = 2,
      r#s = 3,
      r#ibinder = 4,
      r#ss = 5,
      r#be = 6,
    }
  }
}
pub(crate) mod mangled {
 pub use super::r#Union as _7_android_4_aidl_5_tests_5_Union;
 pub use super::r#Tag::r#Tag as _7_android_4_aidl_5_tests_5_Union_3_Tag;
}

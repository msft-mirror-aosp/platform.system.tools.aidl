/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=rust -Weverything -Wno-missing-permission-annotation -Werror --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen/android/aidl/tests/OtherParcelableForToString.rs.d -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/OtherParcelableForToString.aidl
 */
#![forbid(unsafe_code)]
#![cfg_attr(rustfmt, rustfmt_skip)]
#[derive(Debug)]
pub struct r#OtherParcelableForToString {
  pub r#field: String,
}
impl Default for r#OtherParcelableForToString {
  fn default() -> Self {
    Self {
      r#field: Default::default(),
    }
  }
}
impl binder::Parcelable for r#OtherParcelableForToString {
  fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_write(|subparcel| {
      subparcel.write(&self.r#field)?;
      Ok(())
    })
  }
  fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_read(|subparcel| {
      if subparcel.has_more_data() {
        self.r#field = subparcel.read()?;
      }
      Ok(())
    })
  }
}
binder::impl_serialize_for_parcelable!(r#OtherParcelableForToString);
binder::impl_deserialize_for_parcelable!(r#OtherParcelableForToString);
impl binder::binder_impl::ParcelableMetadata for r#OtherParcelableForToString {
  fn get_descriptor() -> &'static str { "android.aidl.tests.OtherParcelableForToString" }
}
pub(crate) mod mangled {
 pub use super::r#OtherParcelableForToString as _7_android_4_aidl_5_tests_26_OtherParcelableForToString;
}
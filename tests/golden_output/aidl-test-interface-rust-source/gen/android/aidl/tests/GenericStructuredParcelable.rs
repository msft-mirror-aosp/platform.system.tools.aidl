#![forbid(unsafe_code)]
#![rustfmt::skip]
#[derive(Debug)]
pub struct r#GenericStructuredParcelable {
  pub r#a: i32,
  pub r#b: i32,
}
impl Default for r#GenericStructuredParcelable {
  fn default() -> Self {
    Self {
      r#a: 0,
      r#b: 0,
    }
  }
}
impl binder::Parcelable for r#GenericStructuredParcelable {
  fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_write(|subparcel| {
      subparcel.write(&self.r#a)?;
      subparcel.write(&self.r#b)?;
      Ok(())
    })
  }
  fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_read(|subparcel| {
      if subparcel.has_more_data() {
        self.r#a = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.r#b = subparcel.read()?;
      }
      Ok(())
    })
  }
}
binder::impl_serialize_for_parcelable!(r#GenericStructuredParcelable);
binder::impl_deserialize_for_parcelable!(r#GenericStructuredParcelable);
impl binder::binder_impl::ParcelableMetadata for r#GenericStructuredParcelable {
  fn get_descriptor() -> &'static str { "android.aidl.tests.GenericStructuredParcelable" }
}
pub(crate) mod mangled {
 pub use super::r#GenericStructuredParcelable as _7_android_4_aidl_5_tests_27_GenericStructuredParcelable;
}

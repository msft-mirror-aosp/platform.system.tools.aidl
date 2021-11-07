#![forbid(unsafe_code)]
#![rustfmt::skip]
#[derive(Debug, Clone, PartialEq)]
pub enum UnionInUnion {
  First(crate::mangled::_7_android_4_aidl_5_tests_6_unions_9_EnumUnion),
  Second(i32),
}
impl Default for UnionInUnion {
  fn default() -> Self {
    Self::First(Default::default())
  }
}
impl binder::parcel::Parcelable for UnionInUnion {
  fn write_to_parcel(&self, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    match self {
      Self::First(v) => {
        parcel.write(&0i32)?;
        parcel.write(v)
      }
      Self::Second(v) => {
        parcel.write(&1i32)?;
        parcel.write(v)
      }
    }
  }
  fn read_from_parcel(&mut self, parcel: &binder::parcel::Parcel) -> binder::Result<()> {
    let tag: i32 = parcel.read()?;
    match tag {
      0 => {
        let value: crate::mangled::_7_android_4_aidl_5_tests_6_unions_9_EnumUnion = parcel.read()?;
        *self = Self::First(value);
        Ok(())
      }
      1 => {
        let value: i32 = parcel.read()?;
        *self = Self::Second(value);
        Ok(())
      }
      _ => {
        Err(binder::StatusCode::BAD_VALUE)
      }
    }
  }
}
binder::impl_serialize_for_parcelable!(UnionInUnion);
binder::impl_deserialize_for_parcelable!(UnionInUnion);
impl binder::parcel::ParcelableMetadata for UnionInUnion {
  fn get_descriptor() -> &'static str { "android.aidl.tests.unions.UnionInUnion" }
}
pub(crate) mod mangled {
 pub use super::UnionInUnion as _7_android_4_aidl_5_tests_6_unions_12_UnionInUnion;
}

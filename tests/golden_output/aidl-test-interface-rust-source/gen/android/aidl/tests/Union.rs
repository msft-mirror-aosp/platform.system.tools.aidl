#![forbid(unsafe_code)]
#![rustfmt::skip]
#[derive(Debug, Clone, PartialEq)]
pub enum Union {
  Ns(Vec<i32>),
  N(i32),
  M(i32),
  S(String),
  Ibinder(Option<binder::SpIBinder>),
  Ss(Vec<String>),
  Be(crate::mangled::_7_android_4_aidl_5_tests_8_ByteEnum),
}
pub const S1: &str = "a string constant in union";
impl Default for Union {
  fn default() -> Self {
    Self::Ns(vec!{})
  }
}
impl binder::parcel::Parcelable for Union {
  fn write_to_parcel(&self, parcel: &mut binder::parcel::BorrowedParcel) -> binder::Result<()> {
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
  fn read_from_parcel(&mut self, parcel: &binder::parcel::BorrowedParcel) -> binder::Result<()> {
    let tag: i32 = parcel.read()?;
    match tag {
      0 => {
        let value: Vec<i32> = parcel.read()?;
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
        let value: String = parcel.read()?;
        *self = Self::S(value);
        Ok(())
      }
      4 => {
        let value: Option<binder::SpIBinder> = parcel.read()?;
        *self = Self::Ibinder(value);
        Ok(())
      }
      5 => {
        let value: Vec<String> = parcel.read()?;
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
binder::impl_serialize_for_parcelable!(Union);
binder::impl_deserialize_for_parcelable!(Union);
impl binder::parcel::ParcelableMetadata for Union {
  fn get_descriptor() -> &'static str { "android.aidl.tests.Union" }
}
pub(crate) mod mangled {
 pub use super::Union as _7_android_4_aidl_5_tests_5_Union;
}

#![forbid(unsafe_code)]
#[derive(Debug, Clone, PartialEq)]
pub enum EnumUnion {
  IntEnum(crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum),
  LongEnum(crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum),
}
pub(crate) mod mangled { pub use super::EnumUnion as _7_android_4_aidl_5_tests_6_unions_9_EnumUnion; }
impl Default for EnumUnion {
  fn default() -> Self {
    Self::IntEnum(crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum::FOO)
  }
}
impl binder::parcel::Serialize for EnumUnion {
  fn serialize(&self, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    <Self as binder::parcel::SerializeOption>::serialize_option(Some(self), parcel)
  }
}
impl binder::parcel::SerializeArray for EnumUnion {}
impl binder::parcel::SerializeOption for EnumUnion {
  fn serialize_option(this: Option<&Self>, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    let this = if let Some(this) = this {
      parcel.write(&1i32)?;
      this
    } else {
      return parcel.write(&0i32);
    };
    match this {
      Self::IntEnum(v) => {
        parcel.write(&0i32)?;
        parcel.write(v)
      }
      Self::LongEnum(v) => {
        parcel.write(&1i32)?;
        parcel.write(v)
      }
    }
  }
}
binder::impl_deserialize_for_parcelable!(EnumUnion);
impl EnumUnion {
  fn deserialize_parcelable(&mut self, parcel: &binder::parcel::Parcel) -> binder::Result<()> {
    let tag: i32 = parcel.read()?;
    match tag {
      0 => {
        let value: crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum = parcel.read()?;
        *self = Self::IntEnum(value);
        Ok(())
      }
      1 => {
        let value: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum = parcel.read()?;
        *self = Self::LongEnum(value);
        Ok(())
      }
      _ => {
        Err(binder::StatusCode::BAD_VALUE)
      }
    }
  }
}

/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=rust -Weverything -Wno-missing-permission-annotation -Werror --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen/android/aidl/tests/FixedSize.rs.d -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/FixedSize.aidl
 */
#![forbid(unsafe_code)]
#![cfg_attr(rustfmt, rustfmt_skip)]
#[derive(Debug)]
pub struct r#FixedSize {
}
impl Default for r#FixedSize {
  fn default() -> Self {
    Self {
    }
  }
}
impl binder::Parcelable for r#FixedSize {
  fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_write(|subparcel| {
      Ok(())
    })
  }
  fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
    parcel.sized_read(|subparcel| {
      Ok(())
    })
  }
}
binder::impl_serialize_for_parcelable!(r#FixedSize);
binder::impl_deserialize_for_parcelable!(r#FixedSize);
impl binder::binder_impl::ParcelableMetadata for r#FixedSize {
  fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize" }
}
pub mod r#FixedParcelable {
  #[derive(Debug)]
  pub struct r#FixedParcelable {
    pub r#booleanValue: bool,
    pub r#byteValue: i8,
    pub r#charValue: u16,
    pub r#intValue: i32,
    pub r#longValue: i64,
    pub r#floatValue: f32,
    pub r#intArray: [i32; 3],
    pub r#multiDimensionLongArray: [[i64; 2]; 3],
    pub r#doubleValue: f64,
    pub r#enumValue: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum,
    pub r#parcelableValue: crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion,
    pub r#parcelableArray: [crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_15_EmptyParcelable; 3],
    pub r#unionArray: [crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion; 4],
  }
  impl Default for r#FixedParcelable {
    fn default() -> Self {
      Self {
        r#booleanValue: false,
        r#byteValue: 0,
        r#charValue: '\0' as u16,
        r#intValue: 0,
        r#longValue: 0,
        r#floatValue: 0.000000f32,
        r#intArray: [Default::default(), Default::default(), Default::default()],
        r#multiDimensionLongArray: [[Default::default(), Default::default()], [Default::default(), Default::default()], [Default::default(), Default::default()]],
        r#doubleValue: 0.000000f64,
        r#enumValue: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum::FOO,
        r#parcelableValue: Default::default(),
        r#parcelableArray: [Default::default(), Default::default(), Default::default()],
        r#unionArray: [Default::default(), Default::default(), Default::default(), Default::default()],
      }
    }
  }
  impl binder::Parcelable for r#FixedParcelable {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_write(|subparcel| {
        subparcel.write(&self.r#booleanValue)?;
        subparcel.write(&self.r#byteValue)?;
        subparcel.write(&self.r#charValue)?;
        subparcel.write(&self.r#intValue)?;
        subparcel.write(&self.r#longValue)?;
        subparcel.write(&self.r#floatValue)?;
        subparcel.write(&self.r#intArray)?;
        subparcel.write(&self.r#multiDimensionLongArray)?;
        subparcel.write(&self.r#doubleValue)?;
        subparcel.write(&self.r#enumValue)?;
        subparcel.write(&self.r#parcelableValue)?;
        subparcel.write(&self.r#parcelableArray)?;
        subparcel.write(&self.r#unionArray)?;
        Ok(())
      })
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_read(|subparcel| {
        if subparcel.has_more_data() {
          self.r#booleanValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#byteValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#charValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#intValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#longValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#floatValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#intArray = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#multiDimensionLongArray = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#doubleValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#enumValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#parcelableValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#parcelableArray = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#unionArray = subparcel.read()?;
        }
        Ok(())
      })
    }
  }
  binder::impl_serialize_for_parcelable!(r#FixedParcelable);
  binder::impl_deserialize_for_parcelable!(r#FixedParcelable);
  impl binder::binder_impl::ParcelableMetadata for r#FixedParcelable {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.FixedParcelable" }
  }
}
pub mod r#ExplicitPaddingParcelable {
  #[derive(Debug)]
  pub struct r#ExplicitPaddingParcelable {
    pub r#byteValue: i8,
    pub r#longValue: i64,
    pub r#charValue: u16,
    pub r#doubleValue: f64,
    pub r#intValue: i32,
    pub r#enumValue: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum,
  }
  impl Default for r#ExplicitPaddingParcelable {
    fn default() -> Self {
      Self {
        r#byteValue: 0,
        r#longValue: 0,
        r#charValue: '\0' as u16,
        r#doubleValue: 0.000000f64,
        r#intValue: 0,
        r#enumValue: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum::FOO,
      }
    }
  }
  impl binder::Parcelable for r#ExplicitPaddingParcelable {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_write(|subparcel| {
        subparcel.write(&self.r#byteValue)?;
        subparcel.write(&self.r#longValue)?;
        subparcel.write(&self.r#charValue)?;
        subparcel.write(&self.r#doubleValue)?;
        subparcel.write(&self.r#intValue)?;
        subparcel.write(&self.r#enumValue)?;
        Ok(())
      })
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_read(|subparcel| {
        if subparcel.has_more_data() {
          self.r#byteValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#longValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#charValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#doubleValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#intValue = subparcel.read()?;
        }
        if subparcel.has_more_data() {
          self.r#enumValue = subparcel.read()?;
        }
        Ok(())
      })
    }
  }
  binder::impl_serialize_for_parcelable!(r#ExplicitPaddingParcelable);
  binder::impl_deserialize_for_parcelable!(r#ExplicitPaddingParcelable);
  impl binder::binder_impl::ParcelableMetadata for r#ExplicitPaddingParcelable {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.ExplicitPaddingParcelable" }
  }
}
pub mod r#EmptyParcelable {
  #[derive(Debug)]
  pub struct r#EmptyParcelable {
  }
  impl Default for r#EmptyParcelable {
    fn default() -> Self {
      Self {
      }
    }
  }
  impl binder::Parcelable for r#EmptyParcelable {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_write(|subparcel| {
        Ok(())
      })
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      parcel.sized_read(|subparcel| {
        Ok(())
      })
    }
  }
  binder::impl_serialize_for_parcelable!(r#EmptyParcelable);
  binder::impl_deserialize_for_parcelable!(r#EmptyParcelable);
  impl binder::binder_impl::ParcelableMetadata for r#EmptyParcelable {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.EmptyParcelable" }
  }
}
pub mod r#FixedUnion {
  #[derive(Debug)]
  pub enum r#FixedUnion {
    BooleanValue(bool),
    ByteValue(i8),
    CharValue(u16),
    IntValue(i32),
    LongValue(i64),
    FloatValue(f32),
    IntArray([i32; 3]),
    MultiDimensionLongArray([[i64; 2]; 3]),
    DoubleValue(f64),
    EnumValue(crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum),
  }
  impl Default for r#FixedUnion {
    fn default() -> Self {
      Self::BooleanValue(false)
    }
  }
  impl binder::Parcelable for r#FixedUnion {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      match self {
        Self::BooleanValue(v) => {
          parcel.write(&0i32)?;
          parcel.write(v)
        }
        Self::ByteValue(v) => {
          parcel.write(&1i32)?;
          parcel.write(v)
        }
        Self::CharValue(v) => {
          parcel.write(&2i32)?;
          parcel.write(v)
        }
        Self::IntValue(v) => {
          parcel.write(&3i32)?;
          parcel.write(v)
        }
        Self::LongValue(v) => {
          parcel.write(&4i32)?;
          parcel.write(v)
        }
        Self::FloatValue(v) => {
          parcel.write(&5i32)?;
          parcel.write(v)
        }
        Self::IntArray(v) => {
          parcel.write(&6i32)?;
          parcel.write(v)
        }
        Self::MultiDimensionLongArray(v) => {
          parcel.write(&7i32)?;
          parcel.write(v)
        }
        Self::DoubleValue(v) => {
          parcel.write(&8i32)?;
          parcel.write(v)
        }
        Self::EnumValue(v) => {
          parcel.write(&9i32)?;
          parcel.write(v)
        }
      }
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      let tag: i32 = parcel.read()?;
      match tag {
        0 => {
          let value: bool = parcel.read()?;
          *self = Self::BooleanValue(value);
          Ok(())
        }
        1 => {
          let value: i8 = parcel.read()?;
          *self = Self::ByteValue(value);
          Ok(())
        }
        2 => {
          let value: u16 = parcel.read()?;
          *self = Self::CharValue(value);
          Ok(())
        }
        3 => {
          let value: i32 = parcel.read()?;
          *self = Self::IntValue(value);
          Ok(())
        }
        4 => {
          let value: i64 = parcel.read()?;
          *self = Self::LongValue(value);
          Ok(())
        }
        5 => {
          let value: f32 = parcel.read()?;
          *self = Self::FloatValue(value);
          Ok(())
        }
        6 => {
          let value: [i32; 3] = parcel.read()?;
          *self = Self::IntArray(value);
          Ok(())
        }
        7 => {
          let value: [[i64; 2]; 3] = parcel.read()?;
          *self = Self::MultiDimensionLongArray(value);
          Ok(())
        }
        8 => {
          let value: f64 = parcel.read()?;
          *self = Self::DoubleValue(value);
          Ok(())
        }
        9 => {
          let value: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum = parcel.read()?;
          *self = Self::EnumValue(value);
          Ok(())
        }
        _ => {
          Err(binder::StatusCode::BAD_VALUE)
        }
      }
    }
  }
  binder::impl_serialize_for_parcelable!(r#FixedUnion);
  binder::impl_deserialize_for_parcelable!(r#FixedUnion);
  impl binder::binder_impl::ParcelableMetadata for r#FixedUnion {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.FixedUnion" }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      r#Tag : [i8; 10] {
        r#booleanValue = 0,
        r#byteValue = 1,
        r#charValue = 2,
        r#intValue = 3,
        r#longValue = 4,
        r#floatValue = 5,
        r#intArray = 6,
        r#multiDimensionLongArray = 7,
        r#doubleValue = 8,
        r#enumValue = 9,
      }
    }
  }
}
pub mod r#FixedUnionNoPadding {
  #[derive(Debug)]
  pub enum r#FixedUnionNoPadding {
    ByteValue(i8),
  }
  impl Default for r#FixedUnionNoPadding {
    fn default() -> Self {
      Self::ByteValue(0)
    }
  }
  impl binder::Parcelable for r#FixedUnionNoPadding {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      match self {
        Self::ByteValue(v) => {
          parcel.write(&0i32)?;
          parcel.write(v)
        }
      }
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      let tag: i32 = parcel.read()?;
      match tag {
        0 => {
          let value: i8 = parcel.read()?;
          *self = Self::ByteValue(value);
          Ok(())
        }
        _ => {
          Err(binder::StatusCode::BAD_VALUE)
        }
      }
    }
  }
  binder::impl_serialize_for_parcelable!(r#FixedUnionNoPadding);
  binder::impl_deserialize_for_parcelable!(r#FixedUnionNoPadding);
  impl binder::binder_impl::ParcelableMetadata for r#FixedUnionNoPadding {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.FixedUnionNoPadding" }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      r#Tag : [i8; 1] {
        r#byteValue = 0,
      }
    }
  }
}
pub mod r#FixedUnionSmallPadding {
  #[derive(Debug)]
  pub enum r#FixedUnionSmallPadding {
    CharValue(u16),
  }
  impl Default for r#FixedUnionSmallPadding {
    fn default() -> Self {
      Self::CharValue('\0' as u16)
    }
  }
  impl binder::Parcelable for r#FixedUnionSmallPadding {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      match self {
        Self::CharValue(v) => {
          parcel.write(&0i32)?;
          parcel.write(v)
        }
      }
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      let tag: i32 = parcel.read()?;
      match tag {
        0 => {
          let value: u16 = parcel.read()?;
          *self = Self::CharValue(value);
          Ok(())
        }
        _ => {
          Err(binder::StatusCode::BAD_VALUE)
        }
      }
    }
  }
  binder::impl_serialize_for_parcelable!(r#FixedUnionSmallPadding);
  binder::impl_deserialize_for_parcelable!(r#FixedUnionSmallPadding);
  impl binder::binder_impl::ParcelableMetadata for r#FixedUnionSmallPadding {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.FixedUnionSmallPadding" }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      r#Tag : [i8; 1] {
        r#charValue = 0,
      }
    }
  }
}
pub mod r#FixedUnionLongPadding {
  #[derive(Debug)]
  pub enum r#FixedUnionLongPadding {
    LongValue(i64),
  }
  impl Default for r#FixedUnionLongPadding {
    fn default() -> Self {
      Self::LongValue(0)
    }
  }
  impl binder::Parcelable for r#FixedUnionLongPadding {
    fn write_to_parcel(&self, parcel: &mut binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      match self {
        Self::LongValue(v) => {
          parcel.write(&0i32)?;
          parcel.write(v)
        }
      }
    }
    fn read_from_parcel(&mut self, parcel: &binder::binder_impl::BorrowedParcel) -> std::result::Result<(), binder::StatusCode> {
      let tag: i32 = parcel.read()?;
      match tag {
        0 => {
          let value: i64 = parcel.read()?;
          *self = Self::LongValue(value);
          Ok(())
        }
        _ => {
          Err(binder::StatusCode::BAD_VALUE)
        }
      }
    }
  }
  binder::impl_serialize_for_parcelable!(r#FixedUnionLongPadding);
  binder::impl_deserialize_for_parcelable!(r#FixedUnionLongPadding);
  impl binder::binder_impl::ParcelableMetadata for r#FixedUnionLongPadding {
    fn get_descriptor() -> &'static str { "android.aidl.tests.FixedSize.FixedUnionLongPadding" }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      r#Tag : [i8; 1] {
        r#longValue = 0,
      }
    }
  }
}
pub(crate) mod mangled {
 pub use super::r#FixedSize as _7_android_4_aidl_5_tests_9_FixedSize;
 pub use super::r#FixedParcelable::r#FixedParcelable as _7_android_4_aidl_5_tests_9_FixedSize_15_FixedParcelable;
 pub use super::r#ExplicitPaddingParcelable::r#ExplicitPaddingParcelable as _7_android_4_aidl_5_tests_9_FixedSize_25_ExplicitPaddingParcelable;
 pub use super::r#EmptyParcelable::r#EmptyParcelable as _7_android_4_aidl_5_tests_9_FixedSize_15_EmptyParcelable;
 pub use super::r#FixedUnion::r#FixedUnion as _7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion;
 pub use super::r#FixedUnion::r#Tag::r#Tag as _7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion_3_Tag;
 pub use super::r#FixedUnionNoPadding::r#FixedUnionNoPadding as _7_android_4_aidl_5_tests_9_FixedSize_19_FixedUnionNoPadding;
 pub use super::r#FixedUnionNoPadding::r#Tag::r#Tag as _7_android_4_aidl_5_tests_9_FixedSize_19_FixedUnionNoPadding_3_Tag;
 pub use super::r#FixedUnionSmallPadding::r#FixedUnionSmallPadding as _7_android_4_aidl_5_tests_9_FixedSize_22_FixedUnionSmallPadding;
 pub use super::r#FixedUnionSmallPadding::r#Tag::r#Tag as _7_android_4_aidl_5_tests_9_FixedSize_22_FixedUnionSmallPadding_3_Tag;
 pub use super::r#FixedUnionLongPadding::r#FixedUnionLongPadding as _7_android_4_aidl_5_tests_9_FixedSize_21_FixedUnionLongPadding;
 pub use super::r#FixedUnionLongPadding::r#Tag::r#Tag as _7_android_4_aidl_5_tests_9_FixedSize_21_FixedUnionLongPadding_3_Tag;
}

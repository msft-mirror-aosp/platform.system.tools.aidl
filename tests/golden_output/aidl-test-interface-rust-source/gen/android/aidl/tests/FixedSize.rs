/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=rust -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen/android/aidl/tests/FixedSize.rs.d -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-rust-source/gen -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/FixedSize.aidl
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
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
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(C, align(8))]
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
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#booleanValue), 0);
  static_assertions::const_assert_eq!(std::mem::size_of::<bool>(), 1);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#byteValue), 1);
  static_assertions::const_assert_eq!(std::mem::size_of::<i8>(), 1);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#charValue), 2);
  static_assertions::const_assert_eq!(std::mem::size_of::<u16>(), 2);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#intValue), 4);
  static_assertions::const_assert_eq!(std::mem::size_of::<i32>(), 4);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#longValue), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<i64>(), 8);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#floatValue), 16);
  static_assertions::const_assert_eq!(std::mem::size_of::<f32>(), 4);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#intArray), 20);
  static_assertions::const_assert_eq!(std::mem::size_of::<[i32; 3]>(), 12);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#multiDimensionLongArray), 32);
  static_assertions::const_assert_eq!(std::mem::size_of::<[[i64; 2]; 3]>(), 48);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#doubleValue), 80);
  static_assertions::const_assert_eq!(std::mem::size_of::<f64>(), 8);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#enumValue), 88);
  static_assertions::const_assert_eq!(std::mem::size_of::<crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum>(), 8);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#parcelableValue), 96);
  static_assertions::const_assert_eq!(std::mem::size_of::<crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion>(), 56);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#parcelableArray), 152);
  static_assertions::const_assert_eq!(std::mem::size_of::<[crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_15_EmptyParcelable; 3]>(), 3);
  static_assertions::const_assert_eq!(std::mem::offset_of!(FixedParcelable, r#unionArray), 160);
  static_assertions::const_assert_eq!(std::mem::size_of::<[crate::mangled::_7_android_4_aidl_5_tests_9_FixedSize_10_FixedUnion; 4]>(), 224);
  static_assertions::const_assert_eq!(std::mem::align_of::<FixedParcelable>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<FixedParcelable>(), 384);
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
  impl binder::WriteTo for r#FixedParcelable {
    #[inline]
    unsafe fn write_to(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `FixedParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
        binder::WriteTo::write_to(&self.r#booleanValue, &raw mut (*_target).r#booleanValue);
        binder::WriteTo::write_to(&self.r#byteValue, &raw mut (*_target).r#byteValue);
        binder::WriteTo::write_to(&self.r#charValue, &raw mut (*_target).r#charValue);
        binder::WriteTo::write_to(&self.r#intValue, &raw mut (*_target).r#intValue);
        binder::WriteTo::write_to(&self.r#longValue, &raw mut (*_target).r#longValue);
        binder::WriteTo::write_to(&self.r#floatValue, &raw mut (*_target).r#floatValue);
        binder::WriteTo::write_to(&self.r#intArray, &raw mut (*_target).r#intArray);
        binder::WriteTo::write_to(&self.r#multiDimensionLongArray, &raw mut (*_target).r#multiDimensionLongArray);
        binder::WriteTo::write_to(&self.r#doubleValue, &raw mut (*_target).r#doubleValue);
        binder::WriteTo::write_to(&self.r#enumValue, &raw mut (*_target).r#enumValue);
        binder::WriteTo::write_to(&self.r#parcelableValue, &raw mut (*_target).r#parcelableValue);
        binder::WriteTo::write_to(&self.r#parcelableArray, &raw mut (*_target).r#parcelableArray);
        binder::WriteTo::write_to(&self.r#unionArray, &raw mut (*_target).r#unionArray);
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `FixedParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
        binder::WriteTo::write_to_volatile(&self.r#booleanValue, &raw mut (*_target).r#booleanValue);
        binder::WriteTo::write_to_volatile(&self.r#byteValue, &raw mut (*_target).r#byteValue);
        binder::WriteTo::write_to_volatile(&self.r#charValue, &raw mut (*_target).r#charValue);
        binder::WriteTo::write_to_volatile(&self.r#intValue, &raw mut (*_target).r#intValue);
        binder::WriteTo::write_to_volatile(&self.r#longValue, &raw mut (*_target).r#longValue);
        binder::WriteTo::write_to_volatile(&self.r#floatValue, &raw mut (*_target).r#floatValue);
        binder::WriteTo::write_to_volatile(&self.r#intArray, &raw mut (*_target).r#intArray);
        binder::WriteTo::write_to_volatile(&self.r#multiDimensionLongArray, &raw mut (*_target).r#multiDimensionLongArray);
        binder::WriteTo::write_to_volatile(&self.r#doubleValue, &raw mut (*_target).r#doubleValue);
        binder::WriteTo::write_to_volatile(&self.r#enumValue, &raw mut (*_target).r#enumValue);
        binder::WriteTo::write_to_volatile(&self.r#parcelableValue, &raw mut (*_target).r#parcelableValue);
        binder::WriteTo::write_to_volatile(&self.r#parcelableArray, &raw mut (*_target).r#parcelableArray);
        binder::WriteTo::write_to_volatile(&self.r#unionArray, &raw mut (*_target).r#unionArray);
      }
    }
  }
}
pub mod r#ExplicitPaddingParcelable {
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(C, align(8))]
  pub struct r#ExplicitPaddingParcelable {
    pub r#byteValue: i8,
    _pad_0: [u8; 7],
    pub r#longValue: i64,
    pub r#charValue: u16,
    _pad_1: [u8; 6],
    pub r#doubleValue: f64,
    pub r#intValue: i32,
    pub r#enumValue: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum,
  }
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#byteValue), 0);
  static_assertions::const_assert_eq!(std::mem::size_of::<i8>(), 1);
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#longValue), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<i64>(), 8);
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#charValue), 16);
  static_assertions::const_assert_eq!(std::mem::size_of::<u16>(), 2);
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#doubleValue), 24);
  static_assertions::const_assert_eq!(std::mem::size_of::<f64>(), 8);
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#intValue), 32);
  static_assertions::const_assert_eq!(std::mem::size_of::<i32>(), 4);
  static_assertions::const_assert_eq!(std::mem::offset_of!(ExplicitPaddingParcelable, r#enumValue), 40);
  static_assertions::const_assert_eq!(std::mem::size_of::<crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum>(), 8);
  static_assertions::const_assert_eq!(std::mem::align_of::<ExplicitPaddingParcelable>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<ExplicitPaddingParcelable>(), 48);
  impl Default for r#ExplicitPaddingParcelable {
    fn default() -> Self {
      Self {
        r#byteValue: 0,
        _pad_0: [0; 7],
        r#longValue: 0,
        r#charValue: '\0' as u16,
        _pad_1: [0; 6],
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
  impl binder::WriteTo for r#ExplicitPaddingParcelable {
    #[inline]
    unsafe fn write_to(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `ExplicitPaddingParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
        binder::WriteTo::write_to(&self.r#byteValue, &raw mut (*_target).r#byteValue);
        binder::WriteTo::write_to(&self.r#longValue, &raw mut (*_target).r#longValue);
        binder::WriteTo::write_to(&self.r#charValue, &raw mut (*_target).r#charValue);
        binder::WriteTo::write_to(&self.r#doubleValue, &raw mut (*_target).r#doubleValue);
        binder::WriteTo::write_to(&self.r#intValue, &raw mut (*_target).r#intValue);
        binder::WriteTo::write_to(&self.r#enumValue, &raw mut (*_target).r#enumValue);
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `ExplicitPaddingParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
        binder::WriteTo::write_to_volatile(&self.r#byteValue, &raw mut (*_target).r#byteValue);
        binder::WriteTo::write_to_volatile(&self.r#longValue, &raw mut (*_target).r#longValue);
        binder::WriteTo::write_to_volatile(&self.r#charValue, &raw mut (*_target).r#charValue);
        binder::WriteTo::write_to_volatile(&self.r#doubleValue, &raw mut (*_target).r#doubleValue);
        binder::WriteTo::write_to_volatile(&self.r#intValue, &raw mut (*_target).r#intValue);
        binder::WriteTo::write_to_volatile(&self.r#enumValue, &raw mut (*_target).r#enumValue);
      }
    }
  }
}
pub mod r#EmptyParcelable {
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(C, align(1))]
  pub struct r#EmptyParcelable {
    _unused: u8,
  }
  static_assertions::const_assert_eq!(std::mem::align_of::<EmptyParcelable>(), 1);
  static_assertions::const_assert_eq!(std::mem::size_of::<EmptyParcelable>(), 1);
  impl Default for r#EmptyParcelable {
    fn default() -> Self {
      Self {
        _unused: 0,
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
  impl binder::WriteTo for r#EmptyParcelable {
    #[inline]
    unsafe fn write_to(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `EmptyParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, _target: *mut Self) {
      // SAFETY: We assume that `_target` points to a valid value of type `EmptyParcelable`
      // which implies that all of its fields are correctly aligned and have valid values.
      unsafe {
      }
    }
  }
}
pub mod r#FixedUnion {
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(u64, align(8))]
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
  impl r#FixedUnion {
    #[inline(always)]
    pub const fn tag(&self) -> Tag::Tag {
      // SAFETY: The first byte of a union is the tag.
      // All bitpatterns are valid for `Tag`.
      unsafe { std::mem::transmute_copy::<Self, Tag::Tag>(self) }
    }
  }
  static_assertions::const_assert_eq!(std::mem::size_of::<bool>(), 1);
  static_assertions::const_assert_eq!(std::mem::size_of::<i8>(), 1);
  static_assertions::const_assert_eq!(std::mem::size_of::<u16>(), 2);
  static_assertions::const_assert_eq!(std::mem::size_of::<i32>(), 4);
  static_assertions::const_assert_eq!(std::mem::size_of::<i64>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<f32>(), 4);
  static_assertions::const_assert_eq!(std::mem::size_of::<[i32; 3]>(), 12);
  static_assertions::const_assert_eq!(std::mem::size_of::<[[i64; 2]; 3]>(), 48);
  static_assertions::const_assert_eq!(std::mem::size_of::<f64>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum>(), 8);
  static_assertions::const_assert_eq!(FixedUnion::BooleanValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#booleanValue.get());
  static_assertions::const_assert_eq!(FixedUnion::ByteValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#byteValue.get());
  static_assertions::const_assert_eq!(FixedUnion::CharValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#charValue.get());
  static_assertions::const_assert_eq!(FixedUnion::IntValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#intValue.get());
  static_assertions::const_assert_eq!(FixedUnion::LongValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#longValue.get());
  static_assertions::const_assert_eq!(FixedUnion::FloatValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#floatValue.get());
  static_assertions::const_assert_eq!(FixedUnion::IntArray(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#intArray.get());
  static_assertions::const_assert_eq!(FixedUnion::MultiDimensionLongArray(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#multiDimensionLongArray.get());
  static_assertions::const_assert_eq!(FixedUnion::DoubleValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#doubleValue.get());
  static_assertions::const_assert_eq!(FixedUnion::EnumValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#enumValue.get());
  static_assertions::const_assert_eq!(std::mem::align_of::<FixedUnion>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<FixedUnion>(), 56);
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
  impl binder::WriteTo for r#FixedUnion {
    #[inline]
    unsafe fn write_to(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to(target.cast()) };
      match self {
        Self::r#BooleanValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#ByteValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#CharValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#IntValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#LongValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#FloatValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#IntArray(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#MultiDimensionLongArray(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#DoubleValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#EnumValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to_volatile(target.cast()) };
      match self {
        Self::r#BooleanValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#ByteValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#CharValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#IntValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#LongValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#FloatValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#IntArray(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#MultiDimensionLongArray(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#DoubleValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
        Self::r#EnumValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      #[repr(align(1))]
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
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(u8, align(1))]
  pub enum r#FixedUnionNoPadding {
    ByteValue(i8),
  }
  impl r#FixedUnionNoPadding {
    #[inline(always)]
    pub const fn tag(&self) -> Tag::Tag {
      // SAFETY: The first byte of a union is the tag.
      // All bitpatterns are valid for `Tag`.
      unsafe { std::mem::transmute_copy::<Self, Tag::Tag>(self) }
    }
  }
  static_assertions::const_assert_eq!(std::mem::size_of::<i8>(), 1);
  static_assertions::const_assert_eq!(FixedUnionNoPadding::ByteValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#byteValue.get());
  static_assertions::const_assert_eq!(std::mem::align_of::<FixedUnionNoPadding>(), 1);
  static_assertions::const_assert_eq!(std::mem::size_of::<FixedUnionNoPadding>(), 2);
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
  impl binder::WriteTo for r#FixedUnionNoPadding {
    #[inline]
    unsafe fn write_to(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to(target.cast()) };
      match self {
        Self::r#ByteValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to_volatile(target.cast()) };
      match self {
        Self::r#ByteValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      #[repr(align(1))]
      r#Tag : [i8; 1] {
        r#byteValue = 0,
      }
    }
  }
}
pub mod r#FixedUnionSmallPadding {
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(u16, align(2))]
  pub enum r#FixedUnionSmallPadding {
    CharValue(u16),
  }
  impl r#FixedUnionSmallPadding {
    #[inline(always)]
    pub const fn tag(&self) -> Tag::Tag {
      // SAFETY: The first byte of a union is the tag.
      // All bitpatterns are valid for `Tag`.
      unsafe { std::mem::transmute_copy::<Self, Tag::Tag>(self) }
    }
  }
  static_assertions::const_assert_eq!(std::mem::size_of::<u16>(), 2);
  static_assertions::const_assert_eq!(FixedUnionSmallPadding::CharValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#charValue.get());
  static_assertions::const_assert_eq!(std::mem::align_of::<FixedUnionSmallPadding>(), 2);
  static_assertions::const_assert_eq!(std::mem::size_of::<FixedUnionSmallPadding>(), 4);
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
  impl binder::WriteTo for r#FixedUnionSmallPadding {
    #[inline]
    unsafe fn write_to(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to(target.cast()) };
      match self {
        Self::r#CharValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to_volatile(target.cast()) };
      match self {
        Self::r#CharValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      #[repr(align(1))]
      r#Tag : [i8; 1] {
        r#charValue = 0,
      }
    }
  }
}
pub mod r#FixedUnionLongPadding {
  #[derive(Debug, zerocopy::Immutable, zerocopy::TryFromBytes)]
  #[repr(u64, align(8))]
  pub enum r#FixedUnionLongPadding {
    LongValue(i64),
  }
  impl r#FixedUnionLongPadding {
    #[inline(always)]
    pub const fn tag(&self) -> Tag::Tag {
      // SAFETY: The first byte of a union is the tag.
      // All bitpatterns are valid for `Tag`.
      unsafe { std::mem::transmute_copy::<Self, Tag::Tag>(self) }
    }
  }
  static_assertions::const_assert_eq!(std::mem::size_of::<i64>(), 8);
  static_assertions::const_assert_eq!(FixedUnionLongPadding::LongValue(unsafe { std::mem::zeroed() }).tag().get(), Tag::Tag::r#longValue.get());
  static_assertions::const_assert_eq!(std::mem::align_of::<FixedUnionLongPadding>(), 8);
  static_assertions::const_assert_eq!(std::mem::size_of::<FixedUnionLongPadding>(), 16);
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
  impl binder::WriteTo for r#FixedUnionLongPadding {
    #[inline]
    unsafe fn write_to(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to(target.cast()) };
      match self {
        Self::r#LongValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
    #[inline]
    unsafe fn write_to_volatile(&self, target: *mut Self) {
      // SAFETY: Per the Rust reference, the tag is always at offset 0
      // and matches the primitive type.
      // We assume that `target` is a correctly aligned pointer.
      unsafe { self.tag().write_to_volatile(target.cast()) };
      match self {
        Self::r#LongValue(payload) => {
          // SAFETY: `payload` is the data inside the current variant of `self`.
          let offset = unsafe { (&raw const payload).byte_offset_from(&raw const self) };
          // SAFETY: If `target` is correctly aligned and valid,
          // then `payload` should be as well.
          unsafe { binder::WriteTo::write_to_volatile(payload, target.byte_offset(offset).cast()) };
        }
      }
    }
  }
  pub mod r#Tag {
    #![allow(non_upper_case_globals)]
    use binder::declare_binder_enum;
    declare_binder_enum! {
      #[repr(align(1))]
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

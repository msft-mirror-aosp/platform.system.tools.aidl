#![forbid(unsafe_code)]
#[derive(Debug, Clone, PartialEq)]
pub struct StructuredParcelable {
  pub shouldContainThreeFs: Vec<i32>,
  pub f: i32,
  pub shouldBeJerry: String,
  pub shouldBeByteBar: crate::mangled::_7_android_4_aidl_5_tests_8_ByteEnum,
  pub shouldBeIntBar: crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum,
  pub shouldBeLongBar: crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum,
  pub shouldContainTwoByteFoos: Vec<crate::mangled::_7_android_4_aidl_5_tests_8_ByteEnum>,
  pub shouldContainTwoIntFoos: Vec<crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum>,
  pub shouldContainTwoLongFoos: Vec<crate::mangled::_7_android_4_aidl_5_tests_8_LongEnum>,
  pub stringDefaultsToFoo: String,
  pub byteDefaultsToFour: i8,
  pub intDefaultsToFive: i32,
  pub longDefaultsToNegativeSeven: i64,
  pub booleanDefaultsToTrue: bool,
  pub charDefaultsToC: u16,
  pub floatDefaultsToPi: f32,
  pub doubleWithDefault: f64,
  pub arrayDefaultsTo123: Vec<i32>,
  pub arrayDefaultsToEmpty: Vec<i32>,
  pub boolDefault: bool,
  pub byteDefault: i8,
  pub intDefault: i32,
  pub longDefault: i64,
  pub floatDefault: f32,
  pub doubleDefault: f64,
  pub checkDoubleFromFloat: f64,
  pub checkStringArray1: Vec<String>,
  pub checkStringArray2: Vec<String>,
  pub int32_min: i32,
  pub int32_max: i32,
  pub int64_max: i64,
  pub hexInt32_neg_1: i32,
  pub ibinder: Option<binder::SpIBinder>,
  pub int32_1: Vec<i32>,
  pub int64_1: Vec<i64>,
  pub hexInt32_pos_1: i32,
  pub hexInt64_pos_1: i32,
  pub const_exprs_1: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_2: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_3: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_4: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_5: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_6: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_7: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_8: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_9: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub const_exprs_10: crate::mangled::_7_android_4_aidl_5_tests_22_ConstantExpressionEnum,
  pub addString1: String,
  pub addString2: String,
  pub shouldSetBit0AndBit2: i32,
  pub u: Option<crate::mangled::_7_android_4_aidl_5_tests_5_Union>,
  pub shouldBeConstS1: Option<crate::mangled::_7_android_4_aidl_5_tests_5_Union>,
  pub defaultWithFoo: crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum,
}
pub const BIT0: i32 = 1;
pub const BIT1: i32 = 2;
pub const BIT2: i32 = 4;
pub(crate) mod mangled { pub use super::StructuredParcelable as _7_android_4_aidl_5_tests_20_StructuredParcelable; }
impl Default for StructuredParcelable {
  fn default() -> Self {
    Self {
      shouldContainThreeFs: Default::default(),
      f: 0,
      shouldBeJerry: Default::default(),
      shouldBeByteBar: Default::default(),
      shouldBeIntBar: Default::default(),
      shouldBeLongBar: Default::default(),
      shouldContainTwoByteFoos: Default::default(),
      shouldContainTwoIntFoos: Default::default(),
      shouldContainTwoLongFoos: Default::default(),
      stringDefaultsToFoo: "foo".into(),
      byteDefaultsToFour: 4,
      intDefaultsToFive: 5,
      longDefaultsToNegativeSeven: -7,
      booleanDefaultsToTrue: true,
      charDefaultsToC: 'C' as u16,
      floatDefaultsToPi: 3.140000f32,
      doubleWithDefault: -314000000000000000.000000f64,
      arrayDefaultsTo123: vec!{1, 2, 3},
      arrayDefaultsToEmpty: vec!{},
      boolDefault: false,
      byteDefault: 0,
      intDefault: 0,
      longDefault: 0,
      floatDefault: 0.000000f32,
      doubleDefault: 0.000000f64,
      checkDoubleFromFloat: 3.140000f64,
      checkStringArray1: vec!{"a".into(), "b".into()},
      checkStringArray2: vec!{"a".into(), "b".into()},
      int32_min: -2147483648,
      int32_max: 2147483647,
      int64_max: 9223372036854775807,
      hexInt32_neg_1: -1,
      ibinder: Default::default(),
      int32_1: vec!{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      int64_1: vec!{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      hexInt32_pos_1: 1,
      hexInt64_pos_1: 1,
      const_exprs_1: Default::default(),
      const_exprs_2: Default::default(),
      const_exprs_3: Default::default(),
      const_exprs_4: Default::default(),
      const_exprs_5: Default::default(),
      const_exprs_6: Default::default(),
      const_exprs_7: Default::default(),
      const_exprs_8: Default::default(),
      const_exprs_9: Default::default(),
      const_exprs_10: Default::default(),
      addString1: "hello world!".into(),
      addString2: "The quick brown fox jumps over the lazy dog.".into(),
      shouldSetBit0AndBit2: 0,
      u: Default::default(),
      shouldBeConstS1: Default::default(),
      defaultWithFoo: crate::mangled::_7_android_4_aidl_5_tests_7_IntEnum::FOO,
    }
  }
}
impl binder::parcel::Serialize for StructuredParcelable {
  fn serialize(&self, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    <Self as binder::parcel::SerializeOption>::serialize_option(Some(self), parcel)
  }
}
impl binder::parcel::SerializeArray for StructuredParcelable {}
impl binder::parcel::SerializeOption for StructuredParcelable {
  fn serialize_option(this: Option<&Self>, parcel: &mut binder::parcel::Parcel) -> binder::Result<()> {
    let this = if let Some(this) = this {
      parcel.write(&1i32)?;
      this
    } else {
      return parcel.write(&0i32);
    };
    parcel.sized_write(|subparcel| {
      subparcel.write(&this.shouldContainThreeFs)?;
      subparcel.write(&this.f)?;
      subparcel.write(&this.shouldBeJerry)?;
      subparcel.write(&this.shouldBeByteBar)?;
      subparcel.write(&this.shouldBeIntBar)?;
      subparcel.write(&this.shouldBeLongBar)?;
      subparcel.write(&this.shouldContainTwoByteFoos)?;
      subparcel.write(&this.shouldContainTwoIntFoos)?;
      subparcel.write(&this.shouldContainTwoLongFoos)?;
      subparcel.write(&this.stringDefaultsToFoo)?;
      subparcel.write(&this.byteDefaultsToFour)?;
      subparcel.write(&this.intDefaultsToFive)?;
      subparcel.write(&this.longDefaultsToNegativeSeven)?;
      subparcel.write(&this.booleanDefaultsToTrue)?;
      subparcel.write(&this.charDefaultsToC)?;
      subparcel.write(&this.floatDefaultsToPi)?;
      subparcel.write(&this.doubleWithDefault)?;
      subparcel.write(&this.arrayDefaultsTo123)?;
      subparcel.write(&this.arrayDefaultsToEmpty)?;
      subparcel.write(&this.boolDefault)?;
      subparcel.write(&this.byteDefault)?;
      subparcel.write(&this.intDefault)?;
      subparcel.write(&this.longDefault)?;
      subparcel.write(&this.floatDefault)?;
      subparcel.write(&this.doubleDefault)?;
      subparcel.write(&this.checkDoubleFromFloat)?;
      subparcel.write(&this.checkStringArray1)?;
      subparcel.write(&this.checkStringArray2)?;
      subparcel.write(&this.int32_min)?;
      subparcel.write(&this.int32_max)?;
      subparcel.write(&this.int64_max)?;
      subparcel.write(&this.hexInt32_neg_1)?;
      subparcel.write(&this.ibinder)?;
      subparcel.write(&this.int32_1)?;
      subparcel.write(&this.int64_1)?;
      subparcel.write(&this.hexInt32_pos_1)?;
      subparcel.write(&this.hexInt64_pos_1)?;
      subparcel.write(&this.const_exprs_1)?;
      subparcel.write(&this.const_exprs_2)?;
      subparcel.write(&this.const_exprs_3)?;
      subparcel.write(&this.const_exprs_4)?;
      subparcel.write(&this.const_exprs_5)?;
      subparcel.write(&this.const_exprs_6)?;
      subparcel.write(&this.const_exprs_7)?;
      subparcel.write(&this.const_exprs_8)?;
      subparcel.write(&this.const_exprs_9)?;
      subparcel.write(&this.const_exprs_10)?;
      subparcel.write(&this.addString1)?;
      subparcel.write(&this.addString2)?;
      subparcel.write(&this.shouldSetBit0AndBit2)?;
      subparcel.write(&this.u)?;
      subparcel.write(&this.shouldBeConstS1)?;
      subparcel.write(&this.defaultWithFoo)?;
      Ok(())
    })
  }
}
binder::impl_deserialize_for_parcelable!(StructuredParcelable);
impl StructuredParcelable {
  fn deserialize_parcelable(&mut self, parcel: &binder::parcel::Parcel) -> binder::Result<()> {
    parcel.sized_read(|subparcel| {
      if subparcel.has_more_data() {
        self.shouldContainThreeFs = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.f = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldBeJerry = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldBeByteBar = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldBeIntBar = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldBeLongBar = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldContainTwoByteFoos = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldContainTwoIntFoos = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldContainTwoLongFoos = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.stringDefaultsToFoo = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.byteDefaultsToFour = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.intDefaultsToFive = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.longDefaultsToNegativeSeven = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.booleanDefaultsToTrue = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.charDefaultsToC = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.floatDefaultsToPi = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.doubleWithDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.arrayDefaultsTo123 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.arrayDefaultsToEmpty = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.boolDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.byteDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.intDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.longDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.floatDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.doubleDefault = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.checkDoubleFromFloat = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.checkStringArray1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.checkStringArray2 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.int32_min = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.int32_max = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.int64_max = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.hexInt32_neg_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.ibinder = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.int32_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.int64_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.hexInt32_pos_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.hexInt64_pos_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_2 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_3 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_4 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_5 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_6 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_7 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_8 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_9 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.const_exprs_10 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.addString1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.addString2 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldSetBit0AndBit2 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.u = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.shouldBeConstS1 = subparcel.read()?;
      }
      if subparcel.has_more_data() {
        self.defaultWithFoo = subparcel.read()?;
      }
      Ok(())
    })
  }
}

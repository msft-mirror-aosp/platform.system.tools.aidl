#![allow(non_upper_case_globals)]
#![allow(non_snake_case)]
#[allow(unused_imports)] use binder::IBinder;
use binder::declare_binder_interface;
declare_binder_interface! {
  INamedCallback["android.aidl.tests.INamedCallback"] {
    native: BnNamedCallback(on_transact),
    proxy: BpNamedCallback {
    },
  }
}
pub trait INamedCallback: binder::Interface + Send {
  fn get_descriptor() -> &'static str where Self: Sized { "android.aidl.tests.INamedCallback" }
  fn GetName(&self) -> binder::public_api::Result<String> {
    Err(binder::StatusCode::UNKNOWN_TRANSACTION.into())
  }
  fn getDefaultImpl() -> INamedCallbackDefault where Self: Sized {
    DEFAULT_IMPL.lock().unwrap().clone()
  }
  fn setDefaultImpl(d: INamedCallbackDefault) -> INamedCallbackDefault where Self: Sized {
    std::mem::replace(&mut *DEFAULT_IMPL.lock().unwrap(), d)
  }
}
pub mod transactions {
  #[allow(unused_imports)] use binder::IBinder;
  pub const GetName: binder::TransactionCode = binder::SpIBinder::FIRST_CALL_TRANSACTION + 0;
}
pub type INamedCallbackDefault = Option<std::sync::Arc<dyn INamedCallback + Sync>>;
use lazy_static::lazy_static;
lazy_static! {
  static ref DEFAULT_IMPL: std::sync::Mutex<INamedCallbackDefault> = std::sync::Mutex::new(None);
}
pub(crate) mod mangled { pub use super::INamedCallback as _7_android_4_aidl_5_tests_14_INamedCallback; }
impl INamedCallback for BpNamedCallback {
  fn GetName(&self) -> binder::public_api::Result<String> {
    let _aidl_reply = self.binder.transact(transactions::GetName, 0, |_aidl_data| {
      Ok(())
    });
    if let Err(binder::StatusCode::UNKNOWN_TRANSACTION) = _aidl_reply {
      if let Some(_aidl_default_impl) = <Self as INamedCallback>::getDefaultImpl() {
        return _aidl_default_impl.GetName();
      }
    }
    let _aidl_reply = _aidl_reply?;
    let _aidl_status: binder::Status = _aidl_reply.read()?;
    if !_aidl_status.is_ok() { return Err(_aidl_status); }
    let _aidl_return: String = _aidl_reply.read()?;
    Ok(_aidl_return)
  }
}
impl INamedCallback for binder::Binder<BnNamedCallback> {
  fn GetName(&self) -> binder::public_api::Result<String> { self.0.GetName() }
}
fn on_transact(_aidl_service: &dyn INamedCallback, _aidl_code: binder::TransactionCode, _aidl_data: &binder::parcel::Parcel, _aidl_reply: &mut binder::parcel::Parcel) -> binder::Result<()> {
  match _aidl_code {
    transactions::GetName => {
      let _aidl_return = _aidl_service.GetName();
      match &_aidl_return {
        Ok(_aidl_return) => {
          _aidl_reply.write(&binder::Status::from(binder::StatusCode::OK))?;
          _aidl_reply.write(_aidl_return)?;
        }
        Err(_aidl_status) => _aidl_reply.write(_aidl_status)?
      }
      Ok(())
    }
    _ => Err(binder::StatusCode::UNKNOWN_TRANSACTION)
  }
}

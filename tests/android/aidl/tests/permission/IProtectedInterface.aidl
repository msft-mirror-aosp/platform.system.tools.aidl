package android.aidl.tests.permission;

@EnforcePermission("ACCESS_FINE_LOCATION")
interface IProtectedInterface {
    void Method1();
    void Method2();
}

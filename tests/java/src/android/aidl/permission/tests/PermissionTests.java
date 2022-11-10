/*
 * Copyright (C) 2022, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.aidl.permission.tests;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertThrows;

import android.aidl.tests.permission.IProtected;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import org.junit.Before;
import org.junit.Test;
import org.junit.internal.TextListener;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class PermissionTests {
  private IProtected service;

  @Before
  public void setUp() throws RemoteException {
    // This service considers the INTERNET permission always granted.
    IBinder binder = ServiceManager.waitForService(IProtected.class.getName());
    assertNotNull(binder);
    service = IProtected.Stub.asInterface(binder);
    assertNotNull(service);
  }

  @Test
  public void testProtected() throws Exception {
    // Requires READ_PHONE_STATE.
    assertThrows(SecurityException.class, () -> service.PermissionProtected());
  }

  @Test
  public void testMultiplePermissionsAll() throws Exception {
    // Requires INTERNET and VIBRATE.
    assertThrows(SecurityException.class, () -> service.MultiplePermissionsAll());
  }

  @Test
  public void testMultiplePermissionsAny() throws Exception {
    // Requires INTERNET or VIBRATE.
    service.MultiplePermissionsAny();
  }

  @Test
  public void testNonManifestPermission() throws Exception {
    // Requires android.net.NetworkStack.PERMISSION_MAINLINE_NETWORK_STACK
    assertThrows(SecurityException.class, () -> service.NonManifestPermission());
  }

  public static void main(String[] args) {
    JUnitCore junit = new JUnitCore();
    junit.addListener(new TextListener(System.out));
    Result result = junit.run(PermissionTests.class);
    System.out.println(result.wasSuccessful() ? "TEST SUCCESS" : "TEST FAILURE");
  }
}
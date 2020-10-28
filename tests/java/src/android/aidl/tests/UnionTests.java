/*
 * Copyright (C) 2020, The Android Open Source Project
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

package android.aidl.tests;

import static org.hamcrest.core.Is.is;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

import android.aidl.tests.Union;
import android.aidl.tests.UnionWithFd;
import android.os.Parcel;
import android.os.ParcelFileDescriptor;
import android.os.Parcelable;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.List;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class UnionTests {
  @Test
  public void defaultConstructorInitsWithFirstField() {
    Union u = new Union(); // `int[] ns`
    assertThat(u.getTag(), is(Union.ns));
    assertNull(u.getNs());
  }

  @Test
  public void updatesUnionWithSetter() {
    Union u = new Union();
    u.setNs(new int[] {1, 2, 3});
    assertThat(u.getTag(), is(Union.ns));
    assertThat(u.getNs(), is(new int[] {1, 2, 3}));
  }

  @Test(expected = IllegalStateException.class)
  public void gettingWrongFieldThrowsException() {
    Union u = new Union();
    u.getSs();
  }

  @Test
  public void readWriteViaParcel() {
    List<String> ss = Arrays.asList("hello", "world");

    Union u = Union.ss(ss);
    Parcel parcel = Parcel.obtain();
    u.writeToParcel(parcel, 0);
    parcel.setDataPosition(0);

    Union v = Union.CREATOR.createFromParcel(parcel);

    assertThat(v.getTag(), is(Union.ss));
    assertThat(v.getSs(), is(ss));

    parcel.recycle();
  }

  @Test
  public void unionDescribeContents() {
    UnionWithFd u = UnionWithFd.num(0);
    assertTrue((u.describeContents() & Parcelable.CONTENTS_FILE_DESCRIPTOR) == 0);

    final Parcel parcel = Parcel.obtain();
    try {
      u.setPfd(ParcelFileDescriptor.open(new File("/system"), ParcelFileDescriptor.MODE_READ_ONLY));
    } catch (FileNotFoundException e) {
      throw new RuntimeException("can't open /system", e);
    }
    assertTrue((u.describeContents() & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0);

    u.writeToParcel(parcel, 0);

    UnionWithFd v = UnionWithFd.num(0);
    parcel.setDataPosition(0);
    v.readFromParcel(parcel);
    assertTrue((v.describeContents() & Parcelable.CONTENTS_FILE_DESCRIPTOR) != 0);

    parcel.recycle();
  }
}

/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.aidl.tests;

import android.aidl.tests.LongEnum;

parcelable FixedSize {
    @FixedSize
    parcelable FixedParcelable {
        boolean booleanValue;
        byte byteValue;
        char charValue;
        int intValue;
        long longValue;
        float floatValue;
        int[3] intArray;
        long[3][2] multiDimensionLongArray;
        double doubleValue;
        LongEnum enumValue = LongEnum.FOO;
        FixedUnion parcelableValue;
        EmptyParcelable[3] parcelableArray;
        FixedUnion[4] unionArray;
    }

    @FixedSize
    parcelable EmptyParcelable {}

    @FixedSize
    union FixedUnion {
        boolean booleanValue = false;
        byte byteValue;
        char charValue;
        int intValue;
        long longValue;
        float floatValue;
        int[3] intArray;
        long[3][2] multiDimensionLongArray;
        double doubleValue;
        LongEnum enumValue;
    }

    /* A union with no padding between the tag and value */
    @FixedSize
    union FixedUnionNoPadding {
        byte byteValue;
    }

    /* A union with one byte of padding between the tag and value */
    @FixedSize
    union FixedUnionSmallPadding {
        char charValue;
    }

    /* A union with seven bytes of padding between the tag and value */
    @FixedSize
    union FixedUnionLongPadding {
        long longValue;
    }
}

/*
 * Copyright (C) 2023, The Android Open Source Project
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

#pragma once

#include <string>

#include "aidl_language.h"
#include "code_writer.h"
#include "options.h"

// This is used to help generate code targeting to any language

namespace android {
namespace aidl {

// currently relies on all backends having the same comment style, but we
// could take a comment type argument in the future
void GenerateAutoGenHeader(CodeWriter& out, const Options& options);

}  // namespace aidl
}  // namespace android

#!/usr/bin/env bash

# Copyright (C) 2020 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

if [ $# != 1 ]; then
  echo "Usage: golden_test.sh [check|update]"
fi

function _golden_test() {
  local update=0
  case $1 in
    check)
      update=0
      ;;
    update)
      update=1
      ;;
    *)
      echo "Argument must be 'check' or 'update' but is $1"
      exit 1
      ;;
  esac

  # warning: this list must be kept in sync with system/tools/aidl/Android.bp
  modules=(
    "aidl-test-interface-cpp-source"
    "aidl-test-interface-java-source"
    "aidl-test-versioned-interface-V1-cpp-source"
    "aidl-test-versioned-interface-V1-java-source"
    "aidl-test-versioned-interface-V1-ndk-source"
    "aidl-test-versioned-interface-V1-rust-source"
    "aidl-test-versioned-interface-V2-cpp-source"
    "aidl-test-versioned-interface-V2-java-source"
    "aidl-test-versioned-interface-V2-ndk-source"
    "aidl-test-versioned-interface-V2-rust-source"
    "aidl-test-versioned-interface-V3-cpp-source"
    "aidl-test-versioned-interface-V3-java-source"
    "aidl-test-versioned-interface-V3-ndk-source"
    "aidl-test-versioned-interface-V3-rust-source"
    "aidl-test-interface-ndk-source"
    "aidl-test-interface-rust-source"
    "aidl_test_loggable_interface-cpp-source"
    "aidl_test_loggable_interface-java-source"
    "aidl_test_loggable_interface-ndk-source"
    "aidl-test-interface-permission-java-source"
    "aidl-test-fixedsizearray-cpp-source"
    "aidl-test-fixedsizearray-java-source"
    "aidl-test-fixedsizearray-ndk-source"
    "aidl-test-fixedsizearray-rust-source"
    "aidl-test-interface-cpp-analyzer-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V1-cpp-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V2-cpp-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V1-ndk-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V2-ndk-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V1-java-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V2-java-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V1-rust-source"
    "tests/trunk_stable_test/android.aidl.test.trunk-V2-rust-source"
  )

  local use_unfrozen="$3"
  # If override_unfrozen is true, we override the RELEASE_AIDL_USE_UNFROZEN build flag with an
  # environment variable.
  local override_unfrozen="$4"
  local root="$2"
  if [ "$update" = 1 ]; then
    if [ "$override_unfrozen" == true ]; then
      export AIDL_USE_UNFROZEN_OVERRIDE="$use_unfrozen"
    fi
    "$root"/build/soong/soong_ui.bash --make-mode \
      $(for i in "${modules[@]}"; do
          echo "out/soong/.intermediates/system/tools/aidl/$i/timestamp"
        done) ; export AIDL_USE_UNFROZEN_OVERRIDE=
  fi
  local e=0
  for module in "${modules[@]}"; do
    local built="$root/out/soong/.intermediates/system/tools/aidl/$module/"
    local module_path
    if [ "$use_unfrozen" == "True" ]; then
      module_path=$module
    else
      module_path="frozen/$module"
    fi

    local golden="$root/system/tools/aidl/tests/golden_output/$module_path/"

    if [ "$update" = 1 ]; then
      rm -rf "$golden"
      mkdir -p "$golden"
      cp -r "$built/gen" "$golden"
    else
      diff -rN "$built/gen" "$golden/gen" || e=1
    fi
  done

  if [ "$e" = 1 ]; then
    echo "ERROR: The AIDL compiler is outputting files in an unknown way."
    echo "ERROR: to accept these changes, please run:"
    echo "ERROR:     \$ANDROID_BUILD_TOP/system/tools/aidl/tests/golden_test.sh update"
    exit 1
  else
    if [ "$update" = 1 ]; then
      echo "UPDATE GOLDEN TEST SUCCESS"
    fi
  fi
}

root="."
if [ "$ANDROID_BUILD_TOP" != "" ]; then
  root="$ANDROID_BUILD_TOP"
fi
use_unfrozen=$(sed -rn 's/ *flag\(\"RELEASE_AIDL_USE_UNFROZEN\", ALL, (.*)\),/\1/p' "$root/build/release/build_flags.bzl")
if [[ "$use_unfrozen" != "True" && "$use_unfrozen" != "False" ]]; then
  echo "ERROR: Unexpected value of RELEASE_AIDL_USE_UNFROZEN in $root/build/release/build_flags.bzl of $use_unfrozen."
  exit 1
fi

if [ "$1" == "update" ]; then
  if [ "$use_unfrozen" == "True" ]; then
    # build update the opposite value first, so we leave the intermediates
    # in the state of the tree
    _golden_test "$@" "$root" "False" true
    _golden_test "$@" "$root" "True" false
  else
    _golden_test "$@" "$root" "True" true
    _golden_test "$@" "$root" "False" false
  fi
else
_golden_test "$@" "$root" "$use_unfrozen"
fi

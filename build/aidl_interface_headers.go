// Copyright (C) 2022 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package aidl

import (
	"path/filepath"

	"android/soong/android"

	"github.com/google/blueprint"
	"github.com/google/blueprint/proptools"
)

func init() {
	android.RegisterModuleType("aidl_interface_headers", aidlInterfaceHeadersFactory)
}

type AidlInterfaceHeadersInfo struct {
	Srcs       android.Paths
	IncludeDir string
}

var AidlInterfaceHeadersProvider = blueprint.NewProvider(AidlInterfaceHeadersInfo{})

type aidlInterfaceHeadersProperties struct {
	// List of .aidl files which compose this interface.
	Srcs []string `android:"path"`

	// Relative path for includes. assumes AIDL path is relative to current directory.
	Include_dir *string
}

type aidlInterfaceHeaders struct {
	android.ModuleBase

	properties aidlInterfaceHeadersProperties

	srcs android.Paths
}

// Modules which provide AIDL sources that are only used to provide "-I" flags to the
// aidl tool. No language bindings are generated from these modules. Typically this will
// be used to provide includes for UnstructuredParcelable AIDL definitions such as those
// coming from framework modules.
func aidlInterfaceHeadersFactory() android.Module {
	i := &aidlInterfaceHeaders{}
	i.AddProperties(&i.properties)
	android.InitAndroidModule(i)
	return i
}

func (i *aidlInterfaceHeaders) GenerateAndroidBuildActions(ctx android.ModuleContext) {
	ctx.SetProvider(AidlInterfaceHeadersProvider, AidlInterfaceHeadersInfo{
		Srcs:       android.PathsForModuleSrc(ctx, i.properties.Srcs),
		IncludeDir: filepath.Join(ctx.ModuleDir(), proptools.String(i.properties.Include_dir)),
	})
}

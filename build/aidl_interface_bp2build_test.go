package aidl

import (
	"testing"

	"android/soong/aidl_library"
	"android/soong/android"
	"android/soong/bp2build"
	"android/soong/cc"
)

func runAidlInterfaceTestCase(t *testing.T, tc bp2build.Bp2buildTestCase) {
	t.Helper()
	tc.ExtraFixturePreparer = android.FixtureModifyContext(func(ctx *android.TestContext) {
		ctx.PreArchBp2BuildMutators(registerPreArchMutators)
	})
	bp2build.RunBp2BuildTestCase(
		t,
		func(ctx android.RegistrationContext) {
			ctx.RegisterModuleType("aidl_interface", AidlInterfaceFactory)
			ctx.RegisterModuleType("aidl_library", aidl_library.AidlLibraryFactory)
			ctx.RegisterModuleType("cc_library_shared", cc.LibrarySharedFactory)
		},
		tc,
	)
}

func TestAidlInterface(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with single "latest" aidl_interface import`,
		Blueprint: `
			aidl_library {
				name: "aidl-interface-headers",
			}
			aidl_interface {
				name: "aidl-interface-import",
				versions: [
					"1",
					"2",
				],
			}
			aidl_interface {
				name: "aidl-interface1",
				flags: ["-Wsomething"],
				imports: [
					"aidl-interface-import-V1",
				],
				headers: [
					"aidl-interface-headers",
				],
				versions: [
					"1",
					"2",
					"3",
				],
			}`,
		ExpectedConvertedModules: []string{"aidl-interface1", "aidl-interface-headers", "aidl-interface-import"},
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_library", "aidl-interface-headers", bp2build.AttrNameToString{
				"tags": `["apex_available=//apex_available:anyapex"]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface-import", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"versions_with_info": `[
        {
        "version": "1",
    },
        {
        "version": "2",
    },
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"hdrs":  `[":aidl-interface-headers"]`,
				"flags": `["-Wsomething"]`,
				"versions_with_info": `[
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "1",
    },
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "2",
    },
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "3",
    },
    ]`,
			}),
		},
	})
}

func TestAidlInterfaceWithIncludeDirsNotGenerated(t *testing.T) {
	runAidlInterfaceTestCase(t,
		bp2build.Bp2buildTestCase{
			Description: `aidl_interface with include_dirs property does not generate a Bazel target`,
			Blueprint: `
				aidl_interface {
					name: "aidl-interface1",
					include_dirs: ["dir1"],
				}
			`,
			ExpectedBazelTargets: []string{},
		},
	)
}

func TestAidlInterfaceWithNoProperties(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface no properties set`,
		Blueprint: `
			aidl_interface {
				name: "aidl-interface1",
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithDisabledBackends(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with some backends disabled`,
		Blueprint: `
			aidl_interface {
				name: "aidl-interface1",
				backend: {
					ndk: {
						enabled: false,
					},
					cpp: {
						enabled: false,
					},
				},
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithVersionImport(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with version aidl_interface import`,
		Blueprint: `
			aidl_interface {
				name: "aidl-interface-import",
				versions: [
					"1",
					"2",
				],
			}
			aidl_interface {
				name: "aidl-interface1",
				imports: [
					"aidl-interface-import-V1",
				],
				versions: [
					"1",
					"2",
					"3",
				],
			}`,
		ExpectedConvertedModules:   []string{"aidl-interface-import", "aidl-interface1"},
		ExpectedHandcraftedModules: []string{"aidl-interface1_interface"},
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface-import", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"versions_with_info": `[
        {
        "version": "1",
    },
        {
        "version": "2",
    },
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"versions_with_info": `[
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "1",
    },
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "2",
    },
        {
        "deps": [":aidl-interface-import-V1"],
        "version": "3",
    },
    ]`,
			}),
		},
	})
}

func TestAidlInterfaceWithVersionedImport(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with single versioned aidl_interface import`,
		Blueprint: `
			aidl_interface {
				name: "aidl-interface-import",
				versions: [
					"1",
					"2",
				],
			}
			aidl_interface {
				name: "aidl-interface1",
				imports: [
					"aidl-interface-import-V2",
				],
				versions: [
					"1",
					"2",
					"3",
				],
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface-import", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"versions_with_info": `[
        {
        "version": "1",
    },
        {
        "version": "2",
    },
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"java_config": `{
        "enabled": True,
    }`,
				"cpp_config": `{
        "enabled": True,
    }`,
				"ndk_config": `{
        "enabled": True,
    }`,
				"versions_with_info": `[
        {
        "deps": [":aidl-interface-import-V2"],
        "version": "1",
    },
        {
        "deps": [":aidl-interface-import-V2"],
        "version": "2",
    },
        {
        "deps": [":aidl-interface-import-V2"],
        "version": "3",
    },
    ]`,
			}),
		},
	})
}

func TestAidlInterfaceWithCppAndNdkConfigs(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with cpp and ndk configs`,
		Blueprint: `
			aidl_interface {
				name: "foo",
                backend: {
                    java: {
                        enabled: false,
                    },
                    cpp: {
                        min_sdk_version: "2",
                    },
                    ndk: {
                        min_sdk_version: "1",
                    },
                }
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "foo", bp2build.AttrNameToString{
				"cpp_config": `{
        "enabled": True,
        "min_sdk_version": "2",
    }`,
				"ndk_config": `{
        "enabled": True,
        "min_sdk_version": "1",
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithUnstablePropSet(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with unstable prop set`,
		Blueprint: `
			aidl_interface {
				name: "foo",
				unstable: true,
                backend: {
                    java: {
                        enabled: false,
                    },
                    cpp: {
                        enabled: false,
                    },
                }
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "foo", bp2build.AttrNameToString{
				"unstable": "True",
				"ndk_config": `{
        "enabled": True,
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithFrozenPropSet(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with frozen prop set`,
		Blueprint: `
			aidl_interface {
				name: "foo",
				frozen: true,
				versions: ["1"],
                backend: {
                    java: {
                        enabled: false,
                    },
                    cpp: {
                        enabled: false,
                    },
                }
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "foo", bp2build.AttrNameToString{
				"frozen": "True",
				"versions_with_info": `[{
        "version": "1",
    }]`,
				"ndk_config": `{
        "enabled": True,
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithApexAvailable(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface apex_available`,
		Blueprint: `
			aidl_interface {
				name: "aidl-interface1",
                backend: {
                    java: {
                        enabled: false,
                    },
                    cpp: {
                        enabled: false,
                    },
                    ndk: {
                        enabled: true,
                        apex_available: [
                            "com.android.abd",
                            "//apex_available:platform",
                        ],
                    },
                }
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"ndk_config": `{
        "enabled": True,
        "tags": [
            "apex_available=com.android.abd",
            "apex_available=//apex_available:platform",
        ],
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithAdditionalDynamicDeps(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description:             `aidl_interface apex_available`,
		StubbedBuildDefinitions: []string{"shared_dep", "shared_stub_dep"},
		Blueprint: `
			cc_library_shared {
				name: "shared_dep",
			}
			cc_library_shared {
				name: "shared_stub_dep",
				stubs: {
				    symbol_file: "libnativewindow.map.txt",
				    versions: ["29"],
				},
			}
			aidl_interface {
				name: "aidl-interface1",
				backend: {
					java: {
						enabled: false,
					},
					cpp: {
						enabled: true,
						apex_available: ["com.android.myapex"],
						additional_shared_libraries: [
							"shared_dep",
							"shared_stub_dep",
						],
					},
					ndk: {
						enabled: true,
						apex_available: ["com.android.myapex"],
						additional_shared_libraries: [
							"shared_dep",
							"shared_stub_dep",
						],
					},
				}
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"cpp_config": `{
        "additional_dynamic_deps": [":shared_dep"] + select({
            "//build/bazel/rules/apex:com.android.myapex": ["@api_surfaces//module-libapi/current:shared_stub_dep"],
            "//conditions:default": [":shared_stub_dep"],
        }),
        "enabled": True,
        "tags": ["apex_available=com.android.myapex"],
    }`,
				"ndk_config": `{
        "additional_dynamic_deps": [":shared_dep"] + select({
            "//build/bazel/rules/apex:com.android.myapex": ["@api_surfaces//module-libapi/current:shared_stub_dep"],
            "//conditions:default": [":shared_stub_dep"],
        }),
        "enabled": True,
        "tags": ["apex_available=com.android.myapex"],
    }`,
			}),
		},
	})
}

func TestAidlInterfaceWithCppBackend(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface apex_available`,
		Blueprint: `
			cc_library_shared {
				name: "shared_dep",
			}
			cc_library_shared {
				name: "shared_stub_dep",
				stubs: {
				    symbol_file: "libnativewindow.map.txt",
				    versions: ["29"],
				},
			}
			aidl_interface {
				name: "aidl-interface1",
				srcs: [
					"IFoo.aidl",
				],
				backend: {
					java: {
						enabled: false,
					},
					cpp: {
						enabled: true,
						apex_available: ["com.android.myapex"],
						additional_shared_libraries: [
							"shared_dep",
							"shared_stub_dep",
						],
					},
					ndk: {
						enabled: false,
					},
				}
			}`,
		StubbedBuildDefinitions:    []string{"shared_dep", "shared_stub_dep"},
		ExpectedHandcraftedModules: []string{"aidl-interface1_interface", "aidl-interface1-V1-cpp"},
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"cpp_config": `{
        "additional_dynamic_deps": [":shared_dep"] + select({
            "//build/bazel/rules/apex:com.android.myapex": ["@api_surfaces//module-libapi/current:shared_stub_dep"],
            "//conditions:default": [":shared_stub_dep"],
        }),
        "enabled": True,
        "tags": ["apex_available=com.android.myapex"],
    }`,
				"srcs": `["IFoo.aidl"]`,
			}),
		},
	})
}

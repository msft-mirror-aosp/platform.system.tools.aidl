package aidl

import (
	"android/soong/android"
	"android/soong/bp2build"
	"testing"
)

func runAidlInterfaceTestCase(t *testing.T, tc bp2build.Bp2buildTestCase) {
	t.Helper()
	bp2build.RunBp2BuildTestCase(
		t,
		func(ctx android.RegistrationContext) {
			ctx.RegisterModuleType("aidl_interface", AidlInterfaceFactory)
			ctx.RegisterModuleType("aidl_interface_headers", AidlInterfaceHeadersFactory)
		},
		tc,
	)
}

func TestAidlInterfaceHeaders(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface_headers`,
		Blueprint: `
			aidl_interface_headers {
				name: "aidl-interface-headers",
				include_dir: "src",
				srcs: [
					"src/A.aidl",
				],
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_library", "aidl-interface-headers", bp2build.AttrNameToString{
				"strip_import_prefix": `"src"`,
				"hdrs":                `["src/A.aidl"]`,
			}),
		},
	})
}

func TestAidlInterface(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with single "latest" aidl_interface import`,
		Blueprint: `
			aidl_interface_headers {
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
				flags: ["--flag1"],
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
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_library", "aidl-interface-headers", bp2build.AttrNameToString{}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface-import", bp2build.AttrNameToString{
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"versions": `[
        "1",
        "2",
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"deps": `[
        ":aidl-interface-import-V1",
        ":aidl-interface-headers",
    ]`,
				"flags": `["--flag1"]`,
				"versions": `[
        "1",
        "2",
        "3",
    ]`,
			}),
		},
	})
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
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
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
				"backends": `["java"]`,
			}),
		},
	})
}

func TestAidlInterfaceWithLatestImport(t *testing.T) {
	runAidlInterfaceTestCase(t, bp2build.Bp2buildTestCase{
		Description: `aidl_interface with single "latest" aidl_interface import`,
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
					"aidl-interface-import",
				],
				versions: [
					"1",
					"2",
					"3",
				],
			}`,
		ExpectedBazelTargets: []string{
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface-import", bp2build.AttrNameToString{
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"versions": `[
        "1",
        "2",
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"deps": `[":aidl-interface-import-latest"]`,
				"versions": `[
        "1",
        "2",
        "3",
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
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"versions": `[
        "1",
        "2",
    ]`,
			}),
			bp2build.MakeBazelTargetNoRestrictions("aidl_interface", "aidl-interface1", bp2build.AttrNameToString{
				"backends": `[
        "cpp",
        "java",
        "ndk",
    ]`,
				"deps": `[":aidl-interface-import-V2"]`,
				"versions": `[
        "1",
        "2",
        "3",
    ]`,
			}),
		},
	})
}

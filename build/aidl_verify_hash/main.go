// Copyright 2026 Google Inc. All rights reserved.
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

package main

import (
	"bufio"
	"crypto/sha1"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"android/soong/response"
)

func main() {
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "Usage: %s <input_list_file> <api_dir> <version> <hash_file> <out> <message_file>\n", os.Args[0])
	}
	flag.Parse()
	args := flag.Args()

	if len(args) != 6 {
		flag.Usage()
		os.Exit(1)
	}

	inputListFile := args[0]
	apiDir := args[1]
	version := args[2]
	hashFile := args[3]
	outFile := args[4]
	messageFile := args[5]

	// 1. Read and sort .aidl files from inputListFile
	f := must2(os.Open(inputListFile))
	files := must2(response.ReadRspFile(f))
	must(f.Close())
	for i, f := range files {
		rel := must2(filepath.Rel(apiDir, f))
		files[i] = "./" + rel
	}
	sort.Strings(files)

	// 2. Compute hashes of files and combine with version. This has to be careful to match
	// the exact hash format that was used historically with this shell command:
	// cd '${apiDir}' && { find ./ -name "*.aidl" -print0 | LC_ALL=C sort -z | xargs -0 sha1sum && echo ${version}; } | sha1sum | cut -d " " -f 1
	h := sha1.New()
	for _, file := range files {
		f := must2(os.Open(filepath.Join(apiDir, file)))
		fh := sha1.New()
		must2(io.Copy(fh, f))
		f.Close()
		fmt.Fprintf(h, "%x  %s\n", fh.Sum(nil), file)
	}
	fmt.Fprintln(h, version)

	actualHash := fmt.Sprintf("%x", h.Sum(nil))

	// 3. Read expected hash (last line of hashFile)
	expectedHash := readLastLine(hashFile)

	// 4. Compare and act
	if actualHash == expectedHash {
		must(os.WriteFile(outFile, []byte{}, 0666))
	} else {
		fmt.Print(string(must2(os.ReadFile(messageFile))))
		os.Exit(1)
	}
}

func readLastLine(path string) string {
	file := must2(os.Open(path))
	defer file.Close()

	var lastLine string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line != "" {
			lastLine = line
		}
	}
	must(scanner.Err())
	return lastLine
}

func must(err error) {
	if err != nil {
		fmt.Fprintf(os.Stderr, "%s\n", err)
		os.Exit(1)
	}
}

func must2[T any](x T, err error) T {
	if err != nil {
		fmt.Fprintf(os.Stderr, "%s\n", err)
		os.Exit(1)
	}
	return x
}

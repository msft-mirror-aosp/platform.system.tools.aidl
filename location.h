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

#pragma once

#include <iostream>
#include <optional>
#include <string>

class AidlLocation {
 public:
  struct Point {
    int line;
    int column;
  };

  enum class Source {
    // From internal aidl source code
    INTERNAL,
    // From a parsed file
    EXTERNAL,
    // Derived from a parsed file. These are used for types generated by
    // the compiler that we still want to track mostly like EXTERNAL types.
    // An example is the Tag enum that is generated for each EXTERNAL union.
    DERIVED_INTERNAL,
  };

  AidlLocation(const std::string& file, Point begin, Point end, Source source);
  AidlLocation(const std::string& file, Source source)
      : AidlLocation(file, {0, 0}, {0, 0}, source) {}

  bool IsInternal() const { return source_ == Source::INTERNAL; }
  bool IsDerived() const { return source_ == Source::DERIVED_INTERNAL; }

  // The first line of a file is line 1.
  bool LocationKnown() const { return begin_.line != 0; }

  std::string GetFile() const { return file_; }

  // Get an AidlLocation derived from this external location.
  // nullopt if this location is not EXTERNAL
  std::optional<AidlLocation> ToDerivedLocation() const;

  friend std::ostream& operator<<(std::ostream& os, const AidlLocation& l);
  friend class AidlNode;

 private:
  // INTENTIONALLY HIDDEN: only operator<< should access details here.
  // Otherwise, locations should only ever be copied around to construct new
  // objects.
  const std::string file_;
  Point begin_;
  Point end_;
  Source source_;
};

#define AIDL_LOCATION_HERE \
  (AidlLocation{__FILE__, {__LINE__, 0}, {__LINE__, 0}, AidlLocation::Source::INTERNAL})

std::ostream& operator<<(std::ostream& os, const AidlLocation& l);

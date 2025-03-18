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

#include "location.h"

AidlLocation::AidlLocation(const std::string& file, Point begin, Point end, Source source)
    : file_(file), begin_(begin), end_(end), source_(source) {}

std::ostream& operator<<(std::ostream& os, const AidlLocation& l) {
  if (l.source_ == AidlLocation::Source::DERIVED_INTERNAL) {
    os << "(derived from)";
  }
  os << l.file_;
  if (l.LocationKnown()) {
    os << ":" << l.begin_.line << "." << l.begin_.column << "-";
    if (l.begin_.line != l.end_.line) {
      os << l.end_.line << ".";
    }
    os << l.end_.column;
  }
  return os;
}

std::optional<AidlLocation> AidlLocation::ToDerivedLocation() const {
  if (source_ != Source::EXTERNAL) return std::nullopt;
  return AidlLocation(file_, begin_, end_, Source::DERIVED_INTERNAL);
}

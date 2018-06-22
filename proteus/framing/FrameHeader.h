// Copyright (c) 2018-present, Netifi Inc.
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

#pragma once

#include <iosfwd>

#include "proteus/framing/FrameFlags.h"
#include "proteus/framing/FrameType.h"
#include "rsocket/internal/Common.h"

namespace proteus {

class FrameHeader {
 public:
  FrameHeader() {}

  FrameHeader(FrameType ty, FrameFlags fflags, rsocket::StreamId stream)
      : type{ty}, flags{fflags}, streamId{stream} {}

  bool flagsComplete() const {
    return !!(flags & FrameFlags::COMPLETE);
  }

  bool flagsNext() const {
    return !!(flags & FrameFlags::NEXT);
  }

  bool flagsFollows() const {
    return !!(flags & FrameFlags::FOLLOWS);
  }

  FrameType type{FrameType::RESERVED};
  FrameFlags flags{FrameFlags::EMPTY};
    rsocket::StreamId streamId{0};
};

std::ostream& operator<<(std::ostream&, const FrameHeader&);

} // namespace proteus

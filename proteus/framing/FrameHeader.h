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

#include "proteus/framing/FrameType.h"
#include "rsocket/internal/Common.h"

namespace proteus {

class FrameHeader {
 public:
  FrameHeader() {}

  FrameHeader(uint16_t _major, uint16_t _minor, FrameType ty)
      : major(_major), minor(_minor), type{ty} {}

  uint16_t major{};
  uint16_t minor{};
  FrameType type{FrameType::UNDEFINED};
};

std::ostream& operator<<(std::ostream&, const FrameHeader&);

} // namespace proteus

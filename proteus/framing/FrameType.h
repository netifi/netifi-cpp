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

#include <cstdint>
#include <iosfwd>

#include <folly/Range.h>

namespace proteus {

enum class FrameType : uint8_t {
  UNDEFINED = 0x00,
  BROKER_SETUP = 0x01,
  DESTINATION_SETUP = 0x02,
  DESTINATION = 0x03,
  GROUP = 0x04,
  BROADCAST = 0x05,
  SHARD = 0x06,
};

folly::StringPiece toString(FrameType);

std::ostream& operator<<(std::ostream&, FrameType);

} // namespace proteus

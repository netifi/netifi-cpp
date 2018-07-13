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

namespace proteus {

enum class FrameFlags : uint16_t {
  EMPTY = 0x000,
  IGNORE = 0x200,
  METADATA = 0x100,

  // BROKER_SETUP.
  RESUME_ENABLE = 0x80,
  LEASE = 0x40,

  // DESTINATION
  KEEPALIVE_RESPOND = 0x80,

  // GROUP, BROADCAST, SHARD, REQUEST_CHANNEL, PAYLOAD.
  FOLLOWS = 0x80,

  // REQUEST_CHANNEL, PAYLOAD.
  COMPLETE = 0x40,

  // PAYLOAD.
  NEXT = 0x20,
};

constexpr uint16_t raw(FrameFlags flags) {
  return static_cast<uint16_t>(flags);
}

constexpr FrameFlags operator|(FrameFlags a, FrameFlags b) {
  return static_cast<FrameFlags>(raw(a) | raw(b));
}

constexpr FrameFlags operator&(FrameFlags a, FrameFlags b) {
  return static_cast<FrameFlags>(raw(a) & raw(b));
}

inline FrameFlags& operator|=(FrameFlags& a, FrameFlags b) {
  return a = (a | b);
}

inline FrameFlags& operator&=(FrameFlags& a, FrameFlags b) {
  return a = (a & b);
}

constexpr bool operator!(FrameFlags a) {
  return !raw(a);
}

constexpr FrameFlags operator~(FrameFlags a) {
  return static_cast<FrameFlags>(~raw(a));
}

std::ostream& operator<<(std::ostream& ostr, FrameFlags a);

} // namespace proteus

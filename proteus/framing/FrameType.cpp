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

#include "proteus/framing/FrameType.h"

#include <ostream>

#include <glog/logging.h>

namespace proteus {

constexpr folly::StringPiece kUnknown{"UNKNOWN_FRAME_TYPE"};

folly::StringPiece toString(FrameType type) {
  switch (type) {
    case FrameType::UNDEFINED:
      return "UNDEFINED";
    case FrameType::BROKER_SETUP:
      return "BROKER_SETUP";
    case FrameType::DESTINATION_SETUP:
      return "DESTINATION_SETUP";
    case FrameType::DESTINATION:
      return "DESTINATION";
    case FrameType::GROUP:
      return "GROUP";
    case FrameType::BROADCAST:
      return "BROADCAST";
    case FrameType::SHARD:
      return "SHARD";
    default:
      DLOG(FATAL) << "Unknown frame type";
      return kUnknown;
  }
}

std::ostream& operator<<(std::ostream& os, FrameType type) {
  auto const str = toString(type);
  if (str == kUnknown) {
    return os << "Unknown FrameType[" << static_cast<int>(type) << "]";
  }
  return os << str;
}
} // namespace proteus

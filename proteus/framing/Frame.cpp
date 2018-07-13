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

#include "proteus/framing/Frame.h"

#include <folly/Memory.h>
#include <folly/io/Cursor.h>
#include <map>
#include <sstream>

#include "rsocket/RSocketParameters.h"

namespace proteus {

namespace detail {

FrameFlags getFlags(const rsocket::Payload& p) {
  return p.metadata ? FrameFlags::METADATA : FrameFlags::EMPTY;
}

void checkFlags(const rsocket::Payload& p, FrameFlags flags) {
  if (bool(p.metadata) != bool(flags & FrameFlags::METADATA)) {
    throw std::invalid_argument{
        "Value of METADATA flag doesn't match payload metadata"};
  }
}

} // namespace detail

std::ostream& operator<<(std::ostream& os, const Frame_BROKER_SETUP& frame) {
  return os << frame.header_ << "(" << frame.requestN_ << ")";
}

std::ostream& operator<<(std::ostream& os, const Frame_DESTINATION_SETUP& frame) {
  return os << frame.header_ << ", " << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_DESTINATION& frame) {
  return os << frame.header_ << ", " << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_GROUP& frame) {
  return os << frame.header_ << ", "
            << (frame.metadata_ ? frame.metadata_->computeChainDataLength()
                                : 0);
}

std::ostream& operator<<(std::ostream& os, const Frame_BROADCAST& frame) {
  return os << frame.header_;
}

std::ostream& operator<<(std::ostream& os, const Frame_SHARD& frame) {
  return os << frame.header_;
}

} // namespace proteus

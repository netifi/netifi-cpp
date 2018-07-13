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

#include <array>
#include <iosfwd>
#include <limits>

#include <folly/io/IOBuf.h>
#include <folly/io/IOBufQueue.h>

#include "rsocket/Payload.h"
#include "proteus/framing/FrameHeader.h"
#include "proteus/framing/FrameType.h"
#include "rsocket/internal/Common.h"

namespace folly {
template <typename V>
class Optional;
namespace io {
class Cursor;
class QueueAppender;
} // namespace io
} // namespace folly

namespace proteus {

namespace detail {

FrameFlags getFlags(const rsocket::Payload&);

void checkFlags(const rsocket::Payload&, FrameFlags);

} // namespace detail

/// Frames do not form hierarchy, as we never perform type erasure on a frame.
/// We use inheritance only to save code duplication.
///
/// Since frames are only meaningful for stream automata on both ends of a
/// stream, intermediate layers that are frame-type-agnostic pass around
/// serialized frame.

class Frame_BROKER_SETUP {
 public:
  /*
   * Maximum value for ReactiveSocket Subscription::request.
   * Value is a signed int, however negative values are not allowed.
   *
   * n.b. this is less than size_t because of the Frame encoding restrictions.
   */
  static constexpr int64_t kMaxRequestN = rsocket::kMaxRequestN;

  Frame_BROKER_SETUP() = default;
  Frame_BROKER_SETUP(rsocket::StreamId streamId, uint32_t requestN)
      : header_(FrameType::BROKER_SETUP, FrameFlags::EMPTY, streamId),
        requestN_(requestN) {
    DCHECK(requestN_ > 0);
    DCHECK(requestN_ <= kMaxRequestN);
  }

  FrameHeader header_;
  uint32_t requestN_{};
};
std::ostream& operator<<(std::ostream&, const Frame_BROKER_SETUP&);

class Frame_DESTINATION_SETUP {
 public:
  constexpr static const FrameFlags AllowedFlags =
      FrameFlags::METADATA | FrameFlags::FOLLOWS;

  Frame_DESTINATION_SETUP() = default;
  Frame_DESTINATION_SETUP(rsocket::StreamId streamId, FrameFlags flags, rsocket::Payload payload)
      : header_(
            FrameType::DESTINATION_SETUP,
            (flags & AllowedFlags) | detail::getFlags(payload),
            streamId),
        payload_(std::move(payload)) {
    detail::checkFlags(payload_, header_.flags);
  }

  FrameHeader header_;
  rsocket::Payload payload_;
};
std::ostream& operator<<(std::ostream&, const Frame_DESTINATION_SETUP&);

class Frame_DESTINATION {
 public:
  constexpr static const FrameFlags AllowedFlags =
      FrameFlags::METADATA | FrameFlags::FOLLOWS;

  Frame_DESTINATION() = default;
  Frame_DESTINATION(rsocket::StreamId streamId, FrameFlags flags, rsocket::Payload payload)
      : header_(
            FrameType::DESTINATION,
            (flags & AllowedFlags) | detail::getFlags(payload),
            streamId),
        payload_(std::move(payload)) {
    detail::checkFlags(payload_, header_.flags);
  }

  FrameHeader header_;
  rsocket::Payload payload_;
};
std::ostream& operator<<(std::ostream&, const Frame_DESTINATION&);

class Frame_GROUP {
 public:
  Frame_GROUP() {}
  explicit Frame_GROUP(std::unique_ptr<folly::IOBuf> metadata)
      : header_(FrameType::GROUP, FrameFlags::METADATA, 0),
        metadata_(std::move(metadata)) {
    CHECK(metadata_);
  }

  FrameHeader header_;
  std::unique_ptr<folly::IOBuf> metadata_;
};
std::ostream& operator<<(std::ostream&, const Frame_GROUP&);

class Frame_BROADCAST {
 public:
  Frame_BROADCAST() = default;
  explicit Frame_BROADCAST(rsocket::StreamId streamId)
      : header_(FrameType::BROADCAST, FrameFlags::EMPTY, streamId) {}

  FrameHeader header_;
};
std::ostream& operator<<(std::ostream&, const Frame_BROADCAST&);

class Frame_SHARD {
public:
    Frame_SHARD() = default;
    explicit Frame_SHARD(rsocket::StreamId streamId)
            : header_(FrameType::SHARD, FrameFlags::EMPTY, streamId) {}

    FrameHeader header_;
};
std::ostream& operator<<(std::ostream&, const Frame_SHARD&);

rsocket::StreamType getStreamType(FrameType frameType);
} // namespace proteus

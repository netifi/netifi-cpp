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

#include <folly/Optional.h>

#include <memory>

#include "proteus/framing/Frame.h"

namespace proteus {

// interface separating serialization/deserialization of ReactiveSocket frames
class FrameSerializer {
 public:
  virtual ~FrameSerializer() = default;

  virtual ProtocolVersion protocolVersion() const = 0;

  static std::unique_ptr<FrameSerializer> createFrameSerializer(
      const ProtocolVersion& protocolVersion);

  static std::unique_ptr<FrameSerializer> createAutodetectedSerializer(
      const folly::IOBuf& firstFrame);

  virtual FrameType peekFrameType(const folly::IOBuf& in) const = 0;
  virtual folly::Optional<rsocket::StreamId> peekStreamId(
      const folly::IOBuf& in) const = 0;

  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_BROKER_SETUP&&) const = 0;
  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_DESTINATION_SETUP&&) const = 0;
  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_DESTINATION&&) const = 0;
  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_GROUP&&) const = 0;
  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_BROADCAST&&) const = 0;
  virtual std::unique_ptr<folly::IOBuf> serializeOut(Frame_SHARD&&) const = 0;

  virtual bool deserializeFrom(Frame_BROKER_SETUP&, std::unique_ptr<folly::IOBuf>) const = 0;
  virtual bool deserializeFrom(Frame_DESTINATION_SETUP&, std::unique_ptr<folly::IOBuf>) const = 0;
  virtual bool deserializeFrom(Frame_DESTINATION&, std::unique_ptr<folly::IOBuf>) const = 0;
  virtual bool deserializeFrom(Frame_GROUP&, std::unique_ptr<folly::IOBuf>) const = 0;
  virtual bool deserializeFrom(Frame_BROADCAST&, std::unique_ptr<folly::IOBuf>) const = 0;
  virtual bool deserializeFrom(Frame_SHARD&, std::unique_ptr<folly::IOBuf>) const = 0;

  virtual size_t frameLengthFieldSize() const = 0;
  bool& preallocateFrameSizeField();

 protected:
  folly::IOBufQueue createBufferQueue(size_t bufferSize) const;

 private:
  bool preallocateFrameSizeField_{false};
};

} // namespace proteus

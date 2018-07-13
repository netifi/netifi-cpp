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

#include "proteus/framing/FrameSerializer.h"

namespace proteus {

class FrameSerializerV1_0 : public FrameSerializer {
 public:
  constexpr static const ProtocolVersion Version = ProtocolVersion(1, 0);
  constexpr static const size_t kFrameHeaderSize = 6; // bytes
  constexpr static const size_t kMinBytesNeededForAutodetection = 10; // bytes

  ProtocolVersion protocolVersion() const override;

  static ProtocolVersion detectProtocolVersion(
      const folly::IOBuf& firstFrame,
      size_t skipBytes = 0);

  FrameType peekFrameType(const folly::IOBuf& in) const override;
  folly::Optional<rsocket::StreamId> peekStreamId(const folly::IOBuf& in) const override;

  std::unique_ptr<folly::IOBuf> serializeOut(Frame_BROKER_SETUP&&) const override;
  std::unique_ptr<folly::IOBuf> serializeOut(Frame_DESTINATION_SETUP&&) const override;
  std::unique_ptr<folly::IOBuf> serializeOut(Frame_DESTINATION&&) const override;
  std::unique_ptr<folly::IOBuf> serializeOut(Frame_GROUP&&) const override;
  std::unique_ptr<folly::IOBuf> serializeOut(Frame_BROADCAST&&) const override;
  std::unique_ptr<folly::IOBuf> serializeOut(Frame_SHARD&&) const override;

  bool deserializeFrom(Frame_BROKER_SETUP&, std::unique_ptr<folly::IOBuf>)
      const override;
  bool deserializeFrom(Frame_DESTINATION_SETUP&, std::unique_ptr<folly::IOBuf>)
      const override;
  bool deserializeFrom(Frame_DESTINATION&, std::unique_ptr<folly::IOBuf>)
      const override;
  bool deserializeFrom(Frame_GROUP&, std::unique_ptr<folly::IOBuf>)
      const override;
  bool deserializeFrom(Frame_BROADCAST&, std::unique_ptr<folly::IOBuf>)
      const override;
  bool deserializeFrom(Frame_SHARD&, std::unique_ptr<folly::IOBuf>)
      const override;

  static std::unique_ptr<folly::IOBuf> deserializeMetadataFrom(
      folly::io::Cursor& cur,
      FrameFlags flags);

 private:
  size_t frameLengthFieldSize() const override;
};
} // namespace proteus

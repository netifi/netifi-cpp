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

constexpr uint32_t Frame_LEASE::kMaxTtl;
constexpr uint32_t Frame_LEASE::kMaxNumRequests;
constexpr uint32_t Frame_SETUP::kMaxKeepaliveTime;
constexpr uint32_t Frame_SETUP::kMaxLifetime;

std::ostream& operator<<(std::ostream& os, const Frame_REQUEST_Base& frame) {
  return os << frame.header_ << "(" << frame.requestN_ << ", "
            << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_REQUEST_N& frame) {
  return os << frame.header_ << "(" << frame.requestN_ << ")";
}

std::ostream& operator<<(
    std::ostream& os,
    const Frame_REQUEST_RESPONSE& frame) {
  return os << frame.header_ << ", " << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_REQUEST_FNF& frame) {
  return os << frame.header_ << ", " << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_METADATA_PUSH& frame) {
  return os << frame.header_ << ", "
            << (frame.metadata_ ? frame.metadata_->computeChainDataLength()
                                : 0);
}

std::ostream& operator<<(std::ostream& os, const Frame_CANCEL& frame) {
  return os << frame.header_;
}

Frame_PAYLOAD Frame_PAYLOAD::complete(rsocket::StreamId streamId) {
  return Frame_PAYLOAD(streamId, FrameFlags::COMPLETE, rsocket::Payload());
}

std::ostream& operator<<(std::ostream& os, const Frame_PAYLOAD& frame) {
  return os << frame.header_ << ", " << frame.payload_;
}

Frame_ERROR Frame_ERROR::invalidSetup(folly::StringPiece message) {
  return connectionErr(ErrorCode::INVALID_SETUP, message);
}

Frame_ERROR Frame_ERROR::unsupportedSetup(folly::StringPiece message) {
  return connectionErr(ErrorCode::UNSUPPORTED_SETUP, message);
}

Frame_ERROR Frame_ERROR::rejectedSetup(folly::StringPiece message) {
  return connectionErr(ErrorCode::REJECTED_SETUP, message);
}

Frame_ERROR Frame_ERROR::rejectedResume(folly::StringPiece message) {
  return connectionErr(ErrorCode::REJECTED_RESUME, message);
}

Frame_ERROR Frame_ERROR::connectionError(folly::StringPiece message) {
  return connectionErr(ErrorCode::CONNECTION_ERROR, message);
}

Frame_ERROR Frame_ERROR::applicationError(
    rsocket::StreamId stream,
    folly::StringPiece message) {
  return streamErr(ErrorCode::APPLICATION_ERROR, message, stream);
}

Frame_ERROR Frame_ERROR::applicationError(rsocket::StreamId stream, rsocket::Payload&& payload) {
  if (stream == 0) {
    throw std::invalid_argument{"Can't make stream error for stream zero"};
  }
  return Frame_ERROR(stream, ErrorCode::APPLICATION_ERROR, std::move(payload));
}

Frame_ERROR Frame_ERROR::rejected(rsocket::StreamId stream, folly::StringPiece message) {
  return streamErr(ErrorCode::REJECTED, message, stream);
}

Frame_ERROR Frame_ERROR::canceled(rsocket::StreamId stream, folly::StringPiece message) {
  return streamErr(ErrorCode::CANCELED, message, stream);
}

Frame_ERROR Frame_ERROR::invalid(rsocket::StreamId stream, folly::StringPiece message) {
  return streamErr(ErrorCode::INVALID, message, stream);
}

Frame_ERROR Frame_ERROR::connectionErr(
    ErrorCode err,
    folly::StringPiece message) {
  return Frame_ERROR{0, err, rsocket::Payload{message}};
}

Frame_ERROR Frame_ERROR::streamErr(
    ErrorCode err,
    folly::StringPiece message,
    rsocket::StreamId stream) {
  if (stream == 0) {
    throw std::invalid_argument{"Can't make stream error for stream zero"};
  }
  return Frame_ERROR{stream, err, rsocket::Payload{message}};
}

std::ostream& operator<<(std::ostream& os, const Frame_ERROR& frame) {
  return os << frame.header_ << ", " << frame.errorCode_ << ", "
            << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_KEEPALIVE& frame) {
  return os << frame.header_ << "(<"
            << (frame.data_ ? frame.data_->computeChainDataLength() : 0)
            << ">)";
}

std::ostream& operator<<(std::ostream& os, const Frame_SETUP& frame) {
  return os << frame.header_ << ", Version: " << frame.versionMajor_ << "."
            << frame.versionMinor_ << ", "
            << "Token: " << frame.token_ << ", " << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_LEASE& frame) {
  return os << frame.header_ << ", ("
            << (frame.metadata_ ? frame.metadata_->computeChainDataLength() : 0)
            << ")";
}

std::ostream& operator<<(std::ostream& os, const Frame_RESUME& frame) {
  return os << frame.header_ << ", ("
            << "token " << frame.token_ << ", @server "
            << frame.lastReceivedServerPosition_ << ", @client "
            << frame.clientPosition_ << ")";
}

std::ostream& operator<<(std::ostream& os, const Frame_RESUME_OK& frame) {
  return os << frame.header_ << ", (@" << frame.position_ << ")";
}

std::ostream& operator<<(std::ostream& os, const Frame_REQUEST_CHANNEL& frame) {
  return os << frame.header_ << ", initialRequestN=" << frame.requestN_ << ", "
            << frame.payload_;
}

std::ostream& operator<<(std::ostream& os, const Frame_REQUEST_STREAM& frame) {
  return os << frame.header_ << ", initialRequestN=" << frame.requestN_ << ", "
            << frame.payload_;
}

rsocket::StreamType getStreamType(FrameType frameType) {
  if (frameType == FrameType::REQUEST_STREAM) {
    return rsocket::StreamType::STREAM;
  } else if (frameType == FrameType::REQUEST_CHANNEL) {
    return rsocket::StreamType::CHANNEL;
  } else if (frameType == FrameType::REQUEST_RESPONSE) {
    return rsocket::StreamType::REQUEST_RESPONSE;
  } else if (frameType == FrameType::REQUEST_FNF) {
    return rsocket::StreamType::FNF;
  } else {
    LOG(FATAL) << "Unknown open stream frame : " << frameType;
  }
}
} // namespace proteus

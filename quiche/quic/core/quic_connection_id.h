// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_QUIC_CORE_QUIC_CONNECTION_ID_H_
#define QUICHE_QUIC_CORE_QUIC_CONNECTION_ID_H_

#include <cstdint>
#include <string>
#include <vector>

#include "absl/types/span.h"
#include "quiche/quic/platform/api/quic_export.h"

namespace quic {

// This is a property of QUIC headers, it indicates whether the connection ID
// should actually be sent over the wire (or was sent on received packets).
enum QuicConnectionIdIncluded : uint8_t {
  CONNECTION_ID_PRESENT = 1,
  CONNECTION_ID_ABSENT = 2,
};

// Maximum connection ID length supported by versions that use the encoding from
// draft-ietf-quic-invariants-06.
const uint8_t kQuicMaxConnectionIdWithLengthPrefixLength = 20;

// Maximum connection ID length supported by versions that use the encoding from
// draft-ietf-quic-invariants-05.
const uint8_t kQuicMaxConnectionId4BitLength = 18;

// kQuicDefaultConnectionIdLength is the only supported length for QUIC
// versions < v99, and is the default picked for all versions.
const uint8_t kQuicDefaultConnectionIdLength = 8;

// According to the IETF spec, the initial server connection ID generated by
// the client must be at least this long.
const uint8_t kQuicMinimumInitialConnectionIdLength = 8;

class QUIC_EXPORT_PRIVATE QuicConnectionId {
 public:
  // Creates a connection ID of length zero.
  QuicConnectionId() = default;

  // Creates a connection ID from network order bytes.
  QuicConnectionId(const char* data, uint8_t length);

  // Creates a connection ID from another connection ID.
  QuicConnectionId(const QuicConnectionId& other);

  // Assignment operator.
  QuicConnectionId& operator=(const QuicConnectionId& other) = default;

  ~QuicConnectionId() = default;

  // Returns the length of the connection ID, in bytes.
  uint8_t length() const;

  // Sets the length of the connection ID, in bytes.
  // WARNING: Calling set_length() can change the in-memory location of the
  // connection ID. Callers must therefore ensure they call data() or
  // mutable_data() after they call set_length().
  void set_length(uint8_t length);

  // Returns a pointer to the connection ID bytes, in network byte order.
  const char* data() const;

  // Returns a mutable pointer to the connection ID bytes,
  // in network byte order.
  char* mutable_data();

  // Returns whether the connection ID has length zero.
  bool IsEmpty() const;

  // Hash() is required to use connection IDs as keys in hash tables.
  // During the lifetime of a process, the output of Hash() is guaranteed to be
  // the same for connection IDs that are equal to one another. Note however
  // that this property is not guaranteed across process lifetimes. This makes
  // Hash() suitable for data structures such as hash tables but not for sending
  // a hash over the network.
  size_t Hash() const;

  // Generates an ASCII string that represents
  // the contents of the connection ID, or "0" if it is empty.
  std::string ToString() const;

  // operator<< allows easily logging connection IDs.
  friend QUIC_EXPORT_PRIVATE std::ostream& operator<<(
      std::ostream& os, const QuicConnectionId& v);

  bool operator==(const QuicConnectionId& v) const;
  bool operator!=(const QuicConnectionId& v) const;
  // operator< is required to use connection IDs as keys in hash tables.
  bool operator<(const QuicConnectionId& v) const;

 private:
  // The connection ID is represented in network byte order.
    // If the connection ID fits in |data_short_|, it is stored in the
    // first |length_| bytes of |data_short_|.
    // Otherwise it is stored in |data_long_| which is guaranteed to have a size
    // equal to |length_|.
    // A value of 11 was chosen because our commonly used connection ID length
    // is 8 and with the length, the class is padded to at least 12 bytes
    // anyway.

    int64_t data_short_ = 0;
    uint8_t length_ = 0;  // length of the connection ID, in bytes.
};

// Creates a connection ID of length zero, unless the restart flag
// quic_connection_ids_network_byte_order is false in which case
// it returns an 8-byte all-zeroes connection ID.
QUIC_EXPORT_PRIVATE QuicConnectionId EmptyQuicConnectionId();

// QuicConnectionIdHash can be passed as hash argument to hash tables.
// During the lifetime of a process, the output of QuicConnectionIdHash is
// guaranteed to be the same for connection IDs that are equal to one another.
// Note however that this property is not guaranteed across process lifetimes.
// This makes QuicConnectionIdHash suitable for data structures such as hash
// tables but not for sending a hash over the network.
class QUIC_EXPORT_PRIVATE QuicConnectionIdHash {
 public:
  size_t operator()(QuicConnectionId const& connection_id) const noexcept {
    return connection_id.Hash();
  }
};

}  // namespace quic

#endif  // QUICHE_QUIC_CORE_QUIC_CONNECTION_ID_H_

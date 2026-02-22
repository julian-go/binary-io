// Copyright (c) 2024 Julian Gottwald
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

/// @file binary-io.hpp
/// @brief Portable binary serialization and deserialization library.
///
/// Provides byte-order-aware readers and writers for reading and writing
/// primitive types from/to raw byte buffers. Supports both little-endian
/// and big-endian byte orders via codec policies.

#ifndef BINARYIO_BINARYIO_HPP_
#define BINARYIO_BINARYIO_HPP_

#include <cstddef>
#include <cstdint>
#include <cstring>

/// @brief Binary I/O library namespace.
namespace bio {

/// @brief Result type indicating success or failure of a read/write operation.
///
/// Convertible to @c bool for convenient error checking. A @c Status that
/// evaluates to @c true indicates success; @c false indicates an out-of-range
/// error (not enough bytes remaining in the buffer).
struct [[nodiscard]] Status {
  bool ok;  ///< @c true when the operation succeeded.

  /// @brief Contextual conversion to @c bool.
  /// @return @c true if the operation succeeded.
  constexpr explicit operator bool() const { return ok; }

  /// @brief Construct a success status.
  /// @return A @c Status representing success.
  static constexpr Status Ok() { return {true}; }

  /// @brief Construct a failure status (out of range).
  /// @return A @c Status representing an out-of-range error.
  static constexpr Status OutOfRange() { return {false}; }
};

/// @brief Codec that loads and stores integers in little-endian byte order.
struct LittleEndianCodec {
  /// @brief Load a 16-bit unsigned integer from memory in little-endian order.
  /// @param p Pointer to at least 2 bytes of data.
  /// @return The decoded value.
  static inline uint16_t LoadU16(const uint8_t* p) {
    return static_cast<uint16_t>(static_cast<uint16_t>(p[0]) |
                                 (static_cast<uint16_t>(p[1]) << 8));
  }
  /// @brief Load a 32-bit unsigned integer from memory in little-endian order.
  /// @param p Pointer to at least 4 bytes of data.
  /// @return The decoded value.
  static inline uint32_t LoadU32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 0) |
           (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
  }

  /// @brief Load a 64-bit unsigned integer from memory in little-endian order.
  /// @param p Pointer to at least 8 bytes of data.
  /// @return The decoded value.
  static inline uint64_t LoadU64(const uint8_t* p) {
    return (static_cast<uint64_t>(p[0]) << 0) |
           (static_cast<uint64_t>(p[1]) << 8) |
           (static_cast<uint64_t>(p[2]) << 16) |
           (static_cast<uint64_t>(p[3]) << 24) |
           (static_cast<uint64_t>(p[4]) << 32) |
           (static_cast<uint64_t>(p[5]) << 40) |
           (static_cast<uint64_t>(p[6]) << 48) |
           (static_cast<uint64_t>(p[7]) << 56);
  }

  /// @brief Store a 16-bit unsigned integer to memory in little-endian order.
  /// @param p Pointer to at least 2 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU16(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>((v >> 0) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
  }

  /// @brief Store a 32-bit unsigned integer to memory in little-endian order.
  /// @param p Pointer to at least 4 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>((v >> 0) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 24) & 0xFFu);
  }

  /// @brief Store a 64-bit unsigned integer to memory in little-endian order.
  /// @param p Pointer to at least 8 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU64(uint8_t* p, uint64_t v) {
    p[0] = static_cast<uint8_t>((v >> 0) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 24) & 0xFFu);
    p[4] = static_cast<uint8_t>((v >> 32) & 0xFFu);
    p[5] = static_cast<uint8_t>((v >> 40) & 0xFFu);
    p[6] = static_cast<uint8_t>((v >> 48) & 0xFFu);
    p[7] = static_cast<uint8_t>((v >> 56) & 0xFFu);
  }
};

/// @brief Codec that loads and stores integers in big-endian byte order.
struct BigEndianCodec {
  /// @brief Load a 16-bit unsigned integer from memory in big-endian order.
  /// @param p Pointer to at least 2 bytes of data.
  /// @return The decoded value.
  static inline uint16_t LoadU16(const uint8_t* p) {
    return static_cast<uint16_t>((static_cast<uint16_t>(p[0]) << 8) |
                                 static_cast<uint16_t>(p[1]));
  }
  /// @brief Load a 32-bit unsigned integer from memory in big-endian order.
  /// @param p Pointer to at least 4 bytes of data.
  /// @return The decoded value.
  static inline uint32_t LoadU32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
           (static_cast<uint32_t>(p[3]) << 0);
  }

  /// @brief Load a 64-bit unsigned integer from memory in big-endian order.
  /// @param p Pointer to at least 8 bytes of data.
  /// @return The decoded value.
  static inline uint64_t LoadU64(const uint8_t* p) {
    return (static_cast<uint64_t>(p[0]) << 56) |
           (static_cast<uint64_t>(p[1]) << 48) |
           (static_cast<uint64_t>(p[2]) << 40) |
           (static_cast<uint64_t>(p[3]) << 32) |
           (static_cast<uint64_t>(p[4]) << 24) |
           (static_cast<uint64_t>(p[5]) << 16) |
           (static_cast<uint64_t>(p[6]) << 8) |
           (static_cast<uint64_t>(p[7]) << 0);
  }

  /// @brief Store a 16-bit unsigned integer to memory in big-endian order.
  /// @param p Pointer to at least 2 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU16(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 0) & 0xFFu);
  }

  /// @brief Store a 32-bit unsigned integer to memory in big-endian order.
  /// @param p Pointer to at least 4 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>((v >> 24) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 0) & 0xFFu);
  }

  /// @brief Store a 64-bit unsigned integer to memory in big-endian order.
  /// @param p Pointer to at least 8 bytes of writable memory.
  /// @param v The value to encode.
  static inline void StoreU64(uint8_t* p, uint64_t v) {
    p[0] = static_cast<uint8_t>((v >> 56) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 48) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 40) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 32) & 0xFFu);
    p[4] = static_cast<uint8_t>((v >> 24) & 0xFFu);
    p[5] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[6] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[7] = static_cast<uint8_t>((v >> 0) & 0xFFu);
  }
};

/// @brief Byte reader that deserializes primitives from a fixed-size buffer.
///
/// Reads are performed sequentially; the internal cursor advances after each
/// successful read. Every read method returns a @ref Status indicating whether
/// enough bytes were available.
///
/// @tparam Codec Byte-order codec (e.g. @ref LittleEndianCodec,
///               @ref BigEndianCodec).
template <typename Codec>
class ByteReaderT {
 public:
  /// @brief Construct a reader over an existing buffer.
  /// @param data Pointer to the beginning of the buffer.
  /// @param size Size of the buffer in bytes.
  ByteReaderT(const void* data, size_t size)
      : p_(static_cast<const uint8_t*>(data)), n_(size), size_(size) {}

  /// @brief Return the number of bytes remaining to be read.
  /// @return Remaining byte count.
  size_t remaining() const { return n_; }

  /// @brief Return the current read position (bytes consumed so far).
  /// @return Current byte offset from the start of the buffer.
  size_t position() const { return size_ - n_; }

  /// @brief Read an unsigned 8-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 1 byte remains.
  Status read_u8(uint8_t& out) {
    if (1 > n_) return Status::OutOfRange();
    out = *p_++;
    --n_;
    return Status::Ok();
  }

  /// @brief Read an unsigned 16-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 2 bytes remain.
  Status read_u16(uint16_t& out) {
    if (sizeof(uint16_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU16(p_);
    p_ += sizeof(uint16_t);
    n_ -= sizeof(uint16_t);
    return Status::Ok();
  }

  /// @brief Read an unsigned 32-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes remain.
  Status read_u32(uint32_t& out) {
    if (sizeof(uint32_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU32(p_);
    p_ += sizeof(uint32_t);
    n_ -= sizeof(uint32_t);
    return Status::Ok();
  }

  /// @brief Read an unsigned 64-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes remain.
  Status read_u64(uint64_t& out) {
    if (sizeof(uint64_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU64(p_);
    p_ += sizeof(uint64_t);
    n_ -= sizeof(uint64_t);
    return Status::Ok();
  }

  /// @brief Read a signed 8-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 1 byte remains.
  Status read_i8(int8_t& out) {
    uint8_t bits = 0;
    if (!read_u8(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a signed 16-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 2 bytes remain.
  Status read_i16(int16_t& out) {
    uint16_t bits = 0;
    if (!read_u16(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a signed 32-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes remain.
  Status read_i32(int32_t& out) {
    uint32_t bits = 0;
    if (!read_u32(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a signed 64-bit integer.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes remain.
  Status read_i64(int64_t& out) {
    uint64_t bits = 0;
    if (!read_u64(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a 32-bit IEEE 754 floating-point value.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes remain.
  Status read_f32(float& out) {
    static_assert(sizeof(float) == 4, "float must be 32-bit");
    uint32_t bits = 0;
    if (!read_u32(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a 64-bit IEEE 754 floating-point value.
  /// @param[out] out Receives the read value on success.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes remain.
  Status read_f64(double& out) {
    static_assert(sizeof(double) == 8, "double must be 64-bit");
    uint64_t bits = 0;
    if (!read_u64(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  /// @brief Read a sequence of raw bytes into a caller-provided buffer.
  /// @param[out] out Destination buffer; must be at least @p len bytes.
  /// @param len Number of bytes to read.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than @p len bytes remain.
  Status read_bytes(void* out, size_t len) {
    if (len > n_) return Status::OutOfRange();
    std::memcpy(out, p_, len);
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

  /// @brief Advance the read cursor without reading any data.
  /// @param len Number of bytes to skip.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than @p len bytes remain.
  Status skip(size_t len) {
    if (len > n_) return Status::OutOfRange();
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

 private:
  const uint8_t* p_;  ///< Current read position.
  size_t n_;          ///< Remaining bytes.
  size_t size_;       ///< Total buffer size.
};

/// @brief Byte writer that serializes primitives into a fixed-size buffer.
///
/// Writes are performed sequentially; the internal cursor advances after each
/// successful write. Every write method returns a @ref Status indicating
/// whether enough space was available.
///
/// @tparam Codec Byte-order codec (e.g. @ref LittleEndianCodec,
///               @ref BigEndianCodec).
template <typename Codec>
class ByteWriterT {
 public:
  /// @brief Construct a writer over an existing buffer.
  /// @param data Pointer to the beginning of the buffer.
  /// @param size Size of the buffer in bytes.
  ByteWriterT(void* data, size_t size)
      : p_(static_cast<uint8_t*>(data)), n_(size), size_(size) {}

  /// @brief Return the number of bytes of remaining capacity.
  /// @return Remaining byte count.
  size_t remaining() const { return n_; }

  /// @brief Return the current write position (bytes written so far).
  /// @return Current byte offset from the start of the buffer.
  size_t position() const { return size_ - n_; }

  /// @brief Write an unsigned 8-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 1 byte of capacity remains.
  Status write_u8(uint8_t v) {
    if (1 > n_) return Status::OutOfRange();
    *p_++ = v;
    --n_;
    return Status::Ok();
  }

  /// @brief Write an unsigned 16-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 2 bytes of capacity remain.
  Status write_u16(uint16_t v) {
    if (sizeof(uint16_t) > n_) return Status::OutOfRange();
    Codec::StoreU16(p_, v);
    p_ += sizeof(uint16_t);
    n_ -= sizeof(uint16_t);
    return Status::Ok();
  }

  /// @brief Write an unsigned 32-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes of capacity remain.
  Status write_u32(uint32_t v) {
    if (sizeof(uint32_t) > n_) return Status::OutOfRange();
    Codec::StoreU32(p_, v);
    p_ += sizeof(uint32_t);
    n_ -= sizeof(uint32_t);
    return Status::Ok();
  }

  /// @brief Write an unsigned 64-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes of capacity remain.
  Status write_u64(uint64_t v) {
    if (sizeof(uint64_t) > n_) return Status::OutOfRange();
    Codec::StoreU64(p_, v);
    p_ += sizeof(uint64_t);
    n_ -= sizeof(uint64_t);
    return Status::Ok();
  }

  /// @brief Write a signed 8-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 1 byte of capacity remains.
  Status write_i8(int8_t v) {
    uint8_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u8(bits);
  }

  /// @brief Write a signed 16-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 2 bytes of capacity remain.
  Status write_i16(int16_t v) {
    uint16_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u16(bits);
  }

  /// @brief Write a signed 32-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes of capacity remain.
  Status write_i32(int32_t v) {
    uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u32(bits);
  }

  /// @brief Write a signed 64-bit integer.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes of capacity remain.
  Status write_i64(int64_t v) {
    uint64_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u64(bits);
  }

  /// @brief Write a 32-bit IEEE 754 floating-point value.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 4 bytes of capacity remain.
  Status write_f32(float v) {
    static_assert(sizeof(float) == 4, "float must be 32-bit");
    uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u32(bits);
  }

  /// @brief Write a 64-bit IEEE 754 floating-point value.
  /// @param v The value to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than 8 bytes of capacity remain.
  Status write_f64(double v) {
    static_assert(sizeof(double) == 8, "double must be 64-bit");
    uint64_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u64(bits);
  }

  /// @brief Write a sequence of raw bytes from a caller-provided buffer.
  /// @param in Source buffer; must contain at least @p len bytes.
  /// @param len Number of bytes to write.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than @p len bytes of capacity remain.
  Status write_bytes(const void* in, size_t len) {
    if (len > n_) return Status::OutOfRange();
    std::memcpy(p_, in, len);
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

  /// @brief Advance the write cursor without writing any data.
  /// @param len Number of bytes to skip.
  /// @return @ref Status::Ok() on success, @ref Status::OutOfRange() if fewer
  ///         than @p len bytes of capacity remain.
  Status skip(size_t len) {
    if (len > n_) return Status::OutOfRange();
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

 private:
  uint8_t* p_;   ///< Current write position.
  size_t n_;     ///< Remaining capacity.
  size_t size_;  ///< Total buffer size.
};

using LEReader = ByteReaderT<LittleEndianCodec>;
using BEReader = ByteReaderT<BigEndianCodec>;
using LEWriter = ByteWriterT<LittleEndianCodec>;
using BEWriter = ByteWriterT<BigEndianCodec>;

}  // namespace bio

#endif  // !BINARYIO_BINARYIO_HPP_

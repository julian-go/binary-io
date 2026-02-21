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

#ifndef BINARYIO_BINARYIO_HPP_
#define BINARYIO_BINARYIO_HPP_

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace bio {

struct [[nodiscard]] Status {
  bool ok;
  constexpr explicit operator bool() const { return ok; }
  static constexpr Status Ok() { return {true}; }
  static constexpr Status OutOfRange() { return {false}; }
};

struct LittleEndianCodec {
  static inline uint16_t LoadU16(const uint8_t* p) {
    return static_cast<uint16_t>(static_cast<uint16_t>(p[0]) |
                                 (static_cast<uint16_t>(p[1]) << 8));
  }
  static inline uint32_t LoadU32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 0) |
           (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
  }
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

  static inline void StoreU16(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>((v >> 0) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
  }
  static inline void StoreU32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>((v >> 0) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 24) & 0xFFu);
  }
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

struct BigEndianCodec {
  static inline uint16_t LoadU16(const uint8_t* p) {
    return static_cast<uint16_t>((static_cast<uint16_t>(p[0]) << 8) |
                                 static_cast<uint16_t>(p[1]));
  }
  static inline uint32_t LoadU32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
           (static_cast<uint32_t>(p[3]) << 0);
  }
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

  static inline void StoreU16(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 0) & 0xFFu);
  }
  static inline void StoreU32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>((v >> 24) & 0xFFu);
    p[1] = static_cast<uint8_t>((v >> 16) & 0xFFu);
    p[2] = static_cast<uint8_t>((v >> 8) & 0xFFu);
    p[3] = static_cast<uint8_t>((v >> 0) & 0xFFu);
  }
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

template <typename Codec>
class ByteReaderT {
 public:
  ByteReaderT(const void* data, size_t size)
      : p_(static_cast<const uint8_t*>(data)), n_(size), size_(size) {}

  size_t remaining() const { return n_; }

  size_t position() const { return size_ - n_; }

  Status read_u8(uint8_t& out) {
    if (1 > n_) return Status::OutOfRange();
    out = *p_++;
    --n_;
    return Status::Ok();
  }

  Status read_u16(uint16_t& out) {
    if (sizeof(uint16_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU16(p_);
    p_ += sizeof(uint16_t);
    n_ -= sizeof(uint16_t);
    return Status::Ok();
  }

  Status read_u32(uint32_t& out) {
    if (sizeof(uint32_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU32(p_);
    p_ += sizeof(uint32_t);
    n_ -= sizeof(uint32_t);
    return Status::Ok();
  }

  Status read_u64(uint64_t& out) {
    if (sizeof(uint64_t) > n_) return Status::OutOfRange();
    out = Codec::LoadU64(p_);
    p_ += sizeof(uint64_t);
    n_ -= sizeof(uint64_t);
    return Status::Ok();
  }

  Status read_i8(int8_t& out) {
    uint8_t bits = 0;
    if (!read_u8(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_i16(int16_t& out) {
    uint16_t bits = 0;
    if (!read_u16(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_i32(int32_t& out) {
    uint32_t bits = 0;
    if (!read_u32(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_i64(int64_t& out) {
    uint64_t bits = 0;
    if (!read_u64(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_f32(float& out) {
    static_assert(sizeof(float) == 4, "float must be 32-bit");
    uint32_t bits = 0;
    if (!read_u32(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_f64(double& out) {
    static_assert(sizeof(double) == 8, "double must be 64-bit");
    uint64_t bits = 0;
    if (!read_u64(bits)) return Status::OutOfRange();
    std::memcpy(&out, &bits, sizeof(bits));
    return Status::Ok();
  }

  Status read_bytes(void* out, size_t len) {
    if (len > n_) return Status::OutOfRange();
    std::memcpy(out, p_, len);
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

  Status skip(size_t len) {
    if (len > n_) return Status::OutOfRange();
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

 private:
  const uint8_t* p_;
  size_t n_;
  size_t size_;
};

template <typename Codec>
class ByteWriterT {
 public:
  ByteWriterT(void* data, size_t size)
      : p_(static_cast<uint8_t*>(data)), n_(size), size_(size) {}

  size_t remaining() const { return n_; }

  size_t position() const { return size_ - n_; }

  Status write_u8(uint8_t v) {
    if (1 > n_) return Status::OutOfRange();
    *p_++ = v;
    --n_;
    return Status::Ok();
  }

  Status write_u16(uint16_t v) {
    if (sizeof(uint16_t) > n_) return Status::OutOfRange();
    Codec::StoreU16(p_, v);
    p_ += sizeof(uint16_t);
    n_ -= sizeof(uint16_t);
    return Status::Ok();
  }

  Status write_u32(uint32_t v) {
    if (sizeof(uint32_t) > n_) return Status::OutOfRange();
    Codec::StoreU32(p_, v);
    p_ += sizeof(uint32_t);
    n_ -= sizeof(uint32_t);
    return Status::Ok();
  }

  Status write_u64(uint64_t v) {
    if (sizeof(uint64_t) > n_) return Status::OutOfRange();
    Codec::StoreU64(p_, v);
    p_ += sizeof(uint64_t);
    n_ -= sizeof(uint64_t);
    return Status::Ok();
  }

  Status write_i8(int8_t v) {
    uint8_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u8(bits);
  }

  Status write_i16(int16_t v) {
    uint16_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u16(bits);
  }

  Status write_i32(int32_t v) {
    uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u32(bits);
  }

  Status write_i64(int64_t v) {
    uint64_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u64(bits);
  }

  Status write_f32(float v) {
    static_assert(sizeof(float) == 4, "float must be 32-bit");
    uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u32(bits);
  }

  Status write_f64(double v) {
    static_assert(sizeof(double) == 8, "double must be 64-bit");
    uint64_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    return write_u64(bits);
  }

  Status write_bytes(const void* in, size_t len) {
    if (len > n_) return Status::OutOfRange();
    std::memcpy(p_, in, len);
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

  Status skip(size_t len) {
    if (len > n_) return Status::OutOfRange();
    p_ += len;
    n_ -= len;
    return Status::Ok();
  }

 private:
  uint8_t* p_;
  size_t n_;
  size_t size_;
};

} // namespace bio

#endif // !BINARYIO_BINARYIO_HPP_

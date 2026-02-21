#include "binary-io/binary-io.hpp"
#include "doctest.h"

#include <cmath>
#include <cstring>
#include <limits>

using namespace bio;

// ============================================================================
// Status
// ============================================================================

TEST_CASE("Status::Ok is truthy") {
    Status s = Status::Ok();
    CHECK(s.ok == true);
    CHECK(static_cast<bool>(s) == true);
    CHECK(s);  // implicit conversion via explicit operator bool in if-context
}

TEST_CASE("Status::OutOfRange is falsy") {
    Status s = Status::OutOfRange();
    CHECK(s.ok == false);
    CHECK(static_cast<bool>(s) == false);
    CHECK_FALSE(s);
}

// ============================================================================
// LittleEndianCodec – Load
// ============================================================================

TEST_CASE("LE LoadU16 typical") {
    const uint8_t d[] = {0x34, 0x12};
    CHECK(LittleEndianCodec::LoadU16(d) == 0x1234);
}

TEST_CASE("LE LoadU16 zero") {
    const uint8_t d[] = {0x00, 0x00};
    CHECK(LittleEndianCodec::LoadU16(d) == 0);
}

TEST_CASE("LE LoadU16 max") {
    const uint8_t d[] = {0xFF, 0xFF};
    CHECK(LittleEndianCodec::LoadU16(d) == 0xFFFF);
}

TEST_CASE("LE LoadU32 typical") {
    const uint8_t d[] = {0x78, 0x56, 0x34, 0x12};
    CHECK(LittleEndianCodec::LoadU32(d) == 0x12345678u);
}

TEST_CASE("LE LoadU32 zero") {
    const uint8_t d[] = {0, 0, 0, 0};
    CHECK(LittleEndianCodec::LoadU32(d) == 0u);
}

TEST_CASE("LE LoadU32 max") {
    const uint8_t d[] = {0xFF, 0xFF, 0xFF, 0xFF};
    CHECK(LittleEndianCodec::LoadU32(d) == 0xFFFFFFFFu);
}

TEST_CASE("LE LoadU64 typical") {
    const uint8_t d[] = {0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};
    CHECK(LittleEndianCodec::LoadU64(d) == 0x0123456789ABCDEFull);
}

TEST_CASE("LE LoadU64 zero") {
    const uint8_t d[] = {0, 0, 0, 0, 0, 0, 0, 0};
    CHECK(LittleEndianCodec::LoadU64(d) == 0ull);
}

TEST_CASE("LE LoadU64 max") {
    const uint8_t d[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    CHECK(LittleEndianCodec::LoadU64(d) == 0xFFFFFFFFFFFFFFFFull);
}

// ============================================================================
// LittleEndianCodec – Store
// ============================================================================

TEST_CASE("LE StoreU16 typical") {
    uint8_t d[2] = {};
    LittleEndianCodec::StoreU16(d, 0x1234);
    CHECK(d[0] == 0x34);
    CHECK(d[1] == 0x12);
}

TEST_CASE("LE StoreU16 zero") {
    uint8_t d[2] = {0xFF, 0xFF};
    LittleEndianCodec::StoreU16(d, 0);
    CHECK(d[0] == 0);
    CHECK(d[1] == 0);
}

TEST_CASE("LE StoreU32 typical") {
    uint8_t d[4] = {};
    LittleEndianCodec::StoreU32(d, 0x12345678u);
    CHECK(d[0] == 0x78);
    CHECK(d[1] == 0x56);
    CHECK(d[2] == 0x34);
    CHECK(d[3] == 0x12);
}

TEST_CASE("LE StoreU64 typical") {
    uint8_t d[8] = {};
    LittleEndianCodec::StoreU64(d, 0x0123456789ABCDEFull);
    CHECK(d[0] == 0xEF);
    CHECK(d[1] == 0xCD);
    CHECK(d[2] == 0xAB);
    CHECK(d[3] == 0x89);
    CHECK(d[4] == 0x67);
    CHECK(d[5] == 0x45);
    CHECK(d[6] == 0x23);
    CHECK(d[7] == 0x01);
}

// ============================================================================
// LittleEndianCodec – Store/Load round-trip
// ============================================================================

TEST_CASE("LE StoreU16 then LoadU16 round-trip") {
    uint8_t d[2];
    LittleEndianCodec::StoreU16(d, 0xBEEF);
    CHECK(LittleEndianCodec::LoadU16(d) == 0xBEEF);
}

TEST_CASE("LE StoreU32 then LoadU32 round-trip") {
    uint8_t d[4];
    LittleEndianCodec::StoreU32(d, 0xDEADBEEFu);
    CHECK(LittleEndianCodec::LoadU32(d) == 0xDEADBEEFu);
}

TEST_CASE("LE StoreU64 then LoadU64 round-trip") {
    uint8_t d[8];
    LittleEndianCodec::StoreU64(d, 0xCAFEBABEDEADBEEFull);
    CHECK(LittleEndianCodec::LoadU64(d) == 0xCAFEBABEDEADBEEFull);
}

// ============================================================================
// BigEndianCodec – Load
// ============================================================================

TEST_CASE("BE LoadU16 typical") {
    const uint8_t d[] = {0x12, 0x34};
    CHECK(BigEndianCodec::LoadU16(d) == 0x1234);
}

TEST_CASE("BE LoadU16 zero") {
    const uint8_t d[] = {0x00, 0x00};
    CHECK(BigEndianCodec::LoadU16(d) == 0);
}

TEST_CASE("BE LoadU16 max") {
    const uint8_t d[] = {0xFF, 0xFF};
    CHECK(BigEndianCodec::LoadU16(d) == 0xFFFF);
}

TEST_CASE("BE LoadU32 typical") {
    const uint8_t d[] = {0x12, 0x34, 0x56, 0x78};
    CHECK(BigEndianCodec::LoadU32(d) == 0x12345678u);
}

TEST_CASE("BE LoadU32 zero") {
    const uint8_t d[] = {0, 0, 0, 0};
    CHECK(BigEndianCodec::LoadU32(d) == 0u);
}

TEST_CASE("BE LoadU32 max") {
    const uint8_t d[] = {0xFF, 0xFF, 0xFF, 0xFF};
    CHECK(BigEndianCodec::LoadU32(d) == 0xFFFFFFFFu);
}

TEST_CASE("BE LoadU64 typical") {
    const uint8_t d[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    CHECK(BigEndianCodec::LoadU64(d) == 0x0123456789ABCDEFull);
}

TEST_CASE("BE LoadU64 zero") {
    const uint8_t d[] = {0, 0, 0, 0, 0, 0, 0, 0};
    CHECK(BigEndianCodec::LoadU64(d) == 0ull);
}

TEST_CASE("BE LoadU64 max") {
    const uint8_t d[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    CHECK(BigEndianCodec::LoadU64(d) == 0xFFFFFFFFFFFFFFFFull);
}

// ============================================================================
// BigEndianCodec – Store
// ============================================================================

TEST_CASE("BE StoreU16 typical") {
    uint8_t d[2] = {};
    BigEndianCodec::StoreU16(d, 0x1234);
    CHECK(d[0] == 0x12);
    CHECK(d[1] == 0x34);
}

TEST_CASE("BE StoreU32 typical") {
    uint8_t d[4] = {};
    BigEndianCodec::StoreU32(d, 0x12345678u);
    CHECK(d[0] == 0x12);
    CHECK(d[1] == 0x34);
    CHECK(d[2] == 0x56);
    CHECK(d[3] == 0x78);
}

TEST_CASE("BE StoreU64 typical") {
    uint8_t d[8] = {};
    BigEndianCodec::StoreU64(d, 0x0123456789ABCDEFull);
    CHECK(d[0] == 0x01);
    CHECK(d[1] == 0x23);
    CHECK(d[2] == 0x45);
    CHECK(d[3] == 0x67);
    CHECK(d[4] == 0x89);
    CHECK(d[5] == 0xAB);
    CHECK(d[6] == 0xCD);
    CHECK(d[7] == 0xEF);
}

// ============================================================================
// BigEndianCodec – Store/Load round-trip
// ============================================================================

TEST_CASE("BE StoreU16 then LoadU16 round-trip") {
    uint8_t d[2];
    BigEndianCodec::StoreU16(d, 0xBEEF);
    CHECK(BigEndianCodec::LoadU16(d) == 0xBEEF);
}

TEST_CASE("BE StoreU32 then LoadU32 round-trip") {
    uint8_t d[4];
    BigEndianCodec::StoreU32(d, 0xDEADBEEFu);
    CHECK(BigEndianCodec::LoadU32(d) == 0xDEADBEEFu);
}

TEST_CASE("BE StoreU64 then LoadU64 round-trip") {
    uint8_t d[8];
    BigEndianCodec::StoreU64(d, 0xCAFEBABEDEADBEEFull);
    CHECK(BigEndianCodec::LoadU64(d) == 0xCAFEBABEDEADBEEFull);
}

// ============================================================================
// LE vs BE produce different byte orderings for the same value
// ============================================================================

TEST_CASE("LE and BE store different byte orderings") {
    uint8_t le[4] = {}, be[4] = {};
    LittleEndianCodec::StoreU32(le, 0x01020304u);
    BigEndianCodec::StoreU32(be, 0x01020304u);
    // LE: least-significant byte first
    CHECK(le[0] == 0x04);
    CHECK(le[3] == 0x01);
    // BE: most-significant byte first
    CHECK(be[0] == 0x01);
    CHECK(be[3] == 0x04);
}

// ============================================================================
// ByteReaderT – construction and remaining()
// ============================================================================

using LEReader = ByteReaderT<LittleEndianCodec>;
using BEReader = ByteReaderT<BigEndianCodec>;
using LEWriter = ByteWriterT<LittleEndianCodec>;
using BEWriter = ByteWriterT<BigEndianCodec>;

TEST_CASE("Reader remaining equals buffer size on construction") {
    uint8_t buf[16] = {};
    LEReader r(buf, sizeof(buf));
    CHECK(r.remaining() == 16);
}

TEST_CASE("Reader remaining is 0 for zero-length buffer") {
    LEReader r(nullptr, 0);
    CHECK(r.remaining() == 0);
}

// ============================================================================
// ByteReaderT – read_u8
// ============================================================================

TEST_CASE("LE read_u8 success") {
    uint8_t buf[] = {0xAB};
    LEReader r(buf, 1);
    uint8_t v = 0;
    CHECK(r.read_u8(v));
    CHECK(v == 0xAB);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_u8 on empty buffer fails") {
    LEReader r(nullptr, 0);
    uint8_t v = 0xFF;
    CHECK_FALSE(r.read_u8(v));
    CHECK(v == 0xFF);  // value unchanged
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_u8 after exhaustion fails") {
    uint8_t buf[] = {0x01};
    LEReader r(buf, 1);
    uint8_t v = 0;
    CHECK(r.read_u8(v));
    CHECK_FALSE(r.read_u8(v));
    CHECK(v == 0x01);  // still holds previous value
}

// ============================================================================
// ByteReaderT – read_u16 out-of-range
// ============================================================================

TEST_CASE("LE read_u16 success") {
    uint8_t buf[] = {0x34, 0x12};
    LEReader r(buf, 2);
    uint16_t v = 0;
    CHECK(r.read_u16(v));
    CHECK(v == 0x1234);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_u16 fails with only 1 byte") {
    uint8_t buf[] = {0x34};
    LEReader r(buf, 1);
    uint16_t v = 0xFFFF;
    CHECK_FALSE(r.read_u16(v));
    CHECK(v == 0xFFFF);  // unchanged
    CHECK(r.remaining() == 1);  // position unchanged
}

TEST_CASE("LE read_u16 fails on empty buffer") {
    LEReader r(nullptr, 0);
    uint16_t v = 0;
    CHECK_FALSE(r.read_u16(v));
}

// ============================================================================
// ByteReaderT – read_u32 out-of-range
// ============================================================================

TEST_CASE("LE read_u32 success") {
    uint8_t buf[] = {0x78, 0x56, 0x34, 0x12};
    LEReader r(buf, 4);
    uint32_t v = 0;
    CHECK(r.read_u32(v));
    CHECK(v == 0x12345678u);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_u32 fails with 3 bytes") {
    uint8_t buf[] = {0x78, 0x56, 0x34};
    LEReader r(buf, 3);
    uint32_t v = 0xDEADBEEF;
    CHECK_FALSE(r.read_u32(v));
    CHECK(v == 0xDEADBEEF);
    CHECK(r.remaining() == 3);
}

TEST_CASE("LE read_u32 fails on empty buffer") {
    LEReader r(nullptr, 0);
    uint32_t v = 0;
    CHECK_FALSE(r.read_u32(v));
}

// ============================================================================
// ByteReaderT – read_u64 out-of-range
// ============================================================================

TEST_CASE("LE read_u64 success") {
    uint8_t buf[] = {0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};
    LEReader r(buf, 8);
    uint64_t v = 0;
    CHECK(r.read_u64(v));
    CHECK(v == 0x0123456789ABCDEFull);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_u64 fails with 7 bytes") {
    uint8_t buf[7] = {};
    LEReader r(buf, 7);
    uint64_t v = 0xFFFFFFFFFFFFFFFFull;
    CHECK_FALSE(r.read_u64(v));
    CHECK(v == 0xFFFFFFFFFFFFFFFFull);
    CHECK(r.remaining() == 7);
}

TEST_CASE("LE read_u64 fails on empty buffer") {
    LEReader r(nullptr, 0);
    uint64_t v = 0;
    CHECK_FALSE(r.read_u64(v));
}

// ============================================================================
// ByteReaderT – read_f32
// ============================================================================

TEST_CASE("LE read_f32 success with 1.0f") {
    // IEEE-754: 1.0f = 0x3F800000
    uint8_t buf[4];
    LittleEndianCodec::StoreU32(buf, 0x3F800000u);
    LEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(v == 1.0f);
}

TEST_CASE("LE read_f32 fails with 3 bytes") {
    uint8_t buf[3] = {};
    LEReader r(buf, 3);
    float v = -1.0f;
    CHECK_FALSE(r.read_f32(v));
    CHECK(v == -1.0f);  // unchanged
}

TEST_CASE("LE read_f32 negative zero round-trip") {
    uint8_t buf[4];
    LEWriter w(buf, 4);
    CHECK(w.write_f32(-0.0f));
    LEReader r(buf, 4);
    float v = 1.0f;
    CHECK(r.read_f32(v));
    CHECK(std::signbit(v));  // negative zero
    CHECK(v == 0.0f);        // compares equal to +0
}

TEST_CASE("LE read_f32 infinity round-trip") {
    uint8_t buf[4];
    float inf = std::numeric_limits<float>::infinity();
    LEWriter w(buf, 4);
    CHECK(w.write_f32(inf));
    LEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(v == inf);
    CHECK(std::isinf(v));
}

TEST_CASE("LE read_f32 negative infinity round-trip") {
    uint8_t buf[4];
    float neg_inf = -std::numeric_limits<float>::infinity();
    LEWriter w(buf, 4);
    CHECK(w.write_f32(neg_inf));
    LEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(v == neg_inf);
}

TEST_CASE("LE read_f32 NaN round-trip") {
    uint8_t buf[4];
    float nan_val = std::numeric_limits<float>::quiet_NaN();
    LEWriter w(buf, 4);
    CHECK(w.write_f32(nan_val));
    LEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(std::isnan(v));
}

TEST_CASE("LE read_f32 smallest subnormal round-trip") {
    uint8_t buf[4];
    float denorm = std::numeric_limits<float>::denorm_min();
    LEWriter w(buf, 4);
    CHECK(w.write_f32(denorm));
    LEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(v == denorm);
}

// ============================================================================
// ByteReaderT – read_bytes
// ============================================================================

TEST_CASE("LE read_bytes success") {
    const uint8_t data[] = {0x0A, 0x0B, 0x0C, 0x0D};
    LEReader r(data, 4);
    uint8_t out[4] = {};
    CHECK(r.read_bytes(out, 4));
    CHECK(out[0] == 0x0A);
    CHECK(out[1] == 0x0B);
    CHECK(out[2] == 0x0C);
    CHECK(out[3] == 0x0D);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_bytes requesting more than available fails") {
    const uint8_t data[] = {0x0A, 0x0B};
    LEReader r(data, 2);
    uint8_t out[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    CHECK_FALSE(r.read_bytes(out, 3));
    // buffer content undefined after failure; remaining unchanged
    CHECK(r.remaining() == 2);
}

TEST_CASE("LE read_bytes zero length succeeds on empty buffer") {
    LEReader r(nullptr, 0);
    uint8_t dummy = 0;
    CHECK(r.read_bytes(&dummy, 0));
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_bytes zero length succeeds on non-empty buffer") {
    uint8_t data[] = {0x01};
    LEReader r(data, 1);
    uint8_t dummy = 0;
    CHECK(r.read_bytes(&dummy, 0));
    CHECK(r.remaining() == 1);  // no bytes consumed
}

// ============================================================================
// ByteReaderT – skip
// ============================================================================

TEST_CASE("LE skip success") {
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04};
    LEReader r(buf, 4);
    CHECK(r.skip(2));
    CHECK(r.remaining() == 2);
    uint8_t v = 0;
    CHECK(r.read_u8(v));
    CHECK(v == 0x03);
}

TEST_CASE("LE skip entire buffer") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    CHECK(r.skip(8));
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE skip past end fails") {
    uint8_t buf[] = {0x01};
    LEReader r(buf, 1);
    CHECK_FALSE(r.skip(2));
    CHECK(r.remaining() == 1);  // position unchanged
}

TEST_CASE("LE skip zero on empty buffer succeeds") {
    LEReader r(nullptr, 0);
    CHECK(r.skip(0));
    CHECK(r.remaining() == 0);
}

// ============================================================================
// ByteReaderT – sequential reads exhausting buffer
// ============================================================================

TEST_CASE("LE sequential reads exhaust buffer then fail") {
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    LEReader r(buf, 5);
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    CHECK(r.read_u8(v8));    // consume 1, remaining 4
    CHECK(v8 == 0x01);
    CHECK(r.remaining() == 4);
    CHECK(r.read_u16(v16));  // consume 2, remaining 2
    CHECK(r.remaining() == 2);
    uint32_t v32 = 0;
    CHECK_FALSE(r.read_u32(v32)); // need 4, have 2 – FAIL
    CHECK(r.remaining() == 2);    // unchanged
}

// ============================================================================
// ByteReaderT – BigEndian variant
// ============================================================================

TEST_CASE("BE read_u16 success") {
    uint8_t buf[] = {0x12, 0x34};
    BEReader r(buf, 2);
    uint16_t v = 0;
    CHECK(r.read_u16(v));
    CHECK(v == 0x1234);
}

TEST_CASE("BE read_u32 success") {
    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78};
    BEReader r(buf, 4);
    uint32_t v = 0;
    CHECK(r.read_u32(v));
    CHECK(v == 0x12345678u);
}

TEST_CASE("BE read_u64 success") {
    uint8_t buf[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    BEReader r(buf, 8);
    uint64_t v = 0;
    CHECK(r.read_u64(v));
    CHECK(v == 0x0123456789ABCDEFull);
}

TEST_CASE("BE read_u16 out of range") {
    uint8_t buf[] = {0x12};
    BEReader r(buf, 1);
    uint16_t v = 0;
    CHECK_FALSE(r.read_u16(v));
}

TEST_CASE("BE read_f32 success") {
    uint8_t buf[4];
    BigEndianCodec::StoreU32(buf, 0x3F800000u);
    BEReader r(buf, 4);
    float v = 0.0f;
    CHECK(r.read_f32(v));
    CHECK(v == 1.0f);
}

// ============================================================================
// ByteWriterT – construction and remaining()
// ============================================================================

TEST_CASE("Writer remaining equals buffer size on construction") {
    uint8_t buf[16] = {};
    LEWriter w(buf, sizeof(buf));
    CHECK(w.remaining() == 16);
}

TEST_CASE("Writer remaining is 0 for zero-length buffer") {
    LEWriter w(nullptr, 0);
    CHECK(w.remaining() == 0);
}

// ============================================================================
// ByteWriterT – write_u8
// ============================================================================

TEST_CASE("LE write_u8 success") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK(w.write_u8(0xAB));
    CHECK(buf[0] == 0xAB);
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE write_u8 on full buffer fails") {
    LEWriter w(nullptr, 0);
    CHECK_FALSE(w.write_u8(0x01));
}

TEST_CASE("LE write_u8 after exhaustion fails") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK(w.write_u8(0x42));
    CHECK_FALSE(w.write_u8(0x43));
    CHECK(buf[0] == 0x42);  // first write intact
}

// ============================================================================
// ByteWriterT – write_u16 out-of-range
// ============================================================================

TEST_CASE("LE write_u16 success") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    CHECK(w.write_u16(0x1234));
    CHECK(buf[0] == 0x34);
    CHECK(buf[1] == 0x12);
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE write_u16 fails with 1 byte remaining") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK_FALSE(w.write_u16(0x1234));
    CHECK(w.remaining() == 1);  // unchanged
}

TEST_CASE("LE write_u16 fails on empty buffer") {
    LEWriter w(nullptr, 0);
    CHECK_FALSE(w.write_u16(0x1234));
}

// ============================================================================
// ByteWriterT – write_u32 out-of-range
// ============================================================================

TEST_CASE("LE write_u32 success") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_u32(0x12345678u));
    CHECK(buf[0] == 0x78);
    CHECK(buf[1] == 0x56);
    CHECK(buf[2] == 0x34);
    CHECK(buf[3] == 0x12);
}

TEST_CASE("LE write_u32 fails with 3 bytes remaining") {
    uint8_t buf[3] = {};
    LEWriter w(buf, 3);
    CHECK_FALSE(w.write_u32(0x12345678u));
    CHECK(w.remaining() == 3);
}

TEST_CASE("LE write_u32 fails on empty buffer") {
    LEWriter w(nullptr, 0);
    CHECK_FALSE(w.write_u32(0x12345678u));
}

// ============================================================================
// ByteWriterT – write_u64 out-of-range
// ============================================================================

TEST_CASE("LE write_u64 success") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_u64(0x0123456789ABCDEFull));
    CHECK(buf[0] == 0xEF);
    CHECK(buf[7] == 0x01);
}

TEST_CASE("LE write_u64 fails with 7 bytes remaining") {
    uint8_t buf[7] = {};
    LEWriter w(buf, 7);
    CHECK_FALSE(w.write_u64(0x0123456789ABCDEFull));
    CHECK(w.remaining() == 7);
}

TEST_CASE("LE write_u64 fails on empty buffer") {
    LEWriter w(nullptr, 0);
    CHECK_FALSE(w.write_u64(0x01ull));
}

// ============================================================================
// ByteWriterT – write_f32
// ============================================================================

TEST_CASE("LE write_f32 success") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_f32(1.0f));
    // 1.0f == 0x3F800000
    CHECK(LittleEndianCodec::LoadU32(buf) == 0x3F800000u);
}

TEST_CASE("LE write_f32 fails with 3 bytes remaining") {
    uint8_t buf[3] = {};
    LEWriter w(buf, 3);
    CHECK_FALSE(w.write_f32(1.0f));
    CHECK(w.remaining() == 3);
}

// ============================================================================
// ByteWriterT – write_bytes
// ============================================================================

TEST_CASE("LE write_bytes success") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    const uint8_t src[] = {0x0A, 0x0B, 0x0C, 0x0D};
    CHECK(w.write_bytes(src, 4));
    CHECK(buf[0] == 0x0A);
    CHECK(buf[3] == 0x0D);
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE write_bytes fails when len exceeds remaining") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    const uint8_t src[] = {0x01, 0x02, 0x03};
    CHECK_FALSE(w.write_bytes(src, 3));
    CHECK(w.remaining() == 2);
}

TEST_CASE("LE write_bytes zero length succeeds on empty buffer") {
    LEWriter w(nullptr, 0);
    uint8_t dummy = 0;
    CHECK(w.write_bytes(&dummy, 0));
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE write_bytes zero length succeeds on non-empty buffer") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    uint8_t dummy = 0;
    CHECK(w.write_bytes(&dummy, 0));
    CHECK(w.remaining() == 4);  // no bytes consumed
}

// ============================================================================
// ByteWriterT – sequential writes exhausting buffer
// ============================================================================

TEST_CASE("LE sequential writes exhaust buffer then fail") {
    uint8_t buf[5] = {};
    LEWriter w(buf, 5);
    CHECK(w.write_u8(0x01));      // consume 1, remaining 4
    CHECK(w.remaining() == 4);
    CHECK(w.write_u16(0x0203));   // consume 2, remaining 2
    CHECK(w.remaining() == 2);
    CHECK_FALSE(w.write_u32(0x01020304u)); // need 4, have 2 – FAIL
    CHECK(w.remaining() == 2);            // unchanged
}

// ============================================================================
// ByteWriterT – BigEndian variant
// ============================================================================

TEST_CASE("BE write_u16 success") {
    uint8_t buf[2] = {};
    BEWriter w(buf, 2);
    CHECK(w.write_u16(0x1234));
    CHECK(buf[0] == 0x12);
    CHECK(buf[1] == 0x34);
}

TEST_CASE("BE write_u32 success") {
    uint8_t buf[4] = {};
    BEWriter w(buf, 4);
    CHECK(w.write_u32(0x12345678u));
    CHECK(buf[0] == 0x12);
    CHECK(buf[3] == 0x78);
}

TEST_CASE("BE write_u64 success") {
    uint8_t buf[8] = {};
    BEWriter w(buf, 8);
    CHECK(w.write_u64(0x0123456789ABCDEFull));
    CHECK(buf[0] == 0x01);
    CHECK(buf[7] == 0xEF);
}

TEST_CASE("BE write_u16 out of range") {
    uint8_t buf[1] = {};
    BEWriter w(buf, 1);
    CHECK_FALSE(w.write_u16(0x1234));
}

TEST_CASE("BE write_f32 success") {
    uint8_t buf[4] = {};
    BEWriter w(buf, 4);
    CHECK(w.write_f32(1.0f));
    CHECK(BigEndianCodec::LoadU32(buf) == 0x3F800000u);
}

// ============================================================================
// Full round-trip: Writer -> Reader (LE)
// ============================================================================

TEST_CASE("LE write then read all types round-trip") {
    uint8_t buf[64] = {};

    // Write
    LEWriter w(buf, sizeof(buf));
    CHECK(w.write_u8(0x42));
    CHECK(w.write_u16(0xBEEF));
    CHECK(w.write_u32(0xDEADBEEFu));
    CHECK(w.write_u64(0xCAFEBABE12345678ull));
    CHECK(w.write_f32(3.14f));
    const uint8_t raw[] = {0xAA, 0xBB, 0xCC};
    CHECK(w.write_bytes(raw, 3));

    size_t written = sizeof(buf) - w.remaining();

    // Read back
    LEReader r(buf, written);
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    uint32_t v32 = 0;
    uint64_t v64 = 0;
    float vf = 0.0f;
    uint8_t vraw[3] = {};

    CHECK(r.read_u8(v8));
    CHECK(v8 == 0x42);
    CHECK(r.read_u16(v16));
    CHECK(v16 == 0xBEEF);
    CHECK(r.read_u32(v32));
    CHECK(v32 == 0xDEADBEEFu);
    CHECK(r.read_u64(v64));
    CHECK(v64 == 0xCAFEBABE12345678ull);
    CHECK(r.read_f32(vf));
    CHECK(vf == doctest::Approx(3.14f));
    CHECK(r.read_bytes(vraw, 3));
    CHECK(vraw[0] == 0xAA);
    CHECK(vraw[1] == 0xBB);
    CHECK(vraw[2] == 0xCC);
    CHECK(r.remaining() == 0);
}

// ============================================================================
// Full round-trip: Writer -> Reader (BE)
// ============================================================================

TEST_CASE("BE write then read all types round-trip") {
    uint8_t buf[64] = {};

    BEWriter w(buf, sizeof(buf));
    CHECK(w.write_u8(0x42));
    CHECK(w.write_u16(0xBEEF));
    CHECK(w.write_u32(0xDEADBEEFu));
    CHECK(w.write_u64(0xCAFEBABE12345678ull));
    CHECK(w.write_f32(-0.5f));
    const uint8_t raw[] = {0xDD, 0xEE};
    CHECK(w.write_bytes(raw, 2));

    size_t written = sizeof(buf) - w.remaining();

    BEReader r(buf, written);
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    uint32_t v32 = 0;
    uint64_t v64 = 0;
    float vf = 0.0f;
    uint8_t vraw[2] = {};

    CHECK(r.read_u8(v8));
    CHECK(v8 == 0x42);
    CHECK(r.read_u16(v16));
    CHECK(v16 == 0xBEEF);
    CHECK(r.read_u32(v32));
    CHECK(v32 == 0xDEADBEEFu);
    CHECK(r.read_u64(v64));
    CHECK(v64 == 0xCAFEBABE12345678ull);
    CHECK(r.read_f32(vf));
    CHECK(vf == doctest::Approx(-0.5f));
    CHECK(r.read_bytes(vraw, 2));
    CHECK(vraw[0] == 0xDD);
    CHECK(vraw[1] == 0xEE);
    CHECK(r.remaining() == 0);
}

// ============================================================================
// Boundary: exactly-sized buffer succeeds, one-less fails
// ============================================================================

TEST_CASE("Exact buffer for u16 succeeds, one byte short fails") {
    uint8_t buf2[2] = {0x01, 0x02};
    LEReader r2(buf2, 2);
    uint16_t v = 0;
    CHECK(r2.read_u16(v));

    uint8_t buf1[1] = {0x01};
    LEReader r1(buf1, 1);
    CHECK_FALSE(r1.read_u16(v));
}

TEST_CASE("Exact buffer for u32 succeeds, one byte short fails") {
    uint8_t buf4[4] = {};
    LEReader r4(buf4, 4);
    uint32_t v = 0;
    CHECK(r4.read_u32(v));

    uint8_t buf3[3] = {};
    LEReader r3(buf3, 3);
    CHECK_FALSE(r3.read_u32(v));
}

TEST_CASE("Exact buffer for u64 succeeds, one byte short fails") {
    uint8_t buf8[8] = {};
    LEReader r8(buf8, 8);
    uint64_t v = 0;
    CHECK(r8.read_u64(v));

    uint8_t buf7[7] = {};
    LEReader r7(buf7, 7);
    CHECK_FALSE(r7.read_u64(v));
}

// ============================================================================
// Failed reads/writes do not advance the cursor
// ============================================================================

TEST_CASE("Failed read_u32 does not consume bytes") {
    uint8_t buf[] = {0x01, 0x02, 0x03};
    LEReader r(buf, 3);
    uint32_t v32 = 0;
    CHECK_FALSE(r.read_u32(v32));
    // Should still be able to read the 3 individual bytes
    uint8_t a = 0, b = 0, c = 0;
    CHECK(r.read_u8(a));
    CHECK(r.read_u8(b));
    CHECK(r.read_u8(c));
    CHECK(a == 0x01);
    CHECK(b == 0x02);
    CHECK(c == 0x03);
    CHECK(r.remaining() == 0);
}

TEST_CASE("Failed write_u32 does not consume space") {
    uint8_t buf[3] = {};
    LEWriter w(buf, 3);
    CHECK_FALSE(w.write_u32(0x12345678u));
    // Should still be able to write 3 individual bytes
    CHECK(w.write_u8(0xAA));
    CHECK(w.write_u8(0xBB));
    CHECK(w.write_u8(0xCC));
    CHECK(buf[0] == 0xAA);
    CHECK(buf[1] == 0xBB);
    CHECK(buf[2] == 0xCC);
    CHECK(w.remaining() == 0);
}

// ============================================================================
// Max-value round-trips
// ============================================================================

TEST_CASE("LE max u8 round-trip") {
    uint8_t buf[1];
    LEWriter w(buf, 1);
    CHECK(w.write_u8(0xFF));
    LEReader r(buf, 1);
    uint8_t v = 0;
    CHECK(r.read_u8(v));
    CHECK(v == 0xFF);
}

TEST_CASE("LE max u16 round-trip") {
    uint8_t buf[2];
    LEWriter w(buf, 2);
    CHECK(w.write_u16(0xFFFF));
    LEReader r(buf, 2);
    uint16_t v = 0;
    CHECK(r.read_u16(v));
    CHECK(v == 0xFFFF);
}

TEST_CASE("LE max u32 round-trip") {
    uint8_t buf[4];
    LEWriter w(buf, 4);
    CHECK(w.write_u32(0xFFFFFFFFu));
    LEReader r(buf, 4);
    uint32_t v = 0;
    CHECK(r.read_u32(v));
    CHECK(v == 0xFFFFFFFFu);
}

TEST_CASE("LE max u64 round-trip") {
    uint8_t buf[8];
    LEWriter w(buf, 8);
    CHECK(w.write_u64(0xFFFFFFFFFFFFFFFFull));
    LEReader r(buf, 8);
    uint64_t v = 0;
    CHECK(r.read_u64(v));
    CHECK(v == 0xFFFFFFFFFFFFFFFFull);
}

// ============================================================================
// Zero-value round-trips
// ============================================================================

TEST_CASE("LE zero u16 round-trip") {
    uint8_t buf[2] = {0xFF, 0xFF};
    LEWriter w(buf, 2);
    CHECK(w.write_u16(0));
    LEReader r(buf, 2);
    uint16_t v = 0xFFFF;
    CHECK(r.read_u16(v));
    CHECK(v == 0);
}

TEST_CASE("LE zero u32 round-trip") {
    uint8_t buf[4];
    std::memset(buf, 0xFF, 4);
    LEWriter w(buf, 4);
    CHECK(w.write_u32(0));
    LEReader r(buf, 4);
    uint32_t v = 0xFFFFFFFF;
    CHECK(r.read_u32(v));
    CHECK(v == 0);
}

TEST_CASE("LE zero u64 round-trip") {
    uint8_t buf[8];
    std::memset(buf, 0xFF, 8);
    LEWriter w(buf, 8);
    CHECK(w.write_u64(0));
    LEReader r(buf, 8);
    uint64_t v = 0xFFFFFFFFFFFFFFFF;
    CHECK(r.read_u64(v));
    CHECK(v == 0);
}

// ============================================================================
// ByteReaderT – position()
// ============================================================================

TEST_CASE("Reader position starts at 0") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    CHECK(r.position() == 0);
}

TEST_CASE("Reader position starts at 0 for empty buffer") {
    LEReader r(nullptr, 0);
    CHECK(r.position() == 0);
}

TEST_CASE("Reader position advances after read_u8") {
    uint8_t buf[4] = {};
    LEReader r(buf, 4);
    uint8_t v = 0;
    CHECK(r.read_u8(v));
    CHECK(r.position() == 1);
    CHECK(r.read_u8(v));
    CHECK(r.position() == 2);
}

TEST_CASE("Reader position advances after read_u16") {
    uint8_t buf[4] = {};
    LEReader r(buf, 4);
    uint16_t v = 0;
    CHECK(r.read_u16(v));
    CHECK(r.position() == 2);
}

TEST_CASE("Reader position advances after read_u32") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    uint32_t v = 0;
    CHECK(r.read_u32(v));
    CHECK(r.position() == 4);
}

TEST_CASE("Reader position advances after read_u64") {
    uint8_t buf[16] = {};
    LEReader r(buf, 16);
    uint64_t v = 0;
    CHECK(r.read_u64(v));
    CHECK(r.position() == 8);
}

TEST_CASE("Reader position advances after read_f32") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    float v = 0;
    CHECK(r.read_f32(v));
    CHECK(r.position() == 4);
}

TEST_CASE("Reader position advances after read_bytes") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    uint8_t out[3] = {};
    CHECK(r.read_bytes(out, 3));
    CHECK(r.position() == 3);
}

TEST_CASE("Reader position advances after skip") {
    uint8_t buf[8] = {};
    LEReader r(buf, 8);
    CHECK(r.skip(5));
    CHECK(r.position() == 5);
}

TEST_CASE("Reader position unchanged after failed read") {
    uint8_t buf[2] = {};
    LEReader r(buf, 2);
    uint32_t v = 0;
    CHECK_FALSE(r.read_u32(v));
    CHECK(r.position() == 0);
}

TEST_CASE("Reader position unchanged after failed skip") {
    uint8_t buf[2] = {};
    LEReader r(buf, 2);
    CHECK_FALSE(r.skip(5));
    CHECK(r.position() == 0);
}

TEST_CASE("Reader position tracks mixed operations") {
    uint8_t buf[16] = {};
    LEReader r(buf, 16);
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    CHECK(r.read_u8(v8));     // pos = 1
    CHECK(r.position() == 1);
    CHECK(r.read_u16(v16));   // pos = 3
    CHECK(r.position() == 3);
    CHECK(r.skip(2));         // pos = 5
    CHECK(r.position() == 5);
    CHECK(r.position() + r.remaining() == 16);
}

// ============================================================================
// ByteWriterT – position()
// ============================================================================

TEST_CASE("Writer position starts at 0") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.position() == 0);
}

TEST_CASE("Writer position starts at 0 for empty buffer") {
    LEWriter w(nullptr, 0);
    CHECK(w.position() == 0);
}

TEST_CASE("Writer position advances after write_u8") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_u8(0x01));
    CHECK(w.position() == 1);
    CHECK(w.write_u8(0x02));
    CHECK(w.position() == 2);
}

TEST_CASE("Writer position advances after write_u16") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_u16(0x1234));
    CHECK(w.position() == 2);
}

TEST_CASE("Writer position advances after write_u32") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_u32(0x12345678u));
    CHECK(w.position() == 4);
}

TEST_CASE("Writer position advances after write_u64") {
    uint8_t buf[16] = {};
    LEWriter w(buf, 16);
    CHECK(w.write_u64(0x01ull));
    CHECK(w.position() == 8);
}

TEST_CASE("Writer position advances after write_f32") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_f32(1.0f));
    CHECK(w.position() == 4);
}

TEST_CASE("Writer position advances after write_bytes") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    const uint8_t src[] = {0x01, 0x02, 0x03};
    CHECK(w.write_bytes(src, 3));
    CHECK(w.position() == 3);
}

TEST_CASE("Writer position advances after skip") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.skip(5));
    CHECK(w.position() == 5);
}

TEST_CASE("Writer position unchanged after failed write") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    CHECK_FALSE(w.write_u32(0x12345678u));
    CHECK(w.position() == 0);
}

TEST_CASE("Writer position unchanged after failed skip") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    CHECK_FALSE(w.skip(5));
    CHECK(w.position() == 0);
}

TEST_CASE("Writer position tracks mixed operations") {
    uint8_t buf[16] = {};
    LEWriter w(buf, 16);
    CHECK(w.write_u8(0x01));   // pos = 1
    CHECK(w.position() == 1);
    CHECK(w.write_u16(0x0203)); // pos = 3
    CHECK(w.position() == 3);
    CHECK(w.skip(2));           // pos = 5
    CHECK(w.position() == 5);
    CHECK(w.position() + w.remaining() == 16);
}

// ============================================================================
// ByteWriterT – skip()
// ============================================================================

TEST_CASE("LE writer skip success") {
    uint8_t buf[] = {0xFF, 0xFF, 0xFF, 0xFF};
    LEWriter w(buf, 4);
    CHECK(w.skip(2));
    CHECK(w.remaining() == 2);
    CHECK(w.write_u8(0xAA));
    CHECK(buf[2] == 0xAA);
}

TEST_CASE("LE writer skip entire buffer") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.skip(8));
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE writer skip past end fails") {
    uint8_t buf[] = {0x01};
    LEWriter w(buf, 1);
    CHECK_FALSE(w.skip(2));
    CHECK(w.remaining() == 1);  // unchanged
}

TEST_CASE("LE writer skip zero on empty buffer succeeds") {
    LEWriter w(nullptr, 0);
    CHECK(w.skip(0));
    CHECK(w.remaining() == 0);
}

TEST_CASE("LE writer skip zero on non-empty buffer succeeds") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.skip(0));
    CHECK(w.remaining() == 4);  // nothing consumed
}

TEST_CASE("LE writer skip then write verifies position") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_u16(0x1234));  // bytes 0-1
    CHECK(w.skip(2));            // skip bytes 2-3
    CHECK(w.write_u16(0x5678));  // bytes 4-5
    CHECK(w.remaining() == 2);

    // Verify skipped bytes were not modified (still zero-initialized)
    CHECK(buf[2] == 0x00);
    CHECK(buf[3] == 0x00);

    // Verify written values
    CHECK(LittleEndianCodec::LoadU16(buf) == 0x1234);
    CHECK(LittleEndianCodec::LoadU16(buf + 4) == 0x5678);
}

TEST_CASE("LE writer skip fails then write still works") {
    uint8_t buf[3] = {};
    LEWriter w(buf, 3);
    CHECK_FALSE(w.skip(4));    // fail – need 4, have 3
    CHECK(w.remaining() == 3); // unchanged
    CHECK(w.write_u8(0xBB));   // should still succeed
    CHECK(buf[0] == 0xBB);
    CHECK(w.remaining() == 2);
}

TEST_CASE("BE writer skip success") {
    uint8_t buf[4] = {};
    BEWriter w(buf, 4);
    CHECK(w.skip(2));
    CHECK(w.remaining() == 2);
    CHECK(w.write_u16(0x1234));
    CHECK(buf[2] == 0x12);
    CHECK(buf[3] == 0x34);
}

// ============================================================================
// Round-trip with skip on both reader and writer
// ============================================================================

TEST_CASE("Writer skip + Reader skip round-trip") {
    uint8_t buf[16] = {};
    LEWriter w(buf, 16);
    CHECK(w.write_u32(0xAAAAAAAAu));  // bytes 0-3
    CHECK(w.skip(4));                  // skip bytes 4-7 (reserved)
    CHECK(w.write_u32(0xBBBBBBBBu));  // bytes 8-11

    LEReader r(buf, 16);
    uint32_t v1 = 0, v2 = 0;
    CHECK(r.read_u32(v1));
    CHECK(v1 == 0xAAAAAAAAu);
    CHECK(r.skip(4));  // skip the reserved region
    CHECK(r.read_u32(v2));
    CHECK(v2 == 0xBBBBBBBBu);
    CHECK(r.remaining() == 4);
}

// ============================================================================
// ByteReaderT – read_i8
// ============================================================================

TEST_CASE("LE read_i8 positive value") {
    uint8_t buf[] = {0x7F};
    LEReader r(buf, 1);
    int8_t v = 0;
    CHECK(r.read_i8(v));
    CHECK(v == 127);
    CHECK(r.remaining() == 0);
}

TEST_CASE("LE read_i8 negative value") {
    uint8_t buf[] = {0x80};
    LEReader r(buf, 1);
    int8_t v = 0;
    CHECK(r.read_i8(v));
    CHECK(v == -128);
}

TEST_CASE("LE read_i8 minus one") {
    uint8_t buf[] = {0xFF};
    LEReader r(buf, 1);
    int8_t v = 0;
    CHECK(r.read_i8(v));
    CHECK(v == -1);
}

TEST_CASE("LE read_i8 zero") {
    uint8_t buf[] = {0x00};
    LEReader r(buf, 1);
    int8_t v = 42;
    CHECK(r.read_i8(v));
    CHECK(v == 0);
}

TEST_CASE("LE read_i8 on empty buffer fails") {
    LEReader r(nullptr, 0);
    int8_t v = 42;
    CHECK_FALSE(r.read_i8(v));
    CHECK(v == 42);
}

// ============================================================================
// ByteReaderT – read_i16
// ============================================================================

TEST_CASE("LE read_i16 positive value") {
    uint8_t buf[2];
    LittleEndianCodec::StoreU16(buf, 0x7FFF);
    LEReader r(buf, 2);
    int16_t v = 0;
    CHECK(r.read_i16(v));
    CHECK(v == 32767);
}

TEST_CASE("LE read_i16 negative value") {
    uint8_t buf[2];
    LittleEndianCodec::StoreU16(buf, 0x8000);
    LEReader r(buf, 2);
    int16_t v = 0;
    CHECK(r.read_i16(v));
    CHECK(v == -32768);
}

TEST_CASE("LE read_i16 minus one") {
    uint8_t buf[2];
    LittleEndianCodec::StoreU16(buf, 0xFFFF);
    LEReader r(buf, 2);
    int16_t v = 0;
    CHECK(r.read_i16(v));
    CHECK(v == -1);
}

TEST_CASE("LE read_i16 fails with 1 byte") {
    uint8_t buf[] = {0x01};
    LEReader r(buf, 1);
    int16_t v = 0;
    CHECK_FALSE(r.read_i16(v));
}

TEST_CASE("BE read_i16 positive value") {
    uint8_t buf[2];
    BigEndianCodec::StoreU16(buf, 0x0102);
    BEReader r(buf, 2);
    int16_t v = 0;
    CHECK(r.read_i16(v));
    CHECK(v == 0x0102);
}

// ============================================================================
// ByteReaderT – read_i32
// ============================================================================

TEST_CASE("LE read_i32 positive value") {
    uint8_t buf[4];
    LittleEndianCodec::StoreU32(buf, 0x7FFFFFFFu);
    LEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == 2147483647);
}

TEST_CASE("LE read_i32 negative value") {
    uint8_t buf[4];
    LittleEndianCodec::StoreU32(buf, 0x80000000u);
    LEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == (-2147483647 - 1));
}

TEST_CASE("LE read_i32 minus one") {
    uint8_t buf[4];
    LittleEndianCodec::StoreU32(buf, 0xFFFFFFFFu);
    LEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == -1);
}

TEST_CASE("LE read_i32 fails with 3 bytes") {
    uint8_t buf[3] = {};
    LEReader r(buf, 3);
    int32_t v = 0;
    CHECK_FALSE(r.read_i32(v));
}

TEST_CASE("BE read_i32 negative value") {
    uint8_t buf[4];
    BigEndianCodec::StoreU32(buf, 0xFFFFFFFEu);
    BEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == -2);
}

// ============================================================================
// ByteReaderT – read_i64
// ============================================================================

TEST_CASE("LE read_i64 positive value") {
    uint8_t buf[8];
    LittleEndianCodec::StoreU64(buf, 0x7FFFFFFFFFFFFFFFull);
    LEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == INT64_MAX);
}

TEST_CASE("LE read_i64 negative value") {
    uint8_t buf[8];
    LittleEndianCodec::StoreU64(buf, 0x8000000000000000ull);
    LEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == INT64_MIN);
}

TEST_CASE("LE read_i64 minus one") {
    uint8_t buf[8];
    LittleEndianCodec::StoreU64(buf, 0xFFFFFFFFFFFFFFFFull);
    LEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == -1);
}

TEST_CASE("LE read_i64 fails with 7 bytes") {
    uint8_t buf[7] = {};
    LEReader r(buf, 7);
    int64_t v = 0;
    CHECK_FALSE(r.read_i64(v));
}

TEST_CASE("BE read_i64 negative value") {
    uint8_t buf[8];
    BigEndianCodec::StoreU64(buf, 0xFFFFFFFFFFFFFFFEull);
    BEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == -2);
}

// ============================================================================
// ByteReaderT – read_f64
// ============================================================================

TEST_CASE("LE read_f64 success with 1.0") {
    // IEEE-754: 1.0 = 0x3FF0000000000000
    uint8_t buf[8];
    LittleEndianCodec::StoreU64(buf, 0x3FF0000000000000ull);
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == 1.0);
}

TEST_CASE("LE read_f64 fails with 7 bytes") {
    uint8_t buf[7] = {};
    LEReader r(buf, 7);
    double v = -1.0;
    CHECK_FALSE(r.read_f64(v));
    CHECK(v == -1.0);
}

TEST_CASE("LE read_f64 negative zero round-trip") {
    uint8_t buf[8];
    LEWriter w(buf, 8);
    CHECK(w.write_f64(-0.0));
    LEReader r(buf, 8);
    double v = 1.0;
    CHECK(r.read_f64(v));
    CHECK(std::signbit(v));
    CHECK(v == 0.0);
}

TEST_CASE("LE read_f64 infinity round-trip") {
    uint8_t buf[8];
    double inf = std::numeric_limits<double>::infinity();
    LEWriter w(buf, 8);
    CHECK(w.write_f64(inf));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == inf);
    CHECK(std::isinf(v));
}

TEST_CASE("LE read_f64 negative infinity round-trip") {
    uint8_t buf[8];
    double neg_inf = -std::numeric_limits<double>::infinity();
    LEWriter w(buf, 8);
    CHECK(w.write_f64(neg_inf));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == neg_inf);
}

TEST_CASE("LE read_f64 NaN round-trip") {
    uint8_t buf[8];
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    LEWriter w(buf, 8);
    CHECK(w.write_f64(nan_val));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(std::isnan(v));
}

TEST_CASE("LE read_f64 smallest subnormal round-trip") {
    uint8_t buf[8];
    double denorm = std::numeric_limits<double>::denorm_min();
    LEWriter w(buf, 8);
    CHECK(w.write_f64(denorm));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == denorm);
}

TEST_CASE("LE read_f64 max value round-trip") {
    uint8_t buf[8];
    double maxv = std::numeric_limits<double>::max();
    LEWriter w(buf, 8);
    CHECK(w.write_f64(maxv));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == maxv);
}

TEST_CASE("BE read_f64 success with 1.0") {
    uint8_t buf[8];
    BigEndianCodec::StoreU64(buf, 0x3FF0000000000000ull);
    BEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == 1.0);
}

// ============================================================================
// ByteWriterT – write_i8
// ============================================================================

TEST_CASE("LE write_i8 positive value") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK(w.write_i8(127));
    CHECK(buf[0] == 0x7F);
}

TEST_CASE("LE write_i8 negative value") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK(w.write_i8(-128));
    CHECK(buf[0] == 0x80);
}

TEST_CASE("LE write_i8 minus one") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK(w.write_i8(-1));
    CHECK(buf[0] == 0xFF);
}

TEST_CASE("LE write_i8 on empty buffer fails") {
    LEWriter w(nullptr, 0);
    CHECK_FALSE(w.write_i8(1));
}

// ============================================================================
// ByteWriterT – write_i16
// ============================================================================

TEST_CASE("LE write_i16 positive value") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    CHECK(w.write_i16(0x0102));
    CHECK(LittleEndianCodec::LoadU16(buf) == 0x0102);
}

TEST_CASE("LE write_i16 negative value") {
    uint8_t buf[2] = {};
    LEWriter w(buf, 2);
    CHECK(w.write_i16(-1));
    CHECK(LittleEndianCodec::LoadU16(buf) == 0xFFFF);
}

TEST_CASE("LE write_i16 fails with 1 byte") {
    uint8_t buf[1] = {};
    LEWriter w(buf, 1);
    CHECK_FALSE(w.write_i16(1));
}

TEST_CASE("BE write_i16 negative value") {
    uint8_t buf[2] = {};
    BEWriter w(buf, 2);
    CHECK(w.write_i16(-1));
    CHECK(buf[0] == 0xFF);
    CHECK(buf[1] == 0xFF);
}

// ============================================================================
// ByteWriterT – write_i32
// ============================================================================

TEST_CASE("LE write_i32 positive value") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_i32(0x12345678));
    CHECK(LittleEndianCodec::LoadU32(buf) == 0x12345678u);
}

TEST_CASE("LE write_i32 negative value") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_i32(-1));
    CHECK(LittleEndianCodec::LoadU32(buf) == 0xFFFFFFFFu);
}

TEST_CASE("LE write_i32 min value") {
    uint8_t buf[4] = {};
    LEWriter w(buf, 4);
    CHECK(w.write_i32(-2147483647 - 1));
    CHECK(LittleEndianCodec::LoadU32(buf) == 0x80000000u);
}

TEST_CASE("LE write_i32 fails with 3 bytes") {
    uint8_t buf[3] = {};
    LEWriter w(buf, 3);
    CHECK_FALSE(w.write_i32(1));
}

// ============================================================================
// ByteWriterT – write_i64
// ============================================================================

TEST_CASE("LE write_i64 positive value") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_i64(INT64_MAX));
    CHECK(LittleEndianCodec::LoadU64(buf) == 0x7FFFFFFFFFFFFFFFull);
}

TEST_CASE("LE write_i64 negative value") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_i64(-1));
    CHECK(LittleEndianCodec::LoadU64(buf) == 0xFFFFFFFFFFFFFFFFull);
}

TEST_CASE("LE write_i64 min value") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_i64(INT64_MIN));
    CHECK(LittleEndianCodec::LoadU64(buf) == 0x8000000000000000ull);
}

TEST_CASE("LE write_i64 fails with 7 bytes") {
    uint8_t buf[7] = {};
    LEWriter w(buf, 7);
    CHECK_FALSE(w.write_i64(1));
}

// ============================================================================
// ByteWriterT – write_f64
// ============================================================================

TEST_CASE("LE write_f64 success") {
    uint8_t buf[8] = {};
    LEWriter w(buf, 8);
    CHECK(w.write_f64(1.0));
    CHECK(LittleEndianCodec::LoadU64(buf) == 0x3FF0000000000000ull);
}

TEST_CASE("LE write_f64 fails with 7 bytes") {
    uint8_t buf[7] = {};
    LEWriter w(buf, 7);
    CHECK_FALSE(w.write_f64(1.0));
    CHECK(w.remaining() == 7);
}

TEST_CASE("BE write_f64 success") {
    uint8_t buf[8] = {};
    BEWriter w(buf, 8);
    CHECK(w.write_f64(1.0));
    CHECK(BigEndianCodec::LoadU64(buf) == 0x3FF0000000000000ull);
}

// ============================================================================
// Signed integer round-trips
// ============================================================================

TEST_CASE("LE i8 write then read round-trip") {
    uint8_t buf[1];
    LEWriter w(buf, 1);
    CHECK(w.write_i8(-42));
    LEReader r(buf, 1);
    int8_t v = 0;
    CHECK(r.read_i8(v));
    CHECK(v == -42);
}

TEST_CASE("LE i16 write then read round-trip") {
    uint8_t buf[2];
    LEWriter w(buf, 2);
    CHECK(w.write_i16(-12345));
    LEReader r(buf, 2);
    int16_t v = 0;
    CHECK(r.read_i16(v));
    CHECK(v == -12345);
}

TEST_CASE("LE i32 write then read round-trip") {
    uint8_t buf[4];
    LEWriter w(buf, 4);
    CHECK(w.write_i32(-123456789));
    LEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == -123456789);
}

TEST_CASE("LE i64 write then read round-trip") {
    uint8_t buf[8];
    LEWriter w(buf, 8);
    CHECK(w.write_i64(-1234567890123456789LL));
    LEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == -1234567890123456789LL);
}

TEST_CASE("BE i32 write then read round-trip") {
    uint8_t buf[4];
    BEWriter w(buf, 4);
    CHECK(w.write_i32(-98765));
    BEReader r(buf, 4);
    int32_t v = 0;
    CHECK(r.read_i32(v));
    CHECK(v == -98765);
}

TEST_CASE("BE i64 write then read round-trip") {
    uint8_t buf[8];
    BEWriter w(buf, 8);
    CHECK(w.write_i64(INT64_MIN));
    BEReader r(buf, 8);
    int64_t v = 0;
    CHECK(r.read_i64(v));
    CHECK(v == INT64_MIN);
}

TEST_CASE("LE f64 write then read round-trip with pi") {
    uint8_t buf[8];
    LEWriter w(buf, 8);
    CHECK(w.write_f64(3.141592653589793));
    LEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == doctest::Approx(3.141592653589793));
}

TEST_CASE("BE f64 write then read round-trip with negative value") {
    uint8_t buf[8];
    BEWriter w(buf, 8);
    CHECK(w.write_f64(-2.718281828459045));
    BEReader r(buf, 8);
    double v = 0.0;
    CHECK(r.read_f64(v));
    CHECK(v == doctest::Approx(-2.718281828459045));
}

// ============================================================================
// Full round-trip with all types including signed and f64
// ============================================================================

TEST_CASE("LE write then read all types including signed and f64") {
    uint8_t buf[128] = {};

    LEWriter w(buf, sizeof(buf));
    CHECK(w.write_u8(0x42));
    CHECK(w.write_i8(-1));
    CHECK(w.write_u16(0xBEEF));
    CHECK(w.write_i16(-300));
    CHECK(w.write_u32(0xDEADBEEFu));
    CHECK(w.write_i32(-100000));
    CHECK(w.write_u64(0xCAFEBABE12345678ull));
    CHECK(w.write_i64(-9999999999LL));
    CHECK(w.write_f32(3.14f));
    CHECK(w.write_f64(2.718281828459045));

    size_t written = sizeof(buf) - w.remaining();

    LEReader r(buf, written);
    uint8_t vu8 = 0;
    int8_t vi8 = 0;
    uint16_t vu16 = 0;
    int16_t vi16 = 0;
    uint32_t vu32 = 0;
    int32_t vi32 = 0;
    uint64_t vu64 = 0;
    int64_t vi64 = 0;
    float vf32 = 0.0f;
    double vf64 = 0.0;

    CHECK(r.read_u8(vu8));
    CHECK(vu8 == 0x42);
    CHECK(r.read_i8(vi8));
    CHECK(vi8 == -1);
    CHECK(r.read_u16(vu16));
    CHECK(vu16 == 0xBEEF);
    CHECK(r.read_i16(vi16));
    CHECK(vi16 == -300);
    CHECK(r.read_u32(vu32));
    CHECK(vu32 == 0xDEADBEEFu);
    CHECK(r.read_i32(vi32));
    CHECK(vi32 == -100000);
    CHECK(r.read_u64(vu64));
    CHECK(vu64 == 0xCAFEBABE12345678ull);
    CHECK(r.read_i64(vi64));
    CHECK(vi64 == -9999999999LL);
    CHECK(r.read_f32(vf32));
    CHECK(vf32 == doctest::Approx(3.14f));
    CHECK(r.read_f64(vf64));
    CHECK(vf64 == doctest::Approx(2.718281828459045));
    CHECK(r.remaining() == 0);
}
"""Tests for the bio_generator package."""

from __future__ import annotations

import pathlib
import textwrap

import pytest

from bio_generator.parser import ParseError, load_protocol
from bio_generator.codegen import generate_header
from bio_generator.types import PRIMITIVES, TypeKind


FIXTURES_DIR = pathlib.Path(__file__).parent / "fixtures"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _write_yaml(tmp_path: pathlib.Path, content: str) -> pathlib.Path:
    p = tmp_path / "test.yaml"
    p.write_text(textwrap.dedent(content), encoding="utf-8")
    return p


# ---------------------------------------------------------------------------
# Type system
# ---------------------------------------------------------------------------

class TestPrimitives:
    def test_all_primitives_have_methods(self):
        for name, prim in PRIMITIVES.items():
            assert prim.read_method.startswith("read_")
            assert prim.write_method.startswith("write_")
            assert prim.size > 0

    def test_u8(self):
        p = PRIMITIVES["u8"]
        assert p.cpp_type == "uint8_t"
        assert p.size == 1

    def test_f64(self):
        p = PRIMITIVES["f64"]
        assert p.cpp_type == "double"
        assert p.size == 8


# ---------------------------------------------------------------------------
# Parser – valid inputs
# ---------------------------------------------------------------------------

class TestParserValid:
    def test_minimal_protocol(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            protocol:
              name: Minimal
            structs:
              - name: Msg
                fields:
                  - name: value
                    type: u8
        """)
        proto = load_protocol(path)
        assert proto.name == "Minimal"
        assert len(proto.structs) == 1
        assert proto.structs[0].fields[0].kind == TypeKind.PRIMITIVE

    def test_enum_parsing(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            protocol:
              name: EnumTest
            enums:
              - name: Color
                type: u8
                values:
                  - name: Red
                    value: 0
                  - name: Green
                    value: 1
                  - name: Blue
                    value: 2
            structs:
              - name: Pixel
                fields:
                  - name: color
                    type: Color
        """)
        proto = load_protocol(path)
        assert len(proto.enums) == 1
        assert proto.enums[0].name == "Color"
        assert proto.structs[0].fields[0].kind == TypeKind.ENUM

    def test_hex_values(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            protocol:
              name: HexTest
            enums:
              - name: Cmd
                type: u16
                values:
                  - name: Init
                    value: "0xFF01"
            structs:
              - name: Frame
                fields:
                  - name: magic
                    type: u32
                    expected: 0xDEADBEEF
        """)
        proto = load_protocol(path)
        assert proto.enums[0].values[0].value == 0xFF01
        assert proto.structs[0].fields[0].expected == 0xDEADBEEF

    def test_byte_order(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            protocol:
              name: BigEnd
              byte_order: big_endian
            structs:
              - name: Msg
                fields:
                  - name: val
                    type: u16
        """)
        proto = load_protocol(path)
        assert proto.reader_alias == "BEReader"
        assert proto.writer_alias == "BEWriter"


# ---------------------------------------------------------------------------
# Parser – invalid inputs
# ---------------------------------------------------------------------------

class TestParserInvalid:
    def test_empty_file(self, tmp_path):
        path = _write_yaml(tmp_path, "")
        with pytest.raises(ParseError, match="Empty"):
            load_protocol(path)

    def test_missing_protocol_key(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            structs:
              - name: Foo
                fields:
                  - name: x
                    type: u8
        """)
        with pytest.raises(ParseError, match="validation failed"):
            load_protocol(path)

    def test_unknown_type(self, tmp_path):
        path = _write_yaml(tmp_path, """\
            protocol:
              name: Bad
            structs:
              - name: S
                fields:
                  - name: x
                    type: nonexistent_type_xyz
        """)
        with pytest.raises(ValueError, match="Unknown type"):
            load_protocol(path)


# ---------------------------------------------------------------------------
# Code generation
# ---------------------------------------------------------------------------

class TestCodegen:
    def _gen(self, tmp_path, yaml_text: str) -> str:
        path = _write_yaml(tmp_path, yaml_text)
        proto = load_protocol(path)
        return generate_header(proto, source_file="test.yaml")

    def test_header_guard(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Demo
            structs:
              - name: Msg
                fields:
                  - name: x
                    type: u32
        """)
        assert "#ifndef DEMO_PROTOCOL_HPP_" in code
        assert "#define DEMO_PROTOCOL_HPP_" in code
        assert "#endif" in code

    def test_namespace(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Ns
              namespace: my_ns
            structs:
              - name: Msg
                fields:
                  - name: x
                    type: u8
        """)
        assert "namespace my_ns {" in code
        assert "}  // namespace my_ns" in code

    def test_enum_generation(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: EnumGen
            enums:
              - name: Dir
                type: u8
                values:
                  - name: Up
                    value: 0
                  - name: Down
                    value: 1
            structs:
              - name: Msg
                fields:
                  - name: dir
                    type: Dir
        """)
        assert "enum class Dir : uint8_t" in code
        assert "Up = 0x0" in code
        assert "Down = 0x1" in code
        # Parse: switch statement (no UB static_cast)
        assert "switch (_dir_raw)" in code
        assert "dir = Dir::Up; break;" in code
        assert "dir = Dir::Down; break;" in code
        assert "default: dir = Dir::Up; break;" in code

    def test_parse_primitive(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Prim
            structs:
              - name: Msg
                fields:
                  - name: val
                    type: u32
        """)
        assert "reader.read_u32(val)" in code
        assert "writer.write_u32(val)" in code

    def test_expected_value(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Magic
            structs:
              - name: Hdr
                fields:
                  - name: magic
                    type: u32
                    expected: 0xDEAD
        """)
        assert "0xDEAD" in code
        assert "OutOfRange" in code

    def test_bytes_field(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Bytes
            structs:
              - name: Msg
                fields:
                  - name: data
                    type: bytes
                    length: 16
        """)
        assert "std::array<uint8_t, 16> data" in code
        assert "read_bytes" in code

    def test_string_field(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Str
            structs:
              - name: Msg
                fields:
                  - name: name
                    type: string
                    length: 32
        """)
        assert "std::array<char, 32> name" in code

    def test_padding(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Pad
            structs:
              - name: Msg
                fields:
                  - name: reserved
                    type: padding
                    pad_size: 3
                  - name: val
                    type: u8
        """)
        assert "reader.skip(3)" in code
        assert "writer.skip(3)" in code

    def test_nested_struct(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Nested
            structs:
              - name: Inner
                fields:
                  - name: x
                    type: u8
              - name: Outer
                fields:
                  - name: inner
                    type: Inner
        """)
        assert "Inner inner{};" in code
        assert "inner.parse(reader)" in code
        assert "inner.serialize(writer)" in code

    def test_array_of_primitives(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Arr
            structs:
              - name: Msg
                fields:
                  - name: values
                    type: array
                    element_type: u16
                    length: 8
        """)
        assert "std::array<uint16_t, 8> values" in code
        assert "read_u16(elem)" in code

    def test_big_endian(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: BE
              byte_order: big_endian
            structs:
              - name: Msg
                fields:
                  - name: x
                    type: u16
        """)
        assert "bio::BEReader" in code
        assert "bio::BEWriter" in code

    def test_conditional_field(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Cond
            structs:
              - name: Msg
                fields:
                  - name: flags
                    type: u8
                  - name: extra
                    type: u32
                    condition: "flags & 0x01"
        """)
        assert "if (flags & 0x01)" in code

    def test_bitfield_u8(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Bf
            structs:
              - name: Reg
                fields:
                  - name: ctrl
                    type: bitfield_u8
                    bits:
                      - name: enable
                        offset: 0
                        width: 1
                      - name: mode
                        offset: 1
                        width: 3
                      - name: channel
                        offset: 4
                        width: 4
        """)
        # Struct members — width-based types (1-bit = bool)
        assert "bool enable{};" in code
        assert "uint8_t mode{};" in code
        assert "uint8_t channel{};" in code
        # Parse: reads one u8, extracts bits
        assert "read_u8(_ctrl_raw)" in code
        assert ">> 0) & 0x1" in code
        assert ">> 1) & 0x7" in code
        assert ">> 4) & 0xF" in code
        # Serialize: packs bits, writes one u8
        assert "write_u8(_ctrl_raw)" in code
        assert "<< 0)" in code
        assert "<< 1)" in code
        assert "<< 4)" in code

    def test_bitfield_u16(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Bf16
            structs:
              - name: Flags
                fields:
                  - name: status
                    type: bitfield_u16
                    bits:
                      - name: low
                        offset: 0
                        width: 8
                      - name: high
                        offset: 8
                        width: 8
        """)
        assert "uint8_t low{};" in code
        assert "uint8_t high{};" in code
        assert "read_u16(_status_raw)" in code
        assert "write_u16(_status_raw)" in code

    def test_bitfield_u32(self, tmp_path):
        code = self._gen(tmp_path, """\
            protocol:
              name: Bf32
            structs:
              - name: Word
                fields:
                  - name: reg
                    type: bitfield_u32
                    bits:
                      - name: addr
                        offset: 0
                        width: 16
                      - name: data
                        offset: 16
                        width: 16
        """)
        assert "uint16_t addr{};" in code
        assert "uint16_t data{};" in code
        assert "read_u32(_reg_raw)" in code
        assert "write_u32(_reg_raw)" in code

    def test_bitfield_enum(self, tmp_path):
        """A bit-slice referencing an enum should use the enum type."""
        code = self._gen(tmp_path, """\
            protocol:
              name: BfEnum
            enums:
              - name: Mode
                type: u8
                values:
                  - name: ModeOff
                    value: 0
                  - name: ModeOn
                    value: 1
                  - name: ModeAuto
                    value: 2
            structs:
              - name: Ctrl
                fields:
                  - name: flags
                    type: bitfield_u8
                    bits:
                      - name: enable
                        offset: 0
                        width: 1
                      - name: mode
                        offset: 1
                        width: 2
                        type: Mode
                      - name: channel
                        offset: 3
                        width: 4
        """)
        # Member type: enum for mode, bool for enable, uint8_t for channel
        assert "Mode mode{};" in code
        assert "bool enable{};" in code
        assert "uint8_t channel{};" in code
        # Parse: switch statement for enum (no UB static_cast)
        assert "switch (" in code
        assert "mode = Mode::ModeOff; break;" in code
        assert "mode = Mode::ModeOn; break;" in code
        assert "mode = Mode::ModeAuto; break;" in code
        assert "default: mode = Mode::ModeOff; break;" in code
        # Serialize: static_cast from enum to raw
        assert "static_cast<uint8_t>(mode)" in code


# ---------------------------------------------------------------------------
# Integration: example protocol files
# ---------------------------------------------------------------------------

class TestExampleProtocols:
    PROTOCOLS_DIR = pathlib.Path(__file__).parent.parent / "protocols"

    @pytest.mark.parametrize("filename", [
        "sensor_telemetry.yaml",
        "command_protocol.yaml",
    ])
    def test_example_generates(self, filename, tmp_path):
        path = self.PROTOCOLS_DIR / filename
        if not path.exists():
            pytest.skip(f"{path} not found")
        proto = load_protocol(path)
        code = generate_header(proto, source_file=filename)
        # Basic sanity: has include guard, includes binary-io, has parse/serialize
        assert "#ifndef" in code
        assert "binary-io.hpp" in code
        assert "parse" in code
        assert "serialize" in code

"""Type system mapping YAML type names to bio reader/writer methods and C++ types."""

from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Any, Dict, List, Optional


class TypeKind(Enum):
    """Classification of protocol field types."""

    PRIMITIVE = auto()
    ENUM = auto()
    STRUCT = auto()
    BYTES = auto()
    STRING = auto()
    ARRAY = auto()
    BITFIELD = auto()
    PADDING = auto()


@dataclass
class PrimitiveType:
    """A built-in numeric type supported by binary-io."""

    yaml_name: str
    cpp_type: str
    read_method: str
    write_method: str
    size: int  # bytes


# All primitive types supported by binary-io.hpp
PRIMITIVES: Dict[str, PrimitiveType] = {
    "u8": PrimitiveType("u8", "uint8_t", "read_u8", "write_u8", 1),
    "u16": PrimitiveType("u16", "uint16_t", "read_u16", "write_u16", 2),
    "u32": PrimitiveType("u32", "uint32_t", "read_u32", "write_u32", 4),
    "u64": PrimitiveType("u64", "uint64_t", "read_u64", "write_u64", 8),
    "i8": PrimitiveType("i8", "int8_t", "read_i8", "write_i8", 1),
    "i16": PrimitiveType("i16", "int16_t", "read_i16", "write_i16", 2),
    "i32": PrimitiveType("i32", "int32_t", "read_i32", "write_i32", 4),
    "i64": PrimitiveType("i64", "int64_t", "read_i64", "write_i64", 8),
    "f32": PrimitiveType("f32", "float", "read_f32", "write_f32", 4),
    "f64": PrimitiveType("f64", "double", "read_f64", "write_f64", 8),
}

PRIMITIVE_NAMES = set(PRIMITIVES.keys())

BYTE_ORDERS = {
    "little_endian": ("LittleEndianCodec", "LEReader", "LEWriter"),
    "big_endian": ("BigEndianCodec", "BEReader", "BEWriter"),
}


@dataclass
class EnumValue:
    """One named constant within an enum."""

    name: str
    value: int
    description: str = ""


@dataclass
class EnumDef:
    """An enumeration backed by a primitive integer type."""

    name: str
    underlying_type: str  # key into PRIMITIVES
    values: List[EnumValue] = field(default_factory=list)
    description: str = ""

    @property
    def cpp_underlying(self) -> str:
        return PRIMITIVES[self.underlying_type].cpp_type


@dataclass
class BitDef:
    """One named bit-slice within a bitfield."""

    name: str
    offset: int  # bit offset from LSB
    width: int   # number of bits
    description: str = ""
    enum_type: Optional[str] = None  # enum name if this slice maps to an enum


BITFIELD_TYPES: Dict[str, PrimitiveType] = {
    "bitfield_u8": PRIMITIVES["u8"],
    "bitfield_u16": PRIMITIVES["u16"],
    "bitfield_u32": PRIMITIVES["u32"],
}


@dataclass
class FieldDef:
    """A single field inside a struct definition."""

    name: str
    type: str  # primitive name, enum name, struct name, or special
    kind: TypeKind = TypeKind.PRIMITIVE
    description: str = ""

    # For bytes/string/array: expression giving the length
    length: Optional[str] = None

    # For arrays: element type
    element_type: Optional[str] = None

    # For expected-value checks (magic numbers, version guards)
    expected: Optional[Any] = None

    # For padding: number of bytes to skip
    pad_size: Optional[int] = None

    # For conditional fields: expression that must be true
    condition: Optional[str] = None

    # For bitfields: list of bit-slice definitions
    bits: List[BitDef] = field(default_factory=list)


@dataclass
class StructDef:
    """A binary-serializable struct definition."""

    name: str
    fields: List[FieldDef] = field(default_factory=list)
    description: str = ""


@dataclass
class ProtocolDef:
    """Top-level protocol definition parsed from one YAML file."""

    name: str
    namespace: str = ""
    byte_order: str = "little_endian"
    description: str = ""
    includes: List[str] = field(default_factory=list)
    enums: List[EnumDef] = field(default_factory=list)
    structs: List[StructDef] = field(default_factory=list)

    # Built during resolution
    enum_map: Dict[str, EnumDef] = field(default_factory=dict, repr=False)
    struct_map: Dict[str, StructDef] = field(default_factory=dict, repr=False)

    def resolve(self) -> None:
        """Build lookup maps and resolve field kinds."""
        self.enum_map = {e.name: e for e in self.enums}
        self.struct_map = {s.name: s for s in self.structs}

        for struct in self.structs:
            for f in struct.fields:
                f.kind = self._resolve_kind(f)

    def _resolve_kind(self, f: FieldDef) -> TypeKind:
        if f.type == "padding":
            return TypeKind.PADDING
        if f.type in BITFIELD_TYPES:
            return TypeKind.BITFIELD
        if f.type == "bytes":
            return TypeKind.BYTES
        if f.type == "string":
            return TypeKind.STRING
        if f.type == "array":
            return TypeKind.ARRAY
        if f.type in self.enum_map:
            return TypeKind.ENUM
        if f.type in self.struct_map:
            return TypeKind.STRUCT
        if f.type in PRIMITIVES:
            return TypeKind.PRIMITIVE
        raise ValueError(
            f"Unknown type '{f.type}' in field '{f.name}'"
        )

    @property
    def codec_name(self) -> str:
        return BYTE_ORDERS[self.byte_order][0]

    @property
    def reader_alias(self) -> str:
        return BYTE_ORDERS[self.byte_order][1]

    @property
    def writer_alias(self) -> str:
        return BYTE_ORDERS[self.byte_order][2]

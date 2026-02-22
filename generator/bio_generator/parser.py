"""Load and validate YAML protocol definitions."""

from __future__ import annotations

import pathlib
from typing import List

import jsonschema
import yaml

from .schema import PROTOCOL_SCHEMA
from .types import (
    BitDef,
    EnumDef,
    EnumValue,
    FieldDef,
    ProtocolDef,
    StructDef,
)


class ParseError(Exception):
    """Raised when a YAML file cannot be parsed or validated."""


def _parse_int(value) -> int:
    """Convert a YAML value to int, supporting hex strings like '0xFF'."""
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise ParseError(f"Cannot convert {value!r} to int")


def _build_enum(raw: dict) -> EnumDef:
    values = [
        EnumValue(
            name=v["name"],
            value=_parse_int(v["value"]),
            description=v.get("description", ""),
        )
        for v in raw["values"]
    ]
    return EnumDef(
        name=raw["name"],
        underlying_type=raw["type"],
        values=values,
        description=raw.get("description", ""),
    )


def _build_field(raw: dict) -> FieldDef:
    length = raw.get("length")
    if isinstance(length, int):
        length = str(length)

    expected = raw.get("expected")
    if isinstance(expected, str) and expected.startswith("0x"):
        expected = int(expected, 16)

    bits = [
        BitDef(
            name=b["name"],
            offset=b["offset"],
            width=b["width"],
            description=b.get("description", ""),
            enum_type=b.get("type"),
        )
        for b in raw.get("bits", [])
    ]

    return FieldDef(
        name=raw["name"],
        type=raw["type"],
        description=raw.get("description", ""),
        length=length if length is not None else None,
        element_type=raw.get("element_type"),
        expected=expected,
        pad_size=raw.get("pad_size"),
        condition=raw.get("condition"),
        bits=bits,
    )


def _build_struct(raw: dict) -> StructDef:
    fields = [_build_field(f) for f in raw["fields"]]
    return StructDef(
        name=raw["name"],
        fields=fields,
        description=raw.get("description", ""),
    )


def load_protocol(path: pathlib.Path) -> ProtocolDef:
    """Load a single YAML protocol file, validate, and return a ProtocolDef."""
    text = path.read_text(encoding="utf-8")
    raw = yaml.safe_load(text)
    if raw is None:
        raise ParseError(f"Empty YAML file: {path}")

    try:
        jsonschema.validate(instance=raw, schema=PROTOCOL_SCHEMA)
    except jsonschema.ValidationError as exc:
        raise ParseError(
            f"Schema validation failed for {path}: {exc.message}"
        ) from exc

    proto_raw = raw["protocol"]
    proto = ProtocolDef(
        name=proto_raw["name"],
        namespace=proto_raw.get("namespace", ""),
        byte_order=proto_raw.get("byte_order", "little_endian"),
        description=proto_raw.get("description", ""),
        includes=proto_raw.get("includes", []),
    )

    for e in raw.get("enums", []):
        proto.enums.append(_build_enum(e))

    for s in raw.get("structs", []):
        proto.structs.append(_build_struct(s))

    proto.resolve()
    return proto


def load_protocols_from_dir(directory: pathlib.Path) -> List[ProtocolDef]:
    """Load all .yaml / .yml files from a directory."""
    protocols: List[ProtocolDef] = []
    if not directory.is_dir():
        raise ParseError(f"Not a directory: {directory}")

    for path in sorted(directory.glob("*.yaml")):
        protocols.append(load_protocol(path))
    for path in sorted(directory.glob("*.yml")):
        # Avoid duplicates if both .yaml and .yml exist with same stem
        if not (directory / (path.stem + ".yaml")).exists():
            protocols.append(load_protocol(path))

    if not protocols:
        raise ParseError(f"No YAML files found in {directory}")
    return protocols

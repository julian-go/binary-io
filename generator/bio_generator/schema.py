"""JSON-Schema definition for validating protocol YAML files."""

PROTOCOL_SCHEMA = {
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "title": "Binary-IO Protocol Definition",
    "type": "object",
    "required": ["protocol"],
    "additionalProperties": False,
    "properties": {
        "protocol": {
            "type": "object",
            "required": ["name"],
            "additionalProperties": False,
            "properties": {
                "name": {
                    "type": "string",
                    "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                    "description": "Protocol name (used as header guard / class prefix).",
                },
                "namespace": {
                    "type": "string",
                    "pattern": "^[A-Za-z_][A-Za-z0-9_:]*$",
                    "description": "C++ namespace (may contain ::).",
                },
                "byte_order": {
                    "type": "string",
                    "enum": ["little_endian", "big_endian"],
                    "default": "little_endian",
                },
                "description": {"type": "string"},
                "includes": {
                    "type": "array",
                    "items": {"type": "string"},
                    "description": "Extra C++ #include paths.",
                },
            },
        },
        "enums": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["name", "type", "values"],
                "additionalProperties": False,
                "properties": {
                    "name": {
                        "type": "string",
                        "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                    },
                    "type": {
                        "type": "string",
                        "enum": [
                            "u8", "u16", "u32", "u64",
                            "i8", "i16", "i32", "i64",
                        ],
                    },
                    "description": {"type": "string"},
                    "values": {
                        "type": "array",
                        "minItems": 1,
                        "items": {
                            "type": "object",
                            "required": ["name", "value"],
                            "additionalProperties": False,
                            "properties": {
                                "name": {
                                    "type": "string",
                                    "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                                },
                                "value": {
                                    "oneOf": [
                                        {"type": "integer"},
                                        {"type": "string", "pattern": "^0x[0-9A-Fa-f]+$"},
                                    ]
                                },
                                "description": {"type": "string"},
                            },
                        },
                    },
                },
            },
        },
        "structs": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["name", "fields"],
                "additionalProperties": False,
                "properties": {
                    "name": {
                        "type": "string",
                        "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                    },
                    "description": {"type": "string"},
                    "fields": {
                        "type": "array",
                        "minItems": 1,
                        "items": {
                            "type": "object",
                            "required": ["name", "type"],
                            "additionalProperties": False,
                            "properties": {
                                "name": {
                                    "type": "string",
                                    "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                                },
                                "type": {"type": "string"},
                                "description": {"type": "string"},
                                "length": {
                                    "type": "integer",
                                    "minimum": 1,
                                    "description": "Fixed size for bytes/string/array (compile-time constant).",
                                },
                                "element_type": {
                                    "type": "string",
                                    "description": "Element type for arrays.",
                                },
                                "expected": {
                                    "oneOf": [
                                        {"type": "integer"},
                                        {"type": "string"},
                                    ],
                                    "description": "Expected value (magic number check).",
                                },
                                "pad_size": {
                                    "type": "integer",
                                    "minimum": 1,
                                    "description": "Number of padding bytes (type must be 'padding').",
                                },
                                "condition": {
                                    "type": "string",
                                    "description": "C++ boolean expression guarding this field.",
                                },
                                "bits": {
                                    "type": "array",
                                    "minItems": 1,
                                    "description": "Bit-slice definitions (for bitfield_u8/u16/u32).",
                                    "items": {
                                        "type": "object",
                                        "required": ["name", "offset", "width"],
                                        "additionalProperties": False,
                                        "properties": {
                                            "name": {
                                                "type": "string",
                                                "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                                            },
                                            "offset": {
                                                "type": "integer",
                                                "minimum": 0,
                                                "description": "Bit offset from LSB.",
                                            },
                                            "width": {
                                                "type": "integer",
                                                "minimum": 1,
                                                "description": "Number of bits.",
                                            },
                                            "type": {
                                                "type": "string",
                                                "pattern": "^[A-Za-z_][A-Za-z0-9_]*$",
                                                "description": "Enum type for this bit-slice.",
                                            },
                                            "description": {"type": "string"},
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
            },
        },
    },
}

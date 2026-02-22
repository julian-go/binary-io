# bio-gen — Binary Protocol Code Generator

Generate C++17 structs with `parse()` / `serialize()` methods from YAML protocol
definitions, targeting embedded systems with no dynamic allocation. The generated
code uses the [binary-io](../include/binary-io/binary-io.hpp) header for portable
byte-order-aware serialization and only fixed-size types (`std::array`).

## Installation

```bash
cd generator
pip install -e ".[dev]"
```

## Quick start

```bash
# Generate headers from a directory of YAML files
bio-gen protocols/ -o generated/ -v

# Generate from a single file
bio-gen protocols/sensor_telemetry.yaml -o include/generated/
```

## YAML schema

Each `.yaml` file describes one protocol. The top-level keys are:

### `protocol` (required)

| Key           | Type     | Default          | Description                                   |
|---------------|----------|------------------|-----------------------------------------------|
| `name`        | string   | *(required)*     | Protocol name — used for the header guard.     |
| `namespace`   | string   | `""`             | C++ namespace wrapping all generated types.    |
| `byte_order`  | string   | `little_endian`  | `little_endian` or `big_endian`.               |
| `description` | string   | `""`             | Doxygen comment on the generated header.       |
| `includes`    | string[] | `[]`             | Extra `#include` paths.                        |

### `enums` (optional)

An array of enumeration definitions.

| Key     | Type     | Description                                         |
|---------|----------|-----------------------------------------------------|
| `name`  | string   | C++ `enum class` name.                              |
| `type`  | string   | Underlying integer type (`u8`, `u16`, `u32`, `u64`, `i8`–`i64`). |
| `values`| array    | List of `{ name, value, description? }` entries.    |

Values can be decimal integers or hex strings (`"0xFF"`).

### `structs` (optional)

An array of struct definitions. Each struct has a `name` and a `fields` array.

#### Field properties

| Key            | Type          | Description                                                        |
|----------------|---------------|--------------------------------------------------------------------|
| `name`         | string        | C++ member name.                                                   |
| `type`         | string        | Type — see *Supported types* below.                                |
| `description`  | string        | Doxygen comment.                                                   |
| `expected`     | int/hex       | Magic-number check — parsing fails if the read value differs.      |
| `length`       | int           | Fixed size for `bytes`, `string`, or `array` fields.               |
| `element_type` | string        | Element type for `array` fields.                                   |
| `pad_size`     | int           | Number of bytes to skip (when `type: padding`).                    |
| `condition`    | string        | C++ boolean expression — field is only read/written when true.      |

#### Supported types

| YAML type   | C++ type                 | Notes                                             |
|-------------|--------------------------|---------------------------------------------------|
| `u8`–`u64`  | `uint8_t`–`uint64_t`     | Unsigned integers.                                |
| `i8`–`i64`  | `int8_t`–`int64_t`       | Signed integers.                                  |
| `f32`       | `float`                  | IEEE 754 single.                                  |
| `f64`       | `double`                 | IEEE 754 double.                                  |
| `bytes`     | `std::array<uint8_t, N>` | Requires `length` (fixed size).                   |
| `string`    | `std::array<char, N>`    | Requires `length` (fixed size).                   |
| `array`     | `std::array<T, N>`       | Requires `element_type` and `length` (fixed size).|
| `padding`   | *(skipped)*              | Requires `pad_size`.                              |
| *EnumName*  | `EnumName`               | References a defined enum.                        |
| *StructName*| `StructName`             | Nested struct — calls its `parse`/`serialize`.    |

## Example

```yaml
protocol:
  name: SensorTelemetry
  namespace: sensor
  byte_order: little_endian

enums:
  - name: MessageType
    type: u8
    values:
      - name: Heartbeat
        value: 0x00
      - name: Temperature
        value: 0x01

structs:
  - name: FrameHeader
    fields:
      - name: magic
        type: u32
        expected: 0xFEEDFACE
      - name: msg_type
        type: MessageType
      - name: payload_length
        type: u16

  - name: SensorFrame
    fields:
      - name: header
        type: FrameHeader
      - name: data
        type: bytes
        length: 256
```

This generates a self-contained C++17 header with:

```cpp
namespace sensor {

enum class MessageType : uint8_t {
    Heartbeat = 0x0,
    Temperature = 0x1,
};

struct FrameHeader {
    uint32_t magic{};
    MessageType msg_type{};
    uint16_t payload_length{};

    bio::Status parse(bio::LEReader& reader) { /* ... */ }
    bio::Status serialize(bio::LEWriter& writer) const { /* ... */ }
};

struct SensorFrame {
    FrameHeader header{};
    std::array<uint8_t, 256> data{};

    bio::Status parse(bio::LEReader& reader) { /* ... */ }
    bio::Status serialize(bio::LEWriter& writer) const { /* ... */ }
};

}  // namespace sensor
```

## Running tests

```bash
cd generator
pip install -e ".[dev]"
pytest -v
```

## Project structure

```
generator/
  pyproject.toml              # Package metadata & CLI entry point
  bio_generator/
    __init__.py
    __main__.py               # CLI (bio-gen command)
    types.py                  # Type system & data classes
    schema.py                 # JSON-Schema for YAML validation
    parser.py                 # YAML loading & validation
    codegen.py                # C++ code generation (Jinja2)
  protocols/                  # Example protocol definitions
    sensor_telemetry.yaml
    command_protocol.yaml
  tests/
    test_generator.py
```

"""CLI entry point for bio-gen."""

from __future__ import annotations

import argparse
import pathlib
import sys
from typing import List

from .codegen import generate_to_file
from .parser import ParseError, load_protocol, load_protocols_from_dir


def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="bio-gen",
        description=(
            "Generate C++17 binary protocol parsers/serializers from YAML "
            "definitions using binary-io.hpp."
        ),
    )
    p.add_argument(
        "input",
        type=pathlib.Path,
        help="Path to a directory of YAML protocol files, or a single .yaml file.",
    )
    p.add_argument(
        "-o",
        "--output",
        type=pathlib.Path,
        default=pathlib.Path("."),
        help="Output directory for generated headers (default: current directory).",
    )
    p.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Print progress information.",
    )
    return p


def main(argv: List[str] | None = None) -> int:
    parser = _build_parser()
    args = parser.parse_args(argv)

    input_path: pathlib.Path = args.input.resolve()
    output_dir: pathlib.Path = args.output.resolve()

    try:
        if input_path.is_file():
            protocols = [load_protocol(input_path)]
        elif input_path.is_dir():
            protocols = load_protocols_from_dir(input_path)
        else:
            print(f"Error: '{input_path}' is not a file or directory.", file=sys.stderr)
            return 1
    except ParseError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    for proto in protocols:
        source = str(input_path.name)
        out = generate_to_file(proto, output_dir, source_file=source)
        if args.verbose:
            print(f"Generated {out}")

    if args.verbose:
        print(f"Done — {len(protocols)} protocol(s) generated.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

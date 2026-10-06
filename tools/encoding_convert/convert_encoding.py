#!/usr/bin/env python3
"""Switch C/C++ source encodings between Shift-JIS (CP932) and UTF-8.

The sources in XR_FrameV2025_1125 are kept in Shift-JIS.  While working
on them, convert them to UTF-8 with BOM so that Visual Studio shows the
comments correctly.  Before delivering the sources to the client, convert
them back with the second command.

    python tools/encoding_convert/convert_encoding.py to-utf8
    python tools/encoding_convert/convert_encoding.py to-sjis

to-utf8 records the original encoding of every file in
.encoding-manifest.json (next to this script).  to-sjis uses that record
to put each file back into its original encoding, so the delivery matches
the client's tree.  Files without a record (new files) are converted to
CP932.  Files that cannot be encoded in CP932 are left as UTF-8 and
reported.

Only *.c, *.cpp, *.h and similar files are touched.  ASCII-only files are
never rewritten, so both commands are no-ops for them.
"""

from __future__ import annotations

import argparse
import codecs
import json
import os
import sys
from pathlib import Path

SOURCE_EXTS = {
    ".c", ".cc", ".cpp", ".cxx",
    ".h", ".hh", ".hpp", ".hxx",
    ".inl", ".ipp",
}
SKIP_DIRS = {".git", ".vs", "Debug", "Release", "x64", "x86", "ipch"}

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_ROOT = SCRIPT_DIR.parents[1] / "XR_FrameV2025_1125"
MANIFEST_PATH = SCRIPT_DIR / ".encoding-manifest.json"

ASCII = "ascii"
CP932 = "cp932"
UTF8 = "utf-8"
UTF8_BOM = "utf-8-bom"
UTF16_LE = "utf-16-le"
UTF16_BE = "utf-16-be"


def out(message: str) -> None:
    """Print without crashing on a console whose codepage cannot show the text."""
    try:
        print(message)
    except UnicodeEncodeError:
        enc = sys.stdout.encoding or "ascii"
        print(message.encode(enc, "backslashreplace").decode(enc, "replace"))


def detect(data: bytes) -> tuple[str | None, str | None]:
    """Return (encoding name, decoded text) for a source file.

    The name is one of ascii, cp932, utf-8, utf-8-bom, utf-16-le, utf-16-be.
    (None, None) means the file could not be decoded.
    """
    if data.startswith(codecs.BOM_UTF8):
        try:
            return UTF8_BOM, data.decode("utf-8-sig")
        except UnicodeDecodeError:
            return None, None
    for bom, name in ((codecs.BOM_UTF16_LE, UTF16_LE), (codecs.BOM_UTF16_BE, UTF16_BE)):
        if data.startswith(bom):
            try:
                return name, data[len(bom):].decode(name)
            except UnicodeDecodeError:
                return None, None
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError:
        try:
            return CP932, data.decode(CP932)
        except UnicodeDecodeError:
            return None, None
    return (ASCII if text.isascii() else UTF8), text


def collect(targets: list[Path]) -> list[Path]:
    """List source files under the targets, sorted and without duplicates."""
    found: set[Path] = set()
    for target in targets:
        if target.is_file():
            found.add(target.resolve())
        elif target.is_dir():
            for dirpath, dirnames, filenames in os.walk(target):
                dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
                for filename in filenames:
                    if Path(filename).suffix.lower() in SOURCE_EXTS:
                        found.add((Path(dirpath) / filename).resolve())
        else:
            out(f"skip   {target}: not found")
    return sorted(found)


def make_key(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def write_file(path: Path, data: bytes) -> None:
    tmp = path.with_name(path.name + ".enc_tmp")
    try:
        tmp.write_bytes(data)
        os.replace(tmp, path)
    finally:
        if tmp.exists():
            try:
                tmp.unlink()
            except OSError:
                pass


def load_manifest() -> dict:
    if MANIFEST_PATH.exists():
        try:
            manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
            manifest.setdefault("files", {})
            return manifest
        except (OSError, json.JSONDecodeError) as exc:
            out(f"ERROR  cannot read {MANIFEST_PATH}: {exc}")
            raise SystemExit(1)
    return {"version": 1, "files": {}}


def save_manifest(manifest: dict) -> None:
    payload = json.dumps(manifest, ensure_ascii=True, indent=2, sort_keys=True) + "\n"
    write_file(MANIFEST_PATH, payload.encode("utf-8"))


def cmd_to_utf8(args: argparse.Namespace, files: list[Path], root: Path, manifest: dict) -> int:
    recorded = manifest["files"]
    converted = kept = ascii_files = errors = 0
    manifest_changed = False

    for path in files:
        key = make_key(path, root)
        data = path.read_bytes()
        kind, text = detect(data)
        if kind is None:
            out(f"ERROR  {key}: unknown encoding, not changed")
            errors += 1
            continue

        if key not in recorded:
            recorded[key] = kind
            manifest_changed = True

        if kind == ASCII:
            ascii_files += 1
            if args.verbose:
                out(f"keep   {key}: ASCII")
            continue
        if kind == UTF8_BOM and not args.no_bom:
            kept += 1
            if args.verbose:
                out(f"keep   {key}: UTF-8 BOM")
            continue

        new_data = text.encode("utf-8" if args.no_bom else "utf-8-sig")
        if new_data == data:
            kept += 1
            if args.verbose:
                out(f"keep   {key}: already UTF-8")
            continue

        target = UTF8 if args.no_bom else UTF8_BOM
        if not args.dry_run:
            write_file(path, new_data)
        converted += 1
        prefix = "would convert" if args.dry_run else "convert"
        out(f"{prefix} {key}: {kind} -> {target}")

    if manifest_changed and not args.dry_run:
        save_manifest(manifest)

    out(
        f"\n{converted} converted, {kept} already UTF-8, "
        f"{ascii_files} ASCII, {errors} error(s)"
    )
    return 1 if errors else 0


def cmd_to_sjis(args: argparse.Namespace, files: list[Path], root: Path, manifest: dict) -> int:
    recorded = manifest["files"]
    converted = kept = ascii_files = errors = 0

    for path in files:
        key = make_key(path, root)
        data = path.read_bytes()
        kind, text = detect(data)
        if kind is None:
            out(f"ERROR  {key}: unknown encoding, not changed")
            errors += 1
            continue

        if kind == ASCII:
            ascii_files += 1
            if args.verbose:
                out(f"keep   {key}: ASCII")
            continue
        if kind == CP932 and not args.force:
            kept += 1
            if args.verbose:
                out(f"keep   {key}: already CP932")
            continue

        target = CP932 if args.force else (recorded.get(key) or CP932)
        try:
            if target == UTF16_LE:
                new_data = codecs.BOM_UTF16_LE + text.encode("utf-16-le")
            elif target == UTF16_BE:
                new_data = codecs.BOM_UTF16_BE + text.encode("utf-16-be")
            elif target == UTF8_BOM:
                new_data = text.encode("utf-8-sig")
            elif target == UTF8:
                new_data = text.encode("utf-8")
            else:
                target = CP932
                new_data = text.encode(CP932)
        except UnicodeEncodeError as exc:
            line = text.count("\n", 0, exc.start) + 1
            chars = ascii(text[exc.start:exc.end])
            out(f"ERROR  {key}: cannot encode {chars} (line {line}), left as UTF-8")
            errors += 1
            continue

        if new_data == data:
            kept += 1
            if args.verbose:
                out(f"keep   {key}: already {target}")
            continue

        if not args.dry_run:
            write_file(path, new_data)
        converted += 1
        prefix = "would convert" if args.dry_run else "convert"
        out(f"{prefix} {key}: {kind} -> {target}")

    out(
        f"\n{converted} converted, {kept} already in the target encoding, "
        f"{ascii_files} ASCII, {errors} error(s)"
    )
    return 1 if errors else 0


def add_common_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "paths", nargs="*", metavar="PATH",
        help="files or directories to process (default: the XR_FrameV2025_1125 tree)",
    )
    parser.add_argument(
        "--root", metavar="DIR",
        help="base directory for the manifest keys (default: the default target)",
    )
    parser.add_argument("--dry-run", action="store_true", help="report changes without writing")
    parser.add_argument("--verbose", action="store_true", help="also report unchanged files")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    commands = parser.add_subparsers(dest="command", required=True)

    to_utf8 = commands.add_parser(
        "to-utf8", help="Shift-JIS / UTF-16 -> UTF-8 with BOM"
    )
    add_common_arguments(to_utf8)
    to_utf8.add_argument(
        "--no-bom", action="store_true",
        help="write UTF-8 without BOM (Visual Studio then needs /source-charset:utf-8)",
    )

    to_sjis = commands.add_parser(
        "to-sjis", aliases=["to-shift-jis"],
        help="UTF-8 -> original encoding, normally CP932",
    )
    add_common_arguments(to_sjis)
    to_sjis.add_argument(
        "--force", action="store_true",
        help="convert every UTF-8 file to CP932, ignoring the manifest",
    )

    args = parser.parse_args(argv)
    root = Path(args.root).resolve() if args.root else DEFAULT_ROOT
    if not args.paths and not root.is_dir():
        out(f"ERROR  default target not found: {root}")
        return 1
    targets = [Path(p) for p in args.paths] or [root]

    files = collect(targets)
    if args.command == "to-utf8":
        return cmd_to_utf8(args, files, root, load_manifest())
    return cmd_to_sjis(args, files, root, load_manifest())


if __name__ == "__main__":
    sys.exit(main())

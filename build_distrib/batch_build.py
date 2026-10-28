#!/usr/bin/env python3
"""
batch_build.py – Batch build ESP-IDF variants with proper EIM activation (Windows)
"""

import argparse
import json
import os
import platform
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

def find_eim_profile() -> Path | None:
    candidates = [
        Path(r"C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1"),
        Path(r"C:\Espressif\tools\Microsoft.PowerShell_profile.ps1"),
        Path.home() / "Espressif" / "tools" / "Microsoft.v6.1.PowerShell_profile.ps1",
    ]
    for p in candidates:
        if p.exists():
            return p
    return None

def run_command(cmd: list[str], project_dir: Path, eim_profile: Path | None, dry_run: bool) -> bool:
    """Run one idf.py command, optionally inside EIM-activated PowerShell."""

    if eim_profile and platform.system() == "Windows":
        # Build a single PowerShell script block
        # This is the most reliable way
        ps_script = f"""
        $ErrorActionPreference = 'Stop'
        . '{eim_profile}'
        Set-Location -LiteralPath '{project_dir}'
        Write-Host "=== PATH (first 300 chars) ==="
        Write-Host ($env:PATH.Substring(0, [Math]::Min(300, $env:PATH.Length)))
        Write-Host "=== Running: {' '.join(cmd)} ==="
        & {' '.join(f'"{c}"' if ' ' in c else c for c in cmd)}
        if ($LASTEXITCODE -ne 0) {{ exit $LASTEXITCODE }}
        """

        full_cmd = [
            "powershell.exe",
            "-NoProfile",
            "-ExecutionPolicy", "Bypass",
            "-Command",
            ps_script
        ]
        print(f"  → (EIM) {' '.join(cmd)}")
    else:
        full_cmd = cmd
        print(f"  → {' '.join(cmd)}")

    if dry_run:
        return True

    # IMPORTANT: do NOT capture_output so you can see real errors
    result = subprocess.run(full_cmd, cwd=None if eim_profile else project_dir)
    return result.returncode == 0

def load_presets(presets_file: Path):
    with open(presets_file, encoding="utf-8") as f:
        return json.load(f).get("configurePresets", [])

def build_one(name: str, project_dir: Path, extra: list[str],
              do_clean: bool, eim_profile: Path | None, dry_run: bool):
    print(f"\n>>> Preset: {name}")

    if do_clean:
        print("  Cleaning...")
        if not run_command(["idf.py", "--preset", name, "fullclean"],
                           project_dir, eim_profile, dry_run):
            return name, False, "clean failed"

    print("  Building...")
    ok = run_command(["idf.py", "--preset", name, "build"] + extra,
                     project_dir, eim_profile, dry_run)
    return name, ok, "OK" if ok else "FAILED"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--project", default=".")
    parser.add_argument("--presets", default="CMakePresets.json")
    parser.add_argument("--only", nargs="+")
    parser.add_argument("--exclude", nargs="+")
    parser.add_argument("-c", "--clean", action="store_true")
    parser.add_argument("-j", "--jobs", type=int, default=1)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--eim-profile")
    parser.add_argument("--no-eim", action="store_true")
    parser.add_argument("extra", nargs=argparse.REMAINDER)
    args = parser.parse_args()

    project_dir = Path(args.project).resolve()
    presets_file = project_dir / args.presets

    if not presets_file.exists():
        print(f"Error: {presets_file} not found")
        sys.exit(1)

    eim_profile = None
    if not args.no_eim and platform.system() == "Windows":
        eim_profile = Path(args.eim_profile) if args.eim_profile else find_eim_profile()
        if eim_profile and eim_profile.exists():
            print(f"Using EIM profile: {eim_profile}")
        else:
            print("No EIM profile found → using current environment")
            eim_profile = None

    if eim_profile is None and "IDF_PATH" not in os.environ:
        print("Error: IDF_PATH not set and no EIM profile available.")
        sys.exit(1)

    presets = load_presets(presets_file)
    names = [p["name"] for p in presets]
    if args.only:
        names = [n for n in names if n in args.only]
    if args.exclude:
        names = [n for n in names if n not in args.exclude]

    print(f"\nWill process: {names}")
    print(f"Clean first : {args.clean}")

    results = []
    for name in names:                      # sequential is safer while debugging
        results.append(build_one(name, project_dir, args.extra,
                                 args.clean, eim_profile, args.dry_run))

    print("\n" + "="*50)
    failed = 0
    for name, ok, msg in results:
        print(f"  {'✓' if ok else '✗'}  {name}: {msg}")
        if not ok:
            failed += 1

    if failed:
        print(f"\n{failed} failed.")
        sys.exit(1)
    print("\nAll done.")

if __name__ == "__main__":
    main()
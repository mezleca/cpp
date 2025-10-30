#!/usr/bin/env python3
"""
    build script for my c++ projects
    if you want a different gcc version check https://github.com/xpack-dev-tools/gcc-xpack/releases
"""

import os
import sys
import shutil
import subprocess
import platform
import argparse
import tarfile
import requests

from pathlib import Path

# gcc pack config
GCC_PACK = "xpack-gcc"
GCC_VERSION = "12.5.0-1"
GCC_ARCH = "linux-x64"

# paths
CWD = Path.cwd()
BUILD_DIR = CWD / "build"
EXTERNAL_DIR = CWD / "external"
SOURCE_DIR = CWD
BINARY_NAME = "app"

# even more paths
GCC_FOLDER = f"{GCC_PACK}-{GCC_VERSION}"
GCC_URL = f"https://github.com/xpack-dev-tools/gcc-xpack/releases/download/v{GCC_VERSION}/{GCC_PACK}-{GCC_VERSION}-{GCC_ARCH}.tar.gz"
GCC_BIN_DIR = EXTERNAL_DIR / GCC_FOLDER / "bin"
GCC_PATH = GCC_BIN_DIR / "gcc"
GPP_PATH = GCC_BIN_DIR / "g++"

SYSTEM = platform.system()

def run(cmd: str, check: bool = True, env: dict[str, str] | None = None) -> int:
    print(f"exec: {cmd}")
    
    result = subprocess.run(
        cmd, 
        shell=True,
        env=env,
        # windows needs this to handle paths correctly
        cwd=str(CWD) if SYSTEM == "Windows" else None
    )
    
    if check and result.returncode != 0:
        print(f"command failed with exit code {result.returncode}")
        sys.exit(result.returncode)
    
    return result.returncode

def download_with_progress(url: str, destination: Path) -> bool:
    try:
        print(f"downloading from {url}...")
        response = requests.get(url, stream=True, timeout=30)
        response.raise_for_status()
        
        total_size = int(response.headers.get('content-length', 0))
        downloaded = 0
        
        with open(destination, 'wb') as f:
            for chunk in response.iter_content(chunk_size=8192):
                if chunk:
                    _ = f.write(chunk)
                    downloaded += len(chunk)
                    
                    if total_size > 0:
                        progress = (downloaded / total_size) * 100
                        print(f"\rprogress: {progress:.1f}%", end='', flush=True)
        
        print()
        return True
        
    except requests.exceptions.RequestException as e:
        print(f"\nerror downloading: {e}")
        return False
    except IOError as e:
        print(f"\nerror saving file: {e}")
        return False

# I LOVE WINDOWS MAN I REALLY LOVE WINDOWS
def find_vcvarsall():
    base_paths = [
        r"C:\Program Files\Microsoft Visual Studio",
        r"C:\Program Files (x86)\Microsoft Visual Studio",
    ]
    
    versions = ["2022", "2019", "2017"]
    editions = ["Community", "BuildTools"]
    
    for base_path in base_paths:
        if not os.path.exists(base_path):
            continue
            
        for version in versions:
            for edition in editions:
                vcvarsall = Path(base_path) / version / edition / "VC" / "Auxiliary" / "Build" / "vcvarsall.bat"
                
                if vcvarsall.exists():
                    # print(f"found vcvarsall.bat at: {vcvarsall}")
                    return str(vcvarsall)
    
    return None

def get_msvc_env() -> dict[str, str]:
    vcvarsall_path = find_vcvarsall()

    if vcvarsall_path == None:
        print("failed to find vcvarsall.bat...\nmake sure you have build tools installed (2017, 2019 or 2022)")
        sys.exit(1)

    # run ts and attempt to capture env
    cmd = f'"{vcvarsall_path}" x64 && set'
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    
    # parse vars
    env: dict[str, str] = {}
    for line in result.stdout.split('\n'):
        if '=' in line:
            key, _, value = line.partition('=')
            env[key] = value
    
    return env

def check_tool_exists(tool: str) -> bool:
    return shutil.which(tool) is not None

def init() -> int:
    print("initializing submodules...")

    if run("git submodule update --init --recursive", check=False) != 0:
        print("failed to initialize submodules")
        return 1
    
    # check if cmake and ninja exist
    if not check_tool_exists("cmake"):
        print("error: cmake not found in PATH")
        return 1
    
    if not check_tool_exists("ninja"):
        print("error: ninja not found in PATH")
        return 1
    
    # only download gcc on linux
    if SYSTEM != "Linux":
        return 0
    
    # check if gcc already exists
    if GCC_PATH.exists() and GPP_PATH.exists():
        return 0
    
    EXTERNAL_DIR.mkdir(exist_ok=True)
    tar_path = EXTERNAL_DIR / "gcc.tar.gz"
    
    # download gcc
    if not download_with_progress(GCC_URL, tar_path):
        return 1
    
    # extract
    print("extracting gcc...")
    try:
        with tarfile.open(tar_path, 'r:gz') as tar:
            # validate members to prevent path traversal
            for member in tar.getmembers():
                member_path = (EXTERNAL_DIR / member.name).resolve()
                if not str(member_path).startswith(str(EXTERNAL_DIR.resolve())):
                    raise ValueError(f"attempted path traversal: {member.name}")
            tar.extractall(path=EXTERNAL_DIR)
    except Exception as e:
        print(f"failed to extract gcc: {e}")
        return 1
    
    # cleanup tar file
    tar_path.unlink()
    print("gcc installed successfully")
    
    return 0

def clean() -> int:
    if not BUILD_DIR.exists():
        print("build directory doesnt exist")
        return 0
    
    print("removing build directory...")
    try:
        shutil.rmtree(BUILD_DIR)
        print("build directory removed")
        return 0
    except PermissionError:
        print("permission denied - try running as admin/sudo")
        return 1

def get_cpu_count() -> int:
    if SYSTEM == "Linux":
        result = subprocess.run("nproc", shell=True, capture_output=True, text=True)
        cores = result.stdout.strip()
        return int(cores) if cores.isdigit() else os.cpu_count() or 4
    
    return os.cpu_count() or 4

def configure_linux(debug: bool) -> int:
    cache = BUILD_DIR / "CMakeCache.txt"
    
    if cache.exists():
        cache.unlink()
    
    BUILD_DIR.mkdir(exist_ok=True)
    
    # check if gcc exists
    if not GPP_PATH.exists():
        print(f"error: g++ not found at {GPP_PATH}")
        print("run './build.py init' first")
        return 1
    
    build_type = "Debug" if debug else "Release"
    cmd = (
        f"cmake -G Ninja "
        f"-B {BUILD_DIR} "
        f"-S {SOURCE_DIR} "
        f"-DCMAKE_C_COMPILER={GCC_PATH} "
        f"-DCMAKE_CXX_COMPILER={GPP_PATH} "
        f"-DCMAKE_BUILD_TYPE={build_type} "
        f"-DOUTPUT_NAME={BINARY_NAME}"
    )
    
    return run(cmd)

def configure_windows(debug: bool) -> int:
    cache = BUILD_DIR / "CMakeCache.txt"
    
    if cache.exists():
        cache.unlink()

    BUILD_DIR.mkdir(exist_ok=True)

    # 99% of the time i attempt to use mingw i get weird ass errors (mostly due to old gcc version or smtg) 
    vcvarsall_env = get_msvc_env()
    build_type = "Debug" if debug else "Release"

    cmd = (
        f"cmake -G Ninja "
        f"-B {BUILD_DIR} "
        f"-S {SOURCE_DIR} "
        f"-DCMAKE_BUILD_TYPE={build_type} "
        f"-DOUTPUT_NAME={BINARY_NAME}"
    )

    return run(cmd, env=vcvarsall_env)

def configure(debug: bool) -> int:
    if SYSTEM == "Linux":
        return configure_linux(debug)
    elif SYSTEM == "Windows":
        return configure_windows(debug)
    else:
        print(f"unsupported platform: {SYSTEM}")
        return 1

def build(debug: bool) -> int:
    if not BUILD_DIR.exists():
        print("build directory doesnt exist, configuring...")
        if configure(debug) != 0:
            return 1
    
    cores = get_cpu_count()
    env = get_msvc_env() if SYSTEM == "Windows" else None

    return run(f"ninja -C {BUILD_DIR} -j{cores}", env=env)

def run_binary() -> int:
    if SYSTEM == "Linux":
        binary = BUILD_DIR / BINARY_NAME
    elif SYSTEM == "Windows":
        candidates = [
            BUILD_DIR / f"{BINARY_NAME}.exe",
            BUILD_DIR / "Release" / f"{BINARY_NAME}.exe",
            BUILD_DIR / "Debug" / f"{BINARY_NAME}.exe",
        ]
        binary = next((path for path in candidates if path.exists()), None)
        if binary is None:
            print("binary not found in any expected location")
            print("build the project first")
            return 1
    else:
        print(f"unsupported platform: {SYSTEM}")
        return 1
    
    if not binary.exists():
        print(f"binary not found at {binary}")
        print("build the project first")
        return 1

    return run(str(binary))

def main():
    parser = argparse.ArgumentParser(
        description="build script for c++ projects",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    _ = parser.add_argument(
        "command",
        choices=["init", "configure", "build", "clean", "run"],
        help="command to execute"
    )
    
    _ = parser.add_argument(
        "--debug",
        action="store_true",
        help="use debug build type"
    )
    
    _ = parser.add_argument(
        "--run",
        action="store_true",
        help="run binary after building"
    )
    
    args = parser.parse_args()
    
    commands = {
        "init": init,
        "configure": lambda: configure(args.debug),
        "build": lambda: build(args.debug),
        "clean": clean,
        "run": run_binary
    }
    
    exit_code = commands[args.command]()
    
    # run binary after build if requested
    if args.command == "build" and exit_code == 0 and args.run:
        exit_code = run_binary()
    
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
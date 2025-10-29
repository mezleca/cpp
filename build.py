#!/usr/bin/env python3
"""
    simple python script to build my c++ projects
    it has all the lazy commands for build, clean, etc

    it uses docker(gcc) to build on linux
    and msvc for windows

    - compilers (msvc windows, gcc12 linux)
    - cmake 
    - ninja  
"""

import os
import sys
import shutil
import subprocess
import platform
import argparse

BUILD_DIR = "./build/"
SOURCE_DIR = "./"
LINUX_DOCKER_IMAGE = "linux-builder"
BINARY_NAME = "app"

def run(cmd: str, check=True):
    print(f"exec: {cmd}")
    result = subprocess.run(cmd, shell=True)
    if check and result.returncode != 0:
        print(f"command failed with exit code {result.returncode}")
        sys.exit(result.returncode)
    return result.returncode

def init_repos():
    return run("git submodule update --init --recursive")

def clean_build_dir():
    if not os.path.exists(BUILD_DIR):
        return 0

    try:
        print("removing build directory...")
        shutil.rmtree(BUILD_DIR)
        print("build directory removed")
        return 0
    except PermissionError:
        if platform.system() == "Linux":
            print("permission denied, attempting with sudo...")
            return run(f"sudo rm -rf {BUILD_DIR}")
        else:
            print("cant remove build directory, permission denied")
            return 1

def build_docker_image():
    if platform.system() != "Linux":
        return 1

    print("building docker image...")
    return run(f"docker build -t {LINUX_DOCKER_IMAGE} .")

def check_docker_image():
    result = subprocess.run(
        f"docker image inspect {LINUX_DOCKER_IMAGE}",
        shell=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if result.returncode != 0:
        return build_docker_image()
    return 0

def configure_linux(debug: bool):
    ret = check_docker_image()
    if ret != 0:
        return ret

    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")
    
    if os.path.exists(cache):
        os.remove(cache)

    os.makedirs(BUILD_DIR, exist_ok=True)

    uid = os.getuid()
    gid = os.getgid()
    current_dir = os.path.abspath(".")

    build_type = "Debug" if debug else "Release"
    
    docker_cmd = (
        f"docker run --rm "
        f"--user {uid}:{gid} "
        f'-v "{current_dir}:{current_dir}" '
        f'-w "{current_dir}" '
        f"{LINUX_DOCKER_IMAGE} bash -c "
        f'"cmake -G Ninja -B {BUILD_DIR} -S {SOURCE_DIR} '
        f'-DCMAKE_BUILD_TYPE={build_type} -DOUTPUT_NAME={BINARY_NAME}"'
    )
    return run(docker_cmd)

def get_cpu_count():
    system = platform.system()
    
    if system == "Linux":
        cores_cmd = f"docker run --rm {LINUX_DOCKER_IMAGE} bash -c 'nproc'"
        result = subprocess.run(cores_cmd, shell=True, capture_output=True, text=True)
        cores = result.stdout.strip()
        return int(cores) if cores.isdigit() else os.cpu_count() or 4
    else:
        return os.cpu_count() or 4


def build_linux(debug: bool):
    ret = check_docker_image()
    if ret != 0:
        return ret

    if not os.path.exists(BUILD_DIR):
        configure_linux(debug)

    uid = os.getuid()
    gid = os.getgid()
    current_dir = os.path.abspath(".")
    cores = get_cpu_count()

    print(f"building with {cores} cores...")

    docker_cmd = (
        f"docker run --rm "
        f"--user {uid}:{gid} "
        f'-v "{current_dir}:{current_dir}" '
        f'-w "{current_dir}" '
        f"{LINUX_DOCKER_IMAGE} bash -c "
        f'"ninja -C {BUILD_DIR} -j{cores}"'
    )
    
    print(f"exec: {docker_cmd}")
    return run(docker_cmd)

def configure_windows(debug: bool):
    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")

    if os.path.exists(cache):
        os.remove(cache)

    os.makedirs(BUILD_DIR, exist_ok=True)

    build_type = "Debug" if debug else "Release"

    return run(
        f'cmake -G Ninja '
        f'-B {BUILD_DIR} -S {SOURCE_DIR} -A x64 '
        f'-DCMAKE_BUILD_TYPE={build_type} -DOUTPUT_NAME={BINARY_NAME}'
    )

def build_windows(debug: bool):
    if not os.path.exists(BUILD_DIR):
        configure_windows(debug)

    cores = get_cpu_count()
    print(f"building with {cores} cores...")
    return run(f'ninja -C {BUILD_DIR} -j{cores}')

def configure_project(debug: bool):
    system = platform.system()

    if system == "Linux":
        return configure_linux(debug)
    elif system == "Windows":
        return configure_windows(debug)
    else:
        print(f"unsupported platform: {system}")
        return 1

def build_project(debug: bool):
    system = platform.system()

    if system == "Linux":
        return build_linux(debug)
    elif system == "Windows":
        return build_windows(debug)
    else:
        print(f"unsupported platform: {system}")
        return 1

def run_binary():
    system = platform.system()
    
    if system == "Linux":
        binary = os.path.join(BUILD_DIR, BINARY_NAME)
        if not os.path.exists(binary):
            print(f"binary not found at {binary}")
            return 1
        print(f"running {binary}...")
        return run(binary)
    elif system == "Windows":
        binary = os.path.join(BUILD_DIR, "Release", f"{BINARY_NAME}.exe")
        if not os.path.exists(binary):
            print(f"binary not found at {binary}")
            return 1
        print(f"running {binary}...")
        return run(binary)
    else:
        print(f"unsupported platform: {system}")
        return 1

def main():
    parser = argparse.ArgumentParser(
        description="build script for c++ projects"
    )
    
    parser.add_argument(
        "command",
        choices=["build", "configure", "clean", "run", "init", "docker-build"],
        help="command to execute"
    )
    
    parser.add_argument(
        "--debug",
        action="store_true",
        help="build in debug mode"
    )
    
    parser.add_argument(
        "--run",
        action="store_true",
        help="run the binary after building"
    )
    
    args = parser.parse_args()
    
    exit_code = 0
    
    if args.command == "build":
        exit_code = build_project(args.debug)
        if exit_code == 0 and args.run:
            exit_code = run_binary()
            
    elif args.command == "configure":
        exit_code = configure_project(args.debug)
        
    elif args.command == "clean":
        exit_code = clean_build_dir()
        
    elif args.command == "run":
        exit_code = run_binary()
        
    elif args.command == "docker-build":
        exit_code = build_docker_image()
        
    elif args.command == "init":
        exit_code = init_repos()
    
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
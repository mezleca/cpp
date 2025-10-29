#!/usr/bin/env python3
"""
    simple python script to build my c++ projects
    it has all the lazy commands for build, clean, etc

    it uses docker to build on linux (uses the DockerFile on root dir)
    and msvc for windows
"""

import os
import sys
import shutil
import subprocess
import platform

BUILD_DIR = "./build/"
SOURCE_DIR = "./"
LINUX_DOCKER_IMAGE = "linux-builder"
BINARY_NAME = "app"


def run(cmd: str, check=True):
    result = subprocess.run(cmd, shell=True)
    if check and result.returncode != 0:
        print(f"command failed with exit code {result.returncode}")
        sys.exit(result.returncode)
    return result.returncode


def show_help():
    print(f"Usage: {sys.argv[0]} [OPTION]")
    print("")
    print("Options:")
    print("  --build         Build the project")
    print("  --build-run     Build and run the binary")
    print("  --configure     Configure the project")
    print("  --clear         Clear build directory")
    print("  --run           Run the binary")
    print("  --init          Initialize and fetch repositories recursively")
    print("  --docker-build  Build Docker image (Linux only)")
    print("  -h, --help      Show this help message")


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


def configure_linux():
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

    docker_cmd = (
        f"docker run --rm "
        f"--user {uid}:{gid} "
        f'-v "{current_dir}:{current_dir}" '
        f'-w "{current_dir}" '
        f"{LINUX_DOCKER_IMAGE} bash -c "
        f'"cmake -B {BUILD_DIR} -S {SOURCE_DIR} -DOUTPUT_NAME={BINARY_NAME}"'
    )
    return run(docker_cmd)


def build_linux():
    ret = check_docker_image()

    if ret != 0:
        return ret

    if not os.path.exists(BUILD_DIR):
        configure_linux()

    uid = os.getuid()
    gid = os.getgid()
    current_dir = os.path.abspath(".")

    docker_cmd = (
        f"docker run --rm "
        f"--user {uid}:{gid} "
        f'-v "{current_dir}:{current_dir}" '
        f'-w "{current_dir}" '
        f"{LINUX_DOCKER_IMAGE} bash -c "
        f'"cd {BUILD_DIR} && make -j$(nproc)"'
    )
    return run(docker_cmd)


def configure_windows():
    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")

    if os.path.exists(cache):
        os.remove(cache)

    os.makedirs(BUILD_DIR, exist_ok=True)

    return run(
        f'cmake -G "Visual Studio 17 2022" -A x64 '
        f'-B {BUILD_DIR} -S {SOURCE_DIR} -DOUTPUT_NAME={BINARY_NAME}'
    )


def build_windows():
    if not os.path.exists(BUILD_DIR):
        configure_windows()

    return run(f"cmake --build {BUILD_DIR} --config Release -j")


def configure_project():
    system = platform.system()

    if system == "Linux":
        return configure_linux()
    elif system == "Windows":
        return configure_windows()
    else:
        print(f"unsupported platform: {system}")
        return 1


def build_project():
    system = platform.system()

    if system == "Linux":
        return build_linux()
    elif system == "Windows":
        return build_windows()
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


if len(sys.argv) == 1:
    show_help()
    sys.exit(0)

opt = sys.argv[1]
exit_code = 0

if opt == "--build":
    exit_code = build_project()
elif opt == "--build-run":
    exit_code = build_project()
    if exit_code == 0:
        exit_code = run_binary()
elif opt == "--configure":
    exit_code = configure_project()
elif opt == "--clear":
    exit_code = clean_build_dir()
    if exit_code == 0:
        exit_code = build_project()
elif opt == "--run":
    exit_code = run_binary()
elif opt == "--docker-build":
    exit_code = build_docker_image()
elif opt == "--init":
    exit_code = init_repos()
elif opt in ("-h", "--help"):
    show_help()
else:
    print(f"unknown option: {opt}")
    show_help()
    exit_code = 1

sys.exit(exit_code)
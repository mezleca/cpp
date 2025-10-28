#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
import platform

BUILD_DIR = "./build/"
SOURCE_DIR = "./"
LINUX_DOCKER_IMAGE = "linux-builder"


def run(cmd: str):
    return subprocess.run(cmd, shell=True, check=True)


def show_help():
    print(f"Usage: {sys.argv[0]} [OPTION]")
    print("")
    print("Options:")
    print("  --build         Build the project")
    print("  --configure     Configure the project")
    print("  --clean-build   Remove build directory and build")
    print("  --init          Initialize and fetch repositories recursively")
    print("  --docker-build  Build Docker image (Linux only)")
    print("  -h, --help      Show this help message")


def init_repos():
    _ = run("git submodule update --init --recursive")


def clean_build_dir():
    if os.path.exists(BUILD_DIR):
        try:
            shutil.rmtree(BUILD_DIR)
        except PermissionError:
            if platform.system() == "Linux":
                print("attempting to remove with sudo...")
                _ = run(f"sudo rm -rf {BUILD_DIR}")
            else:
                print("cant remove build directory...\npermission denied")
                sys.exit(1)


def build_docker_image():
    if platform.system() != "Linux":
        print("docker builds are only supported on linux")
        sys.exit(1)

    print("building docker image...")
    _ = run(f"docker build -t {LINUX_DOCKER_IMAGE} .")


def check_docker_image():
    result = subprocess.run(
        f"docker image inspect {LINUX_DOCKER_IMAGE}",
        shell=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if result.returncode != 0:
        build_docker_image()

def configure_linux():
    check_docker_image()

    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")
    if os.path.exists(cache):
        os.remove(cache)

    os.makedirs(BUILD_DIR, exist_ok=True)

    docker_cmd = (
        f'docker run --rm -v "{os.getcwd()}:/workspace" {LINUX_DOCKER_IMAGE} bash -c '
        f'"cmake -B {BUILD_DIR} -S {SOURCE_DIR}"'
    )
    _ = run(docker_cmd)

def build_linux():
    check_docker_image()
    configure_linux()

    docker_cmd = (
        f'docker run --rm -v "{os.getcwd()}:/workspace" {LINUX_DOCKER_IMAGE} bash -c '
        f'"cd {BUILD_DIR} && make -j$(nproc)"'
    )
    _ = run(docker_cmd)

def configure_windows():
    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")
    if os.path.exists(cache):
        os.remove(cache)

    os.makedirs(BUILD_DIR, exist_ok=True)

    _ = run(f'cmake -G "Visual Studio 17 2022" -A x64 -B {BUILD_DIR} -S {SOURCE_DIR}')

def build_windows():
    configure_windows()
    _ = run(f"cmake --build {BUILD_DIR} --config Release -j")

def configure_project():
    system = platform.system()

    # always clear dir
    clean_build_dir()

    if system == "Linux":
        configure_linux()
    elif system == "Windows":
        configure_windows()
    else:
        sys.exit(1)

def build_project():
    system = platform.system()

    if system == "Linux":
        build_linux()
    elif system == "Windows":
        build_windows()
    else:
        sys.exit(1)


if len(sys.argv) == 1:
    show_help()
    sys.exit(0)

opt = sys.argv[1]

if opt == "--build":
    build_project()
elif opt == "--configure":
    configure_project()
elif opt == "--clean-build":
    clean_build_dir()
    build_project()
elif opt == "--docker-build":
    build_docker_image()
elif opt == "--init":
    init_repos()
elif opt in ("-h", "--help"):
    show_help()
else:
    print(f"unknown option: {opt}")
    show_help()
    sys.exit(1)

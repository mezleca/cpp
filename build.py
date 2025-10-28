#!/usr/bin/env python3

import os
import sys
import shutil
import subprocess
import platform

BUILD_DIR = "./build/"
SOURCE_DIR = "./"
DOCKER_IMAGE = "gcc10-builder"

def run(cmd):
    return subprocess.run(cmd, shell=True, check=True)

def show_help():
    print(f"Usage: {sys.argv[0]} [OPTION]")
    print("")
    print("Options:")
    print("  --build         Build the project")
    print("  --clean-build   Remove build directory and build")
    print("  --init          Initialize and fetch repositories recursively")
    print("  --docker-build  Build Docker image")
    print("  -h, --help      Show this help message")

def init_repos():
    run("git submodule update --init --recursive")

def clean_build_dir():
    if os.path.exists(BUILD_DIR):
        try:
            shutil.rmtree(BUILD_DIR)
        except PermissionError:
            if platform.system() == "Linux":
                print("attempting to remove with sudo...")
                run(f"sudo rm -rf {BUILD_DIR}")
            else:
                print("cant remove build directory...\npermission denied")
                sys.exit(1)

def build_docker_image():
    print("building docker image...")
    run(f"docker build -t {DOCKER_IMAGE} .")

def check_docker_image():
    result = subprocess.run(
        f"docker image inspect {DOCKER_IMAGE}",
        shell=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if result.returncode != 0:
        build_docker_image()

def build_project():
    check_docker_image()
    cache = os.path.join(BUILD_DIR, "CMakeCache.txt")
    if os.path.exists(cache):
        os.remove(cache)
    os.makedirs(BUILD_DIR, exist_ok=True)
    docker_cmd = (
        f'docker run --rm -v "{os.getcwd()}:/workspace" {DOCKER_IMAGE} bash -c '
        f'"cmake -B {BUILD_DIR} -S {SOURCE_DIR} && cd {BUILD_DIR} && make -j4"'
    )
    run(docker_cmd)

if len(sys.argv) == 1:
    show_help()
    sys.exit(0)

opt = sys.argv[1]
if opt == "--build":
    build_project()
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
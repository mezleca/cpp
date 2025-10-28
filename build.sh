#!/bin/bash
BUILD_DIR="./build/"
SOURCE_DIR="./"

# WHO WOULD TOUGHT THAT USING DOCKER WOULD BE WAY EASIER THAN COMPILLING GCC 10 ON ARCH FOR 3 WHOLE HOURS...
DOCKER_IMAGE="gcc10-builder"

show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  --build         Build the project"
    echo "  --clean-build   Remove build directory and build"
    echo "  --init          Initialize and fetch repositories recursively"
    echo "  --docker-build  Build Docker image"
    echo "  -h, --help      Show this help message"
}

init_repos() {
    echo "Initializing repositories..."
    git submodule update --init --recursive
    if [ $? -eq 0 ]; then
        echo "initialized"
    else
        exit 1
    fi
}

clean_build_dir() {
    if [ -e "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}

build_docker_image() {
    echo "Building Docker image..."
    docker build -t "$DOCKER_IMAGE" .
    if [ $? -ne 0 ]; then
        echo "Failed to build Docker image"
        exit 1
    fi
}

check_docker_image() {
    if ! docker image inspect "$DOCKER_IMAGE" > /dev/null 2>&1; then
        echo "Docker image not found, building..."
        build_docker_image
    fi
}

build_project() {
    check_docker_image
    
    if [ -e "$BUILD_DIR/CMakeCache.txt" ]; then
        rm -f "$BUILD_DIR/CMakeCache.txt"
    fi
    
    if [ ! -e "$BUILD_DIR" ]; then
        mkdir "$BUILD_DIR"
    fi
    
    docker run --rm -v "$(pwd):/workspace" "$DOCKER_IMAGE" bash -c "
        cmake -B $BUILD_DIR -S $SOURCE_DIR
        if [ \$? -ne 0 ]; then
            exit 1
        fi
        cd $BUILD_DIR
        make -j 4
        if [ \$? -ne 0 ]; then
            exit 1
        fi
    "
    
    if [ $? -ne 0 ]; then
        exit 1
    fi
}

if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

case "$1" in
    --build)
        build_project
        ;;
    --clean-build)
        clean_build_dir
        build_project
        ;;
    --init)
        init_repos
        ;;
    -h|--help)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        show_help
        exit 1
        ;;
esac
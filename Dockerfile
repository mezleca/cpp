FROM debian:trixie

RUN apt-get update && apt-get install -y \
    build-essential \
    ninja-build \
    libterm-readline-perl-perl \
    libcurl4-openssl-dev \
    libuv1-dev \
    libwayland-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libxkbcommon-dev \
    libxinerama-dev \
    libxcursor-dev \
    libegl1-mesa-dev \
    pkg-config \
    wayland-scanner++ \
    libssl-dev \
    zlib1g-dev \
    git \
    doxygen \
    cmake \
    gcc-12 \
    g++-12 \
    \
    # force gcc/g++ 12 on trixie (prevent realm errors)
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-12 100 \
    \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
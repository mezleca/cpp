FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    libuv1-dev \
    libssl-dev \
    zlib1g-dev \
    git \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
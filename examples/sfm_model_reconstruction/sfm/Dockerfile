# Builder
FROM nvidia/cuda:12.3.2-devel-ubuntu22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    build-essential \
    ninja-build \
    libboost-all-dev \
    libsqlite3-dev \
    libflann-dev \
    libfreeimage-dev \
    libmetis-dev \
    libglog-dev \
    libgflags-dev \
    libglew-dev \
    qtbase5-dev \
    libatlas-base-dev \
    libsuitesparse-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES=native

RUN cmake --build build --target sfm_pipeline

# Runtime (slim)
FROM nvidia/cuda:12.3.2-runtime-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libboost-program-options1.74.0 \
    libboost-filesystem1.74.0 \
    libboost-graph1.74.0 \
    libboost-system1.74.0 \
    libsqlite3-0 \
    libfreeimage3 \
    libglog0v5 \
    libgflags2.2 \
    libatlas3-base \
    libsuitesparseconfig5 \
    libamd2 \
    libcholmod3 \
    libcolamd2 \
    libcxsparse3 \
    libmetis5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/sfm_pipeline /usr/local/bin/sfm_pipeline

ENTRYPOINT ["sfm_pipeline"]
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
    libgoogle-glog-dev \
    libgflags-dev \
    libglew-dev \
    qtbase5-dev \
    libatlas-base-dev \
    libsuitesparse-dev \
    libceres-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

# snd dependencies loop, my internet is killing me

RUN apt-get update && apt-get install -y \
    libeigen3-dev \
    libcgal-dev \
    libcgal-qt5-dev \
    libglfw3-dev \
    libglu1-mesa-dev \
    libqt5opengl5-dev \
    qttools5-dev \
    libsqlite3-dev \
    libflann-dev \
    libmetis-dev \
    libatlas-base-dev \
    && rm -rf /var/lib/apt/lists/*
    

RUN apt-get update && apt-get install -y libopenimageio-dev libopenexr-dev

RUN apt-get install -y openimageio-tools

WORKDIR /deps

RUN git clone https://github.com/colmap/colmap.git --depth 1 --branch 3.13.0 colmap

RUN sed -i \
    -e 's/add_custom_target(uninstall/add_custom_target(colmap_uninstall/g' \
    -e 's/set_target_properties(uninstall /set_target_properties(colmap_uninstall /g' \
    -e 's/add_dependencies(uninstall/add_dependencies(colmap_uninstall/g' \
    colmap/CMakeLists.txt

WORKDIR /app
COPY . .

RUN apt-get install -y \
    libopencv-dev \
    libopenimageio-dev \
    libopenexr-dev \
    openimageio-tools \
    libboost-all-dev \
    libcgal-dev \
    libcgal-qt5-dev \
    libglfw3-dev \
    libglu1-mesa-dev \
    libgl-dev \
    libqt5opengl5-dev \
    qttools5-dev \
    qtbase5-dev \
    libceres-dev \
    libatlas-base-dev \
    libsuitesparse-dev \
    libflann-dev \
    libmetis-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libhdf5-dev

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES="70;75;80;86;89" \
    -DCMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake" \
    -DFETCHCONTENT_SOURCE_DIR_COLMAP=/deps/colmap \
    -DFETCHCONTENT_SOURCE_DIR_colmap=/deps/colmap \
    -DCOLMAP_BUILD_TESTS=OFF \
    -DCOLMAP_BUILD_EXAMPLES=OFF \
    -DCOLMAP_BUILD_GUI=OFF \
    -DCMAKE_POLICY_DEFAULT_CMP0002=OLD

RUN cmake --build build --target colmap_controllers --parallel $(nproc)

RUN find /app/build/_deps -name "pairing.h" 2>/dev/null
RUN find /deps/colmap -name "pairing.h" 2>/dev/null

RUN cmake --build build --target sfm_pipeline --parallel $(nproc)

# Runtime (slim)
FROM nvidia/cuda:12.3.2-runtime-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libboost-program-options-dev \
    libboost-filesystem-dev \
    libboost-graph-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libsqlite3-0 \
    libfreeimage3 \
    libgoogle-glog-dev \
    libgoogle-glog0v5 \
    libgflags2.2 \
    libatlas3-base \
    libsuitesparseconfig5 \
    libamd2 \
    libbtf1 \
    libcamd2 \
    libccolamd2 \
    libcholmod3 \
    libcolamd2 \
    libcxsparse3 \
    libklu1 \
    libldl2 \
    libumfpack5 \
    libspqr2 \
    libgraphblas6 \
    libmetis5 \
    libflann1.9 \
    libceres2 \
    libgl1 \
    libglu1-mesa \
    libglew2.2 \
    libqt5core5a \
    libqt5gui5 \
    libqt5widgets5 \
    libqt5opengl5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/sfm_pipeline /usr/local/bin/sfm_pipeline

ENTRYPOINT ["sfm_pipeline"]
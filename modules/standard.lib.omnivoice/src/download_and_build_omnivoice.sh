#!/bin/bash

git clone https://github.com/bluryar/omnivoice.cpp.git omnivoice-ggml-cpp
cd omnivoice-ggml-cpp
git submodule update --init --recursive
patch_file="$PWD/patches/ggml-v0.9.11-omnivoice-cuda-fixes.patch"
git -C vendor/ggml apply --check "$patch_file"
git -C vendor/ggml apply "$patch_file"

export PATH=$PATH:/usr/local/cuda-12.6/bin
cmake -S . -B build -DGGML_CUDA=ON -DGGML_STATIC=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD_LIBRARIES="-ldl -Bstatic -lpthread -pthread" -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DGGML_NATIVE=OFF -DGGML_CPU_ALL_VARIANTS=OFF  -DGGML_SSE42=ON -DGGML_AVX=ON -DGGML_AVX2=ON -DGGML_FMA=ON -DGGML_F16C=ON -DGGML_BMI2=ON -DCMAKE_CUDA_ARCHITECTURES="86"
cmake --build build -j8

export OMNIVOICE_CFLAGS=-I./omnivoice-ggml-cpp/include
export "OMNIVOICE_LIBS=-L./omnivoice-ggml-cpp/build/ -lomnivoice_ggml_cpp -lomnivoice_shine -lomnivoice_miniaudio -L./omnivoice-ggml-cpp/build/ggml/src/ -lggml -lggml-cpu -lggml-base -L./omnivoice-ggml-cpp/build/ggml/src/ggml-cuda/ -lggml-cuda -L/usr/local/cuda-12.6/targets/x86_64-linux/lib/ -lcudart -lcuda -lgomp -lcublas"

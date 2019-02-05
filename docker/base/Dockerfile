# Base image
FROM phusion/baseimage

# Install essentials
RUN apt-get update \
&&  apt-get install -y --no-install-recommends \
    tar \
    wget \
    build-essential \
    software-properties-common

WORKDIR /root

# Fetch and install boost and cmake from binaries
RUN wget "https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz" --no-check-certificate \
&&  tar xf boost_1_68_0.tar.gz \
&&  rm boost_1_68_0.tar.gz \
&&  wget "https://cmake.org/files/v3.11/cmake-3.11.1-Linux-x86_64.tar.gz" --no-check-certificate \
&&  tar xf cmake-3.11.1-Linux-x86_64.tar.gz \
&&  rm cmake-3.11.1-Linux-x86_64.tar.gz \
&&  cp -r cmake-3.11.1-Linux-x86_64/bin/* /usr/local/bin \
&&  cp -r cmake-3.11.1-Linux-x86_64/doc/* /usr/local/doc \
&&  cp -r cmake-3.11.1-Linux-x86_64/share/* /usr/local/share \
&&  cp -r cmake-3.11.1-Linux-x86_64/man/* /usr/local/man

# Fetch and install protobuf from source
RUN wget "https://github.com/google/protobuf/releases/download/v3.5.1/protobuf-cpp-3.5.1.tar.gz" --no-check-certificate \
&& tar xf protobuf-cpp-3.5.1.tar.gz \
&&  rm protobuf-cpp-3.5.1.tar.gz \
&&  cd protobuf-3.5.1 \
&& ./configure --disable-shared --with-pic --prefix=/root/protobuf \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Fetch and install Clang and LLVM 4.0.0 from source
RUN wget "http://releases.llvm.org/4.0.0/clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz" \
&& tar xf clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz \
&& rm clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz \
&& cp -r clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04/* /usr/local

# Set env
ENV BOOST_ROOT="/root/boost_1_68_0" \
    LD_LIBRARY_PATH="/root/simulator/lib:${LD_LIBRARY_PATH}" CXX="clang++" \
    CMAKE_PREFIX_PATH="/root/protobuf" CMAKE_BUILD_TYPE="Release"

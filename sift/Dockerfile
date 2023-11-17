# Build stage
FROM ubuntu:20.04 as builder
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \
    apt-get install -y build-essential cmake software-properties-common

RUN add-apt-repository -y ppa:chrberger/libcluon && \
    apt-get update && \
    apt-get install -y libcluon

RUN apt-get install -y \
    libopencv-dev \
    libcanberra-gtk-module \
    libcanberra-gtk3-module \
    libopencv-core-dev \
    libopencv-imgproc-dev \
    libopencv-video-dev \
    libopencv-calib3d-dev \
    libopencv-features2d-dev \
    libopencv-objdetect-dev \
    libopencv-highgui-dev \
    libopencv-videoio-dev \
    libopencv-flann-dev \
    python3-opencv


COPY . /opt/sources

RUN mkdir -p /opt/sources/build && \
    cd /opt/sources/build && \
    cmake -D CMAKE_BUILD_TYPE=Release .. && \
    make

WORKDIR /opt/sources/build

CMD ["./sift-detector"]

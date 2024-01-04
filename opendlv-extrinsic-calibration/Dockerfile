FROM ubuntu:18.04 as builder
ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update -y && \
        apt-get install -y build-essential cmake software-properties-common

RUN add-apt-repository -y ppa:chrberger/libcluon && \
        apt-get update && \
        apt-get install libcluon

RUN apt-get install -y libopencv-dev
RUN apt-get install -y libyaml-cpp-dev

ADD . /opt/sources
WORKDIR /opt/sources

RUN mkdir /tmp/build && \
        cd /tmp/build && \
        cmake -D CMAKE_BUILD_TYPE=Release /opt/sources && \
        make && cp intrinsic /tmp


# Deploy .
FROM ubuntu:18.04
ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update && \
    apt-get install -y \
    libcanberra-gtk-module \
    libcanberra-gtk3-module \
    libopencv-core3.2 \
    libopencv-imgproc3.2 \
    libopencv-video3.2 \
    libopencv-calib3d3.2 \
    libopencv-features2d3.2 \
    libopencv-objdetect3.2 \
    libopencv-highgui3.2 \
    libopencv-videoio3.2 \
    libopencv-flann3.2 \
    python3-opencv

RUN apt-get install -y libyaml-cpp-dev

WORKDIR /opt
COPY --from=builder /tmp .
FROM gocv/opencv:4.8.1-ubuntu-20.04

RUN apt-get update -y && \
    apt-get install -y build-essential cmake software-properties-common

RUN add-apt-repository -y ppa:chrberger/libcluon && \
    apt-get update && \
    apt-get install -y libcluon

RUN apt-get install -y libyaml-cpp-dev

ADD . /opt/sources
WORKDIR /opt/sources


RUN mkdir /tmp/build && \
    cd /tmp/build && \
    cmake -D CMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 /opt/sources && \
    make
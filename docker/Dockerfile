FROM ubuntu:20.04

ENV DEBIAN_FRONTEND    noninteractive
RUN sed -i -e 's/archive.ubuntu.com/mirrors.163.com/' /etc/apt/sources.list

RUN apt-get update -qq

RUN apt-get install -y libssl-dev wget curl xterm build-essential gcc g++ pkg-config libz-dev libglib2.0-dev libpixman-1-dev libfdt-dev git libstdc++6 libmbedtls-dev git vim ninja-build python3 python-is-python3 flex bison autoconf bc

COPY ./*.sh /qemu-loongarch-runenv/

WORKDIR /qemu-loongarch-runenv

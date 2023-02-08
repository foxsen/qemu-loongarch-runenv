FROM ubuntu:20.04

ENV DEBIAN_FRONTEND    noninteractive
RUN sed -i -e 's/archive.ubuntu.com/mirrors.163.com/' /etc/apt/sources.list

RUN apt-get update -qq

RUN apt-get install -y libssl-dev wget curl xterm build-essential gcc g++ pkg-config libz-dev libglib2.0-dev libpixman-1-dev libfdt-dev git libstdc++6 libmbedtls-dev git vim ninja-build python3 python-is-python3 flex bison autoconf bc

# optional support(graphic etc.) for qemu
RUN apt-get install -y libsdl2-dev libgtk-3-dev

# Chinese locale
RUN apt-get install -y language-pack-zh-hans
RUN locale-gen zh_CN.UTF-8

COPY ./*.sh /qemu-loongarch-runenv/

COPY ./*.patch /qemu-loongarch-runenv/

COPY ./poky-minimal.ext4 /qemu-loongarch-runenv/

WORKDIR /qemu-loongarch-runenv
ENV LANG zh_CN.UTF-8
ENV LC_ALL zh_CN.UTF-8

# docker for build and run loongarch system mode qemu

## Usage

steps:

1. build docker image

    docker build -t qemu-la .

This is just an ubuntu os with necessary software components installed.

2. run the docker images

    docker run -it qemu-la /bin/bash

3. setup

In the docker container, run ./setup.sh to download/build/install qemu/bios/kernel etc. You can run the individual setup-*.sh if anything goes wrong.

Be sure to keep the network smooth. Github is not stable for most Chinese users, please find some ways to workaround.

4. run

Use ./run.sh as an example to run qemu with loongarch UEFI bios and kernel.

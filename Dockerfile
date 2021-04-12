FROM debian:buster

USER root

WORKDIR /workspace
VOLUME /workspace

VOLUME /mnt/boot

RUN apt-get update && \
    apt-get install -y gcc make gcc-arm-linux-gnueabi binutils-arm-linux-gnueabi \
        gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu



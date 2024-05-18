FROM ubuntu:24.04

RUN apt update && apt install -y curl python3 xz-utils file make

WORKDIR /staging

RUN curl -O https://updates.victronenergy.com/feeds/venus/release/sdk/venus-dunfell-x86_64-arm-cortexa8hf-neon-toolchain-v3.31.sh
RUN chmod u+x /staging/venus-dunfell-x86_64-arm-cortexa8hf-neon-toolchain-v3.31.sh
RUN /staging/venus-dunfell-x86_64-arm-cortexa8hf-neon-toolchain-v3.31.sh

RUN rm -rf /staging

WORKDIR /

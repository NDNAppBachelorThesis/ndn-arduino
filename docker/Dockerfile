FROM ubuntu:22.04

EXPOSE 6363

WORKDIR /tmp

RUN apt update
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt install -y git

# Compile and install ndn-cxx lib
WORKDIR /tmp
RUN apt install -y build-essential pkg-config python3-minimal libboost-all-dev libssl-dev libsqlite3-dev
RUN git clone https://github.com/named-data/ndn-cxx.git \
    && cd ndn-cxx \
    && git fetch --all --tags \
    && git checkout tags/ndn-cxx-0.8.1 \
    && ./waf configure \
    && ./waf \
    && ./waf install  \
    && ldconfig \
    && rm -rf *

# Compile and install NFD
WORKDIR /tmp
RUN apt install -y libpcap-dev libsystemd-dev
RUN git clone --recursive https://github.com/named-data/NFD.git \
    && cd NFD \
    && git fetch --all --tags \
    && git checkout tags/NFD-22.12 \
    && ./waf configure \
    && ./waf \
    && ./waf install \
    && rm -rf *

# Post setup
WORKDIR /
ADD addNode.sh .
# Custom conf file, which enabled 'localhop_security' and disables 'auto_prefix_propagate'
ADD nfd.conf /usr/local/etc/ndn

CMD ["nfd"]

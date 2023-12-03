FROM ubuntu:22.04

WORKDIR /tmp

RUN apt update
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt install -y git

RUN git clone https://github.com/named-data/ndn-cxx.git
RUN git clone --recursive https://github.com/named-data/NFD.git

# Install lib
WORKDIR /tmp/ndn-cxx
RUN apt install -y build-essential pkg-config python3-minimal libboost-all-dev libssl-dev libsqlite3-dev
RUN git fetch --all --tags \
    && git checkout tags/ndn-cxx-0.8.1
RUN ./waf configure
RUN ./waf
RUN ./waf install && ldconfig

# Install nfd
WORKDIR /tmp/NFD
RUN apt install -y libpcap-dev libsystemd-dev
RUN git fetch --all --tags \
    && git checkout tags/NFD-22.12
RUN ./waf configure
RUN ./waf
RUN ./waf install

# Post setup
WORKDIR /tmp
RUN cp /usr/local/etc/ndn/nfd.conf.sample /usr/local/etc/ndn/nfd.conf

EXPOSE 6363

ADD addNode.sh .

CMD ["nfd"]

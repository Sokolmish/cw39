FROM ubuntu:20.04

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

ENV APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1
RUN apt-get update \
    && apt-get install -y software-properties-common wget \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && (wget --no-check-certificate -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -) \
    && add-apt-repository 'deb https://apt.llvm.org/focal/ llvm-toolchain-focal-13 main' \
    && apt-get install -y \
      make cmake \
      libfmt-dev \
      g++-11     \
      llvm-13    \
      flex bison \
      zlib1g-dev \
    && apt-get remove -y \
      software-properties-common wget \
      gcc-9 cpp-9 gcc-9-base \
    && apt-get autoremove -y \
    && rm -rf /var/lib/apt/lists/* \
    && ln -s /usr/bin/gcc-11 /usr/bin/gcc \
    && ln -s /usr/bin/g++-11 /usr/bin/g++ \
    && touch /usr/lib/llvm-13/lib/libMLIRSupportIndentedOstream.a

RUN mkdir /root/build
WORKDIR /root

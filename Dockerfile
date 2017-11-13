#!/bin/echo docker build . -f
#{
# Copyright 2017 Samsung Electronics France SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#}

FROM ubuntu:14.04
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)
ENV project iotivity

ARG SCONSFLAGS
ENV SCONSFLAGS ${SCONSFLAGS:-"VERBOSE=1"}

ARG prefix
ENV prefix ${prefix:-/usr/}
ARG destdir
ENV destdir ${destdir:-/usr/local/opt/${project}}

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL en_US.UTF-8
ENV LANG ${LC_ALL}

RUN echo "#log: Configuring locales" \
  && apt-get update \
  && apt-get install -y locales \
  && echo "${LC_ALL} UTF-8" | tee /etc/locale.gen \
  && locale-gen ${LC_ALL} \
  && dpkg-reconfigure locales

RUN echo "#log: ${project}: Setup system" \
  && apt-get update -y \
  && apt-get install -y \
    devscripts \
    debhelper \
    base-files \
\
    autoconf \
    automake \
    autotools-dev \
    bash \
    git \
    libtool \
    make \
    python-dev \
    scons \
    sudo \
    unzip \
    valgrind \
    wget \
\
    libboost-date-time-dev \
    libboost-iostreams-dev \
    libboost-log-dev \
    libboost-program-options-dev \
    libboost-regex-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libbz2-dev \
    libcurl4-openssl-dev \
    libglib2.0-dev \
    libicu-dev \
    libsqlite3-dev \
    uuid-dev \
  && apt-get clean \
  && sync

ADD . /usr/local/src/${project}
WORKDIR /usr/local/src/${project}
RUN echo "#log: ${project}: Preparing sources" \
  && uname -a \
  && cat /etc/os-release \
  && scons --version \
  && gcc --version \
  && g++ --version \
  && [ ! -x prep.sh ] || EXEC_MODE=true ./prep.sh \
  && sync

RUN echo "#log: ${project}: Building sources" \
  && scons \
  && sync

RUN echo "#log: ${project}: Installing binaries" \
  && scons install --prefix="${prefix}" --install-sandbox="${destdir}" \
  && find ${destdir} \
  && sync

RUN echo "#log: ${project}: Cleaning objects" \
  && scons -c \
  && find . \
  && sync

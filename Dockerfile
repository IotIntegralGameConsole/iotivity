#!/bin/echo docker build . -f
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

FROM ubuntu:14.04
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)
ENV project iotivity

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL en_US.UTF-8
ENV LANG ${LC_ALL}

RUN echo "#log: Configuring locales" \
 && apt-get update \
 && apt-get install -y locales \
 && echo "${LC_ALL} UTF-8" | tee /etc/locale.gen \
 && locale-gen ${LC_ALL} \
 && dpkg-reconfigure locales

RUN echo "#log: Preparing system for ${project}" \
 && apt-get update -y \
 && apt-get install -y \
  bash \
  git \
  make \
  sudo \
  devscripts \
  debhelper \
\
  wget \
  autoconf \
  automake \
  autotools-dev \
  libtool \
  libbz2-dev \
  libglib2.0-dev \
  libicu-dev \
  python-dev \
  scons \
  unzip \
  uuid-dev \
  libcurl4-openssl-dev \
  valgrind \
  libboost-thread-dev \
  libboost-date-time-dev \
  libboost-iostreams-dev \
  libboost-log-dev \
  libboost-program-options-dev \
  libboost-regex-dev \
  libboost-system-dev \
  libsqlite3-dev \
 && apt-get clean \
 && sync

ADD . /root/${project}
WORKDIR /root/${project}
RUN echo "#log: Building ${project}" \
 && EXEC_MODE=true ./prep.sh \
 && scons VERBOSE=1 \
 && sync

ARG destdir
ENV destdir ${destdir:-/tmp/opt/%{project}}

ARG prefix
ENV prefix ${prefix:-/usr/local}

RUN echo "#log: Installing ${project}" \
 && scons VERBOSE=1 install --install-sandbox=${destdir} --prefix=${prefix} \
 && sync
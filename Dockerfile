#!/bin/echo docker build . -f
# -*- coding: utf-8 -*-
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

FROM resin/rpi-raspbian
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)

RUN [ "cross-build-start" ]

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL en_US.UTF-8
ENV LANG ${LC_ALL}

RUN echo "#log: Configuring locales" \
  && set -x \
  && apt-get update -y \
  && apt-get install -y locales \
  && echo "${LC_ALL} UTF-8" | tee /etc/locale.gen \
  && locale-gen ${LC_ALL} \
  && dpkg-reconfigure locales \
  && sync

ENV project iotivity
ARG SCONSFLAGS
ENV SCONSFLAGS ${SCONSFLAGS:-"VERBOSE=1"}

ARG prefix
ENV prefix ${prefix:-/usr/}
ARG destdir
ENV destdir ${destdir:-/usr/local/opt/${project}}

RUN echo "#log: ${project}: Setup system" \
  && set -x \
  && apt-get update -y \
  && apt-get install -y \
  fakeroot \
  make \
  sudo \
  dpkg-dev \
  debhelper \
  && apt-get clean \
  && sync

ADD . /usr/local/src/${project}/${project}/
WORKDIR /usr/local/src/${project}/${project}/
RUN echo "#log: ${project}: Preparing sources" \
  && set -x \
  && ./debian/rules rule/dist \
  && sync

RUN echo "#log: ${project}: Building sources" \
  && set -x \
  && ./debian/rules \
  && sudo debi \
  && ls -la /usr/local/src/${project}/*.* \
  && dpkg -L ${project} \
  && sync

RUN [ "cross-build-end" ]  

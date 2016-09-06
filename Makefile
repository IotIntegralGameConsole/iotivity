#!/usr/bin/make -f
name?=iotivity

default/%:
	@echo "# $@ ignored"


default: all

# Overide variables
host_arch?=$(shell uname -m || echo "default")
OSTYPE?=$(shell echo "$${OSTYPE}")
uname?=$(shell echo `uname -s` || echo "")

TARGET_OS?=${uname}
ifeq ("",${TARGET_OS})
TARGET_OS=${OSTYPE}
endif

ifeq ("linux-gnu","${TARGET_OS}")
TARGET_OS=linux
endif
ifeq ("Linux","${TARGET_OS}")
TARGET_OS=linux
endif
ifeq ("",${TARGET_OS})
TARGET_OS=default
endif

ifeq ("i386","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i486","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i586","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i686","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("armel","${host_arch}")
TARGET_ARCH?=arm
else
ifeq ("armhf","${host_arch}")
TARGET_ARCH?=arm
endif
ifeq ("aarch64","${host_arch}")
TARGET_ARCH?=arm64
endif
ifeq ("armv7l","${host_arch}")
TARGET_ARCH?=arm
endif
endif
endif
endif
endif
endif



# Default variables to override:
#release_mode?=0
#secure_mode?=0
#logging_mode?=True

TARGET_ARCH?=${host_arch}
TARGET_OS?=${uname}
TARGET_TRANSPORT?=IP
includedir?=/usr/include
libdir?=/usr/lib
sbindir?=/usr/sbin

ifeq (0,${release_mode})
build_dir?=debug
debug_mode?=1
else
build_dir?=release
release_mode?=1
debug_mode?=0
endif


config_build?=VERBOSE=1 V=1
config_build+=DEBUG=${debug_mode} RELEASE=${release_mode}
config_build+=TARGET_ARCH=${TARGET_ARCH}
config_build+=TARGET_OS=${TARGET_OS}

# Overide default config if defined
ifneq ("","${logging_mode}")
config_build+=LOGGING=${logging_mode}
endif

ifneq ("","${secure_mode}")
config_build+=SECURED=${secure_mode}
endif

# Default variables to override:
#config_build+=TARGET_TRANSPORT=IP
#config_build+=WITH_TCP=1


all: build

build: SConstruct
	scons ${config_build}


check/%: auto_build.sh
	${<D}/${<F} unit_tests\
 && "log: $@: succeeded" \
 || echo "warning: $@: failed and ignored, TODO: fix $@ (and don't ignore $@ failures)"

check/default: auto_build.sh
	${<D}/${<F} unit_tests

check: check/${host_arch}


install:
	scons ${config_build} $@
	${MAKE} rule/install

rule/install: rule/fix rule/install_headers
	@echo "TODO:  rule/install_examples"
	install -d ${DESTDIR}${libdir}/${name}/examples/
	touch ${DESTDIR}${libdir}/${name}/examples/TODO

	install -d ${DESTDIR}${libdir}

	find . -iname "lib*.a" -exec install "{}" ${DESTDIR}${libdir}/ \;
	find . -iname "lib*.so" -exec install "{}" ${DESTDIR}${libdir}/ \;
	-rm -fv ${DESTDIR}${libdir}/lib*.a
	install -d ${DESTDIR}${libdir}/pkgconfig
	install -d ${DESTDIR}/usr/share/pkgconfig
	install ${name}.pc ${DESTDIR}${libdir}/pkgconfig/
	ln -fs ../../../${libdir}/pkgconfig/${name}.pc ${DESTDIR}/usr/share/pkgconfig

	@echo "TODO: workaround headers namespaces"
	install -d ${DESTDIR}${includedir}/
	ln -fs iotivity/resource ${DESTDIR}${includedir}/
	ln -fs iotivity/service ${DESTDIR}${includedir}/
	ln -fs iotivity/resource/c_common ${DESTDIR}${includedir}/


clean:
	rm -f *~

rule/fix: ${name}.pc
	sed -e 's|^prefix=.*|prefix=/usr|g' -i ${<}


rule/install_headers: resource service
	find $< \
  -iname "include" -o -iname 'inc' -a -type d\
 | grep -v example | grep -v csdk \
 | while read dir ; do \
  dirname=$$(dirname -- "$${dir}") ; \
  install -d ${DESTDIR}${includedir}/${name}/$${dirname} ; \
  install $${dir}/*.* ${DESTDIR}${includedir}/${name}/$${dirname}/ ; \
 done
	cd resource/csdk && \
 find . -iname "include" -o -iname 'inc' -a -type d\
 | while read dir ; do \
  dirname=$$(dirname -- "$${dir}") ; \
  install -d ${DESTDIR}${includedir}/${name}/resource/$${dirname} ; \
  install $${dir}/*.* ${DESTDIR}${includedir}/${name}/resource/$${dirname}/ ; \
 done
	install -d ${DESTDIR}${includedir}/${name}/resource/oc_logger/targets/

	install ./resource/oc_logger/include/targets/*.* \
 ${DESTDIR}${includedir}/${name}/resource/oc_logger/targets/

	install -d ${DESTDIR}${includedir}/${name}/resource/c_common

	install ./resource/c_common/*.h \
 ${DESTDIR}${includedir}/${name}/resource/c_common/

	install -d ${DESTDIR}${libdir}/${name}/examples/


rule/install_examples: out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}
	install $</resource/examples/*client ${DESTDIR}${libdir}/${name}/examples/
	install $</resource/examples/*server ${DESTDIR}${libdir}/${name}/examples/
	install $</*/*/*/*client ${DESTDIR}${libdir}/${name}/examples/
	install $</*/*/*/*server ${DESTDIR}${libdir}/${name}/examples/
	-find out -type f -executable | grep -v '.so$'


rule/help:
	@echo "OSTYPE=${OSTYPE}"
	@echo "TARGET_OS=${TARGET_OS}"
	@echo "host_arch=${host_arch}"
	@echo "uname=${uname}"

docs_file?=${name}-docs.zip

rule/docs: resource/docs/javadocGen.sh
	rm -rf ${docs_file}
	mkdir -p ${<D}/html
	touch ${<D}/html/footer.html
	cd ${<D} && ./${<F}
	cd ${CURDIR} && zip -r9 ${docs_file} ./${<D}
	find . \
 -iname 'DoxyFile' -printf "%h\n" \
 -o -iname 'extlibs' -prune -a -iname 'out' -prune \
 | while read file ; do \
  cd "${CURDIR}/$${file}" && doxygen ; \
  cd ${CURDIR} ; \
  zip -r9 ${docs_file} $${file} ; \
done

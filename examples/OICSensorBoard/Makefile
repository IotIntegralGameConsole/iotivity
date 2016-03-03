default: all

#Set the two variables below for the client build.
CLIENTARCH?=x86
BUILDTYPE?=release
OSTYPE?=linux
##

YOCTOCXXFLAGS=-I$(SDKTARGETSYSROOT)/usr/include/iotivity/resource/ -I$(SDKTARGETSYSROOT)/usr/include/iotivity/resource/stack -I$(SDKTARGETSYSROOT)/usr/include/iotivity/resource/ocrandom -I$(SDKTARGETSYSROOT)/usr/include/iotivity/resource/logger -I$(SDKTARGETSYSROOT)/usr/include/iotivity/resource/oc_logger

YOCTOLDFLAGS=-loc -loctbstack -loc_logger -lmraa


IOTIVITY_DIR?=../../

CXXFLAGS=-I${IOTIVITY_DIR}resource/include -I${IOTIVITY_DIR}resource/c_common -I${IOTIVITY_DIR}resource/csdk/stack/include -I${IOTIVITY_DIR}resource/c_common/ocrandom/include -I${IOTIVITY_DIR}resource/csdk/logger/include -I${IOTIVITY_DIR}resource/oc_logger/include

CXXFLAGS+=-I${IOTIVITY_DIR}resource/c_common -I${IOTIVITY_DIR}resource/include


LDFLAGS=-L${IOTIVITY_DIR}out/${OSTYPE}/$(CLIENTARCH)/$(BUILDTYPE) \
  -loc -loctbstack -loc_logger \
  -lconnectivity_abstraction

install_dir?=${DESTDIR}/usr/lib/iotivity/examples/OICSensorBoard

exes?=client
ifneq ($(SDKTARGETSYSROOT),)
exes+=server
endif


all: client
	@echo "server depends on mraa"

client: client.o
	g++ -o client client.o $(LDFLAGS)

client.o: client.cpp
	g++ -std=c++0x -c -o $@ $< $(CXXFLAGS)

observer.o: observer.cpp
	$(CXX) -std=c++0x -c -o $@ $< $(YOCTOCXXFLAGS)

server.o: server.cpp
ifeq ($(SDKTARGETSYSROOT),)
	echo "Error: Yocto cross-toolchain environment not initialized"
	exit 1
endif
	$(CXX) -std=c++0x -c -o $@ $< $(YOCTOCXXFLAGS)

server: server.o observer.o
	$(CXX) -o server server.o observer.o $(YOCTOLDFLAGS)

clean:
	rm -rf server client *.o

install: ${exes}
	install -d ${install_dir}
	install $^ ${install_dir}

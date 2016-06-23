$(info TC_PREFIX=$(TOOLCHAIN_PREFIX))
$(info CFLAGS=$(TARGET_CFLAGS))
$(info CXXFLAGS=$(TARGET_CXXFLAGS) $(TARGET_NO_EXECUTE_CFLAGS))
$(info CPPPATH=$(TARGET_C_INCLUDES) $(__ndk_modules.$(APP_STL).EXPORT_C_INCLUDES))
$(info SYSROOT=$(SYSROOT_LINK))
$(info LDFLAGS=$(TARGET_LDFLAGS) $(TARGET_NO_EXECUTE_LDFLAGS) $(TARGET_NO_UNDEFINED_LDFLAGS) $(TARGET_RELRO_LDFLAGS))
$(info TC_VER=$(TOOLCHAIN_VERSION))
$(info PLATFORM=$(APP_PLATFORM))


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define build type
BUILD = debug

PROJECT_ROOT_PATH           = $(IOTIVITY_PATH)/resource/csdk/connectivity
ROOT_DIR_PATH               = $(IOTIVITY_PATH)
EXT_LIB_PATH                = $(ROOT_DIR_PATH)/extlibs
PROJECT_API_PATH            = $(PROJECT_ROOT_PATH)/api
PROJECT_INC_PATH            = $(PROJECT_ROOT_PATH)/inc
PROJECT_SRC_PATH            = $(PROJECT_ROOT_PATH)/src
PROJECT_COMMON_PATH         = $(PROJECT_ROOT_PATH)/common
PROJECT_COMMON_INC_PATH     = $(PROJECT_COMMON_PATH)/inc
PROJECT_COMMON_SRC_PATH     = $(PROJECT_COMMON_PATH)/src
PROJECT_LIB_PATH            = $(PROJECT_ROOT_PATH)/lib
PROJECT_EXTERNAL_PATH       = $(PROJECT_ROOT_PATH)/external/inc
DTLS_LIB                    = $(EXT_LIB_PATH)/tinydtls
OIC_C_COMMON_PATH           = $(ROOT_DIR_PATH)/resource/c_common
CSDK_PATH                   = $(ROOT_DIR_PATH)/resource/csdk
EXTLIBS_PATH                = $(ROOT_DIR_PATH)/extlibs
UTIL_PATH                   = $(ROOT_DIR_PATH)/resource/csdk/connectivity/util
#GLIB_PATH                  = ../../../../../../extlibs/glib/glib-2.40.2

#Modify below values to enable/disable the Adapter
#Suffix "NO_" to disable given adapter
EDR             = $(EDR_ADAPTER_FLAG)
IP              = $(IP_ADAPTER_FLAG)
LE              = $(LE_ADAPTER_FLAG)

#Add Pre processor definitions
DEFINE_FLAG =  -DWITH_POSIX -D__ANDROID__
DEFINE_FLAG +=  -D__WITH_DTLS__
DEFINE_FLAG += -D$(EDR) -D$(LE) -D$(IP)

#Add Debug flags here
DEBUG_FLAG      = -DTB_LOG

BUILD_FLAG.debug        = $(DEFINE_FLAG) $(DEBUG_FLAG)
BUILD_FLAG.release      =       $(DEFINE_FLAG)
BUILD_FLAG = $(BUILD_FLAG.$(BUILD))
BUILD_FLAG = $(BUILD_FLAG.debug)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include glib

#include $(CLEAR_VARS)
#LOCAL_PATH = $(PROJECT_LIB_PATH)/android
#LOCAL_MODULE = Glib
#LOCAL_SRC_FILES := libglib-2.40.2.so
#LOCAL_EXPORT_C_INCLUDES = $(GLIB_PATH) \
#                          $(GLIB_PATH)/glib
#
#include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include glibthread

#include $(CLEAR_VARS)
#LOCAL_PATH = $(PROJECT_LIB_PATH)/android
#LOCAL_MODULE = GLibThread
#LOCAL_SRC_FILES := libgthread-2.40.2.so
#LOCAL_EXPORT_C_INCLUDES = $(GLIB_PATH) \
#                          $(GLIB_PATH)/glib
#
#include $(PREBUILT_SHARED_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build TinyDtls

include $(CLEAR_VARS)
include $(DTLS_LIB)/Android.mk

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build OIC C Common libraries required for CA

include $(CLEAR_VARS)
LOCAL_MODULE = OICCommon

#Build Common Libraries
LOCAL_PATH = $(OIC_C_COMMON_PATH)
LOCAL_CFLAGS = -D__ANDROID__ $(DEBUG_FLAG)
LOCAL_CFLAGS += -std=c99

LOCAL_C_INCLUDES = $(OIC_C_COMMON_PATH)/oic_malloc/include \
                   $(OIC_C_COMMON_PATH)/oic_string/include
LOCAL_SRC_FILES  = oic_malloc/src/oic_malloc.c \
                   oic_string/src/oic_string.c

include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build CACommon

include $(CLEAR_VARS)

#Build Common Libraries
LOCAL_PATH = $(PROJECT_COMMON_SRC_PATH)
LOCAL_MODULE = CACommon
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
#LOCAL_SHARED_LIBRARIES = Glib GLibThread
LOCAL_STATIC_LIBRARIES = rt pthread
LOCAL_STATIC_LIBRARIES += OICCommon

LOCAL_CFLAGS = -D__ANDROID__ $(DEBUG_FLAG)
LOCAL_CFLAGS += -std=c99  -DADB_SHELL
#LOCAL_CFLAGS += -std=c99

LOCAL_C_INCLUDES = $(PROJECT_COMMON_INC_PATH)
LOCAL_C_INCLUDES += $(PROJECT_API_PATH)
LOCAL_C_INCLUDES += $(OIC_C_COMMON_PATH)/oic_malloc/include
LOCAL_C_INCLUDES += $(OIC_C_COMMON_PATH)/oic_string/include
LOCAL_C_INCLUDES += $(CSDK_PATH)/logger/include

LOCAL_SRC_FILES =       uarraylist.c uqueue.c \
                        cathreadpool_pthreads.c camutex_pthreads.c \
                        caremotehandler.c

include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build CACoap

include $(CLEAR_VARS)

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_PATH = $(PROJECT_LIB_PATH)/libcoap-4.1.1
LOCAL_MODULE = CACoap
LOCAL_EXPORT_C_INCLUDES = $(PROJECT_LIB_PATH)/libcoap-4.1.1
LOCAL_CFLAGS = -std=c99 -DWITH_POSIX -DWITH_TCP
LOCAL_SRC_FILES = pdu.c net.c debug.c encode.c uri.c coap_list.c resource.c hashkey.c \
                                        str.c option.c async.c subscribe.c block.c

include $(BUILD_STATIC_LIBRARY)

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#Build CA

#Relative path to LOCAL_PATH (PROJECT_SRC_PATH)
LOCAL_PLATFORM                          = android

ENET_ADAPTER_PATH                       = ethernet_adapter/$(LOCAL_PLATFORM)
EDR_ADAPTER_PATH                        = bt_edr_adapter/$(LOCAL_PLATFORM)
LE_ADAPTER_PATH                         = bt_le_adapter/$(LOCAL_PLATFORM)
IP_ADAPTER_PATH                         = ip_adapter
ADAPTER_UTILS                           = adapter_util

include $(CLEAR_VARS)

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_PATH = $(PROJECT_SRC_PATH)
LOCAL_MODULE = CA

LOCAL_STATIC_LIBRARIES = CACommon CACoap TinyDtls

LOCAL_C_INCLUDES = $(PROJECT_API_PATH)
LOCAL_C_INCLUDES += $(PROJECT_COMMON_INC_PATH)
LOCAL_C_INCLUDES += $(PROJECT_INC_PATH)
LOCAL_C_INCLUDES += $(PROJECT_LIB_PATH)/libcoap-4.1.1
LOCAL_C_INCLUDES += $(PROJECT_EXTERNAL_PATH)
LOCAL_C_INCLUDES += $(OIC_C_COMMON_PATH)/oic_malloc/include
LOCAL_C_INCLUDES += $(OIC_C_COMMON_PATH)/oic_string/include
LOCAL_C_INCLUDES += $(OIC_C_COMMON_PATH)/ocrandom/include
LOCAL_C_INCLUDES += $(CSDK_PATH)/logger/include
LOCAL_C_INCLUDES += $(EXTLIBS_PATH)/timer/
LOCAL_C_INCLUDES += $(PROJECT_SRC_PATH)/bt_le_adapter/android/
LOCAL_C_INCLUDES += $(DTLS_LIB)
#LOCAL_C_INCLUDES += $(UTIL_PATH)/inc/
#LOCAL_C_INCLUDES += $(UTIL_PATH)/src/btpairing/android/
#LOCAL_C_INCLUDES += $(UTIL_PATH)/src/camanager/android/

LOCAL_CFLAGS += $(BUILD_FLAG) 
LOCAL_CFLAGS += -std=c99 -DWITH_POSIX -DWITH_BWT -DWITH_TCP

LOCAL_SRC_FILES = \
                caconnectivitymanager.c cainterfacecontroller.c \
                camessagehandler.c canetworkconfigurator.c caprotocolmessage.c \
                caretransmission.c caqueueingthread.c cablockwisetransfer.c \
                $(ADAPTER_UTILS)/caadapternetdtls.c $(ADAPTER_UTILS)/caadapterutils.c \
                bt_le_adapter/caleadapter.c $(LE_ADAPTER_PATH)/caleclient.c \
                $(LE_ADAPTER_PATH)/caleserver.c $(LE_ADAPTER_PATH)/caleutils.c \
                $(LE_ADAPTER_PATH)/calenwmonitor.c \
                bt_edr_adapter/caedradapter.c $(EDR_ADAPTER_PATH)/caedrutils.c \
                $(EDR_ADAPTER_PATH)/caedrclient.c $(EDR_ADAPTER_PATH)/caedrserver.c \
                $(EDR_ADAPTER_PATH)/caedrnwmonitor.c \
                $(IP_ADAPTER_PATH)/caipadapter.c $(IP_ADAPTER_PATH)/caipserver.c \
                $(IP_ADAPTER_PATH)/android/caipnwmonitor.c \
                $(OIC_C_COMMON_PATH)/ocrandom/src/ocrandom.c \
                $(CSDK_PATH)/logger/src/logger.c \
                $(EXTLIBS_PATH)/timer/timer.c 
                #$(UTIL_PATH)/src/cautilinterface.c \
                #$(UTIL_PATH)/src/btpairing/android/cabtpairing.c \
                #$(UTIL_PATH)/src/camanager/android/caleautoconnector.c \
                #$(UTIL_PATH)/src/camanager/android/caleconnectionmanager.c \
                #$(UTIL_PATH)/src/camanager/android/camanagerdevice.c \
                #$(UTIL_PATH)/src/camanager/android/camanagerleutil.c \
                
                
include $(BUILD_STATIC_LIBRARY)
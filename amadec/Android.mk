LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := \
        -fPIC -D_POSIX_SOURCE

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/include \

LOCAL_SRC_FILES := \
           adec-external-ctrl.c adec-internal-mgt.c adec-message.c adec-pts-mgt.c feeder.c \
           dsp/audiodsp-ctl.c audio_out/android-out.cpp

LOCAL_CFLAGS += -DENABLE_WAIT_FORMAT

LOCAL_MODULE := libamadec

LOCAL_ARM_MODE := arm

$(shell cd $(LOCAL_PATH)/firmware && { \
for f in *.bin; do \
  md5sum "$$f" > "$$f".checksum; \
done;})

copy_from := $(wildcard $(LOCAL_PATH)/firmware/*.bin)

copy_from += $(wildcard $(LOCAL_PATH)/firmware/*.checksum)

install_pairs := $(foreach f,$(copy_from),$(f):system/etc/firmware/$(notdir $(f)))

PRODUCT_COPY_FILES += $(install_pairs)

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_CFLAGS := \
        -fPIC -D_POSIX_SOURCE

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/include \

LOCAL_SRC_FILES := \
           adec-external-ctrl.c adec-internal-mgt.c adec-message.c adec-pts-mgt.c feeder.c \
           dsp/audiodsp-ctl.c audio_out/android-out.cpp

LOCAL_MODULE := libamadec

LOCAL_ARM_MODE := arm

LOCAL_SHARED_LIBRARIES += libutils libmedia libz libbinder libdl libcutils libc

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


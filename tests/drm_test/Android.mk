# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdrm \
	libEGL \
	libGLESv2 \
	libhardware \
	liblog \
	libsync \
	libui \
	libutils 


LOCAL_C_INCLUDES := \
	system/core/include/utils \
	system/core/libsync \
	system/core/libsync/include \
	$(LOCAL_PATH)/../../public \
	$(LOCAL_PATH)/../../common/core \
	$(LOCAL_PATH)/../../common/compositor \
	$(LOCAL_PATH)/../../common/compositor/gl \
	$(LOCAL_PATH)/../../common/display \
	$(LOCAL_PATH)/../../common/utils \
	$(LOCAL_PATH)/../../common/watchers \
	$(LOCAL_PATH)/../../os/android

LOCAL_SRC_FILES := \
	modeset.c \
        uevents.c

#LOCAL_CPPFLAGS += \
	-DHWC2_USE_CPP11 \
	-DHWC2_INCLUDE_STRINGIFICATION \
	-DUSE_ANDROID_SYNC \
	-DUSE_ANDROID_SHIM \
	-fPIC -O2 \
	-D_FORTIFY_SOURCE=2 \
	-fstack-protector-strong \
	-fPIE -Wformat -Wformat-security

LOCAL_MODULE := modeset
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


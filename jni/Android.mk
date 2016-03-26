LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := unpack
LOCAL_CFLAGS    :=   -c -O2 -Wall  -flto -dM 
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := \
	lzma/7zAlloc.c \
    lzma/Alloc.c \
    lzma/7zBuf.c \
    lzma/7zBuf2.c \
    lzma/7zCrc.c \
    lzma/7zDec.c \
    lzma/7zIn.c  \
    lzma/7zFile.c \
    lzma/7zStream.c \
    lzma/7zCrcOpt.c \
    lzma/CpuArch.c \
    lzma/LzmaDec.c \
    lzma/Lzma2Dec.c \
    lzma/Bra.c \
    lzma/Bra86.c \
    lzma/Bcj2.c \
    lzma/Ppmd7.c \
    lzma/Ppmd7Dec.c \
	MemMap.cpp \
	unpack.cpp 

include $(BUILD_SHARED_LIBRARY)

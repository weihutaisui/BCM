STR_SRCS := strCatSize.c    \
              strCmpNoCase.c  \
              strCmpNoCaseSize.c \
              strCpyLower.c   \
              strCpyUpper.c   \
              strMaxCat.c     \
              strMaxCpy.c     \
              strPrintf.c     \
              strScanf.c      \
              strTok.c

LOCAL_INCLUDES := inc

# build defines
STR_DEFS := $(addprefix -I$(STR_ROOT)/,$(LOCAL_INCLUDES))

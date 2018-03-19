ASSERT_SRCS := src/xchgAssertFailed.c

LOCAL_INCLUDES := inc

# build defines
ASSERT_DEFS := $(addprefix -I$(ASSERT_ROOT)/,$(LOCAL_INCLUDES))

BOS_SRCS := LinuxUser/bosCritSectLinuxUser.c \
              LinuxUser/bosErrorLinuxUser.c \
              LinuxUser/bosEventLinuxUser.c \
              LinuxUser/bosFileLinuxUser.c \
              LinuxUser/bosMsgQLinuxUser.c \
              LinuxUser/bosMutexLinuxUser.c \
              LinuxUser/bosSemLinuxUser.c \
              LinuxUser/bosSleepLinuxUser.c \
              LinuxUser/bosSocketLinuxUser.c \
              LinuxUser/bosTaskLinuxUser.c \
              LinuxUser/bosTimeLinuxUser.c \
              LinuxUser/bosTimerLinuxUser.c \
              generic/bosCritSectGeneric.c \
              generic/bosErrorGeneric.c \
              generic/bosEventGeneric.c \
              generic/bosFileGeneric.c \
              generic/bosInitGeneric.c \
              generic/bosIpAddrGeneric.c \
              generic/bosMsgQGeneric.c \
              generic/bosMutexGeneric.c \
              generic/bosSemGeneric.c \
              generic/bosSleepGeneric.c \
              generic/bosSocketGeneric.c \
              generic/bosTaskGeneric.c \
              generic/bosTimeGeneric.c \
              generic/bosTimerGeneric.c

LOCAL_INCLUDES := privateInc publicInc LinuxUser

# build defines
BOS_DEFS := $(addprefix -I$(BOS_ROOT)/,$(LOCAL_INCLUDES)) -DBUILDING_BOS
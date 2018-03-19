
# Openwrt pre-init LXC script

# If we previously started the container and it crashed, it may have a lot of
# things mounted which need to go away. Unmount everything that's not required.
umount /dev/pts 2>&1 > /dev/null || true

cd $LXC_ROOTFS_PATH

# Make the minimal set of devices
mkdir -p $LXC_ROOTFS_PATH/dev
mknod $LXC_ROOTFS_PATH/dev/null c 1 3
mknod $LXC_ROOTFS_PATH/dev/zero c 1 5
mknod $LXC_ROOTFS_PATH/dev/console c 5 1 
mknod $LXC_ROOTFS_PATH/dev/tty c 5 0
mknod $LXC_ROOTFS_PATH/dev/tty0 c 4 0
mknod $LXC_ROOTFS_PATH/dev/tty1 c 4 1
mknod $LXC_ROOTFS_PATH/dev/tty5 c 4 5
mknod $LXC_ROOTFS_PATH/dev/random c 1 8
mknod $LXC_ROOTFS_PATH/dev/urandom c 1 9
mknod $LXC_ROOTFS_PATH/dev/ram0 b 1 0


# Create /dev/pts if missing
mkdir -p $LXC_ROOTFS_PATH/dev/pts

# LXC 1.0.6 would create pty using host's devpts system in lxc_create_tty().
# The created pts would be bind-mounted to /dev/ttyN in the container.
mount -t devpts -o newinstance -o ptmxmode=0666 devpts /dev/pts
rm /dev/ptmx
ln -s pts/ptmx /dev/ptmx
chmod 666 /dev/pts/ptmx
chmod 666 $LXC_ROOTFS_PATH/dev/tty $LXC_ROOTFS_PATH/dev/console $LXC_ROOTFS_PATH/dev/ram0 $LXC_ROOTFS_PATH/dev/tty0 $LXC_ROOTFS_PATH/dev/null
chmod 777 $LXC_ROOTFS_PATH/dev


# Make sure proc and sysfs dirs are available
mkdir -p $LXC_ROOTFS_PATH/proc $LXC_ROOTFS_PATH/sys
mkdir -p $LXC_ROOTFS_PATH/sys/fs/cgroup

mount -t tmpfs tmpfs $LXC_ROOTFS_PATH/var
chmod 777 $LXC_ROOTFS_PATH/var


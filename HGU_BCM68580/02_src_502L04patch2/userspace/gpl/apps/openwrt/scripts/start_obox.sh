#!/bin/sh

mkdir -p /local/busgate/openwrt

#Get unique username
lxc_id_map_file="/local/busgate/openwrt/id_map"
if [ -f "$lxc_id_map_file" ]
then
	uniqueName=`awk 'NR==1{print $1}' ${lxc_id_map_file}`
	echo "unique user name(from ${lxc_id_map_file}) = $uniqueName"
else
	dbusGetUniqueName=`dbus-send --system --type=method_call --print-reply --dest=com.broadcom.spd /com/broadcom/busgate com.broadcom.busgate.GetUniqueUsername string:obox`
	uniqueName=${dbusGetUniqueName#*string \"}
	uniqueName=${uniqueName:0:32}
	echo "unique user name(from busgate) = $uniqueName"
	echo "${uniqueName} 0 1000">> ${lxc_id_map_file}
fi


#Prepare uid/gid for unpriviledged container
adduser -H -D -u 1000 $uniqueName
chmod 777 /tmp

#Prepare secret key and manifest file for BEEP bus gate service
cp /var/busgate/busgatekey /local/busgate/openwrt/busgatekey
echo -e '{\n"username": "'$uniqueName'",' > /local/busgate/openwrt/busgateconf
cat /opt/openwrt/config/busgateconf >> /local/busgate/openwrt/busgateconf
chmod 777 /local/busgate/openwrt

#Prepare rootfs using overlay filesystem 
mkdir -p /local/openwrt 
cd /local/openwrt 
mkdir -p pivot_old; chown $uniqueName:$uniqueName pivot_old 
mkdir -p upper; chown $uniqueName:$uniqueName upper 
mkdir -p work; chown $uniqueName:$uniqueName work 
mkdir -p merged_rootfs 

mount -t overlay overlay -o lowerdir=/opt/openwrt/rootfs,upperdir=./upper,workdir=./work ./merged_rootfs
chown $uniqueName:$uniqueName merged_rootfs
chown $uniqueName:$uniqueName /local/openwrt/merged_rootfs/tmp
chown $uniqueName:$uniqueName /local/openwrt/work/work

/bin/lxc-create -t none -n obox -f /opt/openwrt/config/lxc_obox.conf
/bin/lxc-start -n obox -d & 


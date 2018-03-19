#
#  Copyright 2011-2012, Broadcom Corporation
#

#!/bin/sh

echo "======Start openvswitch======"

mkdir /var/log/openvswitch
mkdir /var/run/openvswitch
mkdir /var/openvswitch
insmod /opt/modules/openvswitch/openvswitch.ko
/bin/ovsdb-tool create /var/openvswitch/conf.db /etc/openvswitch/openvswitch/vswitch.ovsschema
/bin/ovsdb-server /var/openvswitch/conf.db --remote=punix:/var/run/openvswitch/db.sock --pidfile --detach --log-file
/bin/ovs-vsctl --no-wait init
/bin/ovs-vswitchd --pidfile --detach unix:/var/run/openvswitch/db.sock --log-file 

ethswctl -c hw-switching -o disable

/bin/ovs-vsctl add-br brsdn


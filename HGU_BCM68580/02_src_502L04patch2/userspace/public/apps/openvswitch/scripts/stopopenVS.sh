#
#  Copyright 2011-2012, Broadcom Corporation
#

#!/bin/sh

echo "======shutdown openvswitch======"

/bin/ovs-vsctl del-br brsdn

/bin/ovs-appctl -t ovs-vswitchd exit
/bin/ovs-appctl -t ovsdb-server exit

rmmod openvswitch

rm -r /var/log/openvswitch
rm -r /var/run/openvswitch
rm -r /var/openvswitch

ethswctl -c hw-switching -o enable



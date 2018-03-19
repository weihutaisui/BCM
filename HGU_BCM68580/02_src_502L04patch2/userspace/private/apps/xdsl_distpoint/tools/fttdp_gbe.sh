#!/bin/sh

# Bash function to configure a single dsl line (sid number ${IFNUM})
add_line()
{
# tagged traffic with VLAN will be routed to interface id IFNUM
let VLAN=256+${IFNUM}

# map interface id to Oren internal sid numbering which is different from
# the physical sid numbering
case ${IFNUM} in
    0) let SID=6 ;;
    1) let SID=7 ;;
    2) let SID=8 ;;
    3) let SID=3 ;;
    4) let SID=4 ;;
    5) let SID=5 ;;
    6) let SID=0 ;;
    7) let SID=1 ;;
    8) let SID=2 ;;
    9) let SID=9 ;;
    10) let SID=10 ;;
    11) let SID=11 ;;
    *) echo "Interface number is out of range."
       exit 2
       ;;
esac
echo "Adding physical interface ${IFNUM} - VLAN ${VLAN} - internal sid${SID}"

# Create bridge per SID/filtered-virtual-interface
brctl addbr bronu${IFNUM}

# sid ${SID} vlanctl interface - single tagged vid ${VID}
ifconfig sid${SID} up
vlanctl --if-create sid${SID} 99
vlanctl --rx --tags 0 --if sid${SID} --default-miss-drop
vlanctl --rx --tags 1 --if sid${SID} --default-miss-drop
vlanctl --rx --tags 2 --if sid${SID} --default-miss-drop
vlanctl --tx --tags 0 --if sid${SID} --default-miss-drop
vlanctl --tx --tags 1 --if sid${SID} --default-miss-drop
vlanctl --tx --tags 2 --if sid${SID} --default-miss-drop
vlanctl --rx --tags 1 --if sid${SID} --filter-vid ${VLAN} 0 --set-rxif sid${SID}.99 --rule-append
vlanctl --tx --tags 1 --if sid${SID} --filter-vid ${VLAN} 0 --rule-append
ifconfig sid${SID}.99 up
brctl addif bronu${IFNUM} sid${SID}.99

vlanctl --if-create-name eth0 eth0.${IFNUM}
# TM mapping
# DS VID + PBIT -> SID + QUEUE
vlanctl --rx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 0 0 --set-skb-mark-queue 0 --set-rxif eth0.${IFNUM} --rule-append
vlanctl --rx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 1 0 --set-skb-mark-queue 1 --set-rxif eth0.${IFNUM} --rule-append
vlanctl --rx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 2 0 --set-skb-mark-queue 2 --set-rxif eth0.${IFNUM} --rule-append
vlanctl --rx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 3 0 --set-skb-mark-queue 3 --set-rxif eth0.${IFNUM} --rule-append
# US VID + PBIT + SID -> QUEUE
vlanctl --tx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 0 0 --filter-txif eth0.${IFNUM} --set-skb-mark-queue 0 --rule-append
vlanctl --tx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 1 0 --filter-txif eth0.${IFNUM} --set-skb-mark-queue 1 --rule-append
vlanctl --tx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 2 0 --filter-txif eth0.${IFNUM} --set-skb-mark-queue 2 --rule-append
vlanctl --tx --tags 1 --if eth0 --filter-vid ${VLAN} 0 --filter-pbits 3 0 --filter-txif eth0.${IFNUM} --set-skb-mark-queue 3 --rule-append
ifconfig eth0.${IFNUM} up
brctl addif bronu${IFNUM} eth0.${IFNUM}

# <flows push accelerated US/DS rules to runner>
vlanctl --create-flows eth0.${IFNUM} sid${SID}.99
vlanctl --create-flows sid${SID}.99 eth0.${IFNUM}
# </flows
}

case "$1" in
    start)
        if [ "`pspctl dump RdpaWanType`" == "GBE" ]; then
            for sid in 0 1 2 3 4 5 6 7 8 9 10 11;
            do
                brctl delif br0 sid$sid
            done
            brctl delif br0 eth0

            bs /b/n ingress_class/dir=ds,index=1,cfg={type=flow,fields=vlan_num}
            bs /bdmf/attr/add ingress_class/dir=ds,index=1 flow string key={vlan_num=0},result={action=host,forw_mode=flow};

            # dummy device so we can set miss to drop on all interfaces
            vlanctl --if-create-name eth0 eth0.99
            vlanctl --rx --tags 0 --if eth0 --default-miss-drop
            vlanctl --rx --tags 1 --if eth0 --default-miss-drop
            vlanctl --rx --tags 2 --if eth0 --default-miss-drop
            vlanctl --tx --tags 0 --if eth0 --default-miss-drop
            vlanctl --tx --tags 1 --if eth0 --default-miss-drop
            vlanctl --tx --tags 2 --if eth0 --default-miss-drop

            for IFNUM in 0 1 2 3 4 5 6 7 8 9 10 11;
            do
                add_line
            done

            vlanctl --rx --tags 0 --if eth0 --set-skb-mark-queue 0 --set-rxif eth0.99 --rule-append
            vlanctl --tx --tags 0 --if eth0 --filter-txif eth0.99 --set-skb-mark-queue 0 --rule-append
            ifconfig eth0.99 up
            brctl addif br0 eth0.99

        fi
        ;;

    *)
        echo "$0: unrecognized option $1"
        exit 1
        ;;

esac

#!/bin/sh
#
# QoS script example for FTTDP:
# US SP 4 shared queues on 1 tcont
# DS 4 queues per SID
# Mapping:
# DS: GEM + VID + PBIT to SID, QUEUE
# US: SID + VID + PBIT to GEM, QUEUE
#
#
# Useful debug commands in RDPA level:
# bs /b/c system cfg={ic_dbg_stats=yes} # enable statistics for ingress_class object
# bs /b/e ingress_class max_prints:-1 # examine accelerated flows in Runner
# bs /b/e egress_tm max_prints:-1 # examine TM queue configuration in Runner
#
# vlanctl --tx --tags 1 --if sid0 --show-table # print vlanctl configuration on sid0 interface (rx/single tagged)
#



# Bash function to configure a single dsl line (sid number ${IFNUM})
add_line()
{
echo Adding line ${IFNUM}
let VLAN=256+${IFNUM}
let GEM_IDX=10+${IFNUM}

# <gem - port ${IFNUM}/index ${GEM_IDX} (should be configured in OLT)>
bs /bdmf/new gem/index=${GEM_IDX},flow_type=ethernet,gem_port=${IFNUM},us_cfg={tcont={tcont/index=1}},ds_cfg={discard_prty=low,destination=eth}
bs /b/c gpon gem_ds_cfg[{gem/index=${GEM_IDX}}]={port=${IFNUM},discard_prty=low,encryption=no}
# </gem>

# Create bridge per SID/filtered-gpon-virtual-interface
brctl addbr bronu${IFNUM}

# sid ${IFNUM} vlanctl interface - single tagged vid ${VID}
ifconfig sid${IFNUM} up
vlanctl --if-create sid${IFNUM} 99
vlanctl --rx --tags 0 --if sid${IFNUM} --default-miss-drop
vlanctl --rx --tags 1 --if sid${IFNUM} --default-miss-drop
vlanctl --rx --tags 2 --if sid${IFNUM} --default-miss-drop
vlanctl --tx --tags 0 --if sid${IFNUM} --default-miss-drop
vlanctl --tx --tags 1 --if sid${IFNUM} --default-miss-drop
vlanctl --tx --tags 2 --if sid${IFNUM} --default-miss-drop
vlanctl --rx --tags 1 --if sid${IFNUM} --filter-vid ${VLAN} 0 --set-rxif sid${IFNUM}.99 --rule-append
vlanctl --tx --tags 1 --if sid${IFNUM} --filter-vid ${VLAN} 0 --rule-append
ifconfig sid${IFNUM}.99 up
brctl addif bronu${IFNUM} sid${IFNUM}.99

# Configure new gpon interface
vlanctl --if-create-name gpondef gpon${IFNUM}
# TM mapping
# DS VID + PBIT + GEM -> SID + QUEUE
vlanctl --rx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 0 0 --filter-skb-mark-port ${GEM_IDX} --set-skb-mark-queue 0 --set-rxif gpon${IFNUM} --rule-append
vlanctl --rx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 1 0 --filter-skb-mark-port ${GEM_IDX} --set-skb-mark-queue 1 --set-rxif gpon${IFNUM} --rule-append
vlanctl --rx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 2 0 --filter-skb-mark-port ${GEM_IDX} --set-skb-mark-queue 2 --set-rxif gpon${IFNUM} --rule-append
vlanctl --rx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 3 0 --filter-skb-mark-port ${GEM_IDX} --set-skb-mark-queue 3 --set-rxif gpon${IFNUM} --rule-append
# US VID + PBIT + SID -> GEM + QUEUE
vlanctl --tx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 0 0 --filter-txif gpon${IFNUM} --set-skb-mark-queue 0 --set-skb-mark-port ${GEM_IDX} --rule-append
vlanctl --tx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 1 0 --filter-txif gpon${IFNUM} --set-skb-mark-queue 1 --set-skb-mark-port ${GEM_IDX} --rule-append
vlanctl --tx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 2 0 --filter-txif gpon${IFNUM} --set-skb-mark-queue 2 --set-skb-mark-port ${GEM_IDX} --rule-append
vlanctl --tx --tags 1 --if gpondef --filter-vid ${VLAN} 0 --filter-pbits 3 0 --filter-txif gpon${IFNUM} --set-skb-mark-queue 3 --set-skb-mark-port ${GEM_IDX} --rule-append
ifconfig gpon${IFNUM} up
brctl addif bronu${IFNUM} gpon${IFNUM}

# <flows push accelerated US/DS rules to runner>
vlanctl --create-flows gpon${IFNUM} sid${IFNUM}.99
vlanctl --create-flows sid${IFNUM}.99 gpon${IFNUM}
# </flows
}


case "$1" in
    start)
        if [ "`pspctl dump RdpaWanType`" == "GPON" ]; then
            # Basic GPON configuration
            # <tcont with allocid 256 (should be the same in OLT)>
            bs /bdmf/new tcont/index=1
            bs /b/c gpon tcont_alloc_id[{tcont/index=1}]=256
            # <tcont>

            # <scheduling>
            # Create Strict Priority root scheduler for multi-level US queues
            bs /bdmf/new egress_tm/dir=us,index=44,level=egress_tm,mode=sp tcont/index=1
            # Create 4 US queues to be shared among all lines
            bs /bdmf/new egress_tm/dir=us,index=1,level=queue,mode=sp,queue_cfg[0]={drop_threshold=128,queue_id=3} egress_tm/dir=us,index=44
            bs /bdmf/new egress_tm/dir=us,index=2,level=queue,mode=sp,queue_cfg[0]={drop_threshold=128,queue_id=2} egress_tm/dir=us,index=44
            bs /bdmf/new egress_tm/dir=us,index=3,level=queue,mode=sp,queue_cfg[0]={drop_threshold=128,queue_id=1} egress_tm/dir=us,index=44
            bs /bdmf/new egress_tm/dir=us,index=4,level=queue,mode=sp,queue_cfg[0]={drop_threshold=128,queue_id=0} egress_tm/dir=us,index=44
            # Attach individual queues to root
            bs /bdmf/configure egress_tm/dir=us,index=44 subsidiary[0]={egress_tm/dir=us,index=1}
            bs /bdmf/configure egress_tm/dir=us,index=44 subsidiary[1]={egress_tm/dir=us,index=2}
            bs /bdmf/configure egress_tm/dir=us,index=44 subsidiary[2]={egress_tm/dir=us,index=3}
            bs /bdmf/configure egress_tm/dir=us,index=44 subsidiary[3]={egress_tm/dir=us,index=4}
            # </scheduling>

            # 4 queues are by default present on each of the SID ports

            # <gpon vlanctl interface>
            gponif -c gpondef
            gponif -a gpondef -g 0
            ifconfig gpondef up
            # dummy device so we can set miss to drop on all interfaces
            vlanctl --if-create-name gpondef gpon99
            vlanctl --rx --tags 0 --if gpondef --default-miss-drop
            vlanctl --rx --tags 1 --if gpondef --default-miss-drop
            vlanctl --rx --tags 2 --if gpondef --default-miss-drop
            vlanctl --tx --tags 0 --if gpondef --default-miss-drop
            vlanctl --tx --tags 1 --if gpondef --default-miss-drop
            vlanctl --tx --tags 2 --if gpondef --default-miss-drop
            # </gpon>

            for sid in 0 1 2 3; do IFNUM=$sid; add_line; done
        fi
        ;;

    *)
        echo "$0: unrecognized option $1"
        exit 1
        ;;

esac

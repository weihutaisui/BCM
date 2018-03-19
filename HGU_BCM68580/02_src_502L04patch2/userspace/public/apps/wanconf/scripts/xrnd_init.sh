#!/bin/bash

BOARDTYPE=$(cat /proc/nvram/boardid)

if [[ "$BOARDTYPE" == "968580XRND" ]]
then

     echo Start XRND board init!!!
# Initialize bdmf shell
    bdmf_shell -c init | while read a b ; do echo $b ; done > /var/bdmf_sh_id
    alias bs="bdmf_shell -c `cat /var/bdmf_sh_id` -cmd "

# Instantiate the i2c device through bcmsfp_i2c driver
    echo bcmsfp 0x44 > /sys/bus/i2c/devices/i2c-0/new_device

    #Set GPIO's 60, 59, 53, 52, 50 direction as output (this are the MUX control GPIOs (60, 59, 53) and the 2 outputs going thru the mux (52, 50)
     sw 0xff800504 0x18340000

     SYSTEMMODE=$(/bin/pspctl dump RdpaWanType)
     SYSRATE=$(/bin/pspctl dump WanRate)

     case "$SYSTEMMODE" in
     'XGPON1')
     SIDX=0
     SIDXSEL=0
     echo xgpon1!!!
     ;;
     'XGS')
     SIDX=5
     SIDXSEL=11
     echo xgs!!!
     ;;
     'GPON')
     SIDX=10
     SIDXSEL=22
     echo gpon!!!
     ;;
     'EPON')
     if [[ "$SYSRATE" == "1010" ]]
     then
     SIDX=0
     SIDXSEL=0
     echo epon10G!!!
     else
     SIDX=10
     SIDXSEL=22
     echo epon!!!
     fi
     ;;
     'NGPON2')
     SIDX=15
     SIDXSEL=33
     echo ngpon2!!!
     ;;
     *)
     echo unsuported wan type!!!
     exit 0
     ;;
     esac

    # Mux 00, 01, 10, 11 respectively
    MUXSELECT='0x00440000 0x10440000 0x18440000 0x08440000'

    # I2C-Switch channel 7, 6, 5, 4 respectively
    I2CSELECT='0x80 0x40 0x10 0x20'

    # Mux the control IOs
    sw 0xff800524 "${MUXSELECT:$SIDXSEL:10}"     # register has been moved in B0

    # Select I2C-Switch channel
    echo "w ${I2CSELECT:$SIDX:4}" > /proc/i2c_mux/i2cmuxOper
    
    # Verify I2C-Switch channel write
#   echo "r" > /proc/i2c_mux/i2cmuxOper

    # Switch WAN Phy
    RX5TO0='0x01 0x21 0x01 0x01'
    RX11TO6='0x01 0x00 0x20 0x02' 
    SWITCHOUT10='0x01 0x10 0x11 0x11'
    SWITCHOUT32='0x11 0x11 0x11 0x11'
    SWITCHOUT54='0x11 0x11 0x11 0x11'
    SWITCHOUT76='0x11 0x11 0x11 0x10'
    SWITCHOUT98='0x11 0x11 0x11 0x11'
    SWITCHOUT1110='0x61 0x51 0xb0 0x71'
    TX7TO0='0x02 0x01 0x00 0x40'
    TX11TO8='0x08 0x08 0x0C 0x08'

    # Enable receivers
    echo "a 0x44 0x86 1 ${RX5TO0:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0x96 1 ${RX11TO6:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest

    # Enable equalizers
    echo "a 0x44 0x87 1 ${RX5TO0:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0x97 1 ${RX11TO6:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    
    # Switch to SMA - Write to "next state"
    echo "a 0x44 0xb0 1 ${SWITCHOUT10:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0xb1 1 ${SWITCHOUT32:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0xb2 1 ${SWITCHOUT54:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0xb3 1 ${SWITCHOUT76:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0xb4 1 ${SWITCHOUT98:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0xb5 1 ${SWITCHOUT1110:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    
    # XPT_Update - Write "next state" map to "current state" map (i.e. save changes to map)
    echo "a 0x44 0xdf 1 0x1" > /proc/i2c_bcmsfp_0/bcmsfpTest

    # Verify (read) "current state" map
#    echo "a 0x44 0xd0 1" > /proc/i2c_bcmsfp_0/bcmsfpTest
#    echo "a 0x44 0xd1 1" > /proc/i2c_bcmsfp_0/bcmsfpTest
#    echo "a 0x44 0xd2 1" > /proc/i2c_bcmsfp_0/bcmsfpTest
#    echo "a 0x44 0xd3 1" > /proc/i2c_bcmsfp_0/bcmsfpTest
#    echo "a 0x44 0xd4 1" > /proc/i2c_bcmsfp_0/bcmsfpTest
#    echo "a 0x44 0xd5 1" > /proc/i2c_bcmsfp_0/bcmsfpTest

    # Enable transmitters
    echo "a 0x44 0x02 1 ${TX7TO0:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest
    echo "a 0x44 0x03 1 ${TX11TO8:$SIDX:4}" > /proc/i2c_bcmsfp_0/bcmsfpTest

    #Invert polarity of input to sequoia (transmitter 11, register 0x0B [Tx11to8_Sign])
    echo "a 0x44 0x0b 1 0x08" > /proc/i2c_bcmsfp_0/bcmsfpTest

    if [[ $SIDX == 0 ]]
    then
        echo innn!!!!
        #Invert polarity of input to xgpon cage (transmitter 1, register 0x0A [Tx7to0_Sign])
        echo "a 0x44 0x0a 1 0x02" > /proc/i2c_bcmsfp_0/bcmsfpTest
    fi
    sw ff800028   8   # Kick the opticaldet module by simulating TRX Inserted ext interrupt
    echo XRND board trx configuration completed!!!
fi

exit 0





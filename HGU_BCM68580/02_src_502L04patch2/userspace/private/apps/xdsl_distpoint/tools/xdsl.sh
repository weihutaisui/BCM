#!/bin/sh
#
# sdk deamon
#
# The scratchpad is used to control some startup options of the xdsl_distpoint
# sdk. See /etc/xdsl/howto.txt for more details.
#

model="`cat /proc/nvram/boardid`"
sdk="xdslctl"
options=""
dir="/etc/xdsl"

# {{{ get_key
# Retrieve key from sratchpad, in case key has not been set return a default
# value.
get_key() {
    key_name=$1
    key_default=$2
    key_value="`pspctl dump "$key_name"`"
    if [ $? -eq 255 ] ; then
        echo "$key_default"
    else
        echo "$key_value"
    fi
}
# }}}

# {{{ set_vectoring_option
set_vectoring_option() {
    board_cfg=$1
    vectoring=$(get_key xd_vectoring disable)
    case "$vectoring" in
        enable)
            options="$options -x"
	    if [ ! -z "$board_cfg" ]; then
		options="$options -B $board_cfg"
	    fi
            ;;
        disable)
            ;;
        *)
            echo "unknown value for scratchpad key xd_vectoring: $vectoring"
            echo "valid values are: [enable | disable]"
            exit 1
            ;;
    esac
}
# }}}

# {{{ get_inband_options
get_inband_options() {
    inband=$(get_key xd_inband all)
    yst_traffic_interface=$(get_key xd_traf_itf 10G)
    if [ $yst_traffic_interface == "RGMII" ]; then
	inband="none"
    fi
    case "$inband" in
        yes|all)
            echo "-r0:0"
            ;;
        no|none)
            echo "-r0:1"
	    inband="none"
            ;;
        boot)
            echo "-r200:200"
            ;;
        download)
            echo "-rFFFF:201"
            ;;
        fw)
            echo "-r200:0"
            ;;
	*)
            echo "unknown value for scratchpad key xd_inband: $inband"
            echo "valid values are: [all | none | boot | download | fw]"
            exit 1
            ;;
    esac
    #remove existing vlan as they break inband traffic
    if [ $inband != "none" ]; then
        ifconfig eth4.0 >& /dev/null
        if [ $? -eq 0 ]; then
            vlanctl --if-delete eth4.0
        fi
        ifconfig eth4.1 >& /dev/null
        if [ $? -eq 0 ]; then
            vlanctl --if-delete eth4.1
        fi
    fi
}
# }}}

# {{{ set_booter_options
set_booter_options() {
    booter=$1
    shift
    ext=$*
    booter_options=""
    for e in $ext; do
	if [ -f $dir/${booter}_$e.bin ]; then
	    booter_options="$booter_options -b $dir/${booter}_$e.bin"
	fi
    done
    if [ -z "$booter_options" ]; then
        echo "no $booter available for those extensions $ext"
        exit 1
    fi
    options="$options $booter_options"
}
# }}}

# {{{ set_965400X_booter_options
set_965400X_booter_options() {
    itf=$(get_key xd_traf_itf 10G)
    if [ $itf == "RGMII" ]; then
	booter="booter_Mode0_2xRGMII_400X"
    else 
	booter="booter_Mode*${itf}"
	if [ $itf != "1G" ]; then
            booter="${booter}_4x2G5"
	fi
	mode=$(get_key xd_traf_mode G.999.1)
    	case "$mode" in
    	    raw_ethernet)
    	        booter="${booter}_stdeth_400X"
    	        ;;
    	    G.999.1)
    	        booter="${booter}_g9991_unencapsulated_400X"
    	        ;;
    	esac
    fi
    booter_options=""
    for e in f8yco f8zco; do
	if [ -f $dir/${booter}_$e.bin ]; then
	    booter_options="$booter_options -b $dir/${booter}_$e.bin"
	fi
    done
    if [ -z "$booter_options" ]; then
        echo "no $booter available (selected from combination of scratchpad keys)"
        echo "xd_traf_itf and xd_traf_mode: $itf $mode"
        echo "valid values are:"
        echo "      xd_traf_itf [RGMII | 1G | 10G | 10G_xfi | 10G_sfi | 10G_sfi_long]"
        echo "      xd_traf_mode [G.999.1 | raw_ethernet]"
        exit 1
    fi
    options="$options $booter_options -p eth4"
}
# }}}

# {{{ set_fw_options
set_fw_options() {
    if [ $# -eq 0 ]; then return 0; fi
    ext=$*
    fw_options=""
    for e in $ext; 
    do
	if [ -f $dir/modem_$e.bin ]; then
	    fw_options="$fw_options -t $dir/modem_$e.bin"
	fi
    done
    if [ -z "$fw_options" ]; then
        echo "no fw available for those extensions $ext"
        exit 1
    fi
    options="$options $fw_options"
}
# }}}

# {{{ set_sdk_options: set board model dependent startup options
set_sdk_options() {
    case "$model" in
        # {{{ FCOPE/DPF2 CPE boards
        965200F_CPE|965200DPF2_CPE)
            afe=$(get_key xd_afe g.fast)
            if [ $model == 965200F_CPE ]; then
                sdk="xdslctl_fcope_cpe"
                booter="booter_SGMII_1G_RGMII_FCOPE*"
            else
                sdk="xdslctl_dpf2_cpe"
                booter="booter_SGMII_1G_RGMII_DPF2*"
            fi
            case "$afe" in
                g.fast)
                    fwext="f4kcpe f4ccpe"
                    ;;
                vdsl)
                    fwext="v12kcpe"
                    ;;
                *)
                    echo "unknown value for scratchpad key xd_afe: $afe"
                    echo "valid values are: [g.fast | vdsl]"
                    exit 1
                    ;;
            esac
	    set_fw_options $fwext
	    set_booter_options "$booter" v36kco old
            bonding=$(get_key xd_bonding disable)
            case "$afe-$bonding" in
                g.fast-enable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_gfast_cpe_bonding.txt"
                    ;;
                g.fast-disable)
                    ;;
                vdsl-enable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_vdsl_cpe_bonding.txt"
                    ;;
                vdsl-disable)
                    ;;
                *)
                    echo "unknown value for scratchpad key xd_bonding: $bonding"
                    echo "valid values are: [enable | disable]"
                    exit 1
                    ;;
            esac
            ;;
        # }}}
        # {{{ FCOPE/DPF2 CO boards
        965200F_CO|965200DPF2_CO)
            if [ $model == 965200F_CO ]; then
                sdk=xdslctl_fcope
                booter="booter_SGMII_1G_RGMII_FCOPE*"
            else
                sdk=xdslctl_dpf2
                booter="booter_SGMII_1G_RGMII_DPF2*"
            fi
            afe=$(get_key xd_afe g.fast)
            case "$afe" in
                g.fast)
                    fwext=f6kco
                    ;;
                vdsl)
                    sdk="${sdk}_vdsl"
                    fwect=v24kco
                    ;;
                *)
                    echo "unknown value for scratchpad key xd_afe: $afe"
                    echo "valid values are: [g.fast | vdsl]"
                    exit 1
                    ;;
            esac
            set_vectoring_option
	    set_fw_options $fwext
	    set_booter_options "$booter" v36kco old
            bonding=$(get_key xd_bonding disable)
            case "$afe-$bonding" in
                g.fast-enable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_gfast_co_bonding.txt"
                    ;;
                g.fast-disable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_co.txt"
                    ;;
                vdsl-enable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_vdsl_co_bonding.txt"
                    ;;
                vdsl-disable)
                    options="$options -f $dir/xdsl_cmd_script_fcope_co.txt"
                    ;;
                *)
                    echo "unknown value for scratchpad key xd_bonding: $bonding"
                    echo "valid values are: [enable | disable]"
                    exit 1
                    ;;
            esac
            ;;
        # }}}
        # {{{ 400X CO boards
        965400X_CO)
	    cpld_cfg=`pspctl dump xd_cpld_cfg`
	    if [ $? -eq 255 ] ; then
		echo "leaving cpld config as is"
		echo "" > /tmp/xdsl_cmd_script.txt
	    else
		echo "source $dir/cpld_965400x_cfg"$cpld_cfg".txt" > /tmp/xdsl_cmd_script.txt
	    fi
	    board_cfg=""
	    afe=$(get_key xd_afe g.fast)
	    case "$afe" in
		g.fast)
		    sdk=xdslctl_400x
		    echo "source $dir/xdsl_cmd_script_965400x_gfast_co.txt" >> /tmp/xdsl_cmd_script.txt
		    fwext="f4yco f4zco"
		    board_cfg=BCM65400_4L_BLV_BC 
		    ;;
		vdsl)
		    sdk=xdslctl_400x_vdsl
		    echo "source $dir/xdsl_cmd_script_965400x_vdsl_co.txt" >> /tmp/xdsl_cmd_script.txt
		    fwext="f8yco f8zco"
		    ;;
		*)
		    echo "unknown value for scratchpad key xd_afe: $afe"
		    echo "valid values are: [g.fast | vdsl]"
		    exit 1
		    ;;
	    esac
	    set_vectoring_option $board_cfg
	    set_fw_options $fwext
	    set_965400X_booter_options
	    options="$options $(get_inband_options) -f /tmp/xdsl_cmd_script.txt"
	    ;;
        # }}}
        # {{{ 400X CPE boards
	965400X_CPE)
	    sdk=xdslctl_400x_cpe
	    afe=$(get_key xd_afe g.fast)
	    case "$afe" in
		g.fast)
		    fwext="f4ycpe f4zcpe"
		    ;;
		vdsl)
		    fwext="f8ycpe f8zcpe"
		    ;;
		*)
		    echo "unknown value for scratchpad key xd_afe: $afe"
		    echo "valid values are: [g.fast | vdsl]"
		    exit 1
		    ;;
	    esac
	    set_fw_options $fwext
	    set_965400X_booter_options
	    options="$options $(get_inband_options) -f $dir/xdsl_cmd_start_all.txt"
	    ;;
        # }}}
     # {{{ 55045 boards
    BCM955045DPU)
	    sdk=xdslctl_955045dpu
		echo "" > /tmp/xdsl_cmd_script.txt
		echo "api cfg apply all" >> /tmp/xdsl_cmd_script.txt
		echo "api startline all" >> /tmp/xdsl_cmd_script.txt
	    set_vectoring_option $board_cfg
	    set_fw_options f4zco
        options="$options -b $dir/booter.bin"
	    options="$options -f /tmp/xdsl_cmd_script.txt"
	    ;;
        # }}}
     # {{{ DPF boards
	965200DPF)
	    echo "Setting up external phys 54220..."
            # 54220 BOOT_CONTROL_1 Register, set 125MHz differential clock input
	    ethctl phy ext 0x13 0x1e 0x819 no_read_back
	    ethctl phy ext 0x13 0x1f 0x680 no_read_back
	    sleep 1
            # enter/exit 54220 reset to activate new BOOT_CONTROL_1 settings.
	    echo "reset enter BCM54220_RST_N" > /proc/xdsl_distpoint
	    sleep 1
	    echo "reset exit BCM54220_RST_N" > /proc/xdsl_distpoint
	    sleep 1
            # enable SGMII-copper interface on both phys of 54220
	    ethctl phy ext 0x13 0x1e 0x021
	    ethctl phy ext 0x13 0x1f 0x5
	    ethctl phy ext 0x14 0x1e 0x021
	    ethctl phy ext 0x14 0x1f 0x5

	    echo "bring up eth3 and eth4"
	    ifconfig eth3 up
	    ifconfig eth4 up

	    echo "removing GPHY1, GPHY2, GPHY3 from br0"
	    brctl delif br0 eth1.0
	    brctl delif br0 eth2.0
	    brctl delif br0 eth3.0
	    vectoring=$(get_key xd_vectoring disable)
	    booter="booter_*_DPF_*SGMII2"
	    sdk=xdslctl_dpf
	    case "$vectoring" in
		vce_fast_internal|enable)
		    sdk=${sdk}_evce
		    options="$options -x -f $dir/xdsl_cmd_script_965200dpf.txt"
		    fwext=f6kco
		    ;;
		vce_fast_external)
		    options="$options -t none -f $dir/xdsl_cmd_script_disable_coredump.txt"
		    booter="${booter}_inband"
		    ;;
		disable)
		    options="$options -f $dir/xdsl_cmd_script_965200dpf.txt"
		    fwext=f6kco
		    ;;
		*)
		    echo "unknown value for scratchpad key xd_vectoring: $vectoring"
		    echo "valid values are: [vce_fast_internal | vce_fast_external | enable | disable]"
		    exit 1
		    ;;
	    esac
	    set_fw_options $fwext
	    set_booter_options "$booter" v36kco old
	    ;;
        # }}}
        # {{{ FTTDP boards
	968380FTTDPS|968380DP2)
	    if [ $model == 968380FTTDPS ]; then
		sdk="xdslctl_fttdp1"
	    else
		sdk="xdslctl_fttdp2"
	    fi
	    echo "Setting up external GPHY ..."
	    ethctl phy ext 0x01 0x1c 0x8c00
	    ifconfig dsp0 up
	    set_vectoring_option
	    set_fw_options v8sco v16sco
	    options="$options -h e0:03:03:02:01:07 -p dsp0 -f $dir/xdsl_cmd_script.txt"
	    ;;
        # }}}
    esac

    if [ ! -f /bin/$sdk ]; then
	echo "Board $model not supported by this flash ($sdk not found)"
	exit 1
    fi
    options="$options -v $(get_key xd_log_level 3)"
}
# }}}

case "$1" in
    start)
	set_sdk_options
        echo "$model - starting sdk deamon ..."
        echo $sdk $options
        telnetd4cli 57660 $sdk $options &

        # debug port is not considered a regular port so here we add it to br0
        # so the board can be accessible for debug purposes
        ifconfig eth0 down
#        ifconfig eth0 up
#        brctl addif br0 eth0
        exit 0
        ;;

    stop)
        if [ `type pidof > /dev/null;echo $?` -eq 0 ] ; then
            pid=`pidof telnetd4cli`
            if [ $? -eq 0 ] ; then
                echo "Stoping xdsl sdk deamon."
                kill $pid
            else
                echo "xdsl sdk deamon was not running, nothing to stop"
            fi
            exit 0
        else
            echo "pidof applet is not available, failed to stop"
            exit 1
        fi
        ;;

    *)
        echo "$0: unrecognized option $1"
        exit 1
        ;;

esac

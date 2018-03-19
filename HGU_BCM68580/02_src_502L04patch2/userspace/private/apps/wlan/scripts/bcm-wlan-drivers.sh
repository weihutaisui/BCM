#!/bin/sh
# This script loads all the loadable kernel modules for Wireless feature for non-HND images
# use modules list from nvram, if not specified use default list
# check existance of driver in the system before loading

trap "" 2

#The following will be populated by buildFS during the make process:
KERNELVER=_set_by_buildFS_
HNDROUTER=_set_by_buildFS_
CPEROUTER=_set_by_buildFS_
PROD_FW_PATH=_set_by_buildFS_
if [ ! -z $PROD_FW_PATH ]; then
    MFG_FW_PATH=$PROD_FW_PATH"/mfg"
else
    MFG_FW_PATH="/etc/wlan/dhd/mfg"
fi
case "$1" in
    start)
        # do nothing for hnd only builds (HNDROUTER=y and CPEROUTER=)
        if [ ! -z $HNDROUTER ]; then
            if [ -z $CPEROUTER ]; then
                echo "Skipping wlan-drivers.sh for HND images ..."
                exit 0
            fi
        fi

        # Sanity check for drivers directory
        if [ ! -d /lib/modules/$KERNELVER/extra ]; then
            echo "ERROR: wlan-drivers.sh: /lib/modules/$KERNELVER/extra does not exist" 1>&2
            exit 1
        fi

        # set the default module list and parameters
        # check whether the module exists or not before adding
        # set the module parameters based on hnd (cperouter) or wlemf (legacy)
        if [ ! -z $CPEROUTER ]; then
            #CPE Router
            dhd_module_params="iface_name=wl dhd_console_ms=0"
            wl_module_params="intf_name=wl%d"
            all_wlan_modules="hnd emf igs dhd wl"
            if [ -f /proc/nvram/wl_nand_manufacturer ]; then
                is_mfg=`cat /proc/nvram/wl_nand_manufacturer`
                case $is_mfg in
                    2|3|6|7)
                        dhd_module_params="iface_name=wl dhd_console_ms=0 firmware_path=$MFG_FW_PATH"
                        ;;
                    *)
                        ;;
                esac
            fi
        else
            #Legacy
            dhd_module_params="iface_name=wl dhd_console_ms=0 firmware_path=/etc/wlan/dhd mfg_firmware_path=/etc/wlan/dhd/mfg  dhd_dpc_prio=5"
            wl_module_params="intf_name=wl%d"
            all_wlan_modules="wlemf dhd wl"
        fi

        # Update the wlan module list from nvram if exists
        is_nvmodules_list=`nvram show 2>&1 |grep kernel_mods | grep -c '^'`
        if [ $is_nvmodules_list -eq '0' ]; then
            echo "no modules list in nvram, using defaults..."
            modules_list=$all_wlan_modules
        else
            modules_list=`nvram get kernel_mods`
        fi

        echo "loading WLAN kernel modules ... $modules_list"

        for module in $modules_list
        do
            case "$module" in
                wlemf|hnd|emf|igs)
                    #no module parameters
                    module_params=""
                    ;;
                wl)
                    module_params=$wl_module_params
                    ;;
                dhd)
                    module_params=$dhd_module_params
                    ;;
                *)
                    echo "wlan-drivers: unrecognized module [$module] in the load module list"
                    module_params=""
                    ;;
            esac

            if [ -e /lib/modules/$KERNELVER/extra/$module.ko ]; then
                insmod /lib/modules/$KERNELVER/extra/$module.ko $module_params
            fi
        done
        exit 0
        ;;

    stop)
        echo "removing WLAN kernel modules not implemented yet..."
        exit 1
        ;;

    *)
        echo "wlan-drivers: unrecognized option [$1]"
        exit 1
        ;;
esac

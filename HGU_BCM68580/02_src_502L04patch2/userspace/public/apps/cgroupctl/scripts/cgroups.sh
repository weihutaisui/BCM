#!/bin/sh

case "$1" in
	start)
		echo "Configuring cgroups..."
		/etc/cgroups.conf
		exit $?
		;;

	stop)
		echo "stop cgroups not implemented"
		exit 1
		;;

	*)
		echo "$0: unrecognized option $1"
		exit 1
		;;

esac


#!/bin/sh

case "$1" in
	start)
		echo "Starting fttdpctl ..."
		/bin/fttdpctl
		exit 0
		;;

	stop)
		echo "No stop for fttdpctl"
		exit 1
		;;

	*)
		echo "$0: unrecognized option $1"
		exit 1
		;;

esac


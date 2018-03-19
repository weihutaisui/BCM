#!/bin/sh

case "$1" in
	start)
		echo "Starting bcm_thrdBinder..."
		/bin/bcm_thrdBinder
		exit 0
		;;

	*)
		echo "$0: unrecognized option $1"
		exit 1
		;;

esac


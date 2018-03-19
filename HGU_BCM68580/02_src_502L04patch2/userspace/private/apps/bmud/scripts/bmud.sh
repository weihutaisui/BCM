#!/bin/sh

case "$1" in
	start)
		# echo "Starting bmud..."
		/bin/bmud &
		exit 0
		;;

	stop)
		echo "Stopping bmud..."
		echo "Not implemented yet..."
		exit 0
		;;

	*)
		echo "$0: unrecognized option $1"
		exit 1
		;;

esac


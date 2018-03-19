#!/bin/sh

dev_path="/dev/"$1
volname_path="/sys/class/"$2"/"$1"/volname"
link_path="/dev/"$(cat $volname_path)
ln -s $dev_path $link_path

#!/bin/sh
git clone -b chaos_calmer git://github.com/openwrt/openwrt.git
cd openwrt
git reset --hard 35769e8

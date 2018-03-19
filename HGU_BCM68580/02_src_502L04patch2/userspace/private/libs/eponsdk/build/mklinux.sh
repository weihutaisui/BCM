#!/bin/sh
SDK_BASE_DIR=$(cd ../;/bin/pwd)
TARGET_OS=LINUX

echo "start make $1 now."
echo $SDK_BASE_DIR
echo $TARGET_OS
echo "start making"
echo "make $1 now"

make $1 SDK_BASE=$SDK_BASE_DIR OS_TYPE=$TARGET_OS

echo "#############finished##########################"
echo "## the Lib file: SdkLib.a is in ../obj dir   ##"
echo "## if you have make the sdkApp then the file ##"
echo "## is in current dir.                        ##"
echo "###############################################"


## ----------------------------------
# Step #1: Define variables
# ----------------------------------
EDITOR=vim
PASSWD=/etc/passwd
RED='\033[0;41;30m'
STD='\033[0;0;39m'

KERNEL_DIR=$PWD
KERN_IMG=$KERNEL_DIR/arch/arm/boot/Image
DTBTOOL=$KERNEL_DIR/tools/dtbToolCM
BUILD_START=$(date +"%s")
green='\033[01;32m'
red='\033[01;31m'
cyan='\033[01;36m'
blue='\033[01;34m'
blink_red='\033[05;31m'
restore='\033[0m'
nocol='\033[0m'

# Device varibles (Modify this)
device='wt88047' # Device Id
base_version='octopus' # Kernel Id
version='testing' # Kernel Version

export USE_CCACHE=1
export ARCH=arm
export SUBARCH=arm
export KBUILD_BUILD_USER="octo"
export KBUILD_BUILD_HOST="joindet69"

MODULES_DIR=$KERNEL_DIR/OctoOutput

make octopus_defconfig
make -j2
$DTBTOOL -2 -o $KERNEL_DIR/arch/arm/boot/dt.img -s 2048 -p $KERNEL_DIR/scripts/dtc/ $KERNEL_DIR/arch/arm/boot/dts/

rm $MODULES_DIR/liboutput/tools/Image
	rm $MODULES_DIR/liboutput/tools/dt.img
	cp $KERNEL_DIR/arch/arm/boot/Image  $MODULES_DIR/liboutput/tools
	cp $KERNEL_DIR/arch/arm/boot/dt.img  $MODULES_DIR/liboutput/tools
	cd $MODULES_DIR/liboutput/
	zipfile="octopus-$version-$(date +"%Y-%m-%d(%I.%M%p)").zip"
	echo $zipfile
	zip -r $zipfile tools META-INF system -x *kernel/.gitignore*
	BUILD_END=$(date +"%s")
	DIFF=$(($BUILD_END - $BUILD_START))

 	echo "${green}"
 	echo "------------------------------------------"
 	echo "Build $version Completed :"
 	echo "------------------------------------------"
 	echo "${restore}"
	echo ${yellow}"Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds."${restore}
	echo "Finishing build for "$device

        exit 0
}

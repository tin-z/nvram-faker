#!/bin/sh

export ARCH=arm
export TARGET=$1

# Sets up toolchain environment variables for various mips toolchain

warn()
{
	echo "$1" >&2
}

tmp_z=$( which arm-linux-gnueabi-gcc )
if [ ! -z $tmp_z ];
then
	export CC=$tmp_z
else
	warn "Not setting CC: can't locate mipsel-linux-gcc."
fi

tmp_z=$( which arm-linux-gnueabi-ld )
if [ ! -z $tmp_z ];
then
	export LD=$tmp_z
else
	warn "Not setting LD: can't locate mipsel-linux-ld."
fi

tmp_z=$( which arm-linux-gnueabi-ar )
if [ ! -z $tmp_z ];
then
	export AR=$tmp_z
else
	warn "Not setting AR: can't locate mipsel-linux-ar."
fi

tmp_z=$( which arm-linux-gnueabi-strip )
if [ ! -z $tmp_z ];
then
	export STRIP=$tmp_z
else
	warn "Not setting STRIP: can't locate mipsel-linux-strip."
fi

tmp_z=$( which arm-linux-gnueabi-nm )
if [ ! -z $tmp_z ];
then
	export NM=$tmp_z
else
	warn "Not setting NM: can't lcoate mipsel-linux-nm."
fi

tmp_z=$( which arm-linux-gnueabi-as )
if [ ! -z $tmp_z ];
then
	export AS=$tmp_z
else
	warn "Not setting NM: can't lcoate mipsel-linux-nm."
fi

make $TARGET || exit $?


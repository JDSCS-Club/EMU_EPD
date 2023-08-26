################################################
#	Make Image : RFM
################################################

if [ -b $1 ]
then
APP_NAME=RFM
else
APP_NAME=$1
fi

# 200123
cdate=`date "+%y%m%d"`

#if [ $APP_NAME = "RFM" ]; then
VER_FILE=Library/version.h
#else
#VER_FILE=Src/version.h
#fi

ver_main=`grep "#define	APP_MAIN_VER" $VER_FILE | sed 's/[^0-9]//g'`
#ver_sub=`grep "#define	APP_SUB_VER" $VER_FILE  | sed 's/[^0-9]//g'`
#ver_det=`grep "#define	APP_DETAIL_VER" $VER_FILE  | sed 's/[^0-9]//g'`
ver_build=`grep "#define	APP_BUILD_VER" $VER_FILE  | sed 's/[^0-9]//g'`

#ver=$ver_main.$ver_sub.$ver_det.$ver_build
ver=$ver_main.$ver_build

#echo $ver_main.$ver_sub.$ver_det.$ver_build
#echo $ver
#echo $cdate

TARGET_PATH=../bin
WORK_PATH=`pwd`

#echo $WORK_PATH

################################################
#	Copy build/ -> bin/
mkdir -p ${TARGET_PATH}

IMG_NAME=${APP_NAME}_${cdate}_v${ver}

echo "./bin/${IMG_NAME}.hex"
echo "./bin/${IMG_NAME}.bin"

cp -fr ./build/${APP_NAME}.bin ${TARGET_PATH}/${IMG_NAME}.bin

if [ $APP_NAME != "bootloader" ]; then

################################################
#	create bootapp.hex

echo "bin/Boot${IMG_NAME}.hex"

#	Del last line. ( .hex file )
cat ./build/Bootloader.hex | sed '$d' > ${TARGET_PATH}/Boot${IMG_NAME}.hex
#	Append app.hex
cat ./build/${APP_NAME}.hex >> ${TARGET_PATH}/Boot${IMG_NAME}.hex

fi

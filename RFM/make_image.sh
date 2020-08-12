################################################
#	Make Image : RFM
################################################

if [ -b $1 ]
then
APP_NAME=RFM_F407_NOS
else
APP_NAME=$1
fi

# 200123
cdate=`date "+%y%m%d"`

if [ $APP_NAME = "RF_PA_407VG_BD" ]; then
VER_FILE=Core/Src/version.h
else
VER_FILE=Src/version.h
fi

ver_main=`grep "#define	APP_MAIN_VER" $VER_FILE | sed 's/[^0-9]//g'`
ver_sub=`grep "#define	APP_SUB_VER" $VER_FILE  | sed 's/[^0-9]//g'`
ver_det=`grep "#define	APP_DETAIL_VER" $VER_FILE  | sed 's/[^0-9]//g'`
ver_build=`grep "#define	APP_BUILD_VER" $VER_FILE  | sed 's/[^0-9]//g'`

ver=$ver_main.$ver_sub.$ver_det.$ver_build

#echo $ver_main.$ver_sub.$ver_det.$ver_build
#echo $ver
#echo $cdate

TARGET_PATH=./bin
WORK_PATH=`pwd`

#echo $WORK_PATH

################################################
#	Copy build/ -> bin/

IMG_NAME=${APP_NAME}_${cdate}_v${ver}

echo "./bin/${IMG_NAME}.hex"
echo "./bin/${IMG_NAME}.bin"

#cp -fr ./build/${APP_NAME}.bin ./bin/${IMG_NAME}.bin
cp -fr ./build/${APP_NAME}.hex ./bin/${IMG_NAME}.hex

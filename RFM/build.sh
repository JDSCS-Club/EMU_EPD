#make -f Makefile_RFM clean
#make -j12 -f Makefile_RFM
#./make_image.sh RFM

make -f Makefile_Boot clean
make -j12 -f Makefile_Boot

make clean
make -j12
./make_image.sh RFM

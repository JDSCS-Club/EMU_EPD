#make -f Makefile_RFM clean
#make -j12 -f Makefile_RFM
#./make_image.sh RFM

make clean
make -j12
./make_image.sh RFM

#! /bin/bash

# Make build directory
mkdir build
cd build

# Install libusb-1.0.26
if [ ! -d "libusb-1.0.26" ]; then
    curl -OL https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26.tar.bz2
    tar xvfj libusb-1.0.26.tar.bz2 
    cd libusb-1.0.26/
    ./configure
    make
    make check
    sudo make install
    cd ..
    rm libusb-1.0.26.tar.bz2
fi

# Install liblabjackusb
if [ ! -d "exodriver" ]; then
    git clone https://github.com/labjack/exodriver.git
    cd exodriver/
    sudo ./install.sh
    cd ..
fi

# Make code
cmake ..
make -j

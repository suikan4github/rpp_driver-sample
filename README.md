# rpp_driver-sample
Sample program for the rpp_driver library. 

## Installing tool
To build the samples or test, you need to install the build tools on Ubuntu : 

```sh
apt-get -y update
apt-get -y install build-essential cmake ninja-build git doxygen
apt-get -y install gcc-arm-none-eabi libnewlib-arm-none-eabi
```

```sh
mkdir build
cmake -B build -S . -DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico
cmake --build build --config Debug --target all
```

```sh
mkdir build
cmake -B build -S . -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2
cmake --build build --config Debug --target all
```

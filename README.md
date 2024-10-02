# rpp_driver-sample
Sample program for the [rpp_driver](https://github.com/suikan4github/rpp_driver) library. 
The programs are tested with Raspberry Pi Pico SDK 2.0.0.

## Installing tool
To build the samples or test, you need to install the build tools. 
The following commands will install the tools on the Ubuntu: 

```sh
apt-get -y update
apt-get -y install build-essential cmake ninja-build git doxygen
apt-get -y install gcc-arm-none-eabi libnewlib-arm-none-eabi
```
## Build for the Raspberry Pi Pico
You can build with the following commands for Raspberry Pi Pico (RP2040).
```sh
mkdir build
cmake -B build -S . -DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico
cmake --build build --config Debug --target all
```
In the case of VS Code, you just need to configure the project from GUI. So, VS Code CMake extension will configure the project for the Raspberry Pi Pico.   

## Build for the Raspberry Pi Pico2
In the case of the Raspberry Pi Pico2 (RP2350), run the following commands. 
```sh
mkdir build
cmake -B build -S . -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2
cmake --build build --config Debug --target all
```
In the case of VS Code, place following .vscode/settings.json in the project. 
```json
{
    "cmake.configureArgs": [
        "-DPICO_PLATFORM=rp2350",
        "-DPICO_BOARD=pico2"
    ]
}
```
So, VS Code CMake extension will configure the project for Raspberry Pi Pico2. 

## Raspberry Pi Pico SDK
During the build sequence, the Cmake command will download the newest Raspberry Pi Pico SDK. 
So, you don't need to download and set the environmental variables by yourself. 
You will obtain the executable files (*.uf2) under the build/samples/ directories. 

# License
These programs are distributed under [MIT license](LICENSE). 

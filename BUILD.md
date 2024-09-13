# Building the sample for RP2040
The sample program and I2S drivers are stored in the rpi directory. Follow the procedure to build the test. 

From the repository root ( where this README.md stays), run the following commands. 

```sh
cd rpi_pico

# Configure the project for Rpi Pico(RP2040)
cmake -DCMAKE_BUILD_TYPE:STRING=Debug \
-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-gcc \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-g++ \
--no-warn-unused-cli \
-S . \
-B build \
-G Ninja

# Build
cmake --build build --config Debug --target all
```

# Building the sample for RP2350
In the case of RP2035, the build procedure should be changed to the followings : 

```sh
cd rpi_pico

# Configure the project for Rpi Pico(RP2040)
cmake -DCMAKE_BUILD_TYPE:STRING=Debug \
-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-gcc \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/arm-none-eabi-g++ \
--no-warn-unused-cli \
-DPICO_PLATFORM=rp2350 \
-DPICO_BOARD=pico2 \
-S . \
-B build \
-G Ninja

# Build
cmake --build build --config Debug --target all
```

The PICO_PLATFORM macro and PICO_BOARD macro are added in the configuration phase. 
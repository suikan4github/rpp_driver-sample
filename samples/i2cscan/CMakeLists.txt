cmake_minimum_required(VERSION 3.13)

# Redefine the project name for this executable. 
project(i2cscan)

# executable is created here. 
add_executable(${PROJECT_NAME}
        main.cpp
        )

# pull in common dependencies
        target_link_libraries(${PROJECT_NAME} pico_stdlib 
                                                hardware_i2c
                                                hardware_pio)
# local dependencies
        target_link_libraries(${PROJECT_NAME}  rpp_driver)

# enable usb output, disable uart output
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
   
# create map/bin/hex file etc.
   pico_add_extra_outputs(${PROJECT_NAME})


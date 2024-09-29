/*
 * I2C bus scanning program.
 * Copyright Seiichi Horie 2024.
 *
 * Scan all possible address on the I2C bus, and print the
 * map
 *
 * To customize for your board, modify following 3 definitions:
 * - I2C_PORT
 * - kI2cSclPin
 * - kI2cSdaPin
 *
 */
// Include local definition
#include <stdio.h>

#include "gpiobasic.hpp"
#include "i2cmaster.hpp"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "sdkwrapper.hpp"

// Definition of the I2C port and pin to scan.
#define I2C_PORT i2c1
const unsigned int kI2cSclPin = 7;  // GPIO pin number.
const unsigned int kI2cSdaPin = 6;  // GPIO pin number.

// Scan algorithm.
void I2cSearch(::rpp_driver::I2cMaster i2c) {
  printf("\n            Probing I2C devices \n");
  printf("   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
  printf("---+------------------------------------------------\n");

  // Search raw
  for (int raw = 0; raw < 128; raw += 16) {
    // Search column
    printf("%2x |", raw);
    for (int col = 0; col < 16; col++) {
      const int address = raw + col;
      // check whether device exist or not.
      if ((address >= 0x78) || (0x03 >= address))
        printf("   ");                         // reserved or special address.
      else if (i2c.IsDeviceExisting(address))  // device acknowledged.
        printf(" %2X", address);               // print address
      else                                     // no device
        printf(" --");
    }
    printf("\n");
  }
}

int main() {
  // Constants declaration.
  const unsigned int kI2cClock = 100'000;  // Hz.
  const unsigned int kLedPin = PICO_DEFAULT_LED_PIN;

  ::rpp_driver::SdkWrapper sdk;
  ::rpp_driver::I2cMaster i2c(sdk,          // Inject SDK Dependency.
                              *I2C_PORT,    // I2C controller to use.
                              kI2cClock,    // I2C Clock [Hz]
                              kI2cSclPin,   // GPIO pin for SCL
                              kI2cSdaPin);  // GPIO pin for SDA
  ::rpp_driver::GpioBasic led(sdk, kLedPin);

  // Prepare for printf.
  sdk.stdio_init_all();

  // Setup on-board LED.
  led.SetDir(true);
  led.Put(true);  // turn on;

  // Count down to 0 and then, start the job.
  // This count down enable user to connect USB serial port.
  int count = 5;
  while (true) {
    sdk.sleep_ms(1000);
    led.Toggle();
    printf("%d\n", count);
    count--;               // count down
    if (count < 0) break;  // If reached to 0, break the loop.
  }

  // Print the map of the I2C device.
  I2cSearch(i2c);

  // Finally, blinking loop.
  while (true) {
    sdk.sleep_ms(250);
    led.Toggle();
  }

  // Information for the picotool.
  // clang-format off
  bi_decl(bi_program_description("I2c device scan program."));
  bi_decl(bi_program_url("https://github.com/suikan4github/rpp_driver-sample"));
    bi_decl(bi_2pins_with_func(kI2cSclPin, kI2cSdaPin, GPIO_FUNC_I2C));
  // clang-format on
}

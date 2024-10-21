/*
 * LED blinking program.
 * Copyright Seiichi Horie 2024.
 *
 * Blink on board LED of RasPi Pico/Pico2 board.
 */

// Include local definition
#include "gpio/gpiobasic.hpp"
#include "pico/binary_info.h"
#include "sdk/sdkwrapper.hpp"

int main() {
  // Constants declaration.
  const uint kLedPin = PICO_DEFAULT_LED_PIN;
  ::rpp_driver::SdkWrapper sdk;
  ::rpp_driver::GpioBasic led(sdk,       // SDK dependency injection.
                              kLedPin);  // GPIO pin # for LED.

  // Use RasPi Pico on-board LED.
  // 1=> Turn on, 0 => Turn pff.
  led.SetDir(true);

  // Blinking loop.
  while (true) {
    sdk.sleep_ms(250);
    led.Toggle();
  }

  // Information for the picotool.
  // clang-format off
  bi_decl(bi_program_description("Blinking LED with GpioBasic class."));
  bi_decl(bi_program_url("https://github.com/suikan4github/rpp_driver-sample"));
  bi_decl(bi_1pin_with_name(kLedPin, "LED"));
  // clang-format on
}

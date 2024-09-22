// #define I2S_DEBUG

// Include local definition
#include "gpiobasic.hpp"
#include "pico/binary_info.h"
#include "sdkwrapper.hpp"

int main() {
  // Constants declaration.
  const uint kLedPin = PICO_DEFAULT_LED_PIN;
  ::rpp_driver::SdkWrapper sdk;
  ::rpp_driver::GpioBasic led(sdk, kLedPin);

  // Use RasPi Pico on-board LED.
  // 1=> Turn on, 0 => Turn pff.
  led.SetDir(true);

  // Audio talk thorough
  while (true) {
    led.Put(true);  // turn on
    sdk.sleep_ms(125);
    led.Put(false);  // turn off
    sdk.sleep_ms(875);
  }

  // Information for the picotool.
  // clang-format off
  bi_decl(bi_program_description("Blinking LED with GpioBasic class."));
  bi_decl(bi_program_url("https://github.com/suikan4github/rpp_driver-sample"));
  bi_decl(bi_1pin_with_name(kLedPin, "LED"));
  // clang-format on
}

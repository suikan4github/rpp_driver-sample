// #define I2S_DEBUG

// Include local definition
#include <math.h>
#ifdef I2S_DEBUG
#include <stdio.h>
#endif

#include "adau1361.hpp"
#include "duplexslavei2s.hpp"
#include "gpiobasic.hpp"
#include "hardware/pio.h"
#include "i2cmaster.hpp"
#include "pico/binary_info.h"
#include "sdkwrapper.hpp"
#include "umbadau1361lower.hpp"

int main() {
  // Constants declaration.
  const unsigned int kAdau1361I2cAddress = 0x38;
  const unsigned int kI2cClock = 100'000;  // Hz.
  const unsigned int kI2cScl_pin = 7;      // GPIO pin number.
  const unsigned int kI2cSdaPin = 6;       // GPIO pin number.
  const unsigned int kMClock =
      12'000'000;                   // Hz. Master clock of the UMB-ADAU1361-A
  const unsigned int kFs = 48'000;  // Hz. Sampling frequency
  const uint kLedPin = PICO_DEFAULT_LED_PIN;

  /*
   * Pin usage of I2S. These pins must be consecutive.
   * The usage is defined by its relative position.
   *
   * 10 : SDOUT
   * 11 : SDIN
   * 12 : BCLK (IN)
   * 13 : WS (IN)
   */
  const unsigned int kI2sGpioPinBase = 10;
  const unsigned int kI2sGpioPinDebug = 15;

  // PIO peripheral and state machine.
  PIO i2s_pio = pio0;
  const uint kI2sStateMachine = 0;

  // SDK Wrapper object.
  ::rpp_driver::SdkWrapper sdk;

  // Init USB-Serial port by 9600bps, 1stop bit, 8bit.
  // add following lines to CMakeLists.txt
  // to enable usb-serial and disable serial.
  //     pico_enable_stdio_usb($ { PROJECT_NAME } 1)
  //     pico_enable_stdio_uart($ { PROJECT_NAME } 0)
  sdk.stdio_init_all();

#ifdef I2S_DEBUG
  // Delay count down to connect the serial terminal for debugging.
  uint const count = 7;
  printf("%d\n", count);
  for (size_t i = 0; i < count; i++) {
    sleep_ms(1000);
    printf("%d\n", count - i - 1);
  }
  printf("Go!\n");
#endif

  // Prepare the Audio CODEC.
  ::rpp_driver::I2cMaster i2c(sdk, *i2c1, kI2cClock, kI2cScl_pin, kI2cSdaPin);
  ::rpp_driver::UmbAdau1361Lower codec_lower(i2c, kAdau1361I2cAddress);
  ::rpp_driver::Adau1361 codec(kFs, kMClock, codec_lower);

  // I2S Initialization. We run the I2S PIO program from here.
  ::rpp_driver::DuplexSlaveI2s i2s(sdk, i2s_pio, kI2sStateMachine,
                                   kI2sGpioPinBase);

  // Use RasPi Pico on-board LED.
  // 1=> Turn on, 0 => Turn pff.
  ::rpp_driver::GpioBasic led(sdk, kLedPin);
  led.SetDir(true);
  // Debug pin to watch the processing time by oscilloscope.
  // This pin is "H" during the audio processing.
  ::rpp_driver::GpioBasic debug_pin(sdk, kI2sGpioPinDebug);
  debug_pin.SetDir(true);

  // CODEC initialization and run.
  codec.Start();
  // Un mute the codec to enable the sound. Default gain is 0dB.
  codec.Mute(rpp_driver::Adau1361::LineInput, false);        // unmute
  codec.Mute(rpp_driver::Adau1361::HeadphoneOutput, false);  // unmute

  // Sync with WS, and then start to transfer.
  // After this line, We have to wait for the RX FIFO ASAP.
  i2s.Start();

  // Audio talk thorough
  while (true) {
    // Get Left/Right I2S samples from RX FIFO.
    // Wait until the FIFO is ready.
    int32_t left_sample = i2s.GetFIFOBlocking();
    int32_t right_sample = i2s.GetFIFOBlocking();
    // Signaling the start of processing to the external pin.
    // We have to complete the processing within 1 sample time.
    //    sdk.gpio_put(kI2sGpioPinDebug, true);
    debug_pin.Put(true);

// __arm__ predefined macro is defined by compiler if the target is
// ARM 32bit architecture.
// RP2350A has dual architecture :
// - ARM Cortex-M33 with FPU
// - RV32I without FPU
// So, we use the floating point math only for the ARM architecture.
#if (defined(PICO_RP2350A) && defined(__arm__))
    left_sample = static_cast<int32_t>(left_sample * 0.7f);
    right_sample = static_cast<int32_t>(right_sample * 0.7f);
#else
    left_sample /= 2;
    right_sample /= 2;
#endif

    // Put Left/Right I2S samples to TX FIFO.
    i2s.PutFIFOBlocking(left_sample);
    i2s.PutFIFOBlocking(right_sample);
    // Signaling the end of processing to the external pin.
    //   sdk.gpio_put(kI2sGpioPinDebug, false);
    debug_pin.Put(true);
  }

  // Information for the picotool.
  // clang-format off
  bi_decl(bi_program_description(
      "Working with UMB-ADAU1361A board. ADAU1361A I2C address is 0x38."));
  bi_decl(bi_program_url("https://github.com/suikan4github/rpp_driver-sample"));
  bi_decl(bi_2pins_with_func(kI2cScl_pin, kI2cSdaPin, GPIO_FUNC_I2C));
  bi_decl(bi_4pins_with_names(kI2sGpioPinBase, "I2S SDO", kI2sGpioPinBase + 1,
                              "I2S_SDI", kI2sGpioPinBase + 2, "I2S BCLK IN",
                              kI2sGpioPinBase + 3, "I2S WS IN"));
  bi_decl(bi_1pin_with_name(kI2sGpioPinDebug, "DEBUG OUT"));
  // clang-format on
}

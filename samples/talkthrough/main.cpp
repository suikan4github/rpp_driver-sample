/*
 * I2S audio input output program.
 * Copyright Seiichi Horie 2024.
 *
 * Initialize the Audio CODEC as master mode. And then,
 * Start the I2S data transfer. I2S timing signals of RasPi Pico/Pico2
 * are slave. In the other words, BCLK and WS are input.
 *
 * 6  : I2C SCL
 * 7  : I2C SDA
 *
 * 10 : I2S SDOUT
 * 11 : I2S SDIN
 * 12 : I2S BCLK (IN)
 * 13 : I2S WS (IN)
 *
 * 15 : Processing status (OUT)
 */

// Include local definition
#include <math.h>
#ifdef I2S_DEBUG
#include <stdio.h>
#endif

#include "codec/adau1361.hpp"
#include "codec/umbadau1361lower.hpp"
#include "gpio/gpiobasic.hpp"
#include "hardware/pio.h"
#include "i2c/i2cmaster.hpp"
#include "i2s/i2sslaveduplex.hpp"
#include "pico/binary_info.h"
#include "sdk/sdkwrapper.hpp"

int main() {
  // Constants declaration.
  const unsigned int kAdau1361I2cAddress = 0x38;
  const unsigned int kI2cClock = 100'000;  // Hz.
  const unsigned int kI2cSclPin = 7;       // GPIO pin number.
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
  // The target CODEC board is UMB-ADAU1361-A
  ::rpp_driver::I2cMaster i2c(sdk,          // Inject SDK Dependency.
                              *i2c1,        // I2C controller to use.
                              kI2cClock,    // I2C Clock [Hz]
                              kI2cSclPin,   // GPIO pin for SCL
                              kI2cSdaPin);  // GPIO pin # for SDA

  ::rpp_driver::UmbAdau1361Lower codec_lower(
      i2c,                   // Inject I2C controller dependency.
      kAdau1361I2cAddress);  // I2C address of UMB-ADAU1361-A

  ::rpp_driver::Adau1361 codec(
      kFs,           // Sampling frequency[Hz].
      kMClock,       // Master clock of UMB-ADAU1361-A[Hz].
      codec_lower);  // Inject Codec lower part dependency.

  // I2S Class.
  ::rpp_driver::I2sSlaveDuplex i2s(
      sdk,               // Inject SDK dependency.
      i2s_pio,           // Specify PIO controller to use.
      kI2sStateMachine,  // Specify PIO State machine to use.
      kI2sGpioPinBase);  // The youngest GPIO pin # for I2S which is SDO.

  // Use RasPi Pico on-board LED.
  ::rpp_driver::GpioBasic led(sdk,       // Inject SDK dependency.
                              kLedPin);  // GPIO pin # of LED.
  // 1=> Turn on, 0 => Turn pff.
  led.SetDir(true);

  // Debug pin to watch the processing time by oscilloscope.
  // This pin is "H" during the audio processing.
  ::rpp_driver::GpioBasic debug_pin(sdk, kI2sGpioPinDebug);
  // 1=> Turn on, 0 => Turn pff.
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
    int32_t left_sample = i2s.GetFifoBlocking();
    int32_t right_sample = i2s.GetFifoBlocking();
    // Signaling the start of processing to the external pin.
    // We have to complete the processing within 1 sample time.
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
    i2s.PutFifoBlocking(left_sample);
    i2s.PutFifoBlocking(right_sample);
    // Signaling the end of processing to the external pin.
    debug_pin.Put(false);
  }

  // Information for the picotool.
  // clang-format off
  bi_decl(bi_program_description(
      "Working with UMB-ADAU1361A board. ADAU1361A I2C address is 0x38."));
  bi_decl(bi_program_url("https://github.com/suikan4github/rpp_driver-sample"));
  bi_decl(bi_2pins_with_func(kI2cSclPin, kI2cSdaPin, GPIO_FUNC_I2C));
  bi_decl(bi_4pins_with_names(kI2sGpioPinBase, "I2S SDO", kI2sGpioPinBase + 1,
                              "I2S_SDI", kI2sGpioPinBase + 2, "I2S BCLK IN",
                              kI2sGpioPinBase + 3, "I2S WS IN"));
  bi_decl(bi_1pin_with_name(kI2sGpioPinDebug, "DEBUG OUT"));
  // clang-format on
}

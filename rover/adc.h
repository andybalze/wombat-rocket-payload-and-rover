#ifndef _ADC_H
#define _ADC_H

////////////////////////////////////////////////////////////////////////////////
//
// ADC
//
// Provides functions that allow designs to use the ATMega328p's ADC peripheral.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

///////////////////// ADC Settings /////////////////////////////////////////////

// Determines the reference value of the ADC.
// 0 selects the voltage on the AREF pin.
// 1 selects the value on the AVCC pin.
// 2 selects the internal 1.1V reference.
#define ADC_REFERENCE_MODE (1)

///////////////////// Type Definitions /////////////////////////////////////////

// The digitized result of an analog-to-digital conversion.
typedef int16_t adc_result_t;

// The channels accessible by the ADC.
enum adc_channel_enum {
  ADC_CHANNEL_ADC0,
  ADC_CHANNEL_ADC1,
  ADC_CHANNEL_ADC2,
  ADC_CHANNEL_ADC3,
  ADC_CHANNEL_ADC4,
  ADC_CHANNEL_ADC5,
  ADC_CHANNEL_ADC6,
  ADC_CHANNEL_ADC7,
  ADC_CHANNEL_TEMP,
  ADC_CHANNEL_REF,
  ADC_CHANNEL_GND
};
typedef enum adc_channel_enum adc_channel_t;

///////////////////// Public Function Prototypes ///////////////////////////////

// Initializes the ADC, including configuring the appropriate pins.
void adc_initialize(void);

// Gets the most recent value of a particular ADC channel.
adc_result_t adc_get_channel_result(adc_channel_t channel);

#endif
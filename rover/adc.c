////////////////////////////////////////////////////////////////////////////////
//
// ADC
//
// Provides functions that allow designs to use the ATMega328p's ADC peripheral.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"

//////////////// Private Defines ///////////////////////////////////////////////

#define ADC_REFS_AREF     (0)
#define ADC_REFS_AVCC     ( _BV(REFS0)              )
#define ADC_REFS_INTERNAL ( _BV(REFS0) | _BV(REFS1) )
#define ADC_REFS_MASK     ( _BV(REFS0) | _BV(REFS1) )
#if ADC_REFERENCE_MODE == ADC_REFERENCE_MODE_AREF
  #define ADC_REFS (ADC_REFS_AREF & ADC_REFS_MASK)
#elif ADC_REFERENCE_MODE == ADC_REFERENCE_MODE_AVCC
  #define ADC_REFS (ADC_REFS_AVCC & ADC_REFS_MASK)
#elif ADC_REFERENCE_MODE == ADC_REFERENCE_MODE_INTERNAL
  #define ADC_REFS (ADC_REFS_INTERNAL & ADC_REFS_MASK)
#else
  #define ADC_REFS (ADC_REFS_INTERNAL & ADC_REFS_MASK)
#endif

#define ADC_ADPS_2    (0)
#define ADC_ADPS_4    (              _BV(ADPS1)               )
#define ADC_ADPS_8    ( _BV(ADPS0) | _BV(ADPS1)               )
#define ADC_ADPS_16   (                           _BV(ADPS2)  )
#define ADC_ADPS_32   ( _BV(ADPS0)              | _BV(ADPS2)  )
#define ADC_ADPS_64   (              _BV(ADPS1) | _BV(ADPS2)  )
#define ADC_ADPS_128  ( _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2)  )
#define ADC_ADPS_MASK ( _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2)  )
#if ADC_PRESCALER == 2
  #define ADC_ADPS (ADC_ADPS_2 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 4
  #define ADC_ADPS (ADC_ADPS_4 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 8
  #define ADC_ADPS (ADC_ADPS_8 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 16
  #define ADC_ADPS (ADC_ADPS_16 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 32
  #define ADC_ADPS (ADC_ADPS_32 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 64
  #define ADC_ADPS (ADC_ADPS_64 & ADC_ADPS_MASK)
#elif ADC_PRESCALER == 128
  #define ADC_ADPS (ADC_ADPS_128 & ADC_ADPS_MASK)
#else
  #define ADC_ADPS (ADC_ADPS_128 & ADC_ADPS_MASK)
#endif

#define ADC_CHANNEL_COUNT (11)
#define ADC_MUX_ADC0 (0)
#define ADC_MUX_ADC1 (  _BV(MUX0)                                     )
#define ADC_MUX_ADC2 (              _BV(MUX1)                         )
#define ADC_MUX_ADC3 (  _BV(MUX0) | _BV(MUX1)                         )
#define ADC_MUX_ADC4 (                          _BV(MUX2)             )
#define ADC_MUX_ADC5 (  _BV(MUX0)             | _BV(MUX2)             )
#define ADC_MUX_ADC6 (              _BV(MUX1) | _BV(MUX2)             )
#define ADC_MUX_ADC7 (  _BV(MUX0) | _BV(MUX1) | _BV(MUX2)             )
#define ADC_MUX_TEMP (                                      _BV(MUX3) )
#define ADC_MUX_REF  (              _BV(MUX1) | _BV(MUX2) | _BV(MUX3) )
#define ADC_MUX_GND  (  _BV(MUX0) | _BV(MUX1) | _BV(MUX2) | _BV(MUX3) )
#define ADC_MUX_MASK (  _BV(MUX0) | _BV(MUX1) | _BV(MUX2) | _BV(MUX3) )

#define ADC_RESULTS_BUFFER_INDEX_ADC0 (0)
#define ADC_RESULTS_BUFFER_INDEX_ADC1 (1)
#define ADC_RESULTS_BUFFER_INDEX_ADC2 (2)
#define ADC_RESULTS_BUFFER_INDEX_ADC3 (3)
#define ADC_RESULTS_BUFFER_INDEX_ADC4 (4)
#define ADC_RESULTS_BUFFER_INDEX_ADC5 (5)
#define ADC_RESULTS_BUFFER_INDEX_ADC6 (6)
#define ADC_RESULTS_BUFFER_INDEX_ADC7 (7)
#define ADC_RESULTS_BUFFER_INDEX_TEMP (8)
#define ADC_RESULTS_BUFFER_INDEX_REF  (9)
#define ADC_RESULTS_BUFFER_INDEX_GND  (10)

//////////////// Static Variable Definitions ///////////////////////////////////

// A buffer that stores the most-recently-converted values for each channel.
#define ADC_CHANNEL_COUNT (11)
static adc_result_t adc_results_buffer[ADC_CHANNEL_COUNT];

// The sequence of MUX values to rotate through. Includes only the enabled
// channels.
static char adc_mux_sequence[] = {
#ifdef ADC_ENABLE_ADC0
  ADC_MUX_ADC0,
#endif
#ifdef ADC_ENABLE_ADC1
  ADC_MUX_ADC1,
#endif
#ifdef ADC_ENABLE_ADC2
  ADC_MUX_ADC2,
#endif
#ifdef ADC_ENABLE_ADC3
  ADC_MUX_ADC3,
#endif
#ifdef ADC_ENABLE_ADC4
  ADC_MUX_ADC4,
#endif
#ifdef ADC_ENABLE_ADC5
  ADC_MUX_ADC5,
#endif
#ifdef ADC_ENABLE_ADC6
  ADC_MUX_ADC6,
#endif
#ifdef ADC_ENABLE_ADC7
  ADC_MUX_ADC7,
#endif
#ifdef ADC_ENABLE_TEMP
  ADC_MUX_TEMP,
#endif
#ifdef ADC_ENABLE_REF
  ADC_MUX_REF,
#endif
#ifdef ADC_ENABLE_GND
  ADC_MUX_GND,
#endif
};

// The sequence of results buffer indices to run through. Includes only enabled
// channels.
static int adc_results_buffer_index_sequence[] = {
#ifdef ADC_ENABLE_ADC0
  ADC_RESULTS_BUFFER_INDEX_ADC0,
#endif
#ifdef ADC_ENABLE_ADC1
  ADC_RESULTS_BUFFER_INDEX_ADC1,
#endif
#ifdef ADC_ENABLE_ADC2
  ADC_RESULTS_BUFFER_INDEX_ADC2,
#endif
#ifdef ADC_ENABLE_ADC3
  ADC_RESULTS_BUFFER_INDEX_ADC3,
#endif
#ifdef ADC_ENABLE_ADC4
  ADC_RESULTS_BUFFER_INDEX_ADC4,
#endif
#ifdef ADC_ENABLE_ADC5
  ADC_RESULTS_BUFFER_INDEX_ADC5,
#endif
#ifdef ADC_ENABLE_ADC6
  ADC_RESULTS_BUFFER_INDEX_ADC6,
#endif
#ifdef ADC_ENABLE_ADC7
  ADC_RESULTS_BUFFER_INDEX_ADC7,
#endif
#ifdef ADC_ENABLE_TEMP
  ADC_RESULTS_BUFFER_INDEX_TEMP,
#endif
#ifdef ADC_ENABLE_REF
  ADC_RESULTS_BUFFER_INDEX_REF,
#endif
#ifdef ADC_ENABLE_GND
  ADC_RESULTS_BUFFER_INDEX_GND,
#endif
};

// The length of the adc channel sequence. Equal to the length of both
// adc_mux_sequence and adc_results_buffer_index_sequence.
int adc_sequence_length = sizeof(adc_mux_sequence) / sizeof(*adc_mux_sequence);

// Tracks what the next ADC channel to be sampled is. Indexes both 
// adc_mux_sequence and adc_results_buffer_index_sequence.
int adc_sequence_index;

//////////////// Public Function Bodies ////////////////////////////////////////

// Initializes the ADC, including configuring the appropriate pins.
void adc_initialize(void) {

  // Clears the results buffer
  int i;
  for (i = 0; i < ADC_CHANNEL_COUNT; i++) {
    adc_results_buffer[i] = 0;
  }

  adc_sequence_index = 0;

  // Set up ADC registers

  // Selects the reference mode
  #ifdef SETTINGS_PARANOID_REGISTERS
    ADMUX &= (ADC_REFS_MASK | ADC_MUX_MASK);
  #endif
  ADMUX = (ADC_REFS | adc_mux_sequence[0]);

  // Configures ADC Control and Status Register A
  ADCSRA = (
    _BV(ADEN) | // Turns on the ADC.
    _BV(ADIE) | // Enables the conversion complete interrupt.
    ADC_ADPS    // Sets the ADC clock prescaler.
  );

  // Enable or disable digital inputs for appropriate pins.
  DIDR0 = (
    0
    #ifdef ADC_ENABLE_ADC0
      | _BV(ADC0D)
    #endif
    #ifdef ADC_ENABLE_ADC1
      | _BV(ADC1D)
    #endif
    #ifdef ADC_ENABLE_ADC2
      | _BV(ADC2D)
    #endif
    #ifdef ADC_ENABLE_ADC3
      | _BV(ADC3D)
    #endif
    #ifdef ADC_ENABLE_ADC4
      | _BV(ADC4D)
    #endif
    #ifdef ADC_ENABLE_ADC5
      | _BV(ADC5D)
    #endif
  );

  // Begin the first conversion.
  ADCSRA |= _BV(ADSC);

}

// Gets the most recent value of a particular ADC channel.
adc_result_t adc_get_channel_result(adc_channel_t channel) {

  switch (channel)
  {
  case ADC_CHANNEL_ADC0:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC1:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC2:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC3:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC4:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC5:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC6:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_ADC7:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_TEMP:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_REF:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;

  case ADC_CHANNEL_GND:
    return adc_results_buffer[ADC_RESULTS_BUFFER_INDEX_ADC0];
    break;
  
  default:
    return 0;
    break;
  }

}

///////////// Interrupt Service Routines ///////////////////////////////////////

// Runs each time a conversion finishes. Stores the result of the conversion
// and begins the next conversion.
ISR(ADC_vect) {

  // Reads out the ADC conversion result.
  adc_result_t result;
  result = (ADCH << 8 + ADCL);

  // Stores the result.
  // (Goodness gracious this is an ugly line)
  adc_results_buffer[adc_results_buffer_index_sequence[adc_sequence_index]] = result;

  // Moves on to the next channel.
  if (adc_sequence_index = adc_sequence_length - 1) {
    adc_sequence_length = 0;
  } else {
    adc_sequence_index = adc_sequence_index + 1;
  }

  // Selects the next channel.
  ADMUX &= ~ADC_MUX_MASK;
  ADMUX |= adc_mux_sequence[adc_sequence_index];

  // Starts the new conversion.
  ADCSRA |= _BV(ADSC);

}
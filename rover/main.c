////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "ir.h"


int main() {

	digital_io_initialize();
	uart_initialize();
	adc_initialize();
	motors_initialize();

	while(1);

}

ISR(USART_RX_vect) {
	received_data = UDR0;
	adc_result_t channel_value;
	char *channel_name;
	switch (received_data)
	{
	case KEYSTROKE_ADC0:
		channel_value = ir_distance_read();	// Edited this line to return distance in centimeters
		channel_name  = channel_name_adc0;
		break;
	case KEYSTROKE_ADC1:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC1);
		channel_name  = channel_name_adc1;
		break;
	case KEYSTROKE_ADC2:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC2);
		channel_name  = channel_name_adc2;
		break;
	case KEYSTROKE_ADC3:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC3);
		channel_name  = channel_name_adc3;
		break;
	case KEYSTROKE_ADC4:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC4);
		channel_name  = channel_name_adc4;
		break;
	case KEYSTROKE_ADC5:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC5);
		channel_name  = channel_name_adc5;
		break;
	case KEYSTROKE_ADC6:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC6);
		channel_name  = channel_name_adc6;
		break;
	case KEYSTROKE_ADC7:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC7);
		channel_name  = channel_name_adc7;
		break;
	case KEYSTROKE_TEMP:
		channel_value = adc_get_channel_result(ADC_CHANNEL_TEMP);
		channel_name  = channel_name_temp;
		break;
	case KEYSTROKE_REF:
		channel_value = adc_get_channel_result(ADC_CHANNEL_REF);
		channel_name  = channel_name_ref;
		break;
	case KEYSTROKE_GND:
		channel_value = adc_get_channel_result(ADC_CHANNEL_GND);
		channel_name  = channel_name_gnd;
		break;
	default:
		return;
	}
	transmit_output_message(channel_name, channel_value);
}

void transmit_output_message(
	char *channel_name,
	adc_result_t channel_value
) {

	uart_transmit_formatted_message(
		output_message_format,
		channel_name,
		channel_value
	);

}

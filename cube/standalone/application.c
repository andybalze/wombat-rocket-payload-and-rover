#include "application.h"

void application() {

    while(1) {

        // Poll to see if we received any data.
        // This is a blocking function that will wait until we
        // get a message. While we're blocked, we may also be
        // forwarding messages.

        if (transport_rx()) {

            // Store in EEPROM.

            // Form a response.

            // Transmit the response.
            transport_tx();

        }
    }
}

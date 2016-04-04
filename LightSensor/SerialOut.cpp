#include <Arduino.h>

#include "SerialOut.h"

SerialOut::SerialOut (byte pin) : tx_pin (pin), state (0)
{
}

void
SerialOut::intKick ()
{
  switch (state)
  {
    case 0:                       // Idle
      break;
      
    case 1:                       // Start bit
      count = 0;
      state = 2;
#if 0
      digitalWrite (tx_pin, LOW);
#else
      PORTB &= ~ _BV (tx_pin);
#endif
      break;
      
    case 2:                       // Clock data
      if (count == 8)
      {
        // All data clocked, start stop bit
#if 0
        digitalWrite (tx_pin, HIGH);
#else
        PORTB |= _BV (tx_pin);
#endif

        state = 0;
      }
      else
      {
        // write out the next bit
#if 0
        digitalWrite (tx_pin, ((buffer >> count) & 1) ? HIGH : LOW);
#else
        PORTB = (PORTB & ~ _BV (tx_pin)) | (((buffer >> count) & 1) ? _BV (tx_pin) : 0);
#endif

        count += 1;
      }
      
      break;

    default:                      // Just in case
      state = 0;
      break;
  }
}

size_t
SerialOut::write (uint8_t c)
{
  while (state != 0)
    ;
      
  buffer = c;
  state = 1;

  return 1;
}


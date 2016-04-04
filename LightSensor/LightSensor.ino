#include <Arduino.h>
#include <core_timers.h>
#include <avr/wdt.h>

#include "SerialOut.h"

// Pin configuration

const byte tx_pin (3);

const byte light_digital_pin (4);
const byte light_analogue_pin (A2);

SerialOut Out (tx_pin);

void
setup ()
{
  // Set the watchdog up. We need to do this within 16mS of startup
  WDTCR = /*_BV (WDIE) |*/ _BV (WDE) | _BV (WDCE) | _BV (WDP3) | _BV (WDP0);  
    
#if 0
  pinMode (tx_pin, OUTPUT);
  pinMode (light_digitial_pin, INPUT_PULLUP);
#else
  DDRB = (DDRB &  ~(_BV (tx_pin) | _BV (light_digital_pin))) | _BV (tx_pin);
  PORTB |= _BV (light_digital_pin) | _BV (tx_pin);
#endif   

  cli();                      // Stop interrupts for a bit

  // Ensure the timer is in a sane state
  Timer0_SetToPowerup ();  

  // We count up to 103. That takes 104 cycles at 1MHz this gives us
  // 9615 baud which is (hopefully) close enough.
  Timer0_SetOutputCompareMatchAndClear (103);
  
  // Use clear on timer compare mode
  Timer0_SetWaveformGenerationMode (Timer0_CTC_OCR);
  
  // Kick us when done
  Timer0_EnableOutputCompareInterruptA ();
  
  // Now start it running with a 1 MHz clock
  Timer0_ClockSelect (Timer0_Prescale_Value_8);

  sei();                      // Enable interrupts again
  
  // Check out reset reasons.
  
  const bool wd_reset    ((MCUSR & _BV (WDRF))  != 0);  
  const bool bo_reset    ((MCUSR & _BV (BORF))  != 0);
  const bool ext_reset   ((MCUSR & _BV (EXTRF)) != 0);
  const bool power_reset ((MCUSR & _BV (PORF))  != 0);
    
  if (wd_reset)
    Out.println ("Woof!");
    
  if (bo_reset)
    Out.println ("Brown out!");
    
  if (ext_reset)
    Out.println ("External reset");
    
  if (power_reset)
    Out.println ("Power reset");

  // Reset reasons
  
  MCUSR = 0;
}

void
loop ()
{
  int v (analogRead (light_analogue_pin));

  Out.println (v);
  
  wdt_reset ();
  
  delay (50);
}

ISR (TIMER0_COMPA_vect)
{
  Out.intKick ();
}


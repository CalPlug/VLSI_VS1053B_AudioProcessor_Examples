#include "Arduino.h"
const int led_pin = 12;
uint32_t b12 = g_APinDescription[12].ulPin;
Pio *p = g_APinDescription[12].pPort
int i = 0;
int val = 0;
int sync1, sync2, unsync = 0;
int intcount = 0;
int message[112] = {0};

void setup()
{
  Serial.begin(115200);
  pinMode(led_pin, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  /* turn on the timer clock in the power management controller */
  pmc_set_writeprotect(false);     // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC7);   // enable peripheral clock TC7

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC2,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
  TC_SetRC(TC2, 1, 21);
  TC_Start(TC2, 1);

  // enable timer interrupts on the timer
  TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS; // IER = interrupt enable register
  TC2->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS; // IDR = interrupt disable register

  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
//  NVIC_EnableIRQ(TC7_IRQn);
}

void loop()
{
  // do nothing timer interrupts will handle the blinking;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TC7_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC2, 1);
  
  bool in = !!(p -> PIO_PDSR & b12);
  
  intcount++;
  
  if (in && intcount == 1) 
	  sync1 = 1; 
  else {
	  intcount = 0;
	  return;
  }
  
  if (in && sync1 == 1 && intcount == 3) 
	  sync1 = 2; 
  else {
	  intcount = 0;
	  sync1 = 0;
	  return;
  }
  
  if (in && sync1 == 2 && intcount == 8) 
	  sync2 = 1; 
  else {
	  intcount = 0;
	  sync1 = 0;
	  return;
  }
	  
  
  if (in && sync2 == 1 && intcount == 10) 
	  sync2 = 2; 
  else {
	  intcount = 0;
	  sync1 = 0;
	  sync2 = 0;
	  return;
  }
  
  if (sync1 == 2 && sync2 == 2 && intcount >= 17 && intcount%2 == 1) {
	  message[intcount - 17] = in;
	  if ((intcount - 17) > 112) {
		  sync1 = 0;
		  sync2 = 0;
		  intcount = 0;
		  return;
	  }
  }
}

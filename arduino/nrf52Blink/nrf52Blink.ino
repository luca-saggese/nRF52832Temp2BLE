
//////////////
// Hardware //
//////////////

#define LED_ACTIVE LOW // Pin 7 LED is active low
#define LED_DEFAULT LOW

bool LedState = 0; 

extern "C" void TIMER2_IRQHandler(void)
{
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0)) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event 
  }
  
  if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0)) {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;           //Clear compare register 1 event
  }

  LedState = !LedState;
  digitalWrite(LED_BUILTIN, LedState);
   
  Serial.print("*"); // use when playing with prascale and compare values
  
}


void start_timer(void)
{    
  NRF_TIMER2->TASKS_STOP = 1;
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;  // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;               // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER = 1;                             //Set prescaler. Higher number gives slower timer.
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_24Bit;     //Set counter to 24 bit resolution
  NRF_TIMER2->CC[0] = 25000;                             //Set value for TIMER1 compare register 0
  NRF_TIMER2->CC[1] = 25000;                                 //Set value for TIMER1 compare register 1
    
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
     
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER
  NVIC_EnableIRQ(TIMER2_IRQn);
 
}


void setup(void)
{
  // start serial port
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  start_timer();
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 
 delay(10000);
}

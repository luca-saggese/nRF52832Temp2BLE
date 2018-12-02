// Include the libraries we need
#include <OneWire.h>
#include <SPI.h>
#include <BLEPeripheral.h>
#include <DallasTemperature.h>


BLEPeripheral blePeripheral = BLEPeripheral();

BLEService tempService = BLEService("CCC0");
BLEFloatCharacteristic tempCharacteristic = BLEFloatCharacteristic("CCC1", BLERead | BLENotify);
BLEDescriptor tempDescriptor = BLEDescriptor("2901", "Temp Celsius");

// Data wire is plugged into pin 31 on the Arduino
#define ONE_WIRE_BUS 31

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

volatile bool readFromSensor = false;

// Interrup function

extern "C" void TIMER2_IRQHandler(void)
{
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0)) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event 
  }
  
  if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0)) {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;           //Clear compare register 1 event
  }
   
  readFromSensor =  true;
  Serial.print("*"); // use when playing with prascale and compare values
  
}


void start_timer(void)
{    
  NRF_TIMER2->TASKS_STOP = 1;
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;  // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;               // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER = 4;                             //Set prescaler. Higher number gives slower timer.
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_24Bit;     //Set counter to 24 bit resolution
  NRF_TIMER2->CC[0] = 25000;                             //Set value for TIMER compare register 0
  NRF_TIMER2->CC[1] = 25000;                             //Set value for TIMER1 compare register 1
    
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
     
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER
  NVIC_EnableIRQ(TIMER2_IRQn);
 
}


void setup(void)
{
  // start serial port
  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  blePeripheral.setLocalName("Temperature");

  blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
  blePeripheral.addAttribute(tempService);
  blePeripheral.addAttribute(tempCharacteristic);
  blePeripheral.addAttribute(tempDescriptor);

  // Start up the library
  sensors.begin();

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  blePeripheral.begin();

  start_timer();
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 
   blePeripheral.poll();

  if (readFromSensor) {
     
     setTempCharacteristicValue();
    
     readFromSensor = false;
  }

  
  
}


void setTempCharacteristicValue() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  float reading = sensors.getTempCByIndex(0);

  if (!isnan(reading) && significantChange(tempCharacteristic.value(), reading, 0.5)) {
    tempCharacteristic.setValue(reading);
    Serial.print(F("Temperature: ")); Serial.print(reading); Serial.println(F("C"));
  }
}


boolean significantChange(float val1, float val2, float threshold) {
  return (abs(val1 - val2) >= threshold);
}

void blePeripheralConnectHandler(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

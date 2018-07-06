

// Enable debug prints to serial monitor
#define MY_DEBUG 


// Enable and select radio type attached
//#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level. 
//#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Flash leds on rx/tx/err
// #define MY_LEDS_BLINKING_FEATURE
// Set blinking period
// #define MY_DEFAULT_LED_BLINK_PERIOD 300

// Inverses the behavior of leds
// #define MY_WITH_LEDS_BLINKING_INVERSE

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60 
// Digital pin used for inclusion mode button
#define MY_INCLUSION_MODE_BUTTON_PIN  3 

// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED

// Enable repeater fucontactronOnctionality for this node
// #define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>  
#include <Bounce2.h>


class Contactron
{ 

  uint8_t child_id;
  int contactronPin;
  bool contactronON;
  bool contactronOFF;
  int value;
  int oldValue = -1;
  unsigned long last_update;
  unsigned long refresh_time;
  Bounce debouncer = Bounce();

  public:
  Contactron(int childId, int contactron, int debaunce, bool invertedContactron, unsigned long refreshTime) : msg(childId, V_TRIPPED)
  {
    child_id = childId;
    contactronPin = contactron;
    contactronON = !invertedContactron;
    contactronOFF = invertedContactron;
    refresh_time = refreshTime * 1000;
    pinMode(contactronPin, INPUT_PULLUP);
    debouncer.attach(contactronPin);
    debouncer.interval(debaunce);
  }
  
  MyMessage msg;
  
  void Update()
  {
    debouncer.update();
    value = debouncer.read();
      // Send in the new value.
      if(value != oldValue)
      {
         SyncController();
      }
      else
      {
        if(millis() - last_update >= refresh_time)
        {
            SyncController();
        }
      }    
  }

  void SyncController()
  {
    send(msg.set(value==contactronON ? 1 : 0));
    last_update = millis();
  }
  
  void Present()
  {
    present(child_id, S_DOOR);
  }

  
};

// define your switch/relay objects here
// Contactron(int childId , int contactron , int debaunce  ,             bool invertedContactron       ,     unsigned long refreshTime          )
//               0-255    ;   pin number   ; default = 50ms; 0: Contact ON = HIGH, 1: Contact ON = LOW ; update time if no change in contactron [s];
Contactron switch1(1, 4, 50, 0, 3600);   // 0 = D3,  12 = D6 
Contactron switch2(2, 5, 50, 0, 3600);  // 10= SD3, 13 = D7
Contactron switch3(3, 6, 50, 0, 3600);   // 4 = D2,  14 = D5
Contactron switch4(4, 7, 50, 0, 3600);    // 2 = D4,  5  = D1


void setup() 
{ 
  // Setup locally attached sensors
  delay(10000);
  switch1.SyncController(); // send actual value to controller
  switch2.SyncController();
  switch3.SyncController();
  switch4.SyncController();
}
void presentation()  
{   
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay as class object", "1.0");
  switch1.Present();
  switch2.Present();
  switch3.Present();
  switch4.Present();
}

void loop() 
{ 
  switch1.Update();
  switch2.Update();
  switch3.Update();
  switch4.Update();
}

void receive(const MyMessage &message) {

}

#include "SoftwareSerial.h"
#include "Troolean.h"

#include "BasketballLedControl.h"
#include "StramatelProtocolParser.h"

const int YellowLedOutputPin = 2;
const int RedLedOutputPin = 3;
const int ReceiverOutputPin = 4;
const int ReceiverEnablePin = 5;
const int DriverEnablePin = 6;
const int DriverInputPin = 7;
const unsigned long FlashDuration = 3000;
const unsigned long FlashDelay = 0;
const int Baudrate = 19200;

BasketballLedControl gameClockLight(RedLedOutputPin, FlashDuration, FlashDelay);
BasketballLedControl shotClockLight(YellowLedOutputPin, FlashDuration, FlashDelay);
SoftwareSerial serialInput(ReceiverOutputPin, DriverInputPin);
StramatelProtocolParser protocolParser;

void setup()
{
  // Configure the MAX485 DE & RE GPIO pins as LOW to only enable receiving
  pinMode(ReceiverEnablePin, OUTPUT);
  digitalWrite(ReceiverEnablePin, LOW);
  pinMode(DriverEnablePin, OUTPUT);
  digitalWrite(DriverEnablePin, LOW);

  // Configure serial receive (through SerialInput)
  serialInput.begin(Baudrate);

  // Configure serial send (through RX/TX)
  Serial.begin(Baudrate);

  // Delay further execution to ensure that millis() doesn't return 0 as reference
  delay(10);

  gameClockLight.setState(true, true);
  shotClockLight.setState(false, true);
  
  delay(FlashDuration);
  
  gameClockLight.setState(false, true);
  shotClockLight.setState(true, true);
  
  delay(FlashDuration);

  gameClockLight.setState(false, true);
  shotClockLight.setState(false, true);
}

Troolean gameClockZeroState(unknown);
Troolean gameClockZeroStateReceived(unknown);
Troolean shotClockZeroState(unknown);
Troolean shotClockZeroStateReceived(unknown);

void loop()
{
  // RECEIVE
  while (serialInput.available() > 0)
  {
    // Read next byte from input
    byte value = serialInput.read();

    Serial.write(value);

    protocolParser.push(value);
  }

  // PARSE
  if (protocolParser.isValidMessage())
  {
    // Parse game clock
    gameClockZeroStateReceived = protocolParser.isGameClockZero();
  
    // Parse shot clock
    shotClockZeroStateReceived = protocolParser.isShotClockZero();
  }

  // CONTROL
  // Check whether a delay to enable or timeout to disable the light has been exceeded
  gameClockLight.updateState();
  shotClockLight.updateState();

  // Check shot clock first since it's state may be overridden by the game clock
  
  // If we transition from non-zero to zero shot clock set yellow LED
  if (shotClockZeroState == false && shotClockZeroStateReceived == true)
    shotClockLight.setState(true);

  // If the shot clock light is active and the received state is not active, unset
  if (shotClockLight.getState() == true && shotClockZeroStateReceived != true)
    shotClockLight.setState(false);

  // Remember the received state
  shotClockZeroState = shotClockZeroStateReceived;

  // If we transition from non-zero to zero game clock unset yellow and set red LED
  if (gameClockZeroState == false && gameClockZeroStateReceived == true)
  {
    shotClockLight.setState(false);
    gameClockLight.setState(true);
  }

  // Remember the received state
  gameClockZeroState = gameClockZeroStateReceived;
}
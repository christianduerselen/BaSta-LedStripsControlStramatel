#include "SoftwareSerial.h"
#include "Troolean.h"

#include "BasketballLedControl.h"
#include "StramatelProtocolParser.h"

const int RS485RxPin = 2;
const int RS485TxPin = 3;
const int ReceiverEnablePin = 6;

const unsigned long FlashDuration = 3000;
const unsigned long FlashDelay = 0;

const int Baudrate = 19200;

const int Relay1Pin = 4;
const int Relay2Pin = 7;

BasketballLedControl gameClockLight(Relay1Pin, FlashDuration, FlashDelay);
BasketballLedControl shotClockLight(Relay2Pin, FlashDuration, FlashDelay);
SoftwareSerial serialInput(RS485RxPin, RS485TxPin);
StramatelProtocolParser protocolParser;

void setup()
{
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
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

const int clockPin = 7; // CLOCK signal connected to digital pin 7
const int dataPin = 8;  // DATA signal connected to digital pin 13

const unsigned long highTriggerTimeout = 120000; // 113,9 ms is the gap from the end of a measure message to the start of the next. More than this, the caliper is OFF.
const unsigned long lowTriggerTimeout = 800; // 631,208 us is the max gap from a lowTrigger and a next one in a message. More than this and we are out of sync.

const int maxNumberOfSync = 3;
const char versionTitle[16] = "Caliper reader";

bool bits[24];
int outOfSyncCounter;
bool readMeasureFailed;
bool caliperIsOn;
bool mmIsActive;
bool measureIsNegative;
int rawValue;
double measureValue;
char measureText[16];

char receivedCommand;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(clockPin, INPUT); // sets the digital pin 7 as input
  pinMode(dataPin, INPUT);  // sets the digital pin 8 as input

  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  lcd.setRGB(colorR, colorG, colorB); // set up the LCD's background color
  
  // Print the greeting message to the LCD, wait a second and clear it
  writeLineMessage(0, versionTitle);
  writeLineMessage(1, "by @TPCWare");
  delay(3000);
  writeLineMessage(1, "");
}

void loop() 
{
  // Try to get the measure
  readMeasure();
  
  // Show result on LCD
  if(!caliperIsOn)
  {
    writeLineMessage(1, "caliper is off");
    delay(1000);
  }
  else if(readMeasureFailed)
  {
    writeLineMessage(1, "Read error");
    delay(1000);
  }
  else
  {
    writeLineMessage(1, measureText);
    delay(100);
  }

  // receive command
  getCommand();
  
  if (receivedCommand == 'R') // Read command
  {
    // Send measure via serial port
    sendMeasure();
  
    // Send debug info via serial port
    // debugMeasure();
  }
}

void writeLineMessage(int line, String msg)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
  lcd.print(msg);
}

// Read the measure coming from the caliper.
// Set the result in global variables.
void readMeasure()
{
  int bitCounter;

  bool clockWasHigh;
  bool clockIsHigh;
  bool skippingData;
  
  readMeasureFailed = false;
  measureText[0] = 0;
  bitCounter = 0;
  outOfSyncCounter = 0;
  
  while (bitCounter < 24)
  {
    if (lowTrigger())
    {
      bits[bitCounter] = !digitalRead(dataPin); // Negation is required because the amplifier invert the DATA signal.
      bitCounter++;
    }
    else
    {
      outOfSyncCounter++;  // Increment the out of sync counter
      bitCounter = 0;      // Lets restart from bit one...
      
      // If we have not exceeded the max number of retries, we need to wait until the start of new message.
      // In case of timeout then we failed probably because the caliper is off.
      if (outOfSyncCounter > maxNumberOfSync or !highTrigger())
      {        
        readMeasureFailed = true;
        break;
      }
    }
  }

  if (readMeasureFailed)
  {
    caliperIsOn = false;
    mmIsActive = false;
    measureIsNegative = false;
    rawValue = 0;
    measureValue = 0.0;
    strcat(measureText, "N/A");
  }
  else
  {
    caliperIsOn = true;
    mmIsActive = !bits[23];
    measureIsNegative = bits[20];
    rawValue = bits[19];
    for (int i = 18; i >= 0; i--)
    {
      rawValue = rawValue << 1;
      rawValue += bits[i];
    }
    measureValue = ((measureIsNegative) ? -1 : 1) * rawValue / ((mmIsActive) ? 100.0 : 2000.0);
    dtostrf(measureValue, 4, ((mmIsActive) ? 2 : 4), measureText);
    strcat(measureText, ((mmIsActive) ? " mm" : " in"));
  }
}

// Wait for the Clock signal going from LOW value to HIGH value.
// Return true if this happens prior to the timeout, false elseware.
bool highTrigger()
{
  unsigned long start = micros();

  // Wait for the Low clock
  while(digitalRead(clockPin))
  {
      // Check timeout
      if(micros()-start > highTriggerTimeout)
      {
        return(false);
      }
  }

  // Wait for the High clock
  while(!digitalRead(clockPin))
  {
      // Check timeout
      if(micros()-start > highTriggerTimeout)
      {
        return(false);
      }
  }
  return true;
}

// Wait for the Clock signal going from HIGH value to LOW value.
// Return true if this happens prior to the timeout, false elseware.
bool lowTrigger()
{
  unsigned long start = micros();

  // Wait for the High clock
  while(!digitalRead(clockPin))
  {
      // Check timeout
      if(micros()-start > lowTriggerTimeout)
      {
        return(false);
      }
  }

  // Wait for the Low clock
  while(digitalRead(clockPin))
  {
      // Check timeout
      if(micros()-start > lowTriggerTimeout)
      {
        return(false);
      }
  }
  return true;
}

// Send the measure data via the serial port
void sendMeasure()
{
  Serial.print("[");
  Serial.print(measureText);
  Serial.println("]");
}

// Send the measurement debug data via the serial port
void debugMeasure()
{
  for (int i = 0; i < 24; i++)
  {
    Serial.print(bits[i] ? "1" : "0");
  }
  Serial.println("");
  for (int i = 23; i >= 0; i--)
  {
    Serial.print(bits[i] ? "1" : "0");
  }
  Serial.println("");  
  Serial.print("outOfSyncCounter: ");
  Serial.println(outOfSyncCounter);
  Serial.print("mmIsActive: ");
  Serial.println(mmIsActive);
  Serial.print("measureIsNegative: ");
  Serial.println(measureIsNegative ? "YES" : "NO");
  Serial.print("measureValue: ");
  Serial.println(measureValue);
  Serial.println("------------------");
}

// Get command
void getCommand()
{
  if (Serial.available() > 0)
  {
    receivedCommand = Serial.read();
  }
  else
  {
    receivedCommand = ' ';
  }
}



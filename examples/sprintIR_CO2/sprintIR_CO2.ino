// Test program for the SprintIR_CO2 sensor
//   Normal air is ~400 ppm (0.04 %)
//   Exhaled breathe around 4 %
//
// Matt Anderson, 2020

// This script is written to work with Arduino boards
// that have multiple serial ports (or a seperate USB
// serial port).  If you're using a board that shares
// the hardware serial port with the USB connection,
// (such as the Uno), create a SoftwareSerial port 
// and connect the CO2 Sensor (_CO2_SERIAL) to that
// port.

#include <sprintIR_CO2.h>

#define _CO2_SERIAL Serial1   // Serial port the CO2 Sensor is connected to
#define SERIAL_DEBUG Serial   // Serial port to see the outputs on

sprintIR_CO2 CO2(_CO2_SERIAL);

void setup()
{
  // Set up the comms ports
  Serial.begin(115200); delay(3500);
  Serial.print("SprintIR CO2 Sensor Test Program\n");
  Serial.print("================================\n");

  _CO2_SERIAL.begin(9600);

  // Set up a toggling LED
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Set up the encoder
  CO2._debug = 0;
  CO2.init();

  // Print out the firmware version
  CO2.get_version();

  // Calibrate the sensor to fresh air
  CO2.calibrate();

}

void loop() 
{

  // Update the sensor
  CO2.update();
  delay(50);
  
  // Print the results
  char buf[50];
  sprintf(buf, "CO2           : %9.3f [ %% ]\n",CO2.get_CO2());             Serial.print(buf);
  sprintf(buf, "CO2 Mutliplier: %7.1f   [ - ]\n",CO2.get_CO2_mutliplier()); Serial.print(buf);
  sprintf(buf, "CO2           : %5.0f     [ ppm ]\n",CO2.get_CO2_ppm());    Serial.print(buf);
  sprintf(buf, "Humidity      : %7.1f   [ %% ]\n",CO2.get_humidity()  );    Serial.print(buf);
  sprintf(buf, "Temperature   : %7.1f   [ C ] \n",CO2.get_temperature());   Serial.print(buf); // Doesn't work at the moment for some reason
  Serial.print("\n");

  // Delay until the next round
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
    
}
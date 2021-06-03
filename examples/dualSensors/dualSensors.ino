// Test program for the SprintIR_CO2 sensor
//   Normal air is ~400 ppm (0.04 %)
//   Exhaled breathe around 4 %
//
// Matt Anderson, 2020

#include <sprintIR_CO2.h>

#define _CO2_SERIAL0 Serial1
#define _CO2_SERIAL1 Serial2

#define SERIAL_DEBUG Serial

sprintIR_CO2 CO2_0(_CO2_SERIAL0);
sprintIR_CO2 CO2_1(_CO2_SERIAL1);

void setup()
{
  // Set up the comms ports
  Serial.begin(115200); delay(3500);
  Serial.print("SprintIR CO2 Sensor Test Program (Dual)\n");
  Serial.print("=======================================\n");

  _CO2_SERIAL0.begin(9600);
  _CO2_SERIAL1.begin(9600);

  // Set up a toggling LED
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Set up the sensor
  CO2_0._debug = 0; CO2_0.init();
  CO2_1._debug = 0; CO2_1.init();

  // Print out the firmware version
  Serial.print("Sensor 0\n"); CO2_0.get_version();
  Serial.print("Sensor 1\n"); CO2_1.get_version();

  // Calibrate the sensor to fresh air
  CO2_0.calibrate();
  CO2_1.calibrate();

}

void loop() 
{

  // Update the sensor
  CO2_0.update();
  CO2_1.update();
  delay(50);
  
  // Print the results
  char buf[50];
  sprintf(buf, "CO2           : %9.3f | %9.3f [ %% ]\n"          ,CO2_0.get_CO2()            ,CO2_1.get_CO2());            Serial.print(buf);
  sprintf(buf, "CO2 Mutliplier: %7.1f   | %7.1f   [ - ]\n"       ,CO2_0.get_CO2_mutliplier() ,CO2_1.get_CO2_mutliplier()); Serial.print(buf);
  sprintf(buf, "CO2           : %5.0f     | %5.0f     [ ppm ]\n" ,CO2_0.get_CO2_ppm()        ,CO2_1.get_CO2_ppm());        Serial.print(buf);
  sprintf(buf, "Humidity      : %7.1f   | %7.1f   [ %% ]\n"      ,CO2_0.get_humidity()       ,CO2_1.get_humidity()  );     Serial.print(buf);
  sprintf(buf, "Temperature   : %7.1f   | %7.1f   [ C ] \n"      ,CO2_0.get_temperature()    ,CO2_1.get_temperature());    Serial.print(buf); // Doesn't work at the moment for some reason
  Serial.print("\n");

  // Delay until the next round
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
    
}

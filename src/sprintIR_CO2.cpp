#include "sprintIR_CO2.h"

sprintIR_CO2::sprintIR_CO2(Stream& CO2_SERIAL) :
  _CO2_SERIAL(CO2_SERIAL)
{

}

// Functions for the CO2 sensor
void sprintIR_CO2::init()
{
  // Set Polling Mode (Mode 2) on the Sensor
  _CO2_SERIAL.print("K 2\r\n");

  // Set the n_samples for the digital filter (10)
  _CO2_SERIAL.print("A 10\r\n");

  // Get the CO2 multiplier
  update_CO2_multiplier();
  
  // Debugging
  if (_debug) { SERIAL_DEBUG.print("sprintIR_CO2: Initialised\n"); }

  // All done
  _enabled = 1;

  return;
}

uint32_t sprintIR_CO2::calibrate() {
  // Send the command to calibrate with fresh air
  uint8_t retries = 0;
  
  if (_debug) { SERIAL_DEBUG.print("sprintIR_CO2: Calibrating..."); }
  
  while (_zero_point_calibration == 0) {
        
    _CO2_SERIAL.print("G\r\n");

    // Wait for the response
    delay(50);
    read_sensor_buffer();
    
    // Create an exit loop
    if (retries > 5) {
        // Failed to calibrate with fresh air, exit out
        if (_debug) { SERIAL_DEBUG.print("...FAILED!\n"); }
        return (_zero_point_calibration);
        
    } else {
        retries++;
    }
    
  }
  
  // Debugging
  if (_debug) { SERIAL_DEBUG.print(_zero_point_calibration); SERIAL_DEBUG.print("...Done!\n"); }

  // Sensor calibrated
  return (_zero_point_calibration);
  
}

unsigned long sprintIR_CO2::t_calibrate() {
  return (_t_CO2_calibrate);
}

void sprintIR_CO2::update() {
  if (_debug) { SERIAL_DEBUG.print("sprintIR_CO2: Taking a sample\n"); }

  // Send command
  _CO2_SERIAL.print("Z\r\n");  // Take a CO2 reading
  read_sensor_buffer();
  _CO2_SERIAL.print("H\r\n");  // Take a humidity reading
  read_sensor_buffer();
  _CO2_SERIAL.print("T\r\n");  // Take a temperature reading
  read_sensor_buffer();
  
  // Record the time the sample was taken
  _t_sample_latest = millis();

  return;

}

void sprintIR_CO2::get_version() {
  _CO2_SERIAL.print("Y\r\n");
  delay(200);

  SERIAL_DEBUG.print("Version Information:\n\t");
  while(_CO2_SERIAL.available())
  {
    SERIAL_DEBUG.print((char) _CO2_SERIAL.read());
  }

  return;
}

void sprintIR_CO2::update_CO2_multiplier() {
  // Gets the CO2 multiplier from the CO2 sensor
  uint8_t retries = 0;
    
   do {
      
    if (_debug) { SERIAL_DEBUG.print("sprintIR_CO2: Getting CO2 Multiplier...\n"); }
    _CO2_SERIAL.print(".\r\n");
        
    // Wait for the response
    delay(50);
    read_sensor_buffer();
    
    if (retries > 20) {
        // something has gone wrong, exit the loop
        // and set the CO2 multiplier to -1 to tell the user
        _CO2_multiplier = -1.0f;
    } else {
        retries++;
    }
    
  } while (fabsf(_CO2_multiplier) < 0.1f);
  
  // Debugging
  if (_debug) { SERIAL_DEBUG.print("\t\tGot "); SERIAL_DEBUG.print(_CO2_multiplier); SERIAL_DEBUG.print("\n"); }

  return;
}

float sprintIR_CO2::get_CO2_mutliplier() {
  return (_CO2_multiplier);
}

void sprintIR_CO2::update_altitude(float altitude) {
    // Altitude Compensation
    // from Page 19, SprintIR-W-Data-Sheet-Rev-4.3.pdf
    
    // Calculate difference from sea level pressure (and convert to mBar)
    float pressure_diff = (_pressure - 101325.0f) * 0.01f;
    float comp_value = 8192.0f*(1.0f + (pressure_diff * 0.14f/100.0f));
    
    // Write compensation value to CO2 sensor
    _CO2_SERIAL.print("S");
    _CO2_SERIAL.print(comp_value,0);
    _CO2_SERIAL.print("\r\n");
    
    return;    
}

/*
float sprintIR_CO2::correct_CO2_reading(float CO2, float pressure) {
    // Convert pressure Pa -> mBar
    
    
    // Calculate Y factor
    Y=   2.37472E-30*pow(CO2,6) 
       - 2.70695E-25*pow(CO2,5)
       + 1.24012E-20*pow(CO2,4)
       - 2.91716E-16*pow(CO2,3)
       + 3.62939E-12*pow(CO2,2)
       - 1.82753E-08*pow(CO2,1)
       - 1.35129E-03;    // Correction factor
    
    float CO2_new = CO2/(1+Y*(1013.0-pressure)
    
    return (CO2_new)
    
} */

float sprintIR_CO2::get_CO2() { // CO2 in percent

  // Read whatever we have in the Serial buffers to update these values
  read_sensor_buffer();

  // Returns the latest CO2 reading
  return (_CO2_latest);
}

float sprintIR_CO2::get_CO2_ppm() { // CO2 in ppm

  // Read whatever we have in the Serial buffers to update these values
  read_sensor_buffer();

  // Returns the latest CO2 reading
  return (_CO2_latest*10000.0f);
}

float sprintIR_CO2::get_humidity() { // Humidity in percent

  // Read whatever we have in the Serial buffers to update these values
  read_sensor_buffer();

  // Return the latest value
  return (_humidity_latest);
}

float sprintIR_CO2::get_temperature() { // Temperature in deg

  // Read whatever we have in the Serial buffers to update these values
  read_sensor_buffer();

  // Return the latest value
  return (_temperature_latest);
}

unsigned long sprintIR_CO2::t_sample() {

  // Return when the sample was taken
  return (_t_sample_latest);
}

void sprintIR_CO2::read_sensor_buffer()
{
  static String value_string = "";    // string to hold input from sensor

  // Reads all the data we currently have in the serial buffer from the sensor
  while (_CO2_SERIAL.available())
  {
    uint8_t a = _CO2_SERIAL.read();

    switch (_packet_type)
    {
      case (_UNKNOWN_PACKET_TYPE) :
        // We don't know what type of packet we have so let's just start searching
        switch (a)
        {
          case ('Z'):
            // Getting a CO2 packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming CO2 Packet\n"); }
            _packet_type = _CO2_PACKET_TYPE;
            break;

          case ('H'):
            // Getting a Humidity packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming Humidity Packet\n"); }
            _packet_type = _HUMIDITY_PACKET_TYPE;
            break;

          case ('T'):
            // Getting a Temperature packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming Temperature Packet\n"); }
            _packet_type = _TEMPERATURE_PACKET_TYPE;
            break;

          case ('.'):
            // Getting a multiplier packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming CO2 Mutliplier Packet\n"); } 
            _packet_type = _MULTIPLIER_PACKET_TYPE;
            break;
            
          case ('G'):
            // Getting a zero-point calibration packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming Zero Point Calibration Packet\n"); } 
            _packet_type = _ZERO_POINT_CALIBRATION_TYPE;
            break;      

          case ('S'):
            // Getting a zero-point calibration packet
            if (_debug > 1) { SERIAL_DEBUG.print("sprintIR_CO2: Incoming Altitude Calibration Packet\n"); } 
            _packet_type = _ALTITUDE_CALIBRATION_TYPE;
            break;           

          default:
            // Don't know yet, keep searching
            break;

        }

        break;

      case (_CO2_PACKET_TYPE) :
      case (_HUMIDITY_PACKET_TYPE) :
      case (_TEMPERATURE_PACKET_TYPE) :
      case (_MULTIPLIER_PACKET_TYPE):
      case (_ZERO_POINT_CALIBRATION_TYPE):
      case (_ALTITUDE_CALIBRATION_TYPE):
        // Add the incoming data to the value_in string
        if (isDigit(a))
        {
          value_string += (char)a;

        }

        break;

      default :
      if (_debug)
      {
        SERIAL_DEBUG.print("sprintIR_CO2: Unknown state (");
        SERIAL_DEBUG.print(_packet_type);
        SERIAL_DEBUG.print(")\n");
      }
        break;

    }

    // Check to see if we've completed the string (\r character) which can then be converted to the number
    if (a == '\r')
    {
      long value_int = value_string.toInt();

      if (_debug > 1)
      {
      SERIAL_DEBUG.print("sprintIR_CO2: Got ");
      SERIAL_DEBUG.print(value_string);
      SERIAL_DEBUG.print(" in value_string buffer\n");
      }

      switch (_packet_type)
      {
        case (_CO2_PACKET_TYPE) :
          _CO2_latest = (float) value_int / 10000.0f * _CO2_multiplier;

          break;
        case (_HUMIDITY_PACKET_TYPE) :
          _humidity_latest = (float) value_int / 10.0f;
          break;

        case (_TEMPERATURE_PACKET_TYPE) :
          _temperature_latest = (float) (value_int - 1000.0f) / 10.0f;
          break;

        case (_MULTIPLIER_PACKET_TYPE) :
          _CO2_multiplier = float (value_int);
          break;

        case (_ZERO_POINT_CALIBRATION_TYPE) :
          _zero_point_calibration = int32_t (value_int);
          break;
          
        case (_ALTITUDE_CALIBRATION_TYPE) :
          _altitude_calibration_code = int32_t (value_int);
          break;
          
        default :
          // do nothing
          break;
      }

      // Switch the state machine back to searching for a data packet
      value_string = "";
      _packet_type = _UNKNOWN_PACKET_TYPE;

    }
  }
}

// Library for reading SprintIR CO2 sensor
//
// CO2 divisor (from datasheet, page 9)
//    Sensor range up to   2 % :   1.0f
//    Sensor range up to  65 % :  10.0f
//    Sensor range up to 100 % : 100.0f
//
// Matt Anderson, 2020

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include "data_structures.h"

#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG Serial
#endif

class sprintIR_CO2 {
  public :
    sprintIR_CO2(Stream& CO2_SERIAL);

    void init();
    uint32_t calibrate();
    unsigned long t_calibrate();
    void update();

    void get_version();

    float get_CO2();
    float get_CO2_ppm();
    float get_humidity();
    float get_temperature();
    float get_CO2_mutliplier();
    unsigned long t_sample();
    
    void update_altitude(float altitude);

    uint8_t _debug = 0;

  private :
    Stream& _CO2_SERIAL;

    void update_CO2_multiplier();

    float _CO2_latest = 0.0f;
    float _humidity_latest = 0.0f;
    float _temperature_latest = 0.0f;
    unsigned long _t_sample_latest = 0;
    bool _enabled = 0;
    
    float _pressure = 1010352.0f;        // Ambient Pressure in [ Pa ]
    float _CO2_multiplier = 0.0f;
    float _CO2_correction_factor = 1.0f;

    uint32_t _zero_point_calibration = 0;
    uint32_t _altitude_calibration_code = 0;

    uint8_t _packet_type = _UNKNOWN_PACKET_TYPE;

    void read_sensor_buffer();
    const unsigned long _t_CO2_calibrate = 10UL * 1000; // Time required to pull the calibrate pin high on the CO2 sensor to run the calibration in [ ms ]


};

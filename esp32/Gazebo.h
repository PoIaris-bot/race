#ifndef GAZEBO_H
#define GAZEBO_H

#include "Arduino.h"

class Gazebo {
public:
  Gazebo();

  ~Gazebo();

  void begin(HardwareSerial &serial, int baud_rate = 115200);

  void end();

  bool isOpen();

  void write_motor(float velocity);

  void write_servo(float angle);

  float read_lidar(int angle);

private:
  HardwareSerial *bound_serial;

  void write();

  float x = 0;
  float z = 0;

  String message;
  float ranges[360];

  void callback(void);
};

#endif  //GAZEBO_H

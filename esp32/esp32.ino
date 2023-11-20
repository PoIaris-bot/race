#include "Gazebo.h"

Gazebo gazebo;

void setup() {
    gazebo.begin(Serial, 115200);
}

void loop() {
    gazebo.write_servo(0.0);
    gazebo.write_motor(2.0);
    delay(2000);

    gazebo.write_servo(-0.5);
    gazebo.write_motor(2.0);
    delay(1000);
}

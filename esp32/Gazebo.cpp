#include "Gazebo.h"

Gazebo::Gazebo() : bound_serial(NULL) {
}

Gazebo::~Gazebo() {
    end();
}

void Gazebo::begin(HardwareSerial &serial, int baud_rate) {
    if (isOpen()) {
        end();
    }
    bound_serial = &serial;
    bound_serial->end();
    bound_serial->begin(baud_rate);
    bound_serial->onReceive([this] {
        callback();
    });
}

void Gazebo::end() {
    if (isOpen()) {
        bound_serial->end();
        bound_serial = NULL;
    }
}

bool Gazebo::isOpen() {
    return bound_serial ? true : false;
}

void Gazebo::write_motor(float velocity) {
    x = velocity;
    write();
}

void Gazebo::write_servo(float angle) {
    z = angle;
    write();
}

int constraint(int value, int lower, int upper) {
    return (value > upper ? upper : (value < lower ? lower : value));
}

void Gazebo::write() {
    int velocity = constraint(x * 500 + 1000, 0, 2000);
    int angle = constraint(z * 2000 + 1000, 0, 2000);
    bound_serial->println('b' + String(velocity) + ',' + String(angle) + 'e');
}

void Gazebo::callback(void) {
    while (bound_serial->available()) {
        char c = bound_serial->read();
        if (('0' <= c and c <= '9') || ('a' <= c && c <= 'z') || c == '.' || c == ',') {
            message += c;
        }
    }
    if (!message.isEmpty()) {
        int begin = -1;
        int middle = -1;
        int end = -1;
        do {
            begin = message.indexOf('b');
            middle = message.indexOf(',', begin);
            end = message.indexOf('e', middle);
            if (begin != -1 && middle != -1 && end != -1) {
                int angle = atoi(message.substring(begin + 1, middle).c_str());
                float range = atof(message.substring(middle + 1, end).c_str());
                ranges[angle] = range;
                message = message.substring(end + 1);
            }
        } while (begin != -1 && middle != -1 && end != -1);
    }
}

float Gazebo::read_lidar(int angle) {
    return ranges[constraint(angle, 0, 359)];
}
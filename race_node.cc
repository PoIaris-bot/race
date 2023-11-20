#include <iostream>
#include <boost/asio.hpp>
#include <ignition/msgs/twist.pb.h>
#include <ignition/msgs/laserscan.pb.h>
#include <ignition/transport/Node.hh>

std::string topic_pub = "/vehicle/cmd_vel";
ignition::transport::Node node;
auto publisher = node.Advertise<ignition::msgs::Twist>(topic_pub);

boost::asio::io_service io;
boost::asio::serial_port serial(io);

void callback(const ignition::msgs::LaserScan &data) {
    std::string message;
    for (int angle = 0; angle < 360; angle++) {
        double range = data.ranges(angle);
        message = 'b' + std::to_string(angle) + ',' + std::to_string(std::isinf(range) ? 10.0 : range) + 'e';
        boost::asio::write(serial, boost::asio::buffer(message));
    }
}

int main(int argc, char **argv) {
    std::string port = argc > 1 ? argv[1] : "/dev/ttyACM0";
    int baud_rate = argc > 2 ? std::stoi(argv[2]) : 115200;
    try {
        serial.open(port);
        serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::string topic_sub = "/vehicle/lidar";
    if (!node.Subscribe(topic_sub, callback)) {
        std::cerr << "Error subscribing to topic [" << topic_sub << "]" << std::endl;
        return -1;
    }

    std::string message;
    while (true) {
        char c;
        do {
            boost::asio::read(serial, boost::asio::buffer(&c, 1));
            if (('0' <= c and c <= '9') || ('a' <= c && c <= 'z') || c == '.' || c == ',') message += c;
        } while (c != '\n');

        if (!message.empty()) {
            auto begin = message.find('b');
            auto middle = message.find(',', begin);
            auto end = message.find('e', middle);
            if (begin != std::string::npos && middle != std::string::npos && end != std::string::npos) {
                try {
                    double velocity = (std::stod(message.substr(begin + 1, middle - begin)) - 1000) / 500;
                    double angle = (std::stod(message.substr(middle + 1, end - middle)) - 1000) / 2000;

                    ignition::msgs::Twist data;
                    data.mutable_linear()->set_x(velocity);
                    data.mutable_angular()->set_z(angle);
                    publisher.Publish(data);
                }
                catch (std::exception &e) {
                    std::cerr << "Exception: " << e.what() << std::endl;
                }
            }
            message = "";
        }
    }
}
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <cstring>
#include "serial/serial.h"
#include "rclcpp/rclcpp.hpp"
#include "message_filters/subscriber.h"
#include "message_filters/time_synchronizer.h"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int8.hpp"
// #include "robot_interfaces/msg/qt_recv.hpp"
#include "robot_interfaces/msg/qt_pub.hpp"


#define BAUDRATE 115200


using namespace std::chrono_literals;
using std::placeholders::_1;

class JointPosPub : public rclcpp :: Node
{
    public:
        JointPosPub(const std::string &node_name) : Node(node_name), stop_flag(true), rx_buff(30) {
            // receive the cubic polynomials from the robot_control node
            subscriber_ = this->create_subscription<sensor_msgs::msg::JointState>("motor_cmd", 10, std::bind(&JointPosPub::joint_pos_callback, this, _1));
            // receive the qt cmd from the DRobot app
            subscriber_motor_states_ = this->create_subscription<robot_interfaces::msg::QtPub>("motor_states_req", 10, std::bind(&JointPosPub::motor_states_request_callback, this, _1));
            // publish the joint attitude from the real motors
            publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);

            
            ros_ser.setPort("/dev/ttyUSB0");
            ros_ser.setBaudrate(BAUDRATE);
            ros_ser.setBytesize(serial::eightbits);
            ros_ser.setParity(serial::parity_none);
            ros_ser.setStopbits(serial::stopbits_one);
            serial::Timeout to = serial::Timeout::simpleTimeout(1000);
            ros_ser.setTimeout(to);

            try {
                ros_ser.open();
                if (ros_ser.isOpen()) {
                    RCLCPP_INFO(this->get_logger(), "serial port is open");
                    receive_thread = std::thread(&JointPosPub::receiveData, this);
                } else {
                    RCLCPP_INFO(this->get_logger(), "serial port error");
                }
            }
            catch (const std::exception &e) {
                RCLCPP_ERROR(this->get_logger(), "Exception opening serial port: %s", e.what());
            }
        }

        ~JointPosPub() {
            stop_flag = false;
            if (receive_thread.joinable()) {
                receive_thread.join();
            }
            if (send_thread.joinable()) {
                send_thread.join();
            }
        }

        
    private:
        serial::Serial ros_ser;
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;
        rclcpp::Subscription<robot_interfaces::msg::QtPub>::SharedPtr subscriber_motor_states_;
        rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
        std::thread receive_thread;
        std::thread send_thread;
        std::atomic<bool> stop_flag;
        std::vector<uint8_t> rx_buff;
        std::vector<double> joint_group_positions;
        std::uint8_t working_mode;
        std::atomic<bool> qt_flag;
        

        void joint_pos_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
            for (size_t i = 0; i < msg->position.size(); i++) {
                RCLCPP_INFO(this->get_logger(), "joint %lu: pos: %lf", i, msg->position[i]);
            }
            working_mode = 0x00;   // 使用rviz拖动控制
            sendData(msg->position);
        }

        void motor_states_request_callback(const robot_interfaces::msg::QtPub::SharedPtr msg) {
            RCLCPP_INFO(this->get_logger(), "working mode: %d", msg->working_mode);

            working_mode = msg->working_mode;
            sendData(msg->joint_group_positions);
        }

        void receiveData() {
            while (rclcpp::ok() && stop_flag) {
                size_t bytes_read = ros_ser.read(rx_buff.data(), rx_buff.size());
                if (bytes_read > 0) {
                    if (rx_buff[0] == 0x55 && rx_buff[25] == 0xAA) {
                        sensor_msgs::msg::JointState joint_state_fdb;
                        joint_state_fdb.header.stamp = this->get_clock()->now();
                        joint_state_fdb.name.resize(6);
                        joint_state_fdb.position.resize(6);
                        
                        for (int i = 0; i < 6; i++) {
                            float position;
                            memcpy(&position, &rx_buff[i * 4 + 1], sizeof(float));
                            joint_state_fdb.position[i] = (double)position / 57.296;
                        }

                        joint_state_fdb.name[0] = "joint1";
                        joint_state_fdb.name[1] = "joint2";
                        joint_state_fdb.name[2] = "joint3";
                        joint_state_fdb.name[3] = "joint4";
                        joint_state_fdb.name[4] = "joint5";
                        joint_state_fdb.name[5] = "joint6";

                        RCLCPP_INFO(this->get_logger(), "Received motor_msg: %f, %f, %f, %f, %f, %f", joint_state_fdb.position[0], joint_state_fdb.position[1],
                                                                                                    joint_state_fdb.position[2], joint_state_fdb.position[3], 
                                                                                                    joint_state_fdb.position[4], joint_state_fdb.position[5]);
                        publisher_->publish(joint_state_fdb);
                    }
                }
            }
        }
        void sendData(const std::vector<double>& positions) {
            if (positions.size() <= 6) {
                uint8_t tx_data[29] = {0};
                tx_data[0] = 0x55;
                tx_data[1] = 0xAA;
                tx_data[2] = working_mode;
                for (int i = 0; i < 6; i++) {
                    float position = positions[i];
                    memcpy(&tx_data[i * 4 + 3], &position, sizeof(float));
                }
                tx_data[26] = 0xEB;
                tx_data[27] = 0xAA;
                ros_ser.write(tx_data, sizeof(tx_data));
            }
            else {
                RCLCPP_ERROR(this->get_logger(), "Invalid number of joint positions received. Expected 6, got %lu", positions.size());
            }
        }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<JointPosPub>("robot_driver");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
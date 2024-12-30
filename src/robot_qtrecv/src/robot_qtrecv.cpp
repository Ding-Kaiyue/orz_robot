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
#include "std_msgs/msg/int32_multi_array.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <robot_interfaces/msg/qt_recv.hpp>
#include <robot_interfaces/msg/arm_state.hpp>



#define BAUDRATE 115200

using std::placeholders::_1;

class QtMessageProcessor : public rclcpp :: Node
{
    public:
        QtMessageProcessor(const std::string &node_name) : Node(node_name), stop_flag(true), rx_buff(100) {
            subscriber_states_ = this->create_subscription<robot_interfaces::msg::ArmState>("arm_states", 10, std::bind(&QtMessageProcessor::arm_states_callback, this, _1));
            publisher_ = this->create_publisher<robot_interfaces::msg::QtRecv>("qt_cmd", 10);
            qt_ser.setPort("/dev/ttyUSB0");
            qt_ser.setBaudrate(BAUDRATE);
            qt_ser.setBytesize(serial::eightbits);
            qt_ser.setParity(serial::parity_none);
            qt_ser.setStopbits(serial::stopbits_one);
            serial::Timeout to = serial::Timeout::simpleTimeout(1000);
            qt_ser.setTimeout(to);

            try {
                qt_ser.open();
                if (qt_ser.isOpen()) {
                    RCLCPP_INFO(this->get_logger(), "serial port is open");
                    receive_thread = std::thread(&QtMessageProcessor::receive_qt_data, this);
                } else {
                    RCLCPP_INFO(this->get_logger(), "serial port error");
                }
                publisher_->publish(qt_cmd);
            } catch (const std::exception &e) {
                RCLCPP_ERROR(this->get_logger(), "Exception opening serial port: %s", e.what());
                publisher_->publish(qt_cmd);
            }
        }

        ~QtMessageProcessor() {
            stop_flag = false;
            if (receive_thread.joinable()) {
                receive_thread.join();
            }
            if (send_thread.joinable()) {
                send_thread.join();
            }

            try
            {
                if (qt_ser.isOpen()) {
                    qt_ser.close();
                }
                publisher_->publish(qt_cmd);
            }
            catch(const std::exception& e)
            {
                RCLCPP_INFO(this->get_logger(), "Exception opening serial port: %s", e.what());
            }
            
        }
    private:
        serial::Serial qt_ser;
        rclcpp::Subscription<robot_interfaces::msg::ArmState>::SharedPtr subscriber_states_;
        rclcpp::Publisher<robot_interfaces::msg::QtRecv>::SharedPtr publisher_;
        robot_interfaces::msg::QtRecv qt_cmd;
        geometry_msgs::msg::Pose end_effector_pose;     // 末端执行器的当前姿态
        tf2::Quaternion end_effector_quat;    // 末端执行器的当前四元数

        std::thread receive_thread;
        std::thread send_thread;
        std::atomic<bool> stop_flag;
        std::vector<uint8_t> rx_buff;

        void receive_qt_data()
        {
            while (rclcpp::ok() && stop_flag) {
                size_t bytes_read = qt_ser.read(rx_buff.data(), rx_buff.size());
                if (bytes_read > 0) {
                    std_msgs::msg::Int32MultiArray qt_cmd_raw;
                    qt_cmd_raw.data.resize(bytes_read);
                    std::memcpy(&qt_cmd_raw.data[0], rx_buff.data(), bytes_read);

                    switch (qt_cmd_raw.data[0]) {
                        case 0x00: {    // Functions
                            qt_cmd.working_mode = qt_cmd_raw.data[3];
                            publisher_->publish(qt_cmd);
                            break;
                        }
                        case 0x01: {    // z
                            qt_cmd.working_mode = 0x07;
                            qt_cmd.arm_pose_goal.position = end_effector_pose.position;
                            qt_cmd.arm_pose_goal.orientation = end_effector_pose.orientation;
                            qt_cmd.gripper_goal.data = {qt_cmd_raw.data[4], qt_cmd_raw.data[5], qt_cmd_raw.data[6]};
                            if (qt_cmd_raw.data[1] == 0x01) {           // Position                  
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "zpos+%d", qt_cmd_raw.data[3]);                                    
                                    qt_cmd.arm_pose_goal.position.z = end_effector_pose.position.z + 0.01 * qt_cmd_raw.data[3];
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "zpos-%d", qt_cmd_raw.data[3]);                                    
                                    qt_cmd.arm_pose_goal.position.z = end_effector_pose.position.z - 0.01 * qt_cmd_raw.data[3];                                    
                                }
                                
                            } 
                            else if (qt_cmd_raw.data[1] == 0x02) {    // Rotation
                                // 绕z轴旋转的旋转四元数
                                tf2::Quaternion rotation_quat;
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "zrot+%d", qt_cmd_raw.data[3]);
                                    rotation_quat.setRPY(0, 0, qt_cmd_raw.data[3] * 5 * M_PI / 180.0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "zrot-%d", qt_cmd_raw.data[3]);
                                    rotation_quat.setRPY(0, 0, -qt_cmd_raw.data[3] * 5 * M_PI / 180.0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                }
                            }
                            publisher_->publish(qt_cmd);
                            break;
                        }
                        case 0x02: {    // x
                            qt_cmd.working_mode = 0x07;
                            qt_cmd.arm_pose_goal.position = end_effector_pose.position;
                            qt_cmd.arm_pose_goal.orientation = end_effector_pose.orientation;

                            if (qt_cmd_raw.data[1] == 0x01) {           // Position
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "xpos+%d", qt_cmd_raw.data[3]);
                                    qt_cmd.arm_pose_goal.position.x = end_effector_pose.position.x + 0.01 * qt_cmd_raw.data[3];                                    
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "xpos-%d", qt_cmd_raw.data[3]);
                                    qt_cmd.arm_pose_goal.position.x = end_effector_pose.position.x - 0.01 * qt_cmd_raw.data[3];
                                }
                            } else if (qt_cmd_raw.data[1] == 0x02) {    // Rotation
                                // 绕x轴旋转的旋转四元数
                                tf2::Quaternion rotation_quat;
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "xrot+%d", qt_cmd_raw.data[3]);                                    
                                    rotation_quat.setRPY(qt_cmd_raw.data[3] * 5 * M_PI / 180.0, 0, 0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "xrot-%d", qt_cmd_raw.data[3]);
                                    rotation_quat.setRPY(-qt_cmd_raw.data[3] * 5 * M_PI / 180.0, 0, 0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                }
                            }
                            publisher_->publish(qt_cmd);
                            break;
                        }
                        case 0x03: {    // y
                            qt_cmd.working_mode = 0x07;
                            qt_cmd.arm_pose_goal.position = end_effector_pose.position;
                            qt_cmd.arm_pose_goal.orientation = end_effector_pose.orientation;

                            if (qt_cmd_raw.data[1] == 0x01) {           // Position
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "ypos+%d", qt_cmd_raw.data[3]);
                                    qt_cmd.arm_pose_goal.position.y = end_effector_pose.position.y + 0.01 * qt_cmd_raw.data[3];
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "ypos-%d", qt_cmd_raw.data[3]);
                                    qt_cmd.arm_pose_goal.position.y = end_effector_pose.position.y - 0.01 * qt_cmd_raw.data[3];
                                }
                            } else if (qt_cmd_raw.data[1] == 0x02) {    // Rotation
                                // 绕y轴旋转的旋转四元数
                                tf2::Quaternion rotation_quat;
                                if (qt_cmd_raw.data[2] == 0x01) {           // +
                                    RCLCPP_INFO(this->get_logger(), "yrot+%d", qt_cmd_raw.data[3]);
                                    rotation_quat.setRPY(0, qt_cmd_raw.data[3] * 5 * M_PI / 180.0, 0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                } else if (qt_cmd_raw.data[2] == 0x00) {    // -
                                    RCLCPP_INFO(this->get_logger(), "yrot-%d", qt_cmd_raw.data[3]);
                                    rotation_quat.setRPY(0, -qt_cmd_raw.data[3] * 5 * M_PI / 180.0, 0);
                                    // 旋转后的新四元数
                                    tf2::Quaternion new_quat = rotation_quat * end_effector_quat;
                                    new_quat.normalize();
                                    // 新四元数转换回geometry_msgs::msg::Quaternion
                                    qt_cmd.arm_pose_goal.orientation = tf2::toMsg(new_quat);
                                }
                            }
                            publisher_->publish(qt_cmd);
                            break;
                        }
                        case 1145: {
                            RCLCPP_INFO(this->get_logger(), "%d, %d, %d", qt_cmd_raw.data[1], qt_cmd_raw.data[2], qt_cmd_raw.data[3]);
                            RCLCPP_INFO(this->get_logger(), "%d, %d, %d", qt_cmd_raw.data[4], qt_cmd_raw.data[5], qt_cmd_raw.data[6]);
                            RCLCPP_INFO(this->get_logger(), "%d, %d, %d", qt_cmd_raw.data[7], qt_cmd_raw.data[8], qt_cmd_raw.data[9]);

                            qt_cmd.joint_angles_goal.data = {qt_cmd_raw.data[1], qt_cmd_raw.data[2], qt_cmd_raw.data[3],
                                                             qt_cmd_raw.data[4], qt_cmd_raw.data[5], qt_cmd_raw.data[6]};

                            qt_cmd.working_mode = 0x08;
                            publisher_->publish(qt_cmd);                            
                            break;
                        }
                        default:
                            RCLCPP_ERROR(this->get_logger(),"error receivedID");
                            break;
                    }
                }
            }
        }

        void arm_states_callback(const robot_interfaces::msg::ArmState::SharedPtr msg)
        {
            //tf2::fromMsg(msg->end_effector_quat, end_effector_quat);
            tf2::fromMsg(msg->end_effector_pose.orientation, end_effector_quat);
            end_effector_pose = msg->end_effector_pose;
        }        
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<QtMessageProcessor>("robot_qtrecv");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
#include <rclcpp/rclcpp.hpp>
#include "can_msgs/msg/frame.hpp"
#include "ros2_socketcan/socket_can_id.hpp"
#include "ros2_socketcan/socket_can_sender.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "robot_interfaces/msg/qt_pub.hpp"
#include "robot_interfaces/msg/robot_control_msg.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"
#include <iostream>
#include <cstdint>

#define POSITION_MODE 0x05
#define SPEED_MODE 0x04
#define EFFORT_MODE 0x02

#define MOTOR_ENABLE 0x01
#define MOTOR_DISABLE 0x00
#define MOTOR_NUM 0x06
#define CAN_FDB_ID 0x200

using namespace drivers::socketcan;
using std::placeholders::_1;

// 06 01 04 00 00 00 00 enable
class SocketCanSenderNode : public rclcpp :: Node 
{
    public:
        SocketCanSenderNode(const std::string& node_name) : Node(node_name)
        {
            // Rviz Controller
            subscriber_ = this->create_subscription<sensor_msgs::msg::JointState>("motor_cmd", 10, std::bind(&SocketCanSenderNode::joint_pos_callback, this, _1));
            // Qt Controller
            subscriber_motor_states_ = this->create_subscription<robot_interfaces::msg::QtPub>("motor_states_req", 10, std::bind(&SocketCanSenderNode::motor_states_request_callback, this, _1));
            subscriber_gripper_states_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>("gripper_cmd", 10, std::bind(&SocketCanSenderNode::gripper_states_request_callback, this, _1));
            // Keyboards Controller
            subscriber_motor_msgs_ = this->create_subscription<robot_interfaces::msg::RobotControlMsg>("motor_msgs", 10, std::bind(&SocketCanSenderNode::motor_msg_cmd_callback, this, _1));

            subscriber_motor_zero_position_set_ = this->create_subscription<std_msgs::msg::Int32>("motor_zero_position_set", 10, std::bind(&SocketCanSenderNode::motor_zero_position_set_callback, this, _1));
            timer_ = this->create_wall_timer(std::chrono::milliseconds(20), std::bind(&SocketCanSenderNode::timer_callback, this));
        }
    private:
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;
        rclcpp::Subscription<robot_interfaces::msg::QtPub>::SharedPtr subscriber_motor_states_;
        rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr subscriber_gripper_states_;
        rclcpp::Subscription<robot_interfaces::msg::RobotControlMsg>::SharedPtr subscriber_motor_msgs_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_motor_zero_position_set_;
        rclcpp::TimerBase::SharedPtr timer_;
        union FloatUintConverter {
            float f;
            uint32_t u;
        };
        
        uint8_t motor_id = 0x01;
        uint8_t current_mode = POSITION_MODE;

        void motor_zero_position_set_callback(const std_msgs::msg::Int32::SharedPtr msg) {
            motor_req(msg->data, MOTOR_DISABLE, POSITION_MODE, 0.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            motor_zero_position_set(msg->data + 0x400);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            motor_zero_position_write_to_flash(msg->data + 0x400);
            RCLCPP_INFO(this->get_logger(), "==========================================");
        }

        // rviz control
        void joint_pos_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
            for (size_t i = 0; i < msg->position.size(); i++) {
                motor_req(i+1, MOTOR_ENABLE, POSITION_MODE, msg->position[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        // QT control
        void motor_states_request_callback(const robot_interfaces::msg::QtPub::SharedPtr msg) {
            if (msg->working_mode != 0x06) {
                for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
                    motor_req(i+1, msg->enable_flag, POSITION_MODE, msg->joint_group_positions[i]);
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
                // gripper_req(msg->gripper_msgs[0], msg->gripper_msgs[1], msg->gripper_msgs[2]);
            } else {
                for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
                    motor_req(i+1, msg->enable_flag, SPEED_MODE, 0.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
            }
        }

        void motor_msg_cmd_callback(const robot_interfaces::msg::RobotControlMsg::SharedPtr msg) {
            for (size_t i = 0; i < msg->motor_mode.size(); i++) {
                motor_req(i+1, msg->motor_enable_flag[i], msg->motor_mode[i], msg->motor_msg[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
            }
        }

        void gripper_states_request_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
            gripper_req(msg->data[0], msg->data[1], msg->data[2]);
            RCLCPP_INFO(this->get_logger(), "Gripper msg sent------------------");
        }

        void motor_req(uint32_t id, bool enable_flag, uint8_t motor_mode, const double data) {
            uint8_t tx_data[8] = {0};
            SocketCanSender sender("can0", false);

            float data_f = (static_cast<float>(data)) * 180.0f / 3.141592654f;
            FloatUintConverter converter;
            converter.f = data_f;
            uint32_t data_uint32 = converter.u;

            CanId canid(id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x06;
            tx_data[1] = (uint8_t)enable_flag;      
            tx_data[2] = motor_mode;
            
            tx_data[3] = data_uint32 >> 24;
            tx_data[4] = data_uint32 >> 16;
            tx_data[5] = data_uint32 >> 8;
            tx_data[6] = data_uint32 && 0xFF;
            sender.send(tx_data, sizeof(tx_data), canid, std::chrono::seconds(1));
        }

        void gripper_req(uint8_t gripper_position, uint8_t gripper_velocity, uint8_t gripper_effort) {
            uint8_t tx_data[8] = {0};
            SocketCanSender sender("can0", false);
            CanId canid(0x3F, 0, FrameType::DATA, StandardFrame);       // 夹爪ID定为0x3F
            tx_data[0] = 0x03;
            tx_data[1] = gripper_position;
            tx_data[2] = gripper_velocity;
            tx_data[3] = gripper_effort;
            sender.send(tx_data, sizeof(tx_data), canid, std::chrono::seconds(1));
        }

        void motor_fdb(uint16_t id, uint8_t mode) {
            uint8_t tx_data[8] = {0};
            SocketCanSender sender("can0", false);

            CanId canid(id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x01;
            tx_data[1] = mode;
            sender.send(tx_data, sizeof(tx_data), canid, std::chrono::seconds(1));
        }

        void motor_zero_position_set(uint32_t id) {
            uint8_t tx_data[8] = {0};
            SocketCanSender sender("can0", false);

            CanId canid(id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x01;
            tx_data[1] = 0x04;
            sender.send(tx_data, sizeof(tx_data), canid, std::chrono::seconds(1));
        }

        void motor_zero_position_write_to_flash(uint16_t id) {
            uint8_t tx_data[8] = {0};
            SocketCanSender sender("can0", false);

            CanId canid(id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x01;
            tx_data[1] = 0x02;
            sender.send(tx_data, sizeof(tx_data), canid, std::chrono::seconds(1));
        }

        void timer_callback() {
            motor_fdb(CAN_FDB_ID + motor_id, current_mode);
            if (current_mode == POSITION_MODE) {
                current_mode = EFFORT_MODE;
            } else {
                current_mode = POSITION_MODE;
                motor_id++;
                if (motor_id > MOTOR_NUM) {
                    motor_id = 0x01;
                }
            }
        }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SocketCanSenderNode>("robot_driver_cantx");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

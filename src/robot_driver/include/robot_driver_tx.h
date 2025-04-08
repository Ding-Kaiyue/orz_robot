#ifndef __ROBOT_DIRVER_TX_H__
#define __ROBOT_DRIVER_TX_H__

#include <rclcpp/rclcpp.hpp>
#include "can_msgs/msg/frame.hpp"
#include "ros2_socketcan/socket_can_id.hpp"
#include "ros2_socketcan/socket_can_sender.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "robot_interfaces/msg/qt_pub.hpp"
#include "robot_interfaces/msg/robot_control_msg.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"
#include <iostream>
#include <cstdint>
#include <cstring>

using namespace drivers::socketcan;
using std::placeholders::_1;

enum DataFdbClass_e {
    DATA1 = 0x01,
    DATA2,
    DATA3,
    DATA4
};
enum ExecuteResult_e {
    SUCCESS = 0x01,
    FAILED = 0xFF,
};
union FloatUintConverter_u {
    float f;
    uint32_t u;
};

// 发送接口类型
typedef enum {
    MOTOR_CTRL_TX = 0x000,
    FDB_REQ_TX = 0x200,
    FUNC_CTRL_TX = 0x400,
    PARM_RW_TX = 0x600,
} CommandID_Tx_e;

typedef enum {
    PARAM_R_TX = 0x01,
    PARAM_W_TX = 0x02,
} Operation_Method_e;

typedef enum {
    CURRENT_MODE = 0x02,
    EFFORT_POSITION_MODE = 0x03,
    SPEED_MODE = 0x04,
    POSITION_ABS_MODE = 0x05,
    POSITION_INC_MODE = 0x06
} Motor_Control_Mode_e;

typedef enum {
    MOTOR_ENABLE = 0x01,
    MOTOR_DISABLE = 0x00
} Motor_Enable_Mode_e;

typedef enum {
    ONCE = 0x00,
    CYCLE = 0x01
} Motor_Feedback_Method_e;

class tx_package_t {
    private:
        uint8_t tx_data[8] = {0};

        std::shared_ptr<SocketCanSender> sender = std::make_shared<SocketCanSender>("can0", false);
        std::unique_ptr<CanId> canid;
    public:
        // Parameter Read
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t operation_method, uint16_t params_addr) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = operation_method;
            tx_data[1] = params_addr >> 8;
            tx_data[2] = params_addr & 0xFF;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Motor Control Commands
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t control_enable_flag, uint8_t control_mode, float control_value) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            FloatUintConverter_u converter;
            converter.f = (static_cast<float>(control_value)) * 180.0f / 3.141592654f;
            uint32_t control_value_uint = converter.u;
            tx_data[0] = control_enable_flag;
            tx_data[1] = control_mode;
            tx_data[2] = 0;
            tx_data[3] = 0;
            tx_data[4] = control_value_uint >> 24;
            tx_data[5] = control_value_uint >> 16;
            tx_data[6] = control_value_uint >> 8;
            tx_data[7] = control_value_uint & 0xFF;
            RCLCPP_INFO(rclcpp::get_logger("tx_package_t"), "Motor %d cmd position: %f", motor_id, control_value);
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Motor Feedback Request
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t feedback_data, uint8_t feedback_method) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = feedback_data;
            tx_data[1] = feedback_method;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Function Operation interfaces
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t func_operation_code) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = func_operation_code;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Parameter Write
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t operation_method, uint16_t param_addr, uint8_t param_type, int32_t param_data) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = operation_method;
            tx_data[1] = param_addr >> 8;
            tx_data[2] = param_addr & 0xFF;
            tx_data[3] = param_type;
            tx_data[4] = param_data >> 24;
            tx_data[5] = param_data >> 16;
            tx_data[6] = param_data >> 8;
            tx_data[7] = param_data & 0xFF;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Parameter Write
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t operation_method, uint16_t param_addr, uint8_t param_type, float_t param_data) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = operation_method;
            tx_data[1] = param_addr >> 8;
            tx_data[2] = param_addr & 0xFF;
            tx_data[3] = param_type;

            FloatUintConverter_u converter;
            converter.f = param_data;
            uint32_t param_data_uint = converter.u;
            tx_data[4] = param_data_uint >> 24;
            tx_data[5] = param_data_uint >> 16;
            tx_data[6] = param_data_uint >> 8;
            tx_data[7] = param_data_uint & 0xFF;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Gripper Control
        tx_package_t(uint8_t gripper_id, uint8_t gripper_position, uint8_t gripper_velocity, uint8_t gripper_effort) {
            this->canid = std::make_unique<CanId>(gripper_id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x03;
            tx_data[1] = gripper_position;
            tx_data[2] = gripper_velocity;
            tx_data[3] = gripper_effort;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }

        ~tx_package_t() = default;
};

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

            subscriber_motor_zero_position_set_ = this->create_subscription<std_msgs::msg::UInt8>("motor_zero_position_set", 10, std::bind(&SocketCanSenderNode::motor_zero_position_set_callback, this, _1));
            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&SocketCanSenderNode::timer_callback, this));
        }

        void motor_data1_message_request() {
            for (uint8_t i = 0; i < 6; i++) {
                tx_package_t data1_msg_request(i+1, static_cast<uint32_t>(FDB_REQ_TX), 
                                                    static_cast<uint8_t>(DATA1), 
                                                    static_cast<uint8_t>(CYCLE));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        void motor_data2_message_request() {
            for (uint8_t i = 0; i < 6; i++) {
                tx_package_t data2_msg_request(i+1, static_cast<uint32_t>(FDB_REQ_TX), 
                                                    static_cast<uint8_t>(DATA2), 
                                                    static_cast<uint8_t>(CYCLE));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        void motor_data3_message_request() {
            for (uint8_t i = 0; i < 6; i++) {
                tx_package_t data3_msg_request(i+1, static_cast<uint32_t>(FDB_REQ_TX), 
                                                    static_cast<uint8_t>(DATA3), 
                                                    static_cast<uint8_t>(CYCLE));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        void motor_data4_message_request() {
            for (uint8_t i = 0; i < 6; i++) {
                tx_package_t data4_msg_request(i+1, static_cast<uint32_t>(FDB_REQ_TX), 
                                                    static_cast<uint8_t>(DATA4), 
                                                    static_cast<uint8_t>(CYCLE));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

    private:
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;
        rclcpp::Subscription<robot_interfaces::msg::QtPub>::SharedPtr subscriber_motor_states_;
        rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr subscriber_gripper_states_;
        rclcpp::Subscription<robot_interfaces::msg::RobotControlMsg>::SharedPtr subscriber_motor_msgs_;
        rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr subscriber_motor_zero_position_set_;
        rclcpp::TimerBase::SharedPtr timer_;
        
        // Motor Zero Position Set
        void motor_zero_position_set_callback(const std_msgs::msg::UInt8::SharedPtr msg);
        // rviz control
        void joint_pos_callback(const sensor_msgs::msg::JointState::SharedPtr msg);
        // QT control
        void motor_states_request_callback(const robot_interfaces::msg::QtPub::SharedPtr msg);
        // Keyboard Control
        void motor_msg_cmd_callback(const robot_interfaces::msg::RobotControlMsg::SharedPtr msg);
        // Gripper Control
        void gripper_states_request_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg);
        // Motor Feedback Message Request
        void timer_callback();
};

#endif
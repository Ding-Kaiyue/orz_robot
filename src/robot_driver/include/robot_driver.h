/*
**
**
**
*/

#ifndef __ROS_CAN_DRIVER__
#define __ROS_CAN_DRIVER__

#include <rclcpp/rclcpp.hpp>
#include "can_msgs/msg/frame.hpp"
#include "ros2_socketcan/socket_can_id.hpp"
#include "ros2_socketcan/socket_can_sender.hpp"
#include "ros2_socketcan/socket_can_receiver.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "robot_interfaces/msg/qt_pub.hpp"
#include "robot_interfaces/msg/robot_control_msg.hpp"
#include "robot_interfaces/msg/motor_fdb.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"
#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>

#define POSITION_MODE 0x05
#define SPEED_MODE 0x04
#define EFFORT_MODE 0x02

#define MOTOR_ENABLE 0x01
#define MOTOR_DISABLE 0x00
#define MOTOR_NUM 0x06
#define CAN_FDB_ID 0x200

using namespace drivers::socketcan;
using std::placeholders::_1;

typedef struct
{
    uint8_t enable_flag = 0;
    uint8_t control_mode = 0;
    uint8_t limit_flag = 0;
    uint16_t temperature = 0.0f;
    float position = 0.0f;
    float velocity = 0.0f;
    float current = 0.0f;
    float vbus = 0.0f;
    uint32_t error_code = 0;
} Motor_Status_t;

class tx_package_t {
    private:
        uint8_t tx_data[8] = {0};

        std::shared_ptr<SocketCanSender> sender = std::make_shared<SocketCanSender>("can0", false);
        std::unique_ptr<CanId> canid;

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
            MOTOR_CTRL_TX = 0x00000000,
            FDB_REQ_TX = 0x00000200,
            FUNC_CTRL_TX = 0x00000400,
            PARM_RW_TX = 0x00000600,
        } CommandID_Tx_e;
        typedef enum {
            PARAM_R_TX = 0x01,
            PARAM_W_TX = 0x02,
        } Operation_Method_e;
    public:
        // Parameter Read
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t operation_method, uint8_t params_addr) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = operation_method;
            tx_data[1] = params_addr;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Motor Control Commands
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t control_enable_flag, uint8_t control_mode, float control_value) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            FloatUintConverter_u converter;
            converter.f = control_value;
            uint32_t control_value_uint = converter.u;
            tx_data[0] = control_enable_flag;
            tx_data[1] = control_mode;
            tx_data[2] = 0;
            tx_data[3] = 0;
            tx_data[4] = control_value_uint >> 24;
            tx_data[5] = control_value_uint >> 16;
            tx_data[6] = control_value_uint >> 8;
            tx_data[7] = control_value_uint & 0xFF;
            
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
        // Motor Feedback Request and Function Operation interfaces
        tx_package_t(uint8_t motor_id, uint32_t interfaces_type, uint8_t data) {
            this->canid = std::make_unique<CanId>(motor_id + interfaces_type, 0, FrameType::DATA, ExtendedFrame);
            tx_data[0] = data;
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
        tx_package_t(uint8_t gripper_id = 0x3F, uint8_t gripper_position = 0, uint8_t gripper_velocity = 100, uint8_t gripper_effort = 0) {
            this->canid = std::make_unique<CanId>(gripper_id, 0, FrameType::DATA, StandardFrame);
            tx_data[0] = 0x03;
            tx_data[1] = gripper_position;
            tx_data[2] = gripper_velocity;
            tx_data[3] = gripper_effort;
            sender->send(tx_data, sizeof(tx_data), *this->canid, std::chrono::seconds(1));
        }
};

class rx_package_t {
    private:
        uint8_t Motor_ID = 0;
        uint8_t rx_data[8] = {0};
        std::shared_ptr<Motor_Status_t> motor_status;

        enum DataFdbClass_e {
            DATA1 = 0x01,
            DATA2,
            DATA3,
            DATA4
        };
        union FloatUintConverter_u {
            float f;
            uint32_t u;
        };
        // 反馈接口类型
        typedef enum  {
            MOTOR_CTRL_RX = 0x00000100,
            FDB_REQ_RX = 0x00000300,
            FUN_CTRL_RX = 0x00000500,
            PAM_RW_RX = 0x00000700,
        } CommandID_Rx_e;

        typedef enum {
            MOTOR1_RET_ID = 0x301,
            MOTOR2_RET_ID ,
            MOTOR3_RET_ID ,
            MOTOR4_RET_ID ,
            MOTOR5_RET_ID ,
            MOTOR6_RET_ID ,
        } motor_ret_id_e;
    public:
        rx_package_t(CanId can_id, uint8_t *data, std::shared_ptr<Motor_Status_t> motor_status_) {
            this->motor_status = motor_status_;
            
            if (sizeof(data) == 8) {
                std::copy(data, data + 8, rx_data);
                Motor_ID = can_id.identifier() & 0x000000FF;

                switch (can_id.identifier() & 0xFFFFFF00) {
                    // 常用控制接口
                    case MOTOR_CTRL_RX: {
                        break;
                    }
                    // 请求反馈接口
                    case FDB_REQ_RX: {
                        if (rx_data[0] == DATA1) {              // data1
                            motor_status->enable_flag = rx_data[1];
                            motor_status->control_mode = rx_data[2];
                            uint32_t position_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = position_uint;
                            motor_status->position = converter.f;
                        } else if (rx_data[0] == DATA2) {       // data2
                            motor_status->limit_flag = rx_data[1];
                            motor_status->temperature = (rx_data[2] << 8) | (rx_data[3] & 0xFF);
                            uint32_t velocity_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = velocity_uint;
                            motor_status->velocity = converter.f;
                        } else if (rx_data[0] == DATA3) {       // data3
                            motor_status->vbus = (rx_data[2] << 8) | (rx_data[3] & 0xFF);
                            uint32_t current_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = current_uint;
                            motor_status->current = converter.f;
                        } else if (rx_data[0] == DATA4) {       // data4
                            motor_status->error_code = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                        }
                        break;
                    }
                    // 函数操作接口
                    case FUN_CTRL_RX: {
                        if (rx_data[1] == 1) {
                            std::cout << unsigned(rx_data[0]) << "%d operation done successfully!";
                        }
                        break;
                    }
                    // 参数读写接口
                    case PAM_RW_RX: {
                        if (rx_data[0] == 0x01) {       // Parameter read
                            if (rx_data[3] == 0x01) {   // The parameter is int
                                std::cout << "Read " << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %d" 
                                          << ((rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF));
                            } else if (rx_data[3] == 0x02) {   // The parameter is float
                                FloatUintConverter_u converter;
                                converter.u = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF);
                                std::cout << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %f" << converter.f;
                            }
                        } else if (rx_data[0] == 0x02) {        // Parameter write
                            if (rx_data[3] == 0x01) {   // The parameter is int
                                std::cout << "Write " << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %d" 
                                          << ((rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF));
                            } else if (rx_data[3] == 0x02) {   // The parameter is float
                                FloatUintConverter_u converter;
                                converter.u = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF);
                                std::cout << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %f" << converter.f;
                            }
                        }
                        break;
                    }
                    default: 
                        break;
                }
            }
        }
};


// 06 01 04 00 00 00 00 enable
class SocketCanSenderNode : public rclcpp ::Node
{

public:
    std::array<std::shared_ptr<Motor_Status_t>, 6> Motor_Status;

    SocketCanSenderNode(const std::string &node_name) : Node(node_name)
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

    // rviz control
    void joint_pos_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        for (size_t i = 0; i < msg->position.size(); i++) {
            package_t(i+1, MOTOR_CTRL_TX, MOTOR_ENABLE, POSITION_MODE, msg->position[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // QT control
    void motor_states_request_callback(const robot_interfaces::msg::QtPub::SharedPtr msg) {
        if (msg->working_mode != 0x06) {
            for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
                package_t(i+1, MOTOR_CTRL_TX, msg->enable_flag, POSITION_MODE, msg->joint_group_positions[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } else {
            for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
                package_t(i+1, MOTOR_CTRL_TX, msg->enable_flag, SPEED_MODE, 0.0f);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    // Gripper Control
    void gripper_states_request_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
        package_t(0x3F, msg->data[0], msg->data[1], msg->data[2]);
        RCLCPP_INFO(this->get_logger(), "Gripper msg sent------------------");
    }

    // Keyboard Control
    void motor_msg_cmd_callback(const robot_interfaces::msg::RobotControlMsg::SharedPtr msg) {
        for (size_t i = 0; i < msg->motor_mode.size(); i++) {
            package_t(i+1, MOTOR_CTRL_TX, msg->motor_enable_flag[i], msg->motor_mode[i], msg->motor_msg[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        }
    }

    void motor_zero_position_set_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        package_t
        motor_req(msg->data, MOTOR_DISABLE, POSITION_MODE, 0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        motor_zero_position_set(msg->data + 0x400);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        motor_zero_position_write_to_flash(msg->data + 0x400);
        RCLCPP_INFO(this->get_logger(), "==========================================");
    }
private:
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;
    rclcpp::Subscription<robot_interfaces::msg::QtPub>::SharedPtr subscriber_motor_states_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr subscriber_gripper_states_;
    rclcpp::Subscription<robot_interfaces::msg::RobotControlMsg>::SharedPtr subscriber_motor_msgs_;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_motor_zero_position_set_;
    rclcpp::TimerBase::SharedPtr timer_;
    union FloatUintConverter
    {
        float f;
        uint32_t u;
    };

    uint8_t motor_id = 0x01;
    uint8_t current_mode = POSITION_MODE;

    void motor_zero_position_set_callback(const std_msgs::msg::Int32::SharedPtr msg);
};

#endif
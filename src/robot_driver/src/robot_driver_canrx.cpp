#include <rclcpp/rclcpp.hpp>
#include <vector>
#include "can_msgs/msg/frame.hpp"
#include "ros2_socketcan/socket_can_id.hpp"
#include "ros2_socketcan/socket_can_receiver.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "robot_interfaces/msg/motor_fdb.hpp"

#define POSITION_MODE 0x05
#define SPEED_MODE 0x04
#define EFFORT_MODE 0x02

#define PI_ 3.141592654f

typedef enum {
    MOTOR1_RET_ID = 0x301,
    MOTOR2_RET_ID ,
    MOTOR3_RET_ID ,
    MOTOR4_RET_ID ,
    MOTOR5_RET_ID ,
    MOTOR6_RET_ID ,
}motor_ret_id_enum;

using namespace drivers::socketcan;

class SocketCanReceiverNode : public rclcpp :: Node
{
    public:
        SocketCanReceiverNode(const std::string& node_name) : Node(node_name)
        {
            SocketCanReceiver receiver("can0", false);
            // publish the joint attitude from the real motors
            publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
            // motor feedback states
            robot_interfaces::msg::MotorFdb motor_fdb_raw;
            motor_fdb_raw.motor_enable_flag.resize(6);
            motor_fdb_raw.motor_fdb_mode.resize(6);
            motor_fdb_raw.motor_angle_fdb.resize(6);
            motor_fdb_raw.motor_effort_fdb.resize(6);

            
            FloatUintConverter converter;

            // Set CAN Filters
            try {
                receiver.SetCanFilters(filters);
            } catch (const std::runtime_error & e) {
                RCLCPP_ERROR(this->get_logger(), "Failed to set filters: %s", e.what());
            }

            // CAN Message Receive
            std::array<uint8_t, 8> data;
            while (rclcpp::ok()) {
                try {
                    CanId can_id = receiver.receive(data, std::chrono::seconds(1));
                    switch (can_id.identifier()) {
                        case MOTOR1_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[0] = data[1];
                            motor_fdb_raw.motor_fdb_mode[0] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[0] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[0] = converter.f;
                                receive_flag_pos[0] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor1 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[0] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[0] = converter.f;
                                receive_flag_effort[0] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor1 effort feedback");
                            }
                            break;
                        }
                        case MOTOR2_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[1] = data[1];
                            motor_fdb_raw.motor_fdb_mode[1] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[1] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[1] = converter.f;
                                receive_flag_pos[1] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor2 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[1] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[1] = converter.f;
                                receive_flag_effort[1] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor2 effort feedback");
                            }
                            break;
                        }
                        case MOTOR3_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[2] = data[1];
                            motor_fdb_raw.motor_fdb_mode[2] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[2] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[2] = converter.f;
                                receive_flag_pos[2] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor3 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[2] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[2] = converter.f;
                                receive_flag_effort[2] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor3 effort feedback");
                            }
                            break;
                        }
                        case MOTOR4_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[3] = data[1];
                            motor_fdb_raw.motor_fdb_mode[3] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[3] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[3] = converter.f;
                                receive_flag_pos[3] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor4 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[3] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[3] = converter.f;
                                receive_flag_effort[3] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor4 effort feedback");
                            }
                            break;
                        }
                        case MOTOR5_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[4] = data[1];
                            motor_fdb_raw.motor_fdb_mode[4] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[4] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[4] = converter.f;
                                receive_flag_pos[4] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor5 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[4] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[4] = converter.f;
                                receive_flag_effort[4] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor5 effort feedback");
                            }
                            break;
                        }
                        case MOTOR6_RET_ID: {
                            motor_fdb_raw.motor_enable_flag[5] = data[1];
                            motor_fdb_raw.motor_fdb_mode[5] = data[3];
                            if (motor_fdb_raw.motor_fdb_mode[5] == POSITION_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_angle_fdb[5] = converter.f;
                                receive_flag_pos[5] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor6 pos feedback");
                            } else if (motor_fdb_raw.motor_fdb_mode[5] == EFFORT_MODE) {
                                converter.u = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | (data[7] & 0xFF);
                                motor_fdb_raw.motor_effort_fdb[5] = converter.f;
                                receive_flag_effort[5] = true;
                                // RCLCPP_INFO(this->get_logger(), "Motor6 effort feedback");
                            }
                            break;
                        }
                        default: break;
                    }
                    
                    if (receive_flag_pos[0] == true && receive_flag_pos[1] == true && receive_flag_pos[2] == true 
                        && receive_flag_pos[3] == true && receive_flag_pos[4] == true && receive_flag_pos[5] == true
                        && receive_flag_effort[0] == true && receive_flag_effort[1] == true && receive_flag_effort[2] == true 
                        && receive_flag_effort[3] == true && receive_flag_effort[4] == true && receive_flag_effort[5] == true) {
                            motor_message_feedback(motor_fdb_raw.motor_angle_fdb, motor_fdb_raw.motor_effort_fdb);
                    }
               } 
                catch (const SocketCanTimeout & e) {
                    RCLCPP_ERROR(this->get_logger(), "Timeout: %s", e.what());
                } 
                catch (const std::runtime_error & e) {
                    RCLCPP_ERROR(this->get_logger(), "Caught exception: %s", e.what());
                }
            }
        }
    private:
        std::string filter_str = "0x000~0xFFF";
        SocketCanReceiver::CanFilterList filters = SocketCanReceiver::CanFilterList::ParseFilters(filter_str);
        rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
        bool receive_flag_pos[6] = {false}, receive_flag_effort[6] = {false};

        union FloatUintConverter {
            float f;
            uint32_t u;
        };

        // 电机反馈信息处理
        void motor_message_feedback(std::vector<float> position_data, std::vector<float> effort_data)
        {
            position_data.resize(6);
            effort_data.resize(6);

            sensor_msgs::msg::JointState joint_state_fdb;
            joint_state_fdb.header.stamp = this->get_clock()->now();
            joint_state_fdb.name.resize(6);
            joint_state_fdb.position.resize(6);
            joint_state_fdb.effort.resize(6);

            joint_state_fdb.name[0] = "joint1";
            joint_state_fdb.name[1] = "joint2";
            joint_state_fdb.name[2] = "joint3";
            joint_state_fdb.name[3] = "joint4";
            joint_state_fdb.name[4] = "joint5";
            joint_state_fdb.name[5] = "joint6";

            joint_state_fdb.position[0] = (double)(position_data[0] * PI_ / 180.0);
            joint_state_fdb.position[1] = (double)(position_data[1] * PI_ / 180.0);
            joint_state_fdb.position[2] = (double)(position_data[2] * PI_ / 180.0);
            joint_state_fdb.position[3] = (double)(position_data[3] * PI_ / 180.0);
            joint_state_fdb.position[4] = (double)(position_data[4] * PI_ / 180.0);
            joint_state_fdb.position[5] = (double)(position_data[5] * PI_ / 180.0);

            joint_state_fdb.effort[0] = (double)effort_data[0];
            joint_state_fdb.effort[1] = (double)effort_data[1];
            joint_state_fdb.effort[2] = (double)effort_data[2];
            joint_state_fdb.effort[3] = (double)effort_data[3];
            joint_state_fdb.effort[4] = (double)effort_data[4];
            joint_state_fdb.effort[5] = (double)effort_data[5];
            
            // RCLCPP_INFO(this->get_logger(), "Motor angle feedback: %f, %f, %f, %f, %f, %f", 
            //                                         joint_state_fdb.position[0], joint_state_fdb.position[1],
            //                                         joint_state_fdb.position[2], joint_state_fdb.position[3], 
            //                                         joint_state_fdb.position[4], joint_state_fdb.position[5]);
            publisher_->publish(joint_state_fdb);

            for (uint8_t i = 0; i < 6; i++) {
                receive_flag_pos[i] = false;
                receive_flag_effort[i] = false;
            }
        }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SocketCanReceiverNode>("robot_driver_canrx");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

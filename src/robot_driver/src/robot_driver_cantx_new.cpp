#include "robot_driver_tx.h"

using namespace drivers::socketcan;
using std::placeholders::_1;

void SocketCanSenderNode::motor_zero_position_set_callback(const std_msgs::msg::UInt8::SharedPtr msg) {
    // motor disable
    tx_package_t motor_disable(static_cast<uint8_t>(msg->data), MOTOR_CTRL_TX, MOTOR_ENABLE, POSITION_ABS_MODE, 0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // Set the current position to motor zero position
    tx_package_t motor_zero_position_set(static_cast<uint8_t>(msg->data), FUNC_CTRL_TX, 0x04);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // Save the parameter to flash
    tx_package_t motor_save_to_flash(static_cast<uint8_t>(msg->data), FUNC_CTRL_TX, 0x02);
    RCLCPP_INFO(this->get_logger(), "==========================================");
}

// rviz control
void SocketCanSenderNode::joint_pos_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    for (size_t i = 0; i < msg->position.size(); i++) {
        tx_package_t motor_control_pos(static_cast<uint8_t>(i+1), MOTOR_CTRL_TX, MOTOR_ENABLE, POSITION_ABS_MODE, msg->position[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// QT control
void SocketCanSenderNode::motor_states_request_callback(const robot_interfaces::msg::QtPub::SharedPtr msg) {
    if (msg->working_mode != 0x06) {
        for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
            tx_package_t motor_control_pos(static_cast<uint8_t>(i+1), MOTOR_CTRL_TX, MOTOR_ENABLE, POSITION_ABS_MODE, msg->joint_group_positions[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } else {
        for (size_t i = 0; i < msg->joint_group_positions.size(); i++) {
            tx_package_t motor_control_speed(static_cast<uint8_t>(i+1), MOTOR_CTRL_TX, MOTOR_ENABLE, SPEED_MODE, 0.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

// Keyboard Control
void SocketCanSenderNode::motor_msg_cmd_callback(const robot_interfaces::msg::RobotControlMsg::SharedPtr msg) {
    for (size_t i = 0; i < msg->motor_mode.size(); i++) {
        tx_package_t motor_control(static_cast<uint8_t>(i+1), MOTOR_CTRL_TX, msg->motor_enable_flag[i], msg->motor_mode[i], msg->motor_msg[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
    }
}

// Gripper Control
void SocketCanSenderNode::gripper_states_request_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
    tx_package_t gripper_control(0x3F, msg->data[0], msg->data[1], msg->data[2]);
    RCLCPP_INFO(this->get_logger(), "Gripper msg sent------------------");
}

// Request Motor Feedback Message
void SocketCanSenderNode::timer_callback() {
    motor_data1_message_request();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // motor_data2_message_request();
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // motor_data3_message_request();
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer_->cancel();
}

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SocketCanSenderNode>("robot_driver_cantx");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
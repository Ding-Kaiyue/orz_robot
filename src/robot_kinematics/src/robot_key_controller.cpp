#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/int8.hpp>
#include <robot_interfaces/msg/robot_control_msg.hpp>
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/int8_multi_array.hpp"
#include <chrono>
#include <thread>
#include "robot_key_controller.h"


KeyboardController::KeyboardController(std::string node_name) : Node(node_name) {
    arm = std::make_shared<moveit::planning_interface::MoveGroupInterface>(rclcpp::Node::SharedPtr(this), std::string("arm"));
    mode_sub = this->create_subscription<std_msgs::msg::Int8>("keyboard_control_mode", 10, std::bind(&KeyboardController::working_mode_callback, this, std::placeholders::_1));
    motor_msg_pub = this->create_publisher<robot_interfaces::msg::RobotControlMsg>("motor_msg", 10);
    gripper_msg_pub = this->create_publisher<std_msgs::msg::Int8MultiArray>("gripper_msg", 10);
    subscriber_joint_states_ = this->create_subscription<sensor_msgs::msg::JointState>("joint_states", 10, std::bind(&KeyboardController::joint_states_callback, this, std::placeholders::_1));
    // 设置机械臂运动允许的误差
    arm->setGoalJointTolerance(0.0007);
    // 设置机械臂运动参数
    arm->setMaxAccelerationScalingFactor(0.1);
    arm->setMaxVelocityScalingFactor(0.1);
    arm->setPoseReferenceFrame("base_link");
    arm->allowReplanning(true);
    arm->setGoalPositionTolerance(0.0005);
    arm->setGoalOrientationTolerance(0.0007);
}


void KeyboardController::working_mode_callback(const std_msgs::msg::Int8::SharedPtr msg) {
    switch (msg->data) {
        case BACKTOSTART: {         // 回到初始位置
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);
            for (size_t i = 0; i < rbt_ctrl_msg.motor_mode.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
                rbt_ctrl_msg.motor_mode[i] = POSITION_MODE;
                rbt_ctrl_msg.motor_msg[i] = 0.0f;
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case PASSIVE: {             // 所有电机进入阻尼状态(上电默认状态)
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);
            for (size_t i = 0; i < rbt_ctrl_msg.motor_mode.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
                rbt_ctrl_msg.motor_mode[i] = VELOCITY_MODE;
                rbt_ctrl_msg.motor_msg[i] = 0.0f;
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINTCONTROL: {         // 关节空间速度控制，可以通过长按键盘直接地给定机械臂六个关节运动的速度
            joint_action_ = this->create_subscription<std_msgs::msg::Int8>("joint_action", 10, std::bind(&KeyboardController::joint_action_callback, this, std::placeholders::_1));
            break;
        }
        case CARTESIAN: {            // 笛卡尔空间控制，可以通过键盘给定机械臂末端的期望位置与姿态的运动速度

        }
        case MOVEJ: {

        }
        case MOVEL: {

        }
        case MOVEC: {

        }
        case TEACH: {

        }
        case TEACHREPEAT: {

        }
        case SAVESTATE: {

        }
        case TOSTATE: {

        }
        case CALIBRATION: {

        }
        default:
            break;
    }
}

void KeyboardController::joint_action_callback(const std_msgs::msg::Int8::SharedPtr msg) {
    switch (msg->data) {
        case JOINT1_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }

            rbt_ctrl_msg.motor_mode[0] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;
            
            if (current_joint_states_.position[0] > -PI_ && current_joint_states_.position[0] < PI_) {
                rbt_ctrl_msg.motor_msg = {JOINT_ROTATE_POSITIVE_SPEED, current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {0.0f, current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }
            
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT1_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }

            rbt_ctrl_msg.motor_mode[0] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[0] > -PI_ && current_joint_states_.position[0] < PI_) {
                rbt_ctrl_msg.motor_msg = {JOINT_ROTATE_NEGATIVE_SPEED, current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {0.0f, current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }
            
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT2_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }

            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[1] > -PI_/2 && current_joint_states_.position[1] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], JOINT_ROTATE_POSITIVE_SPEED, current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], 0.0f, current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT2_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[1] > -PI_/2 && current_joint_states_.position[1] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], JOINT_ROTATE_NEGATIVE_SPEED, current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], 0.0f, current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT3_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[2] > -PI_/2 && current_joint_states_.position[2] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], JOINT_ROTATE_POSITIVE_SPEED, current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], 0.0f, current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT3_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[2] > -PI_/2 && current_joint_states_.position[2] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], JOINT_ROTATE_NEGATIVE_SPEED, current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], 0.0f, current_joint_states_.position[3], 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT4_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[3] > -PI_/2 && current_joint_states_.position[3] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], JOINT_ROTATE_POSITIVE_SPEED, 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], 0.0f, 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT4_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[3] > -PI_/2 && current_joint_states_.position[3] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], JOINT_ROTATE_NEGATIVE_SPEED, 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], 0.0f, 
                                            current_joint_states_.position[4], current_joint_states_.position[5]};
            }

            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT5_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[4] > -PI_ && current_joint_states_.position[4] < PI_) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            JOINT_ROTATE_POSITIVE_SPEED, current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            0.0f, current_joint_states_.position[5]};
            }
            
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT5_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_mode[5] = POSITION_MODE;

            if (current_joint_states_.position[4] > -PI_/2 && current_joint_states_.position[4] < PI_/2) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            JOINT_ROTATE_NEGATIVE_SPEED, current_joint_states_.position[5]};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            0.0f, current_joint_states_.position[5]};
            }
            
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT6_POSITIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = VELOCITY_MODE;

            if (current_joint_states_.position[5] > -PI_ && current_joint_states_.position[5] < PI_) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], JOINT_ROTATE_POSITIVE_SPEED};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], 0.0f};
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINT6_NEGATIVE: {
            rbt_ctrl_msg.motor_enable_flag.resize(6);
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);

            for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
                rbt_ctrl_msg.motor_enable_flag[i] = true;
            }
            
            rbt_ctrl_msg.motor_mode[0] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[1] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[2] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[3] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[4] = POSITION_MODE;
            rbt_ctrl_msg.motor_mode[5] = VELOCITY_MODE;

            if (current_joint_states_.position[5] > -PI_ && current_joint_states_.position[5] < PI_) {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], JOINT_ROTATE_NEGATIVE_SPEED};
            } else {
                rbt_ctrl_msg.motor_msg = {current_joint_states_.position[0], current_joint_states_.position[1], current_joint_states_.position[2], current_joint_states_.position[3], 
                                            current_joint_states_.position[4], 0.0f};
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case GRIPPER_OPEN: {
            std_msgs::msg::Int8MultiArray gripper_states;
            if (gripper_position > 0) {
                gripper_position--;
            }            
            gripper_states.data.clear();
            gripper_states.data.push_back(gripper_position);
            gripper_states.data.push_back(50);
            gripper_states.data.push_back(0);
            gripper_msg_pub->publish(gripper_states);
            break;
        }
        case GRIPPER_CLOSE: {
            std_msgs::msg::Int8MultiArray gripper_states;
            if(gripper_position < 255) {
                gripper_position++;
            }
            gripper_states.data.clear();
            gripper_states.data.push_back(gripper_position);
            gripper_states.data.push_back(0);
            gripper_states.data.push_back(50);
            gripper_msg_pub->publish(gripper_states);
            break;
        }
        default: {
            break;
        }
    }
}

void KeyboardController::joint_states_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
{
    current_joint_states_.header.stamp = this->get_clock()->now();
    current_joint_states_.name.resize(6);
    current_joint_states_.position.resize(6);
    current_joint_states_.effort.resize(6);

    // 只使用了位置，可以添加其他信息
    for (size_t i = 0; i < msg->name.size(); i++) {
        current_joint_states_.name[i] = msg->name[i];
        current_joint_states_.position[i] = msg->position[i];
    }
}

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<KeyboardController>("key_controller");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

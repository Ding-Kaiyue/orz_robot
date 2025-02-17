#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_msgs/msg/move_it_error_codes.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/int8.hpp>
#include <robot_interfaces/msg/robot_control_msg.hpp>
#include <robot_interfaces/msg/arm_state.hpp>
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/int8_multi_array.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>
#include <thread>
#include "robot_key_controller.h"


KeyboardController::KeyboardController(std::string node_name) : Node(node_name) {
    arm = std::make_shared<moveit::planning_interface::MoveGroupInterface>(rclcpp::Node::SharedPtr(this), std::string("arm"));
    mode_sub = this->create_subscription<std_msgs::msg::Int8>("keyboard_control_mode", 10, std::bind(&KeyboardController::working_mode_callback, this, std::placeholders::_1));
    motor_msg_pub = this->create_publisher<robot_interfaces::msg::RobotControlMsg>("motor_msg", 10);
    gripper_msg_pub = this->create_publisher<std_msgs::msg::Int8MultiArray>("gripper_msg", 10);
    subscriber_joint_states_ = this->create_subscription<sensor_msgs::msg::JointState>("joint_states", 10, std::bind(&KeyboardController::joint_states_callback, this, std::placeholders::_1));
    timer_ = this->create_wall_timer(std::chrono::microseconds(50), std::bind(&KeyboardController::timer_callback, this));

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
            timer_ = this->create_wall_timer(std::chrono::microseconds(20), std::bind(&KeyboardController::timer_callback, this));
            jointctrl_action_ = this->create_subscription<std_msgs::msg::Int8>("jointctrl_action", 10, std::bind(&KeyboardController::jointctrl_action_callback, this, std::placeholders::_1));
            break;
        }
        case CARTESIAN: {            // 笛卡尔空间控制，可以通过键盘给定机械臂末端的期望位置与姿态的运动速度
            cartesian_action_ = this->create_subscription<std_msgs::msg::Int8>("cartesian_action", 10, std::bind(&KeyboardController::cartesian_action_callback, this, std::placeholders::_1));
            break;
        }
        case MOVEJ: {
            movej_action_ = this->create_subscription<std_msgs::msg::Float64MultiArray>("movej_action", 10, std::bind(&KeyboardController::movej_action_callback, this, std::placeholders::_1));
            break;
        }
        case MOVEL: {
            movel_action_ = this->create_subscription<std_msgs::msg::Float64MultiArray>("movel_action", 10, std::bind(&KeyboardController::movel_action_callback, this, std::placeholders::_1));
            break;
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

void KeyboardController::jointctrl_action_callback(const std_msgs::msg::Int8::SharedPtr msg) {
    last_msg_time = this->now();
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

void KeyboardController::movej_action_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
    std::vector<double> joint_group_positions;
    if (msg->data.size() % 6 != 0) {
        RCLCPP_ERROR(this->get_logger(), "Error input joint angle values!");
    } else {
        joint_group_positions.resize(6);
        for (size_t i = 0; i < msg->data.size(); i += 6) {
            for (size_t j = 0; j < 6; j++) {
                joint_group_positions[i] = msg->data[i+j];
            }
            arm->setJointValueTarget(joint_group_positions);
            moveit_msgs::msg::MoveItErrorCodes move_result = arm->move();
            if (move_result.val != moveit_msgs::msg::MoveItErrorCodes::SUCCESS) {
                RCLCPP_ERROR(this->get_logger(), "MoveIt error: %d", move_result.val);
            } else {
                RCLCPP_INFO(this->get_logger(), "Joint angle movements group[%zu] are completed.", i/6 + 1);
            }
        }
    }
}

void KeyboardController::movel_action_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
    std_msgs::msg::Bool plan_result;
    geometry_msgs::msg::Pose arm_goal_pose;
    if (msg->data.size() % 6 != 0) {
        RCLCPP_ERROR(this->get_logger(), "Error input pose values!");
    } else {
        for (size_t i = 0; i < msg->data.size(); i += 6) {
            arm_goal_pose.position.x = msg->data[i];
            arm_goal_pose.position.y = msg->data[i+1];
            arm_goal_pose.position.z = msg->data[i+2];
            double roll = msg->data[i+3];
            double pitch = msg->data[i+4];
            double yaw = msg->data[i+5];

            tf2::Quaternion quat_tf;
            quat_tf.setRPY(roll, pitch, yaw);

            geometry_msgs::msg::Quaternion quat_msg;
            quat_msg = tf2::toMsg(quat_tf);
            arm_goal_pose.orientation = quat_msg;

            arm->setStartStateToCurrentState();
            arm->setPoseTarget(arm_goal_pose, arm->getEndEffectorLink());

            moveit::planning_interface::MoveGroupInterface::Plan plan;
            moveit::core::MoveItErrorCode success = arm->plan(plan);

            RCLCPP_INFO(this->get_logger(), "Plan (pose goal) %s", success ? "SUCCEED" : "FAILED");
            
            if (success) { 
                moveit::core::MoveItErrorCode execute_success = arm->execute(plan); 
                RCLCPP_INFO(this->get_logger(), "Execute (pose goal) %s", execute_success? "SUCCEED" : "FAILED");
            }
        }
    }
}

void KeyboardController::timer_callback()
{
    auto elapsed_time = this->now() - last_msg_time;
    // 将时间差转换为ms
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time.to_chrono<std::chrono::nanoseconds>());
    if (elapsed_ms.count() > 100) {
        rbt_ctrl_msg.motor_enable_flag.resize(6);
        rbt_ctrl_msg.motor_mode.resize(6);
        rbt_ctrl_msg.motor_msg.resize(6);

        for (size_t i = 0; i < rbt_ctrl_msg.motor_enable_flag.size(); i++) {
            rbt_ctrl_msg.motor_enable_flag[i] = true;
            rbt_ctrl_msg.motor_mode[i] = VELOCITY_MODE;
            rbt_ctrl_msg.motor_msg[i] = 0.0f;
        }

        motor_msg_pub->publish(rbt_ctrl_msg);
    }
}

void KeyboardController::cartesian_action_callback(const std_msgs::msg::Int8::SharedPtr msg) {
    geometry_msgs::msg::Pose arm_current_pose = arm->getCurrentPose().pose;
    geometry_msgs::msg::Pose arm_goal_pose;

    switch (msg->data) {
        case FORWARD: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.x += 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in FORWARD direction");
            }
            break;
        }
        case BACKWARD: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.x -= 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in BACKWARD direction");
            }
            break;
        }
        case LEFT: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.y -= 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in LEFT direction");
            }
            break;
        }
        case RIGHT: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.y += 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in RIGHT direction");
            }
            break;
        }
        case UP: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.z += 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in UP direction");
            }
            break;
        }
        case DOWN: {
            arm_goal_pose = arm_current_pose;
            arm_goal_pose.position.z -= 0.01;

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in DOWN direction");
            }
            break;
        }
        case ROLL_POSITIVE: {
            arm_goal_pose = arm_current_pose;
            double roll_angle = 0.017453f; // 1度

            // 创建一个表示绕x轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(roll_angle, 0, 0);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in ROLL_POSITIVE direction");
            }
            break;
        }
        case ROLL_NEGATIVE: {
            arm_goal_pose = arm_current_pose;
            double roll_angle = -0.017453f; // -1度

            // 创建一个表示绕x轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(roll_angle, 0, 0);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in ROLL_NEGATIVE direction");
            }
            break;
        }
        case PITCH_POSITIVE: {
            arm_goal_pose = arm_current_pose;
            double pitch_angle = 0.017453f; // 1度

            // 创建一个表示绕y轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(0, pitch_angle, 0);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in PITCH_POSITIVE direction");
            }
            break;
        }
        case PITCH_NEGATIVE: {
            arm_goal_pose = arm_current_pose;
            double pitch_angle = -0.017453f; // 1度

            // 创建一个表示绕y轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(0, pitch_angle, 0);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in PITCH_NEGATIVE direction");
            }
            break;
        }
        case YAW_POSITIVE: {
            arm_goal_pose = arm_current_pose;
            double yaw_angle = 0.017453f; // 1度

            // 创建一个表示绕y轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(0, 0, yaw_angle);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in YAW_POSITIVE direction");
            }
            break;
        }
        case YAW_NEGATIVE: {
            arm_goal_pose = arm_current_pose;
            double yaw_angle = -0.017453f; // 1度

            // 创建一个表示绕y轴旋转的四元数
            tf2::Quaternion q_roll;
            q_roll.setRPY(0, 0, yaw_angle);

            // 将当前姿态的四元数从geometry_msgs::msg::Quaternion 转换为tf2::Quaternion
            tf2::Quaternion q_current;
            tf2::fromMsg(arm_current_pose.orientation, q_current);

            // 组合两个四元数以得到新的姿态
            tf2::Quaternion q_new = q_roll * q_current;
            q_new.normalize(); // 归一化四元数

            // 将新的四元数转换回 geometry_msgs::msg::Quaternion 并赋值给目标姿态
            arm_goal_pose.orientation = tf2::toMsg(q_new);

            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(arm_goal_pose);

            moveit_msgs::msg::RobotTrajectory trajectory;
            const double jump_threshold = 0.0;
            const double eef_step = 0.01;
            double fraction = arm->computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

            if (fraction > 0.9) {
                moveit::planning_interface::MoveGroupInterface::Plan plan;
                plan.trajectory_ = trajectory;
                arm->execute(plan);
            } else {
                RCLCPP_ERROR(this->get_logger(), "MoveIt failed to plan the Cartesian motion in YAW_NEGATIVE direction");
            }
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
            if (gripper_position < 255) {
                gripper_position++;
            }            
            gripper_states.data.clear();
            gripper_states.data.push_back(gripper_position);
            gripper_states.data.push_back(50);
            gripper_states.data.push_back(0);
            gripper_msg_pub->publish(gripper_states);
            break;
        }
        default:
            break;
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

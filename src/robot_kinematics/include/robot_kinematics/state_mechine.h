#ifndef __STATE_MECHINE_H__
#define __STATE_MECHINE_H__

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/int8.hpp>
#include <robot_interfaces/msg/robot_control_msg.hpp>
#include <chrono>
#include <thread>

#define BACKTOSTART 0
#define PASSIVE 1
#define JOINTCONTROL 2
#define CARTESIAN 3
#define MOVEJ 4
#define MOVEL 5
#define MOVEC 6
#define TEACH 7
#define TEACHREPEAT 8
#define SAVESTATE 9
#define TOSTATE 10
#define CALIBRATION 11

#define VELOCITY_MODE 0x04
#define POSITION_MODE 0x05
#define EFFORT_MODE 0x02

class KeyboardController : public rclcpp :: Node {
    public:
        explicit KeyboardController(std::string node_name);
        ~KeyboardController(){}
    private:
        std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm;
        rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr mode_sub;
        rclcpp::Publisher<robot_interfaces::msg::RobotControlMsg>::SharedPtr motor_msg_pub;
        geometry_msgs::msg::Pose current_pose_;
        // std::vector<double> joint_group_positions;
        robot_interfaces::msg::RobotControlMsg rbt_ctrl_msg;

        void working_mode_callback(const std_msgs::msg::Int8::SharedPtr msg);    
};

#endif

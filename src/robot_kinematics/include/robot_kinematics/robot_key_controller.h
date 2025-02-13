#ifndef __ROBOT_KEY_CONTROLLER_H__
#define __ROBOT_KEY_CONTROLLER_H__

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

#define JOINT1_POSITIVE 41
#define JOINT1_NEGATIVE 42
#define JOINT2_POSITIVE 43
#define JOINT2_NEGATIVE 44
#define JOINT3_POSITIVE 45
#define JOINT3_NEGATIVE 46
#define JOINT4_POSITIVE 47
#define JOINT4_NEGATIVE 48
#define JOINT5_POSITIVE 49
#define JOINT5_NEGATIVE 50
#define JOINT6_POSITIVE 51
#define JOINT6_NEGATIVE 52
#define GRIPPER_OPEN 53
#define GRIPPER_CLOSE 54

#define PI_ 3.141592654f
#define JOINT_ROTATE_POSITIVE_SPEED 0.04*3.141592654f
#define JOINT_ROTATE_NEGATIVE_SPEED -0.04*3.141592654f


class KeyboardController : public rclcpp :: Node {
    public:
        explicit KeyboardController(std::string node_name);
        ~KeyboardController(){}
    private:
        std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm;
        rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr mode_sub;
        rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr joint_action_;
        rclcpp::Publisher<robot_interfaces::msg::RobotControlMsg>::SharedPtr motor_msg_pub;
        rclcpp::Publisher<std_msgs::msg::Int8MultiArray>::SharedPtr gripper_msg_pub;
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_joint_states_;
        geometry_msgs::msg::Pose current_pose_;
        sensor_msgs::msg::JointState current_joint_states_;
        
        // std::vector<double> joint_group_positions;
        robot_interfaces::msg::RobotControlMsg rbt_ctrl_msg;
        uint8_t gripper_position;
        void working_mode_callback(const std_msgs::msg::Int8::SharedPtr msg);    
        void joint_action_callback(const std_msgs::msg::Int8::SharedPtr msg);
        void joint_states_callback(const sensor_msgs::msg::JointState::SharedPtr msg);
};

#endif

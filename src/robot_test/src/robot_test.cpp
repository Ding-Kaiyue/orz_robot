#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/int32.hpp>
#include <rclcpp/rclcpp.hpp>
#include "robot_interfaces/msg/qt_recv.hpp"
#include "robot_interfaces/msg/arm_state.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include "geometry_msgs/msg/quaternion.hpp"


class RobotTest : public rclcpp :: Node
{
    public:
        RobotTest(const std::string& node_name) : Node(node_name) {
            publisher_ = this->create_publisher<robot_interfaces::msg::QtRecv>("qt_cmd", 10);
            publisher_nod_flag_ = this->create_publisher<std_msgs::msg::Int32>("nod_begin_flag", 10);
            subscriber_ = this->create_subscription<std_msgs::msg::Int32>("begin_flag", 10, std::bind(&RobotTest::begin_to_show_callback, this, std::placeholders::_1));
            // subscriber_dance_ = this->create_subscription<std_msgs::msg::Int32>("begin_dance", 10, std::bind(&RobotTest::begin_to_dance_callback, this, std::placeholders::_1));
            subscriber_yes_ = this->create_subscription<std_msgs::msg::Int32>("answer_reply", 10, std::bind(&RobotTest::yes_no_callback, this, std::placeholders::_1));
            subscriber_states_ = this->create_subscription<robot_interfaces::msg::ArmState>("arm_states", 10, std::bind(&RobotTest::arm_states_callback, this, std::placeholders::_1));
        }
    private:
        rclcpp::Publisher<robot_interfaces::msg::QtRecv>::SharedPtr publisher_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_nod_flag_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_dance_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_yes_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_no_;
        rclcpp::Subscription<robot_interfaces::msg::ArmState>::SharedPtr subscriber_states_;
        robot_interfaces::msg::QtRecv qt_cmd;
        geometry_msgs::msg::Pose end_effector_pose;     // 末端执行器的当前姿态
        tf2::Quaternion end_effector_quat;    // 末端执行器的当前四元数
        std_msgs::msg::Int32 nod_msg;

        void begin_to_show_callback(const std_msgs::msg::Int32::SharedPtr msg) {
            if (msg->data == 1) {
                qt_cmd.joint_angles_goal.data = {0, 37, 66, 0, 39, 0};
                // qt_cmd.gripper_goal.data = {0, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
            }
            if (msg->data == 2) {
                qt_cmd.joint_angles_goal.data = {57, 15, 66, 0, 39, 0};
                // qt_cmd.gripper_goal.data = {120, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
            }
            if (msg->data == 3) {
                qt_cmd.joint_angles_goal.data = {57, 36, 66, 0, 39, 0};
                // qt_cmd.gripper_goal.data = {107, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
            }
        }

        // void begin_to_dance_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        //     if (msg->data == 1) {
        //         nod_msg.data = 1;
        //         publisher_nod_flag_->publish(nod_msg);

        //         qt_cmd.joint_angles_goal.data = {0, -58, 41, 0, 16, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        //         qt_cmd.joint_angles_goal.data = {0, 58, -41, 0, -16, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        //         qt_cmd.joint_angles_goal.data = {0, -58, 41, 0, 16, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        //         qt_cmd.joint_angles_goal.data = {0, 58, -41, 0, -16, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        //         qt_cmd.joint_angles_goal.data = {0, 45, -72, 0, 27, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        //         qt_cmd.joint_angles_goal.data = {0, -45, 72, 0, -27, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        //         qt_cmd.joint_angles_goal.data = {0, 0, 0, 0, 0, 0};
        //         // qt_cmd.gripper_goal.data = {0, 100, 20};
        //         qt_cmd.working_mode = 0x09;
        //         qt_cmd.arm_pose_goal = end_effector_pose;
        //         publisher_->publish(qt_cmd);

        //         nod_msg.data = 0;
        //     }
        // }

        void yes_no_callback(const std_msgs::msg::Int32::SharedPtr msg)
        {
            RCLCPP_INFO(this->get_logger(), "Yes Received Msg: %d", msg->data);
            if (msg->data == 1) {   // yes
                nod_msg.data = 1;
                publisher_nod_flag_->publish(nod_msg);

                qt_cmd.joint_angles_goal.data = {0, 27, 90, 0, -27, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(5000));

                qt_cmd.joint_angles_goal.data = {0, 23, 81, 0, -14, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {0, 27, 90, 0, -27, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {0, 23, 81, 0, -14, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                nod_msg.data = 0;
            } else if (msg->data == 2) {  // no
                nod_msg.data = 1;
                publisher_nod_flag_->publish(nod_msg);

                qt_cmd.joint_angles_goal.data = {27, 32, 72, 64, -30, -61};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(5000));

                qt_cmd.joint_angles_goal.data = {-27, 32, 72, -64, -30, 61};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {27, 32, 72, 64, -30, -61};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {-27, 32, 72, -64, -30, 61};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                nod_msg.data = 0;
            } else if (msg->data == 3) {    // dance
                nod_msg.data = 1;
                publisher_nod_flag_->publish(nod_msg);

                qt_cmd.joint_angles_goal.data = {-90, -58, 80, 0, 53, 51};
                // qt_cmd.gripper_goal.data = {0, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {-45, 28, 60, -32, -37, -35};
                // qt_cmd.gripper_goal.data = {0, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {45, 28, 60, 32, -37, 35};
                // qt_cmd.gripper_goal.data = {0, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                qt_cmd.joint_angles_goal.data = {90, -58, 80, 0, 53, 51};
                // qt_cmd.gripper_goal.data = {0, 100, 20};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                qt_cmd.joint_angles_goal.data = {0, 27, 90, 0, -27, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
                nod_msg.data = 0;
            } else if (msg->data == 4) {
                nod_msg.data = 1;
                publisher_nod_flag_->publish(nod_msg);

                qt_cmd.joint_angles_goal.data = {0, 31, 90, 0, -85, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
                std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                qt_cmd.joint_angles_goal.data = {30, 31, 90, 0, -40, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
                std::this_thread::sleep_for(std::chrono::milliseconds(6500));
                qt_cmd.joint_angles_goal.data = {60, 31, 90, 0, -85, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
                std::this_thread::sleep_for(std::chrono::milliseconds(6500));
                qt_cmd.joint_angles_goal.data = {90, 31, 90, 0, -30, 0};
                qt_cmd.working_mode = 0x09;
                qt_cmd.arm_pose_goal = end_effector_pose;
                publisher_->publish(qt_cmd);
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
    auto node = std::make_shared<RobotTest>("robot_test");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

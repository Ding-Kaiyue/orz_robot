#include <memory>
#include <tf2_ros/transform_listener.h>
#include <tf2/buffer_core.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include <robot_interfaces/msg/qt_recv.hpp>
#include <robot_interfaces/msg/arm_state.hpp>

/* 
 * @brief 这是一个超级无敌宇宙大爆炸没用的注释
 * @details 这是一个超级无敌宇宙大爆炸没用的类
*/
class RobotStateGet : public rclcpp :: Node
{
    public :
        RobotStateGet(const std::string& node_name) : Node(node_name), tf_buffer_(), tf_listener_(tf_buffer_) {
        // RobotStateGet(const std::string& node_name) : Node(node_name) {
            timer_ = this->create_wall_timer(std::chrono::milliseconds(500), std::bind(&RobotStateGet::timer_callback, this));
            publisher_states_ = this->create_publisher<robot_interfaces::msg::ArmState>("arm_states", 10);
        }
    private :
        tf2::BufferCore tf_buffer_;
        tf2_ros::TransformListener tf_listener_;
        rclcpp::TimerBase::SharedPtr timer_;

        rclcpp::Publisher<robot_interfaces::msg::ArmState>::SharedPtr publisher_states_;

        void timer_callback()
        {
            geometry_msgs::msg::TransformStamped transform;
            try {
                transform = tf_buffer_.lookupTransform("world", "Link6", tf2::TimePointZero);

                robot_interfaces::msg::ArmState arm_states;
                // arm_states.end_effector_quat = transform.transform.rotation;

                arm_states.end_effector_pose.position.x = transform.transform.translation.x;
                arm_states.end_effector_pose.position.y = transform.transform.translation.y;
                arm_states.end_effector_pose.position.z = transform.transform.translation.z;
                arm_states.end_effector_pose.orientation = transform.transform.rotation;

                RCLCPP_INFO(this->get_logger(), "Get the arm position: %lf, %lf, %lf. Get the arm orientation: %lf, %lf, %lf, %lf", 
                                                    arm_states.end_effector_pose.position.x, 
                                                    arm_states.end_effector_pose.position.y,
                                                    arm_states.end_effector_pose.position.z,
                                                    arm_states.end_effector_pose.orientation.w,
                                                    arm_states.end_effector_pose.orientation.x,
                                                    arm_states.end_effector_pose.orientation.y,
                                                    arm_states.end_effector_pose.orientation.z);
                publisher_states_->publish(arm_states);
            } catch (tf2::TransformException &ex) {
                RCLCPP_WARN(this->get_logger(), "Could not transform: %s", ex.what());
            }
        }
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotStateGet>("robot_state_get");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

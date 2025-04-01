#include "robot_driver_rx.h"

using namespace drivers::socketcan;
using std::placeholders::_1;

void SocketCanReceiverNode::timer_callback() {
    auto message = sensor_msgs::msg::JointState();
    message.header.stamp = this->get_clock()->now();

    for (const auto &[motor_id, status] : motor_status_map) {
        message.name.push_back("joint" + std::to_string(motor_id));
        message.position.push_back(status->position * PI_ / 180.0);
        message.velocity.push_back(status->velocity);
        message.effort.push_back(status->current);
    }

    publisher_->publish(message);
}

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SocketCanReceiverNode>("robot_driver_canrx");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
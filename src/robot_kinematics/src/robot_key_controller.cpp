#include "state_mechine.h"


KeyboardController::KeyboardController(std::string node_name) : Node(node_name) {
    arm = std::make_shared<moveit::planning_interface::MoveGroupInterface>(rclcpp::Node::SharedPtr(this), std::string("arm"));
    mode_sub = this->create_subscription<std_msgs::msg::Int8>("keyboard_control_mode", 10, std::bind(&KeyboardController::working_mode_callback, this, std::placeholders::_1));
    motor_msg_pub = this->create_publisher<robot_interfaces::msg::RobotControlMsg>("motor_msg", 10);
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
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);
            for (size_t i = 0; i < rbt_ctrl_msg.motor_mode.size(); i++) {
                rbt_ctrl_msg.motor_mode[i] = POSITION_MODE;
                rbt_ctrl_msg.motor_msg[i] = 0.0f;
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case PASSIVE: {             // 所有电机进入阻尼状态(上电默认状态)
            rbt_ctrl_msg.motor_mode.resize(6);
            rbt_ctrl_msg.motor_msg.resize(6);
            for (size_t i = 0; i < rbt_ctrl_msg.motor_mode.size(); i++) {
                rbt_ctrl_msg.motor_mode[i] = VELOCITY_MODE;
                rbt_ctrl_msg.motor_msg[i] = 0.0f;
            }
            motor_msg_pub->publish(rbt_ctrl_msg);
            break;
        }
        case JOINTCONTROL: {         // 关节空间速度控制，可以通过长按键盘直接地给定机械臂六个关节运动的速度
            
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
        
int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<KeyboardController>("joy_controller");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

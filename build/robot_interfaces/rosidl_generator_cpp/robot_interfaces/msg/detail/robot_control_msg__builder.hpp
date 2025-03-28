// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__BUILDER_HPP_
#define ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/msg/detail/robot_control_msg__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace msg
{

namespace builder
{

class Init_RobotControlMsg_motor_msg
{
public:
  explicit Init_RobotControlMsg_motor_msg(::robot_interfaces::msg::RobotControlMsg & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::msg::RobotControlMsg motor_msg(::robot_interfaces::msg::RobotControlMsg::_motor_msg_type arg)
  {
    msg_.motor_msg = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::msg::RobotControlMsg msg_;
};

class Init_RobotControlMsg_motor_mode
{
public:
  explicit Init_RobotControlMsg_motor_mode(::robot_interfaces::msg::RobotControlMsg & msg)
  : msg_(msg)
  {}
  Init_RobotControlMsg_motor_msg motor_mode(::robot_interfaces::msg::RobotControlMsg::_motor_mode_type arg)
  {
    msg_.motor_mode = std::move(arg);
    return Init_RobotControlMsg_motor_msg(msg_);
  }

private:
  ::robot_interfaces::msg::RobotControlMsg msg_;
};

class Init_RobotControlMsg_motor_enable_flag
{
public:
  Init_RobotControlMsg_motor_enable_flag()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotControlMsg_motor_mode motor_enable_flag(::robot_interfaces::msg::RobotControlMsg::_motor_enable_flag_type arg)
  {
    msg_.motor_enable_flag = std::move(arg);
    return Init_RobotControlMsg_motor_mode(msg_);
  }

private:
  ::robot_interfaces::msg::RobotControlMsg msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::msg::RobotControlMsg>()
{
  return robot_interfaces::msg::builder::Init_RobotControlMsg_motor_enable_flag();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__BUILDER_HPP_

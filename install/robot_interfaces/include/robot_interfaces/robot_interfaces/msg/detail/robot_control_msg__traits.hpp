// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__TRAITS_HPP_
#define ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "robot_interfaces/msg/detail/robot_control_msg__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace robot_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotControlMsg & msg,
  std::ostream & out)
{
  out << "{";
  // member: motor_enable_flag
  {
    if (msg.motor_enable_flag.size() == 0) {
      out << "motor_enable_flag: []";
    } else {
      out << "motor_enable_flag: [";
      size_t pending_items = msg.motor_enable_flag.size();
      for (auto item : msg.motor_enable_flag) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: motor_mode
  {
    if (msg.motor_mode.size() == 0) {
      out << "motor_mode: []";
    } else {
      out << "motor_mode: [";
      size_t pending_items = msg.motor_mode.size();
      for (auto item : msg.motor_mode) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: motor_msg
  {
    if (msg.motor_msg.size() == 0) {
      out << "motor_msg: []";
    } else {
      out << "motor_msg: [";
      size_t pending_items = msg.motor_msg.size();
      for (auto item : msg.motor_msg) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotControlMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: motor_enable_flag
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motor_enable_flag.size() == 0) {
      out << "motor_enable_flag: []\n";
    } else {
      out << "motor_enable_flag:\n";
      for (auto item : msg.motor_enable_flag) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: motor_mode
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motor_mode.size() == 0) {
      out << "motor_mode: []\n";
    } else {
      out << "motor_mode:\n";
      for (auto item : msg.motor_mode) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: motor_msg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motor_msg.size() == 0) {
      out << "motor_msg: []\n";
    } else {
      out << "motor_msg:\n";
      for (auto item : msg.motor_msg) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotControlMsg & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace robot_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use robot_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const robot_interfaces::msg::RobotControlMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  robot_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use robot_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const robot_interfaces::msg::RobotControlMsg & msg)
{
  return robot_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<robot_interfaces::msg::RobotControlMsg>()
{
  return "robot_interfaces::msg::RobotControlMsg";
}

template<>
inline const char * name<robot_interfaces::msg::RobotControlMsg>()
{
  return "robot_interfaces/msg/RobotControlMsg";
}

template<>
struct has_fixed_size<robot_interfaces::msg::RobotControlMsg>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<robot_interfaces::msg::RobotControlMsg>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<robot_interfaces::msg::RobotControlMsg>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__TRAITS_HPP_

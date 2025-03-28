// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_HPP_
#define ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__robot_interfaces__msg__RobotControlMsg __attribute__((deprecated))
#else
# define DEPRECATED__robot_interfaces__msg__RobotControlMsg __declspec(deprecated)
#endif

namespace robot_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotControlMsg_
{
  using Type = RobotControlMsg_<ContainerAllocator>;

  explicit RobotControlMsg_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit RobotControlMsg_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _motor_enable_flag_type =
    std::vector<bool, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<bool>>;
  _motor_enable_flag_type motor_enable_flag;
  using _motor_mode_type =
    std::vector<uint8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<uint8_t>>;
  _motor_mode_type motor_mode;
  using _motor_msg_type =
    std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>>;
  _motor_msg_type motor_msg;

  // setters for named parameter idiom
  Type & set__motor_enable_flag(
    const std::vector<bool, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<bool>> & _arg)
  {
    this->motor_enable_flag = _arg;
    return *this;
  }
  Type & set__motor_mode(
    const std::vector<uint8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<uint8_t>> & _arg)
  {
    this->motor_mode = _arg;
    return *this;
  }
  Type & set__motor_msg(
    const std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>> & _arg)
  {
    this->motor_msg = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> *;
  using ConstRawPtr =
    const robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__robot_interfaces__msg__RobotControlMsg
    std::shared_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__robot_interfaces__msg__RobotControlMsg
    std::shared_ptr<robot_interfaces::msg::RobotControlMsg_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotControlMsg_ & other) const
  {
    if (this->motor_enable_flag != other.motor_enable_flag) {
      return false;
    }
    if (this->motor_mode != other.motor_mode) {
      return false;
    }
    if (this->motor_msg != other.motor_msg) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotControlMsg_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotControlMsg_

// alias to use template instance with default allocator
using RobotControlMsg =
  robot_interfaces::msg::RobotControlMsg_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_HPP_

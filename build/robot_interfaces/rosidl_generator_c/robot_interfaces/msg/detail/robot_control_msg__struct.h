// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_H_
#define ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'motor_enable_flag'
// Member 'motor_mode'
// Member 'motor_msg'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/RobotControlMsg in the package robot_interfaces.
typedef struct robot_interfaces__msg__RobotControlMsg
{
  rosidl_runtime_c__boolean__Sequence motor_enable_flag;
  /// 04-velocity   05-position   02-effort
  rosidl_runtime_c__uint8__Sequence motor_mode;
  /// 04-velocity value   05-position value   02-effort value
  rosidl_runtime_c__double__Sequence motor_msg;
} robot_interfaces__msg__RobotControlMsg;

// Struct for a sequence of robot_interfaces__msg__RobotControlMsg.
typedef struct robot_interfaces__msg__RobotControlMsg__Sequence
{
  robot_interfaces__msg__RobotControlMsg * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__msg__RobotControlMsg__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_INTERFACES__MSG__DETAIL__ROBOT_CONTROL_MSG__STRUCT_H_

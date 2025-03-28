// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_interfaces:msg/QtRecv.idl
// generated code does not contain a copyright notice

#ifndef ROBOT_INTERFACES__MSG__DETAIL__QT_RECV__STRUCT_H_
#define ROBOT_INTERFACES__MSG__DETAIL__QT_RECV__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'joint_angles_goal'
#include "std_msgs/msg/detail/int32_multi_array__struct.h"
// Member 'arm_pose_goal'
#include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in msg/QtRecv in the package robot_interfaces.
typedef struct robot_interfaces__msg__QtRecv
{
  /// 机械臂工作模式
  uint8_t working_mode;
  /// std_msgs/Int32MultiArray gripper_goal               # 夹爪目标状态
  /// 关节目标位置
  std_msgs__msg__Int32MultiArray joint_angles_goal;
  /// 机械臂目标姿态
  geometry_msgs__msg__Pose arm_pose_goal;
} robot_interfaces__msg__QtRecv;

// Struct for a sequence of robot_interfaces__msg__QtRecv.
typedef struct robot_interfaces__msg__QtRecv__Sequence
{
  robot_interfaces__msg__QtRecv * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__msg__QtRecv__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_INTERFACES__MSG__DETAIL__QT_RECV__STRUCT_H_

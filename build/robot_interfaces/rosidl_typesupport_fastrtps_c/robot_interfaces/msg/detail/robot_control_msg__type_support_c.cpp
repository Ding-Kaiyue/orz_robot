// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice
#include "robot_interfaces/msg/detail/robot_control_msg__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "robot_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "robot_interfaces/msg/detail/robot_control_msg__struct.h"
#include "robot_interfaces/msg/detail/robot_control_msg__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rosidl_runtime_c/primitives_sequence.h"  // motor_enable_flag, motor_mode, motor_msg
#include "rosidl_runtime_c/primitives_sequence_functions.h"  // motor_enable_flag, motor_mode, motor_msg

// forward declare type support functions


using _RobotControlMsg__ros_msg_type = robot_interfaces__msg__RobotControlMsg;

static bool _RobotControlMsg__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _RobotControlMsg__ros_msg_type * ros_message = static_cast<const _RobotControlMsg__ros_msg_type *>(untyped_ros_message);
  // Field name: motor_enable_flag
  {
    size_t size = ros_message->motor_enable_flag.size;
    auto array_ptr = ros_message->motor_enable_flag.data;
    cdr << static_cast<uint32_t>(size);
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: motor_mode
  {
    size_t size = ros_message->motor_mode.size;
    auto array_ptr = ros_message->motor_mode.data;
    cdr << static_cast<uint32_t>(size);
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: motor_msg
  {
    size_t size = ros_message->motor_msg.size;
    auto array_ptr = ros_message->motor_msg.data;
    cdr << static_cast<uint32_t>(size);
    cdr.serializeArray(array_ptr, size);
  }

  return true;
}

static bool _RobotControlMsg__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _RobotControlMsg__ros_msg_type * ros_message = static_cast<_RobotControlMsg__ros_msg_type *>(untyped_ros_message);
  // Field name: motor_enable_flag
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    if (ros_message->motor_enable_flag.data) {
      rosidl_runtime_c__boolean__Sequence__fini(&ros_message->motor_enable_flag);
    }
    if (!rosidl_runtime_c__boolean__Sequence__init(&ros_message->motor_enable_flag, size)) {
      fprintf(stderr, "failed to create array for field 'motor_enable_flag'");
      return false;
    }
    auto array_ptr = ros_message->motor_enable_flag.data;
    for (size_t i = 0; i < size; ++i) {
      uint8_t tmp;
      cdr >> tmp;
      array_ptr[i] = tmp ? true : false;
    }
  }

  // Field name: motor_mode
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    if (ros_message->motor_mode.data) {
      rosidl_runtime_c__uint8__Sequence__fini(&ros_message->motor_mode);
    }
    if (!rosidl_runtime_c__uint8__Sequence__init(&ros_message->motor_mode, size)) {
      fprintf(stderr, "failed to create array for field 'motor_mode'");
      return false;
    }
    auto array_ptr = ros_message->motor_mode.data;
    cdr.deserializeArray(array_ptr, size);
  }

  // Field name: motor_msg
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    if (ros_message->motor_msg.data) {
      rosidl_runtime_c__double__Sequence__fini(&ros_message->motor_msg);
    }
    if (!rosidl_runtime_c__double__Sequence__init(&ros_message->motor_msg, size)) {
      fprintf(stderr, "failed to create array for field 'motor_msg'");
      return false;
    }
    auto array_ptr = ros_message->motor_msg.data;
    cdr.deserializeArray(array_ptr, size);
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_robot_interfaces
size_t get_serialized_size_robot_interfaces__msg__RobotControlMsg(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotControlMsg__ros_msg_type * ros_message = static_cast<const _RobotControlMsg__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name motor_enable_flag
  {
    size_t array_size = ros_message->motor_enable_flag.size;
    auto array_ptr = ros_message->motor_enable_flag.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name motor_mode
  {
    size_t array_size = ros_message->motor_mode.size;
    auto array_ptr = ros_message->motor_mode.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name motor_msg
  {
    size_t array_size = ros_message->motor_msg.size;
    auto array_ptr = ros_message->motor_msg.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _RobotControlMsg__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_robot_interfaces__msg__RobotControlMsg(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_robot_interfaces
size_t max_serialized_size_robot_interfaces__msg__RobotControlMsg(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: motor_enable_flag
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }
  // member: motor_mode
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }
  // member: motor_msg
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);

    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = robot_interfaces__msg__RobotControlMsg;
    is_plain =
      (
      offsetof(DataType, motor_msg) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _RobotControlMsg__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_robot_interfaces__msg__RobotControlMsg(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_RobotControlMsg = {
  "robot_interfaces::msg",
  "RobotControlMsg",
  _RobotControlMsg__cdr_serialize,
  _RobotControlMsg__cdr_deserialize,
  _RobotControlMsg__get_serialized_size,
  _RobotControlMsg__max_serialized_size
};

static rosidl_message_type_support_t _RobotControlMsg__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_RobotControlMsg,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, robot_interfaces, msg, RobotControlMsg)() {
  return &_RobotControlMsg__type_support;
}

#if defined(__cplusplus)
}
#endif

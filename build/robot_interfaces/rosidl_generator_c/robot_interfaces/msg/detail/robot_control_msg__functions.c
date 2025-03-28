// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from robot_interfaces:msg/RobotControlMsg.idl
// generated code does not contain a copyright notice
#include "robot_interfaces/msg/detail/robot_control_msg__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `motor_enable_flag`
// Member `motor_mode`
// Member `motor_msg`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
robot_interfaces__msg__RobotControlMsg__init(robot_interfaces__msg__RobotControlMsg * msg)
{
  if (!msg) {
    return false;
  }
  // motor_enable_flag
  if (!rosidl_runtime_c__boolean__Sequence__init(&msg->motor_enable_flag, 0)) {
    robot_interfaces__msg__RobotControlMsg__fini(msg);
    return false;
  }
  // motor_mode
  if (!rosidl_runtime_c__uint8__Sequence__init(&msg->motor_mode, 0)) {
    robot_interfaces__msg__RobotControlMsg__fini(msg);
    return false;
  }
  // motor_msg
  if (!rosidl_runtime_c__double__Sequence__init(&msg->motor_msg, 0)) {
    robot_interfaces__msg__RobotControlMsg__fini(msg);
    return false;
  }
  return true;
}

void
robot_interfaces__msg__RobotControlMsg__fini(robot_interfaces__msg__RobotControlMsg * msg)
{
  if (!msg) {
    return;
  }
  // motor_enable_flag
  rosidl_runtime_c__boolean__Sequence__fini(&msg->motor_enable_flag);
  // motor_mode
  rosidl_runtime_c__uint8__Sequence__fini(&msg->motor_mode);
  // motor_msg
  rosidl_runtime_c__double__Sequence__fini(&msg->motor_msg);
}

bool
robot_interfaces__msg__RobotControlMsg__are_equal(const robot_interfaces__msg__RobotControlMsg * lhs, const robot_interfaces__msg__RobotControlMsg * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // motor_enable_flag
  if (!rosidl_runtime_c__boolean__Sequence__are_equal(
      &(lhs->motor_enable_flag), &(rhs->motor_enable_flag)))
  {
    return false;
  }
  // motor_mode
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->motor_mode), &(rhs->motor_mode)))
  {
    return false;
  }
  // motor_msg
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->motor_msg), &(rhs->motor_msg)))
  {
    return false;
  }
  return true;
}

bool
robot_interfaces__msg__RobotControlMsg__copy(
  const robot_interfaces__msg__RobotControlMsg * input,
  robot_interfaces__msg__RobotControlMsg * output)
{
  if (!input || !output) {
    return false;
  }
  // motor_enable_flag
  if (!rosidl_runtime_c__boolean__Sequence__copy(
      &(input->motor_enable_flag), &(output->motor_enable_flag)))
  {
    return false;
  }
  // motor_mode
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->motor_mode), &(output->motor_mode)))
  {
    return false;
  }
  // motor_msg
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->motor_msg), &(output->motor_msg)))
  {
    return false;
  }
  return true;
}

robot_interfaces__msg__RobotControlMsg *
robot_interfaces__msg__RobotControlMsg__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  robot_interfaces__msg__RobotControlMsg * msg = (robot_interfaces__msg__RobotControlMsg *)allocator.allocate(sizeof(robot_interfaces__msg__RobotControlMsg), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(robot_interfaces__msg__RobotControlMsg));
  bool success = robot_interfaces__msg__RobotControlMsg__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
robot_interfaces__msg__RobotControlMsg__destroy(robot_interfaces__msg__RobotControlMsg * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    robot_interfaces__msg__RobotControlMsg__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
robot_interfaces__msg__RobotControlMsg__Sequence__init(robot_interfaces__msg__RobotControlMsg__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  robot_interfaces__msg__RobotControlMsg * data = NULL;

  if (size) {
    data = (robot_interfaces__msg__RobotControlMsg *)allocator.zero_allocate(size, sizeof(robot_interfaces__msg__RobotControlMsg), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = robot_interfaces__msg__RobotControlMsg__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        robot_interfaces__msg__RobotControlMsg__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
robot_interfaces__msg__RobotControlMsg__Sequence__fini(robot_interfaces__msg__RobotControlMsg__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      robot_interfaces__msg__RobotControlMsg__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

robot_interfaces__msg__RobotControlMsg__Sequence *
robot_interfaces__msg__RobotControlMsg__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  robot_interfaces__msg__RobotControlMsg__Sequence * array = (robot_interfaces__msg__RobotControlMsg__Sequence *)allocator.allocate(sizeof(robot_interfaces__msg__RobotControlMsg__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = robot_interfaces__msg__RobotControlMsg__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
robot_interfaces__msg__RobotControlMsg__Sequence__destroy(robot_interfaces__msg__RobotControlMsg__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    robot_interfaces__msg__RobotControlMsg__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
robot_interfaces__msg__RobotControlMsg__Sequence__are_equal(const robot_interfaces__msg__RobotControlMsg__Sequence * lhs, const robot_interfaces__msg__RobotControlMsg__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!robot_interfaces__msg__RobotControlMsg__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
robot_interfaces__msg__RobotControlMsg__Sequence__copy(
  const robot_interfaces__msg__RobotControlMsg__Sequence * input,
  robot_interfaces__msg__RobotControlMsg__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(robot_interfaces__msg__RobotControlMsg);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    robot_interfaces__msg__RobotControlMsg * data =
      (robot_interfaces__msg__RobotControlMsg *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!robot_interfaces__msg__RobotControlMsg__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          robot_interfaces__msg__RobotControlMsg__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!robot_interfaces__msg__RobotControlMsg__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}

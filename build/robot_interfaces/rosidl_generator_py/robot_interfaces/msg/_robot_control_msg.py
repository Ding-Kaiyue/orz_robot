# generated from rosidl_generator_py/resource/_idl.py.em
# with input from robot_interfaces:msg/RobotControlMsg.idl
# generated code does not contain a copyright notice


# Import statements for member types

# Member 'motor_mode'
# Member 'motor_msg'
import array  # noqa: E402, I100

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_RobotControlMsg(type):
    """Metaclass of message 'RobotControlMsg'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('robot_interfaces')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'robot_interfaces.msg.RobotControlMsg')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__robot_control_msg
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__robot_control_msg
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__robot_control_msg
            cls._TYPE_SUPPORT = module.type_support_msg__msg__robot_control_msg
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__robot_control_msg

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class RobotControlMsg(metaclass=Metaclass_RobotControlMsg):
    """Message class 'RobotControlMsg'."""

    __slots__ = [
        '_motor_enable_flag',
        '_motor_mode',
        '_motor_msg',
    ]

    _fields_and_field_types = {
        'motor_enable_flag': 'sequence<boolean>',
        'motor_mode': 'sequence<uint8>',
        'motor_msg': 'sequence<double>',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.BasicType('boolean')),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.BasicType('uint8')),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.BasicType('double')),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.motor_enable_flag = kwargs.get('motor_enable_flag', [])
        self.motor_mode = array.array('B', kwargs.get('motor_mode', []))
        self.motor_msg = array.array('d', kwargs.get('motor_msg', []))

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.motor_enable_flag != other.motor_enable_flag:
            return False
        if self.motor_mode != other.motor_mode:
            return False
        if self.motor_msg != other.motor_msg:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def motor_enable_flag(self):
        """Message field 'motor_enable_flag'."""
        return self._motor_enable_flag

    @motor_enable_flag.setter
    def motor_enable_flag(self, value):
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, bool) for v in value) and
                 True), \
                "The 'motor_enable_flag' field must be a set or sequence and each value of type 'bool'"
        self._motor_enable_flag = value

    @builtins.property
    def motor_mode(self):
        """Message field 'motor_mode'."""
        return self._motor_mode

    @motor_mode.setter
    def motor_mode(self, value):
        if isinstance(value, array.array):
            assert value.typecode == 'B', \
                "The 'motor_mode' array.array() must have the type code of 'B'"
            self._motor_mode = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, int) for v in value) and
                 all(val >= 0 and val < 256 for val in value)), \
                "The 'motor_mode' field must be a set or sequence and each value of type 'int' and each unsigned integer in [0, 255]"
        self._motor_mode = array.array('B', value)

    @builtins.property
    def motor_msg(self):
        """Message field 'motor_msg'."""
        return self._motor_msg

    @motor_msg.setter
    def motor_msg(self, value):
        if isinstance(value, array.array):
            assert value.typecode == 'd', \
                "The 'motor_msg' array.array() must have the type code of 'd'"
            self._motor_msg = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, float) for v in value) and
                 all(not (val < -1.7976931348623157e+308 or val > 1.7976931348623157e+308) or math.isinf(val) for val in value)), \
                "The 'motor_msg' field must be a set or sequence and each value of type 'float' and each double in [-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000, 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000]"
        self._motor_msg = array.array('d', value)

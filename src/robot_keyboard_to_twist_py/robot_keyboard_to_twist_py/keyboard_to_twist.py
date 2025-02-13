import rclpy
from rclpy.node import Node
from std_msgs.msg import Int8
from rclpy.qos import qos_profile_system_default
import sys, select, termios, tty

BACKTOSTART = 0
PASSIVE = 1
JOINTCONTROL = 2
CARTESIAN = 3
MOVEJ = 4
MOVEL = 5
MOVEC = 6
TEACH = 7
TEACHREPEAT = 8
SAVESTATE = 9
TOSTATE = 10
CALIBRATION = 11

JOINT1_POSITIVE = 41
JOINT1_NEGATIVE = 42
JOINT2_POSITIVE = 43
JOINT2_NEGATIVE = 44
JOINT3_POSITIVE = 45
JOINT3_NEGATIVE = 46
JOINT4_POSITIVE = 47
JOINT4_NEGATIVE = 48
JOINT5_POSITIVE = 49
JOINT5_NEGATIVE = 50
JOINT6_POSITIVE = 51
JOINT6_NEGATIVE = 52
GRIPPER_OPEN = 53
GRIPPER_CLOSE = 54

FORWARD = 55
BACKWARD = 56
LEFT = 57
RIGHT = 58
UP = 59
DOWN = 60
ROLL_POSITIVE = 61
ROLL_NEGATIVE = 62
PITCH_POSITIVE = 63
PITCH_NEGATIVE = 64
YAW_POSITIVE = 65
YAW_NEGATIVE = 66


settings = termios.tcgetattr(sys.stdin)

msg = """
Reading from the keyboard and Publishing to Keyboard_control_mode!
----------------------------------------------------------------
`: BACKTOSTART
1: PASSIVE
2: JOINTCONTROL
3: CARTESIAN
4: MOVEJ
5: MOVEL
6: MOVEC
7: TEACH
8: TEACHREPEAT
9: SAVESTATE
0: TOSTATE
=: CALIBRATION

CTRL-C to quit.
"""

msg_jointcontrol = """
At this mode, you can directly set the speed of the 6 joints of the arm by long pressing the 
keyboard, thereby controlling the movement of the arm.
----------------------------------------------------------------------------------------------------
| Joint ID     |     0     |     1     |     2     |     3     |     4     |     5     |  Gripper  |          
----------------------------------------------------------------------------------------------------
| Keyboard     |    q/a    |    w/s    |    e/d    |    r/f    |    t/g    |    y/h    |    [/]    |
----------------------------------------------------------------------------------------------------
| Joint Action | positive/ | positive/ | positive/ | positive/ | positive/ | positive/ |   open/   |
| (right hand) | negative  | negative  | negative  | negative  | negative  | negative  |   close   |
----------------------------------------------------------------------------------------------------

CTRL-C to quit.
"""

msg_cartesian = """
At this mode, you can directly set the position and orientation of the end-effector of the arm 
by long pressing the keyboard, thereby controlling the movement of the arm.
--------------------------------------------------------------------------------------------------
|  Keyboard  |    q/a   |    w/s   |    e/d    |    r/f    |    t/g    |    y/h    |   Gripper   |
--------------------------------------------------------------------------------------------------
|    Key     | forward/ |   right/ |    up/    |    roll   |   pitch   |    yaw    |     [/]     |
|  Function  | backward |   left   |    down   |    ctrl   |   ctrl    |    ctrl   |  open/close |   
--------------------------------------------------------------------------------------------------     
"""

# 添加一个字典来映射按键和模式
key_mode_mapping = {
    '`': BACKTOSTART,
    '1': PASSIVE,
    '2': JOINTCONTROL,
    '3': CARTESIAN,
    '4': MOVEJ,
    '5': MOVEL,
    '6': MOVEC,
    '7': TEACH,
    '8': TEACHREPEAT,
    '9': SAVESTATE,
    '0': TOSTATE,
    '=': CALIBRATION,
}

key_jointctrl_mapping = {
    'q': JOINT1_POSITIVE,
    'w': JOINT2_POSITIVE,
    'e': JOINT3_POSITIVE,
    'r': JOINT4_POSITIVE,
    't': JOINT5_POSITIVE,
    'y': JOINT6_POSITIVE,
    'a': JOINT1_NEGATIVE,
    's': JOINT2_NEGATIVE,
    'd': JOINT3_NEGATIVE,
    'f': JOINT4_NEGATIVE,
    'g': JOINT5_NEGATIVE,
    'h': JOINT6_NEGATIVE,
    '[': GRIPPER_OPEN,
    ']': GRIPPER_CLOSE,
}

key_cartesian_mapping = {
    'q': FORWARD,
    'w': RIGHT,
    'e': UP,
    'r': ROLL_POSITIVE,
    't': PITCH_POSITIVE,
    'y': YAW_POSITIVE,
    'a': BACKWARD,
    's': LEFT,
    'd': DOWN,
    'f': ROLL_NEGATIVE,
    'g': PITCH_NEGATIVE,
    'h': YAW_NEGATIVE,
    '[': GRIPPER_OPEN,
    ']': GRIPPER_CLOSE,
}

# 定义每个模式的允许前序模式
mode_preconditions = {
    BACKTOSTART: [PASSIVE, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    PASSIVE: [BACKTOSTART, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    JOINTCONTROL: [BACKTOSTART, PASSIVE, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    CARTESIAN: [PASSIVE, JOINTCONTROL, MOVEJ, MOVEL, MOVEC],
    MOVEJ: [JOINTCONTROL, CARTESIAN, MOVEL, MOVEC],
    MOVEL: [JOINTCONTROL, CARTESIAN, MOVEJ, MOVEC],
    MOVEC: [JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL],
    TEACH: [JOINTCONTROL],
    TEACHREPEAT: [JOINTCONTROL], 
    SAVESTATE: [JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC],
    TOSTATE: [JOINTCONTROL],
    CALIBRATION: [PASSIVE]
}

def getKey():
    tty.setraw(sys.stdin.fileno())
    select.select([sys.stdin], [], [], 0)
    key = sys.stdin.read(1)
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key      

def main(args=None):
    if args is None:
        args = sys.argv

    rclpy.init(args=args)
    node = rclpy.create_node('keyboard_control_node')
    publisher_mode_ = node.create_publisher(Int8, 'keyboard_control_mode', qos_profile_system_default)
    publisher_joint_action_ = node.create_publisher(Int8, 'jointctrl_action', qos_profile_system_default)
    publisher_cartesian_ = node.create_publisher(Int8, 'cartesian_action', qos_profile_system_default)

    keyboard_control_working_mode = PASSIVE  # 上电默认电机为阻尼状态(实际上为速度模式速度为0)
    mode_msg = Int8()
    joint_action_msg = Int8()       # JOINTCTRL Mode
    cartesian_msg = Int8()          # CARTESIAN Mode

    try:
        print(msg)
        while(True):
            key = getKey()
            if key in key_mode_mapping:
                target_mode = key_mode_mapping[key]
                # 检查当前模式是否允许切换到目标模式
                if keyboard_control_working_mode in mode_preconditions[target_mode]:
                    keyboard_control_working_mode = target_mode
                    print(f"切换到模式 {target_mode}")
                    if keyboard_control_working_mode == JOINTCONTROL:
                        print(msg_jointcontrol)
                else:
                    print(f"当前模式 {keyboard_control_working_mode} 无法切换到模式 {target_mode}")
            elif key in key_jointctrl_mapping and keyboard_control_working_mode == JOINTCONTROL:
                joint_action_msg.data = key_jointctrl_mapping[key]
            elif key in key_cartesian_mapping and keyboard_control_working_mode == CARTESIAN:
                cartesian_msg.data = key_cartesian_mapping[key]
            else:
                if key == '\x03':
                    break
                print(f"无效按键: {key}")
            mode_msg.data = keyboard_control_working_mode
            publisher_mode_.publish(mode_msg)
            publisher_joint_action_.publish(joint_action_msg)
            publisher_cartesian_.publish(cartesian_msg)
    except KeyboardInterrupt:  
        pass
    
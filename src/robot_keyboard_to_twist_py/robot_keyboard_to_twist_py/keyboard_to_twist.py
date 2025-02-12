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
POSITIVE = 0x0A
NEGATIVE = 0x0B
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
| Keyboard     |    q/a    |    w/s    |    e/d    |    r/f    |    t/g    |    y/h    |    {/}    |
----------------------------------------------------------------------------------------------------
| Joint Action | positive/ | positive/ | positive/ | positive/ | positive/ | positive/ |    open   |
| (right hand) | negative  | negative  | negative  | negative  | negative  | negative  |   close   |
----------------------------------------------------------------------------------------------------

CTRL-C to quit.
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
    'q': POSITIVE,
    'w': POSITIVE,
    'e': POSITIVE,
    'r': POSITIVE,
    't': POSITIVE,
    'y': POSITIVE,
    'a': NEGATIVE,
    's': NEGATIVE,
    'd': NEGATIVE,
    'f': NEGATIVE,
    'g': NEGATIVE,
    'h': NEGATIVE,
    '{': POSITIVE,
    '}': NEGATIVE,
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
    publisher_ = node.create_publisher(Int8, 'keyboard_control_mode', qos_profile_system_default)
    keyboard_control_working_mode = PASSIVE  # 上电默认电机为阻尼状态(实际上为速度模式速度为0)
    mode_msg = Int8()

    try:
        print(msg)
        while(True):
            key = getKey()
            if key in key_mode_mapping:
                target_mode = key_mode_mapping[key]
                # 检查当前模式是否允许切换到目标模式
                if keyboard_control_working_mode in mode_preconditions[target_mode]:
                    keyboard_control_working_mode = target_mode
                else:
                    print(f"当前模式 {keyboard_control_working_mode} 无法切换到模式 {target_mode}")
            elif key in key_jointctrl_mapping[key] and keyboard_control_working_mode == JOINTCONTROL:
                    print(msg_jointcontrol)
            else:
                if key == '\x03':
                    break
                print(f"无效按键: {key}")
            mode_msg.data = keyboard_control_working_mode
            publisher_.publish(mode_msg)
    except KeyboardInterrupt: 
        pass
    
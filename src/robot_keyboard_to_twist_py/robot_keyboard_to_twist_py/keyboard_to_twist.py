import rclpy
from rclpy.node import Node
from std_msgs.msg import Int8
from std_msgs.msg import Bool
from std_msgs.msg import Float64MultiArray
from rclpy.qos import QoSProfile, qos_profile_system_default
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

msg_movej = """
At this mode, please enter the pose data in the format of six numbers separated
by spaces.
    For example: 0.5 0.1 0.1 0.5 -0.2 0.5.
Make sure your input is correct. After that, press the “Enter” key twice, and 
the robotic arm will start moving.

Note: The purpose of pressing “Enter” twice is that when using keyboard control,
we allow you to continue entering the next pose after pressing “Enter” once. 
Press “Enter” twice after finishing your input, and the robotic arm will execute
the tasks in sequence.
--------------------------------------------------------------------------------
Please input:
"""

msg_movel = """
At this mode, please enter the goal robotic arm pose separated by spaces.
    For example: 0.5 0.1 0.1 0.5 -0.2 0.5.
Make sure your input is correct. After that, press the “Enter” key twice, and 
the robotic arm will start moving.

Note: The purpose of pressing “Enter” twice is that when using keyboard control,
we allow you to continue entering the next pose after pressing “Enter” once. 
Press “Enter” twice after finishing your input, and the robotic arm will execute
the tasks in sequence.
--------------------------------------------------------------------------------
Please input:
"""

msg_movec = """
At this mode, please input the poses twice, which are the intermediate pose and
the final pose respectively.
    For example: Enter the intermediate pose: type “0 0 0 0.45 0 0.4” 
                 and then press the “Enter” key.
                 Next, enter the final pose: type “0 0 0 0.45 0.2 0.2” 
                 and then press the “Enter” key twice. 
                 
                 After that, the corresponding operation will be executed.
--------------------------------------------------------------------------------
Please input:
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
    BACKTOSTART: [BACKTOSTART, PASSIVE, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    PASSIVE: [PASSIVE, BACKTOSTART, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    JOINTCONTROL: [JOINTCONTROL, BACKTOSTART, PASSIVE, CARTESIAN, MOVEJ, MOVEL, MOVEC, TEACH],
    CARTESIAN: [CARTESIAN, PASSIVE, JOINTCONTROL, MOVEJ, MOVEL, MOVEC],
    MOVEJ: [MOVEJ, JOINTCONTROL, CARTESIAN, MOVEL, MOVEC],
    MOVEL: [MOVEL, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEC],
    MOVEC: [MOVEC, JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL],
    TEACH: [JOINTCONTROL],
    TEACHREPEAT: [JOINTCONTROL], 
    SAVESTATE: [JOINTCONTROL, CARTESIAN, MOVEJ, MOVEL, MOVEC],
    TOSTATE: [JOINTCONTROL],
    CALIBRATION: [PASSIVE]
}

class KeyboardControlNode(Node):
    def __init__(self):
        super().__init__('keyboard_control_node')
        self.publisher_mode_ = self.create_publisher(Int8, 'keyboard_control_mode', qos_profile_system_default)
        self.publisher_joint_action_ = self.create_publisher(Int8, 'jointctrl_action', qos_profile_system_default)
        self.publisher_cartesian_ = self.create_publisher(Int8, 'cartesian_action', qos_profile_system_default)
        self.publisher_movej_ = self.create_publisher(Float64MultiArray, 'movej_action', qos_profile_system_default)
        self.publisher_movel_ = self.create_publisher(Float64MultiArray, 'movel_action', qos_profile_system_default)
        self.publisher_movec_ = self.create_publisher(Float64MultiArray, 'movec_action', qos_profile_system_default)

        self.keyboard_control_working_mode = PASSIVE
        self.mode_msg = Int8()
        self.joint_action_msg = Int8()      # JOINTCTRL Mode
        self.cartesian_msg = Int8()         # CARTESIAN Mode

        # 保存终端设置
        self.settings = termios.tcgetattr(sys.stdin)

    def get_key(self):
        tty.setraw(sys.stdin.fileno())
        select.select([sys.stdin], [], [], 0)
        key = sys.stdin.read(1)
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
        return key

    def run(self):
        try:
            print(msg)
            while(True):
                key = self.get_key()
                if key in key_mode_mapping:
                    target_mode = key_mode_mapping[key]
                    # 检查当前模式是否允许切换到目标模式
                    if self.keyboard_control_working_mode in mode_preconditions[target_mode]:
                        self.keyboard_control_working_mode = target_mode
                        print(f"切换到模式 {target_mode}")
                        if self.keyboard_control_working_mode == JOINTCONTROL:
                            print(msg_jointcontrol)
                        elif self.keyboard_control_working_mode == MOVEJ:
                            print(msg_movej)
                            all_joint_positions = []
                            empty_input_count_j = 0
                            while True:
                                input_str = input()
                                if input_str:
                                    try:
                                        joint_positions = [float(x) for x in input_str.split()]
                                        if len(joint_positions)!= 6:
                                            print("Invalid input. Please enter exactly six joint positions.")
                                        else:
                                            all_joint_positions.extend(joint_positions)
                                            empty_input_count_j = 0
                                    except ValueError:
                                        print("Invalid input. Please enter valid joint positions separated by spaces.")
                                else:
                                    empty_input_count_j += 1
                                    if empty_input_count_j == 2:
                                        break
                            joint_position_msgs = Float64MultiArray()
                            joint_position_msgs.data = all_joint_positions
                            self.publisher_movej_.publish(joint_position_msgs)
                        elif self.keyboard_control_working_mode == MOVEL:
                            print(msg_movel)
                            all_arm_positions = []
                            empty_input_count_l = 0
                            while True:
                                input_str = input()
                                if input_str:
                                    try:
                                        arm_positions = [float(x) for x in input_str.split()]
                                        if len(arm_positions)!= 6:
                                            print("Invalid input. Please enter exactly six values: x, y, z, roll, yaw and pitch.")
                                        else:
                                            all_arm_positions.extend(arm_positions)
                                            empty_input_count_l = 0
                                    except ValueError:
                                        print("Invalid input. Please enter valid arm pose separated by spaces.")
                                else:
                                    empty_input_count_l += 1
                                    if empty_input_count_l == 2:
                                        break
                            arm_position_msgs = Float64MultiArray()
                            arm_position_msgs.data = all_arm_positions
                            self.publisher_movel_.publish(arm_position_msgs)

                        elif self.keyboard_control_working_mode == MOVEC:
                            print(msg_movec)
                            all_circle_arm_positions = []
                            empty_input_count_c = 0
                            while True:
                                input_str = input()
                                if input_str:
                                    try:
                                        circle_arm_positions = [float(x) for x in input_str.split()]
                                        if len(circle_arm_positions) != 6:
                                            print("Invalid input. Please enter exactly six values: x, y, z, roll, yaw and pitch.")
                                        else:
                                            all_circle_arm_positions.extend(circle_arm_positions)
                                            empty_input_count_c = 0
                                    except ValueError:
                                        print("Invalid input. Please enter valid arm pose separated by spaces.")
                                else:
                                    empty_input_count_c += 1
                                    if empty_input_count_c == 2:
                                        break
                            arm_circle_position_msgs = Float64MultiArray()
                            arm_circle_position_msgs.data = all_circle_arm_positions
                            self.publisher_movec_.publish(arm_circle_position_msgs)
                    else:
                        print(f"当前模式 {self.keyboard_control_working_mode} 无法切换到模式 {target_mode}")
                elif key in key_jointctrl_mapping and self.keyboard_control_working_mode == JOINTCONTROL:
                    self.joint_action_msg.data = key_jointctrl_mapping[key]
                elif key in key_cartesian_mapping and self.keyboard_control_working_mode == CARTESIAN:
                    self.cartesian_msg.data = key_cartesian_mapping[key]
                else:
                    if key == '\x03':
                        break
                    print(f"无效按键: {key}")

                self.mode_msg.data = self.keyboard_control_working_mode
                self.publisher_mode_.publish(self.mode_msg)
                self.publisher_joint_action_.publish(self.joint_action_msg)
                self.publisher_cartesian_.publish(self.cartesian_msg)
        except KeyboardInterrupt:  
            pass
    

def main(args=None):
    if args is None:
        args = sys.argv
    rclpy.init(args=args)
    node = KeyboardControlNode()
    node.run()
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
# <span style="color:#f38b83;">上位机需求说明</span>
## <span style="color:#B885B0;">通信建立</span>
设置好本机IP为`192.168.1.xx`, 上位机作为TCP客户端，输入机械臂的IP地址(`192.168.1.200`)和端口号(`8899`)连接机械臂，机械臂作为TCP服务器，端口号为`8899`.

<p align="center">
    <img src="img/image01.png" alt="网络连接" width="140" />
</p>

点击`Connect`后, 连接成功则提示成功信息，否则提示连接失败，当连接断开时也要提示连接未连接信息

<p align="center">
    <img src="img/image02.png" alt="连接状态显示" width="180" />
</p>

连接成功时按钮变为`disconnect`; 连接失败时按钮变为`connect`, 默认为`connect`.

## <span style="color:#B885B0;">控制</span>
### <span style="color:#8DB732;">正运动</span>
* 给定六个关节位置J1~J6, slider的范围后续再定，默认值均为0；
* 给定夹爪的位置、速度和力， slider的范围均为0~255, 默认值为0, 100, 20
* 在拖动过程中, 将机械臂目标状态实时显示出来，点击[这里](#arm_show)跳转到具体显示说明
* 点击确认按钮后，将这九个值发送给机械臂
<p align="center">
    <img src="img/image03.png" alt="正运动" width="460" />
</p>

* <span id="protocol-forward-kinematics">发送协议为</span>

    | data[0] | data[1] | data[2] | data[3] | data[4] | data[5] | data[6] | data[7] | data[8] | data[9] |
    |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |
    | 1145(head) |  joint1 |  joint2 |  joint3 |  joint4 |  joint5 |  joint6 | gripper_pos | gripper_vel | gripper_effort |

### <span style="color:#8DB732;">逆运动</span>
* 第一个Tab为机械臂平移指令，依次为"z pos + value", "x pos + value", "z pos - value", "y pos - value", "y pos + value", "x pos - value"
* 第二个Tab为机械臂旋转指令，依次为"z rot + value", "x rot + value", "z rot - value", "y rot + value", "y rot - value, " x rot - value"
* 在点击时出现点击特效(在本例中，点击时晕染为黄色，松开后晕染恢复为灰色)
<p align="center">
    <img src="img/image04.png" alt="平移-未点击" width="125" />
    <img src="img/image05.png" alt="旋转-未点击" width="125" />
    <img src="img/image06.png" alt="平移-已点击" width="125" />
    <img src="img/image07.png" alt="旋转-已点击" width="125" />
</p>

* 点击时提示已发送的信息
  <p align="center">
    <img src="img/image08.png" alt="正运动" width="180" />
    <img src="img/image09.png" alt="正运动" width="180" />
  </p>

* 发送协议为
  
    | data[0] | data[1] | data[2] | data[3] | data[4] | data[5] | data[6] |   Operation   |
    |  :---:  |  :---:  |  :---:  |  :---:  | :---: | :---: | :---:|      :---:     |
    |  0x01   |  0x01   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | z pos - value |
    |  0x01   |  0x01   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | z pos + value |
    |  0x01   |  0x02   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | z rot - value |
    |  0x01   |  0x02   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | z rot + value |
    |  0x02   |  0x01   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | x pos - value |
    |  0x02   |  0x01   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | x pos + value |
    |  0x02   |  0x01   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | x rot - value |
    |  0x02   |  0x01   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | x rot + value |
    |  0x03   |  0x01   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | y pos - value |
    |  0x03   |  0x01   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | y pos + value |
    |  0x03   |  0x02   |   0x00  |  value  | gripper_pos | gripper_vel | gripper_effort | y rot - value |
    |  0x03   |  0x02   |   0x01  |  value  | gripper_pos | gripper_vel | gripper_effort | y rot + value |
 短按: pos平移的value单位为cm, 暂定为1, rot旋转的value单位为度，暂定为1
 长按: 根据按下时间长短决定value的值，5s对应10cm(举例说明，不一定这个速度)，松开时发送

### <span style="color:#8DB732;">示教</span>
示教功能共三个putton
* 开始示教
* 停止示教
* 重现轨迹(需要将`Reappear`修改为`Replay`)
  
  <p align="center">
    <img src="img/image12.png" alt="示教" width="130" />
  </p>
* 发送协议为
  | data[0] | data[1] | data[2] | data[3] |    Operation   |
  |  :---:  |  :---:  |  :---:  |  :---:  |      :---:     |
  |  0x00   |  0x00   |   0x00  |   0x01  |      Record    |
  |  0x00   |  0x00   |   0x00  |   0x02  |   Stop Record  |
  |  0x00   |  0x00   |   0x00  |   0x03  |      Replay    |

### <span style="color:#8DB732;">特殊状态</span>
* 初始姿态: 机械位置的初始姿态
* 使能失能(初始时为Enable, 点击后自动变为disable)
* 急停
* XYPlane(将2DPlane修改为XYPlane)
* Initial(将3DSpace修改为Initial, 指的是运动规划的初始姿态)

  <p align="center">
    <img src="img/image13.png" alt="示教" width="150" />
  </p>
* 发送协议为
  | data[0] | data[1] | data[2] | data[3] |    Operation   |
  |  :---:  |  :---:  |  :---:  |  :---:  |      :---:     |
  |  0x00   |  0x00   |   0x00  |   0x04  |      Enable    |
  |  0x00   |  0x00   |   0x00  |   0x05  |     Disable    |
  |  0x00   |  0x00   |   0x00  |   0x06  | Emergency Stop |

  `Home`, `XYPlane`, `Initial` 三个按钮的协议使用[正运动](#protocol-forward-kinematics)的协议.
  * Home: J1~J6均为0, GP, GV, GF分别为0, 100, 20
  * XYPlane: J1~J6分别为某个固定值，暂定为0, 后续需要修改，GP, GV, GF分别为0, 100, 20
  * Initial: J1~J6分别为某个固定值，暂定为0, 后续需要修改，GP, GV, GF分别为0, 100, 20

## <span id="arm_show" style="color:#B885B0;">显示</span>
使用OpenGl显示机械臂的目标位姿和机械臂的反馈位姿，其中不透明的机械臂为目标位姿，透明的机械臂为通过TCP服务器反馈得到的反馈位姿
  <p align="center">
    <img src="img/image10.png" alt="机械臂显示" width="180" />
  </p>
  <p align="center">
    <img src="img/image11.png" alt="机械臂显示" width="450" />
  </p>

## <span style="color:#B885B0;">菜单栏</span>
**Analyze**
* 在Analyse中添加两个Action, 分别为J1～J6的反馈(无需夹爪的反馈)和开发者模式
* J1~J6的反馈为读取50ms反馈一次的关节位置，可画成六幅波形图，接收协议为
  | data[0] | data[1] | data[2] | data[3] | data[4] | data[5] | data[6] | data[7] |
  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  |  :---:  | :---: |  
  | 1145(head) |  joint1 |  joint2 |  joint3 |  joint4 |  joint5 |  joint6 | 14(tail)

* 开发者模式功能如下图所示，用来设置六个joint的限位和零点设置(范围待定)
  <p align="center">
    <img src="img/image14.png" alt="示教" width="360" />
  </p>
**Help**
* 版本信息及版权所有
  
  <p align="center">
    <img src="img/image15.png" alt="示教" width="360" />
  </p>
* 更新固件
  
---
Ref: https://github.com/Ding-Kaiyue/DRobot_Ctrl.git
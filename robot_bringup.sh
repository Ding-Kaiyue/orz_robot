#!/bin/bash

source /home/orin/Projects/orz_robot/install/setup.bash

sudo ip link set can0 up type can bitrate 1000000

ros2 launch robot_bringup robot_real.launch.py > ./start_up/start.log

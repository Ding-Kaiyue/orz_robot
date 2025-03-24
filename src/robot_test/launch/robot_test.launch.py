from launch import LaunchDescription
from launch_ros.actions import Node
# from launch.actions import SetParameter

def generate_launch_description():
    ld = LaunchDescription()
    
    robot_test = Node (
        package = 'robot_test',
        executable ='robot_test',
    )
    ld.add_action(robot_test)
    return ld

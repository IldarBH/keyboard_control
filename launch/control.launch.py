import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

PACKAGE_NAME = 'keyboard_control'
EXECUTABLE_NAME = 'keyboard_control_node'

def generate_launch_description():
    this_pkg_path = get_package_share_directory(PACKAGE_NAME)
    default_config = os.path.join(this_pkg_path, 'config', 'diff_drive_bindings.yaml')
    config_arg = DeclareLaunchArgument(
        name='config', 
        default_value=str(default_config), 
        description='Path to the bindings YAML file'
    )
    control_node = Node(
        package=PACKAGE_NAME, 
        executable=EXECUTABLE_NAME, 
        parameters=[{'config': LaunchConfiguration('config')}],
        output='screen'
    )
    return LaunchDescription([config_arg, control_node])
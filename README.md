# Keyboard Control Node

## Build

```
colcon build --packages-select keyboard_control
```

## Run

General usage:
```
ros2 run keyboard_control keyboard_control_node --ros-args -p config:=<path_to_config_file>
```

Example with diff drive config and debug logging:
```
ros2 run keyboard_control keyboard_control_node --ros-args -p config:=src/keyboard_control/config/diff_drive_bindings.yaml --log-level debug
```

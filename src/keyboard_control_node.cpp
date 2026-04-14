#include "keyboard_control_cpp/keyboard_control_node.hpp"

#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>

KeyboardControlNode::KeyboardControlNode(
    const std::string& node_name,
    const std::chrono::milliseconds& update_rate)
: Node(node_name)
{
    const auto config_file = this->declare_parameter("config", "config.yaml");
    bindings_ = read_bindings(config_file);
    for (const auto& [_, binding] : bindings_) {
        for (const auto& action : binding.actions) {
            if (publishers_.find(action.topic_name) == publishers_.end()) {
                publishers_[action.topic_name] = this->create_publisher<std_msgs::msg::Float64>(action.topic_name, 10);
                RCLCPP_INFO(this->get_logger(), "Created publisher for topic '%s'", action.topic_name.c_str());
            }
        }
    }
    ts_guard_ = std::make_unique<TerminalSettingsGuard>();
    timer_ = this->create_wall_timer(update_rate, std::bind(&KeyboardControlNode::update_, this));
    RCLCPP_INFO(this->get_logger(), "Keyboard Control Started.");
}

KeyboardControlNode::~KeyboardControlNode()
{
    RCLCPP_INFO(this->get_logger(), "Shutting down KeyboardControlNode");
}

void KeyboardControlNode::update_()
{
    const auto key = ts_guard_->get_key();
    if (key == 0) {
        return; // No key was pressed
    }
    
    if (const auto& it = bindings_.find(key); it != bindings_.end()) {
        RCLCPP_DEBUG(this->get_logger(), "Key '%c' pressed: executing binding '%s'", key, it->second.name.c_str());
        process_key_(it->second);
    } else {
        RCLCPP_WARN(this->get_logger(), "Key '%c' pressed: no binding found", key);
        return;
    }
}

void KeyboardControlNode::process_key_(const KeyBinding& binding)
{
    // trajectory_msgs::msg::JointTrajectory traj_msg;
    // traj_msg.header.stamp = this->now();
    // traj_msg.joint_names.reserve(binding.actions.size());
    
    // trajectory_msgs::msg::JointTrajectoryPoint traj_point;
    // for (const auto& action : binding.actions) {
    //     traj_msg.joint_names.push_back(action.joint_name);
    //     traj_point.velocities.push_back(action.value);
    // }
    // traj_point.time_from_start = rclcpp::Duration::from_seconds(0.1);
    // traj_msg.points.push_back(traj_point);
    for (const auto& action : binding.actions) {
        if (const auto& it = publishers_.find(action.topic_name); it != publishers_.end()) {
            std_msgs::msg::Float64 msg;
            msg.data = action.value;
            it->second->publish(msg);
        }
    }
}


int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<KeyboardControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
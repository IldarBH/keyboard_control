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
                current_values_[action.topic_name] = 0.0f;
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
    for (const auto& action : binding.actions) {
        if (const auto& it = publishers_.find(action.topic_name); it != publishers_.end()) {
            if (binding.type == ControlType::RELATIVE) {
                current_values_[action.topic_name] += action.value;
            } else {
                current_values_[action.topic_name] = action.value;
            }
            std_msgs::msg::Float64 msg;
            msg.data = current_values_[action.topic_name];
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
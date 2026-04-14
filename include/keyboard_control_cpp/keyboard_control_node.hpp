#include "keyboard_control_cpp/keyboard_control.hpp"

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>

class KeyboardControlNode : public rclcpp::Node
{
public:
    KeyboardControlNode(
        const std::string& node_name = "keyboard_control_node",
        const std::chrono::milliseconds& update_rate = std::chrono::milliseconds(100)
    );

    ~KeyboardControlNode();

private:
    void update_();

    void process_key_(const KeyBinding& binding);
private:
    BindingMap bindings_;
    
    rclcpp::TimerBase::SharedPtr timer_;
    std::unique_ptr<TerminalSettingsGuard> ts_guard_;
    std::unordered_map<std::string, rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr> publishers_;
};

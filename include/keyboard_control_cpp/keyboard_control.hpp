#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <chrono>
#include <string>
#include <memory>

#include <termios.h>

class TermialSettingsGuard
{
public:
    TermialSettingsGuard();

    ~TermialSettingsGuard();

    char get_key() const;
private:
    struct termios original_settings_;
};

class KeyboardControlNode : public rclcpp::Node
{
public:
    KeyboardControlNode(
        const std::string& node_name = "keyboard_control_node",
        const std::string& cmd_topic_name = "cmd_vel", 
        const std::chrono::milliseconds& update_rate = std::chrono::milliseconds(100)
    );

    ~KeyboardControlNode();

private:
    void update_();

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::unique_ptr<TermialSettingsGuard> ts_guard_;
};
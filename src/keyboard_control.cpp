#include "keyboard_control_cpp/keyboard_control.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

inline void update_termios(int fd, int flags)
{
    struct termios term;
    tcgetattr(fd, &term);
    term.c_lflag &= flags;
    tcsetattr(fd, TCSANOW, &term);
}

TermialSettingsGuard::TermialSettingsGuard()
{
    tcgetattr(STDIN_FILENO, &original_settings_);       // Save original terminal settings.
    update_termios(STDIN_FILENO, ~(ICANON | ECHO));     // Disable canonical mode and echo.
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);           // Set non-blocking mode.
}

TermialSettingsGuard::~TermialSettingsGuard()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings_);      // Restore original terminal settings.
    fcntl(STDIN_FILENO, F_SETFL, 0);                            // Clear non-blocking mode.
}

char TermialSettingsGuard::get_key() const
{
    char key;
    ssize_t n_bytes = read(STDIN_FILENO, &key, 1);
    if (n_bytes > 0)
        return key;
    return 0;
}

KeyboardControlNode::KeyboardControlNode(
    const std::string& node_name,
    const std::string& cmd_topic_name, 
    const std::chrono::milliseconds& update_rate)
: Node(node_name)
{
    RCLCPP_DEBUG(this->get_logger(), "Initializing KeyboardControlNode with node_name=%s, cmd_topic_name=%s, update_rate=%ldms", 
        node_name.c_str(), cmd_topic_name.c_str(), update_rate.count());
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(cmd_topic_name, 10);
    timer_ = this->create_wall_timer(update_rate, std::bind(&KeyboardControlNode::update_, this));
    RCLCPP_INFO(this->get_logger(), "Keyboard Control Started. Use WASD to move, Q to quit");
    
    ts_guard_ = std::make_unique<TermialSettingsGuard>();
}


KeyboardControlNode::~KeyboardControlNode()
{
    RCLCPP_INFO(this->get_logger(), "Shutting down KeyboardControlNode");
}

void KeyboardControlNode::update_()
{
    const auto key = ts_guard_->get_key();
    if (key == 'q')
    {
        RCLCPP_INFO(this->get_logger(), "Exit key pressed. Shutting down.");
        geometry_msgs::msg::Twist stop_msg;
        publisher_->publish(stop_msg);
        rclcpp::shutdown();
        return;
    }
    geometry_msgs::msg::Twist message;
    switch (key)
    {
        case 'w':
            message.linear.x = 0.5;
            break;
        case 's':
            message.linear.x = -0.5;
            break;
        case 'a':
            message.angular.z = 1.0;
            break;
        case 'd':
            message.angular.z = -1.0;
            break;
    }
    publisher_->publish(message);
}

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<KeyboardControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
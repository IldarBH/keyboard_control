#include "keyboard_control_cpp/keyboard_control.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<JointAction> {

    static bool decode(const Node& node, JointAction& rhs) 
    {
        if(!node.IsMap()) {
            return false;
        }

        rhs.joint_name = node["joint"].as<std::string>();
        rhs.value = node["value"].as<float>();
        return true;
    }
};

template<>
struct convert<KeyBinding> {
    static bool decode(const Node& node, KeyBinding& rhs) 
    {
        if(!node.IsMap()) {
            std::cerr << "Invalid key binding format. Expected a map." << std::endl;
            return false;
        }
        rhs.key = node["key"].as<std::string>();
        rhs.name = node["name"].as<std::string>();
        const auto type_str = node["type"].as<std::string>();
        if (type_str == "relative") {
            rhs.type = ControlType::RELATIVE;
        } else if (type_str == "absolute") {
            rhs.type = ControlType::ABSOLUTE;
        } else {
            std::cerr << "Unknown control type: " << type_str << ". Binding: " << rhs.name << std::endl;
            return false;
        }
        rhs.actions = node["actions"].as<std::vector<JointAction>>();
        return true;
    };
};
}

BindingMap read_bindings(const std::string& filename)
{
    BindingMap bindings;
    const auto root_node = YAML::LoadFile(filename);
    if (const auto bindings_node = root_node["key_bindings"]; bindings_node){
        for (const auto& binding_node : bindings_node) {
            const auto binding = binding_node.as<KeyBinding>();
            if (!binding.key.empty()) {
                bindings[binding.key[0]] = binding;
                std::cout << "Loaded binding: " << binding.key << " -> " << binding.name << std::endl;
            }
        }
    } else {
        std::cout << "empty" << std::endl;
    }
    return bindings;
}

namespace {
    inline void update_termios(int fd, int flags)
    {
        struct termios term;
        tcgetattr(fd, &term);
        term.c_lflag &= flags;
        tcsetattr(fd, TCSANOW, &term);
    }
}


TerminalSettingsGuard::TerminalSettingsGuard()
{
    tcgetattr(STDIN_FILENO, &original_settings_);       // Save original terminal settings.
    update_termios(STDIN_FILENO, ~(ICANON | ECHO));     // Disable canonical mode and echo.
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);           // Set non-blocking mode.
}

TerminalSettingsGuard::~TerminalSettingsGuard()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings_);      // Restore original terminal settings.
    fcntl(STDIN_FILENO, F_SETFL, 0);                            // Clear non-blocking mode.
}

char TerminalSettingsGuard::get_key() const
{
    char key;
    ssize_t n_bytes = read(STDIN_FILENO, &key, 1);
    if (n_bytes > 0)
        return key;
    return 0;
}
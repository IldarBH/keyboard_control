#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <termios.h>

struct JointAction
{
    std::string joint_name;
    std::string topic_name;
    float value;
};

enum class ControlType
{
    RELATIVE,
    ABSOLUTE
};

struct KeyBinding
{
    char key;
    std::string name;
    ControlType type;
    std::vector<JointAction> actions;
};

using BindingMap = std::unordered_map<char, KeyBinding>;

BindingMap read_bindings(const std::string& filename);

class TerminalSettingsGuard
{
public:
    TerminalSettingsGuard();

    ~TerminalSettingsGuard();

    static char get_key();
private:
    struct termios original_settings_;
};

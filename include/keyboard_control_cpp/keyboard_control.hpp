#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <termios.h>

struct JointAction
{
    std::string joint_name;
    float value;
};

enum class ControlType
{
    RELATIVE,
    ABSOLUTE
};

struct KeyBinding
{
    std::string key;
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

    char get_key() const;
private:
    struct termios original_settings_;
};

#include "spin_button_dim.hpp"
#include <iomanip>

namespace horizon {
SpinButtonDim::SpinButtonDim() : Gtk::SpinButton()
{
    set_increments(.1e6, .01e6);
    set_width_chars(10);
}

bool SpinButtonDim::on_output()
{
    auto adj = get_adjustment();
    double value = adj->get_value();

    std::stringstream stream;
    stream << std::fixed << std::setprecision(3) << value / 1e6 << " mm";

    set_text(stream.str());
    return true;
}

static bool is_dec(char c)
{
    return (c == '.') || (c == ',');
}

enum class Operation { INVALID, ADD, SUB, MUL, DIV, AVG };

static Operation op_from_char(char c)
{
    switch (c) {
    case '-':
        return Operation::SUB;
    case '+':
        return Operation::ADD;
    case '*':
        return Operation::MUL;
    case '/':
        return Operation::DIV;
    case '|':
        return Operation::AVG;

    default:
        return Operation::INVALID;
    }
}


static int64_t parse_str(const Glib::ustring &s)
{
    int64_t value1 = 0;
    int64_t value = 0;
    int64_t mul = 1000000;
    int sign = 1;
    enum class State { SIGN, INT, DECIMAL };
    auto state = State::SIGN;
    bool parsed_any = false;
    Operation operation = Operation::INVALID;
    for (const auto c : s) {
        switch (state) {
        case State::SIGN:
            if (c == '-') {
                sign = -1;
                state = State::INT;
            }
            else if (is_dec(c)) {
                state = State::DECIMAL;
                mul /= 10;
            }
            else if (isdigit(c)) {
                value += mul * (c - '0');
                state = State::INT;
                parsed_any = true;
            }
            break;


        case State::INT:
            if (isdigit(c)) {
                value *= 10;
                value += mul * (c - '0');
                parsed_any = true;
            }
            else if (is_dec(c)) {
                state = State::DECIMAL;
                mul /= 10;
            }
            else if (op_from_char(c) != Operation::INVALID && operation == Operation::INVALID) {
                operation = op_from_char(c);
                value1 = value * sign;
                sign = 1;
                mul = 1000000;
                value = 0;
                state = State::SIGN;
            }
            break;

        case State::DECIMAL:
            if (isdigit(c)) {
                value += mul * (c - '0');
                mul /= 10;
                parsed_any = true;
            }
            else if (op_from_char(c) != Operation::INVALID && operation == Operation::INVALID) {
                operation = op_from_char(c);
                value1 = value * sign;
                sign = 1;
                mul = 1000000;
                value = 0;
                state = State::SIGN;
            }
            break;
        }
    }
    if (!parsed_any)
        throw std::runtime_error("parse error");
    value *= sign;
    switch (operation) {
    case Operation::ADD:
        return value1 + value;
    case Operation::SUB:
        return value1 - value;
    case Operation::AVG:
        return (value1 + value) / 2;
    case Operation::DIV:
        return value1 / (((double)value) / 1e6);
    case Operation::MUL:
        return value1 * (((double)value) / 1e6);
    default:
        return value;
    }
}

int SpinButtonDim::on_input(double *v)
{
    auto txt = get_text();
    int64_t va = 0;
    try {
        va = parse_str(txt);
        *v = va;
    }
    catch (const std::exception &e) {
        return false;
    }
    return true;
}
} // namespace horizon

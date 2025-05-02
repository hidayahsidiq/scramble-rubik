#include "StopwatchWindow.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <gdk/gdkkeysyms.h>

StopwatchWindow::StopwatchWindow() {
    set_title("Rubik's Timer");
    set_default_size(400, 500);

    m_key_controller = Gtk::EventControllerKey::create();
    m_key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &StopwatchWindow::on_key_pressed), false);
    add_controller(m_key_controller);


    m_scramble_selector.append("2x2");
    m_scramble_selector.append("3x3");
    m_scramble_selector.append("4x4");
    m_scramble_selector.append("5x5");
    m_scramble_selector.set_active(1);
    m_scramble_selector.signal_changed().connect(sigc::mem_fun(*this, &StopwatchWindow::on_scramble_selector_changed));

    m_reset_button.set_label("Reset");
    m_reset_button.signal_clicked().connect(sigc::mem_fun(*this, &StopwatchWindow::on_reset_button_clicked));

    m_reset_session_button.set_label("Reset Session");
    m_reset_session_button.signal_clicked().connect(sigc::mem_fun(*this, &StopwatchWindow::on_reset_session_clicked));

    m_theme_switch.set_active(false);
    m_theme_switch.signal_state_set().connect(sigc::mem_fun(*this, &StopwatchWindow::on_theme_switch_toggled), false);
    m_theme_box.set_spacing(10);
    m_theme_box.append(m_theme_label);
    m_theme_box.append(m_theme_switch);

    m_scramble_label.set_text("Scramble: ");
    m_timer_label.set_text("00:00.000");
    m_inspection_label.set_text("");
    m_avg5_label.set_text("Ao5: -");
    m_avg12_label.set_text("Ao12: -");

    m_main_box.set_margin(10);
    m_main_box.set_spacing(5);
    m_main_box.append(m_scramble_selector);
    m_main_box.append(m_scramble_label);
    m_main_box.append(m_inspection_label);
    m_main_box.append(m_timer_label);
    m_main_box.append(m_avg5_label);
    m_main_box.append(m_avg12_label);
    m_main_box.append(m_theme_box);
    m_main_box.append(m_reset_button);
    m_main_box.append(m_reset_session_button);
    m_main_box.append(m_history_list);

    m_reset_button.set_can_focus(false);
    m_scramble_selector.set_can_focus(false);
    m_reset_session_button.set_can_focus(false);
    m_theme_switch.set_can_focus(false);
    m_history_list.set_can_focus(false);
    m_theme_label.set_can_focus(false);


    set_child(m_main_box);
    set_focusable(true);
    grab_focus(); // <- Penting agar window menerima keyboard input


    generate_scramble();
}

StopwatchWindow::~StopwatchWindow() {
    if (m_timer_connection.connected()) m_timer_connection.disconnect();
}

bool StopwatchWindow::on_key_pressed(guint keyval, guint, Gdk::ModifierType) {
    if (keyval == GDK_KEY_space) {
        if (!m_running && !m_in_inspection) {
            m_in_inspection = true;
            m_inspection_start = std::chrono::steady_clock::now();
            m_elapsed = std::chrono::milliseconds(0);
            m_timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &StopwatchWindow::update_timer_label), 100);
        } else if (m_in_inspection && !m_running) {
            m_in_inspection = false;
            m_running = true;
            m_start_time = std::chrono::steady_clock::now();
            m_inspection_label.set_text("");
        } else if (m_running) {
            m_running = false;
            auto now = std::chrono::steady_clock::now();
            m_elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time);
            m_solve_times.push_back(m_elapsed);
            m_scramble_history.push_back(m_current_scramble);
            update_history();
        }
        return true;
    }
    return false;
}

void StopwatchWindow::on_reset_button_clicked() {
    m_running = false;
    m_in_inspection = false;
    m_elapsed = std::chrono::milliseconds(0);
    m_timer_label.set_text("00:00.000");
    m_inspection_label.set_text("");
    generate_scramble();
}

void StopwatchWindow::on_reset_session_clicked() {
    m_solve_times.clear();
    m_scramble_history.clear();
    m_history_list.remove_all();
    m_avg5_label.set_text("Ao5: -");
    m_avg12_label.set_text("Ao12: -");
}

void StopwatchWindow::on_scramble_selector_changed() {
    m_selected_cube_type = static_cast<CubeType>(m_scramble_selector.get_active_row_number());
    generate_scramble();
}

bool StopwatchWindow::update_timer_label() {
    if (m_in_inspection) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_inspection_start);
        double remaining = 15000 - elapsed.count();
        if (remaining <= 0) {
            m_in_inspection = false;
            m_running = true;
            m_start_time = std::chrono::steady_clock::now();
            m_inspection_label.set_text("");
        } else {
            std::ostringstream oss;
            oss << "Inspection: " << std::fixed << std::setprecision(1) << remaining / 1000.0;
            m_inspection_label.set_text(oss.str());
        }
        return true;
    }

    if (m_running) {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - m_start_time) + m_elapsed;
        m_timer_label.set_text(format_time(diff));
    }
    return true;
}

bool StopwatchWindow::on_theme_switch_toggled(bool state) {
    auto settings = Gtk::Settings::get_default();
    if (settings) settings->property_gtk_application_prefer_dark_theme() = state;
    return true;
}

int get_axis(const std::string& move) {
    switch (move[0]) {
        case 'U': case 'D': return 0;
        case 'L': case 'R': return 1;
        case 'F': case 'B': return 2;
    }
    return -1;
}

void StopwatchWindow::generate_scramble() {
    std::vector<std::string> moves = {
        "U", "U'", "U2", "D", "D'", "D2",
        "L", "L'", "L2", "R", "R'", "R2",
        "F", "F'", "F2", "B", "B'", "B2"
    };

    if (m_selected_cube_type == CubeType::FourByFour || m_selected_cube_type == CubeType::FiveByFive) {
        std::vector<std::string> big_moves = {
            "Uw", "Uw'", "Uw2", "Dw", "Dw'", "Dw2",
            "Lw", "Lw'", "Lw2", "Rw", "Rw'", "Rw2",
            "Fw", "Fw'", "Fw2", "Bw", "Bw'", "Bw2"
        };
        moves.insert(moves.end(), big_moves.begin(), big_moves.end());
    }

    int move_count = 20;
    if (m_selected_cube_type == CubeType::TwoByTwo) move_count = 11;
    else if (m_selected_cube_type == CubeType::FourByFour) move_count = 40;
    else if (m_selected_cube_type == CubeType::FiveByFive) move_count = 60;

    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector<std::string> scramble;
    std::string last_move;
    int last_axis = -1;

    for (int i = 0; i < move_count; ++i) {
        std::string move;
        int axis;
        do {
            move = moves[rand() % moves.size()];
            axis = get_axis(move);
        } while (!last_move.empty() && axis == last_axis);
        scramble.push_back(move);
        last_move = move;
        last_axis = axis;
    }

    std::ostringstream oss;
    for (const auto& m : scramble) oss << m << " ";
    m_current_scramble = oss.str();
    m_scramble_label.set_text("Scramble: " + m_current_scramble);
}

void StopwatchWindow::update_history() {
    m_history_list.remove_all();
    for (size_t i = 0; i < m_solve_times.size(); ++i) {
        std::ostringstream oss;
        oss << format_time(m_solve_times[i]) << " - " << m_scramble_history[i];
        auto* label = Gtk::make_managed<Gtk::Label>(oss.str());
        label->set_wrap(true);
        label->set_max_width_chars(50);
        m_history_list.append(*label);
    }
    m_avg5_label.set_text("Ao5: " + compute_average(5));
    m_avg12_label.set_text("Ao12: " + compute_average(12));
}

std::string StopwatchWindow::format_time(std::chrono::milliseconds ms) {
    int minutes = ms.count() / 60000;
    int seconds = (ms.count() % 60000) / 1000;
    int millis = ms.count() % 1000;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds << "."
        << std::setw(3) << std::setfill('0') << millis;
    return oss.str();
}

std::string StopwatchWindow::compute_average(int count) {
    if (m_solve_times.size() < static_cast<size_t>(count)) return "-";
    std::vector<std::chrono::milliseconds> recent(m_solve_times.end() - count, m_solve_times.end());
    std::sort(recent.begin(), recent.end());
    auto total = std::chrono::milliseconds(0);
    int drop = (count >= 5) ? 1 : 0;
    for (int i = drop; i < count - drop; ++i) total += recent[i];
    return format_time(total / (count - 2 * drop));
}

#include "StopwatchWindow.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <gdk/gdkkeysyms.h>

StopwatchWindow::StopwatchWindow()
    : m_reset_button("Reset"), m_running(false) {

    set_title("Rubik's Timer");
    set_default_size(400, 300);

    // Key event controller
    m_key_controller = Gtk::EventControllerKey::create();
    add_controller(m_key_controller);
    m_key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &StopwatchWindow::on_key_pressed), false);

    // Setup reset button
    m_reset_button.signal_clicked().connect(sigc::mem_fun(*this, &StopwatchWindow::on_reset_button_clicked));
    m_reset_button.set_can_focus(false);
    m_reset_button.set_receives_default(false);

    // Setup scramble selector
    m_scramble_selector.append("2x2");
    m_scramble_selector.append("3x3");
    m_scramble_selector.append("4x4");
    m_scramble_selector.append("5x5");
    m_scramble_selector.set_active(1); // Default 3x3
    m_scramble_selector.signal_changed().connect(sigc::mem_fun(*this, &StopwatchWindow::on_scramble_selector_changed));
    m_scramble_selector.set_can_focus(false);

    // Layout
    m_main_box.set_margin(10);
    m_main_box.set_spacing(5);
    m_main_box.append(m_scramble_selector);
    m_main_box.append(m_scramble_label);
    m_main_box.append(m_timer_label);
    m_main_box.append(m_reset_button);

    m_scramble_label.set_text("Scramble:");
    m_timer_label.set_text("00:00.000");

    set_child(m_main_box);

    generate_scramble();
}

StopwatchWindow::~StopwatchWindow() {
    if (m_timer_connection.connected()) {
        m_timer_connection.disconnect();
    }
}

void StopwatchWindow::generate_scramble() {
    static const char* moves3x3[] = { "U", "U'", "U2", "D", "D'", "D2",
                                      "L", "L'", "L2", "R", "R'", "R2",
                                      "F", "F'", "F2", "B", "B'", "B2" };
    static const char* moves2x2[] = { "U", "U'", "U2", "D", "D'", "D2",
                                      "L", "L'", "L2", "R", "R'", "R2",
                                      "F", "F'", "F2", "B", "B'", "B2" };
    static const char* moves4x4[] = { "U", "U'", "U2", "Uw", "Uw'", "Uw2",
                                      "D", "D'", "D2", "Dw", "Dw'", "Dw2",
                                      "L", "L'", "L2", "Lw", "Lw'", "Lw2",
                                      "R", "R'", "R2", "Rw", "Rw'", "Rw2",
                                      "F", "F'", "F2", "Fw", "Fw'", "Fw2",
                                      "B", "B'", "B2", "Bw", "Bw'", "Bw2" };
    static const char* moves5x5[] = { "U", "U'", "U2", "Uw", "Uw'", "Uw2",
                                      "D", "D'", "D2", "Dw", "Dw'", "Dw2",
                                      "L", "L'", "L2", "Lw", "Lw'", "Lw2",
                                      "R", "R'", "R2", "Rw", "Rw'", "Rw2",
                                      "F", "F'", "F2", "Fw", "Fw'", "Fw2",
                                      "B", "B'", "B2", "Bw", "Bw'", "Bw2" };

    const char** moves = nullptr;
    int num_moves = 0;
    int scramble_length = 0;

    switch (m_selected_cube_type) {
    case CubeType::TwoByTwo:
        moves = moves2x2;
        num_moves = sizeof(moves2x2) / sizeof(moves2x2[0]);
        scramble_length = 11;
        break;
    case CubeType::ThreeByThree:
        moves = moves3x3;
        num_moves = sizeof(moves3x3) / sizeof(moves3x3[0]);
        scramble_length = 20;
        break;
    case CubeType::FourByFour:
        moves = moves4x4;
        num_moves = sizeof(moves4x4) / sizeof(moves4x4[0]);
        scramble_length = 40;
        break;
    case CubeType::FiveByFive:
        moves = moves5x5;
        num_moves = sizeof(moves5x5) / sizeof(moves5x5[0]);
        scramble_length = 60;
        break;
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector<std::string> scramble;
    int last_face = -1;

    for (int i = 0; i < scramble_length; ++i) {
        int move_index;
        int face;
        do {
            move_index = rand() % num_moves;
            std::string move = moves[move_index];
            if (move[0] == 'U' || move[0] == 'D') face = 0;
            else if (move[0] == 'L' || move[0] == 'R') face = 1;
            else face = 2;
        } while (face == last_face);

        scramble.push_back(moves[move_index]);
        last_face = face;
    }

    std::ostringstream oss;
    oss << "Scramble: ";
    for (const auto& move : scramble) {
        oss << move << " ";
    }
    m_scramble_label.set_text(oss.str());
}

bool StopwatchWindow::on_key_pressed(guint keyval, guint, Gdk::ModifierType) {
    if (keyval == GDK_KEY_space) {
        if (!m_running) {
            m_running = true;
            m_start_time = std::chrono::steady_clock::now();
            if (!m_timer_connection.connected()) {
                m_timer_connection = Glib::signal_timeout().connect(
                    sigc::mem_fun(*this, &StopwatchWindow::update_timer_label), 10);
            }
        } else {
            m_running = false;
            auto now = std::chrono::steady_clock::now();
            m_elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time);
        }
        return true; // consume event
    }
    return false;
}

void StopwatchWindow::on_reset_button_clicked() {
    m_running = false;
    m_elapsed = std::chrono::milliseconds(0);
    m_timer_label.set_text("00:00.000");
    generate_scramble();
}

void StopwatchWindow::on_scramble_selector_changed() {
    int active = m_scramble_selector.get_active_row_number();
    switch (active) {
    case 0:
        m_selected_cube_type = CubeType::TwoByTwo;
        break;
    case 1:
        m_selected_cube_type = CubeType::ThreeByThree;
        break;
    case 2:
        m_selected_cube_type = CubeType::FourByFour;
        break;
    case 3:
        m_selected_cube_type = CubeType::FiveByFive;
        break;
    default:
        m_selected_cube_type = CubeType::ThreeByThree;
        break;
    }
    generate_scramble();
}

bool StopwatchWindow::update_timer_label() {
    if (!m_running) {
        return true;
    }

    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time) + m_elapsed;

    int minutes = static_cast<int>(diff.count() / 60000);
    int seconds = static_cast<int>((diff.count() % 60000) / 1000) % 60;
    int milliseconds = static_cast<int>(diff.count() % 1000);

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds << "."
        << std::setw(3) << std::setfill('0') << milliseconds;

    m_timer_label.set_text(oss.str());

    return true;
}

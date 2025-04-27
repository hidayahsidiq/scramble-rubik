#include "StopwatchWindow.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <gdk/gdkkeysyms.h>

StopwatchWindow::StopwatchWindow()
    : m_reset_button("Reset"), m_running(false) {

    set_title("Rubik's Timer");
    set_default_size(400, 250);

    // Key event controller
    m_key_controller = Gtk::EventControllerKey::create();
    add_controller(m_key_controller);
    m_key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &StopwatchWindow::on_key_pressed), false);

    // Setup Reset Button
    m_reset_button.signal_clicked().connect(sigc::mem_fun(*this, &StopwatchWindow::on_reset_button_clicked));
    m_reset_button.set_can_focus(false); // <<<<<< ini kunci penting: tombol reset tidak fokus
    m_reset_button.set_receives_default(false);

    // Layout
    m_main_box.set_margin(10);
    m_main_box.set_spacing(5);
    m_main_box.append(m_scramble_label);
    m_main_box.append(m_timer_label);
    m_main_box.append(m_reset_button);

    m_scramble_label.set_text("Scramble: ");
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
    static const char* moves[] = { "U", "U'", "U2", "D", "D'", "D2", "L", "L'", "L2",
                                   "R", "R'", "R2", "F", "F'", "F2", "B", "B'", "B2" };
    static const int num_moves = sizeof(moves) / sizeof(moves[0]);

    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector<std::string> scramble;
    int last_face = -1;
    int last_axis = -1;

    auto get_axis = [](int move_index) {
        if (move_index / 3 == 0 || move_index / 3 == 1) return 0; // U, D -> Axis 0
        if (move_index / 3 == 2 || move_index / 3 == 3) return 1; // L, R -> Axis 1
        return 2; // F, B -> Axis 2
    };

    for (int i = 0; i < 20; ++i) {
        int move_index;
        int face;
        int axis;
        do {
            move_index = rand() % num_moves;
            face = move_index / 3;
            axis = get_axis(move_index);
        } while (face == last_face || axis == last_axis);

        scramble.push_back(moves[move_index]);
        last_face = face;
        last_axis = axis;
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
            m_timer_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &StopwatchWindow::update_timer_label), 10);
        } else {
            m_running = false;
            auto now = std::chrono::steady_clock::now();
            m_elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time);

            if (m_timer_connection.connected()) {
                m_timer_connection.disconnect();
            }
        }
        return true; // consume event
    }
    return false;
}

void StopwatchWindow::on_reset_button_clicked() {
    if (m_timer_connection.connected()) {
        m_timer_connection.disconnect();
    }
    m_running = false;
    m_elapsed = std::chrono::milliseconds(0);
    m_timer_label.set_text("00:00.000");
    generate_scramble();
}

bool StopwatchWindow::update_timer_label() {
    if (!m_running) return false;

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

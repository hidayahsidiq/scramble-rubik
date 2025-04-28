#ifndef STOPWATCHWINDOW_H
#define STOPWATCHWINDOW_H

#include <gtkmm.h>
#include <chrono>
#include <vector>

class StopwatchWindow : public Gtk::Window {
public:
    StopwatchWindow();
    virtual ~StopwatchWindow();

protected:
    // Signal Handlers
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_reset_button_clicked();
    void on_scramble_selector_changed();
    bool update_timer_label();

    // Helper
    void generate_scramble();

private:
    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL};
    Gtk::Label m_scramble_label;
    Gtk::Label m_timer_label;
    Gtk::Button m_reset_button;
    Gtk::ComboBoxText m_scramble_selector;

    Glib::RefPtr<Gtk::EventControllerKey> m_key_controller;
    sigc::connection m_timer_connection;

    bool m_running;
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::milliseconds m_elapsed{0};

    enum class CubeType { TwoByTwo, ThreeByThree, FourByFour, FiveByFive };
    CubeType m_selected_cube_type = CubeType::ThreeByThree;
};

#endif // STOPWATCHWINDOW_H

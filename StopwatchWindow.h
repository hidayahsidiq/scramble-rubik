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
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_reset_button_clicked();
    void on_reset_session_clicked();
    void on_scramble_selector_changed();
    bool update_timer_label();
    bool on_theme_switch_toggled(bool state);

    void generate_scramble();
    void update_history();
    std::string format_time(std::chrono::milliseconds ms);
    std::string compute_average(int count);

private:
    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL};
    Gtk::Box m_theme_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Label m_scramble_label;
    Gtk::Label m_timer_label;
    Gtk::Label m_avg5_label;
    Gtk::Label m_avg12_label;
    Gtk::Label m_theme_label{"Dark Mode"};
    Gtk::Label m_inspection_label;
    Gtk::Button m_reset_button;
    Gtk::Button m_reset_session_button;
    Gtk::ComboBoxText m_scramble_selector;
    Gtk::ListBox m_history_list;
    Gtk::Switch m_theme_switch;

    Glib::RefPtr<Gtk::EventControllerKey> m_key_controller;
    sigc::connection m_timer_connection;

    bool m_running = false;
    bool m_in_inspection = false;
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::steady_clock::time_point m_inspection_start;
    std::chrono::milliseconds m_elapsed{0};

    std::vector<std::chrono::milliseconds> m_solve_times;
    std::vector<std::string> m_scramble_history;
    std::string m_current_scramble;

    enum class CubeType { TwoByTwo = 0, ThreeByThree, FourByFour, FiveByFive };
    CubeType m_selected_cube_type = CubeType::ThreeByThree;
};

#endif // STOPWATCHWINDOW_H

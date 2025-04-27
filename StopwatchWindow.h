#pragma once

#include <gtkmm.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <string>

class StopwatchWindow : public Gtk::Window {
public:
    StopwatchWindow();
    ~StopwatchWindow();

protected:
    void generate_scramble();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_reset_button_clicked();
    bool update_timer_label();

private:
    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL};
    Gtk::Label m_scramble_label;
    Gtk::Label m_timer_label;
    Gtk::Button m_reset_button;
    Glib::RefPtr<Gtk::EventControllerKey> m_key_controller;

    std::atomic<bool> m_running;
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::milliseconds m_elapsed{0};

    sigc::connection m_timer_connection;
};

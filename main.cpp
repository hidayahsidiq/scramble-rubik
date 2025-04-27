#include "StopwatchWindow.h"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.rubik.timer");
    return app->make_window_and_run<StopwatchWindow>(argc, argv);
}

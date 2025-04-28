#include "StopwatchWindow.h"
#include <gtkmm/application.h>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.example.rubikstimer");

    StopwatchWindow window;
    return app->make_window_and_run<StopwatchWindow>(argc, argv);
}

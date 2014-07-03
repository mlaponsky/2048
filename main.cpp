#include <gtkmm.h>

// #include <iostream>
// #include "gui.hpp"
//#include "game.hpp"

int main(int argc, char *argv[])
{
  Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");

  Gtk::ApplicationWindow window;

  return app->run(window);
}

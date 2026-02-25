#include "LoginForm.hpp"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

LoginForm::LoginForm() : screen(ftxui::ScreenInteractive::Fullscreen()) {
   host_input = ftxui::Input(&connection_data.host, "localhost");
   port_input = ftxui::Input(&connection_data.port, "5432");
   db_input = ftxui::Input(&connection_data.database, "");
   user_input = ftxui::Input(&connection_data.username, "");
   password_input = ftxui::Input(&connection_data.password, "");
   db_type_radio = ftxui::Radiobox(&db_types, &db_type_selected);

   connect_button = ftxui::Button("Connect", [this] {
      connection_data.db_type = static_cast<enum_db_type>(db_type_selected);
      screen.Exit();
   });
}

void LoginForm::Start_Form() { screen.Loop(InitForm()); }

ftxui::Component LoginForm::InitForm() {
   using namespace ftxui;

   auto container = Container::Vertical(
       {host_input, port_input, db_input, user_input, password_input, db_type_radio, connect_button});

   auto renderer = Renderer(container, [this] {
      // clang-format off
      return vbox({text(" Connect to the database ") | bold | color(Color::Cyan) | center,
                   separator(),
                   hbox(text("Host:     "), host_input->Render()),
                   hbox(text("Port:     "), port_input->Render()),
                   hbox(text("Base:     "), db_input->Render()),
                   hbox(text("User:     "), user_input->Render()),
                   hbox(text("Password: "), password_input->Render()),
                   separator(),
                   text("Database type:"),
                   db_type_radio->Render(),
                   separator(),
                   connect_button->Render() | center})
             | border | size(WIDTH, LESS_THAN, 80);
   });
   return renderer;
   // clang-format on
}
LoginForm::ConnectionData LoginForm::GetConnectionParams() const { return this->connection_data; }

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

  connect_button = ftxui::Button("Подключиться", [this] {
    connection_data.db_type = static_cast<enum_db_type>(db_type_selected);
    screen.Exit();
  });
}

void LoginForm::RUN() { screen.Loop(CreateConnectionForm()); }

ftxui::Component LoginForm::CreateConnectionForm() {
  using namespace ftxui;

  auto container =
      Container::Vertical({host_input, port_input, db_input, user_input,
                           password_input, db_type_radio, connect_button});

  auto renderer = Renderer(container, [this] {
    // clang-format off
      return vbox({text(" Подключение к базе данных ") | bold | color(Color::Cyan) | center,
                   separator(),
                   hbox(text("Хост:     "), host_input->Render()),
                   hbox(text("Порт:     "), port_input->Render()),
                   hbox(text("База:     "), db_input->Render()),
                   hbox(text("Пользователь: "), user_input->Render()),
                   hbox(text("Пароль:   "), password_input->Render()),
                   separator(),
                   text("Тип базы данных:"),
                   db_type_radio->Render(),
                   separator(),
                   connect_button->Render() | center})
             | border | size(WIDTH, LESS_THAN, 80);
   });
   return renderer;
  // clang-format on
}
LoginForm::ConnectionData LoginForm::GetConnectionParams() const {
  return this->connection_data;
}

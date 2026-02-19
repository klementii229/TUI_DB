#pragma once
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

class LoginForm {
public:
  LoginForm();
  void RUN();
  ftxui::Component CreateConnectionForm();
  enum class enum_db_type : uint8_t;

  struct ConnectionData {
    std::string host = "localhost";
    std::string port = "5432";
    std::string database;
    std::string username;
    std::string password;
    enum_db_type db_type;
  };
  [[nodiscard]] ConnectionData GetConnectionParams() const;

private:
  ftxui::ScreenInteractive screen;

  ConnectionData connection_data;

  ftxui::Component host_input;
  ftxui::Component port_input;
  ftxui::Component db_input;
  ftxui::Component user_input;
  ftxui::Component password_input;
  ftxui::Component db_type_radio;
  ftxui::Component connect_button;

  int db_type_selected = 0;
  std::vector<std::string> db_types = {"PostgreSQL", "MariaDB", "SQLite"};

public:
  enum class enum_db_type : uint8_t { PostgreSQL, MariaDB, SQLite };
};

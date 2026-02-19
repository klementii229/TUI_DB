#include "DatabaseExplorer.hpp"

#include <cctype>
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <string>

#include "DataBaseInterface.hpp"

static ftxui::ButtonOption CreateRoundedButtonOption();
static void FormatTable(ftxui::Table& table);

DataBaseExplorer::DataBaseExplorer(std::unique_ptr<IDatabaseConnector> conn_)
    : screen(ftxui::ScreenInteractive::Fullscreen()), conn(std::move(conn_)) {
   Ininitalize();
   main_container = ftxui::Container::Vertical({req_input, btn_send_req, table_component, slider_x, slider_y});

   main_window = ftxui::Renderer(main_container, [this] {
      return ftxui::vbox({ftxui::text("T U I D B") | ftxui::center | ftxui::color(ftxui::Color::Cyan) | ftxui::bold,
                          ftxui::separator(),
                          // Верхняя панель - фиксированная высота
                          ftxui::hbox({req_input->Render(),
                                       btn_send_req->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)})
                              | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),
                          ftxui::separator(),
                          table_component->Render(),
                          ftxui::separator(),
                          slider_x->Render(),
                          slider_y->Render()})
             | ftxui::border;
   });
}

void DataBaseExplorer::RUN() { screen.Loop(main_window); }

static ftxui::ButtonOption CreateRoundedButtonOption() {
   ftxui::ButtonOption option;
   option.transform = [](const ftxui::EntryState& s) {
      auto text_element = ftxui::text(s.label) | ftxui::center;
      if (s.focused) text_element |= ftxui::inverted;
      if (s.active) text_element |= ftxui::bold;
      return ftxui::borderRounded(ftxui::hbox({ftxui::filler(), text_element, ftxui::filler()}));
   };
   return option;
}

void DataBaseExplorer::Ininitalize() {
   req_input = ftxui::Input(&req_text, "Enter SQL request") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::flex
               | ftxui::border;

   table_component = ftxui::Renderer([this] {
      if (db_result.empty()) {
         return ftxui::text("No data") | ftxui::center | ftxui::flex;
      }

      auto ftxui_table = ftxui::Table(db_result);
      FormatTable(ftxui_table);
      return ftxui_table.Render() | ftxui::focusPositionRelative(scroll_x, scroll_y) | ftxui::frame
             | ftxui::vscroll_indicator | ftxui::hscroll_indicator | ftxui::flex;
   });

   // 2. Делаем компонент фокусируемым
   auto table_with_scroll = CatchEvent(table_component, [&](ftxui::Event event) {
      if (event == ftxui::Event::ArrowRight) {
         scroll_x = std::min(scroll_x + 0.05f, 1.0f);
         return true;
      }
      if (event == ftxui::Event::ArrowLeft) {
         scroll_x = std::max(scroll_x - 0.05f, 0.0f);
         return true;
      }
      return false;
   });
   slider_x = ftxui::Slider("Horizontal", &scroll_x, 0.0f, 1.0f, 0.01f);
   slider_y = ftxui::Slider("Vertical  ", &scroll_y, 0.0f, 1.0f, 0.01f);

   btn_send_req = ftxui::Button(
       "Enter",
       [this] {
          db_result.clear();
          std::string copy{req_text};
          for (char& x : copy) {
             x = (char)toupper(int(x));
          }

          if (copy.find("SELECT") != std::string::npos) {
             auto res = conn->FetchAll(req_text);
             if (res.has_value()) {
                db_result = std::move(res.value());
             } else {
                db_result = {{res.error()}};
             }
          } else {
             auto res = conn->ExecuteUpdate(req_text);
             if (res.has_value()) {
                db_result = {{std::format("Success, rows affected: {}", res.value())}};
             } else {
                db_result = {{res.error()}};
             }
          }
          // Принудительно обновляем экран
          screen.Post(ftxui::Event::Custom);
       },
       CreateRoundedButtonOption());
}
static void FormatTable(ftxui::Table& table) {
   table.SelectAll().Decorate([](ftxui::Element e) { return e | ftxui::center; });
}

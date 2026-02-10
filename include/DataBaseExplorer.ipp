#include <DataBaseInterface.hpp>
#include <cstddef>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <string>

#include "DataBaseExplorer.hpp"

ftxui::ButtonOption CreateRoundedButtonOption();
std::vector<ftxui::Elements> FormatTable(const Table& table, int current_page, const int rows_per_page = 500);

template <DatabaseConnection Connector>
DataBaseExplorer<Connector>::DataBaseExplorer(std::unique_ptr<Connector> conn_)
    : screen(ftxui::ScreenInteractive::Fullscreen()), conn(std::move(conn_)) {
   Ininitalize();
   main_container = ftxui::Container::Vertical({req_input, btn_send_req, table_component, slider_x, slider_y});

   main_window = ftxui::Renderer(main_container, [this] {
      return ftxui::vbox({ftxui::text("T U I D B") | ftxui::center | ftxui::color(ftxui::Color::Cyan) | ftxui::bold,
                          ftxui::separator(),
                          ftxui::hbox({req_input->Render(),
                                       btn_send_req->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)}) |
                              ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3),
                          ftxui::separator(), table_component->Render(), ftxui::separator(), slider_x->Render(),
                          slider_y->Render()}) |
             ftxui::border;
   });

   main_window = main_window | ftxui::CatchEvent([this](ftxui::Event event) {
                    if (event == ftxui::Event::PageUp) {
                       current_page--;
                       if (current_page < 0) current_page = 0;
                       screen.PostEvent(ftxui::Event::Custom);
                       return true;
                    }

                    if (event == ftxui::Event::PageDown) {
                       current_page++;
                       if (current_page > db_result.size() / rows_per_page) {
                          current_page--;
                       }
                       screen.PostEvent(ftxui::Event::Custom);
                       return true;
                    }

                    return false;
                 });
}

template <DatabaseConnection Connector>
void DataBaseExplorer<Connector>::RUN() {
   screen.Loop(main_window);
}

ftxui::ButtonOption CreateRoundedButtonOption() {
   ftxui::ButtonOption option;
   option.transform = [](const ftxui::EntryState& s) {
      auto text_element = ftxui::text(s.label) | ftxui::center;
      if (s.focused) text_element |= ftxui::inverted;
      if (s.active) text_element |= ftxui::bold;
      return ftxui::borderRounded(ftxui::hbox({ftxui::filler(), text_element, ftxui::filler()}));
   };
   return option;
}
template <DatabaseConnection Connector>
void DataBaseExplorer<Connector>::Ininitalize() {
   req_input = ftxui::Input(&req_text, "Enter SQL request") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) |
               ftxui::flex | ftxui::border;

   table_component = ftxui::Renderer([this] {
      if (db_result.empty()) {
         return ftxui::text("No data") | ftxui::center | ftxui::flex;
      }

      if (pages.size() == 0 || pages.size() - 1 <= current_page) {
         pages.push_back(FormatTable(db_result, current_page, rows_per_page));
      }

      auto ftxui_table = ftxui::Table(pages[current_page]);

      return ftxui_table.Render() | ftxui::focusPositionRelative(scroll_x, scroll_y) | ftxui::frame |
             ftxui::vscroll_indicator | ftxui::hscroll_indicator | ftxui::flex;
   });

   slider_x = ftxui::Slider("Horizontal", &scroll_x, 0.0f, 1.0f, 0.01f);
   slider_y = ftxui::Slider("Vertical  ", &scroll_y, 0.0f, 1.0f, 0.01f);

   btn_send_req = ftxui::Button(
       "Enter",
       [this] {
          current_page = 0;
          db_result.clear();
          pages.clear();
          std::string copy{req_text};
          for (char& x : copy) {
             x = (char)toupper(int(x));
          }

          if (copy.find("SELECT") != std::string::npos) {
             auto res = conn->FetchAll(req_text);
             if (res.has_value()) {
                db_result = std::move(res.value());
                pages.reserve(db_result.size() / rows_per_page);
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
          screen.Post(ftxui::Event::Custom);
       },
       CreateRoundedButtonOption());
}
template <DatabaseConnection Connector>
std::vector<ftxui::Elements> FormatTable(const Table& table, int current_page, const int rows_per_page) {
   using namespace ftxui;
   std::vector<Elements> out{};
   size_t start_index = rows_per_page * current_page;
   size_t end_index = start_index + rows_per_page + 1;  //+1 for column name
   if (start_index + rows_per_page > table.size()) {
      end_index = table.size();
   }
   out.reserve(rows_per_page + 1);

   if (current_page != 0) {
      out.emplace_back();
      auto& row = out.back();
      row.reserve(table[0].size() * 2);
      for (size_t i = 0; i < table[0].size(); i++) {
         if (table[0][i] == "NULL") {
            row.push_back(text(table[0][i]) | color(Color::Blue));
            row.push_back(separator());
         } else {
            row.push_back(text(table[0][i]));
            if (i != table[0].size() - 1) {
               row.push_back(separator());
            }
         }
      }
   }

   for (size_t i = start_index; i < end_index; i++) {
      out.emplace_back();
      auto& out_row = out.back();
      out_row.reserve(table[i].size() * 2);
      for (size_t j = 0; j < table[i].size(); j++) {
         if (table[i][j] == "NULL") {
            out_row.push_back(text(table[i][j]) | color(Color::Blue));
            out_row.push_back(separator());
         } else {
            out_row.push_back(text(table[i][j]));
            if (j != table[i].size() - 1) {
               out_row.push_back(separator());
            }
         }
      }
   }

   return out;
}

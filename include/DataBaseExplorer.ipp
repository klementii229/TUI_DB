#pragma once
#include <expected>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "DataBaseExplorer.hpp"
#include "DataBaseInterface.hpp"

auto CreateRoundedButtonOption() -> ftxui::ButtonOption;
auto FormatTable(const Table& table, size_t current_page, const size_t rows_per_page) -> std::vector<ftxui::Elements>;

template <DatabaseConnection Connector>
DataBaseExplorer<Connector>::DataBaseExplorer(std::unique_ptr<Connector> conn_)
    : screen(ftxui::ScreenInteractive::Fullscreen()), conn(std::move(conn_)) {
   Ininitalize();

   main_container = ftxui::Container::Vertical({header, req_input, btn_send_req, table_component, slider_x, slider_y});
   main_window = ftxui::Renderer(main_container, [this] {
      return ftxui::vbox({header->Render(),
                          ftxui::separator(),
                          table_component->Render(),
                          ftxui::separator(),
                          slider_x->Render(),
                          slider_y->Render()})
             | ftxui::border;
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

auto CreateRoundedButtonOption() -> ftxui::ButtonOption {
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
   req_input = ftxui::Input(&req_text, "Enter SQL request") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::flex
               | ftxui::border;
   btn_send_req = ftxui::Button(
       "Enter",
       [this] {
          current_page = 0;
          db_result.clear();
          pages.clear();
          max_page = 0;

          std::string copy{req_text};
          for (char& x : copy) {
             x = (char)toupper(int(x));
          }

          auto process_res = [this](std::expected<Table, DbStatus>& res) {
             if (res.has_value()) {
                error_message = "";  // Ошибки нет
                db_result = std::move(res.value());
                max_page = db_result.size() / rows_per_page;
                pages.reserve(max_page);
             } else {
                db_result.clear();
                error_message = res.error().details;  // Сохраняем текст ошибки
             }
          };

          if (copy.find("SELECT") != std::string::npos) {
             auto res = conn->FetchAll(req_text);
             process_res(res);
          } else {
             std::string cp_req = {req_text};
             std::size_t idx = cp_req.rfind(";");
             if (idx != std::string::npos) {
                cp_req.erase(idx);
             }
             auto res = conn->FetchAll(cp_req + " returning *;");
             process_res(res);
          }
          screen.Post(ftxui::Event::Custom);
       },
       CreateRoundedButtonOption());

   header = ftxui::Renderer([this] {
      // clang-format off
      return ftxui::vbox({
         ftxui::text("T U I D B")
            | ftxui::center
            | ftxui::bold
            | ftxui::color(ftxui::Color::Cyan),
              ftxui::separator(),
              ftxui::hbox({
            req_input->Render() | ftxui::flex,
            btn_send_req->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20)
         })
      });
      // clang-format on
   });

   table_component = ftxui::Renderer([this] {
      if (!error_message.empty()) {
         return ftxui::paragraph(error_message) | ftxui::color(ftxui::Color::Red);
      }

      else if (db_result.empty() || db_result.size() <= 1) {
         return ftxui::text("No data") | ftxui::center | ftxui::flex;
      }

      if (pages.size() == 0 || (int)pages.size() - 1 <= current_page) {
         pages.push_back(FormatTable(db_result, current_page, rows_per_page));
      }

      auto ftxui_table = ftxui::Table(pages[current_page]);

      // clang-format off
      return ftxui_table.Render()
             | ftxui::focusPositionRelative(scroll_x, scroll_y)
             | ftxui::frame
             | ftxui::vscroll_indicator
             | ftxui::hscroll_indicator
             | ftxui::flex;
      // clang-format on
   });

   slider_x = ftxui::Slider("Horizontal", &scroll_x, 0.0f, 1.0f, 0.01f);
   slider_y = ftxui::Slider("Vertical  ", &scroll_y, 0.0f, 1.0f, 0.01f);
}

template <DatabaseConnection Connector>
std::vector<ftxui::Elements> DataBaseExplorer<Connector>::FormatTable(const Table& table,
                                                                      size_t current_page,
                                                                      const size_t rows_per_page) {
   using namespace ftxui;
   std::vector<Elements> out{};

   size_t start_index = rows_per_page * current_page;
   size_t end_index = start_index + rows_per_page + 1;
   if (start_index + rows_per_page > table.size()) {
      end_index = table.size();
   }

   out.reserve(rows_per_page + 1);

   auto process_row = [&](size_t row_idx) {
      Elements row_elements;
      row_elements.reserve(table[row_idx].size() * 2);
      for (size_t i = 0; i < table[row_idx].size(); i++) {
         if (table[row_idx][i] == "NULL") {
            row_elements.push_back(text("NULL") | color(Color::Blue));
         } else {
            row_elements.push_back(text(table[row_idx][i]));
         }

         if (i != table[row_idx].size() - 1) {
            row_elements.push_back(separator());
         }
      }
      return row_elements;
   };

   if (current_page != 0 && !table.empty()) {
      out.push_back(process_row(0));  // Header row
   }

   for (size_t i = start_index; i < end_index; i++) {
      out.push_back(process_row(i));
   }

   return out;
}

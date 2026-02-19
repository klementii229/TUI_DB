#pragma once
#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "DataBaseExplorer.hpp"
#include "DataBaseInterface.hpp"

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
DataBaseExplorer<Connector>::DataBaseExplorer(std::unique_ptr<Connector> conn_)
    : screen(ftxui::ScreenInteractive::Fullscreen()), conn(std::move(conn_)) {
   Ininitalize();
   // clang-format off
   main_container = ftxui::Container::Vertical({
      header,
      req_input,
      btn_send_req,
      table_component,
      slider_x,
      slider_y
   });

   main_window = ftxui::Renderer(main_container, [this] {
      return ftxui::vbox({
         header->Render(),
         ftxui::separator(),
         table_component->Render(),
         ftxui::separator(),
         slider_x->Render(),
         slider_y->Render()
      }) | ftxui::border;
   });
   // clang-format on
   // Пагинация
   main_window = main_window | ftxui::CatchEvent([this](ftxui::Event event) {
                    if (event == ftxui::Event::PageUp) {
                       if (current_page > 0) {
                          current_page--;
                          table_needs_rebuild = true;
                       }
                       return true;
                    }
                    if (event == ftxui::Event::PageDown) {
                       if (current_page < max_page) {
                          current_page++;
                          table_needs_rebuild = true;
                       }
                       return true;
                    }
                    return false;
                 });
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
          table_needs_rebuild = true;
          std::string copy = req_text;

          std::ranges::transform(
              copy, copy.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

          auto process_res = [this](std::expected<Table, DbError> res) {
             if (res.has_value()) {
                error_message = "";
                db_result = std::move(res.value());
                max_page = db_result.empty() ? 0 : (db_result.size() - 1) / rows_per_page;
             } else {
                db_result.clear();
                error_message = res.error().details;
             }
             table_needs_rebuild = true;
          };

          if (copy.find("SELECT") != std::string::npos) {
             process_res(conn->FetchAll(req_text));
          } else {
             std::string clean_req = req_text;
             if (!clean_req.empty() && clean_req.back() == ';') clean_req.pop_back();
             process_res(conn->FetchAll(clean_req + " RETURNING *;"));
          }
          screen.Post(ftxui::Event::Custom);
       },
       CreateRoundedButtonOption());

   // clang-format off
   header = ftxui::Renderer([this] {
      return ftxui::vbox({
         ftxui::text("T U I D B") | ftxui::center | ftxui::bold | ftxui::color(ftxui::Color::Cyan),
         ftxui::separator(),
         ftxui::hbox({
            req_input->Render() | ftxui::flex,
            btn_send_req->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20)
         })
      });
   });
   // clang-format on
   table_component = ftxui::Renderer([this] {
      if (!error_message.empty()) {
         return ftxui::paragraph(error_message) | ftxui::color(ftxui::Color::Red);
      }
      if (db_result.empty()) {
         return ftxui::text("No data") | ftxui::center | ftxui::flex;
      }

      // вызываем ftxui::Table(...).Render() только один раз
      if (table_needs_rebuild) {
         if (pages.size() <= current_page) {
            pages.push_back(FormatTable(db_result, current_page, rows_per_page));
         }
         // Сохраняем тяжелый объект рендеринга в кэш
         rendered_table_cache = ftxui::Table(pages[current_page]).Render();
         table_needs_rebuild = false;
      }

      // При движении слайдеров scroll_x/y используется уже готовый rendered_table_cache
      return rendered_table_cache | ftxui::focusPositionRelative(scroll_x, scroll_y) | ftxui::frame
             | ftxui::vscroll_indicator | ftxui::hscroll_indicator | ftxui::flex;
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
   // Проверка границ, чтобы не выйти за пределы вектора
   size_t end_index = std::min(start_index + rows_per_page, table.size() - 1);

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
         if (i != table[row_idx].size() - 1) row_elements.push_back(separator());
      }
      return row_elements;
   };

   // Всегда добавляем заголовок (индекс 0), если мы не на первой странице
   if (current_page != 0 && !table.empty()) {
      out.push_back(process_row(0));
   }

   // Добавляем данные текущей страницы
   for (size_t i = (current_page == 0 ? 0 : start_index + 1); i <= end_index; i++) {
      out.push_back(process_row(i));
   }

   return out;
}

template <DatabaseConnection Connector>
void DataBaseExplorer<Connector>::RUN() {
   screen.Loop(main_window);
}

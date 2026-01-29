#include <memory>
#include <string>
#include <vector>

#include "DatabaseExplorer.hpp"
#include "LoginForm.hpp"
#include "SqliteConnector.hpp"

using Row = std::vector<std::string>;
using Table = std::vector<Row>;

int main(void) {
   /* LoginForm Form = {};

   Form.RUN();
   auto Connect_Params = Form.GetConnectionParams();

   std::unique_ptr<IDatabaseConnector> conn = nullptr;
   switch (Connect_Params.db_type) {
      case LoginForm::enum_db_type::SQLite:
         conn = std::make_unique<SQLiteConnector>();
         break;
      default:
         return 1;
   }
   if (!conn->Connect(Connect_Params.database)) {
      return 1;
      }*/

   std::unique_ptr<IDatabaseConnector> conn = std::make_unique<SQLiteConnector>();
   conn->Connect("chinook.db");
   DataBaseExplorer exp = {std::move(conn)};
   exp.RUN();
   return 0;
}
/*
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <string>
#include <vector>

using namespace ftxui;

int main() {
   std::vector<std::vector<std::string>> data;
   for (int i = 0; i < 200; i++) {
      data.push_back({"Row " + std::to_string(i + 1), std::string(50, 'A') + " " + std::to_string(i),
                      std::string(50, 'B') + " " + std::to_string(i * 10), std::string(50, 'C') + " " + std::to_string(i * 100)});
   }

   // 2. ПЕРЕМЕННЫЕ ДЛЯ СКРОЛЛА
   float scroll_y = 0.0f;
   float scroll_x = 0.0f;

   // 3. КОМПОНЕНТ ТАБЛИЦЫ
   auto table_component = Renderer([&] {
      auto table = Table(data);
      table.SelectAll().Border(LIGHT);
      // ВАЖНО: этот порядок РАБОТАЕТ
      return table.Render() | focusPositionRelative(scroll_x, scroll_y) | frame | vscroll_indicator | hscroll_indicator | flex;
   });

   // 4. ОБРАБОТКА СОБЫТИЙ ДЛЯ СКРОЛЛА
   auto table_with_scroll = CatchEvent(table_component, [&](Event event) {
      if (event == Event::ArrowRight) {
         scroll_x = std::min(scroll_x + 0.05f, 1.0f);
         return true;
      }
      if (event == Event::ArrowLeft) {
         scroll_x = std::max(scroll_x - 0.05f, 0.0f);
         return true;
      }
      return false;
   });

   // 5. СЛАЙДЕРЫ ДЛЯ ВИЗУАЛЬНОГО КОНТРОЛЯ
   auto slider_y = Slider("Vertical", &scroll_y, 0.0f, 1.0f, 0.01f);
   auto slider_x = Slider("Horizontal", &scroll_x, 0.0f, 1.0f, 0.01f);

   // 6. ИНФОРМАЦИОННАЯ ПАНЕЛЬ
   auto info_panel = Renderer([&] {
      return hbox({text("Total rows: " + std::to_string(data.size())), text(" | Scroll Y: " + std::to_string(int(scroll_y * 100)) + "%"),
                   text(" | Scroll X: " + std::to_string(int(scroll_x * 100)) + "%"), text(" | Use ARROWS to scroll") | flex}) |
             color(Color::Yellow) | border;
   });

   // 7. СОБИРАЕМ ИНТЕРФЕЙС
   auto layout = Container::Vertical({info_panel, table_with_scroll, slider_y, slider_x});

   auto final_renderer = Renderer(layout, [&] {
      return vbox({info_panel->Render(), separator(), table_with_scroll->Render(), separator(), slider_y->Render(), slider_x->Render()}) |
             border;
   });

   // 8. ЗАПУСКАЕМ
   auto screen = ScreenInteractive::Fullscreen();

   auto final_component = CatchEvent(final_renderer, [&](Event event) {
      if (event == Event::Character('q')) {
         screen.Exit();
         return true;
      }
      return false;
   });

   screen.Loop(final_component);

   return 0;
}
*/

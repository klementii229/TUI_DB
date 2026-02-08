#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/dom/table.hpp>
#include <memory>
#include <vector>

#include "DataBaseInterface.hpp"
using FormTablElem = std::vector<ftxui::Elements>;

class DataBaseExplorer {
  public:
   void RUN();

   DataBaseExplorer(std::unique_ptr<IDatabaseConnector> conn_);
   DataBaseExplorer() = delete;

  private:
   void Ininitalize();
   std::unique_ptr<IDatabaseConnector> conn;
   ftxui::ScreenInteractive screen;

   ftxui::Component main_window;
   ftxui::Component main_container;

   ftxui::Component req_input;
   std::string req_text;
   ftxui::Component btn_send_req;

   ftxui::Component table_component;
   Table db_result;
   float scroll_x = 0.0f;
   float scroll_y = 0.0f;
   ftxui::Component slider_x;
   ftxui::Component slider_y;
   std::vector<FormTablElem> pages;


   int current_page = 0;
   const int rows_per_page = 300;
};

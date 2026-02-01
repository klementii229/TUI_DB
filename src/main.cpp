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

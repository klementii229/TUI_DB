#include "DataBaseExplorer.hpp"
#include "LoginForm.hpp"
#include "PostgresConnector.hpp"
#include "SqliteConnector.hpp"

int main() {
   LoginForm Form = {};
   Form.RUN();
   auto Params = Form.GetConnectionParams();

   auto start_explorer = [&]<typename T>(T&& connector_impl) {
      using CleanType = std::decay_t<T>;
      auto conn_ptr = std::make_unique<CleanType>(std::forward<T>(connector_impl));
      std::expected<bool, DbStatus> res;

      // Статическая проверка типа во время компиляции
      if constexpr (std::is_same_v<CleanType, PostgresConnector>) {
         // Для Postgres
         std::string conn_str = std::format("host={} port={} user={} password={} dbname={}",
                                            Params.host,
                                            Params.port,
                                            Params.username,
                                            Params.password,
                                            Params.database);
         res = conn_ptr->Connect(conn_str);

      } else if constexpr (std::is_same_v<CleanType, SQLiteConnector>) {
         // Для SQLite передаем только путь/имя
         res = conn_ptr->Connect(Params.database);
      }

      if (res && *res) {
         DataBaseExplorer<CleanType> exp(std::move(conn_ptr));
         exp.RUN();
      }
   };

   if (Params.db_type == LoginForm::enum_db_type::SQLite) {
      start_explorer(SQLiteConnector{});
   } else {
      start_explorer(PostgresConnector{});
   }

   return 0;
}


#include <print>

#include "DataBaseExplorer.hpp"
#include "LoginForm.hpp"
#include "PostgresConnector.hpp"
#include "SqliteConnector.hpp"

std::string make_conn_str(const LoginForm::ConnectionData& d) {
   return std::format(
       "host={} port={} user={} password='{}' dbname={}", d.host, d.port, d.username, d.password, d.database);
}

// Explorer Factory
template <DatabaseConnection Connector>
void start_explorer(LoginForm::ConnectionData& params) {
   auto conn = std::make_unique<Connector>();

   std::optional<DbError> conn_err;

   if constexpr (std::is_same_v<Connector, PostgresConnector>) {
      conn_err = conn->Connect(make_conn_str(params));
   } else if constexpr (std::is_same_v<Connector, SQLiteConnector>) {
      conn_err = conn->Connect(params.database);
   }

   if (!conn_err.has_value()) {
      DataBaseExplorer<Connector> explorer(std::move(conn));
      explorer.Explore();
   } else {
      std::println(stderr, "{}", conn_err.value().details);
   }
}

int main() {
   LoginForm login_form;
   login_form.Start_Form();

   auto params = login_form.GetConnectionParams();

   switch (params.db_type) {
      case LoginForm::enum_db_type::PostgreSQL:
         start_explorer<PostgresConnector>(params);
         break;

      case LoginForm::enum_db_type::SQLite:
         start_explorer<SQLiteConnector>(params);
         break;

      case LoginForm::enum_db_type::MariaDB:
         // start_explorer<MariaDBConnector>(params);
         break;
   }

   return 0;
}

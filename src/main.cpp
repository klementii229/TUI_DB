
#include <print>

#include "DataBaseExplorer.hpp"
#include "LoginForm.hpp"
#include "PostgresConnector.hpp"
#include "SqliteConnector.hpp"

std::string make_conn_str(const LoginForm::ConnectionData& d) {
   return std::format(
       "host={} port={} user={} password={} dbname={}", d.host, d.port, d.username, d.password, d.database);
}

// Explorer Factory
template <DatabaseConnection Connector>
void start_explorer(LoginForm::ConnectionData& params) {
   auto conn = std::make_unique<Connector>();

   std::optional<DbError> connected;

   if constexpr (std::is_same_v<Connector, PostgresConnector>) {
      connected = conn->Connect(make_conn_str(params));
   } else if constexpr (std::is_same_v<Connector, SQLiteConnector>) {
      connected = conn->Connect(params.database);
   }

   if (connected) {
      DataBaseExplorer<Connector> explorer(std::move(conn));
      explorer.RUN();
   } else {
      std::println(stderr, "{}", connected.value().details);
   }
}

int main() {
   LoginForm login_form;
   login_form.RUN();

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

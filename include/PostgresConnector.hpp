#pragma once
#include <memory>
#include <pqxx/pqxx>

#include "DataBaseInterface.hpp"
class PostgresConnector {
  private:
   std::unique_ptr<pqxx::connection> conn;

  public:
   std::expected<bool, DbStatus> Connect(const std::string& connectionString);
   void Disconnect();
   std::expected<Table, DbStatus> FetchAll(const std::string& query);

   PostgresConnector() = default;
   ~PostgresConnector();
   PostgresConnector(PostgresConnector&&) = default;
   PostgresConnector& operator=(PostgresConnector&&) = default;
};

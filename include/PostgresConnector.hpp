#pragma once
#include <memory>
#include <pqxx/pqxx>

#include "DataBaseInterface.hpp"
class PostgresConnector {
private:
  std::unique_ptr<pqxx::connection> conn;

public:
  std::expected<void, DbError> Connect(const std::string &connectionString);
  void Disconnect();
  std::expected<Table, DbError> FetchAll(const std::string &query);

  PostgresConnector() = default;
  ~PostgresConnector();
};

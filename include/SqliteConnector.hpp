#pragma once
#include "DataBaseInterface.hpp"

struct sqlite3;

class SQLiteConnector {
  private:
   sqlite3* db = nullptr;

  public:
   std::expected<bool, DbStatus> Connect(const std::string& connectionString);
   std::expected<Table, DbStatus> FetchAll(const std::string& query);
   void Disconnect();

   std::expected<Row, DbStatus> GetTableList();
   std::expected<Row, DbStatus> GetTableSchema(const std::string& tableName);

   SQLiteConnector() = default;
   ~SQLiteConnector();
};

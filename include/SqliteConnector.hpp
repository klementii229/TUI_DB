#pragma once
#include "DataBaseInterface.hpp"

struct sqlite3;
// TODO сделать что сказа епта бафер и оставить 1 метод для sql запросов
class SQLiteConnector {
  private:
   sqlite3* db = nullptr;

  public:
   std::expected<Table, DbError> FetchAll(const std::string& query);

   std::expected<Row, DbError> GetTableList();
   std::expected<Row, DbError> GetTableSchema(const std::string& tableName);

   SQLiteConnector() = default;
   std::expected<bool, DbError> Connect(const std::string& connectionString);
   ~SQLiteConnector();
   void Disconnect();

   SQLiteConnector(const SQLiteConnector&) = default;
   SQLiteConnector(SQLiteConnector&&) = default;
   SQLiteConnector& operator=(const SQLiteConnector&) = default;
   SQLiteConnector& operator=(SQLiteConnector&&) = default;
};

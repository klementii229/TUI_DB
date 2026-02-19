#pragma once
#include "DataBaseInterface.hpp"

struct sqlite3;

class SQLiteConnector {
  private:
   sqlite3* db = nullptr;

  public:
   std::expected<void, DbError> Connect(const std::string& connectionString);
   std::expected<Table, DbError> FetchAll(const std::string& query);
   void Disconnect();

   SQLiteConnector() = default;
   ~SQLiteConnector();
};

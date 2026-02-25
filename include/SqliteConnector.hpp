#pragma once
#include <memory>
#include <optional>
#include <string>

#include "DataBaseInterface.hpp"

struct sqlite3;

struct SqliteDeleter {
   void operator()(sqlite3* db) const;
};

class SQLiteConnector {
  private:
   std::unique_ptr<sqlite3, SqliteDeleter> db;

  public:
   SQLiteConnector() = default;
   ~SQLiteConnector();

   std::optional<DbError> Connect(const std::string& connectionString);
   std::expected<Table, DbError> FetchAll(const std::string& query);
   void Disconnect();
};

#include "SqliteConnector.hpp"

#include <sqlite3.h>

#include <format>
#include <memory>

#include "DataBaseInterface.hpp"

// RAII
struct SQLiteStmtDeleter {
   void operator()(sqlite3_stmt* stmt) const { sqlite3_finalize(stmt); }
};

using UniqueStmtPtr = std::unique_ptr<sqlite3_stmt, SQLiteStmtDeleter>;

std::expected<bool, DbStatus> SQLiteConnector::Connect(const std::string& connectionString) {
   if (sqlite3_open(connectionString.c_str(), &db) == SQLITE_OK) {
      return true;
   }
   std::string err_details = db ? sqlite3_errmsg(db) : "Failed to allocate memory for SQLite";
   if (db) {
      sqlite3_close(db);
      db = nullptr;
   }
   return std::unexpected(DbStatus{.type = DbError::ConnectionError, .details = err_details});
}

std::expected<Table, DbStatus> SQLiteConnector::FetchAll(const std::string& query) {
   Table result;
   sqlite3_stmt* raw_stmt = nullptr;

   int ret_code = sqlite3_prepare_v2(db, query.c_str(), -1, &raw_stmt, nullptr);
   if (ret_code != SQLITE_OK) {
      return std::unexpected(DbStatus{.type = DbError::InvalidQuery, .details = sqlite3_errmsg(db)});
   }

   UniqueStmtPtr stmt(raw_stmt);
   int column_count = sqlite3_column_count(stmt.get());

   Row column_names;
   column_names.reserve(column_count);
   for (int i = 0; i < column_count; ++i) {
      const char* name = sqlite3_column_name(stmt.get(), i);
      column_names.push_back(name ? name : "NULL");
   }
   result.push_back(std::move(column_names));

   while ((ret_code = sqlite3_step(stmt.get())) == SQLITE_ROW) {
      Row row;
      row.reserve(column_count);
      for (int i = 0; i < column_count; ++i) {
         const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), i));
         row.push_back(text ? text : "NULL");
      }
      result.push_back(std::move(row));
   }

   if (ret_code != SQLITE_DONE) {
      return std::unexpected(DbStatus{.type = DbError::Unknown, .details = sqlite3_errmsg(db)});
   }
   return result;
}

void SQLiteConnector::Disconnect() {
   if (db) {
      sqlite3_close(db);
      db = nullptr;
   }
}

std::expected<Row, DbStatus> SQLiteConnector::GetTableList() {
   std::string query =
       "SELECT name FROM sqlite_master WHERE type='table' "
       "AND name NOT LIKE 'sqlite_%' ORDER BY name";

   sqlite3_stmt* raw_stmt = nullptr;
   if (sqlite3_prepare_v2(db, query.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK) {
      return std::unexpected(DbStatus{.type = DbError::Unknown, .details = sqlite3_errmsg(db)});
   }

   UniqueStmtPtr stmt(raw_stmt);
   Row result;

   while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
      const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
      result.push_back(val ? val : "NULL");
   }
   return result;
}

std::expected<Row, DbStatus> SQLiteConnector::GetTableSchema(const std::string& tableName) {
   std::string query = std::format("SELECT sql FROM sqlite_master WHERE type='table' AND name='{}'", tableName);

   sqlite3_stmt* raw_stmt = nullptr;
   if (sqlite3_prepare_v2(db, query.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK) {
      return std::unexpected(DbStatus{.type = DbError::Unknown, .details = sqlite3_errmsg(db)});
   }

   UniqueStmtPtr stmt(raw_stmt);
   Row result;

   int step_res = sqlite3_step(stmt.get());
   if (step_res == SQLITE_ROW) {
      const char* schema = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
      result.push_back(schema ? schema : "NULL");
   } else {
      return std::unexpected(DbStatus{.type = DbError::TableNotFound, .details = "Table not found or no schema"});
   }
   return result;
}

SQLiteConnector::~SQLiteConnector() { Disconnect(); }

#include "SqliteConnector.hpp"

#include <sqlite3.h>

// RAII
struct SQLiteStmtDeleter {
   void operator()(sqlite3_stmt* stmt) const { sqlite3_finalize(stmt); }
};

void SqliteDeleter::operator()(sqlite3* db) const { sqlite3_close(db); }

using UniqueStmtPtr = std::unique_ptr<sqlite3_stmt, SQLiteStmtDeleter>;

std::optional<DbError> SQLiteConnector::Connect(const std::string& connectionString) {
   if (db) return std::nullopt;  // already connect

   sqlite3* raw_ptr = nullptr;
   const int rc = sqlite3_open(connectionString.c_str(), &raw_ptr);

   db.reset(raw_ptr);

   if (rc != SQLITE_OK) {
      std::string err_details = raw_ptr ? sqlite3_errmsg(raw_ptr) : "Unknown memory error";

      db.reset();

      return DbError{.details = err_details};
   }

   return std::nullopt;
}

std::expected<Table, DbError> SQLiteConnector::FetchAll(const std::string& query) {
   if (!db) {
      return std::unexpected(DbError{.details = "Database not connected"});
   }
   Table result;
   sqlite3_stmt* raw_stmt = nullptr;

   int ret_code = sqlite3_prepare_v2(db.get(), query.c_str(), -1, &raw_stmt, nullptr);
   if (ret_code != SQLITE_OK) {
      return std::unexpected(DbError{.details = sqlite3_errmsg(db.get())});
   }

   const UniqueStmtPtr stmt(raw_stmt);
   const int column_count = sqlite3_column_count(stmt.get());

   Row column_names;
   column_names.reserve(column_count);

   for (int i = 0; i < column_count; ++i) {
      const char* name = sqlite3_column_name(stmt.get(), i);
      column_names.emplace_back(name ? name : "NULL");
   }
   result.push_back(std::move(column_names));

   while ((ret_code = sqlite3_step(stmt.get())) == SQLITE_ROW) {
      Row row;
      row.reserve(column_count);
      for (int i = 0; i < column_count; ++i) {
         const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), i));
         row.emplace_back(text ? text : "NULL");
      }
      result.push_back(std::move(row));
   }

   if (ret_code != SQLITE_DONE) {
      return std::unexpected(DbError{.details = sqlite3_errmsg(db.get())});
   }
   return result;
}

void SQLiteConnector::Disconnect() { db.reset(); }

SQLiteConnector::~SQLiteConnector() { Disconnect(); }

#include "SqliteConnector.hpp"

#include <sqlite3.h>

#include <format>
#include <source_location>

#include "DataBaseInterface.hpp"

std::expected<bool, std::string> SQLiteConnector::Connect(const std::string& connectionString) {
   if (sqlite3_open(connectionString.c_str(), &db) == SQLITE_OK) {
      return true;
   }
   std::string errorMessage = sqlite3_errmsg(db);
   sqlite3_close(db);
   db = nullptr;

   return std::unexpected(errorMessage);
}

std::expected<Table, std::string> SQLiteConnector::FetchAll(const std::string& query) {
   Table result;
   sqlite3_stmt* stmt{};

   int ret_code = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
   if (ret_code != SQLITE_OK) {
      std::string error_message = sqlite3_errmsg(db);
      constexpr std::source_location loc = std::source_location::current();
      return std::unexpected(
          std::format("Ошибка запроса в SQLiteConnector::ExecuteQuery {}:{} {}", loc.line(), loc.column(), error_message));
   }

   int column_count = sqlite3_column_count(stmt);
   std::vector<std::string> column_names;
   for (int i = 0; i < column_count; ++i) {
      const char* column_name = sqlite3_column_name(stmt, i);
      column_names.push_back(column_name ? column_name : "NULL");
   }
   result.push_back(std::move(column_names));

   while ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
      Row row;
      for (int i = 0; i < column_count; ++i) {
         const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
         row.reserve(column_count);
         row.push_back(column_value ? column_value : "NULL");
      }
      result.push_back(row);
   }
   if (ret_code != SQLITE_DONE) {
      std::string error_message = sqlite3_errmsg(db);
      constexpr std::source_location loc = std::source_location::current();
      return std::unexpected(
          std::format("Ошибка завершения запроса в SQLiteConnector::ExecuteQuery {}:{} {}", loc.line(), loc.column(), error_message));
   }

   sqlite3_finalize(stmt);
   return result;
}

std::expected<int, std::string> SQLiteConnector::ExecuteUpdate(const std::string& query) const {
   int ret_code = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);

   if (ret_code != SQLITE_OK) {
      std::string error_message = sqlite3_errmsg(db);
      constexpr std::source_location loc = std::source_location::current();
      return std::unexpected(
          std::format("Ошибка запроса в SQLiteConnector::ExecuteQuery {}:{} {}", loc.line(), loc.column(), error_message));
   }
   int rows_affected = sqlite3_changes(db);

   return rows_affected;
}

std::expected<Row, std::string> SQLiteConnector::GetTableList() {
   std::string query =
       "SELECT name FROM sqlite_master "
       "WHERE type='table' AND name NOT LIKE 'sqlite_%' "
       "ORDER BY name";
   sqlite3_stmt* stmt{};
   int return_code = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
   if (return_code != SQLITE_OK) {
      std::string error_message = sqlite3_errmsg(db);
      constexpr std::source_location loc = std::source_location::current();
      return std::unexpected(
          std::format("Ошибка списка таблиц SQLiteConnector::GetTableList {}:{} {}", loc.line(), loc.column(), error_message));
   }

   std::vector<std::string> result{};
   int column_count = sqlite3_column_count(stmt);
   while ((return_code = sqlite3_step(stmt)) == SQLITE_ROW) {
      for (int i = 0; i < column_count; ++i) {
         const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
         result.push_back(column_value ? column_value : "NULL");
      }
   }
   if (return_code != SQLITE_DONE) {
      std::string error_message = sqlite3_errmsg(db);
      constexpr std::source_location loc = std::source_location::current();
      return std::unexpected(
          std::format("Ошибка завершения запроса в SQLiteConnector::GetTableList {}:{} {}", loc.line(), loc.column(), error_message));
   }
   sqlite3_finalize(stmt);
   return result;
}

std::expected<Row, std::string> SQLiteConnector::GetTableSchema(const std::string& tableName) {
   std::string query = std::format("select sql from sqlite_master where type = 'table' and name = '{}'", tableName);

   sqlite3_stmt* stmt = nullptr;
   int ret_code = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

   if (ret_code != SQLITE_OK) {
      std::string error_message = sqlite3_errmsg(db);
      auto loc = std::source_location::current();
      return std::unexpected(std::format("Ошибка получения схемы таблицы {} в SQLiteConnector::GetTableSchema {}:{} {}", tableName,
                                         loc.line(), loc.column(), error_message));
   }

   Row result;
   if (sqlite3_step(stmt) == SQLITE_ROW) {
      const char* schema = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
      if (schema) {
         result.push_back(schema);
      } else {
         result.push_back("NULL");
      }
   } else {
      // Таблица не найдена
      sqlite3_finalize(stmt);
      return std::unexpected(std::format("Таблица '{}' не найдена в SQLiteConnector::GetTableSchema", tableName));
   }

   if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::string error_message = sqlite3_errmsg(db);
      auto loc = std::source_location::current();
      sqlite3_finalize(stmt);
      return std::unexpected(
          std::format("Ошибка завершения запроса в SQLiteConnector::GetTableSchema {}:{} {}", loc.line(), loc.column(), error_message));
   }

   sqlite3_finalize(stmt);
   return result;
}

void SQLiteConnector::Disconnect() { sqlite3_close(db); }
SQLiteConnector::~SQLiteConnector() { Disconnect(); }

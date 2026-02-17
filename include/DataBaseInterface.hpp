#pragma once
#include <concepts>
#include <expected>
#include <string>
#include <vector>

using Row = std::vector<std::string>;
using Table = std::vector<Row>;

enum class DbError {
   Success = 0,  ///< Everything is OK
   // MOST important categories
   InvalidQuery,      ///< SQL syntax error
   ConnectionError,   ///< Can't connect or lost network
   BrokenConnection,  ///< Server is unavailable or network configuration is poor
   LibPqError,        ///< Lib pqxx error
   ReadWriteError,    ///< File system or Disk problems
   AuthError,         ///< Password/Permissions
   TableNotFound,     ///< Table not found
   Unknown            ///< Something else
};

struct DbStatus {
   DbError type;         // Simple enum
   std::string details;  // Direct message from SQLite/Postgres
};

template <typename T>
concept DatabaseConnection = requires(T conn, const std::string& query) {
   { conn.Connect(query) } -> std::same_as<std::expected<bool, DbStatus>>;
   { conn.FetchAll(query) } -> std::same_as<std::expected<Table, DbStatus>>;
   { conn.Disconnect() } -> std::same_as<void>;
};

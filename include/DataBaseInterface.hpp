#pragma once
#include <concepts>
#include <expected>
#include <string>
#include <vector>

using Row = std::vector<std::string>;
using Table = std::vector<Row>;

struct DbError {
  std::string details; // Direct message from SQLite/Postgres
};

template <typename T>
concept DatabaseConnection = requires(T conn, const std::string &query) {
  { conn.Connect(query) } -> std::same_as<std::expected<void, DbError>>;
  { conn.FetchAll(query) } -> std::same_as<std::expected<Table, DbError>>;
  { conn.Disconnect() } -> std::same_as<void>;
};

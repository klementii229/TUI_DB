#pragma once
#include <concepts>
#include <expected>
#include <string>
#include <vector>

using Row = std::vector<std::string>;
using Table = std::vector<Row>;

enum class DbError {
   Success = 0,
   QuerySyntax,       // Ошибка в тексте запроса
   ConnectionFailed,  // Не удалось открыть файл/подключиться
   ExecuteError,      // Ошибка при записи/удалении
   TableNotFound,     // Таблицы не существует
   Unknown
};

template <typename T>
concept DatabaseConnection = requires(T conn, const std::string& query) {
   { conn.Connect(query) } -> std::same_as<std::expected<bool, DbError>>;
   { conn.FetchAll(query) } -> std::same_as<std::expected<Table, DbError>>;
   { conn.Disconnect() } -> std::same_as<void>;
};

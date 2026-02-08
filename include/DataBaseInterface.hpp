#pragma once
#include <expected>
#include <string>
#include <vector>

using Row = std::vector<std::string>;
using Table = std::vector<Row>;

class IDatabaseConnector {
  public:
   // ##########################################
   virtual std::expected<Table, std::string> FetchAll(const std::string& query) = 0;  // Получение результатов с запроса

   virtual std::expected<int, std::string> ExecuteUpdate(const std::string& command) const = 0;
   // Выполнение запроса без результата
   // ##########################################

   virtual std::expected<Row, std::string> GetTableList() = 0;                                // Список таблиц в базе
   virtual std::expected<Row, std::string> GetTableSchema(const std::string& tableName) = 0;  // Схема конкретной таблицы

   // ##########################################
   virtual std::expected<bool, std::string> Connect(const std::string& connectionString) = 0;
   virtual void Disconnect() = 0;
   // ##########################################
   virtual ~IDatabaseConnector() = default;
   IDatabaseConnector() = default;
   IDatabaseConnector(const IDatabaseConnector&) = default;
   IDatabaseConnector(IDatabaseConnector&&) = default;
   IDatabaseConnector& operator=(const IDatabaseConnector&) = default;
   IDatabaseConnector& operator=(IDatabaseConnector&&) = default;
};

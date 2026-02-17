#include "PostgresConnector.hpp"

#include <exception>
#include <memory>

#include "DataBaseInterface.hpp"

void PostgresConnector::Disconnect() { conn.reset(); }

std::expected<bool, DbStatus> PostgresConnector::Connect(const std::string& connectionString) {
   try {
      conn = std::make_unique<pqxx::connection>(connectionString);
   } catch (const pqxx::broken_connection& e) {
      return std::unexpected(DbStatus{.type = DbError::BrokenConnection, .details = e.what()});

   } catch (const pqxx::sql_error& e) {
      return std::unexpected(DbStatus{.type = DbError::ConnectionError, .details = e.what()});

   } catch (const pqxx::failure& e) {
      return std::unexpected(DbStatus{.type = DbError::LibPqError, .details = e.what()});

   } catch (const std::exception& e) {
      return std::unexpected(DbStatus{.type = DbError::Unknown, .details = e.what()});
   }
   return true;
}

std::expected<Table, DbStatus> PostgresConnector::FetchAll(const std::string& query) {
   Table result;
   try {
      pqxx::work tx{*conn};
      pqxx::result p_res = tx.exec(query);
      result.reserve(p_res.size() + 1);

      Row headers;
      headers.reserve(p_res.columns());

      for (int i = 0; i < p_res.columns(); ++i) {
         headers.emplace_back(p_res.column_name(i));
      }
      result.push_back(std::move(headers));

      for (const auto& row : p_res) {
         Row row_data{};
         row_data.reserve(row.size());
         for (const auto& col : row) {
            row_data.emplace_back(col.is_null() ? "NULL" : col.c_str());
         }
         result.push_back(std::move(row_data));
      }

      tx.commit();

   } catch (const pqxx::sql_error& e) {
      return std::unexpected(DbStatus{.type = DbError::InvalidQuery, .details = e.what()});

   } catch (const pqxx::broken_connection& e) {
      return std::unexpected(DbStatus{.type = DbError::BrokenConnection, .details = e.what()});

   } catch (const pqxx::failure& e) {
      return std::unexpected(DbStatus{.type = DbError::LibPqError, .details = e.what()});

   } catch (const std::exception& e) {
      return std::unexpected(DbStatus{.type = DbError::Unknown, .details = e.what()});
   }

   return result;
}
PostgresConnector::~PostgresConnector() { Disconnect(); }

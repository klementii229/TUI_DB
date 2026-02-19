cmake_minimum_required(VERSION 3.16)
project(TUI_DB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

find_package(ftxui CONFIG REQUIRED)
find_package(libpqxx  CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_executable(tui_db ${SOURCES} ${HEADERS})

target_include_directories(tui_db
    PRIVATE
        include
)

target_link_libraries(tui_db
    PRIVATE
        ftxui::dom ftxui::screen ftxui::component
        libpqxx::pqxx
        unofficial::sqlite3::sqlite3
        SQLiteCpp
)

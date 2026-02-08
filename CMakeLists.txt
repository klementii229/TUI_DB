cmake_minimum_required(VERSION 3.16)
project(TUI_DB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ================================
# Поиск библиотек: libsqlite3 и libpq
# ================================
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)


#Поиск libpq (PostgreSQL)
pkg_check_modules(LIBPQ REQUIRED libpq)

#Поиск libsqlite3
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "Выключить установку FTXUI в систему" FORCE)

# ================================
# Добавляем FTXUI
# ================================
include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

# ================================
# Собираем исходники
# ================================
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_executable(tui_db ${SOURCES} ${HEADERS})

# ================================
# Include директории
# ================================
target_include_directories(tui_db
    PRIVATE
        ${SQLITE3_INCLUDE_DIRS}
        ${LIBPQ_INCLUDE_DIRS}
        include
)

# ================================
# Линковка библиотек
# ================================
target_link_libraries(tui_db
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
        ${SQLITE3_LIBRARIES}
        ${LIBPQ_LIBRARIES}
)

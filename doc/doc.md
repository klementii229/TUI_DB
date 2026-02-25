# TUI_DB 🚀

I am a student and an aspiring C++ developer.
My project, TUI_DB, was born from a simple idea: to create the most lightweight and fast tool for working with databases (SQLite and PostgreSQL).

## 🎯 Why use it?

When developing an API or debugging an application, you often need to "take a look" at the data right then and there.
TUI_DB allows you to connect to a database in the shortest possible time with minimal configuration.

## 🖥 Why TUI?

Since the application runs in a terminal, it is ideal for working via SSH.
You don't need to forward ports or install heavy GUI clients on remote servers — just run the binary.
Is it necessary to install db weawer on remote servers? Therefore, it is not necessary anywhere.

## 🛠 Architecture & Components

The application is built using modern C++23 standards and several key components:

1. LoginForm: Manages authorization and database type selection.
2. DataBaseExplorer: The main hub for processing SQL queries and visualizing results.
3. Data Access Layer: Specialized classes for interacting with specific DBMS.
4. Error Handling: If there is a syntax error, the application displays an error message directly in the interface in real-time.

## 🎮 Navigation & Controls

- Movement: Use the [↑] and [↓] arrow keys to navigate between interface elements.
- Execution: Enter your query, switch to the "Enter" button, and press the Enter key.
- Scrolling: The bottom progress bars allow you to scroll through the table. When focused on them, use the [←] and [→] arrows.

## ⚡ Optimization & Speed

To prevent the interface from freezing when retrieving thousands of rows, I implemented pagination:

- Each page displays a fixed 500 records (controlled by the rows_per_page parameter in the code, which you can easily modify).
- Switch between pages using the [PgUp] and [PgDown] keys.

## 📦 Technical Stack

- Standard: C++23
- Build System: CMake
- Dependencies: vcpkg (FTXUI, SQLite3, libpq, libpqxx)

I understand the project is still in its early stages and not perfect, but it represents my contribution to Open Source and a great practice in system design. If you have ideas or found a bug, I look forward to hearing from you in the Discussions section!

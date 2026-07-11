#pragma once
#include <string>

// Forward declaration of sqlite3 struct from SQLite C library
struct sqlite3;

namespace Optix::Repositories {

class IDatabase {
public:
    virtual ~IDatabase() = default;

    // Open connection to the database file
    virtual bool open(const std::string& path) = 0;

    // Close active connection
    virtual void close() = 0;

    // Get the underlying raw sqlite3 handle
    virtual sqlite3* getHandle() const = 0;

    // Execute simple SQL command (DDL or simple DML)
    virtual bool execute(const std::string& sql) = 0;
};

} // namespace Optix::Repositories

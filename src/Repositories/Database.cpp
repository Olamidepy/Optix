#include "Database.hpp"
#include "Utilities/Crypto.hpp"

#ifdef OPTIX_MOCK_BACKEND
#include "Core/MockBackend.hpp"
#else
#include <sqlite3.h>
#endif

#include <iostream>

namespace Optix::Repositories {

Database::Database() {
}

Database::~Database() {
    close();
}

bool Database::open(const std::string& path) {
#ifdef OPTIX_MOCK_BACKEND
    (void)path;
    std::cout << "Optix Database: Running in MOCK BACKEND mode. In-memory data initialized." << std::endl;
    return true;
#else
    close(); // Close any active connection
    
    int rc = sqlite3_open(path.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Optix Database: Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        close();
        return false;
    }
    
    // Enable Foreign Keys in SQLite
    execute("PRAGMA foreign_keys = ON;");
    
    return initializeSchema();
#endif
}

void Database::close() {
#ifndef OPTIX_MOCK_BACKEND
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
#endif
}

sqlite3* Database::getHandle() const {
    return m_db;
}

bool Database::execute(const std::string& sql) {
#ifdef OPTIX_MOCK_BACKEND
    (void)sql;
    return true;
#else
    if (!m_db) {
        return false;
    }
    
    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Optix Database: SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
#endif
}

bool Database::initializeSchema() {
#ifdef OPTIX_MOCK_BACKEND
    return true;
#else
    // 1. Create Users Table
    const std::string createUsersTable = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
        
    // 2. Create Students Table
    const std::string createStudentsTable =
        "CREATE TABLE IF NOT EXISTS Students ("
        "student_id TEXT PRIMARY KEY,"
        "matric_number TEXT UNIQUE NOT NULL,"
        "full_name TEXT NOT NULL,"
        "department TEXT NOT NULL,"
        "level INTEGER NOT NULL,"
        "gender TEXT NOT NULL,"
        "email TEXT,"
        "phone TEXT,"
        "face_registered INTEGER DEFAULT 0,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    // 3. Create Attendance Table
    const std::string createAttendanceTable =
        "CREATE TABLE IF NOT EXISTS Attendance ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "student_id TEXT NOT NULL,"
        "date TEXT NOT NULL,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "status TEXT NOT NULL,"
        "UNIQUE(student_id, date),"
        "FOREIGN KEY (student_id) REFERENCES Students(student_id) ON DELETE CASCADE"
        ");";

    if (!execute(createUsersTable) || !execute(createStudentsTable) || !execute(createAttendanceTable)) {
        return false;
    }

    // 4. Seed default admin account if table is empty
    sqlite3_stmt* stmt = nullptr;
    const std::string selectUsersCount = "SELECT COUNT(*) FROM Users;";
    int rc = sqlite3_prepare_v2(m_db, selectUsersCount.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    int userCount = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userCount = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (userCount == 0) {
        // Seed default: admin / admin123
        std::string passwordHash = Utilities::Crypto::hashPassword("admin123");
        const std::string insertAdmin = "INSERT INTO Users (username, password_hash) VALUES (?, ?);";
        rc = sqlite3_prepare_v2(m_db, insertAdmin.c_str(), -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            std::cout << "Optix Database: Seeded default administrator account (username: admin, password: admin123)" << std::endl;
        }
    }

    return true;
#endif
}

} // namespace Optix::Repositories

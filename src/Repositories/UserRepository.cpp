#include "UserRepository.hpp"
#include <sqlite3.h>
#include <iostream>

namespace Optix::Repositories {

UserRepository::UserRepository(std::shared_ptr<IDatabase> db)
    : m_db(std::move(db)) {
}

bool UserRepository::validateUser(const std::string& username, const std::string& password_hash, Models::User& outUser) {
    sqlite3* handle = m_db->getHandle();
    if (!handle) {
        return false;
    }

    const std::string query = "SELECT id, username, password_hash, created_at FROM Users WHERE username = ? AND password_hash = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Optix UserRepository: Prepare validate statement failed: " << sqlite3_errmsg(handle) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT);

    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outUser.id = sqlite3_column_int(stmt, 0);
        outUser.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        outUser.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        outUser.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        success = true;
    }

    sqlite3_finalize(stmt);
    return success;
}

bool UserRepository::createUser(const std::string& username, const std::string& password_hash) {
    sqlite3* handle = m_db->getHandle();
    if (!handle) {
        return false;
    }

    const std::string query = "INSERT INTO Users (username, password_hash) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool UserRepository::hasUsers() {
    sqlite3* handle = m_db->getHandle();
    if (!handle) {
        return false;
    }

    const std::string query = "SELECT COUNT(*) FROM Users;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

} // namespace Optix::Repositories

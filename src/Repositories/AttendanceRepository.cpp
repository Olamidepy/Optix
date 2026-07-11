#include "AttendanceRepository.hpp"
#include <sqlite3.h>
#include <iostream>

namespace Optix::Repositories {

AttendanceRepository::AttendanceRepository(std::shared_ptr<IDatabase> db)
    : m_db(std::move(db)) {
}

bool AttendanceRepository::recordAttendance(const std::string& student_id, const std::string& date, const std::string& status) {
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    // Use INSERT OR IGNORE or INSERT OR REPLACE since we want unique check-in per student per date
    const std::string query = "INSERT OR REPLACE INTO Attendance (student_id, date, status) VALUES (?, ?, ?);";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool AttendanceRepository::hasRecord(const std::string& student_id, const std::string& date) {
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = "SELECT COUNT(*) FROM Attendance WHERE student_id = ? AND date = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

static Models::Attendance parseAttendance(sqlite3_stmt* stmt) {
    Models::Attendance a;
    a.id = sqlite3_column_int(stmt, 0);
    a.student_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    a.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    a.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    a.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    return a;
}

std::vector<Models::Attendance> AttendanceRepository::getDailyAttendance(const std::string& date) {
    std::vector<Models::Attendance> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    const std::string query = "SELECT id, student_id, date, timestamp, status FROM Attendance WHERE date = ? ORDER BY timestamp DESC;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseAttendance(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<Models::Attendance> AttendanceRepository::getStudentAttendanceHistory(const std::string& student_id) {
    std::vector<Models::Attendance> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    const std::string query = "SELECT id, student_id, date, timestamp, status FROM Attendance WHERE student_id = ? ORDER BY date DESC;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseAttendance(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<Models::Attendance> AttendanceRepository::getAllAttendance() {
    std::vector<Models::Attendance> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    const std::string query = "SELECT id, student_id, date, timestamp, status FROM Attendance ORDER BY date DESC, timestamp DESC;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseAttendance(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

int AttendanceRepository::getPresentTodayCount(const std::string& date) {
    sqlite3* handle = m_db->getHandle();
    if (!handle) return 0;

    const std::string query = "SELECT COUNT(*) FROM Attendance WHERE date = ? AND status = 'Present';";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

} // namespace Optix::Repositories

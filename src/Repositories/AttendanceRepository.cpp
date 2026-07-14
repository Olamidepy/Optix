#include "AttendanceRepository.hpp"

#ifdef OPTIX_MOCK_BACKEND
#include "Core/MockBackend.hpp"
#include "Models/Attendance.hpp"
#include "Models/Student.hpp"
#include <algorithm>

// Define global in-memory attendance database
std::vector<Optix::Models::Attendance> s_mockAttendance = {
    {1, "62", "2026-07-13", "2026-07-13 10:30:15", "Present"},
    {2, "2", "2026-07-13", "2026-07-13 11:15:22", "Present"}
};
#else
#include <sqlite3.h>
#endif

#include <iostream>

namespace Optix::Repositories {

AttendanceRepository::AttendanceRepository(std::shared_ptr<IDatabase> db)
    : m_db(std::move(db)) {
}

bool AttendanceRepository::recordAttendance(const std::string& student_id, const std::string& date, const std::string& status) {
#ifdef OPTIX_MOCK_BACKEND
    if (hasRecord(student_id, date)) {
        return false;
    }

    Optix::Models::Attendance log;
    log.id = static_cast<int>(s_mockAttendance.size() + 1);
    log.student_id = student_id;
    log.date = date;
    log.timestamp = date + " 12:00:00";
    log.status = status;

    s_mockAttendance.push_back(log);
    return true;
#else
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
#endif
}

bool AttendanceRepository::hasRecord(const std::string& student_id, const std::string& date) {
#ifdef OPTIX_MOCK_BACKEND
    for (const auto& log : s_mockAttendance) {
        if (log.student_id == student_id && log.date == date) {
            return true;
        }
    }
    return false;
#else
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
#endif
}

#ifndef OPTIX_MOCK_BACKEND
static Models::Attendance parseAttendance(sqlite3_stmt* stmt) {
    Models::Attendance a;
    a.id = sqlite3_column_int(stmt, 0);
    a.student_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    a.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    a.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    a.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    return a;
}
#endif

std::vector<Models::Attendance> AttendanceRepository::getDailyAttendance(const std::string& date) {
#ifdef OPTIX_MOCK_BACKEND
    std::vector<Models::Attendance> list;
    for (const auto& log : s_mockAttendance) {
        if (log.date == date) {
            list.push_back(log);
        }
    }
    return list;
#else
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
#endif
}

std::vector<Models::Attendance> AttendanceRepository::getStudentAttendanceHistory(const std::string& student_id) {
#ifdef OPTIX_MOCK_BACKEND
    std::vector<Models::Attendance> list;
    for (const auto& log : s_mockAttendance) {
        if (log.student_id == student_id) {
            list.push_back(log);
        }
    }
    return list;
#else
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
#endif
}

std::vector<Models::Attendance> AttendanceRepository::getAllAttendance() {
#ifdef OPTIX_MOCK_BACKEND
    return s_mockAttendance;
#else
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
#endif
}

int AttendanceRepository::getPresentTodayCount(const std::string& date) {
#ifdef OPTIX_MOCK_BACKEND
    int count = 0;
    for (const auto& log : s_mockAttendance) {
        if (log.date == date && (log.status == "Present" || log.status == "PRESENT")) {
            count++;
        }
    }
    return count;
#else
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
#endif
}

} // namespace Optix::Repositories

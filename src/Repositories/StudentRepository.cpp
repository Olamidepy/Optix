#include "StudentRepository.hpp"

#ifdef OPTIX_MOCK_BACKEND
#include "Core/MockBackend.hpp"
#include "Models/Student.hpp"
#include <algorithm>

// Define global in-memory students database
std::vector<Optix::Models::Student> s_mockStudents = {
    {"1", "CPE/23/4824", "Opeoluwa Mustapha", "Computer Engineering", 300, "Male", "opeoluwa@example.com", "08012345678", true, "2026-07-13 10:00:00"},
    {"2", "EEE/2021/001", "John Doe", "EEE", 300, "Male", "johndoe@example.com", "08012345678", true, "2026-07-13 10:00:00"},
    {"3", "MEE/2021/045", "Jane Smith", "MEE", 400, "Female", "janesmith@example.com", "08098765432", false, "2026-07-13 10:05:00"}
};

#else
#include <sqlite3.h>
#endif

#include <iostream>

namespace Optix::Repositories {

StudentRepository::StudentRepository(std::shared_ptr<IDatabase> db)
    : m_db(std::move(db)) {
}

bool StudentRepository::addStudent(const Models::Student& s) {
#ifdef OPTIX_MOCK_BACKEND
    auto copy = s;
    if (copy.student_id.empty()) {
        copy.student_id = std::to_string(s_mockStudents.size() + 1);
    }
    s_mockStudents.push_back(copy);
    return true;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = 
        "INSERT INTO Students (student_id, matric_number, full_name, department, level, gender, email, phone, face_registered) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, s.student_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.matric_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.full_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, s.level);
    sqlite3_bind_text(stmt, 6, s.gender.c_str(), -1, SQLITE_TRANSIENT);
    
    if (s.email.empty()) {
        sqlite3_bind_null(stmt, 7);
    } else {
        sqlite3_bind_text(stmt, 7, s.email.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    if (s.phone.empty()) {
        sqlite3_bind_null(stmt, 8);
    } else {
        sqlite3_bind_text(stmt, 8, s.phone.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    sqlite3_bind_int(stmt, 9, s.face_registered ? 1 : 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

bool StudentRepository::updateStudent(const Models::Student& s) {
#ifdef OPTIX_MOCK_BACKEND
    for (auto& st : s_mockStudents) {
        if (st.student_id == s.student_id) {
            st = s;
            return true;
        }
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = 
        "UPDATE Students SET matric_number = ?, full_name = ?, department = ?, level = ?, gender = ?, email = ?, phone = ?, face_registered = ? "
        "WHERE student_id = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, s.matric_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.full_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, s.level);
    sqlite3_bind_text(stmt, 5, s.gender.c_str(), -1, SQLITE_TRANSIENT);
    
    if (s.email.empty()) {
        sqlite3_bind_null(stmt, 6);
    } else {
        sqlite3_bind_text(stmt, 6, s.email.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    if (s.phone.empty()) {
        sqlite3_bind_null(stmt, 7);
    } else {
        sqlite3_bind_text(stmt, 7, s.phone.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    sqlite3_bind_int(stmt, 8, s.face_registered ? 1 : 0);
    sqlite3_bind_text(stmt, 9, s.student_id.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

bool StudentRepository::deleteStudent(const std::string& student_id) {
#ifdef OPTIX_MOCK_BACKEND
    auto it = std::remove_if(s_mockStudents.begin(), s_mockStudents.end(),
        [&student_id](const auto& s) { return s.student_id == student_id; });
    if (it != s_mockStudents.end()) {
        s_mockStudents.erase(it, s_mockStudents.end());
        return true;
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = "DELETE FROM Students WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

#ifndef OPTIX_MOCK_BACKEND
static Models::Student parseStudent(sqlite3_stmt* stmt) {
    Models::Student s;
    s.student_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    s.matric_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    s.full_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    s.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    s.level = sqlite3_column_int(stmt, 4);
    s.gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    
    if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
        s.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    }
    if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
        s.phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    }
    
    s.face_registered = sqlite3_column_int(stmt, 8) != 0;
    s.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
    return s;
}
#endif

std::optional<Models::Student> StudentRepository::getStudentById(const std::string& student_id) {
#ifdef OPTIX_MOCK_BACKEND
    for (const auto& st : s_mockStudents) {
        if (st.student_id == student_id) return st;
    }
    return std::nullopt;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return std::nullopt;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE student_id = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Models::Student> student;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student = parseStudent(stmt);
    }
    sqlite3_finalize(stmt);
    return student;
#endif
}

std::optional<Models::Student> StudentRepository::getStudentByMatric(const std::string& matric_number) {
#ifdef OPTIX_MOCK_BACKEND
    for (const auto& st : s_mockStudents) {
        if (st.matric_number == matric_number) return st;
    }
    return std::nullopt;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return std::nullopt;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE matric_number = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, matric_number.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Models::Student> student;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student = parseStudent(stmt);
    }
    sqlite3_finalize(stmt);
    return student;
#endif
}

std::vector<Models::Student> StudentRepository::getAllStudents() {
#ifdef OPTIX_MOCK_BACKEND
    return s_mockStudents;
#else
    std::vector<Models::Student> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students ORDER BY full_name ASC;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseStudent(stmt));
    }
    
    sqlite3_finalize(stmt);
    return list;
#endif
}

std::vector<Models::Student> StudentRepository::searchStudents(const std::string& search_query, const std::string& deptFilter) {
#ifdef OPTIX_MOCK_BACKEND
    std::vector<Models::Student> list;
    for (const auto& st : s_mockStudents) {
        bool matchSearch = search_query.empty() || 
                           (st.full_name.find(search_query) != std::string::npos) || 
                           (st.matric_number.find(search_query) != std::string::npos);
        bool matchDept = deptFilter.empty() || (st.department == deptFilter);
        if (matchSearch && matchDept) {
            list.push_back(st);
        }
    }
    return list;
#else
    std::vector<Models::Student> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    // Build the query dynamically depending on filters
    std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE 1=1";
        
    if (!search_query.empty()) {
        query += " AND (full_name LIKE ? OR matric_number LIKE ? OR student_id LIKE ?)";
    }
    if (!deptFilter.empty()) {
        query += " AND department = ?";
    }
    query += " ORDER BY full_name ASC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    int bindIdx = 1;
    if (!search_query.empty()) {
        std::string wild = "%" + search_query + "%";
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (!deptFilter.empty()) {
        sqlite3_bind_text(stmt, bindIdx++, deptFilter.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseStudent(stmt));
    }
    
    sqlite3_finalize(stmt);
    return list;
#endif
}

int StudentRepository::getStudentCount() {
#ifdef OPTIX_MOCK_BACKEND
    return static_cast<int>(s_mockStudents.size());
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return 0;

    const std::string query = "SELECT COUNT(*) FROM Students;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return 0;

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
#endif
}

bool StudentRepository::setFaceRegistered(const std::string& student_id, bool registered) {
#ifdef OPTIX_MOCK_BACKEND
    for (auto& st : s_mockStudents) {
        if (st.student_id == student_id) {
            st.face_registered = registered;
            return true;
        }
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = "UPDATE Students SET face_registered = ? WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, registered ? 1 : 0);
    sqlite3_bind_text(stmt, 2, student_id.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

} // namespace Optix::Repositories

#pragma once
#include "Models/Attendance.hpp"
#include <vector>
#include <string>

namespace Optix::Repositories {

class IAttendanceRepository {
public:
    virtual ~IAttendanceRepository() = default;

    // Logs attendance for a specific student on a given date. Status is "Present", "Absent", "Late".
    virtual bool recordAttendance(const std::string& student_id, const std::string& date, const std::string& status) = 0;

    // Checks if attendance has already been recorded for a student on a specific date
    virtual bool hasRecord(const std::string& student_id, const std::string& date) = 0;

    // Fetches attendance records for a specific date
    virtual std::vector<Models::Attendance> getDailyAttendance(const std::string& date) = 0;

    // Fetches attendance history for a single student
    virtual std::vector<Models::Attendance> getStudentAttendanceHistory(const std::string& student_id) = 0;

    // Fetches all attendance records
    virtual std::vector<Models::Attendance> getAllAttendance() = 0;

    // Gets the count of students present today
    virtual int getPresentTodayCount(const std::string& date) = 0;
};

} // namespace Optix::Repositories

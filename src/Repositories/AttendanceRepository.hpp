#pragma once
#include "IAttendanceRepository.hpp"
#include "IDatabase.hpp"
#include <memory>

namespace Optix::Repositories {

class AttendanceRepository : public IAttendanceRepository {
public:
    explicit AttendanceRepository(std::shared_ptr<IDatabase> db);
    ~AttendanceRepository() override = default;

    bool recordAttendance(const std::string& student_id, const std::string& date, const std::string& status) override;
    bool hasRecord(const std::string& student_id, const std::string& date) override;
    std::vector<Models::Attendance> getDailyAttendance(const std::string& date) override;
    std::vector<Models::Attendance> getStudentAttendanceHistory(const std::string& student_id) override;
    std::vector<Models::Attendance> getAllAttendance() override;
    int getPresentTodayCount(const std::string& date) override;

private:
    std::shared_ptr<IDatabase> m_db;
};

} // namespace Optix::Repositories

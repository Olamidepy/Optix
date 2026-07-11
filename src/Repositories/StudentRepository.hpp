#pragma once
#include "IStudentRepository.hpp"
#include "IDatabase.hpp"
#include <memory>

namespace Optix::Repositories {

class StudentRepository : public IStudentRepository {
public:
    explicit StudentRepository(std::shared_ptr<IDatabase> db);
    ~StudentRepository() override = default;

    bool addStudent(const Models::Student& student) override;
    bool updateStudent(const Models::Student& student) override;
    bool deleteStudent(const std::string& student_id) override;
    std::optional<Models::Student> getStudentById(const std::string& student_id) override;
    std::optional<Models::Student> getStudentByMatric(const std::string& matric_number) override;
    std::vector<Models::Student> getAllStudents() override;
    std::vector<Models::Student> searchStudents(const std::string& query, const std::string& deptFilter = "") override;
    int getStudentCount() override;
    bool setFaceRegistered(const std::string& student_id, bool registered) override;

private:
    std::shared_ptr<IDatabase> m_db;
};

} // namespace Optix::Repositories

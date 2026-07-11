#pragma once
#include "Repositories/IStudentRepository.hpp"
#include "Models/Student.hpp"
#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace Optix::Services {

class StudentService {
public:
    explicit StudentService(std::shared_ptr<Repositories::IStudentRepository> studentRepo);
    ~StudentService() = default;

    // Registers a new student. Handles field validation and returns true if successful.
    bool registerStudent(const Models::Student& student, std::string& outError);

    // Updates an existing student's details.
    bool updateStudent(const Models::Student& student, std::string& outError);

    // Deletes a student and their references.
    bool deleteStudent(const std::string& student_id);

    // Get list of all students.
    std::vector<Models::Student> getStudentList() const;

    // Search and filter students.
    std::vector<Models::Student> searchStudents(const std::string& query, const std::string& deptFilter = "") const;

    // Get specific student.
    std::optional<Models::Student> getStudent(const std::string& student_id) const;

    // Get total number of registered students.
    int getStudentCount() const;

    // Mark face registration status.
    bool updateFaceRegistrationStatus(const std::string& student_id, bool registered);

private:
    // Helper to validate email format and field lengths
    bool validateFields(const Models::Student& student, std::string& outError) const;

    std::shared_ptr<Repositories::IStudentRepository> m_studentRepo;
};

} // namespace Optix::Services

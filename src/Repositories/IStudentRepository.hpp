#pragma once
#include "Models/Student.hpp"
#include <vector>
#include <string>
#include <optional>

namespace Optix::Repositories {

class IStudentRepository {
public:
    virtual ~IStudentRepository() = default;

    // Add a new student record
    virtual bool addStudent(const Models::Student& student) = 0;

    // Update an existing student record
    virtual bool updateStudent(const Models::Student& student) = 0;

    // Delete a student by their unique student_id
    virtual bool deleteStudent(const std::string& student_id) = 0;

    // Retrieve a student by their student_id
    virtual std::optional<Models::Student> getStudentById(const std::string& student_id) = 0;

    // Retrieve a student by their matric_number
    virtual std::optional<Models::Student> getStudentByMatric(const std::string& matric_number) = 0;

    // Fetch all student records
    virtual std::vector<Models::Student> getAllStudents() = 0;

    // Search students by name, matric_number, department, etc.
    virtual std::vector<Models::Student> searchStudents(const std::string& query, const std::string& deptFilter = "") = 0;

    // Get total count of registered students
    virtual int getStudentCount() = 0;

    // Set face registered status
    virtual bool setFaceRegistered(const std::string& student_id, bool registered) = 0;
};

} // namespace Optix::Repositories

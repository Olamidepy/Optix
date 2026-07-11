#include "StudentService.hpp"
#include <algorithm>
#include <regex>

namespace Optix::Services {

StudentService::StudentService(std::shared_ptr<Repositories::IStudentRepository> studentRepo)
    : m_studentRepo(std::move(studentRepo)) {
}

bool StudentService::registerStudent(const Models::Student& s, std::string& outError) {
    if (!validateFields(s, outError)) {
        return false;
    }

    // Check duplicate student ID
    if (m_studentRepo->getStudentById(s.student_id).has_value()) {
        outError = "Student ID '" + s.student_id + "' is already registered.";
        return false;
    }

    // Check duplicate matric number
    if (m_studentRepo->getStudentByMatric(s.matric_number).has_value()) {
        outError = "Matric Number '" + s.matric_number + "' is already registered.";
        return false;
    }

    return m_studentRepo->addStudent(s);
}

bool StudentService::updateStudent(const Models::Student& s, std::string& outError) {
    if (!validateFields(s, outError)) {
        return false;
    }

    // Verify student exists
    auto existing = m_studentRepo->getStudentById(s.student_id);
    if (!existing.has_value()) {
        outError = "Student with ID '" + s.student_id + "' does not exist.";
        return false;
    }

    // Check duplicate matric number on OTHER students
    auto matchingMatric = m_studentRepo->getStudentByMatric(s.matric_number);
    if (matchingMatric.has_value() && matchingMatric->student_id != s.student_id) {
        outError = "Matric Number '" + s.matric_number + "' is already registered to another student.";
        return false;
    }

    return m_studentRepo->updateStudent(s);
}

bool StudentService::deleteStudent(const std::string& student_id) {
    return m_studentRepo->deleteStudent(student_id);
}

std::vector<Models::Student> StudentService::getStudentList() const {
    return m_studentRepo->getAllStudents();
}

std::vector<Models::Student> StudentService::searchStudents(const std::string& query, const std::string& deptFilter) const {
    return m_studentRepo->searchStudents(query, deptFilter);
}

std::optional<Models::Student> StudentService::getStudent(const std::string& student_id) const {
    return m_studentRepo->getStudentById(student_id);
}

int StudentService::getStudentCount() const {
    return m_studentRepo->getStudentCount();
}

bool StudentService::updateFaceRegistrationStatus(const std::string& student_id, bool registered) {
    return m_studentRepo->setFaceRegistered(student_id, registered);
}

bool StudentService::validateFields(const Models::Student& s, std::string& outError) const {
    if (s.student_id.empty()) {
        outError = "Student ID cannot be empty.";
        return false;
    }
    if (s.matric_number.empty()) {
        outError = "Matric Number cannot be empty.";
        return false;
    }
    if (s.full_name.empty()) {
        outError = "Full Name cannot be empty.";
        return false;
    }
    if (s.department.empty()) {
        outError = "Department cannot be empty.";
        return false;
    }
    if (s.level <= 0) {
        outError = "Level must be a positive number.";
        return false;
    }
    if (s.gender.empty()) {
        outError = "Gender must be selected.";
        return false;
    }

    // Simple Email Regex Validation (optional field)
    if (!s.email.empty()) {
        const std::regex emailPattern(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        if (!std::regex_match(s.email, emailPattern)) {
            outError = "Invalid email address format.";
            return false;
        }
    }

    // Simple Phone Number Validation (optional field)
    if (!s.phone.empty()) {
        const std::regex phonePattern(R"(^[+]?[0-9]{8,15}$)");
        if (!std::regex_match(s.phone, phonePattern)) {
            outError = "Invalid phone number format. Must contain 8 to 15 digits.";
            return false;
        }
    }

    return true;
}

} // namespace Optix::Services

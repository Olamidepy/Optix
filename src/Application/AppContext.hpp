#pragma once
#include "Core/ThemeManager.hpp"
#include "Repositories/Database.hpp"
#include "Repositories/UserRepository.hpp"
#include "Repositories/StudentRepository.hpp"
#include "Repositories/AttendanceRepository.hpp"
#include "Services/AuthService.hpp"
#include "Services/StudentService.hpp"
#include "Services/FaceService.hpp"
#include "FaceRecognition/LBPHFaceRecognizer.hpp"
#include <memory>

namespace Optix::Application {

class AppContext {
public:
    AppContext() {
        // 1. Initialize core system modules
        themeManager = std::make_shared<Core::ThemeManager>();
        
        // 2. Initialize database
        database = std::make_shared<Repositories::Database>();
        
        // 3. Initialize repositories (passing IDatabase)
        userRepository = std::make_shared<Repositories::UserRepository>(database);
        studentRepository = std::make_shared<Repositories::StudentRepository>(database);
        attendanceRepository = std::make_shared<Repositories::AttendanceRepository>(database);
        
        // 4. Initialize AI recognition algorithms
        auto recognizer = std::make_shared<FaceRecognition::LBPHFaceRecognizer>();
        
        // 5. Initialize business services (passing repositories/recognizer)
        authService = std::make_shared<Services::AuthService>(userRepository);
        studentService = std::make_shared<Services::StudentService>(studentRepository);
        faceService = std::make_shared<Services::FaceService>(studentRepository, recognizer);
    }
    
    ~AppContext() = default;

    // Public Smart Pointer handles for Dependency Injection
    std::shared_ptr<Core::ThemeManager> themeManager;
    std::shared_ptr<Repositories::IDatabase> database;
    std::shared_ptr<Repositories::IUserRepository> userRepository;
    std::shared_ptr<Repositories::IStudentRepository> studentRepository;
    std::shared_ptr<Repositories::IAttendanceRepository> attendanceRepository;
    
    std::shared_ptr<Services::AuthService> authService;
    std::shared_ptr<Services::StudentService> studentService;
    std::shared_ptr<Services::FaceService> faceService;
};

} // namespace Optix::Application

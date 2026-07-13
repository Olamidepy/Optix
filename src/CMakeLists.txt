# Source level CMake configuration for Optix

# Collect all header and source files for the Optix executable
set(OPTIX_SOURCES
    Main.cpp
    
    Application/MainWindow.hpp
    Application/MainWindow.cpp
    Application/AppContext.hpp
    
    Core/ThemeManager.hpp
    Core/ThemeManager.cpp
    Core/Styles.hpp
    
    Models/User.hpp
    Models/Student.hpp
    Models/Attendance.hpp
    
    Repositories/IDatabase.hpp
    Repositories/Database.hpp
    Repositories/Database.cpp
    Repositories/IUserRepository.hpp
    Repositories/UserRepository.hpp
    Repositories/UserRepository.cpp
    Repositories/IStudentRepository.hpp
    Repositories/StudentRepository.hpp
    Repositories/StudentRepository.cpp
    Repositories/IAttendanceRepository.hpp
    Repositories/AttendanceRepository.hpp
    Repositories/AttendanceRepository.cpp
    
    Services/AuthService.hpp
    Services/AuthService.cpp
    Services/StudentService.hpp
    Services/StudentService.cpp
    Services/FaceService.hpp
    Services/FaceService.cpp
    
    FaceRecognition/IFaceRecognizer.hpp
    FaceRecognition/LBPHFaceRecognizer.hpp
    FaceRecognition/LBPHFaceRecognizer.cpp
    
    UI/Views/LoginView.hpp
    UI/Views/LoginView.cpp
    UI/Views/OnboardingView.hpp
    UI/Views/OnboardingView.cpp
    UI/Views/DashboardView.hpp
    UI/Views/DashboardView.cpp
    UI/Views/StudentManagementView.hpp
    UI/Views/StudentManagementView.cpp
    UI/Views/FaceRegistrationView.hpp
    UI/Views/FaceRegistrationView.cpp
    UI/Views/AttendanceView.hpp
    UI/Views/AttendanceView.cpp
    UI/Views/ReportsView.hpp
    UI/Views/ReportsView.cpp
    UI/Views/SettingsView.hpp
    UI/Views/SettingsView.cpp
    
    UI/Common/Sidebar.hpp
    UI/Common/Sidebar.cpp
    
    Utilities/Crypto.hpp
    Utilities/Crypto.cpp
)

# Create the main executable target
add_executable(Optix ${OPTIX_SOURCES})

# Target Header Include Directories
target_include_directories(Optix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
if(OpenCV_FOUND)
    target_include_directories(Optix PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

# Target Link Libraries
target_link_libraries(Optix PRIVATE
    Qt6::Widgets
)
if(OpenCV_FOUND)
    target_link_libraries(Optix PRIVATE ${OpenCV_LIBS})
endif()

# Link SQLite3 target or variables
if(SQLite3_FOUND)
    if(TARGET SQLite::SQLite3)
        target_link_libraries(Optix PRIVATE SQLite::SQLite3)
    else()
        target_include_directories(Optix PRIVATE ${SQLite3_INCLUDE_DIRS})
        target_link_libraries(Optix PRIVATE ${SQLite3_LIBRARIES})
    endif()
endif()

# Platform-specific configurations
if(WIN32)
    # Hide terminal window for GUI release builds
    set_target_properties(Optix PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

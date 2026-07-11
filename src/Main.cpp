#include <QApplication>
#include "Application/AppContext.hpp"
#include "Application/MainWindow.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // 1. Initialize QApplication
    QApplication app(argc, argv);
    
    // Set application identifiers
    QApplication::setApplicationName("Optix");
    QApplication::setOrganizationName("OptixEdu");
    QApplication::setApplicationVersion("1.0.0");

    // 2. Initialize Dependency Injection Container
    auto context = std::make_shared<Optix::Application::AppContext>();

    // 3. Open Database connection
    // Create database file in current workspace directory
    std::string dbPath = "optix.db";
    std::cout << "Optix Main: Initializing database file: " << dbPath << std::endl;
    
    if (!context->database->open(dbPath)) {
        std::cerr << "Optix Main: Critical error - Could not initialize SQLite3 database connection. Exiting." << std::endl;
        return 1;
    }

    // 4. Instantiate and show the Main Window
    Optix::Application::MainWindow mainWindow(context);
    mainWindow.show();

    // 5. Execute Qt event loop
    return app.exec();
}

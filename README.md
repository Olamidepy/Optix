# Optix

**Face Recognition-Based Attendance Management System**

*CPE 302: C++ Programming Course Work by Engr. Akinlabi*

Optix is a modern, modular, production-quality desktop application built with **C++20** and **Qt 6** (Qt Widgets). It incorporates face detection and recognition using **OpenCV 4.x** (LBPH Face Recognizer) and logs academic check-in sessions into an **SQLite3** database. 

The application is architected following robust software engineering practices, separating concerns into independent layers (Repositories, Services, Domain Models, and custom View Controllers) to guarantee clean, testable, and maintainable C++ code.

---

##  Key Features

- **Administrative Login Session**: Restricts dashboard access with password-protected credentials checked against SQLite (using programmatically generated SHA-256 hashes).
- **First-Run Onboarding**: Prompts administrative users to register student records and capture face datasets if the database is initially empty.
- **Student Profile Management**: Full CRUD capabilities supporting matriculation numbers, emails, phone numbers, departments, levels, and face registration statuses.
- **Continuous Face Dataset Capture**: Automatically collects 60 frames from webcam streams for each student under varying angles (simulated stubs in Sprint 2, OpenCV loops in Sprint 3).
- **LBPH Model Training**: Trains the OpenCV Local Binary Patterns Histograms face recognizer and saves self-contained trained variables and student string mappings inside a single YAML file (`models/lbph_model.yml`).
- **Live Verification Check-in**: Recognizes facial frames, queries database limits to prevent double check-ins on the same day, and updates log tables immediately.
- **Dynamic Theme Manager**: Supports a responsive Light/Dark Mode switcher. Evaluates a raw CSS (QSS) template using placeholder variables (`@bg`, `@card`, `@border`, `@primary`) and compiles them dynamically to redraw active GUI layouts instantly.
- **Clean Split Layout**: Premium minimal UI including space headers, sidebar togglers, checkable active buttons, metric card boards, and grid tables.

---

##  Technology Stack

- **Language**: C++20 (utilizing smart pointers, standard library filesystem, and RAII).
- **GUI Framework**: Qt 6.x (Widgets module, custom layout definitions, QSS stylesheets, and event signals).
- **Computer Vision**: OpenCV 4.x (Grayscale conversions, Haar Cascade classifiers, and `cv::face::LBPHFaceRecognizer`).
- **Database Engine**: SQLite3 (Local file-based system accessed via standard SQLite3 C API using prepared statements for optimal query throughput).
- **Build System**: CMake 3.16+.

---

##  Directory Structure

```
Optix/
├── CMakeLists.txt              # Root CMake configuration (finds Qt6, OpenCV, SQLite3)
├── LICENSE                     # License terms
├── README.md                   # System documentation
├── optix.db                    # SQLite3 local database file (generated at runtime)
├── datasets/                   # Generated student face datasets
│   └── raw/
│       └── {student_id}/       # Grayscale face images (sample_0.jpg to sample_60.jpg)
├── models/
│   └── lbph_model.yml          # Trained LBPH model file with student mappings
└── src/
    ├── CMakeLists.txt          # Source compilation and library target links
    ├── Main.cpp                # App entry point, DB loader, and main event loops
    ├── Application/
    │   ├── AppContext.hpp      # Dependency Injection container (service locator)
    │   ├── MainWindow.hpp/cpp  # Main container frame (handles sidebar layouts & stacked tabs)
    │   └── MainWindow.cpp
    ├── Core/
    │   ├── Styles.hpp          # CSS (QSS) template containing color variable placeholders
    │   ├── ThemeManager.hpp/cpp# Color compiler & dark-mode toggler
    │   └── ThemeManager.cpp
    ├── Models/
    │   ├── User.hpp            # Administrator profile struct
    │   ├── Student.hpp         # Student record struct
    │   └── Attendance.hpp      # Attendance check-in log struct
    ├── Repositories/
    │   ├── IDatabase.hpp       # Connection abstract interface
    │   ├── Database.hpp/cpp    # SQLite3 initialization & schema migrations
    │   ├── IUserRepository.hpp # Contract for admin queries
    │   ├── UserRepository.hpp/cpp
    │   ├── IStudentRepository.hpp # Contract for student CRUD queries
    │   ├── StudentRepository.hpp/cpp
    │   ├── IAttendanceRepository.hpp # Contract for check-in queries
    │   └── AttendanceRepository.hpp/cpp
    ├── Services/
    │   ├── AuthService.hpp/cpp # Login sessions & hash security verification
    │   ├── StudentService.hpp/cpp # Student record validation
    │   └── FaceService.hpp/cpp  # Dataset loader & recognizer coordinator
    ├── FaceRecognition/
    │   ├── IFaceRecognizer.hpp # Recognition engine interface
    │   └── LBPHFaceRecognizer.hpp/cpp # OpenCV face recognizer wrapper
    ├── UI/
    │   ├── Common/
    │   │   └── Sidebar.hpp/cpp # Left-side navigation button board
    │   └── Views/
    │       ├── LoginView.hpp/cpp           # Card authentication prompt page
    │       ├── OnboardingView.hpp/cpp      # Greeting guide for empty databases
    │       ├── DashboardView.hpp/cpp       # Analytic metrics and charts overview
    │       ├── StudentManagementView.hpp/cpp# Student grid list and editing popups
    │       ├── FaceRegistrationView.hpp/cpp # Capture preview and train button
    │       ├── AttendanceView.hpp/cpp       # Verification check-in camera preview
    │       ├── ReportsView.hpp/cpp         # History search, filters, and CSV exporter
    │       └── SettingsView.hpp/cpp        # Camera toggles & slider adjustments
    └── Utilities/
        └── Crypto.hpp/cpp      # SHA-256 helper wrapper (uses QCryptographicHash)
```

---

##  Build and Compilation Instructions

### Prerequisites

Make sure the following dependencies are installed and available on your system's PATH:
1. **CMake** (v3.16 or newer)
2. **C++ Compiler** (supporting C++20, e.g., MSVC 2019+, GCC 10+, Clang 11+)
3. **Qt 6 SDK** (including Widgets component)
4. **OpenCV 4.x** (with Face Contrib module for LBPH Recognizer)
5. **SQLite3** library and headers

> [!TIP]
> Dependency packages are easily installed and configured using `vcpkg` on Windows:
> ```bash
> vcpkg install qtbase opencv[contrib] sqlite3
> ```

### Building from Source

To compile the application using command-line tools:

1. Clone the repository and navigate to the directory:
   ```bash
   git clone https://github.com/your-username/Optix.git
   cd Optix
   ```

2. Create a clean build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to generate build configurations:
   - *Standard command*:
     ```bash
     cmake ..
     ```
   - *If using vcpkg toolchain*:
     ```bash
     cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
     ```

4. Build the executable target:
   ```bash
   cmake --build . --config Release
   ```

5. Run the binary:
   - On Windows:
     ```bash
     ./bin/Optix.exe
     ```
   - On Linux/macOS:
     ```bash
     ./bin/Optix
     ```

---

##  Default Credentials

On database initialization, a default administrative account is automatically seeded into the SQLite database for testing purposes:

- **Username**: `admin`
- **Password**: `admin123`

---

##  Roadmap / Sprint Milestone Board

### 🛠️ Sprint 1: Project Scaffolding & Theme System (Completed)
- **Scaffold Workspace**: Structure directory tree separating UI Views, Common Controls, Services, Models, Repositories, Face Recognition algorithms, and Utilities.
- **CMake Environments**: Set up root and source `CMakeLists.txt` finding Qt6, OpenCV, and SQLite3, specifying MSVC/GCC warning suppressions and C++20 options.
- **Theme Manager**: Code the QSS stylesheet template compile engine (`ThemeManager`, `Styles.hpp`) substituting HEX variables dynamically to redraw themes.
- **Base Layout Split**: Program navigation frames (`MainWindow`, `Sidebar`) responding to button checks.

### 👥 Sprint 2: Administrative Login & Student Profile Management (Completed)
- **SQLite Database Manager**: Implement connection managers (`Database.hpp/cpp`) compiling table schemas (`Users`, `Students`, `Attendance`) and seeding default admin accounts.
- **Data Repositories**: Program user repositories using prepared queries (`UserRepository`, `StudentRepository`, `AttendanceRepository`).
- **Business Services**: Program controller services (`AuthService`, `StudentService`) handling password SHA-256 encryptions and emails/phones validation regexes.
- **GUI Views**: Design login card prompts, greeting onboarding widgets, and directory list tables with sliding modals.

### 📸 Sprint 3: OpenCV Video Acquisition & Face Dataset Capture (Upcoming)
- **Asynchronous Camera Threads**: Implement a concurrent `CameraWorker` class using `QThread`/`QObject` to query webcam video frames without blocking GUI repaint events.
- **Haar Cascade Face Detection**: Integrate OpenCV face classifiers to identify regions of interest (ROI) in real-time frame matrices.
- **Automated Dataset Collection**: Write grayscale images directly into local file paths (`datasets/raw/{student_id}/sample_{index}.jpg`) once faces are verified.
- **Progressive Capture HUD**: Display progressive feedback (e.g. `45 / 60 Images`) during capture, prompting the student to move naturally.

### 🎯 Sprint 4: LBPH Recognizer Model Training & Validation (Upcoming)
- **LBPH Integration**: Interface with `cv::face::LBPHFaceRecognizer` from OpenCV Contrib.
- **Dataset Image Processing**: Write algorithms to read `datasets/raw/` directories, convert images, map string IDs to integer labels, and feed vectors.
- **Self-Contained Model Storage**: Save trained models alongside text label-to-ID mapping arrays directly into a single YAML configuration file (`models/lbph_model.yml`).
- **Progress Tracking**: Design modal loaders informing the administrator about structural compilation and ready states.

### ✅ Sprint 5: Face Recognition Attendance Verification (Upcoming)
- **Trained Model Querying**: Load and cache trained LBPH parameters when starting attendance sessions.
- **Prediction Matrix Matching**: Identify scanned faces from camera worker feeds, returning match confidence indexes.
- **Duplicate Prevention Rules**: Query attendance logs to check if the student has already checked in on the active calendar date.
- **HUD Alerts**: Present success checkmarks or duplicate warnings ("Attendance Recorded Successfully" / "Attendance Already Recorded").

### 📊 Sprint 6: Reports Exporting, System Settings & QA Polish (Upcoming)
- **Attendance Query Filters**: Support reports directories sorted by custom calendars, departments, levels, and name keywords.
- **CSV Data Exporter**: Implement spreadsheet writing operations using file streams to write records into CSV formats.
- **System Settings Panel**: Wire dropdown options for active video hardware selections and slider adjusters for confidence thresholds.
- **QA Polish & Animations**: Add subtle frame transition fades, resolve memory cleanups, and run target build validations.


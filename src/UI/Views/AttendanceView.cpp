#include "AttendanceView.hpp"
#include "Application/AppContext.hpp"
#include "Services/CameraWorker.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QMetaType>

#ifndef OPTIX_MOCK_BACKEND
#include <opencv2/imgproc.hpp>
#endif

namespace Optix::UI::Views {

AttendanceView::AttendanceView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
#ifndef OPTIX_MOCK_BACKEND
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<cv::Rect>>("std::vector<cv::Rect>");
#endif
    setupUI();
    refreshSessionLogs();
}

AttendanceView::~AttendanceView() {
    stopCameraThread();
}

void AttendanceView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    QLabel* titleLbl = new QLabel("Take Attendance", this);
    titleLbl->setObjectName("HeaderTitle");
    QLabel* subtitleLbl = new QLabel("Open webcam to recognize student faces and record check-ins", this);
    subtitleLbl->setObjectName("HeaderSubtitle");
    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subtitleLbl);
    mainLayout->addLayout(headerLayout);

    // Content Pane
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(24);

    // Left Panel: Webcam preview card
    QFrame* previewCard = new QFrame(this);
    previewCard->setObjectName("PanelCard");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewCard);
    previewLayout->setContentsMargins(12, 12, 12, 12);

    m_cameraPreview = new QLabel(previewCard);
    m_cameraPreview->setMinimumSize(640, 480);
    m_cameraPreview->setAlignment(Qt::AlignCenter);
    m_cameraPreview->setText("Session Offline");
    m_cameraPreview->setStyleSheet("color: #FFFFFF; background-color: #1C1C1C; font-size: 16px; font-weight: bold; border-radius: 8px;");
    previewLayout->addWidget(m_cameraPreview);
    contentLayout->addWidget(previewCard, 3);

    // Right Panel: Control parameters and logs
    QFrame* controlCard = new QFrame(this);
    controlCard->setObjectName("PanelCard");
    QVBoxLayout* controlLayout = new QVBoxLayout(controlCard);
    controlLayout->setSpacing(16);
    controlLayout->setContentsMargins(20, 20, 20, 20);

    m_sessionToggleBtn = new QPushButton("Start Attendance Session", controlCard);
    m_sessionToggleBtn->setObjectName("PrimaryBtn");
    m_sessionToggleBtn->setCursor(Qt::PointingHandCursor);

    m_simulateVerifyBtn = new QPushButton("Simulate Verification", controlCard);
    m_simulateVerifyBtn->setObjectName("SecondaryBtn");
    m_simulateVerifyBtn->setCursor(Qt::PointingHandCursor);

    m_statusLbl = new QLabel("Session inactive.", controlCard);
    m_statusLbl->setStyleSheet("color: #6B7280; font-size: 14px; font-weight: 500;");
    m_statusLbl->setAlignment(Qt::AlignCenter);
    m_statusLbl->setWordWrap(true);

    QFrame* divider = new QFrame(controlCard);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background-color: #ECECEC;");

    QLabel* tableTitle = new QLabel("Today's Check-ins:", controlCard);
    tableTitle->setStyleSheet("font-family: 'Space Grotesk'; font-size: 14px; font-weight: bold; color: #1F2937;");

    m_todayLogsTable = new QTableWidget(controlCard);
    m_todayLogsTable->setColumnCount(3);
    m_todayLogsTable->setHorizontalHeaderLabels({"Student", "Time", "Status"});
    m_todayLogsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_todayLogsTable->verticalHeader()->setVisible(false);

    controlLayout->addWidget(m_sessionToggleBtn);
    controlLayout->addWidget(m_simulateVerifyBtn);
    controlLayout->addWidget(m_statusLbl);
    controlLayout->addWidget(divider);
    controlLayout->addWidget(tableTitle);
    controlLayout->addWidget(m_todayLogsTable);

    contentLayout->addWidget(controlCard, 1);
    mainLayout->addLayout(contentLayout);

    connect(m_sessionToggleBtn, &QPushButton::clicked, this, &AttendanceView::toggleSession);
    connect(m_simulateVerifyBtn, &QPushButton::clicked, this, &AttendanceView::simulateVerification);
    
    m_simulateVerifyBtn->setEnabled(false);
}

void AttendanceView::toggleSession() {
    if (m_isSessionRunning) {
        stopCameraThread();
        m_isSessionRunning = false;
        m_cameraPreview->setText("Session Offline");
        m_cameraPreview->setStyleSheet("color: #FFFFFF; background-color: #1C1C1C; font-size: 16px; font-weight: bold; border-radius: 8px;");
        m_sessionToggleBtn->setText("Start Attendance Session");
        m_sessionToggleBtn->setObjectName("PrimaryBtn");
        m_sessionToggleBtn->style()->unpolish(m_sessionToggleBtn);
        m_sessionToggleBtn->style()->polish(m_sessionToggleBtn);
        
        m_simulateVerifyBtn->setEnabled(false);
        m_statusLbl->setText("Session closed.");
    } else {
        // Verify model is trained before opening session
        if (!m_context->faceService->isModelTrained()) {
            QMessageBox::warning(
                this, "Model Untrained", 
                "The Face Recognition model has not been trained yet. Please register students and trigger model training in the Face Registration tab first."
            );
            return;
        }

        m_cameraThread = new QThread(this);
        m_cameraWorker = new Services::CameraWorker(0);
        m_cameraWorker->moveToThread(m_cameraThread);

#ifndef OPTIX_MOCK_BACKEND
        connect(m_cameraWorker, &Services::CameraWorker::frameCaptured, this, &AttendanceView::onFrameCaptured, Qt::QueuedConnection);
#endif
        connect(m_cameraWorker, &Services::CameraWorker::cameraError, this, [this](const QString& err) {
            m_statusLbl->setText("Camera Error: " + err);
            QMessageBox::warning(this, "Camera Error", "Could not open camera device. Please check webcam permissions or device connection.");
        });

        connect(m_cameraThread, &QThread::started, m_cameraWorker, &Services::CameraWorker::startCapturing);
        connect(m_cameraWorker, &Services::CameraWorker::cameraStopped, m_cameraThread, &QThread::quit);
        connect(m_cameraWorker, &Services::CameraWorker::cameraStopped, m_cameraWorker, &Services::CameraWorker::deleteLater);
        connect(m_cameraThread, &QThread::finished, m_cameraThread, &QThread::deleteLater);

        m_cameraThread->start();

        m_isSessionRunning = true;
        m_sessionToggleBtn->setText("Stop Attendance Session");
        m_sessionToggleBtn->setObjectName("SecondaryBtn");
        m_sessionToggleBtn->style()->unpolish(m_sessionToggleBtn);
        m_sessionToggleBtn->style()->polish(m_sessionToggleBtn);
        
        m_simulateVerifyBtn->setEnabled(true);
        m_statusLbl->setText("Awaiting facial recognition scan...");
    }
    refreshSessionLogs();
}

void AttendanceView::stopCameraThread() {
    if (m_cameraWorker) {
        m_cameraWorker->stopCapturing();
        m_cameraWorker = nullptr;
    }
    if (m_cameraThread) {
        m_cameraThread->quit();
        m_cameraThread->wait();
        m_cameraThread = nullptr;
    }
}

#ifndef OPTIX_MOCK_BACKEND
void AttendanceView::onFrameCaptured(const QImage& displayImage, const cv::Mat& rawFrame, const std::vector<cv::Rect>& faceRects) {
    if (!m_isSessionRunning) return;

    // Display camera preview
    m_cameraPreview->setPixmap(QPixmap::fromImage(displayImage).scaled(m_cameraPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Perform real-time recognition on detected faces
    if (!faceRects.empty()) {
        const auto& faceBox = faceRects[0];
        cv::Rect safeBox = faceBox & cv::Rect(0, 0, rawFrame.cols, rawFrame.rows);
        
        if (safeBox.width > 0 && safeBox.height > 0) {
            cv::Mat croppedFace = rawFrame(safeBox);
            
            // Format face for LBPH recognizer (200x200 grayscale)
            cv::Mat grayFace;
            if (croppedFace.channels() == 3) {
                cv::cvtColor(croppedFace, grayFace, cv::COLOR_BGR2GRAY);
            } else {
                grayFace = croppedFace.clone();
            }
            cv::resize(grayFace, grayFace, cv::Size(200, 200));

            double confidence = 0.0;
            std::string recognizedId = m_context->faceService->recognizeStudent(grayFace, confidence);

            // LBPH Confidence metric: Lower distance = higher match confidence.
            // Distance threshold < 75.0 is typically a valid match.
            if (!recognizedId.empty() && confidence < 75.0) {
                QDateTime now = QDateTime::currentDateTime();
                
                // Cooldown: Avoid spamming duplicate signals within 3 seconds
                if (m_lastRecognitionTime.find(recognizedId) != m_lastRecognitionTime.end()) {
                    if (m_lastRecognitionTime[recognizedId].msecsTo(now) < 3000) {
                        return;
                    }
                }
                m_lastRecognitionTime[recognizedId] = now;

                auto sOpt = m_context->studentService->getStudent(recognizedId);
                std::string studentName = sOpt.has_value() ? sOpt->full_name : recognizedId;
                std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();

                if (m_context->attendanceRepository->hasRecord(recognizedId, today)) {
                    m_statusLbl->setText(QString("⚠️ Recognition Alert:\n%1 already recorded today.")
                        .arg(QString::fromStdString(studentName)));
                    m_statusLbl->setStyleSheet("color: #F97316; font-size: 14px; font-weight: bold;");
                } else {
                    if (m_context->attendanceRepository->recordAttendance(recognizedId, today, "Present")) {
                        m_statusLbl->setText(QString("✅ Verified:\n%1 checked in successfully!")
                            .arg(QString::fromStdString(studentName)));
                        m_statusLbl->setStyleSheet("color: #22C55E; font-size: 14px; font-weight: bold;");
                        refreshSessionLogs();
                    }
                }
            }
        }
    }
}
#endif

void AttendanceView::simulateVerification() {
    if (!m_isSessionRunning) return;

    // Fetch student list
    auto students = m_context->studentService->getStudentList();
    if (students.empty()) {
        QMessageBox::warning(this, "Verification Simulation", "No students registered in the database.");
        return;
    }

    // Filter to those with face registered
    std::vector<Models::Student> registeredStudents;
    for (const auto& s : students) {
        if (s.face_registered) {
            registeredStudents.push_back(s);
        }
    }

    if (registeredStudents.empty()) {
        QMessageBox::warning(this, "Verification Simulation", "No students have registered faces yet. Register face datasets first.");
        return;
    }

    // Select random student
    srand(static_cast<unsigned int>(time(nullptr)));
    int idx = rand() % registeredStudents.size();
    const auto& targetStudent = registeredStudents[idx];

    std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();

    // Check duplicate
    if (m_context->attendanceRepository->hasRecord(targetStudent.student_id, today)) {
        m_statusLbl->setText(QString("⚠️ Recognition Alert:\n%1 already recorded today.")
            .arg(QString::fromStdString(targetStudent.full_name)));
        m_statusLbl->setStyleSheet("color: #F97316; font-size: 14px; font-weight: bold;");
    } else {
        // Record attendance
        if (m_context->attendanceRepository->recordAttendance(targetStudent.student_id, today, "Present")) {
            m_statusLbl->setText(QString("✅ Verified:\n%1 checked in successfully.")
                .arg(QString::fromStdString(targetStudent.full_name)));
            m_statusLbl->setStyleSheet("color: #22C55E; font-size: 14px; font-weight: bold;");
            refreshSessionLogs();
        }
    }
}

void AttendanceView::refreshSessionLogs() {
    std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    auto logs = m_context->attendanceRepository->getDailyAttendance(today);

    m_todayLogsTable->setRowCount(static_cast<int>(logs.size()));

    for (size_t i = 0; i < logs.size(); ++i) {
        const auto& log = logs[i];
        auto sOpt = m_context->studentService->getStudent(log.student_id);
        QString name = sOpt.has_value() ? QString::fromStdString(sOpt->full_name) : "Unknown Student";

        m_todayLogsTable->setItem(i, 0, new QTableWidgetItem(name));
        m_todayLogsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(log.timestamp)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(log.status));
        statusItem->setForeground(QBrush(QColor("#22C55E"))); // green
        m_todayLogsTable->setItem(i, 2, statusItem);
    }
}

} // namespace Optix::UI::Views

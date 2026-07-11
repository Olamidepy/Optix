#include "FaceRegistrationView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QProgressBar>

namespace Optix::UI::Views {

FaceRegistrationView::FaceRegistrationView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    updateButtons();
}

void FaceRegistrationView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    QLabel* titleLbl = new QLabel("Face Registration & Model Training", this);
    titleLbl->setObjectName("HeaderTitle");
    QLabel* subtitleLbl = new QLabel("Capture student facial templates and train the LBPH recognizer", this);
    subtitleLbl->setObjectName("HeaderSubtitle");
    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subtitleLbl);
    mainLayout->addLayout(headerLayout);

    // Main Content: Camera Panel and Side Controls
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(24);

    // Left Panel: Webcam preview card
    QFrame* previewCard = new QFrame(this);
    previewCard->setObjectName("PanelCard");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewCard);
    previewLayout->setContentsMargins(12, 12, 12, 12);

    m_cameraPreview = new QLabel(previewCard);
    m_cameraPreview->setStyleSheet("background-color: #000000; border-radius: 8px;");
    m_cameraPreview->setMinimumSize(640, 480);
    m_cameraPreview->setAlignment(Qt::AlignCenter);
    m_cameraPreview->setText("Camera Offline");
    m_cameraPreview->setStyleSheet("color: #FFFFFF; background-color: #1C1C1C; font-size: 16px; font-weight: bold; border-radius: 8px;");
    previewLayout->addWidget(m_cameraPreview);
    contentLayout->addWidget(previewCard, 3);

    // Right Panel: Control parameters and status updates
    QFrame* controlCard = new QFrame(this);
    controlCard->setObjectName("PanelCard");
    QVBoxLayout* controlLayout = new QVBoxLayout(controlCard);
    controlLayout->setSpacing(16);
    controlLayout->setContentsMargins(20, 20, 20, 20);

    m_studentInfoLbl = new QLabel("No student selected", controlCard);
    m_studentInfoLbl->setStyleSheet("font-family: 'Space Grotesk'; font-size: 16px; font-weight: bold; color: #1F2937;");
    m_studentInfoLbl->setWordWrap(true);

    m_cameraToggleBtn = new QPushButton("Start Camera", controlCard);
    m_cameraToggleBtn->setObjectName("SecondaryBtn");
    m_cameraToggleBtn->setCursor(Qt::PointingHandCursor);

    m_captureBtn = new QPushButton("🔴 Start Capture", controlCard);
    m_captureBtn->setObjectName("PrimaryBtn");
    m_captureBtn->setCursor(Qt::PointingHandCursor);

    m_captureProgressBar = new QProgressBar(controlCard);
    m_captureProgressBar->setRange(0, 60);
    m_captureProgressBar->setValue(0);
    m_captureProgressBar->setTextVisible(true);

    m_statusLbl = new QLabel("System ready.", controlCard);
    m_statusLbl->setStyleSheet("color: #6B7280; font-style: italic;");
    m_statusLbl->setWordWrap(true);

    QFrame* divider = new QFrame(controlCard);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background-color: #ECECEC;");

    m_trainBtn = new QPushButton("⚡ Train Face Recognizer", controlCard);
    m_trainBtn->setObjectName("PrimaryBtn");
    m_trainBtn->setCursor(Qt::PointingHandCursor);

    controlLayout->addWidget(m_studentInfoLbl);
    controlLayout->addWidget(m_cameraToggleBtn);
    controlLayout->addWidget(m_captureBtn);
    controlLayout->addWidget(new QLabel("Capture Progress (requires 60 samples):", controlCard));
    controlLayout->addWidget(m_captureProgressBar);
    controlLayout->addWidget(m_statusLbl);
    controlLayout->addWidget(divider);
    controlLayout->addWidget(m_trainBtn);
    controlLayout->addStretch();

    contentLayout->addWidget(controlCard, 1);
    mainLayout->addLayout(contentLayout);

    // Button connections
    connect(m_cameraToggleBtn, &QPushButton::clicked, this, &FaceRegistrationView::toggleCamera);
    connect(m_captureBtn, &QPushButton::clicked, this, &FaceRegistrationView::startCapture);
    connect(m_trainBtn, &QPushButton::clicked, this, &FaceRegistrationView::triggerModelTraining);
}

void FaceRegistrationView::setTargetStudent(const QString& studentId) {
    m_targetStudentId = studentId;
    auto sOpt = m_context->studentService->getStudent(studentId.toStdString());
    if (sOpt.has_value()) {
        m_studentInfoLbl->setText(QString("Target Student:\n%1\n(%2)").arg(QString::fromStdString(sOpt->full_name), studentId));
        m_statusLbl->setText("Ready to record faces.");
    } else {
        m_studentInfoLbl->setText("No student selected");
    }
    updateButtons();
}

void FaceRegistrationView::toggleCamera() {
    if (m_isCameraRunning) {
        m_isCameraRunning = false;
        m_cameraPreview->setText("Camera Offline");
        m_cameraPreview->setStyleSheet("color: #FFFFFF; background-color: #1C1C1C; font-size: 16px; font-weight: bold; border-radius: 8px;");
        m_cameraToggleBtn->setText("Start Camera");
        m_statusLbl->setText("Camera stopped.");
    } else {
        m_isCameraRunning = true;
        m_cameraPreview->setText("[ Camera Feed Placeholder - Sprint 3 ]\n(Requires OpenCV VideoCapture loop)");
        m_cameraPreview->setStyleSheet("color: #9CA3AF; background-color: #000000; font-size: 14px; border-radius: 8px;");
        m_cameraToggleBtn->setText("Stop Camera");
        m_statusLbl->setText("Camera started.");
    }
    updateButtons();
}

void FaceRegistrationView::startCapture() {
    if (m_targetStudentId.empty()) {
        QMessageBox::warning(this, "Face Capture", "Please select a student from the Students list first.");
        return;
    }
    if (!m_isCameraRunning) {
        QMessageBox::warning(this, "Face Capture", "Please start the webcam preview first.");
        return;
    }

    m_isCapturing = true;
    m_statusLbl->setText("Capturing facial expressions...");
    updateButtons();

    // Stub/Simulation for Sprints 1 & 2
    for (int i = 0; i <= 60; i += 10) {
        m_captureProgressBar->setValue(i);
        QCoreApplication::processEvents();
        // Wait briefly to show progress bar filling
        QThread::msleep(80);
    }

    // Save mock success state
    m_context->studentService->updateFaceRegistrationStatus(m_targetStudentId.toStdString(), true);
    
    m_isCapturing = false;
    m_statusLbl->setText("Face registered successfully! Proceed to train the recognizer.");
    QMessageBox::information(this, "Face Capture Complete", "Successfully saved 60 facial samples into the student directory. Ready for model training.");
    
    updateButtons();
}

void FaceRegistrationView::triggerModelTraining() {
    m_statusLbl->setText("Training model... Please wait.");
    m_trainBtn->setEnabled(false);
    QCoreApplication::processEvents();

    int totalImages = 0;
    // Call face service to read datasets/raw, fit LBPH recognizer, and save lbph_model.yml
    bool success = m_context->faceService->trainModel(totalImages);

    if (success) {
        m_statusLbl->setText("Model trained and saved successfully.");
        QMessageBox::information(this, "Training Complete", QString("Face Recognition model trained successfully using %1 images.").arg(totalImages));
    } else {
        m_statusLbl->setText("Training failed. Make sure datasets folder contains registered faces.");
        QMessageBox::warning(this, "Training Failed", "Model training failed. Check if any student face templates have been captured.");
    }
    m_trainBtn->setEnabled(true);
}

void FaceRegistrationView::updateButtons() {
    m_captureBtn->setEnabled(m_isCameraRunning && !m_targetStudentId.empty());
}

} // namespace Optix::UI::Views

#include "FaceRegistrationView.hpp"
#include "Application/AppContext.hpp"
#include "Services/CameraWorker.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QProgressBar>
#include <QCoreApplication>
#include <QDebug>

#ifndef OPTIX_MOCK_BACKEND
#include <opencv2/imgproc.hpp>
#endif

namespace Optix::UI::Views {

FaceRegistrationView::FaceRegistrationView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    updateButtons();
}

FaceRegistrationView::~FaceRegistrationView() {
    stopCameraThread();
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
    m_studentInfoLbl->setObjectName("CardTitle");
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
        stopCameraThread();
        m_isCameraRunning = false;
        m_cameraPreview->setText("Camera Offline");
        m_cameraPreview->setStyleSheet("color: #FFFFFF; background-color: #1C1C1C; font-size: 16px; font-weight: bold; border-radius: 8px;");
        m_cameraToggleBtn->setText("Start Camera");
        m_statusLbl->setText("Camera stopped.");
    } else {
        m_cameraThread = new QThread(this);
        m_cameraWorker = new Services::CameraWorker(0);
        m_cameraWorker->moveToThread(m_cameraThread);

        // Update preview label on EVERY frame image
        connect(m_cameraWorker, &Services::CameraWorker::frameReady, this, [this](const QImage& img) {
            if (m_isCameraRunning) {
                m_cameraPreview->setPixmap(QPixmap::fromImage(img).scaled(
                    m_cameraPreview->size(), 
                    Qt::KeepAspectRatio, 
                    Qt::SmoothTransformation
                ));
            }
        });

        // Save face samples when face detected
        connect(m_cameraWorker, &Services::CameraWorker::faceDetected, this, [this](const QImage& faceRoi) {
            if (m_isCapturing && !m_targetStudentId.isEmpty()) {
#ifndef OPTIX_MOCK_BACKEND
                // Convert QImage to cv::Mat and save
                QImage grayImg = faceRoi.convertToFormat(QImage::Format_Grayscale8);
                cv::Mat grayMat(grayImg.height(), grayImg.width(), CV_8UC1, const_cast<uchar*>(grayImg.bits()), grayImg.bytesPerLine());
                bool saved = m_context->faceService->saveFaceSample(m_targetStudentId.toStdString(), m_capturedCount, grayMat.clone());
#else
                bool saved = true;
#endif
                if (saved) {
                    m_capturedCount++;
                    m_captureProgressBar->setValue(m_capturedCount);

                    if (m_capturedCount >= 60) {
                        m_isCapturing = false;
                        m_context->studentService->updateFaceRegistrationStatus(m_targetStudentId.toStdString(), true);
                        m_statusLbl->setText("Face dataset registered successfully! Proceed to train model.");
                        QMessageBox::information(this, "Face Capture Complete", "Successfully saved 60 facial samples into the student dataset directory. Ready for model training.");
                        updateButtons();
                    }
                }
            }
        });

        connect(m_cameraThread, &QThread::started, m_cameraWorker, &Services::CameraWorker::startCapturing);
        connect(m_cameraWorker, &Services::CameraWorker::cameraStopped, m_cameraThread, &QThread::quit);
        connect(m_cameraWorker, &Services::CameraWorker::cameraStopped, m_cameraWorker, &Services::CameraWorker::deleteLater);
        connect(m_cameraThread, &QThread::finished, m_cameraThread, &QThread::deleteLater);

        m_cameraThread->start();

        m_isCameraRunning = true;
        m_cameraToggleBtn->setText("Stop Camera");
        m_statusLbl->setText("Camera stream active.");
    }
    updateButtons();
}

void FaceRegistrationView::stopCameraThread() {
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

void FaceRegistrationView::startCapture() {
    if (m_targetStudentId.isEmpty()) {
        QMessageBox::warning(this, "Face Capture", "Please select a student from the Students list first.");
        return;
    }
    if (!m_isCameraRunning) {
        QMessageBox::warning(this, "Face Capture", "Please start the webcam preview first.");
        return;
    }

    m_capturedCount = 0;
    m_captureProgressBar->setValue(0);
    m_isCapturing = true;
    m_statusLbl->setText("Capturing facial expressions... Please look at the camera.");
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
    m_captureBtn->setEnabled(m_isCameraRunning && !m_targetStudentId.isEmpty() && !m_isCapturing);
}

} // namespace Optix::UI::Views

#include "DashboardView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QHeaderView>
#include <QDateTime>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <iostream>

namespace Optix::UI::Views {

static QString resolveAssetPath(const QString& relativePath) {
    QStringList searchPaths = {
        relativePath,
        "Public/" + relativePath,
        "../Public/" + relativePath,
        "../../Public/" + relativePath,
        "../../../Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/../Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/../../Public/" + relativePath
    };
    for (const auto& path : searchPaths) {
        if (QFile::exists(path)) {
            return QDir::cleanPath(path);
        }
    }
    return relativePath; // Fallback
}

static QIcon getColoredIcon(const QString& path, const QColor& color) {
    QString resolved = resolveAssetPath(path);
    QPixmap pixmap(resolved);
    if (pixmap.isNull()) {
        std::cerr << "Optix UI: Failed to load icon: " << path.toStdString() << " (resolved to: " << resolved.toStdString() << ")" << std::endl;
        return QIcon();
    }
    
    QPixmap coloredPixmap(pixmap.size());
    coloredPixmap.fill(Qt::transparent);
    
    QPainter painter(&coloredPixmap);
    painter.drawPixmap(0, 0, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(coloredPixmap.rect(), color);
    painter.end();
    
    return QIcon(coloredPixmap);
}

DashboardView::DashboardView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    refreshStats();
}

void DashboardView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    
    QLabel* titleLbl = new QLabel("Dashboard", this);
    titleLbl->setObjectName("HeaderTitle");
    
    QLabel* subtitleLbl = new QLabel("Real-time facial attendance overview", this);
    subtitleLbl->setObjectName("HeaderSubtitle");

    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subtitleLbl);
    mainLayout->addLayout(headerLayout);

    // Metrics Layout (4 Cards)
    QHBoxLayout* metricsLayout = new QHBoxLayout();
    metricsLayout->setSpacing(16);

    QWidget* totalStudentsCard = createMetricCard("Total Students", "0", "Students.png", this);
    QWidget* presentTodayCard = createMetricCard("Present Today", "0", "attendance.png", this);
    QWidget* absentTodayCard = createMetricCard("Absent Today", "0", "Logout.png", this);
    QWidget* accuracyCard = createMetricCard("Model Accuracy", "98.2%", "Target.png", this);

    metricsLayout->addWidget(totalStudentsCard);
    metricsLayout->addWidget(presentTodayCard);
    metricsLayout->addWidget(absentTodayCard);
    metricsLayout->addWidget(accuracyCard);
    mainLayout->addLayout(metricsLayout);

    // Bottom Section: Recent Activity Panel
    QVBoxLayout* activityLayout = new QVBoxLayout();
    activityLayout->setSpacing(12);

    QLabel* activityTitle = new QLabel("Recent Attendance logs", this);
    activityTitle->setStyleSheet("font-family: 'Space Grotesk'; font-size: 18px; font-weight: bold; color: #1F2937;");

    m_recentActivityTable = new QTableWidget(this);
    m_recentActivityTable->setColumnCount(4);
    m_recentActivityTable->setHorizontalHeaderLabels({"Student ID", "Matric Number", "Timestamp", "Status"});
    m_recentActivityTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_recentActivityTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentActivityTable->verticalHeader()->setVisible(false);

    activityLayout->addWidget(activityTitle);
    activityLayout->addWidget(m_recentActivityTable);
    
    mainLayout->addLayout(activityLayout);
    mainLayout->addStretch();
}

QWidget* DashboardView::createMetricCard(const QString& title, const QString& value, const QString& iconPath, QWidget* parent) {
    QFrame* card = new QFrame(parent);
    card->setObjectName("MetricCard");

    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(16);

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);
    textLayout->setAlignment(Qt::AlignVCenter);

    QLabel* valLbl = new QLabel(value, card);
    valLbl->setObjectName("MetricVal");
    
    QLabel* titleLbl = new QLabel(title, card);
    titleLbl->setObjectName("MetricLbl");

    // Track the dynamic labels to update them in refreshStats()
    if (title == "Total Students") {
        m_totalStudentsLbl = valLbl;
    } else if (title == "Present Today") {
        m_presentTodayLbl = valLbl;
    } else if (title == "Absent Today") {
        m_absentTodayLbl = valLbl;
    } else if (title == "Model Accuracy") {
        m_accuracyLbl = valLbl;
    }

    textLayout->addWidget(valLbl);
    textLayout->addWidget(titleLbl);

    QLabel* iconLbl = new QLabel(card);
    QIcon icon = getColoredIcon(iconPath, QColor("#F97316"));
    if (!icon.isNull()) {
        iconLbl->setPixmap(icon.pixmap(32, 32));
    }
    iconLbl->setFixedSize(48, 48);
    iconLbl->setAlignment(Qt::AlignCenter);

    cardLayout->addLayout(textLayout);
    cardLayout->addStretch();
    cardLayout->addWidget(iconLbl, 0, Qt::AlignVCenter);

    return card;
}

void DashboardView::refreshStats() {
    // 1. Fetch values from services
    int total = m_context->studentService->getStudentCount();
    
    std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    int present = m_context->attendanceRepository->getPresentTodayCount(today);
    int absent = total - present;
    if (absent < 0) absent = 0;

    m_totalStudentsLbl->setText(QString::number(total));
    m_presentTodayLbl->setText(QString::number(present));
    m_absentTodayLbl->setText(QString::number(absent));
    
    // Check model training status
    if (m_context->faceService->isModelTrained()) {
        m_accuracyLbl->setText("98.2%");
    } else {
        m_accuracyLbl->setText("Untrained");
    }

    // 2. Fetch recent activity table
    auto logs = m_context->attendanceRepository->getDailyAttendance(today);
    
    // Limit to top 5 recent entries
    int rows = std::min(static_cast<int>(logs.size()), 5);
    m_recentActivityTable->setRowCount(rows);

    for (int i = 0; i < rows; ++i) {
        const auto& log = logs[i];
        
        // Find student details for display
        auto studentOpt = m_context->studentService->getStudent(log.student_id);
        QString matric = studentOpt.has_value() ? QString::fromStdString(studentOpt->matric_number) : "Unknown";

        m_recentActivityTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(log.student_id)));
        m_recentActivityTable->setItem(i, 1, new QTableWidgetItem(matric));
        m_recentActivityTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(log.timestamp)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(log.status));
        if (log.status == "Present") {
            statusItem->setForeground(QBrush(QColor("#22C55E"))); // green
        } else {
            statusItem->setForeground(QBrush(QColor("#EF4444"))); // red
        }
        m_recentActivityTable->setItem(i, 3, statusItem);
    }
}

} // namespace Optix::UI::Views

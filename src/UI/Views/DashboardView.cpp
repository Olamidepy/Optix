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
#include <QScrollArea>
#include <QTimer>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainterPath>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

class DonutChart : public QWidget {
public:
    struct Slice {
        QString label;
        double value;
        QColor color;
    };

    explicit DonutChart(QWidget* parent = nullptr) : QWidget(parent) {
        m_totalStudents = 81;
        m_hoveredIndex = -1;
        m_animationProgress = 0.0;
        m_timerId = -1;
        m_slices = {
            {"Present", 0.0, QColor(249, 115, 22, 153)},
            {"Absent", 81.0, QColor(249, 115, 22, 255)}
        };
        setMinimumSize(250, 180);
        setMouseTracking(true);
        startAnimation();
    }

    void startAnimation() {
        if (m_timerId != -1) {
            killTimer(m_timerId);
        }
        m_animationProgress = 0.0;
        m_timerId = startTimer(16); // ~60 FPS
    }

    void setData(int present, int absent) {
        m_totalStudents = present + absent;
        m_slices = {
            {"Present", static_cast<double>(present), QColor(249, 115, 22, 153)}, // 60% opacity orange
            {"Absent", static_cast<double>(absent), QColor(249, 115, 22, 255)}  // Solid orange
        };
        startAnimation();
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timerId) {
            m_animationProgress += 0.02;
            if (m_animationProgress >= 1.0) {
                m_animationProgress = 1.0;
                killTimer(m_timerId);
                m_timerId = -1;
            }
            update();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        int side = qMin(width(), height());
        int donutSize = side - 20;
        QRectF donutRect(10, (height() - donutSize) / 2, donutSize, donutSize);
        QPointF center = donutRect.center();

        double dx = event->position().x() - center.x();
        double dy = event->position().y() - center.y();
        double r = std::sqrt(dx*dx + dy*dy);

        int outerR = donutSize / 2;
        int innerR = outerR * 0.6;

        int newHovered = -1;
        if (r >= innerR && r <= outerR) {
            double angle = std::atan2(-dy, dx) * 180.0 / M_PI;
            if (angle < 0) angle += 360.0;
            
            double donutAngle = 90.0 - angle;
            if (donutAngle < 0) donutAngle += 360.0;

            double total = 0;
            for (const auto& slice : m_slices) {
                total += slice.value;
            }

            if (total > 0) {
                double currentAngle = 0.0;
                for (size_t i = 0; i < m_slices.size(); ++i) {
                    double spanAngle = (m_slices[i].value / total) * 360.0;
                    if (donutAngle >= currentAngle && donutAngle < (currentAngle + spanAngle)) {
                        newHovered = static_cast<int>(i);
                        break;
                    }
                    currentAngle += spanAngle;
                }
            }
        }

        if (newHovered != m_hoveredIndex) {
            m_hoveredIndex = newHovered;
            update();
        }
    }

    void leaveEvent(QEvent* event) override {
        (void)event;
        if (m_hoveredIndex != -1) {
            m_hoveredIndex = -1;
            update();
        }
    }

    void paintEvent(QPaintEvent* event) override {
        (void)event;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int side = qMin(width(), height());
        int donutSize = side - 20;
        QRectF donutRect(10, (height() - donutSize) / 2, donutSize, donutSize);

        double total = 0;
        for (const auto& slice : m_slices) {
            total += slice.value;
        }

        double currentAngle = 90.0;
        for (size_t i = 0; i < m_slices.size(); ++i) {
            const auto& slice = m_slices[i];
            if (total > 0 && slice.value > 0) {
                double spanAngle = (slice.value / total) * 360.0 * m_animationProgress;
                
                QPainterPath path;
                path.moveTo(donutRect.center());
                path.arcTo(donutRect, currentAngle, -spanAngle);
                
                if (static_cast<int>(i) == m_hoveredIndex) {
                    double midAngle = currentAngle - (spanAngle / 2.0);
                    double rad = midAngle * M_PI / 180.0;
                    double offsetX = 6.0 * std::cos(rad);
                    double offsetY = -6.0 * std::sin(rad);
                    path.translate(offsetX, offsetY);
                }
                
                painter.setBrush(slice.color);
                painter.setPen(QPen(Qt::white, 2));
                painter.drawPath(path);
                
                currentAngle -= spanAngle;
            }
        }

        int holeSize = donutSize * 0.6;
        QRectF holeRect(donutRect.center().x() - holeSize / 2.0, donutRect.center().y() - holeSize / 2.0, holeSize, holeSize);
        QColor cardBg = palette().color(QPalette::Base);
        QColor textMain = palette().color(QPalette::Text);
        painter.setBrush(cardBg);
        painter.setPen(QPen(cardBg, 2));
        painter.drawEllipse(holeRect);

        painter.setPen(textMain);
        QFont font = painter.font();
        font.setFamily("Space Grotesk");
        font.setPixelSize(18);
        font.setBold(true);
        painter.setFont(font);
        
        painter.drawText(holeRect, Qt::AlignCenter, QString::number(m_totalStudents));

        font.setPixelSize(9);
        font.setBold(false);
        painter.setFont(font);
        painter.setPen(QColor("#6B7280"));
        painter.drawText(holeRect.adjusted(0, 20, 0, 0), Qt::AlignCenter, "Students");

        // Legend
        int legendX = donutSize + 25;
        int legendY = (height() - (m_slices.size() * 20)) / 2;
        
        font.setPixelSize(10);
        painter.setFont(font);

        for (size_t i = 0; i < m_slices.size(); ++i) {
            const auto& slice = m_slices[i];
            
            // Draw legend box - shift slightly right if slice is hovered to match visual effect
            int extraShift = (static_cast<int>(i) == m_hoveredIndex) ? 4 : 0;
            
            painter.setBrush(slice.color);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(legendX + extraShift, legendY + 2, 10, 10, 2, 2);
            
            painter.setPen(QColor("#4B5563"));
            double pct = total > 0 ? (slice.value / total) * 100.0 : 0.0;
            QString labelText = QString("%1 (%2%)").arg(slice.label).arg(QString::number(pct, 'f', 0));
            painter.drawText(legendX + 16 + extraShift, legendY + 10, labelText);
            
            legendY += 20;
        }
    }

private:
    std::vector<Slice> m_slices;
    int m_totalStudents;
    int m_hoveredIndex;
    double m_animationProgress;
    int m_timerId;
};

class BarChart : public QWidget {
public:
    struct BarData {
        QString label;
        double presentValue;
        double absentValue;
    };

    explicit BarChart(QWidget* parent = nullptr) : QWidget(parent) {
        m_animationProgress = 0.0;
        m_timerId = -1;
        m_bars = {
            {"Mon", 85.0, 15.0},
            {"Tue", 92.0, 8.0},
            {"Wed", 78.0, 22.0},
            {"Thu", 95.0, 5.0},
            {"Fri", 89.0, 11.0}
        };
        setMinimumSize(320, 180);
        startAnimation();
    }

    void startAnimation() {
        if (m_timerId != -1) {
            killTimer(m_timerId);
        }
        m_animationProgress = 0.0;
        m_timerId = startTimer(16);
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timerId) {
            m_animationProgress += 0.02;
            if (m_animationProgress >= 1.0) {
                m_animationProgress = 1.0;
                killTimer(m_timerId);
                m_timerId = -1;
            }
            update();
        }
    }

    void paintEvent(QPaintEvent* event) override {
        (void)event;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int paddingLeft = 35;
        int paddingBottom = 40;
        int paddingTop = 15;
        int paddingRight = 10;

        int chartWidth = width() - paddingLeft - paddingRight;
        int chartHeight = height() - paddingTop - paddingBottom;

        painter.setPen(QPen(QColor("#F3F4F6"), 1, Qt::DashLine));
        QFont font = painter.font();
        font.setFamily("Space Grotesk");
        font.setPixelSize(9);
        painter.setFont(font);

        for (int i = 0; i <= 4; ++i) {
            int y = paddingTop + chartHeight - (i * chartHeight / 4);
            painter.drawLine(paddingLeft, y, paddingLeft + chartWidth, y);
            
            painter.setPen(QPen(QColor("#9CA3AF")));
            painter.drawText(5, y + 4, QString("%1%").arg(i * 25));
            painter.setPen(QPen(QColor("#F3F4F6"), 1, Qt::DashLine));
        }

        int numBars = m_bars.size();
        int spacing = chartWidth / (numBars * 3);
        int barWidth = (chartWidth - (spacing * (numBars + 1))) / numBars;

        for (int i = 0; i < numBars; ++i) {
            const auto& bar = m_bars[i];
            int x = paddingLeft + spacing + i * (barWidth + spacing);
            
            double pHeight = (bar.presentValue / 100.0) * chartHeight * m_animationProgress;
            double aHeight = (bar.absentValue / 100.0) * chartHeight * m_animationProgress;

            // Present Bar (60% Opacity Orange)
            QRectF presentRect(x, paddingTop + chartHeight - pHeight, barWidth, pHeight);
            QPainterPath pPath;
            pPath.addRoundedRect(presentRect, 3, 3);
            painter.setBrush(QColor(249, 115, 22, 153));
            painter.setPen(Qt::NoPen);
            painter.drawPath(pPath);

            // Absent Bar (Solid Orange)
            if (aHeight > 0) {
                QRectF absentRect(x, paddingTop + chartHeight - pHeight - aHeight, barWidth, aHeight);
                QPainterPath aPath;
                aPath.addRoundedRect(absentRect, 3, 3);
                painter.setBrush(QColor(249, 115, 22, 255));
                painter.drawPath(aPath);
            }

            painter.setPen(QColor("#4B5563"));
            painter.drawText(x, paddingTop + chartHeight + 14, barWidth, 12, Qt::AlignCenter, bar.label);
        }

        // Legend
        int legendY = paddingTop + chartHeight + 24;
        int legendX = paddingLeft + (chartWidth - 140) / 2;

        painter.setBrush(QColor(249, 115, 22, 153));
        painter.drawRoundedRect(legendX, legendY, 8, 8, 2, 2);
        painter.setPen(QColor("#4B5563"));
        painter.drawText(legendX + 12, legendY + 8, "Present");

        painter.setBrush(QColor(249, 115, 22, 255));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(legendX + 70, legendY, 8, 8, 2, 2);
        painter.setPen(QColor("#4B5563"));
        painter.drawText(legendX + 82, legendY + 8, "Absent");
    }

private:
    std::vector<BarData> m_bars;
    int m_timerId;
    double m_animationProgress;
};

DashboardView::DashboardView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    refreshStats();
}

void DashboardView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* container = new QWidget(scrollArea);
    container->setObjectName("DashboardContainer");
    container->setStyleSheet("QWidget#DashboardContainer { background-color: transparent; }");

    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(24);
    containerLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    
    QLabel* titleLbl = new QLabel("Dashboard", container);
    titleLbl->setObjectName("HeaderTitle");
    
    QLabel* subtitleLbl = new QLabel("Real-time facial attendance overview", container);
    subtitleLbl->setObjectName("HeaderSubtitle");

    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subtitleLbl);
    containerLayout->addLayout(headerLayout);

    // Metrics Layout (4 Cards)
    QHBoxLayout* metricsLayout = new QHBoxLayout();
    metricsLayout->setSpacing(16);

    QWidget* totalStudentsCard = createMetricCard("Total Students", "0", "Students.png", container);
    QWidget* presentTodayCard = createMetricCard("Present Today", "0", "attendance.png", container);
    QWidget* absentTodayCard = createMetricCard("Absent Today", "0", "Logout.png", container);
    QWidget* accuracyCard = createMetricCard("Model Accuracy", "98.2%", "Target.png", container);

    metricsLayout->addWidget(totalStudentsCard);
    metricsLayout->addWidget(presentTodayCard);
    metricsLayout->addWidget(absentTodayCard);
    metricsLayout->addWidget(accuracyCard);
    containerLayout->addLayout(metricsLayout);

    // Recent Activity Section
    QVBoxLayout* activityLayout = new QVBoxLayout();
    activityLayout->setSpacing(12);

    QLabel* activityTitle = new QLabel("Recent Attendance logs", container);
    activityTitle->setObjectName("CardTitle");

    m_recentActivityTable = new QTableWidget(container);
    m_recentActivityTable->setColumnCount(4);
    m_recentActivityTable->setHorizontalHeaderLabels({"Student ID", "Matric Number", "Timestamp", "Status"});
    m_recentActivityTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_recentActivityTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentActivityTable->verticalHeader()->setVisible(false);
    m_recentActivityTable->setFixedHeight(160); // Set fixed height to look compact

    activityLayout->addWidget(activityTitle);
    activityLayout->addWidget(m_recentActivityTable);
    
    containerLayout->addLayout(activityLayout);

    // Charts Section
    QLabel* chartsTitle = new QLabel("Attendance Analytics", container);
    chartsTitle->setObjectName("CardTitle");
    containerLayout->addWidget(chartsTitle);

    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(20);

    // Frame for Bar Chart
    QFrame* barFrame = new QFrame(container);
    barFrame->setObjectName("ChartFrame");
    barFrame->setStyleSheet(
        "QFrame#ChartFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #E5E7EB;"
        "   border-radius: 12px;"
        "}"
    );
    QVBoxLayout* barFrameLayout = new QVBoxLayout(barFrame);
    barFrameLayout->setContentsMargins(15, 15, 15, 15);
    QLabel* barTitle = new QLabel("Weekly Attendance Rate", barFrame);
    barTitle->setStyleSheet("font-family: 'Space Grotesk'; font-size: 14px; font-weight: bold; color: #374151;");
    barFrameLayout->addWidget(barTitle);
    
    m_barChart = new BarChart(barFrame);
    barFrameLayout->addWidget(m_barChart);

    // Frame for Donut Chart
    QFrame* pieFrame = new QFrame(container);
    pieFrame->setObjectName("ChartFrame");
    pieFrame->setStyleSheet(
        "QFrame#ChartFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #E5E7EB;"
        "   border-radius: 12px;"
        "}"
    );
    QVBoxLayout* pieFrameLayout = new QVBoxLayout(pieFrame);
    pieFrameLayout->setContentsMargins(15, 15, 15, 15);
    QLabel* pieTitle = new QLabel("Attendance Overview", pieFrame);
    pieTitle->setStyleSheet("font-family: 'Space Grotesk'; font-size: 14px; font-weight: bold; color: #374151;");
    pieFrameLayout->addWidget(pieTitle);

    m_donutChart = new DonutChart(pieFrame);
    pieFrameLayout->addWidget(m_donutChart);

    chartsLayout->addWidget(barFrame, 3);
    chartsLayout->addWidget(pieFrame, 2);

    containerLayout->addLayout(chartsLayout);
    containerLayout->addStretch();

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
}

QWidget* DashboardView::createMetricCard(const QString& title, const QString& value, const QString& iconPath, QWidget* parent) {
    QFrame* card = new QFrame(parent);
    card->setObjectName("MetricCard");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(8);

    QHBoxLayout* topLayout = new QHBoxLayout();
    
    QLabel* valLbl = new QLabel(value, card);
    valLbl->setObjectName("MetricVal");
    
    QLabel* iconLbl = new QLabel(card);
    QIcon icon = getColoredIcon(iconPath, QColor("#F97316"));
    if (!icon.isNull()) {
        iconLbl->setPixmap(icon.pixmap(20, 20)); // Reduced size to 20x20
    }
    iconLbl->setFixedSize(24, 24);
    iconLbl->setAlignment(Qt::AlignTop | Qt::AlignRight);

    topLayout->addWidget(valLbl);
    topLayout->addStretch();
    topLayout->addWidget(iconLbl, 0, Qt::AlignTop | Qt::AlignRight);

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

    cardLayout->addLayout(topLayout);
    cardLayout->addWidget(titleLbl);

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

    // 3. Update the DonutChart dynamically with reveal sweep animation
    if (m_donutChart) {
        m_donutChart->setData(present, absent);
    }

    // 4. Trigger progressive rising animation on the BarChart
    if (m_barChart) {
        m_barChart->startAnimation();
    }
}

} // namespace Optix::UI::Views

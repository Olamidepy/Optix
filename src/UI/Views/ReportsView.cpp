#include "ReportsView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QTextStream>
#include <QFile>

namespace Optix::UI::Views {

ReportsView::ReportsView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    refreshReport();
}

void ReportsView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QHBoxLayout* titleLayout = new QHBoxLayout();
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);
    
    QLabel* titleLbl = new QLabel("Attendance Reports", this);
    titleLbl->setObjectName("HeaderTitle");
    QLabel* subtitleLbl = new QLabel("Review history logs and export data to spreadsheet reports", this);
    subtitleLbl->setObjectName("HeaderSubtitle");
    
    textLayout->addWidget(titleLbl);
    textLayout->addWidget(subtitleLbl);
    titleLayout->addLayout(textLayout);

    m_exportBtn = new QPushButton("📥 Export CSV", this);
    m_exportBtn->setObjectName("PrimaryBtn");
    m_exportBtn->setCursor(Qt::PointingHandCursor);
    titleLayout->addWidget(m_exportBtn);
    
    mainLayout->addLayout(titleLayout);

    // Filters Card
    QFrame* filterCard = new QFrame(this);
    filterCard->setObjectName("PanelCard");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(16, 12, 16, 12);
    filterLayout->setSpacing(16);

    m_searchEdit = new QLineEdit(filterCard);
    m_searchEdit->setPlaceholderText("Search name or student ID...");

    m_deptFilterCombo = new QComboBox(filterCard);
    m_deptFilterCombo->addItem("All Departments");

    m_dateFilterEdit = new QLineEdit(filterCard);
    m_dateFilterEdit->setPlaceholderText("e.g. YYYY-MM-DD");
    // Default to today's date format
    m_dateFilterEdit->setText(QDate::currentDate().toString("yyyy-MM-dd"));

    filterLayout->addWidget(new QLabel("Filter:", filterCard));
    filterLayout->addWidget(m_searchEdit, 2);
    filterLayout->addWidget(new QLabel("Dept:", filterCard));
    filterLayout->addWidget(m_deptFilterCombo, 1);
    filterLayout->addWidget(new QLabel("Date:", filterCard));
    filterLayout->addWidget(m_dateFilterEdit, 1);

    mainLayout->addWidget(filterCard);

    // Reports Table
    m_reportTable = new QTableWidget(this);
    m_reportTable->setColumnCount(7);
    m_reportTable->setHorizontalHeaderLabels({"Log ID", "Student ID", "Full Name", "Department", "Date", "Timestamp", "Status"});
    m_reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_reportTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_reportTable);

    // Connections
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ReportsView::handleFilter);
    connect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &ReportsView::handleFilter);
    connect(m_dateFilterEdit, &QLineEdit::textChanged, this, &ReportsView::handleFilter);
    connect(m_exportBtn, &QPushButton::clicked, this, &ReportsView::handleExportCSV);
}

void ReportsView::refreshReport() {
    // 1. Fetch current filters
    std::string searchQuery = m_searchEdit->text().trimmed().toStdString();
    std::string deptQuery = m_deptFilterCombo->currentIndex() > 0 ? m_deptFilterCombo->currentText().toStdString() : "";
    std::string dateQuery = m_dateFilterEdit->text().trimmed().toStdString();

    // 2. Fetch all logs matching date if provided, else all logs
    std::vector<Models::Attendance> logs;
    if (!dateQuery.empty()) {
        logs = m_context->attendanceRepository->getDailyAttendance(dateQuery);
    } else {
        logs = m_context->attendanceRepository->getAllAttendance();
    }

    // 3. Filter list programmatically based on search names or departments
    std::vector<std::pair<Models::Attendance, Models::Student>> filteredLogs;
    for (const auto& log : logs) {
        auto sOpt = m_context->studentService->getStudent(log.student_id);
        if (!sOpt.has_value()) continue;
        
        const auto& s = sOpt.value();

        // Match search query (names, matric or id)
        if (!searchQuery.empty()) {
            std::string nameLower = s.full_name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string matchQuery = searchQuery;
            std::transform(matchQuery.begin(), matchQuery.end(), matchQuery.begin(), ::tolower);

            if (nameLower.find(matchQuery) == std::string::npos &&
                s.student_id.find(searchQuery) == std::string::npos &&
                s.matric_number.find(searchQuery) == std::string::npos) {
                continue;
            }
        }

        // Match department
        if (!deptQuery.empty() && s.department != deptQuery) {
            continue;
        }

        filteredLogs.push_back({log, s});
    }

    // 4. Fill Table
    m_reportTable->setRowCount(static_cast<int>(filteredLogs.size()));
    
    // Dynamic Department list synchronization (avoid loops)
    disconnect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &ReportsView::handleFilter);
    QString currentDept = m_deptFilterCombo->currentText();
    m_deptFilterCombo->clear();
    m_deptFilterCombo->addItem("All Departments");
    
    std::vector<std::string> depts;
    auto allStudents = m_context->studentService->getStudentList();
    for (const auto& s : allStudents) {
        if (std::find(depts.begin(), depts.end(), s.department) == depts.end()) {
            depts.push_back(s.department);
        }
    }
    for (const auto& d : depts) {
        m_deptFilterCombo->addItem(QString::fromStdString(d));
    }
    int deptIdx = m_deptFilterCombo->findText(currentDept);
    if (deptIdx >= 0) m_deptFilterCombo->setCurrentIndex(deptIdx);
    connect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &ReportsView::handleFilter);

    for (size_t i = 0; i < filteredLogs.size(); ++i) {
        const auto& [log, s] = filteredLogs[i];
        m_reportTable->setItem(i, 0, new QTableWidgetItem(QString::number(log.id)));
        m_reportTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(log.student_id)));
        m_reportTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.full_name)));
        m_reportTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(s.department)));
        m_reportTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(log.date)));
        m_reportTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(log.timestamp)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(log.status));
        statusItem->setForeground(QBrush(QColor("#22C55E"))); // green
        m_reportTable->setItem(i, 6, statusItem);
    }
}

void ReportsView::handleFilter() {
    refreshReport();
}

void ReportsView::handleExportCSV() {
    if (m_reportTable->rowCount() == 0) {
        QMessageBox::warning(this, "Export CSV", "There are no reports to export.");
        return;
    }

    QString defaultName = QString("attendance_report_%1.csv").arg(QDate::currentDate().toString("yyyy_MM_dd"));
    QString filePath = QFileDialog::getSaveFileName(
        this, "Save Report", defaultName, "Comma Separated Values (*.csv)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        // Write header
        out << "Log ID,Student ID,Full Name,Department,Date,Timestamp,Status\n";
        
        // Write rows
        for (int r = 0; r < m_reportTable->rowCount(); ++r) {
            QStringList fields;
            for (int c = 0; c < m_reportTable->columnCount(); ++c) {
                QString val = m_reportTable->item(r, c)->text();
                // Escape commas or quotes if they exist
                if (val.contains(",") || val.contains("\"")) {
                    val = "\"" + val.replace("\"", "\"\"") + "\"";
                }
                fields.append(val);
            }
            out << fields.join(",") << "\n";
        }
        
        file.close();
        QMessageBox::information(this, "Export Success", "Attendance report exported successfully as CSV.");
    } else {
        QMessageBox::critical(this, "Export Failed", "Could not write to file: " + file.errorString());
    }
}

} // namespace Optix::UI::Views

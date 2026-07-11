#include "StudentManagementView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QDate>

namespace Optix::UI::Views {

// ==========================================
// STUDENT FORM DIALOG IMPLEMENTATION
// ==========================================
StudentFormDialog::StudentFormDialog(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QDialog(parent), m_context(std::move(context)) {
    setupUI();
}

void StudentFormDialog::setupUI() {
    setWindowTitle("Student Registration");
    setMinimumWidth(420);
    setStyleSheet("background-color: #FAFAFA;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    QLabel* headerLbl = new QLabel("Register Student Profile", this);
    headerLbl->setStyleSheet("font-family: 'Space Grotesk'; font-size: 18px; font-weight: bold; color: #1F2937;");
    mainLayout->addWidget(headerLbl);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);

    m_idEdit = new QLineEdit(this);
    m_idEdit->setPlaceholderText("e.g. STU1001");
    form->addRow("Student ID *", m_idEdit);

    m_matricEdit = new QLineEdit(this);
    m_matricEdit->setPlaceholderText("e.g. ENG/20/0042");
    form->addRow("Matric Number *", m_matricEdit);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("e.g. John Doe");
    form->addRow("Full Name *", m_nameEdit);

    m_deptEdit = new QLineEdit(this);
    m_deptEdit->setPlaceholderText("e.g. Computer Engineering");
    form->addRow("Department *", m_deptEdit);

    m_levelCombo = new QComboBox(this);
    m_levelCombo->addItems({"100", "200", "300", "400", "500"});
    form->addRow("Academic Level *", m_levelCombo);

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItems({"Male", "Female", "Other"});
    form->addRow("Gender *", m_genderCombo);

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("e.g. name@domain.com");
    form->addRow("Email (Optional)", m_emailEdit);

    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setPlaceholderText("e.g. +2348123456789");
    form->addRow("Phone (Optional)", m_phoneEdit);

    mainLayout->addLayout(form);

    m_errorLbl = new QLabel(this);
    m_errorLbl->setStyleSheet("color: #EF4444; font-size: 12px;");
    m_errorLbl->setWordWrap(true);
    mainLayout->addWidget(m_errorLbl);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    m_cancelBtn = new QPushButton("Cancel", this);
    m_cancelBtn->setObjectName("SecondaryBtn");
    
    m_saveBtn = new QPushButton("Save Student", this);
    m_saveBtn->setObjectName("PrimaryBtn");

    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &StudentFormDialog::handleSave);
}

void StudentFormDialog::setStudentToEdit(const std::string& student_id) {
    m_editStudentId = student_id;
    auto studentOpt = m_context->studentService->getStudent(student_id);
    
    if (studentOpt.has_value()) {
        setWindowTitle("Edit Student Profile");
        m_idEdit->setText(QString::fromStdString(studentOpt->student_id));
        m_idEdit->setEnabled(false); // Do not allow editing primary key ID
        m_matricEdit->setText(QString::fromStdString(studentOpt->matric_number));
        m_nameEdit->setText(QString::fromStdString(studentOpt->full_name));
        m_deptEdit->setText(QString::fromStdString(studentOpt->department));
        
        int lvlIdx = m_levelCombo->findText(QString::number(studentOpt->level));
        if (lvlIdx >= 0) m_levelCombo->setCurrentIndex(lvlIdx);

        int genIdx = m_genderCombo->findText(QString::fromStdString(studentOpt->gender));
        if (genIdx >= 0) m_genderCombo->setCurrentIndex(genIdx);

        m_emailEdit->setText(QString::fromStdString(studentOpt->email));
        m_phoneEdit->setText(QString::fromStdString(studentOpt->phone));
    }
}

Models::Student StudentFormDialog::getStudentData() const {
    Models::Student s;
    s.student_id = m_idEdit->text().trimmed().toStdString();
    s.matric_number = m_matricEdit->text().trimmed().toStdString();
    s.full_name = m_nameEdit->text().trimmed().toStdString();
    s.department = m_deptEdit->text().trimmed().toStdString();
    s.level = m_levelCombo->currentText().toInt();
    s.gender = m_genderCombo->currentText().toStdString();
    s.email = m_emailEdit->text().trimmed().toStdString();
    s.phone = m_phoneEdit->text().trimmed().toStdString();
    return s;
}

void StudentFormDialog::handleSave() {
    m_errorLbl->clear();
    Models::Student s = getStudentData();
    std::string error;
    bool success = false;

    if (m_editStudentId.empty()) {
        // Registering a new student
        success = m_context->studentService->registerStudent(s, error);
    } else {
        // Carry forward previous face registration status
        auto prev = m_context->studentService->getStudent(m_editStudentId);
        if (prev) {
            s.face_registered = prev->face_registered;
        }
        success = m_context->studentService->updateStudent(s, error);
    }

    if (success) {
        accept();
    } else {
        m_errorLbl->setText(QString::fromStdString(error));
    }
}

// ==========================================
// STUDENT MANAGEMENT VIEW IMPLEMENTATION
// ==========================================
StudentManagementView::StudentManagementView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    refreshStudentTable();
}

void StudentManagementView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QHBoxLayout* titleLayout = new QHBoxLayout();
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);
    
    QLabel* titleLbl = new QLabel("Student Directory", this);
    titleLbl->setObjectName("HeaderTitle");
    QLabel* subtitleLbl = new QLabel("Manage student academic profiles and facial data templates", this);
    subtitleLbl->setObjectName("HeaderSubtitle");
    
    textLayout->addWidget(titleLbl);
    textLayout->addWidget(subtitleLbl);
    titleLayout->addLayout(textLayout);

    m_addStudentBtn = new QPushButton("+ Add Student", this);
    m_addStudentBtn->setObjectName("PrimaryBtn");
    m_addStudentBtn->setCursor(Qt::PointingHandCursor);
    titleLayout->addWidget(m_addStudentBtn);
    mainLayout->addLayout(titleLayout);

    // Filter Bar Card Panel
    QFrame* filterCard = new QFrame(this);
    filterCard->setObjectName("PanelCard");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(16, 12, 16, 12);
    filterLayout->setSpacing(16);

    m_searchEdit = new QLineEdit(filterCard);
    m_searchEdit->setPlaceholderText("Search name, matric or student ID...");
    
    m_deptFilterCombo = new QComboBox(filterCard);
    m_deptFilterCombo->addItem("All Departments");

    filterLayout->addWidget(new QLabel("Search:", filterCard));
    filterLayout->addWidget(m_searchEdit, 3);
    filterLayout->addWidget(new QLabel("Department:", filterCard));
    filterLayout->addWidget(m_deptFilterCombo, 2);
    mainLayout->addWidget(filterCard);

    // Directory Table Panel
    m_studentTable = new QTableWidget(this);
    m_studentTable->setColumnCount(7);
    m_studentTable->setHorizontalHeaderLabels({"Student ID", "Matric Number", "Full Name", "Department", "Level", "Face status", "Actions"});
    m_studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentTable->verticalHeader()->setVisible(false);
    m_studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(m_studentTable);

    // Context / action buttons underneath the table
    QHBoxLayout* tableActionLayout = new QHBoxLayout();
    QPushButton* editBtn = new QPushButton("Edit Selected", this);
    editBtn->setObjectName("SecondaryBtn");
    QPushButton* deleteBtn = new QPushButton("Delete Selected", this);
    deleteBtn->setObjectName("SecondaryBtn");
    QPushButton* recordFaceBtn = new QPushButton("📸 Register Face Template", this);
    recordFaceBtn->setObjectName("PrimaryBtn");

    tableActionLayout->addWidget(editBtn);
    tableActionLayout->addWidget(deleteBtn);
    tableActionLayout->addStretch();
    tableActionLayout->addWidget(recordFaceBtn);
    mainLayout->addLayout(tableActionLayout);

    // Trigger Connections
    connect(m_addStudentBtn, &QPushButton::clicked, this, &StudentManagementView::handleAddStudent);
    connect(editBtn, &QPushButton::clicked, this, &StudentManagementView::handleEditStudent);
    connect(deleteBtn, &QPushButton::clicked, this, &StudentManagementView::handleDeleteStudent);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &StudentManagementView::handleSearchAndFilter);
    connect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &StudentManagementView::handleSearchAndFilter);
    
    connect(recordFaceBtn, &QPushButton::clicked, this, [this]() {
        int row = m_studentTable->currentRow();
        if (row >= 0) {
            QString studentId = m_studentTable->item(row, 0)->text();
            emit registerFaceRequested(studentId);
        } else {
            QMessageBox::warning(this, "Face Capture Request", "Please select a student from the directory first.");
        }
    });
}

void StudentManagementView::refreshStudentTable() {
    // 1. Fetch current filters
    std::string query = m_searchEdit->text().trimmed().toStdString();
    std::string dept = m_deptFilterCombo->currentIndex() > 0 ? m_deptFilterCombo->currentText().toStdString() : "";

    auto students = m_context->studentService->searchStudents(query, dept);
    
    m_studentTable->setRowCount(static_cast<int>(students.size()));

    // Update departments dropdown (safely avoid infinite loops)
    disconnect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &StudentManagementView::handleSearchAndFilter);
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
    connect(m_deptFilterCombo, &QComboBox::currentIndexChanged, this, &StudentManagementView::handleSearchAndFilter);

    // Populate rows
    for (size_t i = 0; i < students.size(); ++i) {
        const auto& s = students[i];
        m_studentTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s.student_id)));
        m_studentTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s.matric_number)));
        m_studentTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.full_name)));
        m_studentTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(s.department)));
        m_studentTable->setItem(i, 4, new QTableWidgetItem(QString::number(s.level)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(s.face_registered ? "Registered ✅" : "Not Registered ⚠️");
        statusItem->setForeground(QBrush(QColor(s.face_registered ? "#22C55E" : "#F97316")));
        m_studentTable->setItem(i, 5, statusItem);
        m_studentTable->setItem(i, 6, new QTableWidgetItem("Active"));
    }
}

void StudentManagementView::handleAddStudent() {
    StudentFormDialog dialog(m_context, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshStudentTable();
    }
}

void StudentManagementView::handleEditStudent() {
    int row = m_studentTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Edit Student", "Please select a student from the table.");
        return;
    }

    std::string studentId = m_studentTable->item(row, 0)->text().toStdString();
    StudentFormDialog dialog(m_context, this);
    dialog.setStudentToEdit(studentId);
    
    if (dialog.exec() == QDialog::Accepted) {
        refreshStudentTable();
    }
}

void StudentManagementView::handleDeleteStudent() {
    int row = m_studentTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Delete Student", "Please select a student from the table.");
        return;
    }

    QString studentId = m_studentTable->item(row, 0)->text();
    QString name = m_studentTable->item(row, 2)->text();

    auto reply = QMessageBox::question(
        this, "Confirm Deletion",
        QString("Are you sure you want to delete '%1' (ID: %2)? This will delete all associated datasets and attendance records.").arg(name, studentId),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_context->studentService->deleteStudent(studentId.toStdString())) {
            refreshStudentTable();
        } else {
            QMessageBox::critical(this, "Delete Failed", "An error occurred while deleting the student from the database.");
        }
    }
}

void StudentManagementView::handleSearchAndFilter() {
    refreshStudentTable();
}

} // namespace Optix::UI::Views

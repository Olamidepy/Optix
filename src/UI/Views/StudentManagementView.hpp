#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDialog>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

// Custom dialog to add/edit students
class StudentFormDialog : public QDialog {
    Q_OBJECT

public:
    explicit StudentFormDialog(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    
    // Configure form for editing existing student
    void setStudentToEdit(const std::string& student_id);

    // Get the filled student struct
    Models::Student getStudentData() const;

private slots:
    void handleSave();

private:
    void setupUI();

    std::shared_ptr<Application::AppContext> m_context;
    std::string m_editStudentId; // Empty if registering new student

    QLineEdit* m_idEdit{nullptr};
    QLineEdit* m_matricEdit{nullptr};
    QLineEdit* m_nameEdit{nullptr};
    QLineEdit* m_deptEdit{nullptr};
    QComboBox* m_levelCombo{nullptr};
    QComboBox* m_genderCombo{nullptr};
    QLineEdit* m_emailEdit{nullptr};
    QLineEdit* m_phoneEdit{nullptr};
    
    QPushButton* m_saveBtn{nullptr};
    QPushButton* m_cancelBtn{nullptr};
    QLabel* m_errorLbl{nullptr};
};

// Main Student Management Tab View
class StudentManagementView : public QWidget {
    Q_OBJECT

public:
    explicit StudentManagementView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~StudentManagementView() override = default;

    void refreshStudentTable();

signals:
    // Emitted when we want to transition to capturing faces for a student
    void registerFaceRequested(const QString& studentId);

private slots:
    void handleAddStudent();
    void handleEditStudent();
    void handleDeleteStudent();
    void handleSearchAndFilter();

private:
    void setupUI();

    std::shared_ptr<Application::AppContext> m_context;

    QLineEdit* m_searchEdit{nullptr};
    QComboBox* m_deptFilterCombo{nullptr};
    QPushButton* m_addStudentBtn{nullptr};
    QTableWidget* m_studentTable{nullptr};
};

} // namespace Optix::UI::Views

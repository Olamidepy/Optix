#pragma once
#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class DonutChart;
class BarChart;

class DashboardView : public QWidget {
    Q_OBJECT

public:
    explicit DashboardView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~DashboardView() override = default;

    // Refresh metrics on show/switch
    void refreshStats();

private:
    void setupUI();
    QWidget* createMetricCard(const QString& title, const QString& value, const QString& icon, QWidget* parent);

    std::shared_ptr<Application::AppContext> m_context;

    // Metric labels to update dynamically
    QLabel* m_totalStudentsLbl{nullptr};
    QLabel* m_presentTodayLbl{nullptr};
    QLabel* m_absentTodayLbl{nullptr};
    QLabel* m_accuracyLbl{nullptr};

    QTableWidget* m_recentActivityTable{nullptr};
    DonutChart* m_donutChart{nullptr};
    BarChart* m_barChart{nullptr};
};

} // namespace Optix::UI::Views

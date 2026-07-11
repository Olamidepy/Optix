#include "OnboardingView.hpp"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

namespace Optix::UI::Views {

OnboardingView::OnboardingView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
}

void OnboardingView::setupUI() {
    QGridLayout* outerLayout = new QGridLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* onboardCard = new QFrame(this);
    onboardCard->setObjectName("OnboardingCard");
    
    // Add soft shadow
    auto* shadow = new QGraphicsDropShadowEffect(onboardCard);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 30));
    onboardCard->setGraphicsEffect(shadow);

    QVBoxLayout* cardLayout = new QVBoxLayout(onboardCard);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(40, 50, 40, 50);

    QLabel* iconLbl = new QLabel(onboardCard);
    // Large, modern Unicode smiley face or graduation cap as icon placeholder
    iconLbl->setText(QString::fromUtf8("🎓"));
    iconLbl->setStyleSheet("font-size: 48px;");
    iconLbl->setAlignment(Qt::AlignCenter);

    QLabel* titleLbl = new QLabel("Welcome to Optix", onboardCard);
    titleLbl->setStyleSheet("font-family: 'Space Grotesk'; font-size: 24px; font-weight: bold; color: #1F2937;");
    titleLbl->setAlignment(Qt::AlignCenter);

    QLabel* infoLbl = new QLabel("No student records found in the database. Register students and capture their face templates to configure the model.", onboardCard);
    infoLbl->setStyleSheet("font-size: 13px; color: #6B7280; line-height: 20px;");
    infoLbl->setAlignment(Qt::AlignCenter);
    infoLbl->setWordWrap(true);

    m_actionBtn = new QPushButton("Register Students", onboardCard);
    m_actionBtn->setObjectName("PrimaryBtn");
    m_actionBtn->setCursor(Qt::PointingHandCursor);
    m_actionBtn->setFixedWidth(200);

    // Assemble Card Layout
    cardLayout->addWidget(iconLbl);
    cardLayout->addWidget(titleLbl);
    cardLayout->addWidget(infoLbl);
    cardLayout->addWidget(m_actionBtn, 0, Qt::AlignCenter);
    cardLayout->addStretch();

    outerLayout->addWidget(onboardCard, 0, 0, Qt::AlignCenter);

    connect(m_actionBtn, &QPushButton::clicked, this, &OnboardingView::registerStudentsClicked);
}

} // namespace Optix::UI::Views

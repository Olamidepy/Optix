#include "LoginView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

namespace Optix::UI::Views {

LoginView::LoginView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
}

void LoginView::setupUI() {
    // Outer centered layout
    QGridLayout* outerLayout = new QGridLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    // Centered Login Card Frame
    QFrame* loginCard = new QFrame(this);
    loginCard->setObjectName("LoginCard");
    
    // Add soft shadow to Card
    auto* shadow = new QGraphicsDropShadowEffect(loginCard);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 30));
    loginCard->setGraphicsEffect(shadow);

    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setSpacing(18);
    cardLayout->setContentsMargins(36, 40, 36, 40);

    // Title & Subtitle
    QLabel* titleLbl = new QLabel("Optix Login", loginCard);
    titleLbl->setStyleSheet("font-family: 'Space Grotesk'; font-size: 24px; font-weight: bold; color: #F97316;");
    titleLbl->setAlignment(Qt::AlignCenter);

    QLabel* subtitleLbl = new QLabel("Authenticate to access dashboard", loginCard);
    subtitleLbl->setStyleSheet("font-size: 12px; color: #6B7280;");
    subtitleLbl->setAlignment(Qt::AlignCenter);

    // Username Group
    QLabel* userLbl = new QLabel("Username", loginCard);
    userLbl->setStyleSheet("font-weight: 600; color: #1F2937;");
    m_usernameEdit = new QLineEdit(loginCard);
    m_usernameEdit->setPlaceholderText("Enter administrator username");

    // Password Group
    QLabel* passLbl = new QLabel("Password", loginCard);
    passLbl->setStyleSheet("font-weight: 600; color: #1F2937;");
    m_passwordEdit = new QLineEdit(loginCard);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");

    // Login Button
    m_loginBtn = new QPushButton("Sign In", loginCard);
    m_loginBtn->setObjectName("PrimaryBtn");
    m_loginBtn->setCursor(Qt::PointingHandCursor);

    // Status / Feedback message
    m_statusLbl = new QLabel(loginCard);
    m_statusLbl->setStyleSheet("color: #EF4444; font-size: 12px;");
    m_statusLbl->setAlignment(Qt::AlignCenter);
    m_statusLbl->setWordWrap(true);

    // Layout assemblies
    cardLayout->addWidget(titleLbl);
    cardLayout->addWidget(subtitleLbl);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(userLbl);
    cardLayout->addWidget(m_usernameEdit);
    cardLayout->addWidget(passLbl);
    cardLayout->addWidget(m_passwordEdit);
    cardLayout->addWidget(m_loginBtn);
    cardLayout->addWidget(m_statusLbl);
    cardLayout->addStretch();

    // Center Card inside Outer Layout
    outerLayout->addWidget(loginCard, 0, 0, Qt::AlignCenter);

    // Sign In Trigger connections
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginView::handleLogin);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginView::handleLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginView::handleLogin);
}

void LoginView::handleLogin() {
    m_statusLbl->clear();
    
    std::string username = m_usernameEdit->text().trimmed().toStdString();
    std::string password = m_passwordEdit->text().toStdString();

    if (username.empty() || password.empty()) {
        m_statusLbl->setText("Username and password cannot be empty.");
        return;
    }

    if (m_context->authService->login(username, password)) {
        m_usernameEdit->clear();
        m_passwordEdit->clear();
        emit loginSuccess();
    } else {
        m_statusLbl->setText("Invalid username or password.");
    }
}

} // namespace Optix::UI::Views

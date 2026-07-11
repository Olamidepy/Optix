#pragma once
#include "Repositories/IUserRepository.hpp"
#include "Models/User.hpp"
#include <memory>
#include <optional>

namespace Optix::Services {

class AuthService {
public:
    explicit AuthService(std::shared_ptr<Repositories::IUserRepository> userRepo);
    ~AuthService() = default;

    // Login admin using credentials. Hashes password internally and queries UserRepository.
    bool login(const std::string& username, const std::string& password);

    // Terminate current session
    void logout();

    // Check if administrator session is active
    bool isLoggedIn() const { return m_currentUser.has_value(); }

    // Retrieve active administrator profile
    std::optional<Models::User> getCurrentUser() const { return m_currentUser; }

private:
    std::shared_ptr<Repositories::IUserRepository> m_userRepo;
    std::optional<Models::User> m_currentUser;
};

} // namespace Optix::Services

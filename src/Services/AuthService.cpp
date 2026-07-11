#include "AuthService.hpp"
#include "Utilities/Crypto.hpp"

namespace Optix::Services {

AuthService::AuthService(std::shared_ptr<Repositories::IUserRepository> userRepo)
    : m_userRepo(std::move(userRepo)) {
}

bool AuthService::login(const std::string& username, const std::string& password) {
    std::string hashedPassword = Utilities::Crypto::hashPassword(password);
    
    Models::User user;
    if (m_userRepo->validateUser(username, hashedPassword, user)) {
        m_currentUser = user;
        return true;
    }
    
    return false;
}

void AuthService::logout() {
    m_currentUser.reset();
}

} // namespace Optix::Services

#pragma once
#include "IUserRepository.hpp"
#include "IDatabase.hpp"
#include <memory>

namespace Optix::Repositories {

class UserRepository : public IUserRepository {
public:
    explicit UserRepository(std::shared_ptr<IDatabase> db);
    ~UserRepository() override = default;

    bool validateUser(const std::string& username, const std::string& password_hash, Models::User& outUser) override;
    bool createUser(const std::string& username, const std::string& password_hash) override;
    bool hasUsers() override;

private:
    std::shared_ptr<IDatabase> m_db;
};

} // namespace Optix::Repositories

#pragma once
#include "Models/User.hpp"
#include <string>
#include <optional>

namespace Optix::Repositories {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    // Validate a user by username and password hash. Returns true if valid, fills outUser.
    virtual bool validateUser(const std::string& username, const std::string& password_hash, Models::User& outUser) = 0;

    // Create a new administrative user
    virtual bool createUser(const std::string& username, const std::string& password_hash) = 0;

    // Check if any user exists in the database
    virtual bool hasUsers() = 0;
};

} // namespace Optix::Repositories

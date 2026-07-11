#pragma once
#include <string>

namespace Optix::Utilities {

class Crypto {
public:
    // Generate a SHA-256 hex string of the password
    static std::string hashPassword(const std::string& password);
};

} // namespace Optix::Utilities

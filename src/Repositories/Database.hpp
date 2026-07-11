#pragma once
#include "IDatabase.hpp"
#include <string>

namespace Optix::Repositories {

class Database : public IDatabase {
public:
    Database();
    ~Database() override;

    // Delete copy constructors to enforce singleton/dependency-injected usage
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool open(const std::string& path) override;
    void close() override;
    sqlite3* getHandle() const override;
    bool execute(const std::string& sql) override;

private:
    // Setup tables if they do not exist
    bool initializeSchema();

    sqlite3* m_db{nullptr};
};

} // namespace Optix::Repositories

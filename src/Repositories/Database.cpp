#include "Database.hpp"
#include "Utilities/Crypto.hpp"
#include <iostream>
#include <vector>

#ifndef OPTIX_MOCK_BACKEND
#include <sqlite3.h>
#endif

namespace Optix::Repositories {

Database::Database() : m_db(nullptr) {
}

Database::~Database() {
    close();
}

bool Database::open(const std::string& path) {
#ifdef OPTIX_MOCK_BACKEND
    std::cout << "Optix Database: Running in Mock Mode." << std::endl;
    return true;
#else
    if (m_db) {
        return true; // Already open
    }
    
    int rc = sqlite3_open(path.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Optix Database: Cannot open database at " << path 
                  << " Error: " << (m_db ? sqlite3_errmsg(m_db) : "Unknown") << std::endl;
        close();
        return false;
    }

    // Enable foreign keys
    execute("PRAGMA foreign_keys = ON;");
    
    std::cout << "Optix Database: Opened SQLite database at " << path << std::endl;
    return initializeSchema();
#endif
}

void Database::close() {
#ifndef OPTIX_MOCK_BACKEND
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
#endif
}

sqlite3* Database::getHandle() const {
    return m_db;
}

bool Database::execute(const std::string& sql) {
#ifdef OPTIX_MOCK_BACKEND
    return true;
#else
    if (!m_db) {
        return false;
    }
    
    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Optix Database: SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
#endif
}

bool Database::initializeSchema() {
#ifdef OPTIX_MOCK_BACKEND
    return true;
#else
    // 1. Create Users Table
    const std::string createUsersTable = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
        
    // 2. Create Students Table
    const std::string createStudentsTable =
        "CREATE TABLE IF NOT EXISTS Students ("
        "student_id TEXT PRIMARY KEY,"
        "matric_number TEXT UNIQUE NOT NULL,"
        "full_name TEXT NOT NULL,"
        "department TEXT NOT NULL,"
        "level INTEGER NOT NULL,"
        "gender TEXT NOT NULL,"
        "email TEXT,"
        "phone TEXT,"
        "face_registered INTEGER DEFAULT 0,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    // 3. Create Attendance Table
    const std::string createAttendanceTable =
        "CREATE TABLE IF NOT EXISTS Attendance ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "student_id TEXT NOT NULL,"
        "date TEXT NOT NULL,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "status TEXT NOT NULL,"
        "UNIQUE(student_id, date),"
        "FOREIGN KEY (student_id) REFERENCES Students(student_id) ON DELETE CASCADE"
        ");";

    if (!execute(createUsersTable) || !execute(createStudentsTable) || !execute(createAttendanceTable)) {
        return false;
    }

    // 4. Seed default admin account if table is empty
    sqlite3_stmt* stmt = nullptr;
    const std::string selectUsersCount = "SELECT COUNT(*) FROM Users;";
    int rc = sqlite3_prepare_v2(m_db, selectUsersCount.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        int userCount = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            userCount = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);

        if (userCount == 0) {
            std::string passwordHash = Utilities::Crypto::hashPassword("admin123");
            const std::string insertAdmin = "INSERT INTO Users (username, password_hash) VALUES (?, ?);";
            if (sqlite3_prepare_v2(m_db, insertAdmin.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
                std::cout << "Optix Database: Seeded default administrator account (username: admin, password: admin123)" << std::endl;
            }
        }
    }

    // 5. Seed student class list into SQLite database
    struct StudentSeed {
        std::string id;
        std::string matric;
        std::string name;
        std::string dept;
        int level;
        std::string gender;
    };

    std::vector<StudentSeed> defaultStudents = {
        {"1", "CPE/23/4754", "Abdulsemiu Khaalid Dolapo", "Computer Engineering", 300, "M"},
        {"2", "CPE/23/4755", "Abodunrin Sharon Oluwamayowa", "Computer Engineering", 300, "F"},
        {"3", "CPE/23/4756", "Aboyeji Peace Bolatito", "Computer Engineering", 300, "F"},
        {"4", "CPE/23/4757", "Adebayo Ademide Rebecca", "Computer Engineering", 300, "F"},
        {"5", "CPE/23/4758", "Adebayo Joshua Adebola", "Computer Engineering", 300, "M"},
        {"6", "CPE/23/4759", "Adebayo Opeyemi James", "Computer Engineering", 300, "M"},
        {"7", "CPE/23/4760", "Adebayo Yusuf Olatunji", "Computer Engineering", 300, "M"},
        {"8", "CPE/23/4761", "Adebunmi Peter Ademidun", "Computer Engineering", 300, "M"},
        {"9", "CPE/23/4764", "Adeniran Philip Ajibola", "Computer Engineering", 300, "M"},
        {"10", "CPE/23/4765", "Adeoti Isaac Oluwabukunmi", "Computer Engineering", 300, "M"},
        {"11", "CPE/23/4766", "Adeyanju Olanrewaju Daniel", "Computer Engineering", 300, "M"},
        {"12", "CPE/23/4768", "Adeyemo Paul Oluwaseyi", "Computer Engineering", 300, "M"},
        {"13", "CPE/23/4769", "Afolabi Ayokorede Samuel", "Computer Engineering", 300, "M"},
        {"14", "CPE/23/4770", "Afolabi Ayomide Emmanuel", "Computer Engineering", 300, "M"},
        {"15", "CPE/23/4771", "Afolabi Boluwatife Emmanuel", "Computer Engineering", 300, "M"},
        {"16", "CPE/23/4772", "Afolami Anuoluwapo Oluwafemi", "Computer Engineering", 300, "M"},
        {"17", "CPE/23/4773", "Agada Trinity John", "Computer Engineering", 300, "M"},
        {"18", "CPE/23/4774", "Agbo Oche Joseph-Dominion", "Computer Engineering", 300, "M"},
        {"19", "CPE/23/4776", "Akande Daniel Adeogo", "Computer Engineering", 300, "M"},
        {"20", "CPE/23/4777", "Akinbuwa Osemayowa Emmanuel", "Computer Engineering", 300, "M"},
        {"21", "CPE/23/4778", "Akintoye Favour Truth", "Computer Engineering", 300, "F"},
        {"22", "CPE/23/4779", "Akinyemi Oluwaseun Alexander", "Computer Engineering", 300, "M"},
        {"23", "CPE/23/4780", "Amos Godswill Emediong", "Computer Engineering", 300, "M"},
        {"24", "CPE/23/4781", "Aniekan Victory Oyindamola", "Computer Engineering", 300, "F"},
        {"25", "CPE/23/4783", "Arewa Samuel Oladotun", "Computer Engineering", 300, "M"},
        {"26", "CPE/23/4784", "Ayegba Donald Ojotule", "Computer Engineering", 300, "M"},
        {"27", "CPE/23/4785", "Ayodeji Enoch Ayomikun", "Computer Engineering", 300, "M"},
        {"28", "CPE/23/4786", "Babalola Praise Feranmi", "Computer Engineering", 300, "F"},
        {"29", "CPE/23/4787", "Badru Ibrahim Isola", "Computer Engineering", 300, "M"},
        {"30", "CPE/23/4788", "Balogun Qusman Olawande", "Computer Engineering", 300, "M"},
        {"31", "CPE/23/4789", "Balogun Victor Adeiza", "Computer Engineering", 300, "M"},
        {"32", "CPE/23/4790", "Daramola Afolabi Joshua", "Computer Engineering", 300, "M"},
        {"33", "CPE/23/4791", "Egbayelo Oluwajomiloju Ayomide", "Computer Engineering", 300, "M"},
        {"34", "CPE/23/4793", "Emeka Jacob Chukudi", "Computer Engineering", 300, "M"},
        {"35", "CPE/23/4794", "Emeka Kenneth Miracle", "Computer Engineering", 300, "M"},
        {"36", "CPE/23/4795", "Fagbohunka Toluwani Dominion", "Computer Engineering", 300, "M"},
        {"37", "CPE/23/4796", "Falomo Olamide David", "Computer Engineering", 300, "M"},
        {"38", "CPE/23/4798", "Idowu Oluwateniola Emmanuel", "Computer Engineering", 300, "M"},
        {"39", "CPE/23/4799", "Ikuejamofo Jesutofunmi Sharon", "Computer Engineering", 300, "F"},
        {"40", "CPE/23/4800", "Ilesanmi Mosinmiloluwa Winner", "Computer Engineering", 300, "F"},
        {"41", "CPE/23/4801", "Ilesanmi Praise Temitope", "Computer Engineering", 300, "M"},
        {"42", "CPE/23/4802", "Ilesanmi Testimony", "Computer Engineering", 300, "F"},
        {"43", "CPE/23/4803", "Izuagie Evelyn Oluwaseyi", "Computer Engineering", 300, "F"},
        {"44", "CPE/23/4804", "Julius Micheal Ayomide", "Computer Engineering", 300, "M"},
        {"45", "CPE/23/4805", "Muftaudeen Khadijat Ayomide", "Computer Engineering", 300, "F"},
        {"46", "CPE/23/4806", "Ndee Armstrong Ben", "Computer Engineering", 300, "M"},
        {"47", "CPE/23/4808", "Odubayo John Akintunde", "Computer Engineering", 300, "M"},
        {"48", "CPE/23/4809", "Odunade Victor Oluwaremilekun", "Computer Engineering", 300, "M"},
        {"49", "CPE/23/4810", "Ogbonnaya Ebube Emmanuel", "Computer Engineering", 300, "M"},
        {"50", "CPE/23/4811", "Ogunfolaji Kayode Thomas", "Computer Engineering", 300, "M"},
        {"51", "CPE/23/4812", "Ogunmefun Kevin Okikiola", "Computer Engineering", 300, "M"},
        {"52", "CPE/23/4813", "Oladimeji Faith John", "Computer Engineering", 300, "M"},
        {"53", "CPE/23/4814", "Olanibi Adeolu Samuel", "Computer Engineering", 300, "M"},
        {"54", "CPE/23/4815", "Olaniyan Pelumi Israel", "Computer Engineering", 300, "M"},
        {"55", "CPE/23/4817", "Olaoye Isaac Nifemi", "Computer Engineering", 300, "M"},
        {"56", "CPE/23/4818", "Olarotimi Eniola Samuel", "Computer Engineering", 300, "M"},
        {"57", "CPE/23/4819", "Olatudun Opeyemi Benedict", "Computer Engineering", 300, "M"},
        {"58", "CPE/23/4820", "Olayide Precious Elizabeth", "Computer Engineering", 300, "F"},
        {"59", "CPE/23/4821", "Olorunnisola Mujeeb Tobiloba", "Computer Engineering", 300, "M"},
        {"60", "CPE/23/4822", "Omojola Damilola Isaac", "Computer Engineering", 300, "M"},
        {"61", "CPE/23/4823", "Oni Caleb Temiloluwa", "Computer Engineering", 300, "M"},
        {"62", "CPE/23/4824", "Opeoluwa Mustapha Olamide", "Computer Engineering", 300, "M"},
        {"63", "CPE/23/4825", "Owoseni Daniels Opeyemi", "Computer Engineering", 300, "M"},
        {"64", "CPE/23/4826", "Oyedeji Samuel Oluwagbayi", "Computer Engineering", 300, "M"},
        {"65", "CPE/23/4827", "Oyewusi Zion Abiodun", "Computer Engineering", 300, "M"},
        {"66", "CPE/23/4828", "Sadiq Gbenga Michael", "Computer Engineering", 300, "M"},
        {"67", "CPE/23/4829", "Samuel Joshua Adinoyi", "Computer Engineering", 300, "M"},
        {"68", "CPE/23/4830", "Shittu Priscilla Adedunni", "Computer Engineering", 300, "F"},
        {"69", "CPE/23/4831", "Shoneye Iqmat Sunmisola", "Computer Engineering", 300, "F"},
        {"70", "CPE/23/4832", "Taiwo James Oriola", "Computer Engineering", 300, "M"},
        {"71", "CPE/23/4833", "Thomas Boluwatife Temidayo", "Computer Engineering", 300, "M"},
        {"72", "CPE/23/4834", "Tolowoye Iresubomi Sharon", "Computer Engineering", 300, "F"},
        {"73", "CPE/23/4835", "Tugbobo Ayodeji Samuel", "Computer Engineering", 300, "M"},
        {"74", "CPE/23/4836", "Wasiu Qudus Akorede", "Computer Engineering", 300, "M"},
        {"75", "CPE/24/9860", "Adebowale Ikoghene David", "Computer Engineering", 300, "M"},
        {"76", "CPE/24/9870", "Ajenifuja Abdullahi Ajeniyi", "Computer Engineering", 300, "M"},
        {"77", "CPE/24/9892", "Bamiduro Abiodun Daniel", "Computer Engineering", 300, "M"},
        {"78", "CPE/24/9893", "Betiku Mercy Olamide", "Computer Engineering", 300, "F"},
        {"79", "CPE/24/9905", "Nasrudeen Mojeed Akin", "Computer Engineering", 300, "M"},
        {"80", "CPE/24/9917", "Omisore Daniel Damilola", "Computer Engineering", 300, "M"},
        {"81", "CPE/24/9919", "Orelesi Elijah Olanrewaju", "Computer Engineering", 300, "M"}
    };

    // Purge any old legacy mock matric numbers containing CPE/2021 or CPE/21
    execute("DELETE FROM Students WHERE matric_number LIKE '%CPE/2021/%' OR matric_number LIKE '%CPE/21/%' OR CAST(student_id AS INTEGER) > 81;");

    const std::string insertStudentSql = "INSERT OR REPLACE INTO Students (student_id, matric_number, full_name, department, level, gender, face_registered) VALUES (?, ?, ?, ?, ?, ?, 0);";

    for (const auto& s : defaultStudents) {
        if (sqlite3_prepare_v2(m_db, insertStudentSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, s.id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, s.matric.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, s.name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, s.dept.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 5, s.level);
            sqlite3_bind_text(stmt, 6, s.gender.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }

    // Explicitly reset face_registered to 0 for all students to ensure clean fresh registration start
    execute("UPDATE Students SET face_registered = 0;");
    execute("DELETE FROM Attendance;");

    std::cout << "Optix Database: Synchronized official student roster (" << defaultStudents.size() << " records)" << std::endl;

    return true;
#endif
}

} // namespace Optix::Repositories

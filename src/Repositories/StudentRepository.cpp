#include "StudentRepository.hpp"
#include <vector>
#include <string>
#include <optional>

#ifdef OPTIX_MOCK_BACKEND
#include "Core/MockBackend.hpp"
#include "Models/Student.hpp"
#include <algorithm>

// Define global in-memory students database
static std::vector<Optix::Models::Student> createMockStudents() {
    std::vector<Optix::Models::Student> list;
    list.reserve(85);
    list.push_back({"1", "CPE/23/4754", "Abdulsemiu Khaalid Dolapo", "Computer Engineering", 300, "Male", "abdulsemiukhaaliddolapo@example.com", "0801000001", false, "2026-07-13 10:00:00"});
    list.push_back({"2", "CPE/23/4755", "Abodunrin Sharon Oluwamayowa", "Computer Engineering", 300, "Female", "abodunrinsharonoluwamayowa@example.com", "0801000002", false, "2026-07-13 10:00:00"});
    list.push_back({"3", "CPE/23/4756", "Aboyeji Peace Bolatito", "Computer Engineering", 300, "Female", "aboyejipeacebolatito@example.com", "0801000003", false, "2026-07-13 10:00:00"});
    list.push_back({"4", "CPE/23/4757", "Adebayo Ademide Rebecca", "Computer Engineering", 300, "Female", "adebayoademiderebecca@example.com", "0801000004", false, "2026-07-13 10:00:00"});
    list.push_back({"5", "CPE/23/4758", "Adebayo Joshua Adebola", "Computer Engineering", 300, "Male", "adebayojoshuaadebola@example.com", "0801000005", false, "2026-07-13 10:00:00"});
    list.push_back({"6", "CPE/23/4759", "Adebayo Opeyemi James", "Computer Engineering", 300, "Male", "adebayoopeyemijames@example.com", "0801000006", false, "2026-07-13 10:00:00"});
    list.push_back({"7", "CPE/23/4760", "Adebayo Yusuf Olatunji", "Computer Engineering", 300, "Male", "adebayoyusufolatunji@example.com", "0801000007", false, "2026-07-13 10:00:00"});
    list.push_back({"8", "CPE/23/4761", "Adebunmi Peter Ademidun", "Computer Engineering", 300, "Male", "adebunmipeterademidun@example.com", "0801000008", false, "2026-07-13 10:00:00"});
    list.push_back({"9", "CPE/23/4764", "Adeniran Philip Ajibola", "Computer Engineering", 300, "Male", "adeniranphilipajibola@example.com", "0801000009", false, "2026-07-13 10:00:00"});
    list.push_back({"10", "CPE/23/4765", "Adeoti Isaac Oluwabukunmi", "Computer Engineering", 300, "Male", "adeotiisaacoluwabukunmi@example.com", "0801000010", false, "2026-07-13 10:00:00"});
    list.push_back({"11", "CPE/23/4766", "Adeyanju Olanrewaju Daniel", "Computer Engineering", 300, "Male", "adeyanjuolanrewajudaniel@example.com", "0801000011", false, "2026-07-13 10:00:00"});
    list.push_back({"12", "CPE/23/4768", "Adeyemo Paul Oluwaseyi", "Computer Engineering", 300, "Male", "adeyemopauloluwaseyi@example.com", "0801000012", false, "2026-07-13 10:00:00"});
    list.push_back({"13", "CPE/23/4769", "Afolabi Ayokorede Samuel", "Computer Engineering", 300, "Male", "afolabiayokoredesamuel@example.com", "0801000013", false, "2026-07-13 10:00:00"});
    list.push_back({"14", "CPE/23/4770", "Afolabi Ayomide Emmanuel", "Computer Engineering", 300, "Male", "afolabiayomideemmanuel@example.com", "0801000014", false, "2026-07-13 10:00:00"});
    list.push_back({"15", "CPE/23/4771", "Afolabi Boluwatife Emmanuel", "Computer Engineering", 300, "Male", "afolabiboluwatifeemmanuel@example.com", "0801000015", false, "2026-07-13 10:00:00"});
    list.push_back({"16", "CPE/23/4772", "Afolami Anuoluwapo Oluwafemi", "Computer Engineering", 300, "Male", "afolamianuoluwapooluwafemi@example.com", "0801000016", false, "2026-07-13 10:00:00"});
    list.push_back({"17", "CPE/23/4773", "Agada Trinity John", "Computer Engineering", 300, "Male", "agadatrinityjohn@example.com", "0801000017", false, "2026-07-13 10:00:00"});
    list.push_back({"18", "CPE/23/4774", "Agbo Oche Joseph-Dominion", "Computer Engineering", 300, "Male", "agboochejosephdominion@example.com", "0801000018", false, "2026-07-13 10:00:00"});
    list.push_back({"19", "CPE/23/4776", "Akande Daniel Adeogo", "Computer Engineering", 300, "Male", "akandedanieladeogo@example.com", "0801000019", false, "2026-07-13 10:00:00"});
    list.push_back({"20", "CPE/23/4777", "Akinbuwa Osemayowa Emmanuel", "Computer Engineering", 300, "Male", "akinbuwaosemayowaemmanuel@example.com", "0801000020", false, "2026-07-13 10:00:00"});
    list.push_back({"21", "CPE/23/4778", "Akintoye Favour Truth", "Computer Engineering", 300, "Female", "akintoyefavourtruth@example.com", "0801000021", false, "2026-07-13 10:00:00"});
    list.push_back({"22", "CPE/23/4779", "Akinyemi Oluwaseun Alexander", "Computer Engineering", 300, "Male", "akinyemioluwaseunalexander@example.com", "0801000022", false, "2026-07-13 10:00:00"});
    list.push_back({"23", "CPE/23/4780", "Amos Godswill Emediong", "Computer Engineering", 300, "Male", "amosgodswillemediong@example.com", "0801000023", false, "2026-07-13 10:00:00"});
    list.push_back({"24", "CPE/23/4781", "Aniekan Victory Oyindamola", "Computer Engineering", 300, "Female", "aniekanvictoryoyindamola@example.com", "0801000024", false, "2026-07-13 10:00:00"});
    list.push_back({"25", "CPE/23/4783", "Arewa Samuel Oladotun", "Computer Engineering", 300, "Male", "arewasamueloladotun@example.com", "0801000025", false, "2026-07-13 10:00:00"});
    list.push_back({"26", "CPE/23/4784", "Ayegba Donald Ojotule", "Computer Engineering", 300, "Male", "ayegbadonaldojotule@example.com", "0801000026", false, "2026-07-13 10:00:00"});
    list.push_back({"27", "CPE/23/4785", "Ayodeji Enoch Ayomikun", "Computer Engineering", 300, "Male", "ayodejienochayomikun@example.com", "0801000027", false, "2026-07-13 10:00:00"});
    list.push_back({"28", "CPE/23/4786", "Babalola Praise Feranmi", "Computer Engineering", 300, "Female", "babalolapraiseferanmi@example.com", "0801000028", false, "2026-07-13 10:00:00"});
    list.push_back({"29", "CPE/23/4787", "Badru Ibrahim Isola", "Computer Engineering", 300, "Male", "badruibrahimisola@example.com", "0801000029", false, "2026-07-13 10:00:00"});
    list.push_back({"30", "CPE/23/4788", "Balogun Qusman Olawande", "Computer Engineering", 300, "Male", "balogunqusmanolawande@example.com", "0801000030", false, "2026-07-13 10:00:00"});
    list.push_back({"31", "CPE/23/4789", "Balogun Victor Adeiza", "Computer Engineering", 300, "Male", "balogunvictoradeiza@example.com", "0801000031", false, "2026-07-13 10:00:00"});
    list.push_back({"32", "CPE/23/4790", "Daramola Afolabi Joshua", "Computer Engineering", 300, "Male", "daramolaafolabijoshua@example.com", "0801000032", false, "2026-07-13 10:00:00"});
    list.push_back({"33", "CPE/23/4791", "Egbayelo Oluwajomiloju Ayomide", "Computer Engineering", 300, "Male", "egbayelooluwajomilojuayomide@example.com", "0801000033", false, "2026-07-13 10:00:00"});
    list.push_back({"34", "CPE/23/4793", "Emeka Jacob Chukudi", "Computer Engineering", 300, "Male", "emekajacobchukudi@example.com", "0801000034", false, "2026-07-13 10:00:00"});
    list.push_back({"35", "CPE/23/4794", "Emeka Kenneth Miracle", "Computer Engineering", 300, "Male", "emekakennethmiracle@example.com", "0801000035", false, "2026-07-13 10:00:00"});
    list.push_back({"36", "CPE/23/4795", "Fagbohunka Toluwani Dominion", "Computer Engineering", 300, "Male", "fagbohunkatoluwanidominion@example.com", "0801000036", false, "2026-07-13 10:00:00"});
    list.push_back({"37", "CPE/23/4796", "Falomo Olamide David", "Computer Engineering", 300, "Male", "falomoolamidedavid@example.com", "0801000037", false, "2026-07-13 10:00:00"});
    list.push_back({"38", "CPE/23/4798", "Idowu Oluwateniola Emmanuel", "Computer Engineering", 300, "Male", "idowuoluwateniolaemmanuel@example.com", "0801000038", false, "2026-07-13 10:00:00"});
    list.push_back({"39", "CPE/23/4799", "Ikuejamofo Jesutofunmi Sharon", "Computer Engineering", 300, "Female", "ikuejamofojesutofunmisharon@example.com", "0801000039", false, "2026-07-13 10:00:00"});
    list.push_back({"40", "CPE/23/4800", "Ilesanmi Mosinmiloluwa Winner", "Computer Engineering", 300, "Female", "ilesanmimosinmiloluwawinner@example.com", "0801000040", false, "2026-07-13 10:00:00"});
    list.push_back({"41", "CPE/23/4801", "Ilesanmi Praise Temitope", "Computer Engineering", 300, "Female", "ilesanmipraisetemitope@example.com", "0801000041", false, "2026-07-13 10:00:00"});
    list.push_back({"42", "CPE/23/4802", "Ilesanmi Testimony", "Computer Engineering", 300, "Female", "ilesanmitestimony@example.com", "0801000042", false, "2026-07-13 10:00:00"});
    list.push_back({"43", "CPE/23/4803", "Izuagie Evelyn Oluwaseyi", "Computer Engineering", 300, "Female", "izuagieevelynoluwaseyi@example.com", "0801000043", false, "2026-07-13 10:00:00"});
    list.push_back({"44", "CPE/23/4804", "Julius Micheal Ayomide", "Computer Engineering", 300, "Male", "juliusmichealayomide@example.com", "0801000044", false, "2026-07-13 10:00:00"});
    list.push_back({"45", "CPE/23/4805", "Muftaudeen Khadijat Ayomide", "Computer Engineering", 300, "Female", "muftaudeenkhadijatayomide@example.com", "0801000045", false, "2026-07-13 10:00:00"});
    list.push_back({"46", "CPE/23/4806", "Ndee Armstrong Ben", "Computer Engineering", 300, "Male", "ndeearmstrongben@example.com", "0801000046", false, "2026-07-13 10:00:00"});
    list.push_back({"47", "CPE/23/4808", "Odubayo John Akintunde", "Computer Engineering", 300, "Male", "odubayojohnakintunde@example.com", "0801000047", false, "2026-07-13 10:00:00"});
    list.push_back({"48", "CPE/23/4809", "Odunade Victor Oluwaremilekun", "Computer Engineering", 300, "Male", "odunadevictoroluwaremilekun@example.com", "0801000048", false, "2026-07-13 10:00:00"});
    list.push_back({"49", "CPE/23/4810", "Ogbonnaya Ebube Emmanuel", "Computer Engineering", 300, "Male", "ogbonnayaebubeemmanuel@example.com", "0801000049", false, "2026-07-13 10:00:00"});
    list.push_back({"50", "CPE/23/4811", "Ogunfolaji Kayode Thomas", "Computer Engineering", 300, "Male", "ogunfolajikayodethomas@example.com", "0801000050", false, "2026-07-13 10:00:00"});
    list.push_back({"51", "CPE/23/4812", "Ogunmefun Kevin Okikiola", "Computer Engineering", 300, "Male", "ogunmefunkevinokikiola@example.com", "0801000051", false, "2026-07-13 10:00:00"});
    list.push_back({"52", "CPE/23/4813", "Oladimeji Faith John", "Computer Engineering", 300, "Female", "oladimejifaithjohn@example.com", "0801000052", false, "2026-07-13 10:00:00"});
    list.push_back({"53", "CPE/23/4814", "Olanibi Adeolu Samuel", "Computer Engineering", 300, "Male", "olanibiadeolusamuel@example.com", "0801000053", false, "2026-07-13 10:00:00"});
    list.push_back({"54", "CPE/23/4815", "Olaniyan Pelumi Israel", "Computer Engineering", 300, "Male", "olaniyanpelumiisrael@example.com", "0801000054", false, "2026-07-13 10:00:00"});
    list.push_back({"55", "CPE/23/4817", "Olaoye Isaac Nifemi", "Computer Engineering", 300, "Male", "olaoyeisaacnifemi@example.com", "0801000055", false, "2026-07-13 10:00:00"});
    list.push_back({"56", "CPE/23/4818", "Olarotimi Eniola Samuel", "Computer Engineering", 300, "Male", "olarotimieniolasamuel@example.com", "0801000056", false, "2026-07-13 10:00:00"});
    list.push_back({"57", "CPE/23/4819", "Olatudun Opeyemi Benedict", "Computer Engineering", 300, "Male", "olatudunopeyemibenedict@example.com", "0801000057", false, "2026-07-13 10:00:00"});
    list.push_back({"58", "CPE/23/4820", "Olayide Precious Elizabeth", "Computer Engineering", 300, "Female", "olayidepreciouselizabeth@example.com", "0801000058", false, "2026-07-13 10:00:00"});
    list.push_back({"59", "CPE/23/4821", "Olorunnisola Mujeeb Tobiloba", "Computer Engineering", 300, "Male", "olorunnisolamujeebtobiloba@example.com", "0801000059", false, "2026-07-13 10:00:00"});
    list.push_back({"60", "CPE/23/4822", "Omojola Damilola Isaac", "Computer Engineering", 300, "Male", "omojoladamilolaisaac@example.com", "0801000060", false, "2026-07-13 10:00:00"});
    list.push_back({"61", "CPE/23/4823", "Oni Caleb Temiloluwa", "Computer Engineering", 300, "Male", "onicalebtemiloluwa@example.com", "0801000061", false, "2026-07-13 10:00:00"});
    list.push_back({"62", "CPE/23/4824", "Opeoluwa Mustapha Olamide", "Computer Engineering", 300, "Male", "opeoluwamustaphaolamide@example.com", "0801000062", true, "2026-07-13 10:00:00"});
    list.push_back({"63", "CPE/23/4825", "Owoseni Daniels Opeyemi", "Computer Engineering", 300, "Male", "owosenidanielsopeyemi@example.com", "0801000063", false, "2026-07-13 10:00:00"});
    list.push_back({"64", "CPE/23/4826", "Oyedeji Samuel Oluwagbayi", "Computer Engineering", 300, "Male", "oyedejisamueloluwagbayi@example.com", "0801000064", false, "2026-07-13 10:00:00"});
    list.push_back({"65", "CPE/23/4827", "Oyewusi Zion Abiodun", "Computer Engineering", 300, "Male", "oyewusizionabiodun@example.com", "0801000065", false, "2026-07-13 10:00:00"});
    list.push_back({"66", "CPE/23/4828", "Sadiq Gbenga Michael", "Computer Engineering", 300, "Male", "sadiqgbengamichael@example.com", "0801000066", false, "2026-07-13 10:00:00"});
    list.push_back({"67", "CPE/23/4829", "Samuel Joshua Adinoyi", "Computer Engineering", 300, "Male", "samueljoshuaadinoyi@example.com", "0801000067", false, "2026-07-13 10:00:00"});
    list.push_back({"68", "CPE/23/4830", "Shittu Priscilla Adedunni", "Computer Engineering", 300, "Female", "shittupriscillaadedunni@example.com", "0801000068", false, "2026-07-13 10:00:00"});
    list.push_back({"69", "CPE/23/4831", "Shoneye Iqmat Sunmisola", "Computer Engineering", 300, "Female", "shoneyeiqmatsunmisola@example.com", "0801000069", false, "2026-07-13 10:00:00"});
    list.push_back({"70", "CPE/23/4832", "Taiwo James Oriola", "Computer Engineering", 300, "Male", "taiwojamesoriola@example.com", "0801000070", false, "2026-07-13 10:00:00"});
    list.push_back({"71", "CPE/23/4833", "Thomas Boluwatifife Temidayo", "Computer Engineering", 300, "Male", "thomasboluwatififetemidayo@example.com", "0801000071", false, "2026-07-13 10:00:00"});
    list.push_back({"72", "CPE/23/4834", "Tolowoye Iresubomi Sharon", "Computer Engineering", 300, "Female", "tolowoyeiresubomisharon@example.com", "0801000072", false, "2026-07-13 10:00:00"});
    list.push_back({"73", "CPE/23/4835", "Tugbobo Ayodeji Samuel", "Computer Engineering", 300, "Male", "tugboboayodejisamuel@example.com", "0801000073", false, "2026-07-13 10:00:00"});
    list.push_back({"74", "CPE/23/4836", "Wasiu Qudus Akorede", "Computer Engineering", 300, "Male", "wasiuqudusakorede@example.com", "0801000074", false, "2026-07-13 10:00:00"});
    list.push_back({"75", "CPE/24/9860", "Adebowale Ikoghene David", "Computer Engineering", 200, "Male", "adebowaleikoghenedavid@example.com", "0801000075", false, "2026-07-13 10:00:00"});
    list.push_back({"76", "CPE/24/9870", "Ajenifuja Abdullahi Ajeniyi", "Computer Engineering", 200, "Male", "ajenifujaabdullahiajeniyi@example.com", "0801000076", false, "2026-07-13 10:00:00"});
    list.push_back({"77", "CPE/24/9892", "Bamiduro Abiodun Daniel", "Computer Engineering", 200, "Male", "bamiduroabiodundaniel@example.com", "0801000077", false, "2026-07-13 10:00:00"});
    list.push_back({"78", "CPE/24/9893", "Betiku Mercy Olamide", "Computer Engineering", 200, "Female", "betikumercyolamide@example.com", "0801000078", false, "2026-07-13 10:00:00"});
    list.push_back({"79", "CPE/24/9905", "Nasrudeen Mojeed Akin", "Computer Engineering", 200, "Male", "nasrudeenmojeedakin@example.com", "0801000079", false, "2026-07-13 10:00:00"});
    list.push_back({"80", "CPE/24/9917", "Omisore Daniel Damilola", "Computer Engineering", 200, "Male", "omisoredanieldamilola@example.com", "0801000080", false, "2026-07-13 10:00:00"});
    list.push_back({"81", "CPE/24/9919", "Orelesi Elijah Olanrewaju", "Computer Engineering", 200, "Male", "orelesielijaholanrewaju@example.com", "0801000081", false, "2026-07-13 10:00:00"});
    return list;
}

std::vector<Optix::Models::Student> s_mockStudents = createMockStudents();

#else
#include <sqlite3.h>
#endif

#include <iostream>

namespace Optix::Repositories {

StudentRepository::StudentRepository(std::shared_ptr<IDatabase> db)
    : m_db(std::move(db)) {
}

bool StudentRepository::addStudent(const Models::Student& s) {
#ifdef OPTIX_MOCK_BACKEND
    auto copy = s;
    if (copy.student_id.empty()) {
        copy.student_id = std::to_string(s_mockStudents.size() + 1);
    }
    s_mockStudents.push_back(copy);
    return true;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = 
        "INSERT INTO Students (student_id, matric_number, full_name, department, level, gender, email, phone, face_registered) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, s.student_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.matric_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.full_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, s.level);
    sqlite3_bind_text(stmt, 6, s.gender.c_str(), -1, SQLITE_TRANSIENT);
    
    if (s.email.empty()) {
        sqlite3_bind_null(stmt, 7);
    } else {
        sqlite3_bind_text(stmt, 7, s.email.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    if (s.phone.empty()) {
        sqlite3_bind_null(stmt, 8);
    } else {
        sqlite3_bind_text(stmt, 8, s.phone.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    sqlite3_bind_int(stmt, 9, s.face_registered ? 1 : 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

bool StudentRepository::updateStudent(const Models::Student& s) {
#ifdef OPTIX_MOCK_BACKEND
    for (auto& st : s_mockStudents) {
        if (st.student_id == s.student_id) {
            st = s;
            return true;
        }
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = 
        "UPDATE Students SET matric_number = ?, full_name = ?, department = ?, level = ?, gender = ?, email = ?, phone = ?, face_registered = ? "
        "WHERE student_id = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, s.matric_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.full_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, s.level);
    sqlite3_bind_text(stmt, 5, s.gender.c_str(), -1, SQLITE_TRANSIENT);
    
    if (s.email.empty()) {
        sqlite3_bind_null(stmt, 6);
    } else {
        sqlite3_bind_text(stmt, 6, s.email.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    if (s.phone.empty()) {
        sqlite3_bind_null(stmt, 7);
    } else {
        sqlite3_bind_text(stmt, 7, s.phone.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    sqlite3_bind_int(stmt, 8, s.face_registered ? 1 : 0);
    sqlite3_bind_text(stmt, 9, s.student_id.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

bool StudentRepository::deleteStudent(const std::string& student_id) {
#ifdef OPTIX_MOCK_BACKEND
    auto it = std::remove_if(s_mockStudents.begin(), s_mockStudents.end(),
        [&student_id](const auto& s) { return s.student_id == student_id; });
    if (it != s_mockStudents.end()) {
        s_mockStudents.erase(it, s_mockStudents.end());
        return true;
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = "DELETE FROM Students WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

#ifndef OPTIX_MOCK_BACKEND
static Models::Student parseStudent(sqlite3_stmt* stmt) {
    Models::Student s;
    s.student_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    s.matric_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    s.full_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    s.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    s.level = sqlite3_column_int(stmt, 4);
    s.gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    
    if (sqlite3_column_type(stmt, 6) != SQLITE_NULL) {
        s.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    }
    if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
        s.phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    }
    
    s.face_registered = sqlite3_column_int(stmt, 8) != 0;
    s.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
    return s;
}
#endif

std::optional<Models::Student> StudentRepository::getStudentById(const std::string& student_id) {
#ifdef OPTIX_MOCK_BACKEND
    for (const auto& st : s_mockStudents) {
        if (st.student_id == student_id) return st;
    }
    return std::nullopt;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return std::nullopt;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE student_id = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, student_id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Models::Student> student;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student = parseStudent(stmt);
    }
    sqlite3_finalize(stmt);
    return student;
#endif
}

std::optional<Models::Student> StudentRepository::getStudentByMatric(const std::string& matric_number) {
#ifdef OPTIX_MOCK_BACKEND
    for (const auto& st : s_mockStudents) {
        if (st.matric_number == matric_number) return st;
    }
    return std::nullopt;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return std::nullopt;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE matric_number = ?;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, matric_number.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Models::Student> student;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        student = parseStudent(stmt);
    }
    sqlite3_finalize(stmt);
    return student;
#endif
}

std::vector<Models::Student> StudentRepository::getAllStudents() {
#ifdef OPTIX_MOCK_BACKEND
    return s_mockStudents;
#else
    std::vector<Models::Student> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    const std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students ORDER BY full_name ASC;";
        
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseStudent(stmt));
    }
    
    sqlite3_finalize(stmt);
    return list;
#endif
}

std::vector<Models::Student> StudentRepository::searchStudents(const std::string& search_query, const std::string& deptFilter) {
#ifdef OPTIX_MOCK_BACKEND
    std::vector<Models::Student> list;
    for (const auto& st : s_mockStudents) {
        bool matchSearch = search_query.empty() || 
                           (st.full_name.find(search_query) != std::string::npos) || 
                           (st.matric_number.find(search_query) != std::string::npos);
        bool matchDept = deptFilter.empty() || (st.department == deptFilter);
        if (matchSearch && matchDept) {
            list.push_back(st);
        }
    }
    return list;
#else
    std::vector<Models::Student> list;
    sqlite3* handle = m_db->getHandle();
    if (!handle) return list;

    // Build the query dynamically depending on filters
    std::string query = 
        "SELECT student_id, matric_number, full_name, department, level, gender, email, phone, face_registered, created_at "
        "FROM Students WHERE 1=1";
        
    if (!search_query.empty()) {
        query += " AND (full_name LIKE ? OR matric_number LIKE ? OR student_id LIKE ?)";
    }
    if (!deptFilter.empty()) {
        query += " AND department = ?";
    }
    query += " ORDER BY full_name ASC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    int bindIdx = 1;
    if (!search_query.empty()) {
        std::string wild = "%" + search_query + "%";
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, bindIdx++, wild.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (!deptFilter.empty()) {
        sqlite3_bind_text(stmt, bindIdx++, deptFilter.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(parseStudent(stmt));
    }
    
    sqlite3_finalize(stmt);
    return list;
#endif
}

int StudentRepository::getStudentCount() {
#ifdef OPTIX_MOCK_BACKEND
    return static_cast<int>(s_mockStudents.size());
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return 0;

    const std::string query = "SELECT COUNT(*) FROM Students;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return 0;

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
#endif
}

bool StudentRepository::setFaceRegistered(const std::string& student_id, bool registered) {
#ifdef OPTIX_MOCK_BACKEND
    for (auto& st : s_mockStudents) {
        if (st.student_id == student_id) {
            st.face_registered = registered;
            return true;
        }
    }
    return false;
#else
    sqlite3* handle = m_db->getHandle();
    if (!handle) return false;

    const std::string query = "UPDATE Students SET face_registered = ? WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, registered ? 1 : 0);
    sqlite3_bind_text(stmt, 2, student_id.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
#endif
}

} // namespace Optix::Repositories

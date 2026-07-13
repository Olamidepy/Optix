#pragma once

#ifdef OPTIX_MOCK_BACKEND

#include <string>
#include <vector>
#include <map>
#include <iostream>

// --- Mock OpenCV ---
namespace cv {

    class Mat {
    public:
        Mat() : m_cols(0), m_rows(0) {}
        Mat(int rows, int cols, int type) : m_cols(cols), m_rows(rows) { (void)type; }
        
        bool empty() const { return true; }
        int channels() const { return 1; }
        int cols() const { return m_cols; }
        int rows() const { return m_rows; }

    private:
        int m_cols;
        int m_rows;
    };

    enum ImreadModes {
        IMREAD_UNCHANGED = -1,
        IMREAD_GRAYSCALE = 0,
        IMREAD_COLOR = 1
    };

    inline Mat imread(const std::string&, int = IMREAD_COLOR) {
        return Mat();
    }

    inline bool imwrite(const std::string&, const Mat&) {
        return true;
    }

    template<typename T>
    class Ptr {
    public:
        Ptr() : m_ptr(nullptr) {}
        Ptr(T* p) : m_ptr(p) {}
        ~Ptr() {
            delete m_ptr;
        }
        
        Ptr(const Ptr& other) : m_ptr(other.m_ptr ? new T(*other.m_ptr) : nullptr) {}
        Ptr& operator=(const Ptr& other) {
            if (this != &other) {
                delete m_ptr;
                m_ptr = other.m_ptr ? new T(*other.m_ptr) : nullptr;
            }
            return *this;
        }

        bool empty() const { return m_ptr == nullptr; }
        T* operator->() const { return m_ptr; }
        T& operator*() const { return *m_ptr; }

    private:
        T* m_ptr;
    };

    namespace face {
        class FaceRecognizer {
        public:
            virtual ~FaceRecognizer() = default;
            virtual void train(const std::vector<Mat>&, const std::vector<int>&) {}
            virtual void predict(const Mat&, int&, double&) {}
            virtual void write(const std::string&) const {}
            virtual void read(const std::string&) {}
        };

        class LBPHFaceRecognizer : public FaceRecognizer {
        public:
            static Ptr<LBPHFaceRecognizer> create() {
                return Ptr<LBPHFaceRecognizer>(new LBPHFaceRecognizer());
            }
        };
    }

    enum FileStorageMode {
        READ = 0,
        WRITE = 1,
        APPEND = 2
    };

    class FileNode {
    public:
        enum Type {
            NONE = 0,
            INT = 1,
            REAL = 2,
            STR = 3,
            SEQ = 4,
            MAP = 5
        };

        FileNode() = default;
        int type() const { return SEQ; }
        FileNode operator[](const std::string&) const { return FileNode(); }
        
        operator int() const { return 0; }
        operator std::string() const { return ""; }
        operator double() const { return 0.0; }

        class iterator {
        public:
            iterator() = default;
            bool operator!=(const iterator&) const { return false; }
            iterator& operator++() { return *this; }
            FileNode operator*() const { return FileNode(); }
        };

        iterator begin() const { return iterator(); }
        iterator end() const { return iterator(); }
    };

    class FileStorage {
    public:
        static const int READ = 0;
        static const int WRITE = 1;
        static const int APPEND = 2;

        FileStorage() = default;
        FileStorage(const std::string&, int) {}
        bool isOpened() const { return true; }
        void release() {}
        FileNode operator[](const std::string&) const { return FileNode(); }
    };

    template<typename T>
    inline FileStorage& operator<<(FileStorage& fs, const T&) {
        return fs;
    }
}

// --- Mock SQLite3 ---
struct sqlite3 {};
struct sqlite3_stmt {};

#define SQLITE_OK 0
#define SQLITE_ROW 100
#define SQLITE_DONE 101
#define SQLITE_TRANSIENT nullptr

// Models forward declarations
namespace Optix::Models {
    struct Student;
    struct Attendance;
}

// Global in-memory storage for MOCK mode
extern std::vector<Optix::Models::Student> s_mockStudents;
extern std::vector<Optix::Models::Attendance> s_mockAttendance;

inline int sqlite3_open(const char*, sqlite3**) { return SQLITE_OK; }
inline int sqlite3_close(sqlite3*) { return SQLITE_OK; }
inline const char* sqlite3_errmsg(sqlite3*) { return "Mock SQLite Error"; }
inline int sqlite3_exec(sqlite3*, const char*, int (*)(void*,int,char**,char**), void*, char**) { return SQLITE_OK; }
inline void sqlite3_free(void*) {}

inline int sqlite3_prepare_v2(sqlite3*, const char*, int, sqlite3_stmt**, const char**) { return SQLITE_OK; }
inline int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int, void(*)(void*)) { return SQLITE_OK; }
inline int sqlite3_bind_int(sqlite3_stmt*, int, int) { return SQLITE_OK; }
inline int sqlite3_bind_double(sqlite3_stmt*, int, double) { return SQLITE_OK; }
inline int sqlite3_step(sqlite3_stmt*) { return SQLITE_DONE; }
inline int sqlite3_column_int(sqlite3_stmt*, int) { return 0; }
inline const unsigned char* sqlite3_column_text(sqlite3_stmt*, int) { return reinterpret_cast<const unsigned char*>(""); }
inline double sqlite3_column_double(sqlite3_stmt*, int) { return 0.0; }
inline int sqlite3_finalize(sqlite3_stmt*) { return SQLITE_OK; }

#endif // OPTIX_MOCK_BACKEND

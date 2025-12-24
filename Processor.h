#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <filesystem>
#include "Student.h"

class Processor {
public:
    // ===== VECTOR VERSIONS =====
    static std::vector<Student> readStudentsVector(const std::string& filename);
    static void sortStudents(std::vector<Student>& students);
    static void splitStrategy1(
        const std::vector<Student>& students,
        std::vector<Student>& passed,
        std::vector<Student>& failed
    );
    static void splitStrategy2(
        std::vector<Student>& students,
        std::vector<Student>& failed
    );
    static void saveStudents(
        const std::string& filename,
        const std::vector<Student>& list
    );

    // ===== LIST VERSIONS =====
    static std::list<Student> readStudentsList(const std::string& filename);
    static void sortStudents(std::list<Student>& students);
    static void splitStrategy1(
        const std::list<Student>& students,
        std::list<Student>& passed,
        std::list<Student>& failed
    );
    static void splitStrategy2(
        std::list<Student>& students,
        std::list<Student>& failed
    );
    static void saveStudents(
        const std::string& filename,
        const std::list<Student>& list
    );

    // ===== DEQUE VERSIONS =====
    static std::deque<Student> readStudentsDeque(const std::string& filename);
    static void sortStudents(std::deque<Student>& students);
    static void splitStrategy1(
        const std::deque<Student>& students,
        std::deque<Student>& passed,
        std::deque<Student>& failed
    );
    static void splitStrategy2(
        std::deque<Student>& students,
        std::deque<Student>& failed
    );
    static void saveStudents(
        const std::string& filename,
        const std::deque<Student>& list
    );

// ===== FILESYSTEM::PATH OVERLOADS (for GUI / Unicode paths) =====
static std::vector<Student> readStudentsVector(const std::filesystem::path& filename);
static std::list<Student> readStudentsList(const std::filesystem::path& filename);
static std::deque<Student> readStudentsDeque(const std::filesystem::path& filename);

static void saveStudents(const std::filesystem::path& filename, const std::vector<Student>& list);
static void saveStudents(const std::filesystem::path& filename, const std::list<Student>& list);
static void saveStudents(const std::filesystem::path& filename, const std::deque<Student>& list);

};

#endif

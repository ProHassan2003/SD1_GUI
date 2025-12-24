#include "Processor.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

// Helper predicate
static bool isPassed(const Student& s) {
    return s.getFinalScore() >= 5.0;
}

// ==================== VECTOR ====================

std::vector<Student> Processor::readStudentsVector(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    std::vector<Student> students;
    std::string line;

    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string fn, ln;
        std::vector<int> hw(5);
        int exam;

        ss >> fn >> ln;
        for (int i = 0; i < 5; i++)
            ss >> hw[i];
        ss >> exam;

        students.emplace_back(fn, ln, hw, exam);
    }

    return students;
}

void Processor::sortStudents(std::vector<Student>& students) {
    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) {
                  return a.getFinalScore() < b.getFinalScore();
              });
}

// Strategy 1: copy into two containers (passed + failed)
void Processor::splitStrategy1(
        const std::vector<Student>& students,
        std::vector<Student>& passed,
        std::vector<Student>& failed
    )
{
    passed.clear();
    failed.clear();

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(passed),
                 isPassed);

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(failed),
                 [](const Student& s){ return !isPassed(s); });
}

// Strategy 2: move failed to new container, remove from base (keep only passed)
void Processor::splitStrategy2(
        std::vector<Student>& students,
        std::vector<Student>& failed
    )
{
    failed.clear();

    // Copy failed students into "failed" using remove_copy_if
    std::remove_copy_if(students.begin(), students.end(),
                        std::back_inserter(failed),
                        isPassed); // copies those NOT passed

    // Remove failed students from original using remove_if + erase
    auto newEnd = std::remove_if(students.begin(), students.end(),
                                 [](const Student& s){ return !isPassed(s); });
    students.erase(newEnd, students.end());
}

void Processor::saveStudents(
        const std::string& filename,
        const std::vector<Student>& list
    )
{
    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "FirstName LastName FinalScore\n";

    for (const auto& s : list) {
        out << s.getFirstName() << " "
            << s.getLastName() << " "
            << s.getFinalScore() << "\n";
    }

    out.close();
}


// ==================== LIST ====================

std::list<Student> Processor::readStudentsList(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    std::list<Student> students;
    std::string line;

    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string fn, ln;
        std::vector<int> hw(5);
        int exam;

        ss >> fn >> ln;
        for (int i = 0; i < 5; i++)
            ss >> hw[i];
        ss >> exam;

        students.emplace_back(fn, ln, hw, exam);
    }

    return students;
}

void Processor::sortStudents(std::list<Student>& students) {
    students.sort([](const Student& a, const Student& b) {
        return a.getFinalScore() < b.getFinalScore();
    });
}

void Processor::splitStrategy1(
        const std::list<Student>& students,
        std::list<Student>& passed,
        std::list<Student>& failed
    )
{
    passed.clear();
    failed.clear();

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(passed),
                 isPassed);

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(failed),
                 [](const Student& s){ return !isPassed(s); });
}

// Strategy 2 for list: use list::remove_if
void Processor::splitStrategy2(
        std::list<Student>& students,
        std::list<Student>& failed
    )
{
    failed.clear();

    std::remove_copy_if(students.begin(), students.end(),
                        std::back_inserter(failed),
                        isPassed); // failed go into "failed"

    students.remove_if([](const Student& s){ return !isPassed(s); });
}

void Processor::saveStudents(
        const std::string& filename,
        const std::list<Student>& list
    )
{
    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "FirstName LastName FinalScore\n";

    for (const auto& s : list) {
        out << s.getFirstName() << " "
            << s.getLastName() << " "
            << s.getFinalScore() << "\n";
    }

    out.close();
}


// ==================== DEQUE ====================

std::deque<Student> Processor::readStudentsDeque(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    std::deque<Student> students;
    std::string line;

    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string fn, ln;
        std::vector<int> hw(5);
        int exam;

        ss >> fn >> ln;
        for (int i = 0; i < 5; i++)
            ss >> hw[i];
        ss >> exam;

        students.emplace_back(fn, ln, hw, exam);
    }

    return students;
}

void Processor::sortStudents(std::deque<Student>& students) {
    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) {
                  return a.getFinalScore() < b.getFinalScore();
              });
}

void Processor::splitStrategy1(
        const std::deque<Student>& students,
        std::deque<Student>& passed,
        std::deque<Student>& failed
    )
{
    passed.clear();
    failed.clear();

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(passed),
                 isPassed);

    std::copy_if(students.begin(), students.end(),
                 std::back_inserter(failed),
                 [](const Student& s){ return !isPassed(s); });
}

void Processor::splitStrategy2(
        std::deque<Student>& students,
        std::deque<Student>& failed
    )
{
    failed.clear();

    std::remove_copy_if(students.begin(), students.end(),
                        std::back_inserter(failed),
                        isPassed); // copy failed

    auto newEnd = std::remove_if(students.begin(), students.end(),
                                 [](const Student& s){ return !isPassed(s); });
    students.erase(newEnd, students.end());
}

void Processor::saveStudents(
        const std::string& filename,
        const std::deque<Student>& list
    )
{
    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "FirstName LastName FinalScore\n";

    for (const auto& s : list) {
        out << s.getFirstName() << " "
            << s.getLastName() << " "
            << s.getFinalScore() << "\n";
    }

    out.close();
}


// ============================
// Filesystem::path overloads
// ============================

std::vector<Student> Processor::readStudentsVector(const std::filesystem::path& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename.string());
    }
    std::vector<Student> students;
    students.reserve(1000);

    std::string first, last;
    while (in >> first >> last) {

        // skip header row if present
        if (first == "FirstName" && last == "LastName") {
            std::string restOfLine;
            std::getline(in, restOfLine);
            continue;
        }

        Student s;
        s.setFirstName(first);
        s.setLastName(last);

        std::vector<int> hw(5, 0);
        int exam = 0;

        bool ok = true;
        for (int i = 0; i < 5; i++) {
            if (!(in >> hw[i])) { ok = false; break; }
        }
        if (!ok) break;

        if (!(in >> exam)) break;

        s.setHomework(hw);
        s.setExam(exam);
        s.calculateFinal();
        students.push_back(std::move(s));
    }

    return students;
}

std::list<Student> Processor::readStudentsList(const std::filesystem::path& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename.string());
    }
    std::list<Student> students;

    std::string first, last;
    while (in >> first >> last) {

        // skip header row if present
        if (first == "FirstName" && last == "LastName") {
            std::string restOfLine;
            std::getline(in, restOfLine);
            continue;
        }

        Student s;
        s.setFirstName(first);
        s.setLastName(last);

        std::vector<int> hw(5, 0);
        int exam = 0;

        bool ok = true;
        for (int i = 0; i < 5; i++) {
            if (!(in >> hw[i])) { ok = false; break; }
        }
        if (!ok) break;

        if (!(in >> exam)) break;

        s.setHomework(hw);
        s.setExam(exam);
        s.calculateFinal();
        students.push_back(std::move(s));
    }



    return students;
}

std::deque<Student> Processor::readStudentsDeque(const std::filesystem::path& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename.string());
    }
    std::deque<Student> students;

    std::string first, last;
    while (in >> first >> last) {

        // skip header row if present
        if (first == "FirstName" && last == "LastName") {
            std::string restOfLine;
            std::getline(in, restOfLine);
            continue;
        }

        Student s;
        s.setFirstName(first);
        s.setLastName(last);

        std::vector<int> hw(5, 0);
        int exam = 0;

        bool ok = true;
        for (int i = 0; i < 5; i++) {
            if (!(in >> hw[i])) { ok = false; break; }
        }
        if (!ok) break;

        if (!(in >> exam)) break;

        s.setHomework(hw);
        s.setExam(exam);
        s.calculateFinal();
        students.push_back(std::move(s));
    }

    return students;
}

void Processor::saveStudents(const std::filesystem::path& filename, const std::vector<Student>& list) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to write file: " + filename.string());
    }
    for (const auto& s : list) {
        out << s.getFirstName() << " " << s.getLastName() << " " << s.getFinalScore() << "\n";
    }
}

void Processor::saveStudents(const std::filesystem::path& filename, const std::list<Student>& list) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to write file: " + filename.string());
    }
    for (const auto& s : list) {
        out << s.getFirstName() << " " << s.getLastName() << " " << s.getFinalScore() << "\n";
    }
}

void Processor::saveStudents(const std::filesystem::path& filename, const std::deque<Student>& list) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to write file: " + filename.string());
    }
    for (const auto& s : list) {
        out << s.getFirstName() << " " << s.getLastName() << " " << s.getFinalScore() << "\n";
    }
}

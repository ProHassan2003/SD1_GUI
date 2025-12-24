#ifndef FILEGENERATOR_H
#define FILEGENERATOR_H

#include <string>
#include <filesystem>

class FileGenerator {
public:
    // Generates a file with N random student records:
    // NameX SurnameX HW1 HW2 HW3 HW4 HW5 Exam
    static void generateStudentFile(const std::string& filename, long long count);
    static void generateStudentFile(const std::filesystem::path& filename, long long count);
};

#endif

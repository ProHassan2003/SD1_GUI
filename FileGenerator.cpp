#include "FileGenerator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <random>

void FileGenerator::generateStudentFile(const std::string& filename, long long count) {
    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot create file: " + filename);
    }

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> gradeDist(1, 10);

    // Header
    out << "FirstName LastName HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (long long i = 1; i <= count; i++) {
        out << "Name" << i << " "
            << "Surname" << i << " "
            << gradeDist(gen) << " "
            << gradeDist(gen) << " "
            << gradeDist(gen) << " "
            << gradeDist(gen) << " "
            << gradeDist(gen) << " "
            << gradeDist(gen) << "\n";

        if (i % 1000000 == 0)
            std::cout << i << " records generated...\n";
    }

    out.close();
    std::cout << "File created: " << filename << "\n";
}



void FileGenerator::generateStudentFile(const std::filesystem::path& filename, long long count) {
    // Same implementation as the std::string overload, but with Unicode-safe path support.
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to create file: " + filename.string());
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);

    // Header
    out << "FirstName LastName HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (long long i = 1; i <= count; i++) {
        out << "Name" << i << " " << "Surname" << i << " ";
        for (int j = 0; j < 5; j++) {
            out << dis(gen) << " ";
        }
        out << dis(gen) << "\n";
    }
}

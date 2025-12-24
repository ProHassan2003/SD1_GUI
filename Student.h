#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

class Student {
private:
    std::string firstName;
    std::string lastName;
    std::vector<int> homework;
    int exam;
    double finalScore;

public:
    Student();
    Student(const std::string& fn, const std::string& ln,
            const std::vector<int>& hw, int ex);

    // Setters
    void setFirstName(const std::string& fn);
    void setLastName(const std::string& ln);
    void setHomework(const std::vector<int>& hw);
    void setExam(int ex);

    // Getters
    std::string getFirstName() const;
    std::string getLastName() const;
    std::vector<int> getHomework() const;
    int getExam() const;
    double getFinalScore() const;

    // Calculation using formula:
    // final = 0.3 * homework_avg + 0.7 * exam
    void calculateFinal();
};

#endif

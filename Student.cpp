#include "Student.h"
#include <numeric>

Student::Student() : exam(0), finalScore(0.0) {}

Student::Student(const std::string& fn, const std::string& ln,
                 const std::vector<int>& hw, int ex)
        : firstName(fn), lastName(ln), homework(hw), exam(ex) {
    calculateFinal();
}

void Student::setFirstName(const std::string& fn) { firstName = fn; }
void Student::setLastName(const std::string& ln) { lastName = ln; }
void Student::setHomework(const std::vector<int>& hw) { homework = hw; }
void Student::setExam(int ex) { exam = ex; }

std::string Student::getFirstName() const { return firstName; }
std::string Student::getLastName() const { return lastName; }
std::vector<int> Student::getHomework() const { return homework; }
int Student::getExam() const { return exam; }
double Student::getFinalScore() const { return finalScore; }

void Student::calculateFinal() {
    if (homework.empty()) {
        finalScore = 0;
        return;
    }

    double hw_avg = std::accumulate(homework.begin(), homework.end(), 0.0)
                    / homework.size();

    finalScore = 0.3 * hw_avg + 0.7 * exam;
}

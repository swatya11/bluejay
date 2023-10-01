#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip> 

const int CONSECUTIVE_DAYS_THRESHOLD = 7;
const int MINIMUM_TIME_BETWEEN_SHIFTS_HOURS = 1;
const int MAXIMUM_SHIFT_DURATION_HOURS = 14;

// Define a structure to hold employee information
struct Employee {
    std::string name;
    std::string position;
};

// Function to parse date-time string in the given format
std::tm parseDateTime(const std::string& datetime_str) {
    std::tm tm = {};
    std::istringstream ss(datetime_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return tm;
}

// Function to check if an employee has worked for 7 consecutive days
bool hasWorkedConsecutiveDays(const std::vector<std::pair<std::string, std::string>>& shifts) {
    int consecutiveDays = 0;
    for (size_t i = 0; i < shifts.size() - 1; ++i) {
        std::tm currentShiftEnd = parseDateTime(shifts[i].second);
        std::tm nextShiftStart = parseDateTime(shifts[i + 1].first);
        std::time_t currentShiftEndEpoch = std::mktime(&currentShiftEnd);
        std::time_t nextShiftStartEpoch = std::mktime(&nextShiftStart);

        // Calculate the time difference in seconds
        double timeDifference = std::difftime(nextShiftStartEpoch, currentShiftEndEpoch);

        if (timeDifference / (60 * 60 * 24) == 1) { // 1 day
            consecutiveDays++;
            if (consecutiveDays == CONSECUTIVE_DAYS_THRESHOLD) {
                return true;
            }
        } else {
            consecutiveDays = 0;
        }
    }
    return false;
}

// Function to check if an employee has less than 10 hours but more than 1 hour between shifts
bool hasShortBreaks(const std::vector<std::pair<std::string, std::string>>& shifts) {
    for (size_t i = 0; i < shifts.size() - 1; ++i) {
        std::tm currentShiftEnd = parseDateTime(shifts[i].second);
        std::tm nextShiftStart = parseDateTime(shifts[i + 1].first);
        std::time_t currentShiftEndEpoch = std::mktime(&currentShiftEnd);
        std::time_t nextShiftStartEpoch = std::mktime(&nextShiftStart);

        // Calculate the time difference in seconds
        double timeDifference = std::difftime(nextShiftStartEpoch, currentShiftEndEpoch);

        if (timeDifference / (60 * 60) > MINIMUM_TIME_BETWEEN_SHIFTS_HOURS &&
            timeDifference / (60 * 60) < 10) {
            return true;
        }
    }
    return false;
}

// Function to check if an employee has worked for more than 14 hours in a single shift
bool hasLongShift(const std::pair<std::string, std::string>& shift) {
    std::tm shiftStart = parseDateTime(shift.first);
    std::tm shiftEnd = parseDateTime(shift.second);
    std::time_t shiftStartEpoch = std::mktime(&shiftStart);
    std::time_t shiftEndEpoch = std::mktime(&shiftEnd);

    // Calculate the shift duration in seconds
    double shiftDuration = std::difftime(shiftEndEpoch, shiftStartEpoch);

    return shiftDuration / (60 * 60) > MAXIMUM_SHIFT_DURATION_HOURS;
}

int main() {
    std::string fileName;
    std::cout << "Enter the name of the input CSV file: ";
    std::cin >> fileName;

    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::pair<std::string, std::string>> shifts;
    std::string currentEmployee;
    std::string currentPosition;
    std::vector<Employee> employees;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string employee, position, shiftStart, shiftEnd;
        getline(iss, employee, ',');
        getline(iss, position, ',');
        getline(iss, shiftStart, ',');
        getline(iss, shiftEnd, ',');

        if (employee != currentEmployee) {
            if (!currentEmployee.empty()) {
                if (hasWorkedConsecutiveDays(shifts)) {
                    std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has worked for 7 consecutive days." << std::endl;
                }
                if (hasShortBreaks(shifts)) {
                    std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has less than 10 hours but more than 1 hour between shifts." << std::endl;
                }
                for (const auto& shift : shifts) {
                    if (hasLongShift(shift)) {
                        std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has worked for more than 14 hours in a single shift." << std::endl;
                        break; // No need to check further shifts for this employee
                    }
                }
            }
            currentEmployee = employee;
            currentPosition = position;
            shifts.clear();
        }
        shifts.push_back(std::make_pair(shiftStart, shiftEnd));
    }

    // Check the last employee in the file
    if (!currentEmployee.empty()) {
        if (hasWorkedConsecutiveDays(shifts)) {
            std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has worked for 7 consecutive days." << std::endl;
        }
        if (hasShortBreaks(shifts)) {
            std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has less than 10 hours but more than 1 hour between shifts." << std::endl;
        }
        for (const auto& shift : shifts) {
            if (hasLongShift(shift)) {
                std::cout << "Employee: " << currentEmployee << ", Position: " << currentPosition << " has worked for more than 14 hours in a single shift." << std::endl;
                break; // No need to check further shifts for this employee
            }
        }
    }

    inputFile.close();
    return 0;
}

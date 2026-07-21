/**
 * @file CSVReader.cpp
 * @brief Implementation of the CSVReader class.
 */

#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

CSVReader::CSVReader(const std::string& path) : filePath(path) {}

std::vector<std::string> CSVReader::parseLine(const std::string& line) const {
    // Small state machine to correctly handle quoted fields containing commas.
    std::vector<std::string> fields;
    std::string currentField;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char currentChar = line[i];

        if (currentChar == '"') {
            // Toggle quote state. Handles escaped double-quotes ("") minimally.
            if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                currentField += '"';
                ++i; // skip the escaped quote
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (currentChar == ',' && !insideQuotes) {
            fields.push_back(currentField);
            currentField.clear();
        } else {
            currentField += currentChar;
        }
    }
    fields.push_back(currentField); // last field after final comma

    return fields;
}

std::vector<Record> CSVReader::readRecords() {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        // Robust error handling: caller (main.cpp) is expected to catch this.
        throw std::runtime_error("Unable to open file: " + filePath +
                                  " (file may be missing or inaccessible)");
    }

    std::vector<Record> records;
    std::string line;
    int rowNumber = 0;

    // First line is treated as the header.
    if (std::getline(inputFile, line)) {
        headerColumns = parseLine(line);
    } else {
        throw std::runtime_error("File is empty: " + filePath);
    }

    // Remaining lines are data rows.
    while (std::getline(inputFile, line)) {
        ++rowNumber;

        // Skip completely blank lines (common at end of CSV files).
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        std::vector<std::string> fieldValues = parseLine(line);
        records.emplace_back(fieldValues, rowNumber);
    }

    inputFile.close();

    if (records.empty()) {
        throw std::runtime_error("No data records found in file: " + filePath);
    }

    return records;
}

const std::vector<std::string>& CSVReader::getHeaderColumns() const {
    return headerColumns;
}

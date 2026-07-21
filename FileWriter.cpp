/**
 * @file FileWriter.cpp
 * @brief Implementation of FileWriter.
 */

#include "FileWriter.h"
#include <fstream>
#include <stdexcept>

void FileWriter::writeCleanedCSV(const std::string& filePath,
                                  const std::vector<std::string>& headerColumns,
                                  const std::vector<Record>& uniqueRecords) {
    std::ofstream outputFile(filePath);

    if (!outputFile.is_open()) {
        throw std::runtime_error("Unable to create output file: " + filePath);
    }

    // Write header row.
    for (size_t i = 0; i < headerColumns.size(); ++i) {
        outputFile << headerColumns[i];
        if (i + 1 < headerColumns.size()) {
            outputFile << ',';
        }
    }
    outputFile << '\n';

    // Write each unique record.
    for (const Record& record : uniqueRecords) {
        outputFile << record.toCSVLine() << '\n';
    }

    outputFile.close();
}

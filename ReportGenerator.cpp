/**
 * @file ReportGenerator.cpp
 * @brief Implementation of ReportGenerator.
 */

#include "ReportGenerator.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

DeduplicationStats ReportGenerator::computeStats(const std::vector<Record>& allRecords,
                                                   const std::vector<Record>& uniqueRecords,
                                                   long long collisionsResolved,
                                                   double durationMilliseconds) {
    DeduplicationStats stats{};

    stats.totalRecords       = allRecords.size();
    stats.uniqueRecords      = uniqueRecords.size();
    stats.duplicateRecords   = stats.totalRecords - stats.uniqueRecords;
    stats.collisionsResolved = collisionsResolved;
    stats.durationMilliseconds = durationMilliseconds;

    // Approximate byte size by summing the length of each record's CSV line.
    size_t rawBytes = 0;
    for (const Record& record : allRecords) {
        rawBytes += record.toCSVLine().size();
    }

    size_t cleanedBytes = 0;
    for (const Record& record : uniqueRecords) {
        cleanedBytes += record.toCSVLine().size();
    }

    stats.rawBytes = rawBytes;
    stats.cleanedBytes = cleanedBytes;

    return stats;
}

void ReportGenerator::printToConsole(const DeduplicationStats& stats) {
    double duplicatePercentage = stats.totalRecords == 0 ? 0.0 :
        (static_cast<double>(stats.duplicateRecords) / static_cast<double>(stats.totalRecords)) * 100.0;

    size_t bytesSaved = (stats.rawBytes >= stats.cleanedBytes) ? (stats.rawBytes - stats.cleanedBytes) : 0;
    double percentageSaved = stats.rawBytes == 0 ? 0.0 :
        (static_cast<double>(bytesSaved) / static_cast<double>(stats.rawBytes)) * 100.0;

    std::cout << "\n========== DEDUPLICATION SUMMARY ==========\n";
    std::cout << std::left << std::setw(28) << "Total Records Loaded"   << ": " << stats.totalRecords << '\n';
    std::cout << std::left << std::setw(28) << "Unique Records"         << ": " << stats.uniqueRecords << '\n';
    std::cout << std::left << std::setw(28) << "Duplicate Records"      << ": " << stats.duplicateRecords << '\n';
    std::cout << std::left << std::setw(28) << "Duplicate Percentage"   << ": "
              << std::fixed << std::setprecision(2) << duplicatePercentage << " %\n";
    std::cout << std::left << std::setw(28) << "Hash Collisions Resolved" << ": " << stats.collisionsResolved << '\n';
    std::cout << std::left << std::setw(28) << "Raw Data Size"          << ": " << stats.rawBytes << " bytes\n";
    std::cout << std::left << std::setw(28) << "Cleaned Data Size"      << ": " << stats.cleanedBytes << " bytes\n";
    std::cout << std::left << std::setw(28) << "Storage Saved"          << ": " << bytesSaved << " bytes ("
              << std::fixed << std::setprecision(2) << percentageSaved << " %)\n";
    std::cout << std::left << std::setw(28) << "Execution Time"         << ": "
              << std::fixed << std::setprecision(3) << stats.durationMilliseconds << " ms\n";
    std::cout << "=============================================\n";
}

void ReportGenerator::writeReportFile(const std::string& filePath, const DeduplicationStats& stats) {
    std::ofstream reportFile(filePath);

    if (!reportFile.is_open()) {
        throw std::runtime_error("Unable to create report file: " + filePath);
    }

    double duplicatePercentage = stats.totalRecords == 0 ? 0.0 :
        (static_cast<double>(stats.duplicateRecords) / static_cast<double>(stats.totalRecords)) * 100.0;

    size_t bytesSaved = (stats.rawBytes >= stats.cleanedBytes) ? (stats.rawBytes - stats.cleanedBytes) : 0;
    double percentageSaved = stats.rawBytes == 0 ? 0.0 :
        (static_cast<double>(bytesSaved) / static_cast<double>(stats.rawBytes)) * 100.0;

    std::time_t now = std::time(nullptr);
    char timeBuffer[64];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    reportFile << "==================================================\n";
    reportFile << "     DATA DEDUPLICATION ENGINE - REPORT\n";
    reportFile << "==================================================\n";
    reportFile << "Generated On          : " << timeBuffer << "\n\n";

    reportFile << "--- Record Statistics ---\n";
    reportFile << "Total Records          : " << stats.totalRecords << "\n";
    reportFile << "Unique Records          : " << stats.uniqueRecords << "\n";
    reportFile << "Duplicate Records       : " << stats.duplicateRecords << "\n";
    reportFile << "Duplicate Percentage    : " << std::fixed << std::setprecision(2)
               << duplicatePercentage << " %\n";
    reportFile << "Hash Collisions Resolved: " << stats.collisionsResolved << "\n\n";

    reportFile << "--- Storage Statistics ---\n";
    reportFile << "Raw Data Size           : " << stats.rawBytes << " bytes\n";
    reportFile << "Cleaned Data Size       : " << stats.cleanedBytes << " bytes\n";
    reportFile << "Storage Saved           : " << bytesSaved << " bytes ("
               << std::fixed << std::setprecision(2) << percentageSaved << " %)\n\n";

    reportFile << "--- Performance ---\n";
    reportFile << "Execution Time          : " << std::fixed << std::setprecision(3)
               << stats.durationMilliseconds << " ms\n\n";

    reportFile << "--- Algorithm Used ---\n";
    reportFile << "Hashing Algorithm       : FNV-1a (64-bit)\n";
    reportFile << "Duplicate Detection     : Hash-bucket + verification\n";
    reportFile << "                          (unordered_map<hash, vector<index>>)\n";
    reportFile << "==================================================\n";

    reportFile.close();
}

/**
 * @file ReportGenerator.h
 * @brief Declares ReportGenerator - computes summary statistics from a
 *        completed deduplication run and writes/displays a report.
 *
 * Responsibilities:
 *  - Compute Total / Unique / Duplicate counts and duplicate percentage.
 *  - Estimate "storage saved" by comparing byte-size of raw vs cleaned data.
 *  - Record execution time (measured externally via chrono, passed in).
 *  - Write report.txt and print the same summary to console.
 */

#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <string>
#include <vector>
#include "Record.h"

// Plain data holder describing everything needed to produce a report.
// Kept as a simple struct (not a class) since it has no behavior of its own -
// it is purely a transport object between DeduplicationEngine/main and ReportGenerator.
struct DeduplicationStats {
    size_t totalRecords;
    size_t uniqueRecords;
    size_t duplicateRecords;
    long long collisionsResolved;
    double durationMilliseconds;
    size_t rawBytes;      // approximate size of original dataset in memory
    size_t cleanedBytes;  // approximate size of cleaned dataset in memory
};

class ReportGenerator {
public:
    /**
     * computeStats()
     * Purpose : Derives byte-size figures from record sets and packages
     *           all statistics into a DeduplicationStats struct.
     * Time    : O(N * L)  Space: O(1) additional
     */
    static DeduplicationStats computeStats(const std::vector<Record>& allRecords,
                                            const std::vector<Record>& uniqueRecords,
                                            long long collisionsResolved,
                                            double durationMilliseconds);

    /**
     * printToConsole()
     * Purpose : Displays the statistics in a readable, formatted way on
     *           the console.
     * Time    : O(1) (fixed number of fields)
     */
    static void printToConsole(const DeduplicationStats& stats);

    /**
     * writeReportFile()
     * Purpose : Writes a detailed, formatted report.txt file containing
     *           all statistics required by the project specification.
     * Throws  : std::runtime_error if the file cannot be created
     * Time    : O(1)
     */
    static void writeReportFile(const std::string& filePath, const DeduplicationStats& stats);
};

#endif // REPORT_GENERATOR_H

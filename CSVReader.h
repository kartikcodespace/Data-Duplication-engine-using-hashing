/**
 * @file CSVReader.h
 * @brief Declares CSVReader - responsible for reading a CSV file from
 *        disk and parsing it into a vector of Record objects.
 *
 * Design notes:
 *  - Handles quoted fields (e.g. "Doe, John") correctly using a small
 *    state machine, rather than a naive split-on-comma, which would
 *    break on real-world data.
 *  - Throws exceptions (std::runtime_error) on missing/unreadable files
 *    so the caller (main.cpp) can handle errors gracefully via try/catch,
 *    satisfying the "robust error handling" requirement.
 *  - Does not know about hashing or deduplication (Single Responsibility).
 */

#ifndef CSV_READER_H
#define CSV_READER_H

#include <string>
#include <vector>
#include "Record.h"

class CSVReader {
private:
    std::string filePath;
    std::vector<std::string> headerColumns;

    /**
     * parseLine()
     * Purpose : Splits a single CSV line into fields, correctly handling
     *           quoted fields that may contain commas.
     * Input   : const string& line
     * Output  : vector<string> of parsed field values
     * Time    : O(L) where L = length of the line
     * Space   : O(L)
     */
    std::vector<std::string> parseLine(const std::string& line) const;

public:
    explicit CSVReader(const std::string& path);

    /**
     * readRecords()
     * Purpose : Opens the CSV file, reads the header row, then parses
     *           every subsequent line into a Record object.
     * Input   : none (uses filePath set in constructor)
     * Output  : vector<Record> containing all parsed rows
     * Throws  : std::runtime_error if the file cannot be opened
     * Time    : O(N * L) where N = number of lines, L = avg line length
     * Space   : O(N * L) to store all parsed records in memory
     */
    std::vector<Record> readRecords();

    /**
     * getHeaderColumns()
     * Purpose : Returns the column names read from the CSV header row.
     *           Used by FileWriter to reproduce the header in output files.
     */
    const std::vector<std::string>& getHeaderColumns() const;
};

#endif // CSV_READER_H

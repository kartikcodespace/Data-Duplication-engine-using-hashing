/**
 * @file FileWriter.h
 * @brief Declares FileWriter - responsible for writing the cleaned
 *        (deduplicated) dataset back to a CSV file on disk.
 *
 * Kept separate from CSVReader/ReportGenerator to respect Single
 * Responsibility: this class only knows how to WRITE record data,
 * nothing about parsing or statistics.
 */

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <string>
#include <vector>
#include "Record.h"

class FileWriter {
public:
    /**
     * writeCleanedCSV()
     * Purpose : Writes the header row followed by all unique records
     *           to the target CSV file.
     * Input   : filePath, header columns, unique records
     * Output  : none (creates/overwrites the file on disk)
     * Throws  : std::runtime_error if the file cannot be created/written
     * Time    : O(N * L)  Space: O(1) additional (streams directly to disk)
     */
    static void writeCleanedCSV(const std::string& filePath,
                                 const std::vector<std::string>& headerColumns,
                                 const std::vector<Record>& uniqueRecords);
};

#endif // FILE_WRITER_H

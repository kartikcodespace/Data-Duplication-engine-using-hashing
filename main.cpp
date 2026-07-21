/**
 * @file main.cpp
 * @brief Entry point of the Data Deduplication Engine. Provides a
 *        menu-driven console interface tying together CSVReader,
 *        DeduplicationEngine, FileWriter, and ReportGenerator.
 *
 * Flow:
 *   1. Load dataset (CSVReader)
 *   2. Display records (optional, first N rows to avoid flooding console)
 *   3. Detect & remove duplicates (DeduplicationEngine, timed via chrono)
 *   4. Save cleaned dataset (FileWriter)
 *   5. Generate report (ReportGenerator)
 *   6. Exit
 *
 * All operations are wrapped in try/catch blocks so invalid or missing
 * files, or operations attempted out of order, produce clear error
 * messages instead of crashing.
 */

#include <iostream>
#include <string>
#include <chrono>
#include <limits>

#include "Record.h"
#include "CSVReader.h"
#include "DeduplicationEngine.h"
#include "FileWriter.h"
#include "ReportGenerator.h"

namespace {

const std::string DEFAULT_INPUT_FILE   = "dataset.csv";
const std::string DEFAULT_OUTPUT_FILE  = "cleaned_data.csv";
const std::string DEFAULT_REPORT_FILE  = "report.txt";

void printMenu() {
    std::cout << "\n================ DATA DEDUPLICATION ENGINE ================\n";
    std::cout << "1. Load Dataset (" << DEFAULT_INPUT_FILE << ")\n";
    std::cout << "2. Display Loaded Records (first 10)\n";
    std::cout << "3. Detect & Remove Duplicates\n";
    std::cout << "4. Save Cleaned Dataset (" << DEFAULT_OUTPUT_FILE << ")\n";
    std::cout << "5. Generate Report (" << DEFAULT_REPORT_FILE << ")\n";
    std::cout << "6. Run Full Pipeline (Steps 1,3,4,5 together)\n";
    std::cout << "7. Exit\n";
    std::cout << "=============================================================\n";
    std::cout << "Enter your choice: ";
}

// Reads a menu choice safely, re-prompting on invalid (non-integer) input.
int readMenuChoice() {
    int choice;
    while (!(std::cin >> choice)) {
        std::cin.clear(); // reset error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
        std::cout << "Invalid input. Please enter a number between 1 and 7: ";
    }
    return choice;
}

void displayRecords(const std::vector<Record>& records, size_t maxRows = 10) {
    if (records.empty()) {
        std::cout << "No records to display.\n";
        return;
    }

    std::cout << "\nShowing first " << std::min(maxRows, records.size())
              << " of " << records.size() << " records:\n";
    std::cout << "-------------------------------------------------------------\n";
    for (size_t i = 0; i < records.size() && i < maxRows; ++i) {
        std::cout << "[Row " << records[i].getRowNumber() << "] "
                  << records[i].toCSVLine() << '\n';
    }
    std::cout << "-------------------------------------------------------------\n";
}

} // anonymous namespace

int main() {
    std::vector<Record> loadedRecords;
    std::vector<std::string> headerColumns;
    DeduplicationEngine engine;

    bool datasetLoaded = false;
    bool deduplicationDone = false;
    double lastExecutionMs = 0.0;

    std::cout << "Welcome to the Data Deduplication Engine (C++17)\n";

    bool running = true;
    while (running) {
        printMenu();
        int choice = readMenuChoice();

        try {
            switch (choice) {

                case 1: { // Load Dataset
                    CSVReader reader(DEFAULT_INPUT_FILE);
                    loadedRecords = reader.readRecords();
                    headerColumns = reader.getHeaderColumns();
                    engine.loadRecords(loadedRecords);
                    datasetLoaded = true;
                    deduplicationDone = false;
                    std::cout << "Successfully loaded " << loadedRecords.size()
                              << " records from '" << DEFAULT_INPUT_FILE << "'.\n";
                    break;
                }

                case 2: { // Display Records
                    if (!datasetLoaded) {
                        std::cout << "No dataset loaded yet. Please choose option 1 first.\n";
                        break;
                    }
                    displayRecords(loadedRecords);
                    break;
                }

                case 3: { // Detect & Remove Duplicates
                    if (!datasetLoaded) {
                        std::cout << "No dataset loaded yet. Please choose option 1 first.\n";
                        break;
                    }
                    auto startTime = std::chrono::high_resolution_clock::now();
                    engine.detectAndRemoveDuplicates();
                    auto endTime = std::chrono::high_resolution_clock::now();

                    lastExecutionMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
                    deduplicationDone = true;

                    std::cout << "Deduplication complete.\n";
                    std::cout << "  Unique Records    : " << engine.getUniqueCount() << '\n';
                    std::cout << "  Duplicate Records : " << engine.getDuplicateCount() << '\n';
                    std::cout << "  Time Taken        : " << lastExecutionMs << " ms\n";
                    break;
                }

                case 4: { // Save Cleaned Dataset
                    if (!deduplicationDone) {
                        std::cout << "Please run duplicate detection (option 3) before saving.\n";
                        break;
                    }
                    FileWriter::writeCleanedCSV(DEFAULT_OUTPUT_FILE, headerColumns, engine.getUniqueRecords());
                    std::cout << "Cleaned dataset saved to '" << DEFAULT_OUTPUT_FILE << "'.\n";
                    break;
                }

                case 5: { // Generate Report
                    if (!deduplicationDone) {
                        std::cout << "Please run duplicate detection (option 3) before generating a report.\n";
                        break;
                    }
                    DeduplicationStats stats = ReportGenerator::computeStats(
                        engine.getAllRecords(), engine.getUniqueRecords(),
                        engine.getCollisionsResolved(), lastExecutionMs);

                    ReportGenerator::printToConsole(stats);
                    ReportGenerator::writeReportFile(DEFAULT_REPORT_FILE, stats);
                    std::cout << "Report written to '" << DEFAULT_REPORT_FILE << "'.\n";
                    break;
                }

                case 6: { // Full Pipeline
                    CSVReader reader(DEFAULT_INPUT_FILE);
                    loadedRecords = reader.readRecords();
                    headerColumns = reader.getHeaderColumns();
                    engine.loadRecords(loadedRecords);
                    datasetLoaded = true;
                    std::cout << "Loaded " << loadedRecords.size() << " records.\n";

                    auto startTime = std::chrono::high_resolution_clock::now();
                    engine.detectAndRemoveDuplicates();
                    auto endTime = std::chrono::high_resolution_clock::now();
                    lastExecutionMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
                    deduplicationDone = true;

                    FileWriter::writeCleanedCSV(DEFAULT_OUTPUT_FILE, headerColumns, engine.getUniqueRecords());

                    DeduplicationStats stats = ReportGenerator::computeStats(
                        engine.getAllRecords(), engine.getUniqueRecords(),
                        engine.getCollisionsResolved(), lastExecutionMs);

                    ReportGenerator::printToConsole(stats);
                    ReportGenerator::writeReportFile(DEFAULT_REPORT_FILE, stats);

                    std::cout << "Full pipeline completed successfully.\n";
                    std::cout << "  -> " << DEFAULT_OUTPUT_FILE << " (cleaned dataset)\n";
                    std::cout << "  -> " << DEFAULT_REPORT_FILE << " (detailed report)\n";
                    break;
                }

                case 7: { // Exit
                    std::cout << "Exiting Data Deduplication Engine. Goodbye!\n";
                    running = false;
                    break;
                }

                default: {
                    std::cout << "Invalid choice. Please enter a number between 1 and 7.\n";
                    break;
                }
            }
        } catch (const std::exception& ex) {
            // Robust error handling: catches file errors, runtime errors, etc.
            std::cerr << "Error: " << ex.what() << '\n';
        } catch (...) {
            std::cerr << "An unknown error occurred.\n";
        }
    }

    return 0;
}

/**
 * @file Record.h
 * @brief Declares the Record class - the atomic data unit of the
 *        Data Deduplication Engine. Each row loaded from a CSV file
 *        is represented as one Record object.
 *
 * Design notes:
 *  - Encapsulation: fields and rowNumber are private; access is only
 *    through well-defined public methods.
 *  - A Record does NOT know about hashing algorithms (that is the
 *    responsibility of HashUtility). It only exposes a canonical
 *    "signature" string that HashUtility can hash. This keeps the
 *    two classes loosely coupled (Single Responsibility Principle).
 */

#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>

class Record {
private:
    std::vector<std::string> fields;   // All column values of this row
    int originalRowNumber;             // 1-based row number in source CSV (for traceability/report)

    // Helper: trims leading/trailing whitespace from a single field.
    static std::string trim(const std::string& value);

public:
    // Constructors
    Record();
    Record(const std::vector<std::string>& fieldValues, int rowNumber);

    // --- Accessors (read-only, const-correct) ---
    const std::vector<std::string>& getFields() const;
    int getRowNumber() const;
    size_t getFieldCount() const;
    std::string getField(size_t index) const;

    /**
     * getSignature()
     * Purpose : Builds a canonical, delimiter-safe string representation
     *           of the record (all fields trimmed and joined by '|').
     *           This signature is what gets hashed by HashUtility and
     *           what gets compared for exact-duplicate verification.
     * Input   : none (uses internal fields)
     * Output  : normalized string signature
     * Time    : O(L)  where L = total characters across all fields
     * Space   : O(L)  for the constructed string
     */
    std::string getSignature() const;

    /**
     * toCSVLine()
     * Purpose : Reconstructs a CSV-formatted line from the stored fields,
     *           used when writing the cleaned dataset back to disk.
     * Time    : O(k) where k = number of fields
     * Space   : O(L) for the resulting string
     */
    std::string toCSVLine() const;

    /**
     * operator==
     * Purpose : Exact field-by-field equality check. Used as the
     *           verification step after a hash match, to correctly
     *           handle hash collisions (two different records that
     *           happen to hash to the same bucket).
     * Time    : O(L)
     * Space   : O(1)
     */
    bool operator==(const Record& other) const;
};

#endif // RECORD_H

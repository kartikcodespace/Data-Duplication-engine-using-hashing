/**
 * @file Record.cpp
 * @brief Implementation of the Record class.
 */

#include "Record.h"
#include <sstream>
#include <algorithm>
#include <cctype>

// ---------- Constructors ----------

Record::Record() : originalRowNumber(0) {}

Record::Record(const std::vector<std::string>& fieldValues, int rowNumber)
    : fields(fieldValues), originalRowNumber(rowNumber) {}

// ---------- Private helper ----------

std::string Record::trim(const std::string& value) {
    size_t start = 0;
    size_t end = value.size();

    // Move start forward while whitespace
    while (start < end && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }
    // Move end backward while whitespace
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }
    return value.substr(start, end - start);
}

// ---------- Accessors ----------

const std::vector<std::string>& Record::getFields() const {
    return fields;
}

int Record::getRowNumber() const {
    return originalRowNumber;
}

size_t Record::getFieldCount() const {
    return fields.size();
}

std::string Record::getField(size_t index) const {
    if (index >= fields.size()) {
        return "";
    }
    return fields[index];
}

// ---------- Core logic ----------

std::string Record::getSignature() const {
    // Join all trimmed fields using '|' as a delimiter.
    // '|' is chosen because it is highly unlikely to appear inside
    // normal CSV data (names, emails, phone numbers, addresses).
    std::ostringstream signatureStream;
    for (size_t i = 0; i < fields.size(); ++i) {
        signatureStream << trim(fields[i]);
        if (i + 1 < fields.size()) {
            signatureStream << '|';
        }
    }
    return signatureStream.str();
}

std::string Record::toCSVLine() const {
    std::ostringstream lineStream;
    for (size_t i = 0; i < fields.size(); ++i) {
        lineStream << fields[i];
        if (i + 1 < fields.size()) {
            lineStream << ',';
        }
    }
    return lineStream.str();
}

bool Record::operator==(const Record& other) const {
    // Two records are considered duplicates if their canonical
    // signatures match exactly (case-sensitive, whitespace-trimmed).
    return this->getSignature() == other.getSignature();
}

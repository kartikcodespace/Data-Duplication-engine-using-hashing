/**
 * @file DeduplicationEngine.cpp
 * @brief Implementation of the hash-bucket + verify deduplication algorithm.
 */

#include "DeduplicationEngine.h"
#include "HashUtility.h"

DeduplicationEngine::DeduplicationEngine() : totalCollisionsResolved(0) {}

void DeduplicationEngine::loadRecords(const std::vector<Record>& records) {
    allRecords = records;

    // Reset state so the engine can be re-run cleanly (e.g., from a menu).
    uniqueRecords.clear();
    duplicateRecords.clear();
    hashBuckets.clear();
    totalCollisionsResolved = 0;
}

void DeduplicationEngine::detectAndRemoveDuplicates() {
    uniqueRecords.clear();
    duplicateRecords.clear();
    hashBuckets.clear();
    totalCollisionsResolved = 0;

    for (const Record& currentRecord : allRecords) {
        const std::string signature = currentRecord.getSignature();
        const uint64_t hashValue = HashUtility::generateHash(signature);

        auto bucketIterator = hashBuckets.find(hashValue);

        if (bucketIterator == hashBuckets.end()) {
            // No existing bucket for this hash -> definitely a new unique record.
            int newIndex = static_cast<int>(uniqueRecords.size());
            uniqueRecords.push_back(currentRecord);
            hashBuckets[hashValue] = std::vector<int>{ newIndex };
            continue;
        }

        // Bucket exists -> verify against every candidate in it to rule
        // out a false positive caused by hash collision.
        bool isDuplicate = false;
        for (int candidateIndex : bucketIterator->second) {
            if (uniqueRecords[candidateIndex] == currentRecord) {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate) {
            duplicateRecords.push_back(currentRecord);
        } else {
            // Same hash, different content -> genuine collision.
            ++totalCollisionsResolved;
            int newIndex = static_cast<int>(uniqueRecords.size());
            uniqueRecords.push_back(currentRecord);
            bucketIterator->second.push_back(newIndex);
        }
    }
}

// ---------- Accessors ----------

const std::vector<Record>& DeduplicationEngine::getAllRecords() const {
    return allRecords;
}

const std::vector<Record>& DeduplicationEngine::getUniqueRecords() const {
    return uniqueRecords;
}

const std::vector<Record>& DeduplicationEngine::getDuplicateRecords() const {
    return duplicateRecords;
}

long long DeduplicationEngine::getCollisionsResolved() const {
    return totalCollisionsResolved;
}

size_t DeduplicationEngine::getTotalCount() const {
    return allRecords.size();
}

size_t DeduplicationEngine::getUniqueCount() const {
    return uniqueRecords.size();
}

size_t DeduplicationEngine::getDuplicateCount() const {
    return duplicateRecords.size();
}

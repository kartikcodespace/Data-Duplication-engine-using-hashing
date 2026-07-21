/**
 * @file DeduplicationEngine.h
 * @brief Declares DeduplicationEngine - the core class that detects and
 *        removes duplicate Records using a hash-bucket + verify strategy.
 *
 * Algorithm: Hash-Bucket Deduplication with Collision Verification
 * ------------------------------------------------------------------
 * Problem: Given N records, identify all duplicates and produce a
 * unique-only subset, as fast as possible.
 *
 * Naive approach: Compare every record with every other record.
 *   Time Complexity: O(N^2 * L)  -- infeasible for large N (thousands+).
 *
 * Our approach (hash-bucket + verify):
 *   1. For each record R, compute signature = R.getSignature().
 *   2. Compute hash = HashUtility::generateHash(signature).
 *   3. Look up 'hash' in an unordered_map<uint64_t, vector<int>>
 *      (hash -> list of indices of previously seen records with that hash).
 *   4. If the hash bucket is empty -> record is definitely unique so far.
 *      Insert it, mark as unique.
 *   5. If the hash bucket is non-empty -> POSSIBLE duplicate (collision
 *      candidates). Compare the new record's signature against the
 *      signature of every record already in that bucket using
 *      Record::operator== (exact string comparison).
 *        - If an exact match is found -> confirmed duplicate, discard.
 *        - If no exact match among bucket candidates -> it's a genuine
 *          hash collision (different data, same hash); treat as unique
 *          and add its index to the bucket too.
 *
 * Why this is efficient:
 *   - unordered_map average-case lookup/insert is O(1).
 *   - Because a good hash function (FNV-1a) distributes values uniformly,
 *     buckets stay small, so the "verify" step (step 5) runs in O(1)
 *     amortized rather than O(N).
 *   - Overall average time complexity: O(N * L)
 *       N = number of records, L = average record signature length.
 *   - Worst case (pathological, all records collide into 1 bucket):
 *     O(N^2 * L). This is astronomically unlikely with FNV-1a on real data,
 *     and is the same worst case any hash-based structure has.
 *
 * Space Complexity: O(N * L)
 *   - We store all unique records, plus a hash map of size O(U) where
 *     U = number of unique records (U <= N).
 */

#ifndef DEDUPLICATION_ENGINE_H
#define DEDUPLICATION_ENGINE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include "Record.h"

class DeduplicationEngine {
private:
    std::vector<Record> allRecords;        // Original loaded records
    std::vector<Record> uniqueRecords;     // Records that survive deduplication
    std::vector<Record> duplicateRecords;  // Records identified as duplicates (for the report)

    // hash -> indices into uniqueRecords that currently hold this hash
    std::unordered_map<uint64_t, std::vector<int>> hashBuckets;

    long long totalCollisionsResolved; // count of genuine hash collisions encountered (for report)

public:
    DeduplicationEngine();

    /**
     * loadRecords()
     * Purpose : Loads a set of records into the engine (from CSVReader output)
     *           and resets internal state for a fresh deduplication run.
     * Time    : O(N)  Space: O(N)
     */
    void loadRecords(const std::vector<Record>& records);

    /**
     * detectAndRemoveDuplicates()
     * Purpose : Executes the hash-bucket + verify algorithm described above.
     *           Populates uniqueRecords and duplicateRecords.
     * Input   : none (operates on allRecords loaded via loadRecords)
     * Output  : none (results retrievable via getUniqueRecords() etc.)
     * Time    : O(N * L) average case
     * Space   : O(N * L)
     */
    void detectAndRemoveDuplicates();

    // --- Result accessors ---
    const std::vector<Record>& getAllRecords() const;
    const std::vector<Record>& getUniqueRecords() const;
    const std::vector<Record>& getDuplicateRecords() const;
    long long getCollisionsResolved() const;

    size_t getTotalCount() const;
    size_t getUniqueCount() const;
    size_t getDuplicateCount() const;
};

#endif // DEDUPLICATION_ENGINE_H

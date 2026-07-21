/**
 * @file HashUtility.h
 * @brief Declares HashUtility - a stateless utility class responsible
 *        purely for generating hash values from strings.
 *
 * Algorithm chosen: FNV-1a (Fowler-Noll-Vo, variant 1a)
 * ------------------------------------------------------
 * WHY FNV-1a for this project:
 *  - Simple to explain and implement in a viva (unlike MD5/SHA which
 *    need complex bit-manipulation and are overkill for non-cryptographic
 *    duplicate detection).
 *  - Very fast: single pass over the string, O(L) with tiny constant factor.
 *  - Good avalanche/distribution properties -> low collision rate in
 *    practice for typical text datasets (names, emails, IDs).
 *  - Deterministic across runs (unlike std::hash<string>, which in some
 *    STL implementations is randomized per-process for security reasons -
 *    that randomness would break reproducibility of results/reports,
 *    which matters for an academic project).
 *
 * FNV-1a Algorithm:
 *   hash = FNV_offset_basis
 *   for each byte b in input:
 *       hash = hash XOR b
 *       hash = hash * FNV_prime
 *   return hash
 *
 * We use the 64-bit variant:
 *   FNV_offset_basis = 14695981039346656037ULL
 *   FNV_prime        = 1099511628211ULL
 *
 * Collision handling: A hash is only a fast "candidate filter". This
 * class NEVER claims two records are identical purely because their
 * hashes match - DeduplicationEngine always performs a verification
 * step (Record::operator==) before confirming a duplicate. This is the
 * standard "hash bucket + verify" pattern used in real hash maps.
 */

#ifndef HASH_UTILITY_H
#define HASH_UTILITY_H

#include <string>
#include <cstdint>

class HashUtility {
public:
    /**
     * generateHash()
     * Purpose : Computes a 64-bit FNV-1a hash of the given string.
     * Input   : const string& input - the canonical record signature
     * Output  : uint64_t hash value
     * Time    : O(L) where L = length of input string
     * Space   : O(1) additional space
     */
    static uint64_t generateHash(const std::string& input);

private:
    // FNV-1a constants (64-bit variant)
    static const uint64_t FNV_OFFSET_BASIS;
    static const uint64_t FNV_PRIME;

    // Utility class: no instances should be created.
    HashUtility() = delete;
};

#endif // HASH_UTILITY_H

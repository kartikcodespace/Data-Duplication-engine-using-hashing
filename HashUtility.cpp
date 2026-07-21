/**
 * @file HashUtility.cpp
 * @brief Implementation of the FNV-1a hashing algorithm.
 */

#include "HashUtility.h"

const uint64_t HashUtility::FNV_OFFSET_BASIS = 14695981039346656037ULL;
const uint64_t HashUtility::FNV_PRIME        = 1099511628211ULL;

uint64_t HashUtility::generateHash(const std::string& input) {
    uint64_t hash = FNV_OFFSET_BASIS;

    // Single pass over every byte of the input string.
    for (unsigned char byte : input) {
        hash ^= static_cast<uint64_t>(byte); // XOR with current byte
        hash *= FNV_PRIME;                   // Multiply by prime (causes avalanche effect)
    }

    return hash;
}

# Data Deduplication Engine (C++17)

A console-based C++ application that loads large datasets from CSV files,
detects duplicate records using hashing techniques, removes redundant
records automatically, stores only unique records, and generates a
detailed deduplication report — built for MCA semester project evaluation.

## Features

- Menu-driven console application
- CSV parsing with quoted-field support
- Duplicate detection using FNV-1a hashing + collision-safe verification
- Automatic removal of duplicate records
- Cleaned dataset export (`cleaned_data.csv`)
- Detailed report generation (`report.txt`) with console summary
- Execution time measurement using `<chrono>`
- Robust exception handling for missing/invalid files

## Project Structure

```
DataDeduplicationEngine/
├── main.cpp                  # Menu-driven entry point
├── Record.h / Record.cpp     # Represents one CSV row; signature generation
├── CSVReader.h / .cpp        # CSV file parsing (handles quoted fields)
├── HashUtility.h / .cpp      # FNV-1a 64-bit hashing algorithm
├── DeduplicationEngine.h/.cpp# Core hash-bucket + verify dedup algorithm
├── ReportGenerator.h / .cpp  # Statistics computation & report output
├── FileWriter.h / .cpp       # Writes cleaned CSV to disk
├── dataset.csv                # Sample input (105 rows, 35 intentional duplicates)
├── cleaned_data.csv           # Generated output (unique records only)
├── report.txt                  # Generated deduplication report
└── README.md
```

## Class Responsibilities (OOP Design)

| Class | Responsibility |
|---|---|
| `Record` | Encapsulates one row's fields; produces a canonical signature for hashing/equality |
| `CSVReader` | Reads and parses CSV files into `Record` objects; throws on I/O errors |
| `HashUtility` | Stateless FNV-1a hash computation |
| `DeduplicationEngine` | Core dedup logic using `unordered_map<hash, vector<index>>` |
| `FileWriter` | Writes the cleaned dataset to disk |
| `ReportGenerator` | Computes stats and writes/prints the report |

## Algorithm: Hash-Bucket Deduplication with Collision Verification

1. Compute a canonical signature for each record (trimmed fields joined by `|`).
2. Hash the signature using FNV-1a (64-bit).
3. Look up the hash in an `unordered_map<uint64_t, vector<int>>`.
   - Empty bucket → record is unique; insert.
   - Non-empty bucket → compare against candidates in the bucket using
     exact string equality (handles hash collisions correctly).
     - Match found → duplicate, discard.
     - No match → genuine collision; treat as unique, add to bucket.

**Time Complexity:** O(N·L) average case (N = records, L = avg record length)
**Space Complexity:** O(N·L)

Naive O(N²·L) pairwise comparison is avoided entirely.

## Build Instructions

Requires a C++17-compliant compiler (g++ 9+ / clang++ 10+).

```bash
g++ -std=c++17 -Wall -Wextra -O2 main.cpp Record.cpp CSVReader.cpp HashUtility.cpp DeduplicationEngine.cpp FileWriter.cpp ReportGenerator.cpp -o dedup_engine
```

## Run Instructions

```bash
./dedup_engine
```

Menu options:
1. Load Dataset (`dataset.csv`)
2. Display Loaded Records (first 10)
3. Detect & Remove Duplicates
4. Save Cleaned Dataset (`cleaned_data.csv`)
5. Generate Report (`report.txt`)
6. Run Full Pipeline (steps 1, 3, 4, 5 together)
7. Exit

## Sample Dataset

`dataset.csv` contains 105 rows: 70 unique base records, 25 exact
duplicates, and 10 "whitespace-variant" duplicates (identical data with
extra spaces) to demonstrate the trim-normalization logic in `Record::getSignature()`.

## Sample Output (from included dataset.csv)

```
Total Records Loaded        : 105
Unique Records              : 70
Duplicate Records           : 35
Duplicate Percentage        : 33.33 %
Hash Collisions Resolved    : 0
Storage Saved               : 2064 bytes (33.16 %)
Execution Time              : ~0.3 ms
```

## Concepts Demonstrated

- Object-Oriented Programming (encapsulation, single responsibility)
- STL containers: `vector`, `unordered_map`, `string`
- File handling (`fstream`)
- Custom hashing implementation (FNV-1a)
- Exception handling (`try`/`catch`, `std::runtime_error`)
- Performance measurement (`<chrono>`)
- Modular multi-file C++ program structure

## Author

MCA Semester Project — Data Deduplication Engine

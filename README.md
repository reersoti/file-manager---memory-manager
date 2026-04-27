# File Manager & Memory Manager in C++

![CMake CI](https://github.com/reersoti/file-manager---memory-manager/actions/workflows/cmake.yml/badge.svg)

An educational C++ project focused on low-level file handling and manual memory management.

## Overview

This repository explores systems-level programming concepts in C++, including manual memory handling, file abstraction logic, resource ownership, and test-driven validation of low-level components.

The project is not intended to be a production-ready library. It was created as an educational exercise to better understand how explicit resource control works in practice and how low-level design decisions affect safety, flexibility, and maintainability.

## Goals

The project is intended to deepen understanding of:

- dynamic memory allocation and deallocation
- ownership and lifetime management
- file handling abstractions
- low-level resource management
- CMake-based C++ project organization
- basic automated testing with CTest

## Project Structure

```text
.
├── File_Manager/       # file-related implementation
├── Memory_Manager/     # memory-related implementation
├── tests/              # test executables
├── docs/               # design notes and documentation
├── CMakeLists.txt      # CMake build configuration
└── README.md
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

You can also run the generated test executables directly:

```bash
./build/test_memory_manager
./build/test_file_manager
```

## Documentation

- [Design Notes](docs/DESIGN_NOTES.md)

## Continuous Integration

The repository includes a GitHub Actions workflow that automatically builds the project and runs CTest checks on every push and pull request to `main`.

## What This Project Demonstrates

- manual control over memory-related operations
- separation of file-management and memory-management logic
- understanding of ownership and resource lifetime
- basic C++ testing workflow with CTest
- practical use of CMake for building multiple targets

## Possible Improvements

- add more edge-case tests
- document the internal API of each manager
- add examples of expected input/output behavior
- improve error handling and validation
- compare manual resource handling with RAII-based alternatives

## Tech Stack

- C++17
- CMake
- CTest
- GitHub Actions

## Notes

This repository is intended as an educational project focused on file operations, memory management concepts, and explicit resource control in C++.

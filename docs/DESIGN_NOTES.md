# Design Notes

This document describes the educational design intent of the file manager and memory manager project.

## Purpose

The repository is focused on practicing low-level C++ resource management. It is not intended to be a production-ready memory allocator or file-management library.

## Main Concepts

- explicit allocation and deallocation;
- ownership and lifetime reasoning;
- file-related abstractions;
- separation between memory-management and file-management components;
- basic validation through tests and CTest.

## Design Principles

### Keep Resource Ownership Explicit

The project should make ownership decisions visible so that memory and file lifetime can be reasoned about clearly.

### Prefer Small Components

File-related and memory-related logic should stay separated to avoid mixing responsibilities.

### Test Low-Level Behavior

Tests should verify expected behavior of the managers and catch regressions around allocation, cleanup, and file handling.

### Document Unsafe Assumptions

Any manual resource-management code should clearly document assumptions, limitations, and edge cases.

## Future Improvements

- add more edge-case tests;
- document public functions and expected behavior;
- add examples of successful and invalid usage;
- compare manual resource handling with RAII-based alternatives;
- improve validation and error handling.

# Bound Check Error

Tools that manage to find the bounds error:
- GCC / Clang __with__ fsanatizer=address or fsanitizer=undefined
- Coverity

Tools that don't find it:
- Cppcheck
- GCC / Clang
- Valgrind
- Clang-check
- Clang-tidy

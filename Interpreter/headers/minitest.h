#define ASSERT(condition, success, failure, ...) \
if (condition) { \
    printf(success);\
} else { \
    printf(failure, __VA_ARGS__); \
}
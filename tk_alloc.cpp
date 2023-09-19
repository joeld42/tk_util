#include "tk_alloc.h"
#include <string.h>

void *
tk::DefaultAllocator::alloc(size_t size, uint64_t tag, const char *file, uint32_t line) {
    return ::malloc(size);
}

void *
tk::DefaultAllocator::realloc(void *ptr, size_t size, uint64_t tag, const char *file, uint32_t line) {
    return ::realloc(ptr, size);
}

void
tk::DefaultAllocator::free(void *ptr, const char *file, uint32_t line) {
    ::free(ptr);
}

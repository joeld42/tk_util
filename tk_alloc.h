#pragma once

#include <stdlib.h>
#include <stdint.h>


namespace tk {

    class IAllocator {
    public:
        virtual void* alloc( size_t size, uint64_t tag, const char *file, uint32_t line )=0;
        virtual void* realloc( void *ptr, size_t size, uint64_t tag, const char *file, uint32_t line )=0;
        virtual void free( void *ptr, const char *file, uint32_t line)=0;
    };

    // Default allocator that uses the system Malloc/Free
    class DefaultAllocator : public IAllocator {
    public:
        virtual void* alloc( size_t size, uint64_t tag, const char *file, uint32_t line );
        virtual void* realloc( void *ptr, size_t size, uint64_t tag, const char *file, uint32_t line );
        virtual void free( void *ptr, const char *file, uint32_t line);
    };


    // Macros to use the allocator
    #define TK_ALLOC( allocator, size, tag ) (allocator)->alloc( size, tag, __FILE__, __LINE__ )
    #define TK_REALLOC( allocator, ptr, size, tag ) (allocator)->realloc( ptr, size, tag, __FILE__, __LINE__ )
    #define TK_FREE( allocator, ptr, tag ) (allocator)->free( ptr, __FILE__, __LINE__ )

};

#include <malloc.h>
#include <stdio.h>

extern "C" void* __libc_malloc(size_t size);
extern "C" void __libc_free(void*);
extern "C" void* __libc_calloc(size_t, size_t);
extern "C" void* __libc_realloc(void*, size_t);

//! Set this to "1" in main to get the printouts
int malloc_hook_active = 0;

static void* malloc_hook(size_t size, void* caller) {
    void* result;
    malloc_hook_active = 0;
    result = malloc(size);
    fprintf(stderr, "malloc(0x%lx) = %p  from:%p\n", size, result, caller);
    malloc_hook_active = 1;
    return result;
}

extern "C" void* malloc(size_t size) {
    void* caller = __builtin_frame_address(1);
    if (malloc_hook_active) {
        return malloc_hook(size, caller);
    }
    return __libc_malloc(size);
}

static void free_hook(void* ptr, void* caller) {
    malloc_hook_active = 0;
    fprintf(stderr, "free(%p)    from:%p\n", ptr, caller);
    __libc_free(ptr);
    malloc_hook_active = 1;
}

extern "C" void free(void* ptr) {
    void* caller = __builtin_frame_address(1);
    if (malloc_hook_active) {
        free_hook(ptr, caller);
        return;
    }
    __libc_free(ptr);
}

static void* calloc_hook(size_t nmemb, size_t size, void* caller) {
    malloc_hook_active = 0;
    void* result = calloc(nmemb, size);
    fprintf(stderr, "calloc(0x%lx,0x%lx) = %p   from:%p\n", nmemb, size, result, caller);
    malloc_hook_active = 1;
    return result;
}

extern "C" void* calloc(size_t nmemb, size_t size) {
    void* caller = __builtin_frame_address(1);
    if (malloc_hook_active) {
        return calloc_hook(nmemb, size, caller);
    }
    return __libc_calloc(nmemb, size);
}

static void* realloc_hook(void* ptr, size_t size, void* caller) {
    malloc_hook_active = 0;
    void* result = realloc(ptr, size);
    fprintf(stderr, "realloc(%p,0x%lx) = %p   from:%p\n", ptr, size, result, caller);
    malloc_hook_active = 1;
    return result;
}

extern "C" void* realloc(void* ptr, size_t size) {
    void* caller = __builtin_frame_address(1);
    if (malloc_hook_active) {
        return realloc_hook(ptr, size, caller);
    }
    return __libc_realloc(ptr, size);
}
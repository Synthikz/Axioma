#include <libc.h>
#include <stdint.h>

static uint32_t heap_base = 0x200000;
static uint32_t heap_top = heap_base;

static constexpr uint32_t ALIGNMENT = 4;

static inline uint32_t align_up(uint32_t value) {
    return (value + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    if (dest == src) {
        return dest;
    }
    
    unsigned char* d = reinterpret_cast<unsigned char*>(dest);
    const unsigned char* s = reinterpret_cast<const unsigned char*>(src);
    
    if (((uintptr_t)dest & 3) == 0 && ((uintptr_t)src & 3) == 0 && n >= 4) {
        size_t words = n / 4;
        uint32_t* d_word = reinterpret_cast<uint32_t*>(d);
        const uint32_t* s_word = reinterpret_cast<const uint32_t*>(s);
        
        for (size_t i = 0; i < words; i++) {
            *d_word++ = *s_word++;
        }
        
        n -= words * 4;
        d = reinterpret_cast<unsigned char*>(d_word);
        s = reinterpret_cast<const unsigned char*>(s_word);
    }
    
    for (size_t i = 0; i < n; i++) {
        *d++ = *s++;
    }
    
    return dest;
}

extern "C" void* memset(void* dest, int c, size_t n) {
    
    unsigned char* d = reinterpret_cast<unsigned char*>(dest);
    unsigned char byte = static_cast<unsigned char>(c);
    
    if (((uintptr_t)dest & 3) == 0 && n >= 4) {
        uint32_t word = byte;
        word |= (word << 8);
        word |= (word << 16);
        
        size_t words = n / 4;
        uint32_t* d_word = reinterpret_cast<uint32_t*>(d);
        
        for (size_t i = 0; i < words; i++) {
            *d_word++ = word;
        }
        
        n -= words * 4;
        d = reinterpret_cast<unsigned char*>(d_word);
    }
    
    for (size_t i = 0; i < n; i++) {
        *d++ = byte;
    }
    
    return dest;
}

extern "C" int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

extern "C" size_t strlen(const char* s) {
    const char* p = s;
    while (*p) p++;
    return p - s;
}

extern "C" char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

extern "C" void* malloc(size_t size) {
    if (size == 0) return nullptr;
    uint32_t aligned_size = align_up(size);
    
    void* ptr = reinterpret_cast<void*>(heap_top);
    heap_top += aligned_size;
    
    return ptr;
}

extern "C" void free(void* ptr) {
    (void)ptr;
}

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
    free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
    free(ptr);
}
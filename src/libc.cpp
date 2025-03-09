#include <libc.h>
#include <stdint.h>

static uint32_t heap_base = 0x200000;
static uint32_t heap_top = heap_base;
static uint32_t heap_limit = 0x400000;

struct MemoryBlock {
    size_t size;
    bool used;
    MemoryBlock* next;
};

static MemoryBlock* free_list = nullptr;
static constexpr uint32_t ALIGNMENT = 4;

static inline uint32_t align_up(uint32_t value) {
    return (value + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

static void init_memory_manager() {
    if (free_list == nullptr) {
        free_list = reinterpret_cast<MemoryBlock*>(heap_base);
        free_list->size = heap_limit - heap_base - sizeof(MemoryBlock);
        free_list->used = false;
        free_list->next = nullptr;
        heap_top = heap_base + sizeof(MemoryBlock);
    }
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
    size = (size + 3) & ~3;
    
    init_memory_manager();
    
    MemoryBlock* current = free_list;
    MemoryBlock* prev = nullptr;
    
    while (current != nullptr) {
        if (!current->used && current->size >= size) {
            if (current->size >= size + sizeof(MemoryBlock) + 16) {
                MemoryBlock* new_block = reinterpret_cast<MemoryBlock*>(
                    reinterpret_cast<char*>(current) + sizeof(MemoryBlock) + size);
                    
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->used = false;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            current->used = true;
            return reinterpret_cast<char*>(current) + sizeof(MemoryBlock);
        }
        
        prev = current;
        current = current->next;
    }
    
    uint32_t required_size = size + sizeof(MemoryBlock);
    if (heap_top + required_size > heap_limit) {
        return nullptr;
    }
    
    MemoryBlock* new_block = reinterpret_cast<MemoryBlock*>(heap_top);
    new_block->size = size;
    new_block->used = true;
    new_block->next = nullptr;
    
    if (prev) {
        prev->next = new_block;
    } else {
        free_list = new_block;
    }
    
    void* result = reinterpret_cast<char*>(new_block) + sizeof(MemoryBlock);
    heap_top += required_size;
    
    return result;
}

extern "C" void free(void* ptr) {
    if (!ptr) return;
    
    MemoryBlock* block = reinterpret_cast<MemoryBlock*>(
        reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
        
    block->used = false;
    MemoryBlock* current = free_list;
    
    while (current != nullptr) {
        if (!current->used && !current->next->used) {
            current->size += sizeof(MemoryBlock) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
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
#ifndef LIBC_H
#define LIBC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* dest, int c, size_t n);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);

void* malloc(size_t size);
void free(void* ptr);

#ifdef __cplusplus
}

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete(void* ptr, size_t) noexcept;
void operator delete[](void* ptr, size_t) noexcept;

#endif

#endif
#ifndef ORKA_MODULE_H_2020_05_21_12_09_25
#define ORKA_MODULE_H_2020_05_21_12_09_25

#include <stddef.h>


typedef struct VTable{
    size_t type_size;
    size_t type_align;
    void*(*alloc)(size_t,size_t); // NULL for default allocator function
    void(*dealloc)(void*); // Same here
    void(*construct)(void*); // NULL for memset(0)
    void(*destroy)(void*); // NULL for trivial destructor
#ifndef __cplusplus
    void(*vfn[])(void*); // Entries into the vtable which has the structure defined by the interface
#endif
} VTable;

typedef struct{
    void* data;
    const VTable* vtable;
} InterfacePtr;



typedef struct Interface Interface;

typedef struct Module Module;

typedef const struct FrameworkCalls* Framework;

struct FrameworkCalls{
    void* reserved1;
    void* reserved2;
    void* reserved3;
    void* reserved4;
    
    void*(*default_alloc)(size_t,size_t);
    void(*default_dealloc)(void*);
    void(*default_construct)(void*);
    void(*default_destroy)(void*);
    
    const Module*(*getDependency)(Framework*,Module*,const char*);
    const Interface*(*getDeclaredInterface)(Framework*,const Module*,const char*);
    InterfacePtr (*newInstance)(Framework*,const Interface*);
    InterfacePtr (*globalInstance)(Framework*,const Interface*);
    
    const void* (*getGlobal)(Framework*,const Module*,const char*);
    const VTable* (*getCommonVTable)(Framework*,const Module*,const char*);
    const VTable* (*getProvidedVTable)(Framework*,const Module*,const char*,const Interface*);
    void (*destroy)(InterfacePtr);
};



#endif

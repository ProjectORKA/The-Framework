# Orka Module Descriptor Format

This specification describes the binary format used by the module system of orka to describe, in a platform-independant manner, 
 the module, and properties of the module.
 
 This format is based upon [LCS 4](https://lightningcreations.github.io/LCS/publications/LCS4) and uses the definitions provided there. 
 
 This format shall use the extension .okm
 
 The type `empty` used in this document shall be a type that has an empty byte representation.
 
## Base Structure

```
struct OKM{
    u8 magic[4];
    version ver;
    ConstantTable constants;
    u16 mod_name;
    u16 mod_version;
    DependencyTable requires;
    ExportTable exports;
    TypesTable types;
    InitTable init;
};
```

magic MUST be the bytes `[EE 4F 4B 4D]`. version MUST be a version supported by the implementation. The currently specified version is 1.0, encoded as `[00 00]`. `constants` is the constant pool of the file, `mod_name` is the name of the file and shall be an index into the Constant Pool which is a `Const_Utf8`. `mod_version` is the version of the module, and shall be an index into the constant pool which is a `Const_Version`, 
 which MUST be an exact version specifier, and not a version range specifier. 
 
 ### Constant Pool
 
 ```
 struct ConstantTable{
    u16 nconsts;
    Constant consts[nconsts];
 };

 
 struct Constant{
    u8 tag;
    union{
        string Const_Utf8;
        i32 Const_i32;
        i64 Const_i64;
        u32 Const_u32;
        u64 Const_u64;
        Type Const_Type;
        u16 Const_Version;
    } payload;
 };
 ```
 
 `tag` shall be one of `Const_Utf8` (0), `Const_i32` (1), `Const_i64` (2), `Const_u64` (3), `Const_Type` (4), `Const_Version` (5). 
 
 For `Const_Version`, the payload shall be an index into the constant pool which is a `Const_Utf8`, and the the form of a semantic versioning requirement string (either an exact version, or a version range), or a wildcard version string "*". 
 
 ```
 struct Type{
    u8 kind;
    union{
        empty primitive;
        u16 qualified;
        u16 Type_Named;
        struct{ u8 nparams; u16 params[nparams]; u16 ret;} Type_Function;
        struct { u16 type; u16 len;} Type_Array;
    }payload;
 };
 ```
 
 kind shall be one of `Type_i8` (0), `Type_i16` (1), `Type_i32` (2), `Type_i64` (3), `Type_isize` (4),
  `Type_i128` (5), `Type_u8` (6), `Type_u16` (7), `Type_u32` (8), `Type_u64` (9), `Type_usize` (10),
  `Type_u128` (11), `Type_f32` (12), `Type_f64` (13), `Type_fL` (14) (all of which use the union field `primitive`), 
  `Type_Const` (15), `Type_Volatile` (15), `Type_Pointer` (16), `Type_Reference` (17), `Type_DynArray` (18) (which all use the union field `qualified`),
  `Type_Named` (19), `Type_Function` (20), or `Type_Array` (21).
  
  For types using the field `qualified`, it must be an index into the constant pool which is a `Const_Type`, which is the type to qualify with this. 
  For Type_Named, the payload must be an index into the constant pool which is a `Const_Utf8`. 
  For Type_Function, ret and all params must be indecies into the constant pools which are all `Const_Type`.
  For Type_Array, type shall be an index into the constant pool which is a `Const_Type`. len shall be an index into the constant pool which is either a `Const_u32` or `Const_u64` and is the length of the array.
  
### Dependencies
  
  ```
  struct DependencyTable{
    u16 len;
    struct{
        u16 module;
        u16 version;
        u8 order;
    } dependencies[len];
  };
```

Each `module` in dependencies shall be an index into the constant pool which is a `Const_Utf8`, and is the name of the module.
Each `version` shall be a `Const_Version`, and the version requirement for the dependency. 
Each `order` shall be either `REQUIRED` (0), `OPTIONAL AFTER` (1), `OPTIONAL BEFORE` (2), or `OPTIONAL NONE` (3).

### Attributes and Items

```
struct Attribute{
    u16 attr;
    u16 payload_len;
    u8 payload[payload_len];
};
```

attr shall be an index into the constant pool which is a `Const_Utf8`. The interpretation of `payload` depends on `attr`. 

```
struct Item{
    u8 kind;
    u16 name;
    u16 type;
    u16 value;
    u16 nattrs;
    Attribute attrs[nattrs];
};
```

`kind` shall either be `Field` (0), `Function` (1), `Interface` (2), or `Type` (3).
`name` shall be an index into the constant pool which is the `Const_Utf8`. 
`type` shall be an index into the constant pool which is a `Const_Type`.
For `Field` or `Function`, value shall be an index into the constant pool which is a `Const_Utf8`. Otherwise value MUST be 0.

### Exports and Types

```
struct ExportsTable{
    u16 len;
    Item exports[len];
};
```



```
struct TypesTable{
    u16 len;
    struct{
        u8 kind;
        u16 name;
        union{
            u16 by;
            struct{
                u16 nitems;
                Item items[nitems];
            };
        }
        u16 nattrs;
        Attribute attrs[nattrs];
    } types[len];
};
```


kind shall be either `Struct` (0), `Class` (1), `Interface` (2) (which all use the variant `items`), or `ProvidedInterface` (3) (which uses the variant `by`)
If kind is `ProvidedInterface`, then `by` shall be an index into the Constant Pool which is a `Const_Utf8`, which names a type exported by this module. For this `kind`, `name` is the name of the interface which is provided by this entry. 

No item may be an `Interface` or `Type`.

If `kind` is `Class` or `ProvidedInterface` then at least one Attribute MUST be a `vtable` attribute.

```
struct Attribute_VTable{
    u16 attr;
    u8 length;
    u16 vtable;
};
```

`attr` shall be a `Const_Utf8` which is exactly the text "vtable". `length` shall be 2. 

`vtable` shall be either an entry into the constant pool which is a `Const_Utf8` and does not have index 0, where the vtable represents a symbol in the module dso, or it shall be the value 0, in which case, it represents a null pointer.

A `ProvidedInterface` type item shall not contain a null pointer for a vtable. 

This attribute MUST only be present on a type. A given item may have at most one `vtable` attribute. 

### Initialization

```
struct InitTable{
    u16 load;
    u16 init;
    u16 main;
    u16 unload;
    u16 exit;
};
```

All fields in the InitTable shall be indecies into the constant pool which are `Const_Utf8` and do not have index 0,
 where the item represents a symbol in the module dso, or shall be the value 0, in which case it is a null pointer.
 



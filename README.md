# DJSON

Ultra lightweight JSON parser written in D.

## Table of contents
* [License](#license)
* [Usage](#usage)
  * [Welcome to DJSON](#welcome-to-cjson)
  * [Building](#building)
    * [Copying the source](#copying-the-source)
    * [CMake](#cmake)
    * [Makefile](#makefile)
    * [Meson](#meson)
    * [Vcpkg](#Vcpkg)
  * [Including DJSON](#including-cjson)
  * [Data Structure](#data-structure)
  * [Working with the data structure](#working-with-the-data-structure)
    * [Basic types](#basic-types)
    * [Arrays](#arrays)
    * [Objects](#objects)
  * [Parsing JSON](#parsing-json)
  * [Printing JSON](#printing-json)
  * [Example](#example)
    * [Printing](#printing)
    * [Parsing](#parsing)
  * [Caveats](#caveats)
    * [Zero Character](#zero-character)
    * [Character Encoding](#character-encoding)
    * [C Standard](#c-standard)
    * [Floating Point Numbers](#floating-point-numbers)
    * [Deep Nesting Of Arrays And Objects](#deep-nesting-of-arrays-and-objects)
    * [Thread Safety](#thread-safety)
    * [Case Sensitivity](#case-sensitivity)
    * [Duplicate Object Members](#duplicate-object-members)
  * [Enjoy DJSON!](#enjoy-cjson)

## Usage

### Welcome to DJSON.

DJSON aims to be the dumbest possible parser that you can get your job done with.
It's a single file of C, and a single header file.

JSON is described best here: http://www.json.org/
It's like XML, but fat-free. You use it to move data around, store things, or just
generally represent your program's state.

As a library, DJSON exists to take away as much legwork as it can, but not get in your way.
As a point of pragmatism (i.e. ignoring the truth), I'm going to say that you can use it
in one of two modes: Auto and Manual. Let's have a quick run-through.

I lifted some JSON from this page: http://www.json.org/fatfree.html
That page inspired me to write DJSON, which is a parser that tries to share the same
philosophy as JSON itself. Simple, dumb, out of the way.

### Building

There are several ways to incorporate DJSON into your project.

#### copying the source

Because the entire library is only one C file and one header file, you can just copy `DJSON.h` and `DJSON.c` to your projects source and start using it.

DJSON is written in ANSI C (C89) in order to support as many platforms and compilers as possible.

#### CMake

With CMake, DJSON supports a full blown build system. This way you get the most features. CMake with an equal or higher version than 2.8.5 is supported. With CMake it is recommended to do an out of tree build, meaning the compiled files are put in a directory separate from the source files. So in order to build DJSON with CMake on a Unix platform, make a `build` directory and run CMake inside it.

```
mkdir build
cd build
cmake ..
```

This will create a Makefile and a bunch of other files. You can then compile it:

```
make
```

And install it with `make install` if you want. By default it installs the headers `/usr/local/include/cjson` and the libraries to `/usr/local/lib`. It also installs files for pkg-config to make it easier to detect and use an existing installation of CMake. And it installs CMake config files, that can be used by other CMake based projects to discover the library.

You can change the build process with a list of different options that you can pass to CMake. Turn them on with `On` and off with `Off`:

* `-DENABLE_CJSON_TEST=On`: Enable building the tests. (on by default)
* `-DENABLE_CJSON_UTILS=On`: Enable building BC_JSON_Utils. (off by default)
* `-DENABLE_TARGET_EXPORT=On`: Enable the export of CMake targets. Turn off if it makes problems. (on by default)
* `-DENABLE_CUSTOM_COMPILER_FLAGS=On`: Enable custom compiler flags (currently for Clang, GCC and MSVC). Turn off if it makes problems. (on by default)
* `-DENABLE_VALGRIND=On`: Run tests with [valgrind](http://valgrind.org). (off by default)
* `-DENABLE_SANITIZERS=On`: Compile DJSON with [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) and [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) enabled (if possible). (off by default)
* `-DENABLE_SAFE_STACK`: Enable the [SafeStack](https://clang.llvm.org/docs/SafeStack.html) instrumentation pass. Currently only works with the Clang compiler. (off by default)
* `-DBUILD_SHARED_LIBS=On`: Build the shared libraries. (on by default)
* `-DBUILD_SHARED_AND_STATIC_LIBS=On`: Build both shared and static libraries. (off by default)
* `-DCMAKE_INSTALL_PREFIX=/usr`: Set a prefix for the installation.
* `-DENABLE_LOCALES=On`: Enable the usage of localeconv method. ( on by default )
* `-DCJSON_OVERRIDE_BUILD_SHARED_LIBS=On`: Enable overriding the value of `BUILD_SHARED_LIBS` with `-DCJSON_BUILD_SHARED_LIBS`.
* `-DENABLE_CJSON_VERSION_SO`: Enable DJSON so version. ( on by default )

If you are packaging DJSON for a distribution of Linux, you would probably take these steps for example:
```
mkdir build
cd build
cmake .. -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DCMAKE_INSTALL_PREFIX=/usr
make
make DESTDIR=$pkgdir install
```

On Windows CMake is usually used to create a Visual Studio solution file by running it inside the Developer Command Prompt for Visual Studio, for exact steps follow the official documentation from CMake and Microsoft and use the online search engine of your choice. The descriptions of the the options above still generally apply, although not all of them work on Windows.

#### Makefile

**NOTE:** This Method is deprecated. Use CMake if at all possible. Makefile support is limited to fixing bugs.

If you don't have CMake available, but still have GNU make. You can use the makefile to build DJSON:

Run this command in the directory with the source code and it will automatically compile static and shared libraries and a little test program (not the full test suite).

```
make all
```

If you want, you can install the compiled library to your system using `make install`. By default it will install the headers in `/usr/local/include/cjson` and the libraries in `/usr/local/lib`. But you can change this behavior by setting the `PREFIX` and `DESTDIR` variables: `make PREFIX=/usr DESTDIR=temp install`. And uninstall them with: `make PREFIX=/usr DESTDIR=temp uninstall`.

#### Meson

To make cjson work in a project using meson, the libcjson dependency has to be included:

```meson
project('c-json-example', 'c')

cjson = dependency('libcjson')

example = executable(
    'example',
    'example.c',
    dependencies: [cjson],
)
```


#### Vcpkg

You can download and install DJSON using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
vcpkg install cjson
```

The DJSON port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

### Including DJSON

If you installed it via CMake or the Makefile, you can include DJSON like this:

```c
#include <cjson/DJSON.h>
```

### Data Structure

DJSON represents JSON data using the `DJSON` struct data type:

```c
/* The DJSON structure: */
typedef struct DJSON
{
    DJSON*    next;
    DJSON*    prev;
    DJSON*    child;
    JSON_TYPE type;
    char*     value_string;
    real      value_number;
    char*     text;
} DJSON;
```

An item of this type represents a JSON value. The type is stored in `type` as a bit-flag (**this means that you cannot find out the type by just comparing the value of `type`**).

To check the type of an item, use the corresponding `BC_JSON_Is...` function. It does a `NULL` check followed by a type check and returns a boolean value if the item is of this type.

The type can be one of the following:

* `BC_JSON_Invalid` (check with `BC_JSON_IsInvalid`): Represents an invalid item that doesn't contain any value. You automatically have this type if you set the item to all zero bytes.
* `BC_JSON_False` (check with `BC_JSON_IsFalse`): Represents a `false` boolean value. You can also check for boolean values in general with `BC_JSON_IsBool`.
* `BC_JSON_True` (check with `BC_JSON_IsTrue`): Represents a `true` boolean value. You can also check for boolean values in general with `BC_JSON_IsBool`.
* `BC_JSON_NULL` (check with `BC_JSON_IsNull`): Represents a `null` value.
* `JSON_TYPE.NUMBER` (check with `BC_JSON_IsNumber`): Represents a number value. The value is stored as a double in `value_number`.
* `BC_JSON_String` (check with `BC_JSON_IsString`): Represents a string value. It is stored in the form of a zero terminated string in `value_string`.
* `BC_JSON_Array` (check with `BC_JSON_IsArray`): Represent an array value. This is implemented by pointing `child` to a linked list of `DJSON` items that represent the values in the array. The elements are linked together using `next` and `prev`, where the first element has `prev.next == NULL` and the last element `next == NULL`.
* `BC_JSON_Object` (check with `BC_JSON_IsObject`): Represents an object value. Objects are stored same way as an array, the only difference is that the items in the object store their keys in `string`.
* `BC_JSON_Raw` (check with `BC_JSON_IsRaw`): Represents any kind of JSON that is stored as a zero terminated array of characters in `value_string`. This can be used, for example, to avoid printing the same static JSON over and over again to save performance. DJSON will never create this type when parsing. Also note that DJSON doesn't check if it is valid JSON.

Additionally there are the following two flags:

* `BC_JSON_IsReference`: Specifies that the item that `child` points to and/or `value_string` is not owned by this item, it is only a reference. So `BC_JSON_Delete` and other functions will only deallocate this item, not its `child`/`value_string`.
* `BC_JSON_StringIsConst`: This means that `string` points to a constant string. This means that `BC_JSON_Delete` and other functions will not try to deallocate `string`.

### Working with the data structure

For every value type there is a `BC_JSON_Create...` function that can be used to create an item of that type.
All of these will allocate a `DJSON` struct that can later be deleted with `BC_JSON_Delete`.
Note that you have to delete them at some point, otherwise you will get a memory leak.
**Important**: If you have added an item to an array or an object already, you **mustn't** delete it with `BC_JSON_Delete`. Adding it to an array or object transfers its ownership so that when that array or object is deleted,
it gets deleted as well. You also could use `BC_JSON_SetValuestring` to change a `BC_JSON_String`'s `value_string`, and you needn't to free the previous `value_string` manually.

#### Basic types

* **null** is created with `BC_JSON_CreateNull`
* **booleans** are created with `BC_JSON_CreateTrue`, `BC_JSON_CreateFalse` or `BC_JSON_CreateBool`
* **numbers** are created with `BC_JSON_CreateNumber`. This will set `value_number`.
* **strings** are created with `BC_JSON_CreateString` (copies the string) or with `BC_JSON_CreateStringReference` (directly points to the string. This means that `value_string` won't be deleted by `BC_JSON_Delete` and you are responsible for its lifetime, useful for constants)

#### Arrays

You can create an empty array with `BC_JSON_CreateArray`. `BC_JSON_CreateArrayReference` can be used to create an array that doesn't "own" its content, so its content doesn't get deleted by `BC_JSON_Delete`.

To add items to an array, use `BC_JSON_AddItemToArray` to append items to the end.
Using `BC_JSON_AddItemReferenceToArray` an element can be added as a reference to another item, array or string. This means that `BC_JSON_Delete` will not delete that items `child` or `value_string` properties, so no double frees are occurring if they are already used elsewhere.
To insert items in the middle, use `BC_JSON_InsertItemInArray`. It will insert an item at the given 0 based index and shift all the existing items to the right.

If you want to take an item out of an array at a given index and continue using it, use `BC_JSON_DetachItemFromArray`, it will return the detached item, so be sure to assign it to a pointer, otherwise you will have a memory leak.

Deleting items is done with `BC_JSON_DeleteItemFromArray`. It works like `BC_JSON_DetachItemFromArray`, but deletes the detached item via `BC_JSON_Delete`.

You can also replace an item in an array in place. Either with `BC_JSON_ReplaceItemInArray` using an index or with `BC_JSON_ReplaceItemViaPointer` given a pointer to an element. `BC_JSON_ReplaceItemViaPointer` will return `0` if it fails. What this does internally is to detach the old item, delete it and insert the new item in its place.

To get the size of an array, use `BC_JSON_GetArraySize`. Use `BC_JSON_GetArrayItem` to get an element at a given index.

Because an array is stored as a linked list, iterating it via index is inefficient (`O(n²)`), so you can iterate over an array using the `BC_JSON_ArrayForEach` macro in `O(n)` time complexity.

#### Objects

You can create an empty object with `BC_JSON_CreateObject`. `BC_JSON_CreateObjectReference` can be used to create an object that doesn't "own" its content, so its content doesn't get deleted by `BC_JSON_Delete`.

To add items to an object, use `BC_JSON_AddItemToObject`. Use `BC_JSON_AddItemToObjectCS` to add an item to an object with a name that is a constant or reference (key of the item, `string` in the `DJSON` struct), so that it doesn't get freed by `BC_JSON_Delete`.
Using `BC_JSON_AddItemReferenceToArray` an element can be added as a reference to another object, array or string. This means that `BC_JSON_Delete` will not delete that items `child` or `value_string` properties, so no double frees are occurring if they are already used elsewhere.

If you want to take an item out of an object, use `BC_JSON_DetachItemFromObjectCaseSensitive`, it will return the detached item, so be sure to assign it to a pointer, otherwise you will have a memory leak.

Deleting items is done with `BC_JSON_DeleteItemFromObjectCaseSensitive`. It works like `BC_JSON_DetachItemFromObjectCaseSensitive` followed by `BC_JSON_Delete`.

You can also replace an item in an object in place. Either with `BC_JSON_ReplaceItemInObjectCaseSensitive` using a key or with `BC_JSON_ReplaceItemViaPointer` given a pointer to an element. `BC_JSON_ReplaceItemViaPointer` will return `0` if it fails. What this does internally is to detach the old item, delete it and insert the new item in its place.

To get the size of an object, you can use `BC_JSON_GetArraySize`, this works because internally objects are stored as arrays.

If you want to access an item in an object, use `BC_JSON_GetObjectItemCaseSensitive`.

To iterate over an object, you can use the `BC_JSON_ArrayForEach` macro the same way as for arrays.

DJSON also provides convenient helper functions for quickly creating a new item and adding it to an object, like `BC_JSON_AddNullToObject`. They return a pointer to the new item or `NULL` if they failed.

### Parsing JSON

Given some JSON in a zero terminated string, you can parse it with `BC_JSON_Parse`.

```c
DJSON *json = BC_JSON_Parse(string);
```

Given some JSON in a string (whether zero terminated or not), you can parse it with `BC_JSON_ParseWithLength`.

```c
DJSON *json = BC_JSON_ParseWithLength(string, buffer_length);
```

It will parse the JSON and allocate a tree of `DJSON` items that represents it. Once it returns, you are fully responsible for deallocating it after use with `BC_JSON_Delete`.

The allocator used by `BC_JSON_Parse` is `malloc` and `free` by default but can be changed (globally) with `BC_JSON_InitHooks`.

If an error occurs a pointer to the position of the error in the input string can be accessed using `BC_JSON_GetErrorPtr`. Note though that this can produce race conditions in multithreading scenarios, in that case it is better to use `BC_JSON_ParseWithOpts` with `return_parse_end`.
By default, characters in the input string that follow the parsed JSON will not be considered as an error.

If you want more options, use `BC_JSON_ParseWithOpts(const char *value, const char **return_parse_end, BC_JSON_bool require_null_terminated)`.
`return_parse_end` returns a pointer to the end of the JSON in the input string or the position that an error occurs at (thereby replacing `BC_JSON_GetErrorPtr` in a thread safe way). `require_null_terminated`, if set to `1` will make it an error if the input string contains data after the JSON.

If you want more options giving buffer length, use `BC_JSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, BC_JSON_bool require_null_terminated)`.

### Printing JSON

Given a tree of `DJSON` items, you can print them as a string using `BC_JSON_Print`.

```c
char *string = BC_JSON_Print(json);
```

It will allocate a string and print a JSON representation of the tree into it. Once it returns, you are fully responsible for deallocating it after use with your allocator. (usually `free`, depends on what has been set with `BC_JSON_InitHooks`).

`BC_JSON_Print` will print with whitespace for formatting. If you want to print without formatting, use `BC_JSON_PrintUnformatted`.

If you have a rough idea of how big your resulting string will be, you can use `BC_JSON_PrintBuffered(const DJSON *item, int prebuffer, BC_JSON_bool fmt)`. `fmt` is a boolean to turn formatting with whitespace on and off. `prebuffer` specifies the first buffer size to use for printing. `BC_JSON_Print` currently uses 256 bytes for its first buffer size. Once printing runs out of space, a new buffer is allocated and the old gets copied over before printing is continued.

These dynamic buffer allocations can be completely avoided by using `BC_JSON_PrintPreallocated(DJSON *item, char *buffer, const int length, const BC_JSON_bool format)`. It takes a buffer to a pointer to print to and its length. If the length is reached, printing will fail and it returns `0`. In case of success, `1` is returned. Note that you should provide 5 bytes more than is actually needed, because DJSON is not 100% accurate in estimating if the provided memory is enough.

### Example

In this example we want to build and parse the following JSON:

```json
{
    "name": "Awesome 4K",
    "resolutions": [
        {
            "width": 1280,
            "height": 720
        },
        {
            "width": 1920,
            "height": 1080
        },
        {
            "width": 3840,
            "height": 2160
        }
    ]
}
```

#### Printing

Let's build the above JSON and print it to a string:

```c
//create a monitor with a list of supported resolutions
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *create_monitor(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    DJSON *name = NULL;
    DJSON *resolutions = NULL;
    DJSON *resolution = NULL;
    DJSON *width = NULL;
    DJSON *height = NULL;
    size_t index = 0;

    DJSON *monitor = BC_JSON_CreateObject();
    if (monitor == NULL)
    {
        goto end;
    }

    name = BC_JSON_CreateString("Awesome 4K");
    if (name == NULL)
    {
        goto end;
    }
    /* after creation was successful, immediately add it to the monitor,
     * thereby transferring ownership of the pointer to it */
    BC_JSON_AddItemToObject(monitor, "name", name);

    resolutions = BC_JSON_CreateArray();
    if (resolutions == NULL)
    {
        goto end;
    }
    BC_JSON_AddItemToObject(monitor, "resolutions", resolutions);

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        resolution = BC_JSON_CreateObject();
        if (resolution == NULL)
        {
            goto end;
        }
        BC_JSON_AddItemToArray(resolutions, resolution);

        width = BC_JSON_CreateNumber(resolution_numbers[index][0]);
        if (width == NULL)
        {
            goto end;
        }
        BC_JSON_AddItemToObject(resolution, "width", width);

        height = BC_JSON_CreateNumber(resolution_numbers[index][1]);
        if (height == NULL)
        {
            goto end;
        }
        BC_JSON_AddItemToObject(resolution, "height", height);
    }

    string = BC_JSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    BC_JSON_Delete(monitor);
    return string;
}
```

Alternatively we can use the `BC_JSON_Add...ToObject` helper functions to make our lives a little easier:

```c
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *create_monitor_with_helpers(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    DJSON *resolutions = NULL;
    size_t index = 0;

    DJSON *monitor = BC_JSON_CreateObject();

    if (BC_JSON_AddStringToObject(monitor, "name", "Awesome 4K") == NULL)
    {
        goto end;
    }

    resolutions = BC_JSON_AddArrayToObject(monitor, "resolutions");
    if (resolutions == NULL)
    {
        goto end;
    }

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        DJSON *resolution = BC_JSON_CreateObject();

        if (BC_JSON_AddNumberToObject(resolution, "width", resolution_numbers[index][0]) == NULL)
        {
            goto end;
        }

        if (BC_JSON_AddNumberToObject(resolution, "height", resolution_numbers[index][1]) == NULL)
        {
            goto end;
        }

        BC_JSON_AddItemToArray(resolutions, resolution);
    }

    string = BC_JSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    BC_JSON_Delete(monitor);
    return string;
}
```

#### Parsing

In this example we will parse a JSON in the above format and check if the monitor supports a Full HD resolution while printing some diagnostic output:

```c
/* return 1 if the monitor supports full hd, 0 otherwise */
int supports_full_hd(const char * const monitor)
{
    const DJSON *resolution = NULL;
    const DJSON *resolutions = NULL;
    const DJSON *name = NULL;
    int status = 0;
    DJSON *monitor_json = BC_JSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char *error_ptr = BC_JSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = BC_JSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (BC_JSON_IsString(name) && (name->value_string != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->value_string);
    }

    resolutions = BC_JSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    BC_JSON_ArrayForEach(resolution, resolutions)
    {
        DJSON *width = BC_JSON_GetObjectItemCaseSensitive(resolution, "width");
        DJSON *height = BC_JSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!BC_JSON_IsNumber(width) || !BC_JSON_IsNumber(height))
        {
            status = 0;
            goto end;
        }

        if ((width->value_number == 1920) && (height->value_number == 1080))
        {
            status = 1;
            goto end;
        }
    }

end:
    BC_JSON_Delete(monitor_json);
    return status;
}
```

Note that there are no NULL checks except for the result of `BC_JSON_Parse` because `BC_JSON_GetObjectItemCaseSensitive` checks for `NULL` inputs already, so a `NULL` value is just propagated and `BC_JSON_IsNumber` and `BC_JSON_IsString` return `0` if the input is `NULL`.

### Caveats

#### Zero Character

DJSON doesn't support strings that contain the zero character `'\0'` or `\u0000`. This is impossible with the current API because strings are zero terminated.

#### Character Encoding

DJSON only supports UTF-8 encoded input. In most cases it doesn't reject invalid UTF-8 as input though, it just propagates it through as is. As long as the input doesn't contain invalid UTF-8, the output will always be valid UTF-8.

#### Floating Point Numbers

DJSON does not officially support any `double` implementations other than IEEE754 double precision floating point numbers. It might still work with other implementations but bugs with these will be considered invalid.

The maximum length of a floating point literal that DJSON supports is currently 63 characters.

#### Deep Nesting Of Arrays And Objects

DJSON doesn't support arrays and objects that are nested too deeply because this would result in a stack overflow. To prevent this DJSON limits the depth to `CJSON_NESTING_LIMIT` which is 1000 by default but can be changed at compile time.

#### Thread Safety

In general DJSON is **not thread safe**.

However it is thread safe under the following conditions:

* `BC_JSON_GetErrorPtr` is never used (the `return_parse_end` parameter of `BC_JSON_ParseWithOpts` can be used instead)
* `BC_JSON_InitHooks` is only ever called before using DJSON in any threads.
* `setlocale` is never called before all calls to DJSON functions have returned.

#### Case Sensitivity

When DJSON was originally created, it didn't follow the JSON standard and didn't make a distinction between uppercase and lowercase letters. If you want the correct, standard compliant, behavior, you need to use the `CaseSensitive` functions where available.

#### Duplicate Object Members

DJSON supports parsing and printing JSON that contains objects that have multiple members with the same name. `BC_JSON_GetObjectItemCaseSensitive` however will always only return the first one.

# Enjoy DJSON!

- Dave Gamble (original author)
- Max Bruckner and Alan Wang (current maintainer)
- and the other [DJSON contributors](CONTRIBUTORS.md)

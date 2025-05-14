/*
  Copyright (c) 2009-2017 Dave Gamble and BC_JSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef BC_JSON__h
#define BC_JSON__h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__WINDOWS__)                                                       \
	&& (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
	#define __WINDOWS__
#endif

#ifdef __WINDOWS__

		/* When compiling for windows, we specify a specific calling convention to avoid issues where
		we are being called from a project with a different default calling convention.  For windows
		you have 3 define options:

		CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
		CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols
		(default) CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

		For *nix builds that support visibility attribute, you can define similar behavior by

		setting default visibility to hidden by adding
		-fvisibility=hidden (for gcc)
		or
		-xldscope=hidden (for sun cc)
		to CFLAGS

		then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way
		CJSON_EXPORT_SYMBOLS does

		*/

	#define CJSON_CDECL   __cdecl
	#define CJSON_STDCALL __stdcall

	/* export symbols by default, this is necessary for copy pasting the C and header file */
	#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) \
		&& !defined(CJSON_EXPORT_SYMBOLS)
		#define CJSON_EXPORT_SYMBOLS
	#endif

	#if defined(CJSON_HIDE_SYMBOLS)
		#define CJSON_PUBLIC(type) type CJSON_STDCALL
	#elif defined(CJSON_EXPORT_SYMBOLS)
		#define CJSON_PUBLIC(type) __declspec(dllexport) type CJSON_STDCALL
	#elif defined(CJSON_IMPORT_SYMBOLS)
		#define CJSON_PUBLIC(type) __declspec(dllimport) type CJSON_STDCALL
	#endif
#else /* !__WINDOWS__ */
	#define CJSON_CDECL
	#define CJSON_STDCALL

	#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) \
		&& defined(CJSON_API_VISIBILITY)
		#define CJSON_PUBLIC(type) __attribute__((visibility("default"))) type
	#else
		#define CJSON_PUBLIC(type) type
	#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 18

#include <stddef.h>

/* BC_JSON Types: */
#define BC_JSON_Invalid       (0)
#define BC_JSON_False         (1 << 0)
#define BC_JSON_True          (1 << 1)
#define BC_JSON_NULL          (1 << 2)
#define JSON_TYPE.NUMBER        (1 << 3)
#define BC_JSON_String        (1 << 4)
#define BC_JSON_Array         (1 << 5)
#define BC_JSON_Object        (1 << 6)
#define BC_JSON_Raw           (1 << 7) /* raw json */

#define BC_JSON_IsReference   256
#define BC_JSON_StringIsConst 512

	/* The BC_JSON structure: */
	typedef struct BC_JSON {
		/* next/prev allow you to walk array/object chains. Alternatively, use
		 * GetArraySize/GetArrayItem/GetObjectItem */
		struct BC_JSON* next;
		struct BC_JSON* prev;
		/* An array or object item will have a child pointer pointing to a chain of the items in the
		 * array/object. */
		struct BC_JSON* child;

		/* The type of the item, as above. */
		int           type;

		/* The item's string, if type==BC_JSON_String  and type == BC_JSON_Raw */
		char*         value_string;

		/* The item's number, if type==JSON_TYPE.NUMBER */
		real          value_number;

		/* The item's name string, if this item is the child of, or is in the list of subitems of an
		 * object. */
		char*         name_string;
	} BC_JSON;

	typedef struct BC_JSON_Hooks {
		/* malloc/free are CDECL on Windows regardless of the default calling convention of the
		 * compiler, so ensure the hooks allow passing those functions directly. */
		void*(CJSON_CDECL* malloc_fn)(size_t sz);
		void(CJSON_CDECL* free_fn)(void* ptr);
	} BC_JSON_Hooks;

	typedef int BC_JSON_bool;

/* Limits how deeply nested arrays/objects can be before BC_JSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
	#define CJSON_NESTING_LIMIT 1000
#endif

/* Limits the length of circular references can be before BC_JSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_CIRCULAR_LIMIT
	#define CJSON_CIRCULAR_LIMIT 10000
#endif

	/* returns the version of BC_JSON as a string */
	CJSON_PUBLIC(const char*) BC_JSON_Version(void);

	/* Supply malloc, realloc and free functions to BC_JSON */
	CJSON_PUBLIC(void) BC_JSON_InitHooks(BC_JSON_Hooks* hooks);

	/* Memory Management: the caller is always responsible to free the results from all variants of
	 * BC_JSON_Parse (with BC_JSON_Delete) and BC_JSON_Print (with stdlib free, BC_JSON_Hooks.free_fn, or
	 * BC_JSON_free as appropriate). The exception is BC_JSON_PrintPreallocated, where the caller has
	 * full responsibility of the buffer. */
	/* Supply a block of JSON, and this returns a BC_JSON object you can interrogate. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_Parse(const char* value);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_ParseWithLength(const char* value, size_t buffer_length);
	/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to
	 * retrieve the pointer to the final byte parsed. */
	/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain
	 * a pointer to the error so will match BC_JSON_GetErrorPtr(). */
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_ParseWithOpts(
		const char* value, const char** return_parse_end, BC_JSON_bool require_null_terminated
	);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_ParseWithLengthOpts(
		const char*  value,
		size_t       buffer_length,
		const char** return_parse_end,
		BC_JSON_bool   require_null_terminated
	);

	/* Render a BC_JSON entity to text for transfer/storage. */
	CJSON_PUBLIC(char*) BC_JSON_Print(const BC_JSON* item);
	/* Render a BC_JSON entity to text for transfer/storage without any formatting. */
	CJSON_PUBLIC(char*) BC_JSON_PrintUnformatted(const BC_JSON* item);
	/* Render a BC_JSON entity to text using a buffered strategy. prebuffer is a guess at the final
	 * size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
	CJSON_PUBLIC(char*) BC_JSON_PrintBuffered(const BC_JSON* item, int prebuffer, BC_JSON_bool fmt);
	/* Render a BC_JSON entity to text using a buffer already allocated in memory with given length.
	 * Returns 1 on success and 0 on failure. */
	/* NOTE: BC_JSON is not always 100% accurate in estimating how much memory it will use, so to be
	 * safe allocate 5 bytes more than you actually need */
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_PrintPreallocated(BC_JSON* item, char* buffer, const int length, const BC_JSON_bool format);
	/* Delete a BC_JSON entity and all subentities. */
	CJSON_PUBLIC(void) BC_JSON_Delete(BC_JSON* item);

	/* Returns the number of items in an array (or object). */
	CJSON_PUBLIC(int) BC_JSON_GetArraySize(const BC_JSON* array);
	/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_GetArrayItem(const BC_JSON* array, int index);
	/* Get item "string" from object. Case insensitive. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_GetObjectItem(const BC_JSON* const object, const char* const string);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_GetObjectItemCaseSensitive(const BC_JSON* const object, const char* const string);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_HasObjectItem(const BC_JSON* object, const char* string);
	/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need
	 * to look a few chars back to make sense of it. Defined when BC_JSON_Parse() returns 0. 0 when
	 * BC_JSON_Parse() succeeds. */
	CJSON_PUBLIC(const char*) BC_JSON_GetErrorPtr(void);

	/* Check item type and return its value */
	CJSON_PUBLIC(char*) BC_JSON_GetStringValue(const BC_JSON* const item);
	CJSON_PUBLIC(double) BC_JSON_GetNumberValue(const BC_JSON* const item);

	/* These functions check the type of an item */
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsInvalid(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsFalse(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsTrue(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsBool(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsNull(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsNumber(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsString(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsArray(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsObject(const BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_IsRaw(const BC_JSON* const item);

	/* These calls create a BC_JSON item of the appropriate type. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateNull(void);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateTrue(void);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateFalse(void);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateBool(BC_JSON_bool boolean);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateNumber(double num);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateString(const char* string);
	/* raw json */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateRaw(const char* raw);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateArray(void);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateObject(void);

	/* Create a string where value_string references a string so
	 * it will not be freed by BC_JSON_Delete */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateStringReference(const char* string);
	/* Create an object/array that only references it's elements so
	 * they will not be freed by BC_JSON_Delete */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateObjectReference(const BC_JSON* child);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateArrayReference(const BC_JSON* child);

	/* These utilities create an Array of count items.
	 * The parameter count cannot be greater than the number of elements in the number array,
	 * otherwise array access will be out of bounds.*/
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateIntArray(const int* numbers, int count);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateFloatArray(const float* numbers, int count);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateDoubleArray(const double* numbers, int count);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_CreateStringArray(const char* const* strings, int count);

	/* Append item to the specified array/object. */
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_AddItemToArray(BC_JSON* array, BC_JSON* item);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_AddItemToObject(BC_JSON* object, const char* string, BC_JSON* item);
	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely
	 * survive the BC_JSON object. WARNING: When this function was used, make sure to always check that
	 * (item->type & BC_JSON_StringIsConst) is zero before writing to `item->string` */
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_AddItemToObjectCS(BC_JSON* object, const char* string, BC_JSON* item);
	/* Append reference to item to the specified array/object. Use this when you want to add an
	 * existing BC_JSON to a new BC_JSON, but don't want to corrupt your existing BC_JSON. */
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_AddItemReferenceToArray(BC_JSON* array, BC_JSON* item);
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_AddItemReferenceToObject(BC_JSON* object, const char* string, BC_JSON* item);

	/* Remove/Detach items from Arrays/Objects. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_DetachItemViaPointer(BC_JSON* parent, BC_JSON* const item);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_DetachItemFromArray(BC_JSON* array, int which);
	CJSON_PUBLIC(void) BC_JSON_DeleteItemFromArray(BC_JSON* array, int which);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_DetachItemFromObject(BC_JSON* object, const char* string);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_DetachItemFromObjectCaseSensitive(BC_JSON* object, const char* string);
	CJSON_PUBLIC(void) BC_JSON_DeleteItemFromObject(BC_JSON* object, const char* string);
	CJSON_PUBLIC(void) BC_JSON_DeleteItemFromObjectCaseSensitive(BC_JSON* object, const char* string);

	/* Update array items. */
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_InsertItemInArray(
		BC_JSON* array, int which, BC_JSON* newitem
	); /* Shifts pre-existing items to the right. */
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_ReplaceItemViaPointer(BC_JSON* const parent, BC_JSON* const item, BC_JSON* replacement);
	CJSON_PUBLIC(BC_JSON_bool) BC_JSON_ReplaceItemInArray(BC_JSON* array, int which, BC_JSON* newitem);
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_ReplaceItemInObject(BC_JSON* object, const char* string, BC_JSON* newitem);
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_ReplaceItemInObjectCaseSensitive(BC_JSON* object, const char* string, BC_JSON* newitem);

	/* Duplicate a BC_JSON item */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_Duplicate(const BC_JSON* item, BC_JSON_bool recurse);
	/* Duplicate will create a new, identical BC_JSON item to the one you pass, in new memory that will
	 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
	 * The item->next and ->prev pointers are always zero on return from Duplicate. */
	/* Recursively compare two BC_JSON items for equality. If either a or b is NULL or invalid, they
	 * will be considered unequal. case_sensitive determines if object keys are treated case
	 * sensitive (1) or case insensitive (0) */
	CJSON_PUBLIC(BC_JSON_bool)
	BC_JSON_Compare(const BC_JSON* const a, const BC_JSON* const b, const BC_JSON_bool case_sensitive);

	/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
	 * The input pointer json cannot point to a read-only address area, such as a string constant,
	 * but should point to a readable and writable address area. */
	CJSON_PUBLIC(void) BC_JSON_Minify(char* json);

	/* Helper functions for creating and adding items to an object at the same time.
	 * They return the added item or NULL on failure. */
	CJSON_PUBLIC(BC_JSON*) BC_JSON_AddNullToObject(BC_JSON* const object, const char* const name);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_AddTrueToObject(BC_JSON* const object, const char* const name);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_AddFalseToObject(BC_JSON* const object, const char* const name);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_AddBoolToObject(BC_JSON* const object, const char* const name, const BC_JSON_bool boolean);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_AddNumberToObject(BC_JSON* const object, const char* const name, const double number);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_AddStringToObject(BC_JSON* const object, const char* const name, const char* const string);
	CJSON_PUBLIC(BC_JSON*)
	BC_JSON_AddRawToObject(BC_JSON* const object, const char* const name, const char* const raw);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_AddObjectToObject(BC_JSON* const object, const char* const name);
	CJSON_PUBLIC(BC_JSON*) BC_JSON_AddArrayToObject(BC_JSON* const object, const char* const name);

	/* helper for the BC_JSON_SetNumberValue macro */
	CJSON_PUBLIC(double) BC_JSON_SetNumberHelper(BC_JSON* object, double number);
#define BC_JSON_SetNumberValue(object, number)                                    \
	((object != NULL) ? BC_JSON_SetNumberHelper(object, (double)number) : (number))

	/* Change the value_string of a BC_JSON_String object, only takes effect when type of object is
	 * BC_JSON_String */
	CJSON_PUBLIC(char*) BC_JSON_SetValuestring(BC_JSON* object, const char* value_string);

/* If the object is not a boolean type this does nothing and returns BC_JSON_Invalid else it returns
 * the new type*/
#define BC_JSON_SetBoolValue(object, boolValue)                            \
	((object != NULL && ((object)->type & (BC_JSON_False | BC_JSON_True)))     \
	   ? (object)->type = ((object)->type & (~(BC_JSON_False | BC_JSON_True))) \
	                    | ((boolValue) ? BC_JSON_True : BC_JSON_False)         \
	   : BC_JSON_Invalid)

/* Macro for iterating over an array or object */
#define BC_JSON_ArrayForEach(element, array) \
	for (element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

	/* malloc/free objects using the malloc/free functions that have been set with BC_JSON_InitHooks */
	CJSON_PUBLIC(void*) BC_JSON_malloc(size_t size);
	CJSON_PUBLIC(void) BC_JSON_free(void* object);

#ifdef __cplusplus
}
#endif

#endif

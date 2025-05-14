/*
  Copyright (c) 2025-2025 An-E IOSI and BC_JSON contributors

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

module etc.djson;

extern (C) @nogc nothrow:

/* project version */
enum BC_JSON_VERSION_MAJOR = 1;
enum BC_JSON_VERSION_MINOR = 7;
enum BC_JSON_VERSION_PATCH = 18;

enum JSON_TYPE {
	INVALID   = 0,
	FALSE     = 1 << 0,
	TRUE      = 1 << 1,
	NULL      = 1 << 2,
	NUMBER    = 1 << 3,
	STRING    = 1 << 4,
	ARRAY     = 1 << 5,
	OBJECT    = 1 << 6,
	RAW       = 1 << 7,
	REFERENCE = 1 << 8,
	CONST     = 1 << 9
}

/* The BC_JSON structure: */
struct BC_JSON {
	/* next/prev allow you to walk array/object chains. Alternatively, use
		 * GetArraySize/GetArrayItem/GetObjectItem */
	BC_JSON* next;
	BC_JSON* prev;
	/* An array or object item will have a child pointer pointing to a chain of the items in the
		 * array/object. */
	BC_JSON* child;

	/* The type of the item, as above. */
	JSON_TYPE type;

	/* The item's string, if type==BC_JSON_String  and type == BC_JSON_Raw */
	char* value_string;

	/* The item's number, if type==JSON_TYPE.NUMBER */
	double value_number;

	/* The item's name string, if this item is the child of, or is in the list of subitems of an
		 * object. */
	char* name_string;
}

struct BC_JSON_Hooks {
	/* malloc/free are CDECL on Windows regardless of the default calling convention of the
		 * compiler, so ensure the hooks allow passing those functions directly. */
	void* function(size_t sz) malloc_fn;
	void function(void* ptr) free_fn;
}

/* Limits how deeply nested arrays/objects can be before BC_JSON rejects to parse them.
 * This is to prevent stack overflows. */
static if (!__traits(compiles, BC_JSON_NESTING_LIMIT)) {
	enum BC_JSON_NESTING_LIMIT = 1000;
}

/* Limits how long circular references can be before BC_JSON rejects to parse them.
 * This is to prevent stack overflows. */
static if (!__traits(compiles, BC_JSON_CIRCULAR_LIMIT)) {
	enum BC_JSON_CIRCULAR_LIMIT = 10000;
}

/* returns the version of BC_JSON as a string */
const(char)* BC_JSON_Version();

/* Supply malloc, realloc and free functions to BC_JSON */
void BC_JSON_InitHooks(BC_JSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of
	 * BC_JSON_Parse (with BC_JSON_Delete) and BC_JSON_Print (with stdlib free, BC_JSON_Hooks.free_fn, or
	 * BC_JSON_free as appropriate). The exception is BC_JSON_PrintPreallocated, where the caller has
	 * full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a BC_JSON object you can interrogate. */
BC_JSON* BC_JSON_Parse(const(char)* value);
BC_JSON* BC_JSON_ParseWithLength(const(char)* value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to
	 * retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain
	 * a pointer to the error so will match BC_JSON_GetErrorPtr(). */
BC_JSON* BC_JSON_ParseWithOpts(const(char)* value,
	const(char)** return_parse_end, bool require_null_terminated);

BC_JSON* BC_JSON_ParseWithLengthOpts(const(char)* value, size_t buffer_length,
	const(char)** return_parse_end, bool require_null_terminated);

/* Render a BC_JSON entity to text for transfer/storage. */
char* BC_JSON_Print(const(BC_JSON)* item);
/* Render a BC_JSON entity to text for transfer/storage without any formatting. */
char* BC_JSON_PrintUnformatted(const(BC_JSON)* item);
/* Render a BC_JSON entity to text using a buffered strategy. prebuffer is a guess at the final
	 * size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
char* BC_JSON_PrintBuffered(const(BC_JSON)* item, int prebuffer, bool fmt);
/* Render a BC_JSON entity to text using a buffer already allocated in memory with given length.
	 * Returns 1 on success and 0 on failure. */
/* NOTE: BC_JSON is not always 100% accurate in estimating how much memory it will use, so to be
	 * safe allocate 5 bytes more than you actually need */
bool BC_JSON_PrintPreallocated(BC_JSON* item, char* buffer, const int length, const bool format);
/* Delete a BC_JSON entity and all subentities. */
void BC_JSON_Delete(BC_JSON* item);

/* Returns the number of items in an array (or object). */
int BC_JSON_GetArraySize(const(BC_JSON)* array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
BC_JSON* BC_JSON_GetArrayItem(const(BC_JSON)* array, int index);
/* Get item "string" from object. Case insensitive. */
BC_JSON* BC_JSON_GetObjectItem(const BC_JSON* object, const char* text);
BC_JSON* BC_JSON_GetObjectItemCaseSensitive(const BC_JSON* object, const char* text);
bool BC_JSON_HasObjectItem(const(BC_JSON)* object, const(char)* text);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need
	 * to look a few chars back to make sense of it. Defined when BC_JSON_Parse() returns 0. 0 when
	 * BC_JSON_Parse() succeeds. */
const(char)* BC_JSON_GetErrorPtr();

/* Check item type and return its value */
char* BC_JSON_GetStringValue(const BC_JSON* item);
double BC_JSON_GetNumberValue(const BC_JSON* item);

/* These functions check the type of an item */
bool BC_JSON_IsInvalid(const BC_JSON* item);
bool BC_JSON_IsFalse(const BC_JSON* item);
bool BC_JSON_IsTrue(const BC_JSON* item);
bool BC_JSON_IsBool(const BC_JSON* item);
bool BC_JSON_IsNull(const BC_JSON* item);
bool BC_JSON_IsNumber(const BC_JSON* item);
bool BC_JSON_IsString(const BC_JSON* item);
bool BC_JSON_IsArray(const BC_JSON* item);
bool BC_JSON_IsObject(const BC_JSON* item);
bool BC_JSON_IsRaw(const BC_JSON* item);

/* These calls create a BC_JSON item of the appropriate type. */
BC_JSON* BC_JSON_CreateNull();
BC_JSON* BC_JSON_CreateTrue();
BC_JSON* BC_JSON_CreateFalse();
BC_JSON* BC_JSON_CreateBool(bool boolean);
BC_JSON* BC_JSON_CreateNumber(double num);
BC_JSON* BC_JSON_CreateString(const(char)* text);
/* raw json */
BC_JSON* BC_JSON_CreateRaw(const(char)* raw);
BC_JSON* BC_JSON_CreateArray();
BC_JSON* BC_JSON_CreateObject();

/* Create a string where value_string references a string so
	 * it will not be freed by BC_JSON_Delete */
BC_JSON* BC_JSON_CreateStringReference(const(char)* text);
/* Create an object/array that only references it's elements so
	 * they will not be freed by BC_JSON_Delete */
BC_JSON* BC_JSON_CreateObjectReference(const(BC_JSON)* child);
BC_JSON* BC_JSON_CreateArrayReference(const(BC_JSON)* child);

/* These utilities create an Array of count items.
	 * The parameter count cannot be greater than the number of elements in the number array,
	 * otherwise array access will be out of bounds.*/
BC_JSON* BC_JSON_CreateIntArray(const int* numbers, int count);
BC_JSON* BC_JSON_CreateFloatArray(const float* numbers, int count);
BC_JSON* BC_JSON_CreateDoubleArray(const double* numbers, int count);
BC_JSON* BC_JSON_CreateStringArray(const char** strings, int count);

/* Append item to the specified array/object. */
bool BC_JSON_AddItemToArray(BC_JSON* array, BC_JSON* item);
bool BC_JSON_AddItemToObject(BC_JSON* object, const(char)* text, BC_JSON* item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely
	 * survive the BC_JSON object. WARNING: When this function was used, make sure to always check that
	 * (item.type & BC_JSON_StringIsConst) is zero before writing to `item.string` */
bool BC_JSON_AddItemToObjectCS(BC_JSON* object, const(char)* text, BC_JSON* item);
/* Append reference to item to the specified array/object. Use this when you want to add an
	 * existing BC_JSON to a new BC_JSON, but don't want to corrupt your existing BC_JSON. */
bool BC_JSON_AddItemReferenceToArray(BC_JSON* array, BC_JSON* item);
bool BC_JSON_AddItemReferenceToObject(BC_JSON* object, const(char)* text, BC_JSON* item);

/* Remove/Detach items from Arrays/Objects. */
BC_JSON* BC_JSON_DetachItemViaPointer(BC_JSON* parent, BC_JSON* item);
BC_JSON* BC_JSON_DetachItemFromArray(BC_JSON* array, int which);
void BC_JSON_DeleteItemFromArray(BC_JSON* array, int which);
BC_JSON* BC_JSON_DetachItemFromObject(BC_JSON* object, const(char)* text);
BC_JSON* BC_JSON_DetachItemFromObjectCaseSensitive(BC_JSON* object, const(char)* text);
void BC_JSON_DeleteItemFromObject(BC_JSON* object, const(char)* text);
void BC_JSON_DeleteItemFromObjectCaseSensitive(BC_JSON* object, const(char)* text);

/* Update array items. */
bool BC_JSON_InsertItemInArray(BC_JSON* array, int which, BC_JSON* newitem); /* Shifts pre-existing items to the right. */
bool BC_JSON_ReplaceItemViaPointer(BC_JSON* parent, BC_JSON* item, BC_JSON* replacement);
bool BC_JSON_ReplaceItemInArray(BC_JSON* array, int which, BC_JSON* newitem);
bool BC_JSON_ReplaceItemInObject(BC_JSON* object, const(char)* text, BC_JSON* newitem);
bool BC_JSON_ReplaceItemInObjectCaseSensitive(BC_JSON* object, const(char)* text, BC_JSON* newitem);

/* Duplicate a BC_JSON item */
BC_JSON* BC_JSON_Duplicate(const(BC_JSON)* item, bool recurse);
/* Duplicate will create a new, identical BC_JSON item to the one you pass, in new memory that will
	 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
	 * The item.next and .prev pointers are always zero on return from Duplicate. */
/* Recursively compare two BC_JSON items for equality. If either a or b is NULL or invalid, they
	 * will be considered unequal. case_sensitive determines if object keys are treated case
	 * sensitive (1) or case insensitive (0) */
bool BC_JSON_Compare(const BC_JSON* a, const BC_JSON* b, const bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
	 * The input pointer json cannot point to a read-only address area, such as a string constant,
	 * but should point to a readable and writable address area. */
void BC_JSON_Minify(char* json);

/* Helper functions for creating and adding items to an object at the same time.
	 * They return the added item or NULL on failure. */
BC_JSON* BC_JSON_AddNullToObject(scope BC_JSON* object, const char* name);
BC_JSON* BC_JSON_AddTrueToObject(scope BC_JSON* object, const char* name);
BC_JSON* BC_JSON_AddFalseToObject(scope BC_JSON* object, const char* name);
BC_JSON* BC_JSON_AddBoolToObject(scope BC_JSON* object, const char* name, const bool boolean);
BC_JSON* BC_JSON_AddNumberToObject(scope BC_JSON* object, const char* name, const double number);
BC_JSON* BC_JSON_AddStringToObject(scope BC_JSON* object, const char* name, const char* text);
BC_JSON* BC_JSON_AddRawToObject(scope BC_JSON* object, const char* name, const char* raw);
BC_JSON* BC_JSON_AddObjectToObject(scope BC_JSON* object, const char* name);
BC_JSON* BC_JSON_AddArrayToObject(scope BC_JSON* object, const char* name);

double BC_JSON_SetNumberValue(BC_JSON* object, double number);

/* Change the value_string of a BC_JSON_String object, only takes effect when type of object is
	 * BC_JSON_String */
char* BC_JSON_SetValuestring(BC_JSON* object, const(char)* value_string);

/* If the object is not a boolean type this does nothing and returns BC_JSON_Invalid else it returns
 * the new type*/
enum BC_JSON_SetBoolValue(object, boolValue) = ((object != NULL && ((object)
			.type & (BC_JSON_False | BC_JSON_True))) ? (object).type = ((object)
			.type & (~(BC_JSON_False | BC_JSON_True))) | ((boolValue) ? BC_JSON_True
			: BC_JSON_False) : BC_JSON_Invalid);

/* malloc/free objects using the malloc/free functions that have been set with BC_JSON_InitHooks */
void* BC_JSON_malloc(size_t size);
void BC_JSON_free(void* object);

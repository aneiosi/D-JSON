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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

static void cjson_array_foreach_should_loop_over_arrays(void)
{
    BC_JSON array[1];
    BC_JSON elements[10];
    BC_JSON *element_pointer = NULL;
    size_t i = 0;

    memset(array, 0, sizeof(array));
    memset(elements, 0, sizeof(elements));

    /* create array */
    array[0].child = &elements[0];
    elements[0].prev = NULL;
    elements[9].next = NULL;
    for (i = 0; i < 9; i++)
    {
        elements[i].next = &elements[i + 1];
        elements[i + 1].prev = &elements[i];
    }

    i = 0;
    BC_JSON_ArrayForEach(element_pointer, array)
    {
        TEST_ASSERT_TRUE_MESSAGE(element_pointer == &elements[i], "Not iterating over array properly");
        i++;
    }
}

static void cjson_array_foreach_should_not_dereference_null_pointer(void)
{
    BC_JSON *array = NULL;
    BC_JSON *element = NULL;
    BC_JSON_ArrayForEach(element, array);
}

static void cjson_get_object_item_should_get_object_items(void)
{
    BC_JSON *item = NULL;
    BC_JSON *found = NULL;

    item = BC_JSON_Parse("{\"one\":1, \"Two\":2, \"tHree\":3}");

    found = BC_JSON_GetObjectItem(NULL, "test");
    TEST_ASSERT_NULL_MESSAGE(found, "Failed to fail on NULL pointer.");

    found = BC_JSON_GetObjectItem(item, NULL);
    TEST_ASSERT_NULL_MESSAGE(found, "Failed to fail on NULL string.");

    found = BC_JSON_GetObjectItem(item, "one");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find first item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 1);

    found = BC_JSON_GetObjectItem(item, "tWo");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find first item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 2);

    found = BC_JSON_GetObjectItem(item, "three");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 3);

    found = BC_JSON_GetObjectItem(item, "four");
    TEST_ASSERT_NULL_MESSAGE(found, "Should not find something that isn't there.");

    BC_JSON_Delete(item);
}

static void cjson_get_object_item_case_sensitive_should_get_object_items(void)
{
    BC_JSON *item = NULL;
    BC_JSON *found = NULL;

    item = BC_JSON_Parse("{\"one\":1, \"Two\":2, \"tHree\":3}");

    found = BC_JSON_GetObjectItemCaseSensitive(NULL, "test");
    TEST_ASSERT_NULL_MESSAGE(found, "Failed to fail on NULL pointer.");

    found = BC_JSON_GetObjectItemCaseSensitive(item, NULL);
    TEST_ASSERT_NULL_MESSAGE(found, "Failed to fail on NULL string.");

    found = BC_JSON_GetObjectItemCaseSensitive(item, "one");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find first item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 1);

    found = BC_JSON_GetObjectItemCaseSensitive(item, "Two");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find first item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 2);

    found = BC_JSON_GetObjectItemCaseSensitive(item, "tHree");
    TEST_ASSERT_NOT_NULL_MESSAGE(found, "Failed to find item.");
    TEST_ASSERT_EQUAL_DOUBLE(found->value_number, 3);

    found = BC_JSON_GetObjectItemCaseSensitive(item, "One");
    TEST_ASSERT_NULL_MESSAGE(found, "Should not find something that isn't there.");

    BC_JSON_Delete(item);
}

static void cjson_get_object_item_should_not_crash_with_array(void)
{
    BC_JSON *array = NULL;
    BC_JSON *found = NULL;
    array = BC_JSON_Parse("[1]");

    found = BC_JSON_GetObjectItem(array, "name");
    TEST_ASSERT_NULL(found);

    BC_JSON_Delete(array);
}

static void cjson_get_object_item_case_sensitive_should_not_crash_with_array(void)
{
    BC_JSON *array = NULL;
    BC_JSON *found = NULL;
    array = BC_JSON_Parse("[1]");

    found = BC_JSON_GetObjectItemCaseSensitive(array, "name");
    TEST_ASSERT_NULL(found);

    BC_JSON_Delete(array);
}

static void typecheck_functions_should_check_type(void)
{
    BC_JSON invalid[1];
    BC_JSON item[1];
    invalid->type = BC_JSON_Invalid;
    invalid->type |= BC_JSON_StringIsConst;
    item->type = BC_JSON_False;
    item->type |= BC_JSON_StringIsConst;

    TEST_ASSERT_FALSE(BC_JSON_IsInvalid(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsInvalid(item));
    TEST_ASSERT_TRUE(BC_JSON_IsInvalid(invalid));

    item->type = BC_JSON_False | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsFalse(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsFalse(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsFalse(item));
    TEST_ASSERT_TRUE(BC_JSON_IsBool(item));

    item->type = BC_JSON_True | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsTrue(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsTrue(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsTrue(item));
    TEST_ASSERT_TRUE(BC_JSON_IsBool(item));

    item->type = BC_JSON_NULL | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsNull(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsNull(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsNull(item));

    item->type = JSON_TYPE.NUMBER | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsNumber(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsNumber(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsNumber(item));

    item->type = BC_JSON_String | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsString(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsString(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsString(item));

    item->type = BC_JSON_Array | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsArray(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsArray(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsArray(item));

    item->type = BC_JSON_Object | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsObject(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsObject(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsObject(item));

    item->type = BC_JSON_Raw | BC_JSON_StringIsConst;
    TEST_ASSERT_FALSE(BC_JSON_IsRaw(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsRaw(invalid));
    TEST_ASSERT_TRUE(BC_JSON_IsRaw(item));
}

static void cjson_should_not_parse_to_deeply_nested_jsons(void)
{
    char deep_json[CJSON_NESTING_LIMIT + 1];
    size_t position = 0;

    for (position = 0; position < sizeof(deep_json); position++)
    {
        deep_json[position] = '[';
    }
    deep_json[sizeof(deep_json) - 1] = '\0';

    TEST_ASSERT_NULL_MESSAGE(BC_JSON_Parse(deep_json), "To deep JSONs should not be parsed.");
}

static void cjson_should_not_follow_too_deep_circular_references(void)
{
    BC_JSON *o = BC_JSON_CreateArray();
    BC_JSON *a = BC_JSON_CreateArray();
    BC_JSON *b = BC_JSON_CreateArray();
    BC_JSON *x;

    BC_JSON_AddItemToArray(o, a);
    BC_JSON_AddItemToArray(a, b);
    BC_JSON_AddItemToArray(b, o);

    x = BC_JSON_Duplicate(o, 1);
    TEST_ASSERT_NULL(x);
    BC_JSON_DetachItemFromArray(b, 0);
    BC_JSON_Delete(o);
}

static void cjson_set_number_value_should_set_numbers(void)
{
    BC_JSON number[1] = {{NULL, NULL, NULL, JSON_TYPE.NUMBER, NULL, 0, 0, NULL}};

    BC_JSON_SetNumberValue(number, 1.5);
    TEST_ASSERT_EQUAL_DOUBLE(1.5, number->value_number);

    BC_JSON_SetNumberValue(number, -1.5);
    TEST_ASSERT_EQUAL_DOUBLE(-1.5, number->value_number);

    BC_JSON_SetNumberValue(number, 1 + (double)INT_MAX);
    TEST_ASSERT_EQUAL_DOUBLE(1 + (double)INT_MAX, number->value_number);

    BC_JSON_SetNumberValue(number, -1 + (double)INT_MIN);
    TEST_ASSERT_EQUAL_DOUBLE(-1 + (double)INT_MIN, number->value_number);
}

static void cjson_detach_item_via_pointer_should_detach_items(void)
{
    BC_JSON list[4];
    BC_JSON parent[1];

    memset(list, '\0', sizeof(list));

    /* link the list */
    list[0].next = &(list[1]);
    list[1].next = &(list[2]);
    list[2].next = &(list[3]);

    list[3].prev = &(list[2]);
    list[2].prev = &(list[1]);
    list[1].prev = &(list[0]);
    list[0].prev = &(list[3]);

    parent->child = &list[0];

    /* detach in the middle (list[1]) */
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &(list[1])) == &(list[1]), "Failed to detach in the middle.");
    TEST_ASSERT_TRUE_MESSAGE((list[1].prev == NULL) && (list[1].next == NULL), "Didn't set pointers of detached item to NULL.");
    TEST_ASSERT_TRUE((list[0].next == &(list[2])) && (list[2].prev == &(list[0])));

    /* detach beginning (list[0]) */
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &(list[0])) == &(list[0]), "Failed to detach beginning.");
    TEST_ASSERT_TRUE_MESSAGE((list[0].prev == NULL) && (list[0].next == NULL), "Didn't set pointers of detached item to NULL.");
    TEST_ASSERT_TRUE_MESSAGE((list[2].prev == &(list[3])) && (parent->child == &(list[2])), "Didn't set the new beginning.");

    /* detach end (list[3])*/
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &(list[3])) == &(list[3]), "Failed to detach end.");
    TEST_ASSERT_TRUE_MESSAGE((list[3].prev == NULL) && (list[3].next == NULL), "Didn't set pointers of detached item to NULL.");
    TEST_ASSERT_TRUE_MESSAGE((list[2].next == NULL) && (parent->child == &(list[2])), "Didn't set the new end");

    /* detach single item (list[2]) */
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &list[2]) == &list[2], "Failed to detach single item.");
    TEST_ASSERT_TRUE_MESSAGE((list[2].prev == NULL) && (list[2].next == NULL), "Didn't set pointers of detached item to NULL.");
    TEST_ASSERT_NULL_MESSAGE(parent->child, "Child of the parent wasn't set to NULL.");
}

static void cjson_detach_item_via_pointer_should_return_null_if_item_prev_is_null(void)
{
    BC_JSON list[2];
    BC_JSON parent[1];

    memset(list, '\0', sizeof(list));

    /* link the list */
    list[0].next = &(list[1]);

    parent->child = &list[0];
    TEST_ASSERT_NULL_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &(list[1])), "Failed to detach in the middle.");
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_DetachItemViaPointer(parent, &(list[0])) == &(list[0]), "Failed to detach in the middle.");
}

static void cjson_replace_item_via_pointer_should_replace_items(void)
{
    BC_JSON replacements[3];
    BC_JSON *beginning = NULL;
    BC_JSON *middle = NULL;
    BC_JSON *end = NULL;
    BC_JSON *array = NULL;

    beginning = BC_JSON_CreateNull();
    TEST_ASSERT_NOT_NULL(beginning);
    middle = BC_JSON_CreateNull();
    TEST_ASSERT_NOT_NULL(middle);
    end = BC_JSON_CreateNull();
    TEST_ASSERT_NOT_NULL(end);

    array = BC_JSON_CreateArray();
    TEST_ASSERT_NOT_NULL(array);

    BC_JSON_AddItemToArray(array, beginning);
    BC_JSON_AddItemToArray(array, middle);
    BC_JSON_AddItemToArray(array, end);

    memset(replacements, '\0', sizeof(replacements));

    /* replace beginning */
    TEST_ASSERT_TRUE(BC_JSON_ReplaceItemViaPointer(array, beginning, &(replacements[0])));
    TEST_ASSERT_TRUE(replacements[0].prev == end);
    TEST_ASSERT_TRUE(replacements[0].next == middle);
    TEST_ASSERT_TRUE(middle->prev == &(replacements[0]));
    TEST_ASSERT_TRUE(array->child == &(replacements[0]));

    /* replace middle */
    TEST_ASSERT_TRUE(BC_JSON_ReplaceItemViaPointer(array, middle, &(replacements[1])));
    TEST_ASSERT_TRUE(replacements[1].prev == &(replacements[0]));
    TEST_ASSERT_TRUE(replacements[1].next == end);
    TEST_ASSERT_TRUE(end->prev == &(replacements[1]));

    /* replace end */
    TEST_ASSERT_TRUE(BC_JSON_ReplaceItemViaPointer(array, end, &(replacements[2])));
    TEST_ASSERT_TRUE(replacements[2].prev == &(replacements[1]));
    TEST_ASSERT_NULL(replacements[2].next);
    TEST_ASSERT_TRUE(replacements[1].next == &(replacements[2]));

    BC_JSON_free(array);
}

static void cjson_replace_item_in_object_should_preserve_name(void)
{
    BC_JSON root[1] = {{NULL, NULL, NULL, 0, NULL, 0, 0, NULL}};
    BC_JSON *child = NULL;
    BC_JSON *replacement = NULL;
    BC_JSON_bool flag = false;

    child = BC_JSON_CreateNumber(1);
    TEST_ASSERT_NOT_NULL(child);
    replacement = BC_JSON_CreateNumber(2);
    TEST_ASSERT_NOT_NULL(replacement);

    flag = BC_JSON_AddItemToObject(root, "child", child);
    TEST_ASSERT_TRUE_MESSAGE(flag, "add item to object failed");
    BC_JSON_ReplaceItemInObject(root, "child", replacement);

    TEST_ASSERT_TRUE(root->child == replacement);
    TEST_ASSERT_EQUAL_STRING("child", replacement->string);

    BC_JSON_Delete(replacement);
}

static void cjson_functions_should_not_crash_with_null_pointers(void)
{
    char buffer[10];
    BC_JSON *item = BC_JSON_CreateString("item");
    BC_JSON *array = BC_JSON_CreateArray();
    BC_JSON *item1 = BC_JSON_CreateString("item1");
    BC_JSON *item2 = BC_JSON_CreateString("corrupted array item3");
    BC_JSON *corruptedString = BC_JSON_CreateString("corrupted");
    struct BC_JSON *originalPrev;

    add_item_to_array(array, item1);
    add_item_to_array(array, item2);

    originalPrev = item2->prev;
    item2->prev = NULL;
    free(corruptedString->value_string);
    corruptedString->value_string = NULL;

    BC_JSON_InitHooks(NULL);
    TEST_ASSERT_NULL(BC_JSON_Parse(NULL));
    TEST_ASSERT_NULL(BC_JSON_ParseWithOpts(NULL, NULL, true));
    TEST_ASSERT_NULL(BC_JSON_Print(NULL));
    TEST_ASSERT_NULL(BC_JSON_PrintUnformatted(NULL));
    TEST_ASSERT_NULL(BC_JSON_PrintBuffered(NULL, 10, true));
    TEST_ASSERT_FALSE(BC_JSON_PrintPreallocated(NULL, buffer, sizeof(buffer), true));
    TEST_ASSERT_FALSE(BC_JSON_PrintPreallocated(item, NULL, 1, true));
    BC_JSON_Delete(NULL);
    BC_JSON_GetArraySize(NULL);
    TEST_ASSERT_NULL(BC_JSON_GetArrayItem(NULL, 0));
    TEST_ASSERT_NULL(BC_JSON_GetObjectItem(NULL, "item"));
    TEST_ASSERT_NULL(BC_JSON_GetObjectItem(item, NULL));
    TEST_ASSERT_NULL(BC_JSON_GetObjectItemCaseSensitive(NULL, "item"));
    TEST_ASSERT_NULL(BC_JSON_GetObjectItemCaseSensitive(item, NULL));
    TEST_ASSERT_FALSE(BC_JSON_HasObjectItem(NULL, "item"));
    TEST_ASSERT_FALSE(BC_JSON_HasObjectItem(item, NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsInvalid(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsFalse(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsTrue(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsBool(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsNull(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsNumber(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsString(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsArray(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsObject(NULL));
    TEST_ASSERT_FALSE(BC_JSON_IsRaw(NULL));
    TEST_ASSERT_NULL(BC_JSON_CreateString(NULL));
    TEST_ASSERT_NULL(BC_JSON_CreateRaw(NULL));
    TEST_ASSERT_NULL(BC_JSON_CreateIntArray(NULL, 10));
    TEST_ASSERT_NULL(BC_JSON_CreateFloatArray(NULL, 10));
    TEST_ASSERT_NULL(BC_JSON_CreateDoubleArray(NULL, 10));
    TEST_ASSERT_NULL(BC_JSON_CreateStringArray(NULL, 10));
    BC_JSON_AddItemToArray(NULL, item);
    BC_JSON_AddItemToArray(item, NULL);
    BC_JSON_AddItemToObject(item, "item", NULL);
    BC_JSON_AddItemToObject(item, NULL, item);
    BC_JSON_AddItemToObject(NULL, "item", item);
    BC_JSON_AddItemToObjectCS(item, "item", NULL);
    BC_JSON_AddItemToObjectCS(item, NULL, item);
    BC_JSON_AddItemToObjectCS(NULL, "item", item);
    BC_JSON_AddItemReferenceToArray(NULL, item);
    BC_JSON_AddItemReferenceToArray(item, NULL);
    BC_JSON_AddItemReferenceToObject(item, "item", NULL);
    BC_JSON_AddItemReferenceToObject(item, NULL, item);
    BC_JSON_AddItemReferenceToObject(NULL, "item", item);
    TEST_ASSERT_NULL(BC_JSON_DetachItemViaPointer(NULL, item));
    TEST_ASSERT_NULL(BC_JSON_DetachItemViaPointer(item, NULL));
    TEST_ASSERT_NULL(BC_JSON_DetachItemFromArray(NULL, 0));
    BC_JSON_DeleteItemFromArray(NULL, 0);
    TEST_ASSERT_NULL(BC_JSON_DetachItemFromObject(NULL, "item"));
    TEST_ASSERT_NULL(BC_JSON_DetachItemFromObject(item, NULL));
    TEST_ASSERT_NULL(BC_JSON_DetachItemFromObjectCaseSensitive(NULL, "item"));
    TEST_ASSERT_NULL(BC_JSON_DetachItemFromObjectCaseSensitive(item, NULL));
    BC_JSON_DeleteItemFromObject(NULL, "item");
    BC_JSON_DeleteItemFromObject(item, NULL);
    BC_JSON_DeleteItemFromObjectCaseSensitive(NULL, "item");
    BC_JSON_DeleteItemFromObjectCaseSensitive(item, NULL);
    TEST_ASSERT_FALSE(BC_JSON_InsertItemInArray(array, 0, NULL));
    TEST_ASSERT_FALSE(BC_JSON_InsertItemInArray(array, 1, item));
    TEST_ASSERT_FALSE(BC_JSON_InsertItemInArray(NULL, 0, item));
    TEST_ASSERT_FALSE(BC_JSON_InsertItemInArray(item, 0, NULL));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemViaPointer(NULL, item, item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemViaPointer(item, NULL, item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemViaPointer(item, item, NULL));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInArray(item, 0, NULL));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInArray(NULL, 0, item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObject(NULL, "item", item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObject(item, NULL, item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObject(item, "item", NULL));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObjectCaseSensitive(NULL, "item", item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObjectCaseSensitive(item, NULL, item));
    TEST_ASSERT_FALSE(BC_JSON_ReplaceItemInObjectCaseSensitive(item, "item", NULL));
    TEST_ASSERT_NULL(BC_JSON_Duplicate(NULL, true));
    TEST_ASSERT_FALSE(BC_JSON_Compare(item, NULL, false));
    TEST_ASSERT_FALSE(BC_JSON_Compare(NULL, item, false));
    TEST_ASSERT_NULL(BC_JSON_SetValuestring(NULL, "test"));
    TEST_ASSERT_NULL(BC_JSON_SetValuestring(corruptedString, "test"));
    TEST_ASSERT_NULL(BC_JSON_SetValuestring(item, NULL));
    BC_JSON_Minify(NULL);
    /* skipped because it is only used via a macro that checks for NULL */
    /* BC_JSON_SetNumberHelper(NULL, 0); */

    /* restore corrupted item2 to delete it */
    item2->prev = originalPrev;
    BC_JSON_Delete(corruptedString);
    BC_JSON_Delete(array);
    BC_JSON_Delete(item);
}

static void cjson_set_valuestring_should_return_null_if_strings_overlap(void)
{
    BC_JSON *obj;
    char* str;
    char* str2;

    obj =  BC_JSON_Parse("\"foo0z\"");

    str =  BC_JSON_SetValuestring(obj, "abcde");
    str += 1;
    /* The string passed to strcpy overlap which is not allowed.*/
    str2 = BC_JSON_SetValuestring(obj, str);
    /* If it overlaps, the string will be messed up.*/
    TEST_ASSERT_TRUE(strcmp(str, "bcde") == 0);
    TEST_ASSERT_NULL(str2);
    BC_JSON_Delete(obj);
}

static void *CJSON_CDECL failing_realloc(void *pointer, size_t size)
{
    (void)size;
    (void)pointer;
    return NULL;
}

static void ensure_should_fail_on_failed_realloc(void)
{
    printbuffer buffer = {NULL, 10, 0, 0, false, false, {&malloc, &free, &failing_realloc}};
    buffer.buffer = (unsigned char *)malloc(100);
    TEST_ASSERT_NOT_NULL(buffer.buffer);

    TEST_ASSERT_NULL_MESSAGE(ensure(&buffer, 200), "Ensure didn't fail with failing realloc.");
}

static void skip_utf8_bom_should_skip_bom(void)
{
    const unsigned char string[] = "\xEF\xBB\xBF{}";
    parse_buffer buffer = {0, 0, 0, 0, {0, 0, 0}};
    buffer.content = string;
    buffer.length = sizeof(string);
    buffer.hooks = global_hooks;

    TEST_ASSERT_TRUE(skip_utf8_bom(&buffer) == &buffer);
    TEST_ASSERT_EQUAL_UINT(3U, (unsigned int)buffer.offset);
}

static void skip_utf8_bom_should_not_skip_bom_if_not_at_beginning(void)
{
    const unsigned char string[] = " \xEF\xBB\xBF{}";
    parse_buffer buffer = {0, 0, 0, 0, {0, 0, 0}};
    buffer.content = string;
    buffer.length = sizeof(string);
    buffer.hooks = global_hooks;
    buffer.offset = 1;

    TEST_ASSERT_NULL(skip_utf8_bom(&buffer));
}

static void cjson_get_string_value_should_get_a_string(void)
{
    BC_JSON *string = BC_JSON_CreateString("test");
    BC_JSON *number = BC_JSON_CreateNumber(1);

    TEST_ASSERT_TRUE(BC_JSON_GetStringValue(string) == string->value_string);
    TEST_ASSERT_NULL(BC_JSON_GetStringValue(number));
    TEST_ASSERT_NULL(BC_JSON_GetStringValue(NULL));

    BC_JSON_Delete(number);
    BC_JSON_Delete(string);
}

static void cjson_get_number_value_should_get_a_number(void)
{
    BC_JSON *string = BC_JSON_CreateString("test");
    BC_JSON *number = BC_JSON_CreateNumber(1);

    TEST_ASSERT_EQUAL_DOUBLE(BC_JSON_GetNumberValue(number), number->value_number);
    TEST_ASSERT_DOUBLE_IS_NAN(BC_JSON_GetNumberValue(string));
    TEST_ASSERT_DOUBLE_IS_NAN(BC_JSON_GetNumberValue(NULL));

    BC_JSON_Delete(number);
    BC_JSON_Delete(string);
}

static void cjson_create_string_reference_should_create_a_string_reference(void)
{
    const char *string = "I am a string!";

    BC_JSON *string_reference = BC_JSON_CreateStringReference(string);
    TEST_ASSERT_TRUE(string_reference->value_string == string);
    TEST_ASSERT_EQUAL_INT(BC_JSON_IsReference | BC_JSON_String, string_reference->type);

    BC_JSON_Delete(string_reference);
}

static void cjson_create_object_reference_should_create_an_object_reference(void)
{
    BC_JSON *number_reference = NULL;
    BC_JSON *number_object = BC_JSON_CreateObject();
    BC_JSON *number = BC_JSON_CreateNumber(42);
    const char key[] = "number";

    TEST_ASSERT_TRUE(BC_JSON_IsNumber(number));
    TEST_ASSERT_TRUE(BC_JSON_IsObject(number_object));
    BC_JSON_AddItemToObjectCS(number_object, key, number);

    number_reference = BC_JSON_CreateObjectReference(number);
    TEST_ASSERT_TRUE(number_reference->child == number);
    TEST_ASSERT_EQUAL_INT(BC_JSON_Object | BC_JSON_IsReference, number_reference->type);

    BC_JSON_Delete(number_object);
    BC_JSON_Delete(number_reference);
}

static void cjson_create_array_reference_should_create_an_array_reference(void)
{
    BC_JSON *number_reference = NULL;
    BC_JSON *number_array = BC_JSON_CreateArray();
    BC_JSON *number = BC_JSON_CreateNumber(42);

    TEST_ASSERT_TRUE(BC_JSON_IsNumber(number));
    TEST_ASSERT_TRUE(BC_JSON_IsArray(number_array));
    BC_JSON_AddItemToArray(number_array, number);

    number_reference = BC_JSON_CreateArrayReference(number);
    TEST_ASSERT_TRUE(number_reference->child == number);
    TEST_ASSERT_EQUAL_INT(BC_JSON_Array | BC_JSON_IsReference, number_reference->type);

    BC_JSON_Delete(number_array);
    BC_JSON_Delete(number_reference);
}

static void cjson_add_item_to_object_or_array_should_not_add_itself(void)
{
    BC_JSON *object = BC_JSON_CreateObject();
    BC_JSON *array = BC_JSON_CreateArray();
    BC_JSON_bool flag = false;

    flag = BC_JSON_AddItemToObject(object, "key", object);
    TEST_ASSERT_FALSE_MESSAGE(flag, "add an object to itself should fail");

    flag = BC_JSON_AddItemToArray(array, array);
    TEST_ASSERT_FALSE_MESSAGE(flag, "add an array to itself should fail");

    BC_JSON_Delete(object);
    BC_JSON_Delete(array);
}

static void cjson_add_item_to_object_should_not_use_after_free_when_string_is_aliased(void)
{
    BC_JSON *object = BC_JSON_CreateObject();
    BC_JSON *number = BC_JSON_CreateNumber(42);
    char *name = (char *)BC_JSON_strdup((const unsigned char *)"number", &global_hooks);

    TEST_ASSERT_NOT_NULL(object);
    TEST_ASSERT_NOT_NULL(number);
    TEST_ASSERT_NOT_NULL(name);

    number->string = name;

    /* The following should not have a use after free
     * that would show up in valgrind or with AddressSanitizer */
    BC_JSON_AddItemToObject(object, number->string, number);

    BC_JSON_Delete(object);
}

static void cjson_delete_item_from_array_should_not_broken_list_structure(void)
{
    const char expected_json1[] = "{\"rd\":[{\"a\":\"123\"}]}";
    const char expected_json2[] = "{\"rd\":[{\"a\":\"123\"},{\"b\":\"456\"}]}";
    const char expected_json3[] = "{\"rd\":[{\"b\":\"456\"}]}";
    char *str1 = NULL;
    char *str2 = NULL;
    char *str3 = NULL;

    BC_JSON *root = BC_JSON_Parse("{}");

    BC_JSON *array = BC_JSON_AddArrayToObject(root, "rd");
    BC_JSON *item1 = BC_JSON_Parse("{\"a\":\"123\"}");
    BC_JSON *item2 = BC_JSON_Parse("{\"b\":\"456\"}");

    BC_JSON_AddItemToArray(array, item1);
    str1 = BC_JSON_PrintUnformatted(root);
    TEST_ASSERT_EQUAL_STRING(expected_json1, str1);
    free(str1);

    BC_JSON_AddItemToArray(array, item2);
    str2 = BC_JSON_PrintUnformatted(root);
    TEST_ASSERT_EQUAL_STRING(expected_json2, str2);
    free(str2);

    /* this should not broken list structure */
    BC_JSON_DeleteItemFromArray(array, 0);
    str3 = BC_JSON_PrintUnformatted(root);
    TEST_ASSERT_EQUAL_STRING(expected_json3, str3);
    free(str3);

    BC_JSON_Delete(root);
}

static void cjson_set_valuestring_to_object_should_not_leak_memory(void)
{
    BC_JSON *root = BC_JSON_Parse("{}");
    const char *stringvalue = "value_string could be changed safely";
    const char *reference_valuestring = "reference item should be freed by yourself";
    const char *short_valuestring = "shorter value_string";
    const char *long_valuestring = "new value_string which much longer than previous should be changed safely";
    BC_JSON *item1 = BC_JSON_CreateString(stringvalue);
    BC_JSON *item2 = BC_JSON_CreateStringReference(reference_valuestring);
    char *ptr1 = NULL;
    char *return_value = NULL;

    BC_JSON_AddItemToObject(root, "one", item1);
    BC_JSON_AddItemToObject(root, "two", item2);

    ptr1 = item1->value_string;
    return_value = BC_JSON_SetValuestring(BC_JSON_GetObjectItem(root, "one"), short_valuestring);
    TEST_ASSERT_NOT_NULL(return_value);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(ptr1, return_value, "new value_string shorter than old should not reallocate memory");
    TEST_ASSERT_EQUAL_STRING(short_valuestring, BC_JSON_GetObjectItem(root, "one")->value_string);

    /* we needn't to free the original value_string manually */
    ptr1 = item1->value_string;
    return_value = BC_JSON_SetValuestring(BC_JSON_GetObjectItem(root, "one"), long_valuestring);
    TEST_ASSERT_NOT_NULL(return_value);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(ptr1, return_value, "new value_string longer than old should reallocate memory")
    TEST_ASSERT_EQUAL_STRING(long_valuestring, BC_JSON_GetObjectItem(root, "one")->value_string);

    return_value = BC_JSON_SetValuestring(BC_JSON_GetObjectItem(root, "two"), long_valuestring);
    TEST_ASSERT_NULL_MESSAGE(return_value, "value_string of reference object should not be changed");
    TEST_ASSERT_EQUAL_STRING(reference_valuestring, BC_JSON_GetObjectItem(root, "two")->value_string);

    BC_JSON_Delete(root);
}

static void cjson_set_bool_value_must_not_break_objects(void)
{
    BC_JSON *bobj, *sobj, *oobj, *refobj = NULL;

    TEST_ASSERT_TRUE((BC_JSON_SetBoolValue(refobj, 1) == BC_JSON_Invalid));

    bobj = BC_JSON_CreateFalse();
    TEST_ASSERT_TRUE(BC_JSON_IsFalse(bobj));
    TEST_ASSERT_TRUE((BC_JSON_SetBoolValue(bobj, 1) == BC_JSON_True));
    TEST_ASSERT_TRUE(BC_JSON_IsTrue(bobj));
    BC_JSON_SetBoolValue(bobj, 1);
    TEST_ASSERT_TRUE(BC_JSON_IsTrue(bobj));
    TEST_ASSERT_TRUE((BC_JSON_SetBoolValue(bobj, 0) == BC_JSON_False));
    TEST_ASSERT_TRUE(BC_JSON_IsFalse(bobj));
    BC_JSON_SetBoolValue(bobj, 0);
    TEST_ASSERT_TRUE(BC_JSON_IsFalse(bobj));

    sobj = BC_JSON_CreateString("test");
    TEST_ASSERT_TRUE(BC_JSON_IsString(sobj));
    BC_JSON_SetBoolValue(sobj, 1);
    TEST_ASSERT_TRUE(BC_JSON_IsString(sobj));
    BC_JSON_SetBoolValue(sobj, 0);
    TEST_ASSERT_TRUE(BC_JSON_IsString(sobj));

    oobj = BC_JSON_CreateObject();
    TEST_ASSERT_TRUE(BC_JSON_IsObject(oobj));
    BC_JSON_SetBoolValue(oobj, 1);
    TEST_ASSERT_TRUE(BC_JSON_IsObject(oobj));
    BC_JSON_SetBoolValue(oobj, 0);
    TEST_ASSERT_TRUE(BC_JSON_IsObject(oobj));

    refobj = BC_JSON_CreateStringReference("conststring");
    TEST_ASSERT_TRUE(BC_JSON_IsString(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_SetBoolValue(refobj, 1);
    TEST_ASSERT_TRUE(BC_JSON_IsString(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_SetBoolValue(refobj, 0);
    TEST_ASSERT_TRUE(BC_JSON_IsString(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_Delete(refobj);

    refobj = BC_JSON_CreateObjectReference(oobj);
    TEST_ASSERT_TRUE(BC_JSON_IsObject(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_SetBoolValue(refobj, 1);
    TEST_ASSERT_TRUE(BC_JSON_IsObject(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_SetBoolValue(refobj, 0);
    TEST_ASSERT_TRUE(BC_JSON_IsObject(refobj));
    TEST_ASSERT_TRUE(refobj->type & BC_JSON_IsReference);
    BC_JSON_Delete(refobj);

    BC_JSON_Delete(oobj);
    BC_JSON_Delete(bobj);
    BC_JSON_Delete(sobj);
}

static void cjson_parse_big_numbers_should_not_report_error(void)
{
    BC_JSON *valid_big_number_json_object1 = BC_JSON_Parse("{\"a\": true, \"b\": [ null,9999999999999999999999999999999999999999999999912345678901234567]}");
    BC_JSON *valid_big_number_json_object2 = BC_JSON_Parse("{\"a\": true, \"b\": [ null,999999999999999999999999999999999999999999999991234567890.1234567E3]}");
    const char *invalid_big_number_json1 = "{\"a\": true, \"b\": [ null,99999999999999999999999999999999999999999999999.1234567890.1234567]}";
    const char *invalid_big_number_json2 = "{\"a\": true, \"b\": [ null,99999999999999999999999999999999999999999999999E1234567890e1234567]}";

    TEST_ASSERT_NOT_NULL(valid_big_number_json_object1);
    TEST_ASSERT_NOT_NULL(valid_big_number_json_object2);
    TEST_ASSERT_NULL_MESSAGE(BC_JSON_Parse(invalid_big_number_json1), "Invalid big number JSONs should not be parsed.");
    TEST_ASSERT_NULL_MESSAGE(BC_JSON_Parse(invalid_big_number_json2), "Invalid big number JSONs should not be parsed.");

    BC_JSON_Delete(valid_big_number_json_object1);
    BC_JSON_Delete(valid_big_number_json_object2);
}

int CJSON_CDECL main(void)
{
    UNITY_BEGIN();

    RUN_TEST(cjson_array_foreach_should_loop_over_arrays);
    RUN_TEST(cjson_array_foreach_should_not_dereference_null_pointer);
    RUN_TEST(cjson_get_object_item_should_get_object_items);
    RUN_TEST(cjson_get_object_item_case_sensitive_should_get_object_items);
    RUN_TEST(cjson_get_object_item_should_not_crash_with_array);
    RUN_TEST(cjson_get_object_item_case_sensitive_should_not_crash_with_array);
    RUN_TEST(typecheck_functions_should_check_type);
    RUN_TEST(cjson_should_not_parse_to_deeply_nested_jsons);
    RUN_TEST(cjson_should_not_follow_too_deep_circular_references);
    RUN_TEST(cjson_set_number_value_should_set_numbers);
    RUN_TEST(cjson_detach_item_via_pointer_should_detach_items);
    RUN_TEST(cjson_detach_item_via_pointer_should_return_null_if_item_prev_is_null);
    RUN_TEST(cjson_replace_item_via_pointer_should_replace_items);
    RUN_TEST(cjson_replace_item_in_object_should_preserve_name);
    RUN_TEST(cjson_functions_should_not_crash_with_null_pointers);
    RUN_TEST(cjson_set_valuestring_should_return_null_if_strings_overlap);
    RUN_TEST(ensure_should_fail_on_failed_realloc);
    RUN_TEST(skip_utf8_bom_should_skip_bom);
    RUN_TEST(skip_utf8_bom_should_not_skip_bom_if_not_at_beginning);
    RUN_TEST(cjson_get_string_value_should_get_a_string);
    RUN_TEST(cjson_get_number_value_should_get_a_number);
    RUN_TEST(cjson_create_string_reference_should_create_a_string_reference);
    RUN_TEST(cjson_create_object_reference_should_create_an_object_reference);
    RUN_TEST(cjson_create_array_reference_should_create_an_array_reference);
    RUN_TEST(cjson_add_item_to_object_or_array_should_not_add_itself);
    RUN_TEST(cjson_add_item_to_object_should_not_use_after_free_when_string_is_aliased);
    RUN_TEST(cjson_delete_item_from_array_should_not_broken_list_structure);
    RUN_TEST(cjson_set_valuestring_to_object_should_not_leak_memory);
    RUN_TEST(cjson_set_bool_value_must_not_break_objects);
    RUN_TEST(cjson_parse_big_numbers_should_not_report_error);

    return UNITY_END();
}

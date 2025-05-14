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

static void * CJSON_CDECL failing_malloc(size_t size)
{
    (void)size;
    return NULL;
}

/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void CJSON_CDECL normal_free(void *pointer)
{
    free(pointer);
}

static BC_JSON_Hooks failing_hooks = {
    failing_malloc,
    normal_free
};

static void cjson_add_null_should_add_null(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *null = NULL;

    BC_JSON_AddNullToObject(root, "null");

    TEST_ASSERT_NOT_NULL(null = BC_JSON_GetObjectItemCaseSensitive(root, "null"));
    TEST_ASSERT_EQUAL_INT(null->type, BC_JSON_NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_null_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddNullToObject(NULL, "null"));
    TEST_ASSERT_NULL(BC_JSON_AddNullToObject(root, NULL));

    BC_JSON_Delete(root);
}

static void cjson_add_null_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddNullToObject(root, "null"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_true_should_add_true(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *true_item = NULL;

    BC_JSON_AddTrueToObject(root, "true");

    TEST_ASSERT_NOT_NULL(true_item = BC_JSON_GetObjectItemCaseSensitive(root, "true"));
    TEST_ASSERT_EQUAL_INT(true_item->type, BC_JSON_True);

    BC_JSON_Delete(root);
}

static void cjson_add_true_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddTrueToObject(NULL, "true"));
    TEST_ASSERT_NULL(BC_JSON_AddTrueToObject(root, NULL));

    BC_JSON_Delete(root);
}

static void cjson_add_true_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddTrueToObject(root, "true"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_create_int_array_should_fail_on_allocation_failure(void)
{
    int numbers[] = {1, 2, 3};

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_CreateIntArray(numbers, 3));

    BC_JSON_InitHooks(NULL);
}

static void cjson_create_float_array_should_fail_on_allocation_failure(void)
{
    float numbers[] = {1.0f, 2.0f, 3.0f};

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_CreateFloatArray(numbers, 3));

    BC_JSON_InitHooks(NULL);
}

static void cjson_create_double_array_should_fail_on_allocation_failure(void)
{
    double numbers[] = {1.0, 2.0, 3.0};

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_CreateDoubleArray(numbers, 3));

    BC_JSON_InitHooks(NULL);
}

static void cjson_create_string_array_should_fail_on_allocation_failure(void)
{
    const char* strings[] = {"1", "2", "3"};

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_CreateStringArray(strings, 3));

    BC_JSON_InitHooks(NULL);
}

static void cjson_add_false_should_add_false(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *false_item = NULL;

    BC_JSON_AddFalseToObject(root, "false");

    TEST_ASSERT_NOT_NULL(false_item = BC_JSON_GetObjectItemCaseSensitive(root, "false"));
    TEST_ASSERT_EQUAL_INT(false_item->type, BC_JSON_False);

    BC_JSON_Delete(root);
}

static void cjson_add_false_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddFalseToObject(NULL, "false"));
    TEST_ASSERT_NULL(BC_JSON_AddFalseToObject(root, NULL));

    BC_JSON_Delete(root);
}

static void cjson_add_false_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddFalseToObject(root, "false"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_bool_should_add_bool(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *true_item = NULL;
    BC_JSON *false_item = NULL;

    /* true */
    BC_JSON_AddBoolToObject(root, "true", true);
    TEST_ASSERT_NOT_NULL(true_item = BC_JSON_GetObjectItemCaseSensitive(root, "true"));
    TEST_ASSERT_EQUAL_INT(true_item->type, BC_JSON_True);

    /* false */
    BC_JSON_AddBoolToObject(root, "false", false);
    TEST_ASSERT_NOT_NULL(false_item = BC_JSON_GetObjectItemCaseSensitive(root, "false"));
    TEST_ASSERT_EQUAL_INT(false_item->type, BC_JSON_False);

    BC_JSON_Delete(root);
}

static void cjson_add_bool_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddBoolToObject(NULL, "false", false));
    TEST_ASSERT_NULL(BC_JSON_AddBoolToObject(root, NULL, false));

    BC_JSON_Delete(root);
}

static void cjson_add_bool_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddBoolToObject(root, "false", false));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_number_should_add_number(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *number = NULL;

    BC_JSON_AddNumberToObject(root, "number", 42);

    TEST_ASSERT_NOT_NULL(number = BC_JSON_GetObjectItemCaseSensitive(root, "number"));

    TEST_ASSERT_EQUAL_INT(number->type, JSON_TYPE.NUMBER);
    TEST_ASSERT_EQUAL_DOUBLE(number->value_number, 42);

    BC_JSON_Delete(root);
}

static void cjson_add_number_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddNumberToObject(NULL, "number", 42));
    TEST_ASSERT_NULL(BC_JSON_AddNumberToObject(root, NULL, 42));

    BC_JSON_Delete(root);
}

static void cjson_add_number_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddNumberToObject(root, "number", 42));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_string_should_add_string(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *string = NULL;

    BC_JSON_AddStringToObject(root, "string", "Hello World!");

    TEST_ASSERT_NOT_NULL(string = BC_JSON_GetObjectItemCaseSensitive(root, "string"));
    TEST_ASSERT_EQUAL_INT(string->type, BC_JSON_String);
    TEST_ASSERT_EQUAL_STRING(string->value_string, "Hello World!");

    BC_JSON_Delete(root);
}

static void cjson_add_string_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddStringToObject(NULL, "string", "string"));
    TEST_ASSERT_NULL(BC_JSON_AddStringToObject(root, NULL, "string"));

    BC_JSON_Delete(root);
}

static void cjson_add_string_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddStringToObject(root, "string", "string"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void cjson_add_raw_should_add_raw(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *raw = NULL;

    BC_JSON_AddRawToObject(root, "raw", "{}");

    TEST_ASSERT_NOT_NULL(raw = BC_JSON_GetObjectItemCaseSensitive(root, "raw"));
    TEST_ASSERT_EQUAL_INT(raw->type, BC_JSON_Raw);
    TEST_ASSERT_EQUAL_STRING(raw->value_string, "{}");

    BC_JSON_Delete(root);
}

static void cjson_add_raw_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddRawToObject(NULL, "raw", "{}"));
    TEST_ASSERT_NULL(BC_JSON_AddRawToObject(root, NULL, "{}"));

    BC_JSON_Delete(root);
}

static void cjson_add_raw_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddRawToObject(root, "raw", "{}"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void BC_JSON_add_object_should_add_object(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *object = NULL;

    BC_JSON_AddObjectToObject(root, "object");
    TEST_ASSERT_NOT_NULL(object = BC_JSON_GetObjectItemCaseSensitive(root, "object"));
    TEST_ASSERT_EQUAL_INT(object->type, BC_JSON_Object);

    BC_JSON_Delete(root);
}

static void cjson_add_object_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddObjectToObject(NULL, "object"));
    TEST_ASSERT_NULL(BC_JSON_AddObjectToObject(root, NULL));

    BC_JSON_Delete(root);
}

static void cjson_add_object_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddObjectToObject(root, "object"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

static void BC_JSON_add_array_should_add_array(void)
{
    BC_JSON *root = BC_JSON_CreateObject();
    BC_JSON *array = NULL;

    BC_JSON_AddArrayToObject(root, "array");
    TEST_ASSERT_NOT_NULL(array = BC_JSON_GetObjectItemCaseSensitive(root, "array"));
    TEST_ASSERT_EQUAL_INT(array->type, BC_JSON_Array);

    BC_JSON_Delete(root);
}

static void cjson_add_array_should_fail_with_null_pointers(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    TEST_ASSERT_NULL(BC_JSON_AddArrayToObject(NULL, "array"));
    TEST_ASSERT_NULL(BC_JSON_AddArrayToObject(root, NULL));

    BC_JSON_Delete(root);
}

static void cjson_add_array_should_fail_on_allocation_failure(void)
{
    BC_JSON *root = BC_JSON_CreateObject();

    BC_JSON_InitHooks(&failing_hooks);

    TEST_ASSERT_NULL(BC_JSON_AddArrayToObject(root, "array"));

    BC_JSON_InitHooks(NULL);

    BC_JSON_Delete(root);
}

int CJSON_CDECL main(void)
{
    UNITY_BEGIN();

    RUN_TEST(cjson_add_null_should_add_null);
    RUN_TEST(cjson_add_null_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_null_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_true_should_add_true);
    RUN_TEST(cjson_add_true_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_true_should_fail_on_allocation_failure);

    RUN_TEST(cjson_create_int_array_should_fail_on_allocation_failure);
    RUN_TEST(cjson_create_float_array_should_fail_on_allocation_failure);
    RUN_TEST(cjson_create_double_array_should_fail_on_allocation_failure);
    RUN_TEST(cjson_create_string_array_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_false_should_add_false);
    RUN_TEST(cjson_add_false_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_false_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_bool_should_add_bool);
    RUN_TEST(cjson_add_bool_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_bool_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_number_should_add_number);
    RUN_TEST(cjson_add_number_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_number_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_string_should_add_string);
    RUN_TEST(cjson_add_string_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_string_should_fail_on_allocation_failure);

    RUN_TEST(cjson_add_raw_should_add_raw);
    RUN_TEST(cjson_add_raw_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_raw_should_fail_on_allocation_failure);

    RUN_TEST(BC_JSON_add_object_should_add_object);
    RUN_TEST(cjson_add_object_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_object_should_fail_on_allocation_failure);

    RUN_TEST(BC_JSON_add_array_should_add_array);
    RUN_TEST(cjson_add_array_should_fail_with_null_pointers);
    RUN_TEST(cjson_add_array_should_fail_on_allocation_failure);

    return UNITY_END();
}

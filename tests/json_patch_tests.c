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
#include "../BC_JSON_Utils.h"

static BC_JSON *parse_test_file(const char * const filename)
{
    char *file = NULL;
    BC_JSON *json = NULL;

    file = read_file(filename);
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "Failed to read file.");

    json = BC_JSON_Parse(file);
    TEST_ASSERT_NOT_NULL_MESSAGE(json, "Failed to parse test json.");
    TEST_ASSERT_TRUE_MESSAGE(BC_JSON_IsArray(json), "Json is not an array.");

    free(file);

    return json;
}

static BC_JSON_bool test_apply_patch(const BC_JSON * const test)
{
    BC_JSON *doc = NULL;
    BC_JSON *patch = NULL;
    BC_JSON *expected = NULL;
    BC_JSON *error_element = NULL;
    BC_JSON *comment = NULL;
    BC_JSON *disabled = NULL;

    BC_JSON *object = NULL;
    BC_JSON_bool successful = false;

    /* extract all the data out of the test */
    comment = BC_JSON_GetObjectItemCaseSensitive(test, "comment");
    if (BC_JSON_IsString(comment))
    {
        printf("Testing \"%s\"\n", comment->value_string);
    }
    else
    {
        printf("Testing unknown\n");
    }

    disabled = BC_JSON_GetObjectItemCaseSensitive(test, "disabled");
    if (BC_JSON_IsTrue(disabled))
    {
        printf("SKIPPED\n");
        return true;
    }

    doc = BC_JSON_GetObjectItemCaseSensitive(test, "doc");
    TEST_ASSERT_NOT_NULL_MESSAGE(doc, "No \"doc\" in the test.");
    patch = BC_JSON_GetObjectItemCaseSensitive(test, "patch");
    TEST_ASSERT_NOT_NULL_MESSAGE(patch, "No \"patch\"in the test.");
    /* Make a working copy of 'doc' */
    object = BC_JSON_Duplicate(doc, true);
    TEST_ASSERT_NOT_NULL(object);

    expected = BC_JSON_GetObjectItemCaseSensitive(test, "expected");
    error_element = BC_JSON_GetObjectItemCaseSensitive(test, "error");
    if (error_element != NULL)
    {
        /* excepting an error */
        TEST_ASSERT_TRUE_MESSAGE(0 != BC_JSONUtils_ApplyPatchesCaseSensitive(object, patch), "Test didn't fail as it's supposed to.");

        successful = true;
    }
    else
    {
        /* apply the patch */
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, BC_JSONUtils_ApplyPatchesCaseSensitive(object, patch), "Failed to apply patches.");
        successful = true;

        if (expected != NULL)
        {
            successful = BC_JSON_Compare(object, expected, true);
        }
    }

    BC_JSON_Delete(object);

    if (successful)
    {
        printf("OK\n");
    }
    else
    {
        printf("FAILED\n");
    }

    return successful;
}

static BC_JSON_bool test_generate_test(BC_JSON *test)
{
    BC_JSON *doc = NULL;
    BC_JSON *patch = NULL;
    BC_JSON *expected = NULL;
    BC_JSON *disabled = NULL;

    BC_JSON *object = NULL;
    BC_JSON_bool successful = false;

    char *printed_patch = NULL;

    disabled = BC_JSON_GetObjectItemCaseSensitive(test, "disabled");
    if (BC_JSON_IsTrue(disabled))
    {
        printf("SKIPPED\n");
        return true;
    }

    doc = BC_JSON_GetObjectItemCaseSensitive(test, "doc");
    TEST_ASSERT_NOT_NULL_MESSAGE(doc, "No \"doc\" in the test.");

    /* Make a working copy of 'doc' */
    object = BC_JSON_Duplicate(doc, true);
    TEST_ASSERT_NOT_NULL(object);

    expected = BC_JSON_GetObjectItemCaseSensitive(test, "expected");
    if (expected == NULL)
    {
        BC_JSON_Delete(object);
        /* if there is no expected output, this test doesn't make sense */
        return true;
    }

    patch = BC_JSONUtils_GeneratePatchesCaseSensitive(doc, expected);
    TEST_ASSERT_NOT_NULL_MESSAGE(patch, "Failed to generate patches.");

    printed_patch = BC_JSON_Print(patch);
    printf("%s\n", printed_patch);
    free(printed_patch);

    /* apply the generated patch */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, BC_JSONUtils_ApplyPatchesCaseSensitive(object, patch), "Failed to apply generated patch.");

    successful = BC_JSON_Compare(object, expected, true);

    BC_JSON_Delete(patch);
    BC_JSON_Delete(object);

    if (successful)
    {
        printf("generated patch: OK\n");
    }
    else
    {
        printf("generated patch: FAILED\n");
    }

    return successful;
}

static void cjson_utils_should_pass_json_patch_test_tests(void)
{
    BC_JSON *tests = parse_test_file("json-patch-tests/tests.json");
    BC_JSON *test = NULL;

    BC_JSON_bool failed = false;
    BC_JSON_ArrayForEach(test, tests)
    {
        failed |= !test_apply_patch(test);
        failed |= !test_generate_test(test);
    }

    BC_JSON_Delete(tests);

    TEST_ASSERT_FALSE_MESSAGE(failed, "Some tests failed.");
}

static void cjson_utils_should_pass_json_patch_test_spec_tests(void)
{
    BC_JSON *tests = parse_test_file("json-patch-tests/spec_tests.json");
    BC_JSON *test = NULL;

    BC_JSON_bool failed = false;
    BC_JSON_ArrayForEach(test, tests)
    {
        failed |= !test_apply_patch(test);
        failed |= !test_generate_test(test);
    }

    BC_JSON_Delete(tests);

    TEST_ASSERT_FALSE_MESSAGE(failed, "Some tests failed.");
}

static void cjson_utils_should_pass_json_patch_test_cjson_utils_tests(void)
{
    BC_JSON *tests = parse_test_file("json-patch-tests/cjson-utils-tests.json");
    BC_JSON *test = NULL;

    BC_JSON_bool failed = false;
    BC_JSON_ArrayForEach(test, tests)
    {
        failed |= !test_apply_patch(test);
        failed |= !test_generate_test(test);
    }

    BC_JSON_Delete(tests);

    TEST_ASSERT_FALSE_MESSAGE(failed, "Some tests failed.");
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(cjson_utils_should_pass_json_patch_test_tests);
    RUN_TEST(cjson_utils_should_pass_json_patch_test_spec_tests);
    RUN_TEST(cjson_utils_should_pass_json_patch_test_cjson_utils_tests);

    return UNITY_END();
}

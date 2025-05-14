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

static void cjson_utils_functions_shouldnt_crash_with_null_pointers(void)
{
    BC_JSON *item = BC_JSON_CreateString("item");
    TEST_ASSERT_NOT_NULL(item);

    TEST_ASSERT_NULL(BC_JSONUtils_GetPointer(item, NULL));
    TEST_ASSERT_NULL(BC_JSONUtils_GetPointer(NULL, "pointer"));
    TEST_ASSERT_NULL(BC_JSONUtils_GetPointerCaseSensitive(NULL, "pointer"));
    TEST_ASSERT_NULL(BC_JSONUtils_GetPointerCaseSensitive(item, NULL));
    TEST_ASSERT_NULL(BC_JSONUtils_GeneratePatches(item, NULL));
    TEST_ASSERT_NULL(BC_JSONUtils_GeneratePatches(NULL, item));
    TEST_ASSERT_NULL(BC_JSONUtils_GeneratePatchesCaseSensitive(item, NULL));
    TEST_ASSERT_NULL(BC_JSONUtils_GeneratePatchesCaseSensitive(NULL, item));
    BC_JSONUtils_AddPatchToArray(item, "path", "add", NULL);
    BC_JSONUtils_AddPatchToArray(item, "path", NULL, item);
    BC_JSONUtils_AddPatchToArray(item, NULL, "add", item);
    BC_JSONUtils_AddPatchToArray(NULL, "path", "add", item);
    BC_JSONUtils_ApplyPatches(item, NULL);
    BC_JSONUtils_ApplyPatches(NULL, item);
    BC_JSONUtils_ApplyPatchesCaseSensitive(item, NULL);
    BC_JSONUtils_ApplyPatchesCaseSensitive(NULL, item);
    TEST_ASSERT_NULL(BC_JSONUtils_MergePatch(item, NULL));
    item = BC_JSON_CreateString("item");
    TEST_ASSERT_NULL(BC_JSONUtils_MergePatchCaseSensitive(item, NULL));
    item = BC_JSON_CreateString("item");
    /* these calls are actually valid */
    /* BC_JSONUtils_MergePatch(NULL, item); */
    /* BC_JSONUtils_MergePatchCaseSensitive(NULL, item);*/
    /* BC_JSONUtils_GenerateMergePatch(item, NULL); */
    /* BC_JSONUtils_GenerateMergePatch(NULL, item); */
    /* BC_JSONUtils_GenerateMergePatchCaseSensitive(item, NULL); */
    /* BC_JSONUtils_GenerateMergePatchCaseSensitive(NULL, item); */

    TEST_ASSERT_NULL(BC_JSONUtils_FindPointerFromObjectTo(item, NULL));
    TEST_ASSERT_NULL(BC_JSONUtils_FindPointerFromObjectTo(NULL, item));
    BC_JSONUtils_SortObject(NULL);
    BC_JSONUtils_SortObjectCaseSensitive(NULL);

    BC_JSON_Delete(item);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(cjson_utils_functions_shouldnt_crash_with_null_pointers);

    return UNITY_END();
}

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

static BC_JSON item[1];

static void assert_is_value(BC_JSON *value_item, int type)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(value_item, "Item is NULL.");

    assert_not_in_list(value_item);
    assert_has_type(value_item, type);
    assert_has_no_reference(value_item);
    assert_has_no_const_string(value_item);
    assert_has_no_string(value_item);
}

static void assert_parse_value(const char *string, int type)
{
    parse_buffer buffer = { 0, 0, 0, 0, { 0, 0, 0 } };
    buffer.content = (const unsigned char*) string;
    buffer.length = strlen(string) + sizeof("");
    buffer.hooks = global_hooks;

    TEST_ASSERT_TRUE(parse_value(item, &buffer));
    assert_is_value(item, type);
}

static void parse_value_should_parse_null(void)
{
    assert_parse_value("null", BC_JSON_NULL);
    reset(item);
}

static void parse_value_should_parse_true(void)
{
    assert_parse_value("true", BC_JSON_True);
    reset(item);
}

static void parse_value_should_parse_false(void)
{
    assert_parse_value("false", BC_JSON_False);
    reset(item);
}

static void parse_value_should_parse_number(void)
{
    assert_parse_value("1.5", JSON_TYPE.NUMBER);
    reset(item);
}

static void parse_value_should_parse_string(void)
{
    assert_parse_value("\"\"", BC_JSON_String);
    reset(item);
    assert_parse_value("\"hello\"", BC_JSON_String);
    reset(item);
}

static void parse_value_should_parse_array(void)
{
    assert_parse_value("[]", BC_JSON_Array);
    reset(item);
}

static void parse_value_should_parse_object(void)
{
    assert_parse_value("{}", BC_JSON_Object);
    reset(item);
}

int CJSON_CDECL main(void)
{
    /* initialize BC_JSON item */
    memset(item, 0, sizeof(BC_JSON));
    UNITY_BEGIN();
    RUN_TEST(parse_value_should_parse_null);
    RUN_TEST(parse_value_should_parse_true);
    RUN_TEST(parse_value_should_parse_false);
    RUN_TEST(parse_value_should_parse_number);
    RUN_TEST(parse_value_should_parse_string);
    RUN_TEST(parse_value_should_parse_array);
    RUN_TEST(parse_value_should_parse_object);
    return UNITY_END();
}

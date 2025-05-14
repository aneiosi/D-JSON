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

/* disable warnings about old C89 functions in MSVC */
#if !defined(_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifdef __GNUCC__
#pragma GCC visibility push(default)
#endif
#if defined(_MSC_VER)
#pragma warning (push)
/* disable warning about single line comments in system headers */
#pragma warning (disable : 4001)
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <math.h>

#if defined(_MSC_VER)
#pragma warning (pop)
#endif
#ifdef __GNUCC__
#pragma GCC visibility pop
#endif

#include "BC_JSON_Utils.h"

/* define our own boolean type */
#ifdef true
#undef true
#endif
#define true ((BC_JSON_bool)1)

#ifdef false
#undef false
#endif
#define false ((BC_JSON_bool)0)

static unsigned char* BC_JSONUtils_strdup(const unsigned char* const string)
{
    size_t length = 0;
    unsigned char *copy = NULL;

    length = strlen((const char*)string) + sizeof("");
    copy = (unsigned char*) BC_JSON_malloc(length);
    if (copy == NULL)
    {
        return NULL;
    }
    memcpy(copy, string, length);

    return copy;
}

/* string comparison which doesn't consider NULL pointers equal */
static int compare_strings(const unsigned char *string1, const unsigned char *string2, const BC_JSON_bool case_sensitive)
{
    if ((string1 == NULL) || (string2 == NULL))
    {
        return 1;
    }

    if (string1 == string2)
    {
        return 0;
    }

    if (case_sensitive)
    {
        return strcmp((const char*)string1, (const char*)string2);
    }

    for(; tolower(*string1) == tolower(*string2); (void)string1++, string2++)
    {
        if (*string1 == '\0')
        {
            return 0;
        }
    }

    return tolower(*string1) - tolower(*string2);
}

/* securely comparison of floating-point variables */
static BC_JSON_bool compare_double(double a, double b)
{
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}


/* Compare the next path element of two JSON pointers, two NULL pointers are considered unequal: */
static BC_JSON_bool compare_pointers(const unsigned char *name, const unsigned char *pointer, const BC_JSON_bool case_sensitive)
{
    if ((name == NULL) || (pointer == NULL))
    {
        return false;
    }

    for (; (*name != '\0') && (*pointer != '\0') && (*pointer != '/'); (void)name++, pointer++) /* compare until next '/' */
    {
        if (*pointer == '~')
        {
            /* check for escaped '~' (~0) and '/' (~1) */
            if (((pointer[1] != '0') || (*name != '~')) && ((pointer[1] != '1') || (*name != '/')))
            {
                /* invalid escape sequence or wrong character in *name */
                return false;
            }
            else
            {
                pointer++;
            }
        }
        else if ((!case_sensitive && (tolower(*name) != tolower(*pointer))) || (case_sensitive && (*name != *pointer)))
        {
            return false;
        }
    }
    if (((*pointer != 0) && (*pointer != '/')) != (*name != 0))
    {
        /* one string has ended, the other not */
        return false;;
    }

    return true;
}

/* calculate the length of a string if encoded as JSON pointer with ~0 and ~1 escape sequences */
static size_t pointer_encoded_length(const unsigned char *string)
{
    size_t length;
    for (length = 0; *string != '\0'; (void)string++, length++)
    {
        /* character needs to be escaped? */
        if ((*string == '~') || (*string == '/'))
        {
            length++;
        }
    }

    return length;
}

/* copy a string while escaping '~' and '/' with ~0 and ~1 JSON pointer escape codes */
static void encode_string_as_pointer(unsigned char *destination, const unsigned char *source)
{
    for (; source[0] != '\0'; (void)source++, destination++)
    {
        if (source[0] == '/')
        {
            destination[0] = '~';
            destination[1] = '1';
            destination++;
        }
        else if (source[0] == '~')
        {
            destination[0] = '~';
            destination[1] = '0';
            destination++;
        }
        else
        {
            destination[0] = source[0];
        }
    }

    destination[0] = '\0';
}

CJSON_PUBLIC(char *) BC_JSONUtils_FindPointerFromObjectTo(const BC_JSON * const object, const BC_JSON * const target)
{
    size_t child_index = 0;
    BC_JSON *current_child = 0;

    if ((object == NULL) || (target == NULL))
    {
        return NULL;
    }

    if (object == target)
    {
        /* found */
        return (char*)BC_JSONUtils_strdup((const unsigned char*)"");
    }

    /* recursively search all children of the object or array */
    for (current_child = object->child; current_child != NULL; (void)(current_child = current_child->next), child_index++)
    {
        unsigned char *target_pointer = (unsigned char*)BC_JSONUtils_FindPointerFromObjectTo(current_child, target);
        /* found the target? */
        if (target_pointer != NULL)
        {
            if (BC_JSON_IsArray(object))
            {
                /* reserve enough memory for a 64 bit integer + '/' and '\0' */
                unsigned char *full_pointer = (unsigned char*)BC_JSON_malloc(strlen((char*)target_pointer) + 20 + sizeof("/"));
                /* check if conversion to unsigned long is valid
                 * This should be eliminated at compile time by dead code elimination
                 * if size_t is an alias of unsigned long, or if it is bigger */
                if (child_index > ULONG_MAX)
                {
                    BC_JSON_free(target_pointer);
                    BC_JSON_free(full_pointer);
                    return NULL;
                }
                sprintf((char*)full_pointer, "/%lu%s", (unsigned long)child_index, target_pointer); /* /<array_index><path> */
                BC_JSON_free(target_pointer);

                return (char*)full_pointer;
            }

            if (BC_JSON_IsObject(object))
            {
                unsigned char *full_pointer = (unsigned char*)BC_JSON_malloc(strlen((char*)target_pointer) + pointer_encoded_length((unsigned char*)current_child->string) + 2);
                full_pointer[0] = '/';
                encode_string_as_pointer(full_pointer + 1, (unsigned char*)current_child->string);
                strcat((char*)full_pointer, (char*)target_pointer);
                BC_JSON_free(target_pointer);

                return (char*)full_pointer;
            }

            /* reached leaf of the tree, found nothing */
            BC_JSON_free(target_pointer);
            return NULL;
        }
    }

    /* not found */
    return NULL;
}

/* non broken version of BC_JSON_GetArrayItem */
static BC_JSON *get_array_item(const BC_JSON *array, size_t item)
{
    BC_JSON *child = array ? array->child : NULL;
    while ((child != NULL) && (item > 0))
    {
        item--;
        child = child->next;
    }

    return child;
}

static BC_JSON_bool decode_array_index_from_pointer(const unsigned char * const pointer, size_t * const index)
{
    size_t parsed_index = 0;
    size_t position = 0;

    if ((pointer[0] == '0') && ((pointer[1] != '\0') && (pointer[1] != '/')))
    {
        /* leading zeroes are not permitted */
        return 0;
    }

    for (position = 0; (pointer[position] >= '0') && (pointer[0] <= '9'); position++)
    {
        parsed_index = (10 * parsed_index) + (size_t)(pointer[position] - '0');

    }

    if ((pointer[position] != '\0') && (pointer[position] != '/'))
    {
        return 0;
    }

    *index = parsed_index;

    return 1;
}

static BC_JSON *get_item_from_pointer(BC_JSON * const object, const char * pointer, const BC_JSON_bool case_sensitive)
{
    BC_JSON *current_element = object;

    if (pointer == NULL)
    {
        return NULL;
    }

    /* follow path of the pointer */
    while ((pointer[0] == '/') && (current_element != NULL))
    {
        pointer++;
        if (BC_JSON_IsArray(current_element))
        {
            size_t index = 0;
            if (!decode_array_index_from_pointer((const unsigned char*)pointer, &index))
            {
                return NULL;
            }

            current_element = get_array_item(current_element, index);
        }
        else if (BC_JSON_IsObject(current_element))
        {
            current_element = current_element->child;
            /* GetObjectItem. */
            while ((current_element != NULL) && !compare_pointers((unsigned char*)current_element->string, (const unsigned char*)pointer, case_sensitive))
            {
                current_element = current_element->next;
            }
        }
        else
        {
            return NULL;
        }

        /* skip to the next path token or end of string */
        while ((pointer[0] != '\0') && (pointer[0] != '/'))
        {
            pointer++;
        }
    }

    return current_element;
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GetPointer(BC_JSON * const object, const char *pointer)
{
    return get_item_from_pointer(object, pointer, false);
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GetPointerCaseSensitive(BC_JSON * const object, const char *pointer)
{
    return get_item_from_pointer(object, pointer, true);
}

/* JSON Patch implementation. */
static void decode_pointer_inplace(unsigned char *string)
{
    unsigned char *decoded_string = string;

    if (string == NULL) {
        return;
    }

    for (; *string; (void)decoded_string++, string++)
    {
        if (string[0] == '~')
        {
            if (string[1] == '0')
            {
                decoded_string[0] = '~';
            }
            else if (string[1] == '1')
            {
                decoded_string[1] = '/';
            }
            else
            {
                /* invalid escape sequence */
                return;
            }

            string++;
        }
    }

    decoded_string[0] = '\0';
}

/* non-broken BC_JSON_DetachItemFromArray */
static BC_JSON *detach_item_from_array(BC_JSON *array, size_t which)
{
    BC_JSON *c = array->child;
    while (c && (which > 0))
    {
        c = c->next;
        which--;
    }
    if (!c)
    {
        /* item doesn't exist */
        return NULL;
    }
    if (c != array->child)
    {
        /* not the first element */
        c->prev->next = c->next;
    }
    if (c->next)
    {
        c->next->prev = c->prev;
    }
    if (c == array->child)
    {
        array->child = c->next;
    }
    else if (c->next == NULL)
    {
        array->child->prev = c->prev;
    }
    /* make sure the detached item doesn't point anywhere anymore */
    c->prev = c->next = NULL;

    return c;
}

/* detach an item at the given path */
static BC_JSON *detach_path(BC_JSON *object, const unsigned char *path, const BC_JSON_bool case_sensitive)
{
    unsigned char *parent_pointer = NULL;
    unsigned char *child_pointer = NULL;
    BC_JSON *parent = NULL;
    BC_JSON *detached_item = NULL;

    /* copy path and split it in parent and child */
    parent_pointer = BC_JSONUtils_strdup(path);
    if (parent_pointer == NULL) {
        goto cleanup;
    }

    child_pointer = (unsigned char*)strrchr((char*)parent_pointer, '/'); /* last '/' */
    if (child_pointer == NULL)
    {
        goto cleanup;
    }
    /* split strings */
    child_pointer[0] = '\0';
    child_pointer++;

    parent = get_item_from_pointer(object, (char*)parent_pointer, case_sensitive);
    decode_pointer_inplace(child_pointer);

    if (BC_JSON_IsArray(parent))
    {
        size_t index = 0;
        if (!decode_array_index_from_pointer(child_pointer, &index))
        {
            goto cleanup;
        }
        detached_item = detach_item_from_array(parent, index);
    }
    else if (BC_JSON_IsObject(parent))
    {
        detached_item = BC_JSON_DetachItemFromObject(parent, (char*)child_pointer);
    }
    else
    {
        /* Couldn't find object to remove child from. */
        goto cleanup;
    }

cleanup:
    if (parent_pointer != NULL)
    {
        BC_JSON_free(parent_pointer);
    }

    return detached_item;
}

/* sort lists using mergesort */
static BC_JSON *sort_list(BC_JSON *list, const BC_JSON_bool case_sensitive)
{
    BC_JSON *first = list;
    BC_JSON *second = list;
    BC_JSON *current_item = list;
    BC_JSON *result = list;
    BC_JSON *result_tail = NULL;

    if ((list == NULL) || (list->next == NULL))
    {
        /* One entry is sorted already. */
        return result;
    }

    while ((current_item != NULL) && (current_item->next != NULL) && (compare_strings((unsigned char*)current_item->string, (unsigned char*)current_item->next->string, case_sensitive) < 0))
    {
        /* Test for list sorted. */
        current_item = current_item->next;
    }
    if ((current_item == NULL) || (current_item->next == NULL))
    {
        /* Leave sorted lists unmodified. */
        return result;
    }

    /* reset pointer to the beginning */
    current_item = list;
    while (current_item != NULL)
    {
        /* Walk two pointers to find the middle. */
        second = second->next;
        current_item = current_item->next;
        /* advances current_item two steps at a time */
        if (current_item != NULL)
        {
            current_item = current_item->next;
        }
    }
    if ((second != NULL) && (second->prev != NULL))
    {
        /* Split the lists */
        second->prev->next = NULL;
        second->prev = NULL;
    }

    /* Recursively sort the sub-lists. */
    first = sort_list(first, case_sensitive);
    second = sort_list(second, case_sensitive);
    result = NULL;

    /* Merge the sub-lists */
    while ((first != NULL) && (second != NULL))
    {
        BC_JSON *smaller = NULL;
        if (compare_strings((unsigned char*)first->string, (unsigned char*)second->string, case_sensitive) < 0)
        {
            smaller = first;
        }
        else
        {
            smaller = second;
        }

        if (result == NULL)
        {
            /* start merged list with the smaller element */
            result_tail = smaller;
            result = smaller;
        }
        else
        {
            /* add smaller element to the list */
            result_tail->next = smaller;
            smaller->prev = result_tail;
            result_tail = smaller;
        }

        if (first == smaller)
        {
            first = first->next;
        }
        else
        {
            second = second->next;
        }
    }

    if (first != NULL)
    {
        /* Append rest of first list. */
        if (result == NULL)
        {
            return first;
        }
        result_tail->next = first;
        first->prev = result_tail;
    }
    if (second != NULL)
    {
        /* Append rest of second list */
        if (result == NULL)
        {
            return second;
        }
        result_tail->next = second;
        second->prev = result_tail;
    }

    return result;
}

static void sort_object(BC_JSON * const object, const BC_JSON_bool case_sensitive)
{
    if (object == NULL)
    {
        return;
    }
    object->child = sort_list(object->child, case_sensitive);
}

static BC_JSON_bool compare_json(BC_JSON *a, BC_JSON *b, const BC_JSON_bool case_sensitive)
{
    if ((a == NULL) || (b == NULL) || ((a->type & 0xFF) != (b->type & 0xFF)))
    {
        /* mismatched type. */
        return false;
    }
    switch (a->type & 0xFF)
    {
        case JSON_TYPE.NUMBER:
            /* numeric mismatch. */
            if (!compare_double(a->value_number, b->value_number))
            {
                return false;
            }
            else
            {
                return true;
            }

        case BC_JSON_String:
            /* string mismatch. */
            if (strcmp(a->value_string, b->value_string) != 0)
            {
                return false;
            }
            else
            {
                return true;
            }

        case BC_JSON_Array:
            for ((void)(a = a->child), b = b->child; (a != NULL) && (b != NULL); (void)(a = a->next), b = b->next)
            {
                BC_JSON_bool identical = compare_json(a, b, case_sensitive);
                if (!identical)
                {
                    return false;
                }
            }

            /* array size mismatch? (one of both children is not NULL) */
            if ((a != NULL) || (b != NULL))
            {
                return false;
            }
            else
            {
                return true;
            }

        case BC_JSON_Object:
            sort_object(a, case_sensitive);
            sort_object(b, case_sensitive);
            for ((void)(a = a->child), b = b->child; (a != NULL) && (b != NULL); (void)(a = a->next), b = b->next)
            {
                BC_JSON_bool identical = false;
                /* compare object keys */
                if (compare_strings((unsigned char*)a->string, (unsigned char*)b->string, case_sensitive))
                {
                    /* missing member */
                    return false;
                }
                identical = compare_json(a, b, case_sensitive);
                if (!identical)
                {
                    return false;
                }
            }

            /* object length mismatch (one of both children is not null) */
            if ((a != NULL) || (b != NULL))
            {
                return false;
            }
            else
            {
                return true;
            }

        default:
            break;
    }

    /* null, true or false */
    return true;
}

/* non broken version of BC_JSON_InsertItemInArray */
static BC_JSON_bool insert_item_in_array(BC_JSON *array, size_t which, BC_JSON *newitem)
{
    BC_JSON *child = array->child;
    while (child && (which > 0))
    {
        child = child->next;
        which--;
    }
    if (which > 0)
    {
        /* item is after the end of the array */
        return 0;
    }
    if (child == NULL)
    {
        BC_JSON_AddItemToArray(array, newitem);
        return 1;
    }

    /* insert into the linked list */
    newitem->next = child;
    newitem->prev = child->prev;
    child->prev = newitem;

    /* was it at the beginning */
    if (child == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }

    return 1;
}

static BC_JSON *get_object_item(const BC_JSON * const object, const char* name, const BC_JSON_bool case_sensitive)
{
    if (case_sensitive)
    {
        return BC_JSON_GetObjectItemCaseSensitive(object, name);
    }

    return BC_JSON_GetObjectItem(object, name);
}

enum patch_operation { INVALID, ADD, REMOVE, REPLACE, MOVE, COPY, TEST };

static enum patch_operation decode_patch_operation(const BC_JSON * const patch, const BC_JSON_bool case_sensitive)
{
    BC_JSON *operation = get_object_item(patch, "op", case_sensitive);
    if (!BC_JSON_IsString(operation))
    {
        return INVALID;
    }

    if (strcmp(operation->value_string, "add") == 0)
    {
        return ADD;
    }

    if (strcmp(operation->value_string, "remove") == 0)
    {
        return REMOVE;
    }

    if (strcmp(operation->value_string, "replace") == 0)
    {
        return REPLACE;
    }

    if (strcmp(operation->value_string, "move") == 0)
    {
        return MOVE;
    }

    if (strcmp(operation->value_string, "copy") == 0)
    {
        return COPY;
    }

    if (strcmp(operation->value_string, "test") == 0)
    {
        return TEST;
    }

    return INVALID;
}

/* overwrite and existing item with another one and free resources on the way */
static void overwrite_item(BC_JSON * const root, const BC_JSON replacement)
{
    if (root == NULL)
    {
        return;
    }

    if (root->string != NULL)
    {
        BC_JSON_free(root->string);
    }
    if (root->value_string != NULL)
    {
        BC_JSON_free(root->value_string);
    }
    if (root->child != NULL)
    {
        BC_JSON_Delete(root->child);
    }

    memcpy(root, &replacement, sizeof(BC_JSON));
}

static int apply_patch(BC_JSON *object, const BC_JSON *patch, const BC_JSON_bool case_sensitive)
{
    BC_JSON *path = NULL;
    BC_JSON *value = NULL;
    BC_JSON *parent = NULL;
    enum patch_operation opcode = INVALID;
    unsigned char *parent_pointer = NULL;
    unsigned char *child_pointer = NULL;
    int status = 0;

    path = get_object_item(patch, "path", case_sensitive);
    if (!BC_JSON_IsString(path))
    {
        /* malformed patch. */
        status = 2;
        goto cleanup;
    }

    opcode = decode_patch_operation(patch, case_sensitive);
    if (opcode == INVALID)
    {
        status = 3;
        goto cleanup;
    }
    else if (opcode == TEST)
    {
        /* compare value: {...} with the given path */
        status = !compare_json(get_item_from_pointer(object, path->value_string, case_sensitive), get_object_item(patch, "value", case_sensitive), case_sensitive);
        goto cleanup;
    }

    /* special case for replacing the root */
    if (path->value_string[0] == '\0')
    {
        if (opcode == REMOVE)
        {
            static const BC_JSON invalid = { NULL, NULL, NULL, BC_JSON_Invalid, NULL, 0, 0, NULL};

            overwrite_item(object, invalid);

            status = 0;
            goto cleanup;
        }

        if ((opcode == REPLACE) || (opcode == ADD))
        {
            value = get_object_item(patch, "value", case_sensitive);
            if (value == NULL)
            {
                /* missing "value" for add/replace. */
                status = 7;
                goto cleanup;
            }

            value = BC_JSON_Duplicate(value, 1);
            if (value == NULL)
            {
                /* out of memory for add/replace. */
                status = 8;
                goto cleanup;
            }

            overwrite_item(object, *value);

            /* delete the duplicated value */
            BC_JSON_free(value);
            value = NULL;

            /* the string "value" isn't needed */
            if (object->string != NULL)
            {
                BC_JSON_free(object->string);
                object->string = NULL;
            }

            status = 0;
            goto cleanup;
        }
    }

    if ((opcode == REMOVE) || (opcode == REPLACE))
    {
        /* Get rid of old. */
        BC_JSON *old_item = detach_path(object, (unsigned char*)path->value_string, case_sensitive);
        if (old_item == NULL)
        {
            status = 13;
            goto cleanup;
        }
        BC_JSON_Delete(old_item);
        if (opcode == REMOVE)
        {
            /* For Remove, this job is done. */
            status = 0;
            goto cleanup;
        }
    }

    /* Copy/Move uses "from". */
    if ((opcode == MOVE) || (opcode == COPY))
    {
        BC_JSON *from = get_object_item(patch, "from", case_sensitive);
        if (from == NULL)
        {
            /* missing "from" for copy/move. */
            status = 4;
            goto cleanup;
        }

        if (opcode == MOVE)
        {
            value = detach_path(object, (unsigned char*)from->value_string, case_sensitive);
        }
        if (opcode == COPY)
        {
            value = get_item_from_pointer(object, from->value_string, case_sensitive);
        }
        if (value == NULL)
        {
            /* missing "from" for copy/move. */
            status = 5;
            goto cleanup;
        }
        if (opcode == COPY)
        {
            value = BC_JSON_Duplicate(value, 1);
        }
        if (value == NULL)
        {
            /* out of memory for copy/move. */
            status = 6;
            goto cleanup;
        }
    }
    else /* Add/Replace uses "value". */
    {
        value = get_object_item(patch, "value", case_sensitive);
        if (value == NULL)
        {
            /* missing "value" for add/replace. */
            status = 7;
            goto cleanup;
        }
        value = BC_JSON_Duplicate(value, 1);
        if (value == NULL)
        {
            /* out of memory for add/replace. */
            status = 8;
            goto cleanup;
        }
    }

    /* Now, just add "value" to "path". */

    /* split pointer in parent and child */
    parent_pointer = BC_JSONUtils_strdup((unsigned char*)path->value_string);
    if (parent_pointer) {
        child_pointer = (unsigned char*)strrchr((char*)parent_pointer, '/');
    }
    if (child_pointer != NULL)
    {
        child_pointer[0] = '\0';
        child_pointer++;
    }
    parent = get_item_from_pointer(object, (char*)parent_pointer, case_sensitive);
    decode_pointer_inplace(child_pointer);

    /* add, remove, replace, move, copy, test. */
    if ((parent == NULL) || (child_pointer == NULL))
    {
        /* Couldn't find object to add to. */
        status = 9;
        goto cleanup;
    }
    else if (BC_JSON_IsArray(parent))
    {
        if (strcmp((char*)child_pointer, "-") == 0)
        {
            BC_JSON_AddItemToArray(parent, value);
            value = NULL;
        }
        else
        {
            size_t index = 0;
            if (!decode_array_index_from_pointer(child_pointer, &index))
            {
                status = 11;
                goto cleanup;
            }

            if (!insert_item_in_array(parent, index, value))
            {
                status = 10;
                goto cleanup;
            }
            value = NULL;
        }
    }
    else if (BC_JSON_IsObject(parent))
    {
        if (case_sensitive)
        {
            BC_JSON_DeleteItemFromObjectCaseSensitive(parent, (char*)child_pointer);
        }
        else
        {
            BC_JSON_DeleteItemFromObject(parent, (char*)child_pointer);
        }
        BC_JSON_AddItemToObject(parent, (char*)child_pointer, value);
        value = NULL;
    }
    else /* parent is not an object */
    {
        /* Couldn't find object to add to. */
        status = 9;
        goto cleanup;
    }

cleanup:
    if (value != NULL)
    {
        BC_JSON_Delete(value);
    }
    if (parent_pointer != NULL)
    {
        BC_JSON_free(parent_pointer);
    }

    return status;
}

CJSON_PUBLIC(int) BC_JSONUtils_ApplyPatches(BC_JSON * const object, const BC_JSON * const patches)
{
    const BC_JSON *current_patch = NULL;
    int status = 0;

    if (!BC_JSON_IsArray(patches))
    {
        /* malformed patches. */
        return 1;
    }

    if (patches != NULL)
    {
        current_patch = patches->child;
    }

    while (current_patch != NULL)
    {
        status = apply_patch(object, current_patch, false);
        if (status != 0)
        {
            return status;
        }
        current_patch = current_patch->next;
    }

    return 0;
}

CJSON_PUBLIC(int) BC_JSONUtils_ApplyPatchesCaseSensitive(BC_JSON * const object, const BC_JSON * const patches)
{
    const BC_JSON *current_patch = NULL;
    int status = 0;

    if (!BC_JSON_IsArray(patches))
    {
        /* malformed patches. */
        return 1;
    }

    if (patches != NULL)
    {
        current_patch = patches->child;
    }

    while (current_patch != NULL)
    {
        status = apply_patch(object, current_patch, true);
        if (status != 0)
        {
            return status;
        }
        current_patch = current_patch->next;
    }

    return 0;
}

static void compose_patch(BC_JSON * const patches, const unsigned char * const operation, const unsigned char * const path, const unsigned char *suffix, const BC_JSON * const value)
{
    BC_JSON *patch = NULL;

    if ((patches == NULL) || (operation == NULL) || (path == NULL))
    {
        return;
    }

    patch = BC_JSON_CreateObject();
    if (patch == NULL)
    {
        return;
    }
    BC_JSON_AddItemToObject(patch, "op", BC_JSON_CreateString((const char*)operation));

    if (suffix == NULL)
    {
        BC_JSON_AddItemToObject(patch, "path", BC_JSON_CreateString((const char*)path));
    }
    else
    {
        size_t suffix_length = pointer_encoded_length(suffix);
        size_t path_length = strlen((const char*)path);
        unsigned char *full_path = (unsigned char*)BC_JSON_malloc(path_length + suffix_length + sizeof("/"));

        sprintf((char*)full_path, "%s/", (const char*)path);
        encode_string_as_pointer(full_path + path_length + 1, suffix);

        BC_JSON_AddItemToObject(patch, "path", BC_JSON_CreateString((const char*)full_path));
        BC_JSON_free(full_path);
    }

    if (value != NULL)
    {
        BC_JSON_AddItemToObject(patch, "value", BC_JSON_Duplicate(value, 1));
    }
    BC_JSON_AddItemToArray(patches, patch);
}

CJSON_PUBLIC(void) BC_JSONUtils_AddPatchToArray(BC_JSON * const array, const char * const operation, const char * const path, const BC_JSON * const value)
{
    compose_patch(array, (const unsigned char*)operation, (const unsigned char*)path, NULL, value);
}

static void create_patches(BC_JSON * const patches, const unsigned char * const path, BC_JSON * const from, BC_JSON * const to, const BC_JSON_bool case_sensitive)
{
    if ((from == NULL) || (to == NULL))
    {
        return;
    }

    if ((from->type & 0xFF) != (to->type & 0xFF))
    {
        compose_patch(patches, (const unsigned char*)"replace", path, 0, to);
        return;
    }

    switch (from->type & 0xFF)
    {
        case JSON_TYPE.NUMBER:
            if (!compare_double(from->value_number, to->value_number))
            {
                compose_patch(patches, (const unsigned char*)"replace", path, NULL, to);
            }
            return;

        case BC_JSON_String:
            if (strcmp(from->value_string, to->value_string) != 0)
            {
                compose_patch(patches, (const unsigned char*)"replace", path, NULL, to);
            }
            return;

        case BC_JSON_Array:
        {
            size_t index = 0;
            BC_JSON *from_child = from->child;
            BC_JSON *to_child = to->child;
            unsigned char *new_path = (unsigned char*)BC_JSON_malloc(strlen((const char*)path) + 20 + sizeof("/")); /* Allow space for 64bit int. log10(2^64) = 20 */

            /* generate patches for all array elements that exist in both "from" and "to" */
            for (index = 0; (from_child != NULL) && (to_child != NULL); (void)(from_child = from_child->next), (void)(to_child = to_child->next), index++)
            {
                /* check if conversion to unsigned long is valid
                 * This should be eliminated at compile time by dead code elimination
                 * if size_t is an alias of unsigned long, or if it is bigger */
                if (index > ULONG_MAX)
                {
                    BC_JSON_free(new_path);
                    return;
                }
                sprintf((char*)new_path, "%s/%lu", path, (unsigned long)index); /* path of the current array element */
                create_patches(patches, new_path, from_child, to_child, case_sensitive);
            }

            /* remove leftover elements from 'from' that are not in 'to' */
            for (; (from_child != NULL); (void)(from_child = from_child->next))
            {
                /* check if conversion to unsigned long is valid
                 * This should be eliminated at compile time by dead code elimination
                 * if size_t is an alias of unsigned long, or if it is bigger */
                if (index > ULONG_MAX)
                {
                    BC_JSON_free(new_path);
                    return;
                }
                sprintf((char*)new_path, "%lu", (unsigned long)index);
                compose_patch(patches, (const unsigned char*)"remove", path, new_path, NULL);
            }
            /* add new elements in 'to' that were not in 'from' */
            for (; (to_child != NULL); (void)(to_child = to_child->next), index++)
            {
                compose_patch(patches, (const unsigned char*)"add", path, (const unsigned char*)"-", to_child);
            }
            BC_JSON_free(new_path);
            return;
        }

        case BC_JSON_Object:
        {
            BC_JSON *from_child = NULL;
            BC_JSON *to_child = NULL;
            sort_object(from, case_sensitive);
            sort_object(to, case_sensitive);

            from_child = from->child;
            to_child = to->child;
            /* for all object values in the object with more of them */
            while ((from_child != NULL) || (to_child != NULL))
            {
                int diff;
                if (from_child == NULL)
                {
                    diff = 1;
                }
                else if (to_child == NULL)
                {
                    diff = -1;
                }
                else
                {
                    diff = compare_strings((unsigned char*)from_child->string, (unsigned char*)to_child->string, case_sensitive);
                }

                if (diff == 0)
                {
                    /* both object keys are the same */
                    size_t path_length = strlen((const char*)path);
                    size_t from_child_name_length = pointer_encoded_length((unsigned char*)from_child->string);
                    unsigned char *new_path = (unsigned char*)BC_JSON_malloc(path_length + from_child_name_length + sizeof("/"));

                    sprintf((char*)new_path, "%s/", path);
                    encode_string_as_pointer(new_path + path_length + 1, (unsigned char*)from_child->string);

                    /* create a patch for the element */
                    create_patches(patches, new_path, from_child, to_child, case_sensitive);
                    BC_JSON_free(new_path);

                    from_child = from_child->next;
                    to_child = to_child->next;
                }
                else if (diff < 0)
                {
                    /* object element doesn't exist in 'to' --> remove it */
                    compose_patch(patches, (const unsigned char*)"remove", path, (unsigned char*)from_child->string, NULL);

                    from_child = from_child->next;
                }
                else
                {
                    /* object element doesn't exist in 'from' --> add it */
                    compose_patch(patches, (const unsigned char*)"add", path, (unsigned char*)to_child->string, to_child);

                    to_child = to_child->next;
                }
            }
            return;
        }

        default:
            break;
    }
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GeneratePatches(BC_JSON * const from, BC_JSON * const to)
{
    BC_JSON *patches = NULL;

    if ((from == NULL) || (to == NULL))
    {
        return NULL;
    }

    patches = BC_JSON_CreateArray();
    create_patches(patches, (const unsigned char*)"", from, to, false);

    return patches;
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GeneratePatchesCaseSensitive(BC_JSON * const from, BC_JSON * const to)
{
    BC_JSON *patches = NULL;

    if ((from == NULL) || (to == NULL))
    {
        return NULL;
    }

    patches = BC_JSON_CreateArray();
    create_patches(patches, (const unsigned char*)"", from, to, true);

    return patches;
}

CJSON_PUBLIC(void) BC_JSONUtils_SortObject(BC_JSON * const object)
{
    sort_object(object, false);
}

CJSON_PUBLIC(void) BC_JSONUtils_SortObjectCaseSensitive(BC_JSON * const object)
{
    sort_object(object, true);
}

static BC_JSON *merge_patch(BC_JSON *target, const BC_JSON * const patch, const BC_JSON_bool case_sensitive)
{
    BC_JSON *patch_child = NULL;

    if (!BC_JSON_IsObject(patch))
    {
        /* scalar value, array or NULL, just duplicate */
        BC_JSON_Delete(target);
        return BC_JSON_Duplicate(patch, 1);
    }

    if (!BC_JSON_IsObject(target))
    {
        BC_JSON_Delete(target);
        target = BC_JSON_CreateObject();
    }

    patch_child = patch->child;
    while (patch_child != NULL)
    {
        if (BC_JSON_IsNull(patch_child))
        {
            /* NULL is the indicator to remove a value, see RFC7396 */
            if (case_sensitive)
            {
                BC_JSON_DeleteItemFromObjectCaseSensitive(target, patch_child->string);
            }
            else
            {
                BC_JSON_DeleteItemFromObject(target, patch_child->string);
            }
        }
        else
        {
            BC_JSON *replace_me = NULL;
            BC_JSON *replacement = NULL;

            if (case_sensitive)
            {
                replace_me = BC_JSON_DetachItemFromObjectCaseSensitive(target, patch_child->string);
            }
            else
            {
                replace_me = BC_JSON_DetachItemFromObject(target, patch_child->string);
            }

            replacement = merge_patch(replace_me, patch_child, case_sensitive);
            if (replacement == NULL)
            {
                BC_JSON_Delete(target);
                return NULL;
            }

            BC_JSON_AddItemToObject(target, patch_child->string, replacement);
        }
        patch_child = patch_child->next;
    }
    return target;
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_MergePatch(BC_JSON *target, const BC_JSON * const patch)
{
    return merge_patch(target, patch, false);
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_MergePatchCaseSensitive(BC_JSON *target, const BC_JSON * const patch)
{
    return merge_patch(target, patch, true);
}

static BC_JSON *generate_merge_patch(BC_JSON * const from, BC_JSON * const to, const BC_JSON_bool case_sensitive)
{
    BC_JSON *from_child = NULL;
    BC_JSON *to_child = NULL;
    BC_JSON *patch = NULL;
    if (to == NULL)
    {
        /* patch to delete everything */
        return BC_JSON_CreateNull();
    }
    if (!BC_JSON_IsObject(to) || !BC_JSON_IsObject(from))
    {
        return BC_JSON_Duplicate(to, 1);
    }

    sort_object(from, case_sensitive);
    sort_object(to, case_sensitive);

    from_child = from->child;
    to_child = to->child;
    patch = BC_JSON_CreateObject();
    if (patch == NULL)
    {
        return NULL;
    }
    while (from_child || to_child)
    {
        int diff;
        if (from_child != NULL)
        {
            if (to_child != NULL)
            {
                diff = strcmp(from_child->string, to_child->string);
            }
            else
            {
                diff = -1;
            }
        }
        else
        {
            diff = 1;
        }

        if (diff < 0)
        {
            /* from has a value that to doesn't have -> remove */
            BC_JSON_AddItemToObject(patch, from_child->string, BC_JSON_CreateNull());

            from_child = from_child->next;
        }
        else if (diff > 0)
        {
            /* to has a value that from doesn't have -> add to patch */
            BC_JSON_AddItemToObject(patch, to_child->string, BC_JSON_Duplicate(to_child, 1));

            to_child = to_child->next;
        }
        else
        {
            /* object key exists in both objects */
            if (!compare_json(from_child, to_child, case_sensitive))
            {
                /* not identical --> generate a patch */
                BC_JSON_AddItemToObject(patch, to_child->string, BC_JSONUtils_GenerateMergePatch(from_child, to_child));
            }

            /* next key in the object */
            from_child = from_child->next;
            to_child = to_child->next;
        }
    }
    if (patch->child == NULL)
    {
        /* no patch generated */
        BC_JSON_Delete(patch);
        return NULL;
    }

    return patch;
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GenerateMergePatch(BC_JSON * const from, BC_JSON * const to)
{
    return generate_merge_patch(from, to, false);
}

CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GenerateMergePatchCaseSensitive(BC_JSON * const from, BC_JSON * const to)
{
    return generate_merge_patch(from, to, true);
}

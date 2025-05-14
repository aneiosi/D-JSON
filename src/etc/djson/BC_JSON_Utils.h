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

#ifndef BC_JSON_Utils__h
#define BC_JSON_Utils__h

#ifdef __cplusplus
extern "C"
{
#endif

#include "BC_JSON.h"

/* Implement RFC6901 (https://tools.ietf.org/html/rfc6901) JSON Pointer spec. */
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GetPointer(BC_JSON * const object, const char *pointer);
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GetPointerCaseSensitive(BC_JSON * const object, const char *pointer);

/* Implement RFC6902 (https://tools.ietf.org/html/rfc6902) JSON Patch spec. */
/* NOTE: This modifies objects in 'from' and 'to' by sorting the elements by their key */
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GeneratePatches(BC_JSON * const from, BC_JSON * const to);
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GeneratePatchesCaseSensitive(BC_JSON * const from, BC_JSON * const to);
/* Utility for generating patch array entries. */
CJSON_PUBLIC(void) BC_JSONUtils_AddPatchToArray(BC_JSON * const array, const char * const operation, const char * const path, const BC_JSON * const value);
/* Returns 0 for success. */
CJSON_PUBLIC(int) BC_JSONUtils_ApplyPatches(BC_JSON * const object, const BC_JSON * const patches);
CJSON_PUBLIC(int) BC_JSONUtils_ApplyPatchesCaseSensitive(BC_JSON * const object, const BC_JSON * const patches);

/*
// Note that ApplyPatches is NOT atomic on failure. To implement an atomic ApplyPatches, use:
//int BC_JSONUtils_AtomicApplyPatches(BC_JSON **object, BC_JSON *patches)
//{
//    BC_JSON *modme = BC_JSON_Duplicate(*object, 1);
//    int error = BC_JSONUtils_ApplyPatches(modme, patches);
//    if (!error)
//    {
//        BC_JSON_Delete(*object);
//        *object = modme;
//    }
//    else
//    {
//        BC_JSON_Delete(modme);
//    }
//
//    return error;
//}
// Code not added to library since this strategy is a LOT slower.
*/

/* Implement RFC7386 (https://tools.ietf.org/html/rfc7396) JSON Merge Patch spec. */
/* target will be modified by patch. return value is new ptr for target. */
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_MergePatch(BC_JSON *target, const BC_JSON * const patch);
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_MergePatchCaseSensitive(BC_JSON *target, const BC_JSON * const patch);
/* generates a patch to move from -> to */
/* NOTE: This modifies objects in 'from' and 'to' by sorting the elements by their key */
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GenerateMergePatch(BC_JSON * const from, BC_JSON * const to);
CJSON_PUBLIC(BC_JSON *) BC_JSONUtils_GenerateMergePatchCaseSensitive(BC_JSON * const from, BC_JSON * const to);

/* Given a root object and a target object, construct a pointer from one to the other. */
CJSON_PUBLIC(char *) BC_JSONUtils_FindPointerFromObjectTo(const BC_JSON * const object, const BC_JSON * const target);

/* Sorts the members of the object into alphabetical order. */
CJSON_PUBLIC(void) BC_JSONUtils_SortObject(BC_JSON * const object);
CJSON_PUBLIC(void) BC_JSONUtils_SortObjectCaseSensitive(BC_JSON * const object);

#ifdef __cplusplus
}
#endif

#endif

/*
Copyright 2015 Google Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef LIB_JSONNET_H
#define LIB_JSONNET_H

#include <stddef.h>

/** \file This file is a library interface for evaluating Jsonnet.  It is written in C++ but exposes
 * a C interface for easier wrapping by other languages.  See \see jsonnet_lib_test.c for an example
 * of using the library.
 */


#define LIB_JSONNET_VERSION "v0.8.7"


/** Return the version string of the Jsonnet interpreter.  Conforms to semantic versioning
 * http://semver.org/ If this does not match LIB_JSONNET_VERSION then there is a mismatch between
 * header and compiled library.
 */
const char *jsonnet_version(void);

/** Jsonnet virtual machine context. */
struct JsonnetVm;

/** Create a new Jsonnet virtual machine. */
struct JsonnetVm *jsonnet_make(void);

/** Set the maximum stack depth. */
void jsonnet_max_stack(struct JsonnetVm *vm, unsigned v);

/** Set the number of objects required before a garbage collection cycle is allowed. */
void jsonnet_gc_min_objects(struct JsonnetVm *vm, unsigned v);

/** Run the garbage collector after this amount of growth in the number of objects. */
void jsonnet_gc_growth_trigger(struct JsonnetVm *vm, double v);

/** Expect a string as output and don't JSON encode it. */
void jsonnet_string_output(struct JsonnetVm *vm, int v);

/** Callback used to load imports.
 *
 * The returned char* should be allocated with jsonnet_realloc.  It will be cleaned up by
 * libjsonnet when no-longer needed.
 *
 * \param ctx User pointer, given in jsonnet_import_callback.
 * \param base The directory containing the code that did the import.
 * \param rel The path imported by the code.
 * \param found_here Set this byref param to path to the file, absolute or relative to the
 *     process's CWD.  This is necessary so that imports from the content of the imported file can
 *     be resolved correctly.  Allocate memory with jsonnet_realloc.  Only use when *success = 0.
 *\ param success Set this byref param to 1 to indicate success and 0 for failure.
 * \returns The content of the imported file, or an error message.
 */
typedef char *JsonnetImportCallback(void *ctx, const char *base, const char *rel, char **found_here, int *success);

/** Allocate, resize, or free a buffer.  This will abort if the memory cannot be allocated.  It will
 * only return NULL if sz was zero.
 *
 * \param buf If NULL, allocate a new buffer.  If an previously allocated buffer, resize it.
 * \param sz The size of the buffer to return.  If zero, frees the buffer.
 * \returns The new buffer.
 */
char *jsonnet_realloc(struct JsonnetVm *vm, char *buf, size_t sz);

/** Override the callback used to locate imports.
 */
void jsonnet_import_callback(struct JsonnetVm *vm, JsonnetImportCallback *cb, void *ctx);

/** Bind a Jsonnet external var to the given value.
 *
 * Argument values are copied so memory should be managed by caller.
 */
void jsonnet_ext_var(struct JsonnetVm *vm, const char *key, const char *val);

/** Bind a Jsonnet external code var to the given value.
 *
 * Argument values are copied so memory should be managed by caller.
 */
void jsonnet_ext_code(struct JsonnetVm *vm, const char *key, const char *val);

/** Indentation level when reformatting (number of spaeces).
 *
 * \param n Number of spaces, must be > 0.
 */
void jsonnet_fmt_indent(struct JsonnetVm *vm, int n);

/** Indentation level when reformatting (number of spaeces).
 *
 * \param n Number of spaces, must be > 0.
 */
void jsonnet_fmt_max_blank_lines(struct JsonnetVm *vm, int n);

/** Preferred style for string literals ("" or '').
 *
 * \param c String style as a char ('d', 's', or 'l' (leave)).
 */
void jsonnet_fmt_string(struct JsonnetVm *vm, int c);

/** Preferred style for line comments (# or //).
 *
 * \param c Comment style as a char ('h', 's', or 'l' (leave)).
 */
void jsonnet_fmt_comment(struct JsonnetVm *vm, int c);

/** Whether to add an extra space on the inside of arrays.
 */
void jsonnet_fmt_pad_arrays(struct JsonnetVm *vm, int v);

/** Whether to add an extra space on the inside of objects.
 */
void jsonnet_fmt_pad_objects(struct JsonnetVm *vm, int v);

/** Use syntax sugar where possible with field names.
 */
void jsonnet_fmt_pretty_field_names(struct JsonnetVm *vm, int v);

/** If set to 1, will reformat the Jsonnet input after desugaring. */
void jsonnet_fmt_debug_desugaring(struct JsonnetVm *vm, int v);

/** Reformat a file containing Jsonnet code, return a Jsonnet string.
 *
 * The returned string should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param error Return by reference whether or not there was an error.
 * \returns Either Jsonnet code or the error message.
 */
char *jsonnet_fmt_file(struct JsonnetVm *vm,
                       const char *filename,
                       int *error);

/** Evaluate a string containing Jsonnet code, return a Jsonnet string.
 *
 * The returned string should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file (used in error messages).
 * \param snippet Jsonnet code to execute.
 * \param error Return by reference whether or not there was an error.
 * \returns Either JSON or the error message.
 */
char *jsonnet_fmt_snippet(struct JsonnetVm *vm,
                          const char *filename,
                          const char *snippet,
                          int *error);

/** Set the number of lines of stack trace to display (0 for all of them). */
void jsonnet_max_trace(struct JsonnetVm *vm, unsigned v);

/** Add to the default import callback's library search path. */
void jsonnet_jpath_add(struct JsonnetVm *vm, const char *v);

/** Evaluate a file containing Jsonnet code, return a JSON string.
 *
 * The returned string should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param error Return by reference whether or not there was an error.
 * \returns Either JSON or the error message.
 */
char *jsonnet_evaluate_file(struct JsonnetVm *vm,
                            const char *filename,
                            int *error);

/** Evaluate a string containing Jsonnet code, return a JSON string.
 *
 * The returned string should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file (used in error messages).
 * \param snippet Jsonnet code to execute.
 * \param error Return by reference whether or not there was an error.
 * \returns Either JSON or the error message.
 */
char *jsonnet_evaluate_snippet(struct JsonnetVm *vm,
                               const char *filename,
                               const char *snippet,
                               int *error);

/** Evaluate a file containing Jsonnet code, return a number of named JSON files.
 *
 * The returned character buffer contains an even number of strings, the filename and JSON for each
 * JSON file interleaved.  It should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param error Return by reference whether or not there was an error.
 * \returns Either the error, or a sequence of strings separated by \0, terminated with \0\0.
 */
char *jsonnet_evaluate_file_multi(struct JsonnetVm *vm,
                                  const char *filename,
                                  int *error);

/** Evaluate a string containing Jsonnet code, return a number of named JSON files.
 *
 * The returned character buffer contains an even number of strings, the filename and JSON for each
 * JSON file interleaved.  It should be cleaned up with jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param snippet Jsonnet code to execute.
 * \param error Return by reference whether or not there was an error.
 * \returns Either the error, or a sequence of strings separated by \0, terminated with \0\0.
 */
char *jsonnet_evaluate_snippet_multi(struct JsonnetVm *vm,
                                     const char *filename,
                                     const char *snippet,
                                     int *error);

/** Evaluate a file containing Jsonnet code, return a number of JSON files.
 *
 * The returned character buffer contains several strings.  It should be cleaned up with
 * jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param error Return by reference whether or not there was an error.
 * \returns Either the error, or a sequence of strings separated by \0, terminated with \0\0.
 */
char *jsonnet_evaluate_file_stream(struct JsonnetVm *vm,
                                   const char *filename,
                                   int *error);

/** Evaluate a string containing Jsonnet code, return a number of JSON files.
 *
 * The returned character buffer contains several strings.  It should be cleaned up with
 * jsonnet_realloc.
 *
 * \param filename Path to a file containing Jsonnet code.
 * \param snippet Jsonnet code to execute.
 * \param error Return by reference whether or not there was an error.
 * \returns Either the error, or a sequence of strings separated by \0, terminated with \0\0.
 */
char *jsonnet_evaluate_snippet_stream(struct JsonnetVm *vm,
                                      const char *filename,
                                      const char *snippet,
                                      int *error);

/** Complement of \see jsonnet_vm_make. */
void jsonnet_destroy(struct JsonnetVm *vm);

#endif  // LIB_JSONNET_H

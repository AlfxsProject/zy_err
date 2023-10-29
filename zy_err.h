/*
 * Copyright 2023 Alexandre Fernandez <alex@fernandezfamily.email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <stdint.h>
#include <zy_alloc.h>

/*
 * Types
 */

typedef struct zy_err_bx_s zy_err_bx_t;
typedef struct zy_err_s zy_err_t;

/*
 * Constants
 */

#define ZY_OK (0)

/*
 * Functions
 */

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((nonnull)) int zy_err_construct(zy_err_t **err, const zy_alloc_t *alloc);
    __attribute__((nonnull)) void zy_err_destruct(zy_err_t **err);
    __attribute__((nonnull)) void zy_err_clear(zy_err_t *err);
    __attribute__((nonnull(1, 3, 5))) int zy_err_push_first(zy_err_t *err, int64_t code, const char *file, size_t line,
                                                            const char *function, const void *opaque,
                                                            size_t opaque_size);
    __attribute__((nonnull(1, 3, 5))) int zy_err_push_last(zy_err_t *err, int64_t code, const char *file, size_t line,
                                                           const char *function, const void *opaque,
                                                           size_t opaque_size);
    __attribute__((nonnull)) void zy_err_discard_first(zy_err_t *err);
    __attribute__((nonnull)) void zy_err_discard_last(zy_err_t *err);
    __attribute__((nonnull)) zy_err_bx_t *zy_err_peek_first(const zy_err_t *err);
    __attribute__((nonnull)) zy_err_bx_t *zy_err_peek_last(const zy_err_t *err);
    __attribute__((nonnull)) size_t zy_err_size(const zy_err_t *err);
    __attribute__((nonnull)) bool zy_err_is_empty(const zy_err_t *err);
    __attribute__((nonnull)) int64_t zy_err_bx_code(const zy_err_bx_t *bx);
    __attribute__((nonnull)) const char *zy_err_bx_file(const zy_err_bx_t *bx);
    __attribute__((nonnull)) size_t zy_err_bx_line(const zy_err_bx_t *bx);
    __attribute__((nonnull)) const char *zy_err_bx_function(const zy_err_bx_t *bx);
    __attribute__((nonnull(1))) const void *zy_err_bx_opaque(const zy_err_bx_t *bx, size_t *size);

#ifdef __cplusplus
}
#endif
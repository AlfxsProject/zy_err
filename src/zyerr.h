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
#include "zyalloc.h"
#include <stdint.h>

/*
 * Types
 */

typedef struct zyerrbx_s zyerrbx_t;
typedef struct zyerr_s zyerr_t;

/*
 * Constants
 */

#define ZYERR_OK (0)

/*
 * Functions
 */

#ifdef __cplusplus
extern "C"
{
#endif

    int zyerr_construct(zyerr_t **dqe, const zyalloc_t *alloc) __attribute__((nonnull));
    void zyerr_destruct(zyerr_t **dqe) __attribute__((nonnull));
    void zyerr_clear(zyerr_t *dqe) __attribute__((nonnull));
    int zyerr_push_first(zyerr_t *dqe, int64_t code, const char *file, size_t line, const char *function,
                         const void *opaque, size_t opaque_size) __attribute__((nonnull(1, 3, 5)));
    int zyerr_push_last(zyerr_t *dqe, int64_t code, const char *file, size_t line, const char *function,
                        const void *opaque, size_t opaque_size) __attribute__((nonnull(1, 3, 5)));
    void zyerr_discard_first(zyerr_t *dqe) __attribute__((nonnull));
    void zyerr_discard_last(zyerr_t *dqe) __attribute__((nonnull));
    zyerrbx_t *zyerr_peek_first(const zyerr_t *dqe) __attribute__((nonnull));
    zyerrbx_t *zyerr_peek_last(const zyerr_t *dqe) __attribute__((nonnull));
    size_t zyerr_size(const zyerr_t *dqe) __attribute__((nonnull));
    bool zyerr_is_empty(const zyerr_t *dqe) __attribute__((nonnull));
    int64_t zyerrbx_code(const zyerrbx_t *bx) __attribute__((nonnull));
    const char *zyerrbx_file(const zyerrbx_t *bx) __attribute__((nonnull));
    size_t zyerrbx_line(const zyerrbx_t *bx) __attribute__((nonnull));
    const char *zyerrbx_function(const zyerrbx_t *bx) __attribute__((nonnull));
    const void *zyerrbx_opaque(const zyerrbx_t *bx, size_t *size) __attribute__((nonnull(1)));

#ifdef __cplusplus
}
#endif
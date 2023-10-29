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
#include "zy_err.h"
#include "zy_dequeue.h"
#include <string.h>

/*
 * Types
 */

struct zy_err_s
{
    const zy_alloc_t *const alloc;
    zy_dequeue_t *dequeue;
};

struct zy_err_bx_s
{
    const int64_t code;
    const size_t line;
    const char *const file;
    const char *const function;
    const zy_opaque_t opaque;
};

typedef struct opaque_wrapper_s
{
    size_t size;
    struct
    {
        zy_err_bx_t bx;
    } data;
} opaque_wrapper_t;

/*
 * Functions
 */

int zy_err_construct(zy_err_t **err, const zy_alloc_t *alloc)
{
    int r = zy_malloc(alloc, sizeof(zy_err_t), (void **)err);

    if (r == ZY_OK)
    {
        zy_dequeue_t *dequeue;
        r = zy_dequeue_construct(&dequeue, alloc);
        if (r == ZY_OK)
        {
            const zy_err_t dqe_init = {.alloc = alloc, .dequeue = dequeue};
            memcpy((void *)*err, (const void *)&dqe_init, sizeof(zy_err_t));
        }
        return ZY_OK;
    }
    return r;
}

void zy_err_destruct(zy_err_t **err)
{
    if (*err != nullptr)
    {
        zy_dequeue_destruct(&(*err)->dequeue);
        zy_free((*err)->alloc, (void **)err);
    }
}

void zy_err_clear(zy_err_t *err)
{
    zy_dequeue_clear(err->dequeue);
}

int zy_err_push_first(zy_err_t *err, int64_t code, const char *file, size_t line, const char *function,
                      const void *opaque, size_t opaque_size)
{
    void *data;
    int r = zy_malloc(err->alloc, sizeof(opaque_wrapper_t) + opaque_size, &data);
    const opaque_wrapper_t opaque_init = {
        .size = sizeof(zy_err_bx_t) + opaque_size,
        {{.code = code, .line = line, .file = file, .function = function, {.size = opaque_size}}}};
    memcpy(data, (const void *)&opaque_init, sizeof(opaque_wrapper_t));
    memcpy(data + sizeof(opaque_wrapper_t), (const void *)opaque, opaque_size);
    zy_dequeue_push_first(err->dequeue, (const zy_opaque_t *)data);
    zy_free(err->alloc, &data);
    return r;
}

int zy_err_push_last(zy_err_t *err, int64_t code, const char *file, size_t line, const char *function,
                     const void *opaque, size_t opaque_size)
{
    void *data;
    int r = zy_malloc(err->alloc, sizeof(opaque_wrapper_t) + opaque_size, &data);
    const opaque_wrapper_t opaque_init = {
        .size = sizeof(zy_err_bx_t) + opaque_size,
        {{.code = code, .line = line, .file = file, .function = function, {.size = opaque_size}}}};
    memcpy(data, (const void *)&opaque_init, sizeof(opaque_wrapper_t));
    memcpy(data + sizeof(opaque_wrapper_t), (const void *)opaque, opaque_size);
    zy_dequeue_push_last(err->dequeue, (const zy_opaque_t *)data);
    zy_free(err->alloc, &data);
    return r;
}

void zy_err_discard_first(zy_err_t *err)
{
    zy_dequeue_discard_first(err->dequeue);
}

void zy_err_discard_last(zy_err_t *err)
{
    zy_dequeue_discard_last(err->dequeue);
}

zy_err_bx_t *zy_err_peek_first(const zy_err_t *err)
{
    return (zy_err_bx_t *)(&((zy_opaque_t *)zy_dequeue_peek_first(err->dequeue))->data);
}

zy_err_bx_t *zy_err_peek_last(const zy_err_t *err)
{
    return (zy_err_bx_t *)(&((zy_opaque_t *)zy_dequeue_peek_last(err->dequeue))->data);
}

size_t zy_err_size(const zy_err_t *err)
{
    return zy_dequeue_size(err->dequeue);
}

bool zy_err_is_empty(const zy_err_t *err)
{
    return zy_dequeue_is_empty(err->dequeue);
}

int64_t zy_err_bx_code(const zy_err_bx_t *bx)
{
    return bx->code;
}

const char *zy_err_bx_file(const zy_err_bx_t *bx)
{
    return bx->file;
}

size_t zy_err_bx_line(const zy_err_bx_t *bx)
{
    return bx->line;
}

const char *zy_err_bx_function(const zy_err_bx_t *bx)
{
    return bx->function;
}

const void *zy_err_bx_opaque(const zy_err_bx_t *bx, size_t *size)
{
    if (size != nullptr)
    {
        *size = bx->opaque.size;
    }
    return &bx->opaque.data;
}
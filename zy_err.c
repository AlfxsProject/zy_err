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
#include <stddef.h>
#include <string.h>

/*
 * Types
 */

struct zy_err_s
{
    const zy_alloc_t *alloc;
    zy_dequeue_t *dequeue;
};

struct zy_err_bx_s
{
    int64_t code;
    size_t line;
    const char *file;
    const char *function;
    zy_opaque_t opaque;
};

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
            (*err)->alloc = alloc;
            (*err)->dequeue = dequeue;
        }
        else
        {
            zy_free(alloc, (void **)err);
        }
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
    int r = zy_malloc(err->alloc, sizeof(zy_opaque_t) + sizeof(zy_err_bx_t) + opaque_size, &data);
    *(size_t *)(data + offsetof(zy_opaque_t, size)) = sizeof(zy_opaque_t) + sizeof(zy_err_bx_t) + opaque_size;
    *(int64_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, code)) = code;
    *(size_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, line)) = line;
    *(const char **)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, file)) = file;
    *(const char **)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, function)) = function;
    *(size_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, opaque) + offsetof(zy_opaque_t, size)) =
        opaque_size;
    memcpy(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, opaque) + offsetof(zy_opaque_t, data), opaque,
           opaque_size);
    zy_dequeue_push_first(err->dequeue, (const zy_opaque_t *)data);
    zy_free(err->alloc, &data);
    return r;
}

int zy_err_push_last(zy_err_t *err, int64_t code, const char *file, size_t line, const char *function,
                     const void *opaque, size_t opaque_size)
{
    void *data;
    int r = zy_malloc(err->alloc, sizeof(zy_opaque_t) + sizeof(zy_err_bx_t) + opaque_size, &data);
    *(size_t *)(data + offsetof(zy_opaque_t, size)) = sizeof(zy_opaque_t) + sizeof(zy_err_bx_t) + opaque_size;
    *(int64_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, code)) = code;
    *(size_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, line)) = line;
    *(const char **)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, file)) = file;
    *(const char **)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, function)) = function;
    *(size_t *)(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, opaque) + offsetof(zy_opaque_t, size)) =
        opaque_size;
    memcpy(data + offsetof(zy_opaque_t, data) + offsetof(zy_err_bx_t, opaque) + offsetof(zy_opaque_t, data), opaque,
           opaque_size);
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
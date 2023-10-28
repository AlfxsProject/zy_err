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
#include "zyerr.h"
#include "dequeue.h"
#include <string.h>

/*
 * Types
 */

struct zyerrbx_s
{
    const int64_t code;
    const size_t line;
    const char *const file;
    const char *const function;
    const opaque_t opaque;
};

struct zyerr_s
{
    const zyalloc_t *const alloc;
    dequeue_t *dequeue;
};

/*
 * Functions
 */

int zyerr_construct(zyerr_t **err, const zyalloc_t *alloc)
{
    int r = zymalloc(alloc, sizeof(zyerr_t), (void **)err);

    if (r == ZYALLOC_OK)
    {
        dequeue_t *dequeue;
        r = dequeue_construct(&dequeue, alloc);
        if (r == ZYALLOC_OK)
        {
            const zyerr_t dqe_init = {.alloc = alloc, .dequeue = dequeue};
            memcpy((void *)*err, (const void *)&dqe_init, sizeof(zyerr_t));
        }
        return ZYERR_OK;
    }
    return r;
}

void zyerr_destruct(zyerr_t **err)
{
    if (*err != nullptr)
    {
        dequeue_destruct(&(*err)->dequeue);
        zyfree((*err)->alloc, (void **)err);
    }
}

void zyerr_clear(zyerr_t *err)
{
    dequeue_clear(err->dequeue);
}

int zyerr_push_first(zyerr_t *err, int64_t code, const char *file, size_t line, const char *function,
                     const void *opaque, size_t opaque_size)
{
    void *data;
    int r = zymalloc(err->alloc, sizeof(opaque_t) + sizeof(zyerrbx_t) + opaque_size, &data);
    const opaque_t opaque_init = {.size = sizeof(zyerrbx_t) + opaque_size};
    const zyerrbx_t bx_init = {.code = code, .line = line, .file = file, .function = function, {.size = opaque_size}};
    memcpy(data, (const void *)&opaque_init, sizeof(opaque_t));
    memcpy(data + sizeof(opaque_t), (const void *)&bx_init, sizeof(zyerrbx_t));
    memcpy(data + sizeof(opaque_t) + sizeof(zyerrbx_t), (const void*)opaque, opaque_size);
    dequeue_push_first(err->dequeue, (const opaque_t *)data);
    zyfree(err->alloc, &data);
    return r;
}

int zyerr_push_last(zyerr_t *err, int64_t code, const char *file, size_t line, const char *function, const void *opaque,
                    size_t opaque_size)
{
    void *data;
    int r = zymalloc(err->alloc, sizeof(opaque_t) + sizeof(zyerrbx_t) + opaque_size, &data);
    const opaque_t opaque_init = {.size = sizeof(zyerrbx_t) + opaque_size};
    const zyerrbx_t bx_init = {.code = code, .line = line, .file = file, .function = function, {.size = opaque_size}};
    memcpy(data, (const void *)&opaque_init, sizeof(opaque_t));
    memcpy(data + sizeof(opaque_t), (const void *)&bx_init, sizeof(zyerrbx_t));
    memcpy(data + sizeof(opaque_t) + sizeof(zyerrbx_t), (const void*)opaque, opaque_size);
    dequeue_push_last(err->dequeue, (const opaque_t *)data);
    zyfree(err->alloc, &data);
    return r;
}

void zyerr_discard_first(zyerr_t *err)
{
    dequeue_discard_first(err->dequeue);
}

void zyerr_discard_last(zyerr_t *err)
{
    dequeue_discard_last(err->dequeue);
}

zyerrbx_t *zyerr_peek_first(const zyerr_t *err)
{
    return (zyerrbx_t *)(&((opaque_t*)dequeue_peek_first(err->dequeue))->data);
}

zyerrbx_t *zyerr_peek_last(const zyerr_t *err)
{
    return (zyerrbx_t *)(&((opaque_t*)dequeue_peek_last(err->dequeue))->data);
}

size_t zyerr_size(const zyerr_t *err)
{
    return dequeue_size(err->dequeue);
}

bool zyerr_is_empty(const zyerr_t *err)
{
    return dequeue_is_empty(err->dequeue);
}

int64_t zyerrbx_code(const zyerrbx_t *bx)
{
    return bx->code;
}

const char *zyerrbx_file(const zyerrbx_t *bx)
{
    return bx->file;
}

size_t zyerrbx_line(const zyerrbx_t *bx)
{
    return bx->line;
}

const char *zyerrbx_function(const zyerrbx_t *bx)
{
    return bx->function;
}

const void *zyerrbx_opaque(const zyerrbx_t *bx, size_t *size)
{
    if (size != nullptr)
    {
        *size = bx->opaque.size;
    }
    return &bx->opaque.data;
}
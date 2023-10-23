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
#include <stdatomic.h>
#include <string.h>

/*
 * Types
 */

struct zyerrbx_s
{
    zyerrbx_t *prev, *next;
    const int64_t code;
    const char *const file;
    const size_t line;
    const char *const function;
    const size_t opaque_size;
    const uint8_t opaque[0];
};

struct zyerr_s
{
    const zyalloc_t *const alloc;
    zyerrbx_t *first, *last;
    atomic_size_t size;
};

/*
 * Static Functions
 */

static int zyerrbx_construct(zyerrbx_t **bx, const zyalloc_t *alloc, int64_t code, const char *file, size_t line,
                             const char *function, const void *opaque, size_t opaque_size)
{
    int r = zymalloc(alloc, sizeof(zyerrbx_t) + opaque_size, (void **)bx);

    if (r == ZYALLOC_OK)
    {
        if (opaque != NULL)
        {
            memcpy(((void *)*bx) + sizeof(zyerrbx_t), opaque, opaque_size);
        }
        const zyerrbx_t bx_init = {.prev = NULL,
                                   .next = NULL,
                                   .code = code,
                                   .file = file,
                                   .line = line,
                                   .function = function,
                                   .opaque_size = opaque_size};
        memcpy((void *)*bx, (const void *)&bx_init, sizeof(zyerrbx_t));
        return ZYERR_OK;
    }
    return r;
}

static void zyerrbx_destruct(zyerrbx_t **bx, const zyalloc_t *alloc)
{
    zyfree(alloc, (void **)bx);
}

/*
 * Functions
 */

int zyerr_construct(zyerr_t **dqe, const zyalloc_t *alloc)
{
    int r = zymalloc(alloc, sizeof(zyerr_t), (void **)dqe);

    if (r == ZYALLOC_OK)
    {
        const zyerr_t dqe_init = {.alloc = alloc, .first = NULL, .last = NULL, .size = 0};
        memcpy((void *)*dqe, (const void *)&dqe_init, sizeof(zyerr_t));
        return ZYERR_OK;
    }
    return r;
}

void zyerr_destruct(zyerr_t **dqe)
{
    zyerr_clear(*dqe);
    zyfree((*dqe)->alloc, (void **)dqe);
}

void zyerr_clear(zyerr_t *dqe)
{
    zyerrbx_t *bx = dqe->first;

    while (bx != NULL)
    {
        zyerrbx_t *next = bx->next;
        zyerrbx_destruct(&bx, dqe->alloc);
        bx = next;
    }
    dqe->first = NULL;
    dqe->last = NULL;
    dqe->size = 0;
}

int zyerr_push_first(zyerr_t *dqe, int64_t code, const char *file, size_t line, const char *function,
                     const void *opaque, size_t opaque_size)
{
    zyerrbx_t *bx;
    int r = zyerrbx_construct(&bx, dqe->alloc, code, file, line, function, opaque, opaque_size);

    if (r == ZYERR_OK)
    {
        if (dqe->first != NULL)
        {
            bx->next = dqe->first;
            dqe->first->prev = bx;
            dqe->first = bx;
        }
        else
        {
            dqe->first = bx;
            dqe->last = bx;
        }
        ++dqe->size;
    }
    return r;
}

int zyerr_push_last(zyerr_t *dqe, int64_t code, const char *file, size_t line, const char *function, const void *opaque,
                    size_t opaque_size)
{
    zyerrbx_t *bx;
    int r = zyerrbx_construct(&bx, dqe->alloc, code, file, line, function, opaque, opaque_size);

    if (r == ZYERR_OK)
    {
        if (dqe->last != NULL)
        {
            bx->prev = dqe->last;
            dqe->last->next = bx;
            dqe->last = bx;
        }
        else
        {
            dqe->first = bx;
            dqe->last = bx;
        }
        ++dqe->size;
    }
    return r;
}

void zyerr_discard_first(zyerr_t *dqe)
{
    if (dqe->size != 0)
    {
        zyerrbx_t *bx = dqe->first;
        if (bx->next != NULL)
        {
            bx->next->prev = NULL;
            dqe->first = bx->next;
        }
        else
        {
            dqe->first = NULL;
            dqe->last = NULL;
        }
        --dqe->size;
        zyerrbx_destruct(&bx, dqe->alloc);
    }
}

void zyerr_discard_last(zyerr_t *dqe)
{
    if (dqe->size != 0)
    {
        zyerrbx_t *bx = dqe->last;
        if (bx->prev != NULL)
        {
            bx->prev->next = NULL;
            dqe->last = bx->prev;
        }
        else
        {
            dqe->first = NULL;
            dqe->last = NULL;
        }
        --dqe->size;
        zyerrbx_destruct(&bx, dqe->alloc);
    }
}

zyerrbx_t *zyerr_peek_first(const zyerr_t *dqe)
{
    return dqe->first;
}

zyerrbx_t *zyerr_peek_last(const zyerr_t *dqe)
{
    return dqe->last;
}

size_t zyerr_size(const zyerr_t *dqe)
{
    return dqe->size;
}

bool zyerr_is_empty(const zyerr_t *dqe)
{
    return dqe->size == 0;
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

const void *zyerbx_opaque(const zyerrbx_t *bx, size_t *size)
{
    if (size != NULL)
    {
        *size = bx->opaque_size;
    }
    return &bx->opaque;
}
/**
 * Naïve implementation of Murmur3 for 32 bit with update
 * functionality.
 *
 * See https://en.wikipedia.org/wiki/MurmurHash#Algorithm and
 * https://github.com/aappleby/smhasher/wiki/MurmurHash3
 *
 * Copyright (C) 2020 Olaf Bergmann <bergmann@tzi.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MURMUR_3_32_H
#define MURMUR_3_32_H

#include <stdint.h>

typedef struct murmur3_context_t {
  size_t total_len;
  uint32_t key;
  uint32_t hashval;
  uint8_t len;
} murmur3_context_t;

/**
 * Initializes the provided murmur3 context object @p mctx.
 *
 * @param mctx The murmur3 context to initialize.
 */
void murmur3_32_init(murmur3_context_t *mctx);

/**
 * Updates the state of the murmur3 context object @p mctx with
 * @p len bytes from @p data.
 *
 * @param mctx The murmur3 context to update.
 * @param data The data to hash.
 * @param len  The actual length of @p data.
 */
void murmur3_32_update(murmur3_context_t *mctx, const void *data, size_t len);

/**
 * Finalizes the given murmur3 context @p mctx and calculates the
 * resulting murmur3 hash value.
 *
 * @param mctx The murmur3 context to update.
 *
 * @return The murmur3 hash value of the data that was passed to
 * murmur3_32_update() for the context @p mctx.
 */
uint32_t murmur3_32_finalize(murmur3_context_t *mctx);

/**
 * Calculcates the murmur3 hash value of @p data. This is a
 * convenience function that encapsulates the required calls to
 * murmur3_32_init(), murmur3_32_update(), and murmur3_32_finalize().
 *
 * @param data The data to hash.
 * @param len  The actual length of @p data.
 *
 * @return The murmur3 hash value of the data.
 */
uint32_t murmur3_32(const void *data, size_t len);

#endif /* MURMUR_3_32_H */

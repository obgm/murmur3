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

#include <assert.h>
#include <string.h>

#include "murmur3.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif /* min */

static inline uint32_t
rol(uint32_t val, uint8_t bits) {
  return (val << bits) + (val >> (32 - bits));
}

void
murmur3_32_init(murmur3_context_t *mctx) {
  memset(mctx, 0, sizeof(murmur3_context_t));
}

const size_t murmur3_block_length = sizeof(uint32_t);
const uint32_t c1 = 0xcc9e2d51;
const uint32_t c2 = 0x1b873593;
const uint8_t r1 = 15;
const uint8_t r2 = 13;
const uint8_t m = 5;
const uint32_t n = 0xe6546b64;

static void
murmur3_32_eatblock(murmur3_context_t *mctx) {
  mctx->key *= c1;
  mctx->key = rol(mctx->key, r1);
  mctx->key *= c2;

  mctx->hashval ^= mctx->key;
  mctx->hashval = rol(mctx->hashval, r2);
  mctx->hashval = (mctx->hashval * m) + n;

  mctx->total_len += murmur3_block_length;
  mctx->len = 0;
  mctx->key = 0;
}

static size_t
murmur3_32_fillblock(murmur3_context_t *mctx, const void *data, size_t len) {
  const uint8_t *key = (const uint8_t *)data;
  size_t removed = 0;

  assert(mctx->len < murmur3_block_length);
  if (len) {
    size_t l = min(len, (size_t)(murmur3_block_length - mctx->len));
    removed = l;
    /* little endian */
    while (l--) {
      mctx->key <<= 8;
      mctx->key += key[l];
    }
    mctx->len += removed;

    if (mctx->len == murmur3_block_length) {
      murmur3_32_eatblock(mctx);
    }
  }

  return removed;
}

void
murmur3_32_update(murmur3_context_t *mctx, const void *data, size_t len) {
  const uint8_t *key = (const uint8_t *)data;

  /* first, feed all remaining data from mctx to murmur3 */
  size_t removed = murmur3_32_fillblock(mctx, data, len);
  if (removed) {
    len -= removed;
    key += removed;
  }

  while (len > (murmur3_block_length - 1)) {
    assert(mctx->len == 0);
    /* little endian */
    mctx->key = (key[3] << 24) + (key[2] << 16) + (key[1] << 8) + key[0];
    murmur3_32_eatblock(mctx);

    len -= murmur3_block_length;
    key += murmur3_block_length;
  }

  /* store key[0..len-1] for later use */
  murmur3_32_fillblock(mctx, key, len);
}

uint32_t
murmur3_32_finalize(murmur3_context_t *mctx) {

  if (mctx->len) {
    mctx->key *= c1;
    mctx->key = rol(mctx->key, r1);
    mctx->key *= c2;

    mctx->hashval ^= mctx->key;
    mctx->total_len += mctx->len;
  }

  mctx->hashval ^= mctx->total_len;

  mctx->hashval ^= mctx->hashval >> 16;
  mctx->hashval *= 0x85ebca6b;
  mctx->hashval ^= mctx->hashval >> 13;
  mctx->hashval *= 0xc2b2ae35;
  mctx->hashval ^= mctx->hashval >> 16;
  return mctx->hashval;
}

uint32_t
murmur3_32(const void *data, size_t len) {
  murmur3_context_t mctx;
  murmur3_32_init(&mctx);
  murmur3_32_update(&mctx, data, len);
  return murmur3_32_finalize(&mctx);
}

#include <stdio.h>

#include "murmur3.h"

void TestString(const char *str, size_t len, uint32_t seed, uint32_t expected) {
  uint32_t result;
  murmur3_context_t mctx;
  murmur3_32_init(&mctx);
  mctx.hashval = seed;
  murmur3_32_update(&mctx, str, len);
  result = murmur3_32_finalize(&mctx);

  const char *msg[] = { "FAIL", "OK" };
  printf("%s: '%s' (%zu bytes, expected 0x%0x, got 0x%0x)\n",
         msg[result == expected],
         str, len, expected, result);

  if (seed == 0) {
    result = murmur3_32(str, len);
    printf("%s: '%s' (%zu bytes, expected 0x%0x, got 0x%0x)\n",
         msg[result == expected],
         str, len, expected, result);
  }
}

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif /* min */

void  TestPartial(const char *str, size_t len, uint32_t seed, uint32_t expected) {
  uint32_t result;
  murmur3_context_t mctx;
  murmur3_32_init(&mctx);
  mctx.hashval = seed;
  do {
    const size_t l = min(len, 7);
    murmur3_32_update(&mctx, str, l);
    len -= l;
    str += l;
  } while (len);
  result = murmur3_32_finalize(&mctx);

  const char *msg[] = { "FAIL", "OK" };
  printf("%s: expected 0x%0x, got 0x%0x)\n", msg[result == expected], expected, result);  
}

/* Test vectors from https://stackoverflow.com/a/31929528 */
int main() {
  TestString("", 0, 0, 0); //empty string with zero seed should give zero
  TestString("", 0, 1, 0x514E28B7);
  TestString("", 0, 0xffffffff, 0x81F16F39); //make sure seed value is handled unsigned
  TestString("\000\000\000\000", 4, 0, 0x2362F9DE); //make sure we handle embedded nulls

  TestString("aaaa", 4, 0x9747b28c, 0x5A97808A); //one full chunk
  TestString("aaa", 3, 0x9747b28c, 0x283E0130); //three characters
  TestString("aa", 2, 0x9747b28c, 0x5D211726); //two characters
  TestString("a", 1, 0x9747b28c, 0x7FA09EA6); //one character

  //Endian order within the chunks
  TestString("abcd", 4, 0x9747b28c, 0xF0478627); //one full chunk
  TestString("abc", 3, 0x9747b28c, 0xC84A62DD);
  TestString("ab", 2, 0x9747b28c, 0x74875592);
  TestString("a", 1, 0x9747b28c, 0x7FA09EA6);

  TestString("Hello, world!", 13, 0x9747b28c, 0x24884CBA);

//String of 256 characters.
//Make sure you don't store string lengths in a char, and overflow at 255 bytes (as OpenBSD's canonical BCrypt implementation did)
  TestString("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 256, 0x9747b28c, 0x37405BDC);
  TestPartial("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 256, 0x9747b28c, 0x37405BDC);
}

# murmur3: A naïve implementation of Murmur3 for 32 bit with update functionality

Simple Example:

```c
  uint32_t result;

  result = murmur3_32(data, length);
```


Using incremental updates:

```c
  murmur3_context_t mctx;
  uint32_t result;

  murmur3_32_init(&mctx);
  murmur3_32_update(&mctx, data, length);
  result = murmur3_32_finalize(&mctx);
```

See https://en.wikipedia.org/wiki/MurmurHash#Algorithm and
https://github.com/aappleby/smhasher/wiki/MurmurHash3

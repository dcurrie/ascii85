/* xorshift_e.h */

#include <stdint.h>

uint64_t murmurhash3_avalanche (uint64_t x);

uint64_t xorshift64star_next (void);
void     xorshift64star_seed (uint64_t x);

uint64_t xorshift128plus_next (void);
void     xorshift128plus_seed (uint64_t x);

uint64_t xorshift1024star_next (void);
void     xorshift1024star_seed (uint64_t x);


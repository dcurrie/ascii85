/* xorshift_e.c */

/* Gathered together here are a bunch of Sebastiano Vigna's PRNGs for ease of use. */

/* ** */

/*  Written in 2014 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include "xorshift_e.h"

uint64_t murmurhash3_avalanche (uint64_t x)
{
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    return x ^= x >> 33;
}

/* xorshift64* is a good generator if you're short on memory, but otherwise we
   rather suggest to use a xorshift128+ (for maximum speed) or
   xorshift1024* (for speed and very long period) generator. */

static uint64_t xorshift64star_x; /* The state must be seeded with a nonzero value. */

uint64_t xorshift64star_next (void)
{
    xorshift64star_x ^= xorshift64star_x >> 12; // a
    xorshift64star_x ^= xorshift64star_x << 25; // b
    xorshift64star_x ^= xorshift64star_x >> 27; // c
    return xorshift64star_x * 2685821657736338717LL;
}

void xorshift64star_seed (uint64_t x)
{
    xorshift64star_x = murmurhash3_avalanche((x == 0) ? 42 : x);
}

/* xorshift128+ is the fastest generator passing BigCrush without systematic
   errors, but due to the relatively short period it is acceptable only
   for applications with a very mild amount of parallelism; otherwise, use
   a xorshift1024* generator. */

/* The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to pass it twice through MurmurHash3's
   avalanching function. */

static uint64_t xorshift128plus_s[2];

uint64_t xorshift128plus_next (void)
{ 
    uint64_t s1 = xorshift128plus_s[0];
    const uint64_t s0 = xorshift128plus_s[1];
    xorshift128plus_s[0] = s0;
    s1 ^= s1 << 23; // a
    return (xorshift128plus_s[1] = (s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26))) + s0; // b, c
}

void xorshift128plus_seed (uint64_t x)
{
    uint64_t s0 = murmurhash3_avalanche(murmurhash3_avalanche((x == 0) ? 42 : x));
    xorshift128plus_s[0] = s0;
    xorshift128plus_s[1] = murmurhash3_avalanche(s0);
}

/* xorshift1024* is a fast, top-quality generator. If 1024 bits of state are too
   much, try a xorshift128+ or a xorshift64* generator. */

/* The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed,  we suggest to seed a xorshift64* generator and use its
   output to fill s. */

static uint64_t xorshift1024star_s[16]; 
static int xorshift1024star_p = 0;

uint64_t xorshift1024star_next (void)
{ 
    uint64_t s0 = xorshift1024star_s[xorshift1024star_p];
    uint64_t s1 = xorshift1024star_s[xorshift1024star_p = (xorshift1024star_p + 1) & 15];
    s1 ^= s1 << 31; // a
    s1 ^= s1 >> 11; // b
    s0 ^= s0 >> 30; // c
    return (xorshift1024star_s[xorshift1024star_p] = s0 ^ s1 ) * 1181783497276652981LL; 
}

void xorshift1024star_seed (uint64_t x)
{
    int i;
    xorshift64star_seed(x);
    for (i = 0; i < 16; i++) xorshift1024star_s[i] = xorshift64star_next();
}

/* ************************ testing code below ************************** */

#ifdef TESTU01

#include <stdio.h>
#include <stdlib.h>

#include <unif01.h>
#include <bbattery.h>

/* True if the lower part of y has already been returned. */
static int lower = 0;
static uint64_t (*next)();
static uint64_t y;

/* Note that this function returns a 32-bit number. */
unsigned long xorshift (void *unused0, void *unused1)
{
    lower = 1 - lower;
    if (0 != lower)
    {
        y = next();
        return (uint32_t)y;
    }
    return y >> 32;
}

#define NORM_64 ( ( 1./4 ) / ( 1LL << 62 ) )

double xorshift01 (void *unused0, void *unused1)
{
    lower = 0; // we've used it all
    return next() * NORM_64;
}

void write_state_64 (void *unused) 
{
    printf( "%llu\n", (unsigned long long)xorshift64star_x);
}

void write_state_128 (void *unused) 
{
    for( int i = 0; i < 2; i++ )
        printf("%s%llu", i ? " " : "", (unsigned long long)xorshift128plus_s[i]);
    printf("\n");
}

void write_state_1024 (void *unused) 
{
    for( int i = 0; i < 16; i++ )
        printf("%s%llu", i ? " " : "", (unsigned long long)xorshift1024star_s[i]);
    printf("\n");
}

int main(int argc, char *argv[] )
{
    unif01_Gen gen;
    gen.GetBits = xorshift;
    gen.GetU01 = xorshift01;

    if (argc != 3)
    {
usage:
        fprintf(stderr, "%s {64|128|1024} seed\n", argv[0]);
        exit(1);
    }

    uint32_t x = strtoull(argv[2], NULL, 0);
    if (0 == x) x = 1;

    switch (atoi(argv[1]))
    {
        case 64:
            xorshift64star_seed(x);
            next = xorshift64star_next;
            gen.name = "xorshift64*";
            gen.Write = write_state_64;
            break;

        case 128:
            xorshift128plus_seed(x);
            next = xorshift128plus_next;
            gen.name = "xorshift128+";
            gen.Write = write_state_128;
            break;

        case 1024:
            xorshift1024star_seed(x);
            next = xorshift1024star_next;
            gen.name = "xorshift1024*";
            gen.Write = write_state_1024;
            break;

        default:
            goto usage;
    }
    TESTU01(&gen);
    return 0;
}
#endif

/** @file test_ascii85.c
 *
 * @brief Ascii85 encoder and decoder tester
 *
 * @par
 * @copyright Copyright © 2017 Doug Currie, Londonderry, NH, USA. All rights reserved.
 * 
 * @par
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without 
 * restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/

#include "ascii85.h"

#include "lcut/lcut.h"
#include "lcut/xorshift_e.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Some tests adapted from https://github.com/judsonx/base85 -- thanks Judson Weissert!

// Wikipedia Example for Ascii85
// A quote from Thomas Hobbes's Leviathan:
//
// Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.
//
// If this is initially encoded using US-ASCII, it can be reencoded in Ascii85 as follows:
//
// <~9jqo^BlbD-BleB1DJ+*+F(f,q/0JhKF<GL>Cj@.4Gp$d7F!,L7@<6@)/0JDEF<G%<+EV:2F!,
// O<DJ+*.@<*K0@<6L(Df-\0Ec5e;DffZ(EZee.Bl.9pF"AGXBPCsi+DGm>@3BB/F*&OCAfu2/AKY
// i(DIb:@FD,*)+C]U=@3BN#EcYf8ATD3s@q?d$AftVqCh[NqF<G:8+EV:.+Cf>-FD5W8ARlolDIa
// l(DId<j@<?3r@:F%a+D58'ATD4$Bl@l3De:,-DJs`8ARoFb/0JMK@qB4^F!,R<AKZ&-DfTqBG%G
// >uD.RTpAKYo'+CT/5+Cei#DII?(E,9)oF*2M7/c~>

typedef struct tv_pair_s
{
    const uint8_t *in;
    const uint8_t *out;
} tv_pair_t;

tv_pair_t tp0 =
{
        .in = (const uint8_t *)
        "Man is distinguished, not only by his reason, but by this singular passion from other animals, "
        "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
        "generation of knowledge, exceeds the short vehemence of any carnal pleasure.",

        .out = (const uint8_t *)
        "9jqo^BlbD-BleB1DJ+*+F(f,q/0JhKF<GL>Cj@.4Gp$d7F!,L7@<6@)/0JDEF<G%<+EV:2F!,"
        "O<DJ+*.@<*K0@<6L(Df-\\0Ec5e;DffZ(EZee.Bl.9pF\"AGXBPCsi+DGm>@3BB/F*&OCAfu2/AKY"
        "i(DIb:@FD,*)+C]U=@3BN#EcYf8ATD3s@q?d$AftVqCh[NqF<G:8+EV:.+Cf>-FD5W8ARlolDIa"
        "l(DId<j@<?3r@:F%a+D58'ATD4$Bl@l3De:,-DJs`8ARoFb/0JMK@qB4^F!,R<AKZ&-DfTqBG%G"
        ">uD.RTpAKYo'+CT/5+Cei#DII?(E,9)oF*2M7/c"
};

static void tc_encode_decode (lcut_tc_t *tc, void *data)
{
    tv_pair_t *tv = (tv_pair_t *)data;

    uint8_t buf[1024];

    int32_t olen = encode_ascii85(tv->in, (int32_t )strlen((const char *)tv->in), buf, 1023u);

    LCUT_TRUE(tc, olen >= 0);
    LCUT_TRUE(tc, olen < 1024);

    buf[olen] = 0u;

    LCUT_TRUE(tc, (0 == strcmp((char *)buf, (const char *)tv->out)));

    olen = decode_ascii85(tv->out, (int32_t )strlen((const char *)tv->out), buf, 1023u);

    LCUT_TRUE(tc, olen >= 0);
    LCUT_TRUE(tc, olen < 1024);

    buf[olen] = 0u;

    LCUT_TRUE(tc, (0 == strcmp((char *)buf, (const char *)tv->in)));
}

typedef struct tv_epair_s
{
    const uint8_t *in;
    int32_t isz;
    const uint8_t *out;
    int32_t osz;
} tv_epair_t;

#define helloworld ((const uint8_t *)"hello world!")

static tv_epair_t tps0  = { .in = helloworld, .isz = 0,  .out = (const uint8_t *)"", .osz = 0 };
static tv_epair_t tps1  = { .in = helloworld, .isz = 1,  .out = (const uint8_t *)"BE", .osz = 2 };
static tv_epair_t tps2  = { .in = helloworld, .isz = 2,  .out = (const uint8_t *)"BOq", .osz = 3 };
static tv_epair_t tps3  = { .in = helloworld, .isz = 3,  .out = (const uint8_t *)"BOtu", .osz = 4 };
static tv_epair_t tps4  = { .in = helloworld, .isz = 4,  .out = (const uint8_t *)"BOu!r", .osz = 5 };
static tv_epair_t tps5  = { .in = helloworld, .isz = 5,  .out = (const uint8_t *)"BOu!rDZ", .osz = 7 };
static tv_epair_t tps6  = { .in = helloworld, .isz = 6,  .out = (const uint8_t *)"BOu!rD]f", .osz = 8 };
static tv_epair_t tps7  = { .in = helloworld, .isz = 7,  .out = (const uint8_t *)"BOu!rD]j6", .osz = 9 };
static tv_epair_t tps8  = { .in = helloworld, .isz = 8,  .out = (const uint8_t *)"BOu!rD]j7B", .osz = 10 };
static tv_epair_t tps9  = { .in = helloworld, .isz = 9,  .out = (const uint8_t *)"BOu!rD]j7BEW", .osz = 12 };
static tv_epair_t tps10 = { .in = helloworld, .isz = 10, .out = (const uint8_t *)"BOu!rD]j7BEbk", .osz = 13 };
static tv_epair_t tps11 = { .in = helloworld, .isz = 11, .out = (const uint8_t *)"BOu!rD]j7BEbo7", .osz = 14 };
static tv_epair_t tps12 = { .in = helloworld, .isz = 12, .out = (const uint8_t *)"BOu!rD]j7BEbo80", .osz = 15 };

static uint8_t zeroes[32];

static tv_epair_t tpz0  = { .in = zeroes, .isz = 0,  .out = (const uint8_t *)"",         .osz = 0 };
static tv_epair_t tpz4  = { .in = zeroes, .isz = 4,  .out = (const uint8_t *)"z",        .osz = 1 };
static tv_epair_t tpz8  = { .in = zeroes, .isz = 8,  .out = (const uint8_t *)"zz",       .osz = 2 };
static tv_epair_t tpz12 = { .in = zeroes, .isz = 12, .out = (const uint8_t *)"zzz",      .osz = 3 };
static tv_epair_t tpz16 = { .in = zeroes, .isz = 16, .out = (const uint8_t *)"zzzz",     .osz = 4 };
static tv_epair_t tpz20 = { .in = zeroes, .isz = 20, .out = (const uint8_t *)"zzzzz",    .osz = 5 };
static tv_epair_t tpz24 = { .in = zeroes, .isz = 24, .out = (const uint8_t *)"zzzzzz",   .osz = 6 };
static tv_epair_t tpz28 = { .in = zeroes, .isz = 28, .out = (const uint8_t *)"zzzzzzz",  .osz = 7 };
static tv_epair_t tpz32 = { .in = zeroes, .isz = 32, .out = (const uint8_t *)"zzzzzzzz", .osz = 8 };

static tv_epair_t tpz1  = { .in = zeroes, .isz = 1,  .out = (const uint8_t *)"!!",       .osz = 2 };
static tv_epair_t tpz5  = { .in = zeroes, .isz = 5,  .out = (const uint8_t *)"z!!",      .osz = 3 };
static tv_epair_t tpz9  = { .in = zeroes, .isz = 9,  .out = (const uint8_t *)"zz!!",     .osz = 4 };

static tv_epair_t tpz2  = { .in = zeroes, .isz = 2,  .out = (const uint8_t *)"!!!",      .osz = 3 };
static tv_epair_t tpz6  = { .in = zeroes, .isz = 6,  .out = (const uint8_t *)"z!!!",     .osz = 4 };
static tv_epair_t tpz10 = { .in = zeroes, .isz = 10, .out = (const uint8_t *)"zz!!!",    .osz = 5 };

static tv_epair_t tpz3  = { .in = zeroes, .isz = 3,  .out = (const uint8_t *)"!!!!",     .osz = 4 };
static tv_epair_t tpz7  = { .in = zeroes, .isz = 7,  .out = (const uint8_t *)"z!!!!",    .osz = 5 };
static tv_epair_t tpz11 = { .in = zeroes, .isz = 11, .out = (const uint8_t *)"zz!!!!",   .osz = 6 };

static const uint8_t binary1[] = { 0xff, 0xd8, 0xff, 0xe0 };
static const uint8_t binary2[] = { 0xff, 0xff, 0xff, 0xff };

static tv_epair_t tpb0  = { .in = binary1, .isz = 4, .out = (const uint8_t *)"s4IA0", .osz = 5};
static tv_epair_t tpb1  = { .in = binary2, .isz = 4, .out = (const uint8_t *)"s8W-!", .osz = 5};

static void tc_e_encode_decode (lcut_tc_t *tc, void *data)
{
    tv_epair_t *tv = (tv_epair_t *)data;

    uint8_t buf[1024];

    int32_t olen = encode_ascii85(tv->in, tv->isz, buf, 1023u);

    LCUT_TRUE(tc, olen >= 0);
    LCUT_TRUE(tc, olen < 1024);
    LCUT_TRUE(tc, olen == tv->osz);

    LCUT_TRUE(tc, (0 == memcmp(buf, tv->out, olen)));

    olen = decode_ascii85(tv->out, tv->osz, buf, 1023u);

    LCUT_TRUE(tc, olen >= 0);
    LCUT_TRUE(tc, olen < 1024);
    LCUT_TRUE(tc, olen == tv->isz);

    LCUT_TRUE(tc, (0 == memcmp(buf, tv->in, olen)));
}

static tv_epair_t tpe1  = { .in = (const uint8_t *)"abcx",   .isz = 4, .out = (const uint8_t *)"", .osz = ascii85_err_bad_decode_char };
static tv_epair_t tpe2  = { .in = (const uint8_t *)"~>",     .isz = 2, .out = (const uint8_t *)"", .osz = ascii85_err_bad_decode_char };
static tv_epair_t tpe3  = { .in = (const uint8_t *)"s8W-\"", .isz = 5, .out = (const uint8_t *)"", .osz = ascii85_err_decode_overflow };

static void tc_expect_error (lcut_tc_t *tc, void *data)
{
    tv_epair_t *tv = (tv_epair_t *)data;

    uint8_t buf[1024];

    int32_t olen = decode_ascii85(tv->in, tv->isz, buf, 1023u);

    if (olen != tv->osz) printf("Got: %d sb %d\n", olen, tv->osz);

    LCUT_TRUE(tc, olen < 0);
    LCUT_TRUE(tc, olen == tv->osz);
}


#define MAX_A85_SIZE (4095u)

static uint32_t random_size (void)
{
    uint32_t x = (xorshift128plus_next() % 8u) + 1u;
    uint32_t y = (xorshift128plus_next() % 8u) + 1u;
    uint32_t size = xorshift128plus_next() & ((1u << ((x / y) + 4)) - 1u);

    if (size < 1u)
    {
        //fprintf(stderr, "size increased from %u to 1\n", size);
        size = 1u;
    }
    else if (size > MAX_A85_SIZE)
    {
        //fprintf(stderr, "size decreased from %u to %u\n", size, MAX_A85_SIZE);
        size = MAX_A85_SIZE;
    }
    else
    {
        // ok
    }

    return (size);
}

static void tc_a85_random (lcut_tc_t *tc, void *data)
{
    uint8_t ibuf[MAX_A85_SIZE + 1u];
    uint8_t obuf[MAX_A85_SIZE + (MAX_A85_SIZE / 2u)];
    uint8_t dbuf[MAX_A85_SIZE + 1u];

    int count = 100000;

    uint64_t rand = 0u;

    (void )data;

    xorshift128plus_seed(123456789u);

    while (count--)
    {
        uint32_t isz = random_size();

        for (uint32_t i = 0u; i < isz; )
        {
            if ((i % 8u) == 0u)
            {
                rand = xorshift128plus_next();
            }
            else
            {
                rand >>= 8u;
            }
            ibuf[i++] = rand & 0xffu;
        }

        int32_t olen = encode_ascii85(ibuf, isz, obuf, MAX_A85_SIZE + (MAX_A85_SIZE / 2u));

        LCUT_TRUE(tc, olen >= 0);
        LCUT_TRUE(tc, (uint32_t )olen <= (((isz + 3) / 4) * 5));

        olen = decode_ascii85(obuf, olen, dbuf, MAX_A85_SIZE + 1u);

        if (olen < 0) printf("Err: %d isz: %d \n", olen, isz); else {}

        LCUT_TRUE(tc, olen >= 0);
        LCUT_TRUE(tc, (uint32_t )olen <= MAX_A85_SIZE);
        LCUT_TRUE(tc, (uint32_t )olen == isz);

        LCUT_TRUE(tc, (0 == memcmp(dbuf, ibuf, olen)));
    }
}

static int do_unit_test (void)
{
    lcut_t test;
    lcut_ts_t *suite = NULL;

    LCUT_TEST_BEGIN(&test, "Ascii85 Tests", NULL, NULL);

    LCUT_TS_INIT(&test, suite, "Ascii85 Tests", NULL, NULL);

    LCUT_TC_ADD(&test, suite, "Leviathan",     tc_encode_decode,     (void *)&tp0,   NULL, NULL);

    LCUT_TC_ADD(&test, suite, "HelloWorld 0",  tc_e_encode_decode,   (void *)&tps0,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 1",  tc_e_encode_decode,   (void *)&tps1,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 2",  tc_e_encode_decode,   (void *)&tps2,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 3",  tc_e_encode_decode,   (void *)&tps3,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 4",  tc_e_encode_decode,   (void *)&tps4,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 5",  tc_e_encode_decode,   (void *)&tps5,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 6",  tc_e_encode_decode,   (void *)&tps6,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 7",  tc_e_encode_decode,   (void *)&tps7,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 8",  tc_e_encode_decode,   (void *)&tps8,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 9",  tc_e_encode_decode,   (void *)&tps9,  NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 10", tc_e_encode_decode,   (void *)&tps10, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 11", tc_e_encode_decode,   (void *)&tps11, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "HelloWorld 12", tc_e_encode_decode,   (void *)&tps12, NULL, NULL);

    LCUT_TC_ADD(&test, suite, "Zeroes 0",      tc_e_encode_decode,   (void *)&tpz0 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 1",      tc_e_encode_decode,   (void *)&tpz1 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 2",      tc_e_encode_decode,   (void *)&tpz2 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 3",      tc_e_encode_decode,   (void *)&tpz3 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 4",      tc_e_encode_decode,   (void *)&tpz4 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 5",      tc_e_encode_decode,   (void *)&tpz5 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 6",      tc_e_encode_decode,   (void *)&tpz6 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 7",      tc_e_encode_decode,   (void *)&tpz7 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 8",      tc_e_encode_decode,   (void *)&tpz8 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 9",      tc_e_encode_decode,   (void *)&tpz9 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 10",     tc_e_encode_decode,   (void *)&tpz10, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 11",     tc_e_encode_decode,   (void *)&tpz11, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 12",     tc_e_encode_decode,   (void *)&tpz12, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 16",     tc_e_encode_decode,   (void *)&tpz16, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 20",     tc_e_encode_decode,   (void *)&tpz20, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 24",     tc_e_encode_decode,   (void *)&tpz24, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 28",     tc_e_encode_decode,   (void *)&tpz28, NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Zeroes 32",     tc_e_encode_decode,   (void *)&tpz32, NULL, NULL);

    LCUT_TC_ADD(&test, suite, "Binary 0",      tc_e_encode_decode,   (void *)&tpb0 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Binary 1",      tc_e_encode_decode,   (void *)&tpb1 , NULL, NULL);

    LCUT_TC_ADD(&test, suite, "Error bad char x",  tc_expect_error,  (void *)&tpe1 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Error bad char ~",  tc_expect_error,  (void *)&tpe2 , NULL, NULL);
    LCUT_TC_ADD(&test, suite, "Error overflow",    tc_expect_error,  (void *)&tpe3 , NULL, NULL);

    LCUT_TC_ADD(&test, suite, "Random data",       tc_a85_random,    (void *)NULL  , NULL, NULL);

//    LCUT_TC_ADD(&test, suite, "test random sorts", tc_random_sorts, NULL, tc_random_setup, NULL);

    LCUT_TS_ADD(&test, suite);

    LCUT_TEST_RUN(&test);
    LCUT_TEST_REPORT(&test);
    LCUT_TEST_END(&test);

    LCUT_TEST_RESULT(&test); // exits
    return 0; // not reached
}


static void usage (void)
{
    printf("usage: test [-u] [-i <string>] [-o <string>]\n");
    exit(EXIT_FAILURE);
}

// Examples:
// e$ ./test -i hello
// <~BOu!rDZ~>
// e$ ./test -i hello -o 'BOu!rDZ'
// ~Encode OK
// e$ ./test -o 'BOu!rDZ'
// ~Decoded: 68656c6c6f

int main (int argc, char **argv)
{
    int i = 1;
    bool unit_test = true;
    char *istr = NULL;
    char *ostr = NULL;

    while (i < argc)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            unit_test = false;
            i += 1;
            if (i < argc) istr = argv[i];
            else usage();
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            unit_test = false;
            i += 1;
            if (i < argc) ostr = argv[i];
            else usage();
        }
        else
        {
            //printf("arg %d: %s\n", i, argv[i]);
            break;
        }
        i += 1;
    }

    if (unit_test)
    {
        return do_unit_test();
    }
    else if (NULL != istr)
    {
        size_t isz = strlen(istr);
        uint8_t obuf[((isz + 3) / 4) * 5];
        int32_t olen = encode_ascii85((uint8_t *)istr, isz, obuf, sizeof(obuf));

        if (NULL != ostr)
        {
            size_t osz = strlen(ostr);

            if ((size_t )olen != osz)
            {
                printf("~Encode size mismatch, expected %zu got %d\n", osz, olen);
            }
            else if (0 != memcmp(obuf, ostr, olen))
            {
                printf("~Encode mismatch\n");
            }
            else
            {
                printf("~Encode OK\n");
            }
        }
        else
        {
            printf("<~");
            for (i = 0; i < olen; i++) printf("%c", (char )obuf[i]);
            printf("~>\n");
        }
    }
    else if (NULL != ostr)
    {
        // Not very useful perhaps since ostr is binary so hard to type at console

        size_t osz = strlen(ostr);
        uint8_t dbuf[((osz + 4) / 5) * 4];
        int32_t ilen = decode_ascii85((uint8_t *)ostr, osz, dbuf, sizeof(dbuf));

        if (ilen < 0)
        {
            printf("~Decode error %d\n", ilen);
        }
        else
        {
            printf("~Decoded: ");
            for (i = 0; i < ilen; i++) printf("%02x", dbuf[i]);
            printf("\n");
        }
    }
    else
    {
        usage();
    }
}


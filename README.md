# Ascii85

Ascii85 encoder/decoder written in C.

## Why?

There are many Ascii85 or Base85 implementations available. I needed something with three
features:

  - Complete and working implementation with unit tests
  - Whitespace not ignored (so, non-standard)
  - No memory allocation

The intended use is embedded microcontroller with limited memory and realtime constraints, for
ASCII encoding binary data to be sent/received over serial interfaces. Whitespace is used for 
message framing, so cannot be ignored. Encoding/decoding is done from a complete input buffer to
a complete output buffer.

There is a nice looking standard implementation at https://github.com/judsonx/base85 -- take a
look at that one if you don't share my constraints. It supports a stateful stream processing 
approach as opposed to my complete input buffer to complete output buffer approach.

## C API

~~~~
/*!
 * @brief encode_ascii85: encode binary input into Ascii85
 * @param[in] inp pointer to a buffer of unsigned bytes 
 * @param[in] in_length the number of bytes at inp to encode
 * @param[in] outp pointer to a buffer for the encoded data
 * @param[in] out_max_length available space at outp in bytes; must be >= 5 * ceiling(in_length/4)
 * @return number of bytes in the encoded value at outp if non-negative; error code from
 * ascii85_errs_e if negative
 * @par Possible errors include: ascii85_err_in_buf_too_large, ascii85_err_out_buf_too_small
 */
int32_t encode_ascii85 (const uint8_t *inp, int32_t in_length, uint8_t *outp, int32_t out_max_length);

/*!
 * @brief decode_ascii85: decode Ascii85 input to binary output
 * @param[in] inp pointer to a buffer of Ascii85 encoded unsigned bytes 
 * @param[in] in_length the number of bytes at inp to decode
 * @param[in] outp pointer to a buffer for the decoded data
 * @param[in] out_max_length available space at outp in bytes; must be >= 4 * ceiling(in_length/5)
 * @return number of bytes in the decoded value at outp if non-negative; error code from
 * ascii85_errs_e if negative
 * @par Possible errors include: ascii85_err_in_buf_too_large, ascii85_err_out_buf_too_small, 
 * ascii85_err_bad_decode_char, ascii85_err_decode_overflow
 */
int32_t decode_ascii85 (const uint8_t *inp, int32_t in_length, uint8_t *outp, int32_t out_max_length);
~~~~

## Building

A `Makefile` is included that may work for you. But you really only need: `ascii85.c` and `ascii85.h`

The Makefile targets are: `test` and `lint`.

## Tests

Run the unit tests with: `./test`

## CLI

The `./test` program can be used to encode and decode data in a limited way.

Encode example:

~~~~
  $ ./test -i hello
  <~BOu!rDZ~>
~~~~

Decode example:

~~~~
  $ ./test -o 'BOu!rDZ'
  ~Decoded: 68656c6c6f
~~~~

Quick test example:

~~~~
  $ ./test -i hello -o 'BOu!rDZ'
  ~Encode OK
~~~~

### License

MIT/X11

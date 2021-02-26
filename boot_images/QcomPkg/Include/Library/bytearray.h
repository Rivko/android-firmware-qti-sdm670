
#ifndef BYTEARRAY_H
#define BYTEARRAY_H

typedef struct bytearray_s bytearray_t;

bytearray_t *bytearray_create(unsigned buffer_size);
bytearray_t *bytearray_frombuffer(const void *bytes, unsigned buffer_size);
void bytearray_swapbuffer(bytearray_t *self, const void *bytes, unsigned buffer_size);
void bytearray_destroy(bytearray_t *self);

unsigned bytearray_size(bytearray_t *self);
unsigned bytearray_reserved(bytearray_t *self);
void bytearray_reserve(bytearray_t *self, unsigned minimum);

void bytearray_put(bytearray_t *self, const void *bytes, unsigned num_bytes);
void bytearray_clear(bytearray_t *self);

void *bytearray_at(bytearray_t *self, unsigned index);
void *bytearray_slice(bytearray_t *self, unsigned start, unsigned end);


#endif // BYTEARRAY_H


#include <stdint.h>

#ifndef BUFFERS_H_
#define BUFFERS_H_

typedef struct buffer_s {
    /* The size in bytes as read from socket */
    uint32_t size_bytes;

    /* Current position */
    uint8_t *buffer_current;

    /* The complete data */
    uint8_t *data;

    /* The message number as read from socket */
    uint32_t message_number;

    uint32_t message_id;

    struct buffer_s *previous;
    struct buffer_s *next;
} buffer_t;

typedef struct {
    buffer_t *head;
    buffer_t *tail;

    uint32_t count;
} buffer_list_t;

uint8_t buffer_get_uint8_t(buffer_t *buffer, uint32_t offset);

uint16_t buffer_get_uint16_t(buffer_t *buffer, uint32_t offset);

uint32_t buffer_get_uint32_t(buffer_t *buffer, uint32_t offset);

int buffer_fetch_bits(buffer_t *buffer, uint32_t offset, int nbits, uint32_t *value);

void buffer_dump(buffer_t *buffer, FILE *to);

int buffer_append_data(buffer_t *buffer, const uint8_t *data, const uint32_t length);

int buffer_new_from_data(buffer_t **buffer, uint8_t *data, const uint32_t length,
                         int data_static);

int buffer_has_enouch_data(buffer_t *buffer, uint32_t offset, uint32_t to_get);

int buffer_add_to_list(buffer_t *new_buf);

int buffer_get_from_mn(const uint32_t message_number, buffer_t **buffer);

#endif /* BUFFERS_H_ */
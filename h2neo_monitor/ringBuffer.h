/*
 * ringBuffer.h
 *
 *  Created on: Mar 30, 2021
 *      Author: ericj
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

typedef struct {
    uint8_t * const buffer;
    int head;
    int tail;
    const int max;
} ringBuf;

int ringBuf_push(ringBuf *c, uint8_t data);
int ringBuf_pop(ringBuf *c, uint8_t data);

#endif /* RINGBUFFER_H_ */

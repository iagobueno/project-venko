#ifndef MESAGE_H
#define MESSAGE_H

#define MAX_BUFFER_SIZE 256

typedef struct message_t {
    unsigned int id;                // seq number
    unsigned short int type;        // message type
    char buffer[MAX_BUFFER_SIZE];   // data buffer
} message_t;

#endif
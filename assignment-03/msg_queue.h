/* Richard Davis
 * CSCI 4100 Assignment 3
 * Header file for message queue structures and functions
 */

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#include <pthread.h>

#define QUEUE_SIZE 10

/* Structure for a message */
struct mq_msg {
  int t_id; // sender thread id
  int m_id; // message id
};

/* Structure for a message queue */
struct msg_queue {
  struct mq_msg messages[QUEUE_SIZE];
  unsigned int in; // number of messages sent
  unsigned int out; // number of messages received
  pthread_mutex_t lock;
  pthread_cond_t msg_added;
  pthread_cond_t msg_removed;
};


/* Initialize the message queue */
void mq_init(struct msg_queue * mq);

/* Send a message to a message queue */
void mq_send(struct msg_queue * mq, struct mq_msg * message);

/* Receive a message from a message queue */
void mq_receive(struct msg_queue * mq, struct mq_msg * message);


#endif

/* Richard Davis
 * CSCI 4100 Assignment 3
 * Source file for message queue implementation
 */

#include <stdio.h>
#include "msg_queue.h"

/* Initialize the message queue */
void mq_init(struct msg_queue * mq) {
  // initialize in and out to 0
  mq->in = 0;
  mq->out = 0;
  pthread_mutex_init(&mq->lock, NULL);
  pthread_cond_init(&mq->msg_added, NULL);
  pthread_cond_init(&mq->msg_removed, NULL);
}

/* Send a message to a message queue */
void mq_send(struct msg_queue * mq, struct mq_msg * message) {
  // acquire the lock
  pthread_mutex_lock(&mq->lock);

  // while queue is full, wait on msg_removed condition variable
  while(mq->in - mq->out == 10)
    pthread_cond_wait(&mq->msg_removed, &mq->lock);

  // copy the message from message to correct location in the queue
  mq->messages[mq->in - mq->out] = *message;

  // increment the queue's `in` member variable
  mq->in++;

  // signal the queue's msg_added condition variable
  pthread_cond_signal(&mq->msg_added);

  // release the lock
  pthread_mutex_unlock(&mq->lock);
}

/* Receive a message from a message queue */
void mq_receive(struct msg_queue *mq, struct mq_msg * message) {
  // acquire the lock
  pthread_mutex_lock(&mq->lock);

  // while queue is empty, wait on msg_added condition variable
  while(mq->in - mq->out == 0)
    pthread_cond_wait(&mq->msg_added, &mq->lock);

  // copy the message from location in queue to mq_msg struct
  message->m_id = mq->messages[mq->in - mq->out].m_id;
  message->t_id = mq->messages[mq->in - mq->out].t_id;

  // increment the queue's out member variable
  mq->out++;

  // signal the queue's msg_removed condition variable
  pthread_cond_signal(&mq->msg_removed);

  // release the lock
  pthread_mutex_unlock(&mq->lock);
}


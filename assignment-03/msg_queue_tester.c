/* Richard Davis
 * CSCI 4100 Assignment 3
 * This program tests a message queue by creating multiple sending and
 * receiving threads
 */

#include <stdio.h>
#include <stdlib.h>
#include "msg_queue.h"

/* Structure to hold thread arguments */
struct thread_args {
  int t_id;
  int num_msgs;
  struct msg_queue * mq;
};

/* Function prototypes */
void thread_args_init(struct thread_args *args, int t_id, int num_msgs,
		      struct msg_queue * mq);
void * send_msgs(void * args);
void * receive_msgs(void * args);

/* Main function */
int main(int argc, char *argv[]) {
  pthread_t t_send_1;
  pthread_t t_send_2;
  pthread_t t_send_3;

  pthread_t t_recv_1;
  pthread_t t_recv_2;
  pthread_t t_recv_3;
  pthread_t t_recv_4;

  struct msg_queue queue;
  mq_init(&queue);

  struct thread_args args_send_1;
  thread_args_init(&args_send_1, 1, 8, &queue);
  struct thread_args args_send_2;
  thread_args_init(&args_send_2, 2, 8, &queue);
  struct thread_args args_send_3;
  thread_args_init(&args_send_3, 3, 8, &queue);

  struct thread_args args_recv_1;
  thread_args_init(&args_recv_1, 1, 6, &queue);
  struct thread_args args_recv_2;
  thread_args_init(&args_recv_2, 2, 6, &queue);
  struct thread_args args_recv_3;
  thread_args_init(&args_recv_3, 3, 6, &queue);
  struct thread_args args_recv_4;
  thread_args_init(&args_recv_4, 4, 6, &queue);

  pthread_create(&t_send_1, NULL, send_msgs, &args_send_1);
  pthread_create(&t_send_2, NULL, send_msgs, &args_send_2);
  pthread_create(&t_send_3, NULL, send_msgs, &args_send_3);

  pthread_create(&t_recv_1, NULL, receive_msgs, &args_recv_1);
  pthread_create(&t_recv_2, NULL, receive_msgs, &args_recv_2);
  pthread_create(&t_recv_3, NULL, receive_msgs, &args_recv_3);
  pthread_create(&t_recv_4, NULL, receive_msgs, &args_recv_4);

  pthread_join(t_send_1, NULL);
  pthread_join(t_send_2, NULL);
  pthread_join(t_send_3, NULL);

  pthread_join(t_recv_1, NULL);
  pthread_join(t_recv_2, NULL);
  pthread_join(t_recv_3, NULL);
  pthread_join(t_recv_4, NULL);

  return 0;
}

/* Initialize thread_args structure */
void thread_args_init(struct thread_args *args, int t_id, int num_msgs,
		      struct msg_queue * mq) {
  args->t_id = t_id;
  args->num_msgs = num_msgs;
  args->mq = mq;
}

/* Function for sending thread to execute */
void * send_msgs(void * args) {
  // cast void pointer args to a pointer to a thread_args structure
  struct thread_args * real_args = (struct thread_args *) args;

  // declare a mq_msg structure
  struct mq_msg message;

  // send number of messages specified in the thread_args struct by...
  // - initialize message with thread id (t_id) and message id (m_id)
  // - send message to the queue in thread_args using mq_send
  for(int i = 0; i < real_args->num_msgs; i++) {
    message.t_id = real_args->t_id;
    message.m_id = i;
    mq_send(real_args->mq, &message);
  }

  // return NULL
  return NULL;
}

/* Function for receiving thread to execute */
void * receive_msgs(void * args) {
  // cast void pointer args to pointer to a thread_args structure
  struct thread_args * real_args = (struct thread_args *) args;

  // declare a mq_msg structure
  struct mq_msg message;

  // receive the number of message specified in thread_args struct by...
  // - receive message from queue in thread_args struct using mq_receive
  // - display the message along with the receiver thread id from thread_args struct
  for(int i = 0; i < real_args->num_msgs; i++) {
    mq_receive(real_args->mq, &message);
    printf("[sender: %d, msg id: %d, receiver: %d]\n", message.t_id, message.m_id, real_args->t_id);
  }

  // return NULL
  return NULL;
}

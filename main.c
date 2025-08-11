#include <stdio.h>
#include "tx_api.h"

#define THREAD_STACK_SIZE      1024
#define QUEUE_MESSAGE_COUNT    8

TX_THREAD sender_thread;
TX_THREAD receiver_thread;
ULONG sender_stack[THREAD_STACK_SIZE / sizeof(ULONG)];
ULONG receiver_stack[THREAD_STACK_SIZE / sizeof(ULONG)];

TX_QUEUE my_queue;
ULONG queue_area[QUEUE_MESSAGE_COUNT];

void sender_entry(ULONG thread_input)
{
    (void)thread_input;
    ULONG value = 0;
    while (1)
    {
        value++;
        tx_queue_send(&my_queue, &value, TX_NO_WAIT);
        tx_thread_sleep(50);
    }
}

void receiver_entry(ULONG thread_input)
{
    (void)thread_input;
    ULONG recv_value;
    while (1)
    {
        if (tx_queue_receive(&my_queue, &recv_value, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            printf("Received: %lu\n", recv_value);
        }
    }
}

void tx_application_define(void* first_unused_memory)
{
    tx_queue_create(&my_queue, "MyQueue", sizeof(ULONG),
        queue_area, sizeof(queue_area));

    tx_thread_create(&receiver_thread, "Receiver", receiver_entry, 0,
        receiver_stack, sizeof(receiver_stack),
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&sender_thread, "Sender", sender_entry, 0,
        sender_stack, sizeof(sender_stack),
        2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(void)
{
    tx_kernel_enter();
    return 0;
}

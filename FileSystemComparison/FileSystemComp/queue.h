#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct QNode
{
    char* key;
    struct QNode *next;
}QNode;
 
typedef struct Queue
{
    struct QNode *front, *rear;
}Queue;

QNode * newNode(char* k);
Queue * createQueue();
void enQueue(Queue *q, char* k);
QNode *deQueue(Queue *q);


#endif
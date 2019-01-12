#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define REGISTER 1
#define GET_TOPICS 2
#define RETURN_TOPICS 3

#define SUBSCRIBE 4

#define ERORR_MESSAGE 6
#define NEW_TOPIC 7



#define QUEUE_ID 123456
#define SIZE_OF_MESSAGE 256
#define MAX_SIZE_OF_NAME 30
#define MAX_TOPICS 20
#define MAX_SUBCRIBERS 30
#define MAX_CLIENTS 100
#define MAX_TOPICS_PER_CLIENT 20



#define TRUE 1
#define FALSE 0

typedef struct ClientData{
    pid_t proces_id;
    char name[MAX_SIZE_OF_NAME];
    key_t private_queue_key;
    
}ClientData;


typedef struct MessageBuffer{
    long mtype;
    char message[SIZE_OF_MESSAGE];
    ClientData client_data;

}MessageBuffer;

typedef struct SimpleMessageBuffer{
    long mtype;
    char message[SIZE_OF_MESSAGE];

}SimpleMessageBuffer;

typedef struct Topic{
    char topic_name[MAX_SIZE_OF_NAME];
    long mtype;
}Topic;


typedef struct SendTopic{
    long mtype;
    Topic available_topics[MAX_TOPICS];
    int acutal_topics_size;
}SendTopic;


char vchar();

void print_error();
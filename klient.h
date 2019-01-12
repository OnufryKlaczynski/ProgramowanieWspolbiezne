#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "utils.h"


int send_login(int queue_id);


void add_new_topic(int server_queue_id, int client_queue_id);
void register_topic(int queue_id, char topic_name[]);
void did_register_topic(int queue_id, char topic_name[]);



void subscribe_topic(int server_queue_id, int client_queue_id);
int request_and_get_aveiable_topics(int server_id, int client_id, Topic aveiable_topics[]);
int get_aveiable_topics(int client_id, Topic topics[]);
void request_aveiable_topics(int quque_id);

void send_new_message(int queue_id);
void receive_messages(int client_queue_id);
long choose_topic();



int find_topic_by_mtype(long mtype);
int find_topic_by_name(char name[]);
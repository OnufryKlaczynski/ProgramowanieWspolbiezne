#include "utils.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAXBYTES 80

int send_login(int queue_id);

void add_new_topic(int server_queue_id, int client_queue_id);

void register_topic(int queue_id, char topic_name[]);

void did_register_topic(int queue_id);

void send_new_message(int queue_id);

void subscribe_topic(int server_queue_id, int client_queue_id);

int request_and_get_aveiable_topics(int server_id, int client_id, Topic aveiable_topics[]);

int get_aveiable_topics(int client_id, Topic topics[]);

void request_aveiable_topics(int quque_id);

void receive_messages(int client_queue_id);

long choose_topic();

long choose_topic_for_reading_messages();

int find_topic_by_mtype(long mtype);

int find_topic_by_name(char name[]);

int already_subscribing_topic(char* name);
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

void register_topic(int queue_id);

void send_new_message(int queue_id);

void get_aveiable_topics(int queue_id);

void subscribe_topic(int queue_id);

void receive_messages(int client_queue_id);

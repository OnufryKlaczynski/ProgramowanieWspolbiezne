#include "server.h"

static TopicWithClients topics[MAX_TOPICS];
static int count_topics = 0;
static ClientData clients[MAX_CLIENTS];
static int last_client_index = 0;

int main()
{
	
	int queue_id = msgget(QUEUE_ID, IPC_CREAT | 0666);
	if(queue_id == -1){
		printf("%s\n", strerror(errno));
		return 0;
	}


	while(TRUE){
		MessageBuffer message_buffer;
		size_t size = sizeof(MessageBuffer) - sizeof(long);
		int did_receive = msgrcv(queue_id, &message_buffer, size, 0, 0);
		if(did_receive == -1){
			printf("%s\n", strerror(errno));
			return -1;
		}

		long mtype = message_buffer.mtype;
		if(mtype == REGISTER){
			log_in_client(message_buffer);
		}
		else if(mtype == NEW_TOPIC){
			register_new_topic(message_buffer);
		}
		else if(mtype == SUBSCRIBE){
			subscribe_topic(message_buffer);
			
			  
		}
		else if(mtype == RETURN_TOPICS){
			send_available_topics(queue_id);
		}
		else if(mtype > NEW_TOPIC){
			register_new_message(message_buffer);
			send_message_to_clients(message_buffer);
		}
		
	}

	return 0;
}

int log_in_client(MessageBuffer message_buffer){

	if(search_client(message_buffer.client_data) != -1){
		return -1;
	}else{
		clients[last_client_index] = message_buffer.client_data;
		last_client_index +=1;
		printf("I registered user with process id=%d and name=%s\n", message_buffer.client_data.proces_id, message_buffer.client_data.name);
		return 1;
	}
	
}

int search_client(ClientData client){
	size_t size = sizeof(clients)/sizeof(ClientData);
	for(int i=0; i<size; i++){
		if( strcmp(clients[0].name, client.name) == 0){
			return i;
		}
	}
	return -1;
}

int register_new_topic(MessageBuffer message_buffer){
	
	printf("I Registerd new topic %s\n", message_buffer.message);
	TopicWithClients data;
	Topic topic;
	topic.mtype =  NEW_TOPIC + 1 + count_topics;
	strcpy(topic.topic_name ,message_buffer.message);
	data.topic = topic;
	data.size_of_clients = 0;
	topics[count_topics] = data;
	count_topics += 1;

	return 1;
	
}

int subscribe_topic(MessageBuffer message_buffer){
   
	for(int i = 0; i<=count_topics; i++){
		if(strcmp(topics[i].topic.topic_name, message_buffer.message) == 0){
			int index= topics[i].size_of_clients;
			topics[i].clients[index] = message_buffer.client_data;
			topics[i].size_of_clients += 1;
			printf("Client %s subscribed topic %s\n", message_buffer.client_data.name, message_buffer.message);
			return 1;
		}
	}    
	return -1;
}


int register_new_message(MessageBuffer message_buffer){
	
	printf("I Registerd new message %s\nfrom topic %ld\n", message_buffer.message, message_buffer.mtype);
	return 1;
}

int send_message_to_clients(MessageBuffer message_received){
	printf("I send message to clients %s\n", message_received.message);
	int index = message_received.mtype - (NEW_TOPIC +1);
	int size_of_clients = topics[index].size_of_clients;
	SimpleMessageBuffer message_to_send;
	message_to_send.mtype = message_received.mtype;
	int size = sizeof(SimpleMessageBuffer) - sizeof(long);
	printf("%d halo\n", size);

	strcpy(message_to_send.message, message_received.message);
	for(int i=0; i<size_of_clients;i++){
		
		int queue_id = msgget(topics[index].clients[i].private_queue_key, 0666);
		int did_send = msgsnd(queue_id, &message_to_send, size, IPC_NOWAIT);
		if(did_send == -1){
			print_error();
			return -1;
		}else{
			printf("I Sended message\n");
		}
	}
	return 1;
	
}

int send_available_topics(int queue_id){
	SendTopic message_buffer;
	message_buffer.mtype = RETURN_TOPICS;
	for(int i=0; i<count_topics; i++){
		message_buffer.available_topics[i] = topics[i].topic;

	}
	message_buffer.acutal_topics_size = count_topics;
	size_t size = sizeof(message_buffer) - sizeof(long);
	int did_send = msgsnd(queue_id, &message_buffer, size, 0);
	if(did_send == -1){
		printf("%s\n", strerror(errno));
	}
}



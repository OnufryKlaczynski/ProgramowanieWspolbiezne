#include "klient.h"





static ClientData my_client_data;
static char topics_names[MAX_TOPICS];



int main()
{   
    int server_queue_id = msgget(QUEUE_ID, 0666);
    if(server_queue_id == -1){
        printf("%s\n", strerror(errno));
        return 0;
    }
    size_t path_size = 50;
    char current_path[path_size];
    getcwd(current_path, path_size);

    pid_t process_id = getpid();
    strcpy(my_client_data.name, "Mateusz");
    my_client_data.proces_id = process_id;

    key_t key = ftok(current_path, my_client_data.proces_id);
    my_client_data.private_queue_key = key;

    int client_queue_id = msgget(key, IPC_CREAT | 0666);
    if(client_queue_id == -1){
        printf("%s\n", strerror(errno));
        return 0;
    }

    printf("Tu pogram klienta\n");
    do{
        char option;
        char* menu = "a - Zarejesturj sie\ns - Dodaj nowy temat\nd - Wyślij nową wiadomosć\nf - Zasubskrybuj temat\n";
        printf("Wybierz opcje programu\n%s", menu);
        option = vchar();
        switch(option){
            case 'a':
                send_login(server_queue_id);
                break;
            case 's':
                register_topic(server_queue_id);
                break;
            case 'd':
                send_new_message(server_queue_id);
                
                receive_messages(client_queue_id);
                break;
            case 'f':
                subscribe_topic(server_queue_id);
                break;
            case 'z':
                break; 
            default:
                printf("Podano zla opcje, spróbuj jeszcze raz\n");
                break;
        }

        
    }while(1);

    return 0;
}

int send_login(int queue_id){
    printf("Rejestracja\n");
    
   
   

    MessageBuffer message_buffer;
    size_t size_of_client_data = sizeof(my_client_data) + sizeof(message_buffer.message);
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = REGISTER;

    printf("I send user with process id=%d and name=%s\n", message_buffer.client_data.proces_id, message_buffer.client_data.name);

    int did_send = msgsnd(queue_id, &message_buffer, size_of_client_data, 0);
    if(did_send == -1){
        perror("Ola");

        return -1;
    }
    
    return 1;
}

void register_topic(int queue_id){
    
    char topic_name[MAX_SIZE_OF_NAME] = "Wiadomosci";
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = NEW_TOPIC;
    strcpy(message_buffer.message, topic_name);
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       printf("%s\n", strerror(errno));
    }
}

void send_new_message(int queue_id){
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = NEW_TOPIC+1;
    strcpy(message_buffer.message, "Trump zdominowal swiat!");
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       print_error();
    }else{
        printf("Wyslalem wiadomosc %s\n",message_buffer.message );
    }
}


void get_aveiable_topics(int queue_id){
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = GET_TOPICS;
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       printf("%s \n", strerror(errno));
    }
}

void subscribe_topic(int queue_id){
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = SUBSCRIBE;
    strcpy(message_buffer.message, "Wiadomosci");
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       printf("%s \n", strerror(errno));
    }
}

void receive_messages(int client_queue_id){
    SimpleMessageBuffer message_bufferer;
  
    size_t size = sizeof(SimpleMessageBuffer) - sizeof(long);
    int did_receive = msgrcv(client_queue_id, &message_bufferer, size, NEW_TOPIC+1, IPC_NOWAIT);
    if(did_receive == -1){
        printf("%s\n", strerror(errno)); 
    }else{
        printf("Dostales wiadomosc\n");
        printf("%s\n", message_bufferer.message);
    }
}
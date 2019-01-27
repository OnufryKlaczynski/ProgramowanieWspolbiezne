#include "klient.h"

static ClientData my_client_data;
static Topic topics[MAX_TOPICS];
static int topics_size = 0 ;
static int unread_topics[MAX_TOPICS] = {0};

int main()
{   
    system("clear");
    fd_set readfds;
    int    num_readable;
    struct timeval tv;
    int    fd_stdin;
    fd_stdin = fileno(stdin);


    int server_queue_id = msgget(QUEUE_ID, 0666);
    if(server_queue_id == -1){
        perror("");
        return 0;
    }
    size_t path_size = 50;
    char current_path[path_size];
    getcwd(current_path, path_size);

    pid_t process_id = getpid();
    printf("Podaj swoja nazwe\n");
    scanf("%s", my_client_data.name);

    my_client_data.proces_id = process_id;

    key_t key = ftok(current_path, my_client_data.proces_id);
    my_client_data.private_queue_key = key;
    
    int client_queue_id = msgget(key, IPC_CREAT | 0666);
    if(client_queue_id == -1){
        perror("");
        return 0;
    }


    send_login(server_queue_id);
    

    do{
        FD_ZERO(&readfds);
        FD_SET(fileno(stdin), &readfds);

        tv.tv_sec = 4;
        tv.tv_usec = 0;

        
        char option;
        char* menu = "\nq - Odbierz wiadomosc\ns - Dodaj nowy temat\n\
d - Wyślij nową wiadomosć\nf - Zasubskrybuj temat\n";
        printf("Wybierz opcje programu:\n%s", menu);
        fflush(stdout);
        num_readable = select(fd_stdin + 1, &readfds, NULL, NULL, &tv);
        if(num_readable > 0){
    
            option = vchar();
            switch(option){
                case 'q':
                    receive_messages(client_queue_id);
                    break;
                case 'a':
                    break;
                case 's':
                    add_new_topic(server_queue_id, client_queue_id);
                    break;
                case 'd':    
                    send_new_message(server_queue_id);
                    break;
                case 'f':
                    subscribe_topic(server_queue_id, client_queue_id);
                    break;
                case 'z':
                    break; 
                default:
                    printf("Podano zla opcje, spróbuj jeszcze raz\n");
                    break;
            }
        }
        else if(num_readable == -1){
            perror("");
        }
        else if(num_readable == 0){
            printf("Sprawdzam czy są jakieś wiadomości\n");
            int received_message = 0;
            
            for(int i=0; i<=topics_size; i++){
                if(topics[i].notify == 1){
                    //TODO: check if it works; lolki falosk
                    int did_rcv = msgrcv(client_queue_id, NULL,0, topics[i].mtype , IPC_NOWAIT);
                    if(did_rcv == -1){
                        if(errno ==  E2BIG){
                            received_message = 1;
                            unread_topics[i] = 1;
                            continue;
                        }
                    }
                }
            }
            system("clear");
            if(received_message == 0){

            }else{
                printf("Masz nieprzeczytane wiadomości\n");
            }
            
        }

        
    }while(1);

    return 0;
}

int send_login(int queue_id){
    MessageBuffer message_buffer;
    size_t size_of_client_data = sizeof(my_client_data) + sizeof(message_buffer.message);
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = REGISTER;

    int did_send = msgsnd(queue_id, &message_buffer, size_of_client_data, 0);
    if(did_send == -1){
        print_error();

        return -1;
    }
    system("clear");
    printf("Rejestracja przebiegła pomyślnie\n");
    return 1;
}


void add_new_topic(int server_queue_id, int client_queue_id){
    system("clear");
    char topic_name[MAX_SIZE_OF_NAME];
    printf("Podaj nazwe tematu ktory chcesz zarejestrowac: ");
    fflush(stdout);
    scanf("%s", topic_name);
    register_topic(server_queue_id, topic_name);
    did_register_topic(client_queue_id);
}


void register_topic(int queue_id, char topic_name[]){
    
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = NEW_TOPIC;
    strcpy(message_buffer.message, topic_name);
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       perror("");
    }
}

void did_register_topic(int queue_id){

    MessageBuffer message_buffer;
    message_buffer.mtype = NEW_TOPIC;
    int size = sizeof(message_buffer) - sizeof(long);
    int did_register = msgrcv(queue_id, &message_buffer, size, NEW_TOPIC, 0);
    
    system("clear");
    if(did_register == -1){
       printf("%s\n", strerror(errno));
    }
    else{
        if( strcmp(message_buffer.message, "OK")  == 0){
            printf("Udało sie zarejestrowac temat\n");
        }else{
            printf("Podany temat istnieje na liscie tematów\n");
        }
    }
}


void send_new_message(int queue_id){
    system("clear");
    if(topics_size == 0){
        char * error_message = "Nie subskrybujesz żadnych tematów\n\
By moc wysylac wiadomosci zasubskrybuj najpierw jakiś temat\n";
       puts(error_message);
       return;
    }
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = choose_topic();

    printf("Podaj tresc wiadomosci\n");
    fflush(stdout);
    char c;
    while ( (c = getchar()) != EOF && c != '\n');
    fgets(message_buffer.message, SIZE_OF_MESSAGE, stdin);
    
    size_t size = sizeof(MessageBuffer) - sizeof(long);
    int did_send = msgsnd(queue_id, &message_buffer, size, 0);
    if(did_send == -1){
       print_error();
    }else{
        system("clear");
        printf("Udało się wysłać wiadomość\n");
    }
}



int request_and_get_aveiable_topics(int server_id, int client_id, Topic aveiable_topics[]){
    request_aveiable_topics(server_id);
    int topic_size = get_aveiable_topics(client_id, aveiable_topics);
    if(topic_size == -1){
        return -1;
    }
    return topic_size;
    
}

void request_aveiable_topics(int quque_id){
    MessageBuffer message_buffer;
    message_buffer.client_data = my_client_data;
    message_buffer.mtype = GET_TOPICS;
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgsnd(quque_id, &message_buffer, size, 0);
    if(did_send == -1){
        printf("%s \n", strerror(errno));
        return;
    }
}

int get_aveiable_topics(int client_id, Topic topics[]){
    SendTopic message_buffer;
    message_buffer.mtype = GET_TOPICS;
    int size = sizeof(message_buffer) - sizeof(long);
    int did_send = msgrcv(client_id, &message_buffer, size, GET_TOPICS, 0);
    if(did_send == -1){
        printf("%s \n", strerror(errno));
        return -1;
    }
    for(int i=0; i<message_buffer.acutal_topics_size; i++){
        topics[i] = message_buffer.available_topics[i];
    }
    return message_buffer.acutal_topics_size;
}

void subscribe_topic(int server_queue_id, int client_queue_id){
    system("clear");
    Topic aveiable_topics[MAX_TOPICS];
    int aviable_topics_size = request_and_get_aveiable_topics(server_queue_id, client_queue_id, aveiable_topics);
    char option;
    char char_notify;
    int notify;
    int i_to_j[aviable_topics_size];
    int one_topic = -1;
    do{ 
        int j=0;
        for(int i=0; i<aviable_topics_size; i++){
            if(already_subscribing_topic(aveiable_topics[i].topic_name) == -1){
                printf("%d - %s\n", j, aveiable_topics[i].topic_name);
                i_to_j[j] = i;
                j++;
                one_topic = 1;
            }
        }
        if(one_topic == -1){
            system("clear");
            printf("Nie ma dostępnych tematów do subskrypcji\n");
        }
        printf("Podaj temat który chcesz zaskubskrybować lub 'z' jeśli chcesz wyjsc\n");
        option = vchar();
        if(option == 'z'){
            break;
        }else{
            printf("Chcesz otrzymywać powiadomienia odnosnie wiadomosci?\n");
            printf("Kliknij y--jeśli tak, n--jeśli nie");
            char_notify = vchar();
            system("clear");
        }
        int index = option - '0';
        index = i_to_j[index];
        MessageBuffer message_buffer;
        message_buffer.client_data = my_client_data;
        if(char_notify == 'y'){
            notify = 1;
        }else{
            notify = 0;
        }

        //TODO: do specified struct for this shit XD
        //TODO: przecież to jest niepotrzebne
        
        message_buffer.mtype = SUBSCRIBE;
        strcpy(message_buffer.message, aveiable_topics[index].topic_name);
        int size = sizeof(MessageBuffer) - sizeof(long);
        int did_send = msgsnd(server_queue_id, &message_buffer, size, 0);
    
        if(did_send == -1){
            perror("");
        }
        aveiable_topics[index].notify = notify;
        topics[topics_size] = aveiable_topics[index];
        unread_topics[topics_size] = 0;
        topics_size++;

        
        
    }while(option != 'z');
}


int already_subscribing_topic(char* name){
    for(int i=0; i<topics_size ;i++){
        if(strcmp(topics[i].topic_name, name) ==0 ){
            return 1;
        }
    }
    return -1;
}


void receive_messages(int client_queue_id){
    long mtype;
    do{
        mtype = choose_topic_for_reading_messages();
        if(mtype == -1){
            return;
        }
        
        SimpleMessageBuffer message_bufferer;
        size_t size = sizeof(SimpleMessageBuffer) - sizeof(long);
        int did_receive = msgrcv(client_queue_id, &message_bufferer, size, mtype, IPC_NOWAIT);
        if(did_receive == -1){
            perror("");
            
        }else{
    
            printf("%s\n", message_bufferer.message);  
        }

    }while(mtype != -1);
    
}


long choose_topic_for_reading_messages(){
    system("clear");
    char option;
    int int_option;
    if(topics_size == 0){
        printf("Nie subskrybujesz żadnych tematów\n");
        return -1;
    }
    do{
        
        for(int i=0; i<topics_size; i++){
            if(topics[i].notify == 1){
                if(unread_topics[i] == 1){
                    printf("%d. %s[Masz nieprzeczytaną wiadomość]\n", i+1, topics[i].topic_name);
                }else{
                    printf("%d. %s\n", i+1, topics[i].topic_name);

                }
            }else{
                printf("%d. %s\n", i+1, topics[i].topic_name);
            }
        }
        printf("Wybierz temat dla którego chcesz odczytać wiadomości lub z aby powrócić do menu\n");
        

        option = vchar();
        if(option == 'z'){
            return -1;
        }
        int_option = option - '0';
        int_option --;
        unread_topics[int_option] = 0;
    }while( !(int_option >= 0 && int_option <= topics_size) );
    long mtype = topics[int_option].mtype;
    return mtype;
}


long choose_topic(){
    char option;
    int int_option;
    do{
        
        for(int i=0; i<topics_size; i++){
            printf("%d. %s\n", i+1, topics[i].topic_name);
        }
        printf("Wybierz temat dla ktorego chcesz oglosic wiadomosc\n");
        

        option = vchar();
        if(option == 'z'){
            return -1;
        }
        int_option = option - '0';
        int_option --;
        
    }while( !(int_option >= 0 && int_option <= topics_size) );
    long mtype = topics[int_option].mtype;
    return mtype;
}

int find_topic_by_mtype(long mtype){
    for(int i=0; i<topics_size; i++){
        if(topics[i].mtype == mtype){
            return i;
        }
    }
    return -1;
}

int find_topic_by_name(char name[]){
    for(int i=0; i<topics_size; i++){
        if(strcmp(topics[i].topic_name, name) == 0){
            return i;
        }
    }
    return -1;
}
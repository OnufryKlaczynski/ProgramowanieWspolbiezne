#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "utils.h"


typedef struct TopicWithClients{
    
    Topic topic;
    ClientData clients[MAX_SUBCRIBERS];
    int size_of_clients;
}TopicWithClients;


/* Funkcja poszukuje klienta w głownym zbiorze
 * 
 *  returns: indeks klienta jeśli go znajdzie
 *           -1 jeśli go nie ma         
 */
int search_client(ClientData client);



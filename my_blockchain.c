#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct block {
    char*  bid;
    struct block* next;
} st_block;

typedef struct node {
    int nid;
    struct block* first;
    struct node* next;
} st_node;

int main() {
    st_node* blockchain = NULL;
    char synchronization = 's';
    int nb_node = 0;
    printf("No Backup Found: Starting New Blockchain\n");
    while(1) {
        printf("[%c%d]> ", synchronization, nb_node);
        char* command = read_command();
        if(command == NULL)
            break;
        
        if(compare_string(command, "quit", 0, 4) && command[4] == '\0') {
            free(command);
            break;
        }

        int result = execute_command(command, &blockchain);
        if (result == 6) {
            printf("nok: command not found\n");
        }
        free(command);
    }
    
    return 0;
}

int execute_command(char* command, st_node** blockchain) {
    if (compare_string(command, "add ", 0, 4)) {
        return command_add(command, blockchain);
    }
    else if (compare_string(command, "rm ", 0, 3)) {
        return command_rm();
    }
    else if (compare_string(command, "ls", 0, 2)) {
        return command_ls();
    }
    else if (compare_string(command, "sync", 0, 4) && command[4] == '\0') {
        return command_sync();
    }
    else {
        return 1;
    }
}

/*
COMMANDS
*/


//FUNCTIONS TO COMMAND ADD

int command_add(char* command, st_node** blockchain) {
    if (compare_string(command, "node ", 4, 9) && count_string(command) >= 10) {
        int existing_node = 0, value_nid = 0;
        if(found_node(command, blockchain, &existing_node, &valud_nid))
            return 6;
        if(existing_node) {
            printf("this node already exists\n");
            return 2;
        }
        return fill_node(blockchain, value_nid);
    } else if (compare_string(command, "block ", 4, 10) && count_string(command) >= 11) {
        int value_bid = 0;
        for (int i = 10; i < count_string(command); i++) {
            if(command[i] == ' ')
                break
            nb = nb * 10 + (command[i] - '0');
        }
        int






    } else {
        return 6;
    }
    return 0;
}

int found_node(char* command, st_node** blockchain, int* existing_node, int* value_nid) {
    *value_nid = 0;
    for (int i = 9; i < count_string(command); i++) {
        if(command[i] < '0' || command[i] > '9')
            return 6;
        *value_nid = *value_nid * 10 + (command[i] - '0');
    }
    *existing_node = 0;
    st_node* node = *blockchain;

    while(node != NULL) {
        if(node->nid == *value_nid)
            *existing_node = 1;
        node = node->next;
    }
    return 0;
}

int fill_node(st_node** blockchain, value_nid) {
    st_node* new_node = malloc(sizeof(st_node));
    if(new_node == NULL) {
        printf("nok: no more resources available on the computer\n");
        return 1;
    }
    new_node->nid = value_nid;
    new_node->first = NULL;
    new_node->next = NULL;
    if(*blockchain == NULL)
        *blockchain = new_node;
    else {
        st_node* node = *blockchain;
        while(node->next != NULL)
            node = node->next;
        node->next = new_node;
    }
    return 0;
}



int command_rm() {
    if (compare_string(command, "node ", 3, 8)) {

    } else if (compare_string(command, "block ", 3, 9)) {

    } else {
        return 1;
    }
    return 0;
}

int command_ls() {
    if(compare_string(command, " -l", 2, 5) && command[5] == '\0') {

    } else if (command[2] == '\0') {

    } else {
        return 1;
    }
    return 0;
}

int command_sync() {

    return 0;
}

/*
FUNCTIONS
*/

char* read_command(void) {
    char buffer[100];
    int reading= read(0, buffer, 100);
    if (reading <= 0) {
        return NULL;
    }
    int size_malloc = reading;
    if (buffer[reading - 1] == '\n') {
        size_malloc = reading - 1;
    }
    return fill_new_string(buffer, size_malloc);
}

char* fill_new_string(char* string2, int size) {
    char* string1 = malloc(sizeof(char) * (size + 1));
    if(string1 == NULL)
        return NULL;
    for(int i = 0; i < size; i++) {
        string1[i] = string2[i];
    }
    string1[size] = '\0';
    return string1;
}

int compare_string(char* string1, char* string2, int start, int end) {
    if(end - start != count_string(string2))
        return 0;
    for (int i = start; i < end; i++) {
        if(string1[i] != string2[i - start]) {
            return 0;
        }
    }
    return 1;
}

int count_string(char* name) {
    int i = 0, count = 0;
    while (name[i] != '\0') {
        count++;
        i++;
    }
    return count;
}
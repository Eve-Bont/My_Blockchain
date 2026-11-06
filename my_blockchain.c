#include <stdlib.h>
#include <unistd.h>

struct block {
    char*  bid;
    struct block* next;
};
struct node {
    int nid;
    struct block* first;
    struct node* next;
};

int main() {
    struct node* blockchain = NULL;
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

        if(execute_command(command, &blockchain)) {
            printf("nok: command not found\n");
        }
        free(command);
    }
    
    return 0;
}

int execute_command(char* command, struct node** blockchain) {
    if (compare_string(command, "add ", 0, 4)) {
        return command_add();
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

int command_add() {
    if (compare_string(command, "node ", 4, 9)) {

    } if else (compare_string(command, "block ", 4, 10)) {

    } else {
        return 1;
    }
    return 0;
}

int command_rm() {
    if (compare_string(command, "node ", 3, 8)) {

    } if else (compare_string(command, "block ", 3, 9)) {

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

int comand_sync() {

    return 0;
}

/*

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
        return NULL
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
        if(string1[start] != string2[i - start]) {
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
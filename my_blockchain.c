#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int NODE = 0;
char SYNC = 's';

typedef struct block {
    char*  bid;
    struct block* next;
} st_block;

typedef struct node {
    int nid;
    struct block* first;
    struct node* next;
} st_node;

typedef struct t_char_string {
    int size;
    char** array;
} char_string;

int main() {
    st_node* blockchain = NULL;
    if (restore_blockchain(&blockchain))
        printf("No Valid Backup Found: Starting New Blockchain\n");
    else 
        printf("Restoring From Backup\n");

    while(1) {
        printf("[%c%d]> ", SYNC, NODE);
        char_string* command;
        int result_com = read_command(&command);
        if (result_com == -1) {
            return rm_all_node(&blockchain);
        }
        if (result_com != 0) {
            printf("nok: error reading\n");
            return 1;
        }
        if(compare_string(command->array[0], "quit") && command->size == 1) {
            printf("Backing up blockchain...\n");
            command_free(command);
            return backup_blockchain(blockchain);
        }
        return_code(execute_command(command, &blockchain));

        SYNC = check_sync(blockchain);
        command_free(command);
    }
    return 0;
}

int read_command(char_string** command) {
    char buffer[100];
    int reading = read(0, buffer, 100);
    if (reading == 0)
        return -1;
    if (reading < 0)
        return 1;
    if (buffer[reading - 1] == '\n')
        buffer[reading - 1] = '\0';
    if (my_split(buffer, " ", command))
        return 1;
    return 0;
}

int execute_command(char_string* command, st_node** blockchain) {
    if (compare_string(command->array[0], "add")) {
        return command_add(command, blockchain);
    }
    /*en preparation*/
    if (compare_string(command->array[0], "rm")) {
        return command_rm(command, blockchain);
    }
    if (compare_string(command->array[0], "ls")) {
        return command_ls();
    }
    if (compare_string(command->array[0], "sync") && command->size == 1) {
        return command_sync();
    }
    return 6;
}

void command_free(char_string* command) {
    for (int i = 0; i< command->size; i++) {
        free(command->array[i]);
    }
    free(command->array);
    free(command);
}

void return_code(int code) {
    if (code == 0)
        printf("OK\n")
    if (code == 1)
        printf("nok: no more resources available on the computer.\n");
    if (code == 2)
        printf("nok: this node already exists.\n");
    if (code == 3)
        printf("nok: this block already exists.\n");
    if (code == 4)
        printf("nok: node doesn't exist.\n");
    if (code == 5)
        printf("nok: block doesn't exist.\n");
    if (code == 6)
        printf("nok: command not found.\n");
}





/*
FUNCTIONS
*/



int compare_string(char* string1, char* string2) {
    if(count_string(string1) != count_string(string2))
        return 0;
    for (int i = 0; i < count_string(string1); i++) {
        if(string1[i] != string2[i]) {
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

int count_int(int number) {
    int count = 0;
    if (number == 0)
        return 1;
    while (number != 0) {
        number = number / 10;
        count++;
    }
    return count;
}

void int_to_char(int number, char* string) {
    int size = count_int(number);
    if (number == 0) {
        string[0] = '0';
        string[1] = '\0';
        return;
    }
    int tab[size];
    for (int i = 0; number != 0; i++) {
        tab[i] = number % 10;
        number = number / 10;
    }
    int j = 0;
    for (int i = size - 1 ; i >= 0; i--) {
        string[j] = tab[i] + '0';
        j++;
    }
    string[size] = '\0';
}

int write_all(int fd, char* buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t written = write(fd, buffer + total, size - total);
        if (written <= 0) {
            return 1;
        }
        total += written;
    }
    return 0;
}

/*
FUNCTIONS MY_SPLIT
*/

int my_split(char* param_1, char* param_2, char_string** result) {
    *result = malloc(sizeof(char_string));
    if (*result == NULL)
        return 1;
    
    (*result)->size = count_words(param_1, param_2);

    (*result)->array = malloc(sizeof(char*) * ((*result)->size));
    if ((*result)->array == NULL) {
        free(*result);
        return 1;
    }
    if (malloc_letters(param_1, param_2, *result)) {
        free((*result)->array);
        free(*result);
        return 1;
    }

    fill_final(param_1, param_2, *result);
    return 0;
}

int count_words(char* param_1, char* param_2) {
    int nb_word = 0;
    int i = 0;
    while(param_1[i] != '\0') {
        while (param_1[i] == param_2[0])
            i++;
        if (param_1[i] != '\0') {
            nb_word++;
            while (param_1[i] != param_2[0] && param_1[i] != '\0')
                i++;
        }
    }
    return nb_word;
}

int malloc_letters(char* param_1, char* param_2, char_string* result) {
    int z = 0;
    int nb_letter = 0;
    for (int i = 0; param_1[i] != '\0'; i++) {
        while (param_1[i] == param_2[0])
            i++;
        if (param_1[i] == '\0') 
            break;
        nb_letter++;
        if (param_1[i] != param_2[0] && (param_1[i+1] == param_2[0] || param_1[i+1] == '\0')) {
            result->array[z] = malloc(sizeof(char) * (nb_letter + 1));
            if(result->array[z] == NULL) {
                while(z != 0) {
                    z--;
                    free(result->array[z]);
                }
                return 1;
            }
            nb_letter = 0;
            z++;
        }
    }
    return 0;
}

void fill_final(char* param_1, char* param_2, char_string* result) {
    int j = 0;
    for (int word = 0; word < result->size; word++) {
        while(param_1[j] == param_2[0]) {
            j++;
        }
        int letter = 0;
        while (param_1[j] != param_2[0] && param_1[j] != '\0') {
            result->array[word][letter] = param_1[j];
            j++;
            letter++;
        }
        result->array[word][letter] = '\0';
    }
}
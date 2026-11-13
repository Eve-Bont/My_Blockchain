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

typedef struct t_char_string {
    int size;
    char** array;
} char_string;

int main() {
    st_node* blockchain = NULL;
    char synchronization = 's';
    int nb_node = 0;
    printf("No Backup Found: Starting New Blockchain\n");

    while(1) {
        printf("[%c%d]> ", synchronization, nb_node);
        char_string* command;
        if (read_command(&command)) {
            printf("nok: no more resources available on the omputer");
            return 1;
        }
        if(compare_string(command->array[0], "quit") && command->size == 1) {
            command_free(command);
            break;
        }
        return_code(execute_command(command, &blockchain));
        command_free(command);
    }
    return 0;
}

int execute_command(char_string* command, st_node** blockchain) {
    if (compare_string(command->array[0], "add")) {
        return command_add(command, blockchain);
    }
    /*en preparation*/
    else if (compare_string(command->array[0], "rm")) {
        return command_rm(command, blockchain);
    }
    else if (compare_string(command->array[0], "ls")) {
        return command_ls();
    }
    else if (compare_string(command->array[0], "sync") && command->size == 1) {
        return command_sync();
    }
    else {
        return 1;
    }
}

void return_code(int code) {
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
COMMANDS
*/


//FUNCTIONS TO COMMAND ADD
int command_add(char_string* command, st_node** blockchain) {
    if (command->size < 3)
        return 6;
    if (compare_string(command->array[1], "node") && command->size == 3)
        return add_node(command->array[2], blockchain);
    if (compare_string(command->array[1], "block") && command->size >= 4)
        return add_block(command, blockchain);
    return 6;
}
int add_node(char* command_nid, st_node** blockchain) {
    int value_nid = 0;
    if (calc_nid(command_nid, &value_nid))
        return 6;
    if (found_node(blockchain, value_nid))
        return 2;
    return fill_node(blockchain, value_nid);
}
int add_block(char_string* command, st_node** blockchain) {
    char* value_bid = command->array[2];
    if (command->size == 4 && compare_string(command->array[3], "*"))
        return add_all_block(value_bid, blockchain);
    for (int i = 3; i < command->size; i++) {
        int value_nid = 0;
        if (calc_nid(command->array[i], &value_nid))
            return 6;
        if (!found_node(blockchain, value_nid))
            return 4;
        if (found_block(blockchain, value_nid, value_bid))
            return 3;
    }
    for (int i = 3; i < command->size; i++) {
        int value_nid = 0;
        if (calc_nid(command->array[i], &value_nid))
            return 6;
        if (fill_block(blockchain, value_nid, value_bid))
            return 1;
    }
    return 0;
}
int add_all_block(char* value_bid, st_node** blockchain) {
    st_node* node = *blockchain;
    if (node == NULL)
        return 4;
    while (node != NULL) {
        if (found_block(blockchain, node->nid, value_bid))
            return 3;
        node = node->next;
    }
    node = *blockchain;
    while (node != NULL) {
        if (fill_block(blockchain, node->nid, value_bid))
            return 1;
        node = node->next;
    }
    return 0;
}

//FUNCTIONS TO COMMAND RM
int command_rm(char_string* command, st_node** blockchain) {
    if (command->size < 3)
        return 6;
    if (compare_string(command->array[1], "node") && command->size >= 3)
        return rm_node(command, blockchain);
    if (compare_string(command->array[1], "block") && command->size >= 4)
        return rm_block(/*preparation*/);
    return 6;
}
int rm_node(char_string* command, st_node** blockchain) {
    int value_nid;
    if (compare_string(command->array[2], "*") && command->size == 3)
        return rm_all_node(blockchain);
    for (int i = 3; i < command->size; i++) {
        if (calc_nid(command->array[i], &value_nid))
            return 6;
        if (!found_node(blockchain, value_nid))
            return 4;
    }
    for (int i = 3; i < command->size; i++) {
        if (calc_nid(command->array[i], &value_nid))
            return 6;
        sup_nodes(value_nid, blockchain);
    }
    return 0;
}
int rm_all_node(st_node** blockchain) {
    st_node* node = *blockchain;
    while(node != NULL) {
        st_node* n_next = node->next;
        if (node->first != NULL)
            sup_blocks(node->first);
        free(node);
        node = n_next;
    }
    *blockchain = NULL;
    return 0;
}
void sup_nodes(int value_nids, t_node** blockchain) {
    st_node* node = *blockchain;
    st_node* n_previous = NULL;
    while (node->nid != value_nid) {
        n_previous = node;
        node = node->next;
    }
    if (n_previous == NULL)
        *blockchain = node->next;
    else
        n_previous->next = node->next;

    if (node->first != NULL)
        sup_blocks(node->first);
    free(node);
}
void sup_blocks(st_block* first) {
    st_block* block = first;
    while (block != NULL) {
        st_block* b_next = block->next;
        free(block->bid);
        free(block);
        block = b_next;
    }
}



int calc_nid(char* command_nid, int* value_nid) {
    *value_nid = 0;
    for (int i = 0; command_nid[i] != '\0'; i++) {
        if(command_nid[i] < '0' || command_nid[i] > '9')
            return 6;
        *value_nid = *value_nid * 10 + (command_nid[i] - '0');
    }
    return 0;
}

int found_node(st_node** blockchain, int value_nid) {
    st_node* node = *blockchain;

    while(node != NULL) {
        if(node->nid == value_nid)
            return 1;
        node = node->next;
    }
    return 0;
}

int found_block(st_node** blockchain, int value_nid, char* value_bid) {
    st_node* node = *blockchain;
    while (node->nid != value_nid)
        node = node->next;
    st_block* block = node->first;
    while (block != NULL) {
        if (compare_string(block->bid, value_bid))
            return 1;
        block = block->next;
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

int fill_block(st_node** blockchain, int value_nid, char* value_bid) {
    st_block* new_block = malloc(sizeof(st_block));
    if (new_block == NULL) {
        return 1;
    }
    new_block->bid = malloc(sizeof(char) * (count_string(value_bid) + 1));
    if (new_block->bid == NULL) {
        free(new_block);
        return 1;
    }
    fill_bid(value_bid, new_block->bid);
    new_block->next = NULL;

    st_node* node = *blockchain;
    while(node->nid != value_nid)
        node = node->next;
    if (node->first == NULL) {
        node->first = new_block;
        return 0;
    }
    st_block* block = node->first;
    while(block->next != NULL)
        block = block->next;
    block->next = new_block;
    return 0;
}

void fill_bid(char* value_bid, char* bid) {
    for (int i = 0; value_bid[i] != '\0'; i++)
        bid[i] = value_bid[i];
    bid[count_string(value_bid)] = '\0';
}



/*en preparation*/
int command_ls() {
    if(command->size == 2 && compare_string(command->array[1], "-l") ) {

    } else if (command->size == 1) {

    }
    return 6;
}

int command_sync() {

    return 0;
}

/*
FUNCTIONS
*/

int read_command(char_string** command) {
    char buffer[100];
    int reading = read(0, buffer, 100);
    if (reading <= 0)
        return 1;
    if (buffer[reading - 1] == '\n')
        buffer[reading - 1] = '\0';
    if (my_split(buffer, " ", command))
        return 1;
    return 0;
}

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

void command_free(char_string* command) {
    for (int i = 0; i< command->size; i++) {
        free(command->array[i]);
    }
    free(command->array);
    free(command);
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
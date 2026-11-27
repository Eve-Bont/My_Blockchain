/*
COMMAND ADD
*/

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
        if (!found_block(blockchain, value_nid, value_bid)) {
            if (fill_block(blockchain, value_nid, value_bid))
                return 1;
        } else 
            return 3;
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

/*
COMMAND RM
*/

int command_rm(char_string* command, st_node** blockchain) {
    if (command->size < 3)
        return 6;
    if (compare_string(command->array[1], "node"))
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
        if (found_node(blockchain, value_nid))
            sup_nodes(value_nid, blockchain);
        else 
            return 4;
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
    NODE = 0;
    return 0;
}

void sup_nodes(int value_nid, st_node** blockchain) {
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
    NODE--;
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

/*
COMMAND SYNC
*/

int command_sync(char_string* command, st_node** blockchain) {
    if (command->size == 1) {
        if (SYNC == '-') {
            int node_final = NODE;
            if (backup_blockchain(*blockchain))
                return 1;
            st_node* temporary = NULL;
            if (init_temporary(&temporary) || found_all_block(blockchain, &temporary) || sync_node(blockchain, &temporary)) {
                rm_all_node(&temporary);
                rm_all_node(blockchain);
                return 1;
            }
            rm_all_node(&temporary);
            NODE = node_final;
            SYNC = 's';
        }
        return 0;
    }
    return 6;
}

int init_temporary(st_node** temporary) {
    *temporary = malloc(sizeof(st_node));
    if (*temporary == NULL)
        return 1;
    (*temporary)->nid = 1;
    (*temporary)->first = NULL;
    (*temporary)->next = NULL;
    return 0;
}

int found_all_block(st_node** blockchain, st_node** temporary) {
    st_node* node = *blockchain;
    while (node != NULL) {
        st_block* block = node->first;
        while (block != NULL) {
            if (!found_block(temporary, 1, block->bid)) {
                if (fill_block(temporary, 1, block->bid))
                    return 1;
            }
            block = block->next;
        }
        sup_blocks(node->first);
        node->first = NULL;
        node = node->next;
    }
    return 0;
}

int sync_node(st_node** blockchain, st_node** temporary) {
    st_node* node = *blockchain;
    while (node != NULL) {
        st_block* temp_block = temporary->first;
        while (temp_block != NULL) {
            if (fill_block(blockchain, node->nid, temp_block->bid))
                return 1;
            temp_block = temp_block->next;
        }
        node = node->next;
    }
    return 0;
}


/*
COMMAND LS
*/

int command_ls(char_string* command, st_node** blockchain) {
    if(command->size == 2 && compare_string(command->array[1], "-l") ) {
        return display(1, blockchain);
    } else if (command->size == 1) {
        return display(0, blockchain);
    }
    return 6;
}
int display(int option, st_node** blockchain) {
    st_node* node = *blockchain;
    while (node != NULL) {
        printf("%d\n", node->nid);
        if (option) {
            printf(":");
            st_block* block = node->first;
            while(block != NULL) {
                printf(" %s", block->bid);
                block = block->next;
            }
            printf("\n");
        }
        node = node->next;
    }
    return 0;
}

/*
FUNCTIONS
*/

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
    if (node == NULL)
        return 0;
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

int fill_node(st_node** blockchain, int value_nid) {
    st_node* new_node = malloc(sizeof(st_node));
    if(new_node == NULL)
        return 1;
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
    NODE++;
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
    if (node == NULL)
        return 1;
    while (node->nid != value_nid)
        node = node->next;
    if (node->first == NULL) {
        node->first = new_block;
        return 0;
    }
    st_block* block = node->first;
    while (block->next != NULL)
        block = block->next;
    block->next = new_block;
    return 0;
}

void fill_bid(char* value_bid, char* bid) {
    for (int i = 0; value_bid[i] != '\0'; i++)
        bid[i] = value_bid[i];
    bid[count_string(value_bid)] = '\0';
}

char check_sync(st_node* blockchain) {
    st_node* reference = blockchain;
    if (reference == NULL)
        return 's';

    st_node* node = reference->next;
    while (node != NULL) {
        st_block* block1 = reference->first;
        st_block* block2 = node->first;

        while(block1 != NULL && block2 != NULL) {
            if (!compare_string(block1->bid, block2->bid))
                return '-';
            block1 = block1->next;
            block2 = block2->next;
        }
        if (block1 != NULL || block2 != NULL)
            return '-';
        node = node->next;
    }
    return 's';
}
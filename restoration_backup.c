#include "fonctions.h"
/*
BACKUP
*/

int backup_blockchain(st_node* blockchain) {
    int fd = open(".my_blockchain_backup", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return 1;
    if (backup_base(fd) || backup_nodes(fd, blockchain)) {
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int backup_base(int fd) {
    char buffer[100];
    int_to_char(NODE, buffer);
    if (write_all(fd, &SYNC, 1) || write_all(fd, buffer, count_int(NODE)) || write_all(fd, "\n", 1)) {
        return 1;
    }
    return 0;
}

int backup_nodes(int fd, st_node* blockchain) {
    st_node* node = blockchain;
    while (node != NULL) {
        char buffer[100];
        int_to_char(node->nid, buffer);
        if (write_all(fd, buffer, count_int(node->nid)) || write_all(fd, " ", 1)) {
            return 1;
        }
        st_block* block = node->first;
        if (backup_blocks(fd, block))
            return 1;
        node = node->next;
    }
    return 0;
}

int backup_blocks(int fd, st_block* block) {
    while (block != NULL) {
        if (write_all(fd, block->bid, count_string(block->bid)) || write_all(fd, " ", 1)) {
            return 1;
        }
        block = block->next;
    }
    if (write_all(fd, "\n", 1)) {
        return 1;
    }
    return 0;
}

/*
RESTAURATION
*/

int restore_blockchain(st_node** blockchain) {
    int fd = open(".my_blockchain_backup", O_RDONLY);
    if (fd == -1)
        return 1;
    int saved_nodes = 0;
    if (restore_sync(fd) || restore_nb_node(fd, &saved_nodes) || restore_nodes(fd, saved_nodes, blockchain) || NODE != saved_nodes) {
        close(fd);
        rm_all_node(blockchain);
        SYNC = 's';
        return 1;
    }
    close(fd);
    return 0;
}

int restore_sync(int fd) {
    char buffer;
    if (read(fd, &buffer, 1) != 1)
        return 1;
    if (buffer != 's' && buffer != '-')
        return 1;
    SYNC = buffer;
    return 0;
}

int restore_nb_node(int fd, int* saved_nodes) {
    char buffer;
    NODE = 0;
    if (read(fd, &buffer, 1) != 1) {
        return 1;
    }
    while (buffer != '\n') {
        if (buffer < '0' || buffer > '9') {
            return 1;
        }
        *saved_nodes = *saved_nodes * 10 + (buffer - '0');
        if (read(fd, &buffer, 1) != 1) {
            return 1;
        }
    }
    return 0;
}

int restore_nodes(int fd, int saved_nodes, st_node** blockchain) {
    for (int i = 0; i < saved_nodes; i++) {
        int nid;
        if (restore_nid(fd, &nid, blockchain) || restore_blocks(fd, nid, blockchain))
            return 1;
    }
    char buffer;
    if (read(fd, &buffer, 1) != 0)
        return 1;
    return 0;
}

int restore_nid(int fd, int* nid, st_node** blockchain) {
    char buffer;
    if (read(fd, &buffer, 1) != 1)
            return 1;
    *nid = 0;
    while (buffer != ' ') {
        if (buffer < '0' || buffer > '9')
            return 1;
        *nid = *nid * 10 + (buffer - '0');
        if (read(fd, &buffer, 1) != 1)
            return 1;
    }
    if (found_node(blockchain, *nid) || fill_node(blockchain, *nid))
        return 1;
    return 0;
}

int restore_blocks(int fd, int nid, st_node** blockchain) {
    char buffer;
    while (1) {
        if (read(fd, &buffer, 1) != 1)
            return 1;
        if (buffer == '\n')
            break;
        if (buffer == ' ')
            continue;
        if (restore_bid(fd, &buffer, nid, blockchain))
            return 1;
        if (buffer == '\n')
            break;
    }
    return 0;
}

int restore_bid(int fd, char* buffer, int nid, st_node** blockchain) {
    char bid[1000];
    int i = 0;
    while (*buffer != ' ' && *buffer != '\n') {
        if (i >= 999)
            return 1;
        bid[i] = *buffer;
        i++;
        if (read(fd, buffer, 1) != 1)
            return 1;
    }
    bid[i] = '\0';
    if (fill_block(blockchain, nid, bid))
        return 1;
    return 0;
}
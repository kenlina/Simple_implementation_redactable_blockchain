#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include "preimage_chameleon_hash.h"

struct eval_key ek;
struct trap_key tk;
struct ch_sys_params sys;

typedef struct Block {
    int index;
    long timestamp;
    char data[256];
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    struct randomness rnd;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    struct digest dig; 
    struct Block *next;
} Block;

typedef struct Blockchain {
    Block *head;
} Blockchain;

Block *getBlockAtIndex(Blockchain *chain, int index) {
    Block *current = chain->head;
    while (current != NULL && current->index != index) {
        current = current->next;
    }
    return current;
}


void calculateHash(Block *block) { 
    unsigned char dgt[SHA256_DIGEST_LENGTH]; 
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &block->index, sizeof(block->index));
    SHA256_Update(&sha256, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(&sha256, block->data, strlen(block->data));
    SHA256_Update(&sha256, block->prevHash, SHA256_DIGEST_LENGTH);
    SHA256_Final(&dgt, &sha256);
    // for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", dgt[i]);
    // printf("\n");

    struct message msg;
    msg.size = SHA256_DIGEST_LENGTH;
    msg.msg = dgt;
    chameleon_hash_hash(&sys, &ek, &msg, &block->rnd, &block->dig);  
    unsigned char buffer[1024];
    BN_bn2bin(block->dig.value, buffer);
    size_t size = BN_num_bytes(block->dig.value);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    memcpy(block->hash, buffer, size); 
}
void collide(Block *block, char msg[]) { 
    strncpy(block->data, msg, strlen(msg));
    unsigned char dgt[SHA256_DIGEST_LENGTH]; 
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &block->index, sizeof(block->index));
    SHA256_Update(&sha256, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(&sha256, block->data, strlen(block->data));
    SHA256_Update(&sha256, block->prevHash, SHA256_DIGEST_LENGTH);
    SHA256_Final(&dgt, &sha256);
    

    struct message new_msg;
    new_msg.size = SHA256_DIGEST_LENGTH;
    new_msg.msg = dgt;

    chameleon_hash_preimage(&sys, &tk, &new_msg, &block->dig, &block->rnd);    
}

Block *createBlock(char *data, unsigned char *prevHash) {
    Block *block = malloc(sizeof(Block));
    if (block == NULL) {
        return NULL;
    }
    block->index = 0;
    block->timestamp = time(NULL);
    strncpy(block->data, data, 255);
    block->data[255] = '\0';
    memcpy(block->prevHash, prevHash, SHA256_DIGEST_LENGTH);
    chameleon_hash_init_randomness(&sys, &block->rnd);
    chameleon_hash_init_digest(&sys, &block->dig);
    chameleon_hash_sample_randomness(&sys, &ek, &block->rnd);
    // unsigned char buffer[1024];
    // BN_bn2bin(block->rnd.r, buffer);
    // size_t size = BN_num_bytes(block->rnd.r);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    calculateHash(block);
    
    block->next = NULL;
    return block;
}

void addBlock(Blockchain *chain, Block *newBlock) {
    Block *current = chain->head;
    if (current == NULL) {
        chain->head = newBlock;
        return;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    newBlock->index = current->index + 1;
    current->next = newBlock;
}

int validateBlockchain(Blockchain *chain) {
    if (chain->head == NULL) return 1;
    Block *current = chain->head->next;
    Block *prev = chain->head;

    while (current != NULL) {
        if (memcmp(prev->hash, current->prevHash, SHA256_DIGEST_LENGTH) != 0) {
            
            return 0; // Invalid blockchain
        }
        prev = current;
        current = current->next;
    }
    return 1; // Blockchain is valid
}

int validateByPrint(Blockchain *chain) {
    if (chain->head == NULL) return 1;
    Block *current = chain->head->next;
    Block *prev = chain->head;
    while (current != NULL) {
        printf("Index: %d, ", prev->index);
        printf("Hash:     ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", prev->hash[i]);
        printf("\n");
        printf("Index: %d, ", current->index);
        printf("prevHash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", current->prevHash[i]);
        printf("\n");
        printf("-----------------------\n");
        if (memcmp(prev->hash, current->prevHash, SHA256_DIGEST_LENGTH) != 0) {
            printf("Invalid hash\n");
            return 0; // Invalid blockchain
        }
        prev = current;
        current = current->next;
    }
    return 1; // Blockchain is valid
}

void printBlockchain(Blockchain *chain) {
    Block *current = chain->head;
    printf("Blockchain:\n");

    while (current != NULL) {
        printf("-----------------------\n");
        printf("Index:    %d\n", current->index);
        printf("Timestamp:%ld\n", current->timestamp);
        printf("Data:     %s\n", current->data);
        unsigned char buffer[1024];
        BN_bn2bin(current->rnd.r, buffer);
        size_t size = BN_num_bytes(current->rnd.r);
        printf("Random R: ");
        for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
        printf("\n");
        BN_bn2bin(current->rnd.s, buffer);
        size = BN_num_bytes(current->rnd.s);
        printf("Random S: ");
        for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
        printf("\n");
        printf("PrevHash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", current->prevHash[i]);
        printf("\n");
        printf("Hash:     ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", current->hash[i]);
        printf("\n");
        current = current->next;
    }
}


int main() {
    Blockchain chain = {NULL};
    unsigned char genesisHash[SHA256_DIGEST_LENGTH] = {0};
    chameleon_hash_init_sysparams(128, &sys);  // 初始化系統參數
    chameleon_hash_keygen(&sys, &ek, &tk); 
    while (1)
    {
        printf("Enter command: ");
        char input[51];
        scanf("%s", input);
        while (getchar() != '\n');
        if(strncmp(input, "p", 2) == 0){
            printBlockchain(&chain);
        }
        else if(strncmp(input, "g", 2) == 0){
            printf("Enter block message(less than 50 words): ");
            input[0] = '\0';
            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            input[len - 1] = '\0';
            if (chain.head == NULL){
                Block *b = createBlock(input, genesisHash); 
                addBlock(&chain, b);
            }
            else{
                Block *ptr = chain.head;
                while(ptr->next != NULL) ptr = ptr->next;
                Block *b = createBlock(input, ptr->hash);
                addBlock(&chain, b);
            }
        }
        else if(strncmp(input, "q", 2) == 0){
            printf("quit process\n");
            break;
        }
        else if(strncmp(input, "v", 2) == 0){
            if (validateByPrint(&chain)) {
                printf("Blockchain validated successfully.\n");
            } else {
                printf("Blockchain validation failed.\n");
                exit(-1);
            }
        }
        else if(strncmp(input, "m", 2) == 0){
            printf("Choose the index of the block you want to modify: ");
            scanf("%s", input);
            while (getchar() != '\n');
            int target = atoi(input);
            Block *ptr = chain.head;
            for(int i = 0; i < target; ++i){
                if(ptr == NULL){
                    printf("out of index\n");
                    exit(-1);
                }
                ptr = ptr->next;
            }
            printf("Enter the message you want to modify: ");
            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            input[len - 1] = '\0';
            memset(ptr->data, 0, 256);
            collide(ptr, input);
        }
        else{
            printf("-----------------------\n");
            printf( "usage:\n   enter 'p' to print blockchain\n" 
            "   enter 'g' to generate block\n" 
            "   enter 'v' to rehash the block and validate the block\n" 
            "   enter 'm' to modify the data of the specific block(won't invalidate blockchain,\n"
            "                check the hash and randomness after modification)\n" 
            "   enter 'q' to quit\n");
            printf("-----------------------\n");
        }

        // if (validateBlockchain(&chain)) {
        //     // printf("Blockchain validated successfully.\n");
        // } else {
        //     printf("Blockchain validation failed.\n");
        //     exit(-1);
        // }
    }
    
    return 0;
}

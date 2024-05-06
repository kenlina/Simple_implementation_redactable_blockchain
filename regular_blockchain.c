#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

typedef struct Block {
    int index;
    long timestamp;
    char data[256];
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    struct Block *next;
} Block;

typedef struct Blockchain {
    Block *head;
} Blockchain;

void calculateHash(Block *block, unsigned char *hashOutput) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &block->index, sizeof(block->index));
    SHA256_Update(&sha256, &block->timestamp, sizeof(block->timestamp));
    SHA256_Update(&sha256, block->data, strlen(block->data));
    SHA256_Update(&sha256, block->prevHash, SHA256_DIGEST_LENGTH);
    SHA256_Final(hashOutput, &sha256);
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
    calculateHash(block, block->hash);
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
    newBlock->next = NULL;
    calculateHash(newBlock, newBlock->hash);
    current->next = newBlock;
}

int validateBlockchain(Blockchain *chain) {
    if (chain->head == NULL) return 1;
    Block *current = chain->head->next;
    Block *prev = chain->head;

    while (current != NULL) {
        unsigned char prev_hash[SHA256_DIGEST_LENGTH];
        calculateHash(prev, prev_hash);
        if (memcmp(prev_hash, current->prevHash, SHA256_DIGEST_LENGTH) != 0) {
            
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
        unsigned char prev_hash[SHA256_DIGEST_LENGTH];
        calculateHash(prev, prev_hash);
        printf("Index: %d, ", prev->index);
        printf("Hash:     ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", prev_hash[i]);
        printf("\n");
        printf("Index: %d, ", current->index);
        printf("prevHash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", current->prevHash[i]);
        printf("\n");
        printf("-----------------------\n");
        if (memcmp(prev_hash, current->prevHash, SHA256_DIGEST_LENGTH) != 0) {
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
    printf("-----------------------\n");
    printf("usage:\n   enter 'p' to print blockchain\n" 
                    "   enter 'g' to generate block\n" 
                    "   enter 'q' to quit\n");
    printf("-----------------------\n");
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
            strncpy(ptr->data, input, strlen(input));
            calculateHash(ptr, ptr->hash);
        }
        else{
            printf("-----------------------\n");
            printf("usage:\n   enter 'p' to print blockchain\n" 
                    "   enter 'g' to generate block\n" 
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

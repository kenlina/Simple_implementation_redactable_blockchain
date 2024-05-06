#include<stdio.h>
#include<stdlib.h>
#include<openssl/sha.h>
#include<sys/random.h>
#include<string.h>
#include<time.h>
#define ran_n ( 1024 * 1024 )
#define N 1000
void generateRandom(unsigned char* ran){
    ssize_t n = getrandom(ran, ran_n, 0);
    if( n != ran_n){
        printf("get random error\n");
        exit(-1);
    }
}
void Hash(unsigned char *string, size_t size, unsigned char *digest){
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, size);
  SHA256_Final(digest, &sha256);
}

int main(int argc,  char *argv[]){
    clock_t start, end;
    double cpu_time_used;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char ran1[ran_n];
    
    generateRandom(ran1);

    start = clock();
    for(int i = 0; i < N; ++i){
        // Doing double hash
        Hash(ran1, ran_n, hash);
        Hash(hash, SHA224_DIGEST_LENGTH, hash);
    }
    end = clock();

    // printf("SHA-256 Hash: ");
    // for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    //     printf("%02x", hash[i]);
    // }
    // printf("\n");
    

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // 计算时间差
    double avg = cpu_time_used / N;
    printf("Spend time:%lf s\n", avg);

    return 0;
}
#include "preimage_chameleon_hash.h"
#include <stdio.h>
#include <string.h>
#include <sys/random.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include<time.h>


#define BLOCK_SIZE (1024 * 1024)
#define N 1000

int main(int argc, char **argv){
  clock_t start, end;
  double cpu_time_used;

  struct eval_key ek;
  struct trap_key tk;
  unsigned char blockchain_block[BLOCK_SIZE], second_block[BLOCK_SIZE];
  unsigned char digest[SHA256_DIGEST_LENGTH + 1];
  unsigned char buffer[1024];
  struct ch_sys_params sys;
  struct message msg1, msg2;
  struct randomness rnd1, rnd2;
  struct digest dgt, dgt2;
  chameleon_hash_init_sysparams(166, &sys);
  chameleon_hash_keygen(&sys, &ek, &tk);
  chameleon_hash_init_randomness(&sys, &rnd1);
  chameleon_hash_init_randomness(&sys, &rnd2);
  chameleon_hash_init_digest(&sys, &dgt);
  chameleon_hash_init_digest(&sys, &dgt2);

  /******************hash*******************/
  msg1.msg = blockchain_block;
  msg1.size = BLOCK_SIZE;
  cpu_time_used = 0;
  for(int i = 0 ; i < N; ++i){
    getrandom(&blockchain_block, BLOCK_SIZE, 0);
    start = clock();
    chameleon_hash_sample_randomness(&sys, &ek, &rnd1);

    // BN_bn2bin(rnd1.r, buffer);
    // size_t size = BN_num_bytes(rnd1.r);
    // printf("Random R: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");

    // BN_bn2bin(rnd1.s, buffer);
    // size = BN_num_bytes(rnd1.s);
    // printf("Random S: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");

    chameleon_hash_hash(&sys, &ek, &msg1, &rnd1, &dgt);

    BN_bn2bin(dgt.value, buffer);
    size_t size = BN_num_bytes(dgt.value);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    hash(buffer, size, digest);

    end = clock();
    cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;
  }

  double avg = cpu_time_used / N;
  printf("Generate hash spending time: %lf s\n", avg);
  /*************************************/

  /******************collide*******************/
  msg2.msg = second_block;
  msg2.size = BLOCK_SIZE;
  cpu_time_used = 0;
  for(int i = 0 ; i < N; ++i){
    getrandom(blockchain_block, BLOCK_SIZE, 0);
    getrandom(second_block, BLOCK_SIZE, 0);
    
    chameleon_hash_sample_randomness(&sys, &ek, &rnd1);
    chameleon_hash_hash(&sys, &ek, &msg1, &rnd1, &dgt);

    // BN_bn2bin(rnd1.r, buffer);
    // size_t size = BN_num_bytes(rnd1.r);
    // printf("Random R: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    
    // BN_bn2bin(rnd1.s, buffer);
    // size = BN_num_bytes(rnd1.s);
    // printf("Random S: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");

    // BN_bn2bin(dgt.value, buffer);
    // size = BN_num_bytes(dgt.value);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    


    start = clock();
    chameleon_hash_preimage(&sys, &tk, &msg2, &dgt, &rnd1);

    // BN_bn2bin(rnd1.r, buffer);
    // size = BN_num_bytes(rnd1.r);
    // printf("Random R: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    
    // BN_bn2bin(rnd1.s, buffer);
    // size = BN_num_bytes(rnd1.s);
    // printf("Random S: ");
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");

    // BN_bn2bin(dgt.value, buffer);
    // size = BN_num_bytes(dgt.value);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    


    end = clock();
    chameleon_hash_hash(&sys, &ek, &msg2, &rnd1, &dgt2);

    // BN_bn2bin(dgt2.value, buffer);
    // size = BN_num_bytes(dgt2.value);
    // for (int i = 0; i < size; i++) printf("%02x", buffer[i]);
    // printf("\n");
    cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;
  }

  avg = cpu_time_used / N;
  printf("Generate collision spending time: %lf s\n", avg);
  /*************************************/
  chameleon_hash_destroy_keys(&sys, &ek, &tk);
  chameleon_hash_destroy_randomness(&rnd1);
  chameleon_hash_destroy_randomness(&rnd2);
  chameleon_hash_destroy_digest(&dgt);
  chameleon_hash_destroy_digest(&dgt2);
  chameleon_hash_destroy_sysparams(&sys);

  
  return 0;
}

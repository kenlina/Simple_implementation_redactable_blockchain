all: time_redact time_sha256 sha256 redact
	
time_redact: chameleon_hash_ateniese04a.c redact_time.c
	gcc -g -Wall -O2 -c -include chameleon_hash_ateniese04a.c redact_time.c
	gcc -g -Wall -O2  redact_time.o -o redact_time -lcrypto
	rm -rf *.o

time_sha256:
	gcc -Wall -O2 sha256_time.c -o sha256_time -lcrypto  

redact: chameleon_hash_ateniese04a.c redactable_blockchain.c
	gcc -g -Wall -O2 -c -include chameleon_hash_ateniese04a.c redactable_blockchain.c
	gcc -g -Wall -O2  redactable_blockchain.o -o redactable_blockchain -lcrypto
	rm -rf *.o
	
sha256:
	gcc -g -Wall -O2 blockchain.c -o blockchain -lcrypto

clean:
	rm -rf *.o redactable_blockchain blockchain redact_time sha256_time

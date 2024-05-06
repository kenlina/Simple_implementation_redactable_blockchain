all: time_redact time_sha256 sha256 redact
	
time_redact: chameleon_hash_ateniese04a.c redact.c
	gcc -g -Wall -O2 -c -include chameleon_hash_ateniese04a.c redact.c
	gcc -g -Wall -O2  redact.o -o redact_time -lm -lcrypto
	rm -rf *.o
time_sha256:
	gcc -Wall -O2 SHA256.c -o sha256_time -lcrypto  -lm
redact: chameleon_hash_ateniese04a.c redactable_blockchain.c
	gcc -g -Wall -O2 -c -include chameleon_hash_ateniese04a.c redactable_blockchain.c
	gcc -g -Wall -O2  redactable_blockchain.o -o redact -lm -lcrypto
	rm -rf *.o
sha256:
	gcc -g -Wall -O2 regular_blockchain.c -o sha256 -lcrypto
clean:
	rm -rf *.o redact sha256 redact_time sha256_time

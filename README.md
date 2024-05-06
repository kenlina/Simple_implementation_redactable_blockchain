# Simple implementation of regular blockchain and redactable blockchain

This repository contains a Makefile and associated C source files for a blockchain project focusing on hashing and redactability features. The project demonstrates different aspects of blockchain technology, including performance metrics for hash computations and the capability to edit blockchain elements under controlled conditions.

## Executables

The Makefile in this project automates the compilation and linking process for creating four distinct executables:

1. **redactable_blockchain**: Implements a simple editable blockchain that incorporates chameleon hash functions to allow post-hoc modifications while maintaining the blockchain's integrity.

2. **blockchain**: A basic blockchain implementation using SHA256 hashing to ensure data integrity and security.

3. **redact_time**: Measures the time taken to compute hashes and handle collisions within the editable blockchain, providing essential data for analyzing the efficiency and security of the redactable features.

4. **sha256_time**: Focuses on measuring the computation time of SHA256 hashes, used to evaluate the performance of traditional blockchain hashing.

## Makefile Usage

### Compilation

To compile all the programs listed above, use the following command in your terminal:

```bash=
$ make all
```
You can also compile individual programs by specifying their corresponding target:
```
$ make time_redact # measure time for redaction
$ make time_sha256 # measure time for sha256
$ make redact      # redactable blockchain example 
$ make sha256      # nromal blockchain example
```
## Running the Programs

After compilation, each executable can be run from the command line as follows:
```
$ ./redactable_blockchain
$ ./blockchain
$ ./redact_time
$ ./sha256_time
```
### blockchain commands

- **Generate Block (`g`)**: Allows the user to add a new block to the blockchain by specifying the block's message.
- **Print Blockchain (`p`)**: Displays all the blocks in the blockchain, showing each block's data and hash.
- **Verify Blockchain (`v`)**: Recomputes the hashes for all blocks to ensure that the chain's integrity is maintained. It checks if the blockchain is valid based on the current data.
- **Modify Block (`m`)**: Enables the user to alter the data of a specified block. Note that modifying a block will change its hash and may invalidate the blockchain.
- **Quit (`q`)**: Exits the blockchain application process.

## System Requirements
Make sure to have the necessary dependencies, such as the cryptographic library (`crypto`) and the math library (`lm`), installed on your system to ensure the programs run successfully.

## Acknowledgments

This project makes extensive use of components developed by Thiago Leucz Astrizi, found in his [blockchain_tests](https://github.com/thiagoharry/blockchain_tests) repository on GitHub. Specific files utilized include:

- **sha256_time Implementation**: We use the `sha256.c` file from Thiago's repository to test computation times, critical for assessing our blockchain solution's efficiency.

- **redact_time**: The `classical_redactable_blockchain.c` provided insight into implementing redactable features in blockchains, allowing modifications under certain conditions without compromising integrity.

- **Chameleon Hash Function**: The files `chameleon_hash_ateniese04a.c` and `preimage_chameleon_hash.h` are utilized as APIs in our project to develop a simple editable blockchain. This feature is vital for allowing post-hoc adjustments in the blockchain while maintaining security and verifiability.

I am deeply thankful to [thiagoharry](https://github.com/thiagoharry) for his contributions to the open-source community. 


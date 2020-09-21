# DFA_on_GIFT64
## 1. DFA on GIFT64 round 26:
### Fault Model:
Fault injected at input of Subcell at round 26. 
Location of fault: first nibble.
Random fault values. 

Function to inject fault: GIFT64_Encryption_FaultyRound26(uint64_t plaintext, uint16_t * key) 
Function to recover: GIFT64_Attack_Round26(uint64_t plaintext, uint64_t faulty_plaintext)


## 2. DFA on GIFT64 round 25: 
Based on the paper found in http://jbreier.com/files/papers/tc_2019.pdf
### Fault Model:
Fault injected at output of Subcell at round 25. 
Location of fault: first nibble.
Assume that the attacker can choose the fault. 
The code works only when Hamming weight of the fault is less than 4.

Function to inject fault:  GIFT64_Encryption_FaultyRound25(uint64_t plaintext, uint16_t * key, uint64_t fault)
Function to recover: GIFT64_Attack_Round25(uint64_t ciphertext, uint64_t faulty_ciphertext, uint64_t fault)



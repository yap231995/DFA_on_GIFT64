# DFA_on_GIFT64
## Fault Model:
Fault injected at input of Subcell at round 26. 
Location of fault: first nibble.
Random fault values. 

Function to inject fault: GIFT64_Encryption_FaultyRound26(uint64_t plaintext, uint16_t * key) 
Function to recover: GIFT64_Attack(uint64_t plaintext, uint64_t faulty_plaintext)

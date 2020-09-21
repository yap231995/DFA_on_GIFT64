#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

const uint8_t s_box[16] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x01, 0x0a, 0x04, 0x0c, 0x06, 0x0f, 0x03, 0x09, 0x02, 0x0d, 0x0b, 0x07, 0x05, 0x00, 0x08, 0x0e};

const uint8_t inv_s_box[16] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x0d, 0x00, 0x08, 0x06, 0x02, 0x0c, 0x04, 0x0b, 0x0e, 0x07, 0x01, 0x0a, 0x03, 0x09, 0x0f, 0x05
    };

const uint8_t PermTable[64] = {
0, 17, 34, 51, 48, 1, 18, 35, 32, 49, 2, 19, 16, 33, 50, 3, 
4, 21, 38, 55, 52, 5, 22, 39, 36, 53, 6, 23, 20, 37, 54, 7, 
8, 25, 42, 59, 56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11, 
12, 29, 46, 63, 60, 13, 30, 47, 44, 61, 14, 31, 28, 45, 62, 15
};
const uint8_t InvPermTable[64] = {
    0, 5, 10, 15, 16, 21, 26, 31, 32, 37, 42, 47, 48, 53, 58, 63, 
    12, 1, 6, 11, 28, 17, 22, 27, 44, 33, 38, 43, 60, 49, 54, 59, 
    8, 13, 2, 7, 24, 29, 18, 23, 40, 45, 34, 39, 56, 61, 50, 55, 
    4, 9, 14, 3, 20, 25, 30, 19, 36, 41, 46, 35, 52, 57, 62, 51
};

const uint8_t Constant[49] = {
   //1   2    3    4    5     6     7     8    9     10     11    12     13   14    15   16
0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3e, 0x3d, 0x3b, 0x37, 0x2f, 0x1e, 0x3c, 0x39, 0x33, 0x27, 0xe, 
0x1d, 0x3a, 0x35, 0x2b, 0x16, 0x2c, 0x18, 0x30, 0x21, 0x2, 0x5, 0xb, 0x17, 0x2e, 0x1c, 0x38, 
0x31, 0x23, 0x6, 0xd, 0x1b, 0x36, 0x2d, 0x1a, 0x34, 0x29, 0x12, 0x24, 0x8, 0x11, 0x22, 0x4
};

void convert64bittoBinary(uint64_t n) {
int c,k;
int count = 0;
 for (c = 63; c >= 0; c--) {
    k = n >> c;
    if (k & 1) {
        printf("1");
    }
    else {
        printf("0");
        count= count +1;
    }
    if(c%2 == 0) {
        printf(" ");
    }  
  }
//printf("\n");
//printf("count: %d \n", count);
printf("\n");
}

uint64_t SubCells(uint64_t x){
    uint64_t y = 0;
    int i;
    uint64_t value, sub_value;
    for(i = 0; i < 16; i++ ) {
        value = (x >> 4*i) & 0x0f;
        //printf("SubCells value: %"SCNd8 "\n", value);
        sub_value = s_box[value];
        y = (sub_value << 4*i) ^ y;
    }    
    return y;
}

uint64_t InvSubCells(uint64_t x){
    uint64_t y = 0;
    int i;
    uint64_t value, sub_value;
    for(i = 0; i < 16; i++ ) {
        value = (x >> 4*i) & 0x0f;
        sub_value = inv_s_box[value];
        y = (sub_value << 4*i) ^ y;
    }    
    return y;
}

uint64_t PermBits(uint64_t x) {
    uint8_t TextArr[64];
    int i;
    for(i = 0; i <64; i++) {
        TextArr[i]=(x>>i) & 0x01;
    }
    uint64_t y = 0;
    for(i = 0; i < 64; i++) {
        uint8_t index = PermTable[i];
        uint64_t temp = TextArr[i];
        uint64_t temp2 = temp << index;
        y = temp2 ^ y;
    }

    return y;
 }


uint64_t InvPermBits(uint64_t x) {
    uint8_t TextArr[64];
    int i;
    for(i = 0; i <64; i++) {
        TextArr[i]=(x>>i) & 0x01;
    }
    uint64_t y = 0;
    for(i = 0; i < 64; i++) {
        uint8_t index = InvPermTable[i];
        uint64_t temp = TextArr[i];
        uint64_t temp2 = temp << index;
        y = (temp2) ^ y;
    }
    return y;
 }

uint16_t BitRotation(uint16_t Value, int Rotation_Num) {
    int i;
    uint16_t Num = Value;
    uint8_t rotatedBit;
    for(i = 1; i <= Rotation_Num; i ++) {
        rotatedBit = Num & 0x01;
        Num = (Num >> 1) | (rotatedBit <<15);
    }
    return Num;
}


uint16_t ** KeySchedule(uint16_t * key) {
    uint16_t ** W = (uint16_t **)malloc(28*sizeof(uint16_t *));
    int i;
    W[0] = (uint16_t *)malloc(8*sizeof(uint16_t));
    W[0] = key;
    
    for(i=1; i < 28; i++) {
        W[i] = (uint16_t *)malloc(8*sizeof(uint16_t));
        uint16_t k0 = BitRotation(W[i-1][7], 12);
        uint16_t k1 = BitRotation(W[i-1][6], 2);
        W[i][7] = W[i-1][7-2];
        W[i][6] = W[i-1][7-3];
        W[i][5] = W[i-1][7-4];
        W[i][4] = W[i-1][7-5];
        W[i][3] = W[i-1][7-6];
        W[i][2] = W[i-1][7-7];
        W[i][1] = k0;
        W[i][0] = k1;
    }
    return W;
}

uint64_t AddRoundKey(uint64_t x, uint16_t * key, int Round) {
    //Add Key 
    uint16_t U = key[6];
    uint16_t V = key[7];
    int i;
    uint64_t u,v;
    for(i = 0; i < 16; i++) {
        u= (U >> i) & 0x01;
        v= (V >> i) & 0x01;
        x = (u<<4*i+1) ^ x;
        x = (v<< 4*i) ^ x; 
    }
    //Add Constant
    uint64_t constant = Constant[Round];
    uint64_t c0 = constant & 0x01;
    uint64_t c1 = (constant>>1) & 0x01;
    uint64_t c2 = (constant>>2) & 0x01;
    uint64_t c3 = (constant>>3) & 0x01;
    uint64_t c4 = (constant>>4) & 0x01;
    uint64_t c5 = (constant>>5) & 0x01;
    uint64_t constant1 = 1;
    x = x ^(constant1<<63);
    x = x ^(c5<< 23);
    x = x ^(c4<< 19);
    x = x ^(c3<< 15);
    x = x ^(c2<< 11);
    x = x ^(c1<< 7);
    x = x ^(c0<< 3);
    return x;
}


uint64_t GIFT64_Encryption(uint64_t plaintext, uint16_t * key) {
    uint64_t text = plaintext;
    uint16_t** W = KeySchedule(key);

    //Round 1-28
    int i;
    for(i = 1; i <=28; i++) {
        text = SubCells(text);
        printf("%d: after SubCells: %"SCNx64 "\n", i-1, text);
        text = PermBits(text);
        printf("%d: after PermBits : %"SCNx64 "\n", i-1, text);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        printf("%d: after AddRoundKeys: %"SCNx64 "\n", i-1, text);
        printf("%d current key: ");  
        int k;
        for(k = 0; k < 8; k++) {
             printf("%x " ,W[i-1][k]);
        }
        printf("\n");
        }
    return text;
}

uint64_t GIFT64_Decryption(uint64_t ciphertext, uint16_t * key) {
    uint64_t text = ciphertext;
    uint16_t** W = KeySchedule(key);
    //printf("Decrypted: %"SCNu64 " round: %d\n", text, 0);
    //Round 1-28
    int i;
    for(i = 28; i >=1; i--) {
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        printf("%d: after inverse AddRoundKeys: %"SCNx64 "\n", i-1, text);
        text = InvPermBits(text);
         printf("%d: after inverse PermBits: %"SCNx64 " \n", i-1, text);
        //printf("Decrypted after Perm: %"SCNu64 " round: %d\n", text, i);
        text = InvSubCells(text);
        printf("%d: after inverse SubCells: %"SCNx64 "\n", i-1, text);
        
    }
    //printf("\n");
    return text;
}


static uint8_t Random4Bits() {
	uint8_t x = rand() & 0xf;
	return x;
}

uint64_t GIFT64_Encryption_FaultyRound26(uint64_t plaintext, uint16_t * key) {
    uint64_t text = plaintext;
    uint16_t** W = KeySchedule(key);

    //Round 1-28
    int i;
    for(i = 1; i <=25; i++) {
        text = SubCells(text);
        text = PermBits(text);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        // //print key 
        // int k;
        // for(k = 0; k < 8; k++) {
        //      printf("%x " ,W[i-1][k]);
        // }
        // printf("\n");
    }
    uint64_t faulty_text = text;
    uint64_t fault = Random4Bits(); 
    faulty_text = faulty_text^ fault; 
   // printf("faulty text: %"SCNx64 "\n", faulty_text);
    //printf("text: %"SCNx64 "\n", text);
    uint64_t testfault = text ^faulty_text;
    printf("fault: %"SCNx64 "\n", testfault);

    //Round 26-28
    for(i = 26; i <=28 ; i++) {
        text = SubCells(text);
        faulty_text = SubCells(faulty_text);
        uint64_t Subcell_Differential = text ^faulty_text; 
        //printf("%d: Subcell Differential: %"SCNx64 "\n", i, Subcell_Differential);
        text = PermBits(text);
        faulty_text = PermBits(faulty_text);
        uint64_t PermBit_Differential = text ^faulty_text; 
        //printf("%d: PermBit Differential: %"SCNx64 "\n", i, PermBit_Differential);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        faulty_text = AddRoundKey(faulty_text, Roundkey, i);
        uint64_t AddRoundKey_Differential = text ^faulty_text; 
        printf("%d: AddRoundKey Differential: %"SCNx64 "\n", i, AddRoundKey_Differential);
    }
    //printf("CipherText: %"SCNx64 "\n", text);
    return faulty_text;
}


uint8_t *** GIFT64_Attack_Round26(uint64_t plaintext, uint64_t faulty_plaintext) {
    uint8_t *** keybits = (uint8_t ***)malloc(16*sizeof(uint8_t**));
    uint8_t * y = (uint8_t *)malloc(64*sizeof(uint8_t));  // Plaintext arry
    uint8_t * y_fault = (uint8_t *)malloc(64*sizeof(uint8_t)); //Faulty plaintext array
    uint8_t * differentialArr = (uint8_t *)malloc(64*sizeof(uint8_t));
    uint64_t differential = plaintext^faulty_plaintext;
    int i;
    //Converting unsigned int to array.
    for(i =0; i<64; i++) {
        y[i] = (plaintext >> i) & 0x01;
        y_fault[i] = (faulty_plaintext >> i) & 0x01;
        differentialArr[i] = (differential >> i) & 0x01;
    }

    //Initializing ActiveSbox.
    // 0 if is not active, and 1 if is active.
    uint8_t * ActiveSbox_R28 = (uint8_t *)malloc(16*sizeof(uint8_t));
     for(i =0; i<16; i++) {
         ActiveSbox_R28[i] = 0;
    }

    //Identify which are the active Sbox.
    for(i = 0; i < 64; i ++) {
        if(differentialArr[i] == 1) {
            uint8_t Sbox_num = InvPermTable[i]/4;
            ActiveSbox_R28[Sbox_num] = 1;
        }
    }
    for(i = 0; i < 16; i++) {
        if (ActiveSbox_R28[i] == 1) printf("Active Sbox_num: %d \n", i);
    }

    uint64_t constant = Constant[28];
    uint64_t c0 = constant & 0x01;
    uint64_t c1 = (constant>>1) & 0x01;
    uint64_t c2 = (constant>>2) & 0x01;
    uint64_t c3 = (constant>>3) & 0x01;
    uint64_t c4 = (constant>>4) & 0x01;
    uint64_t c5 = (constant>>5) & 0x01;

    //Equations
    int counter0 = 0;
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;
    int counter4 = 0;
    int counter5 = 0;
    int counter6 = 0;
    int counter7 = 0;
    int counter8 = 0;
    int counter9 = 0;
    int counter10 = 0;
    int counter11 = 0;
    int counter12 = 0;
    int counter13 = 0;
    int counter14 = 0;
    int counter15 = 0;
    

    int u,v;
    if(ActiveSbox_R28[0] == 1)  {
        printf("In SBOX0: \n");
        keybits[0] = (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x0 = (y[51]<<3)^(y[34]<<2)^((y[17] ^u)<<1) ^ (y[0] ^ v);
                uint8_t x0_fault = (y_fault[15]<<3)^(y_fault[34]<<2)^((y_fault[17] ^u)<<1) ^ (y_fault[0] ^ v);
                uint8_t s0 = inv_s_box[x0] ^ inv_s_box[x0_fault];
                if(s0 == 1) {
                    keybits[0][counter0] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[0][counter0][0] = v;//v0
                    keybits[0][counter0][1] = u;//u4
                    counter0 +=1;
                    //keybits[0] = v;//v0
                    //keybits[20] = v;//u4
                    printf("v0: %d ", v);
                    printf("u4: %d \n", u);
                }
            }
        }
    }
    

    if(ActiveSbox_R28[1] == 1)  {
        printf("In SBOX1, \n");
        keybits[1] = (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x1 = (y[35]<<3)^(y[18]<<2)^((y[1] ^u)<<1) ^ (y[48] ^ v);
                uint8_t x1_fault = (y_fault[35]<<3)^(y_fault[18]<<2)^((y_fault[1] ^u)<<1) ^ (y_fault[48] ^ v);
                uint8_t s1 = inv_s_box[x1] ^ inv_s_box[x1_fault];
                if(s1 == 1) {
                    keybits[1][counter1] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[1][counter1][0] = v;//v12
                    keybits[1][counter1][1] = u;//u0
                    counter1+=1;
                    //keybits[12] = v;//v12
                    //keybits[16] = v;//u0
                    printf("v12: %d ", v);
                    printf("u0: %d \n", u);
                }
            }
        }
    }
    
    if(ActiveSbox_R28[2] == 1)  {
        printf("In SBOX2, \n");
        keybits[2] = (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x2 = ((y[19]^c4)<<3)^(y[2]<<2)^((y[49] ^u)<<1) ^ (y[32] ^ v);
                uint8_t x2_fault = ((y_fault[19] ^c4)<<3)^(y_fault[2]<<2)^((y_fault[49] ^u)<<1) ^ (y_fault[32] ^ v);
                 uint8_t s2 = inv_s_box[x2] ^ inv_s_box[x2_fault];
                if(s2 == 1) {
                    keybits[2][counter2] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[2][counter2][0] = v;//v8
                    keybits[2][counter2][1] = u;//u12
                    counter2+=1;
                    //keybits[8] = v;//v8
                    //keybits[28] = v;//v12
                    printf("v8: %d ", v);
                    printf("u12: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[3] == 1)  {
        printf("In SBOX3, \n");
        keybits[3] = (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x3 = ((y[3]^c0)<<3)^(y[50]<<2)^((y[33] ^u)<<1) ^ (y[16] ^ v);
                uint8_t x3_fault =((y_fault[3]^c0)<<3)^(y_fault[50]<<2)^((y_fault[33] ^u)<<1) ^ (y_fault[16] ^ v);
                uint8_t s3 = inv_s_box[x3] ^ inv_s_box[x3_fault];
                if(s3 == 1) {
                    keybits[3][counter3] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[3][counter3][0] = v;//v4
                    keybits[3][counter3][1] = u;//u8
                    counter3+=1;
                    //keybits[4] = v;//v4
                    //keybits[24] = u;//v8
                    printf("v4: %d ", v);
                    printf("u8: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[4] == 1)  {
        printf("In SBOX4, \n");
        keybits[4] = (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x4 = (y[55]<<3)^(y[38]<<2)^((y[21] ^u)<<1) ^ (y[4] ^ v);
                uint8_t x4_fault =(y_fault[55]<<3)^(y_fault[38]<<2)^((y_fault[21] ^u)<<1) ^ (y_fault[4] ^ v);
                uint8_t s4 = inv_s_box[x4] ^ inv_s_box[x4_fault];
                if(s4 == 2) {
                    keybits[4][counter4] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[4][counter4][0] = v;//v1
                    keybits[4][counter4][1] = u;//u5
                    counter4+=1;
                    //keybits[1] = v;//v1
                    //keybits[21] = u;//u5
                    printf("v1: %d ", v);
                    printf("u5: %d \n", u);
                }
            }
        }
    }



    if(ActiveSbox_R28[5] == 1)  {
        printf("In SBOX5, \n");
        keybits[5]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x5 = (y[39]<<3)^(y[22]<<2)^((y[5] ^u)<<1) ^ (y[52] ^ v);
                uint8_t x5_fault = (y_fault[39]<<3)^(y_fault[22]<<2)^((y_fault[5] ^u)<<1) ^ (y_fault[52] ^ v);
                uint8_t s5 = inv_s_box[x5] ^ inv_s_box[x5_fault];
                if(s5 == 2) {
                    keybits[5][counter5] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[5][counter5][0] = v;//v13
                    keybits[5][counter5][1] = u;//u1
                    counter5+=1;
                    //keybits[13] = v;//v13
                    //keybits[17] = u;//u1
                    printf("v13: %d ", v);
                    printf("u1: %d \n", u);
                }
            }
        }
    }

    
    if(ActiveSbox_R28[6] == 1)  {
        printf("In SBOX6, \n");
        keybits[6]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x6 = ((y[23]^c5)<<3)^(y[6]<<2)^((y[53] ^u)<<1) ^ (y[36] ^ v);
                uint8_t x6_fault = ((y_fault[23]^c5)<<3)^(y_fault[6]<<2)^((y_fault[53] ^u)<<1) ^ (y_fault[36] ^ v);
                uint8_t s6 = inv_s_box[x6] ^ inv_s_box[x6_fault];
                if(s6 == 0x02) {
                    keybits[6][counter6] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[6][counter6][0] = v;//v9
                    keybits[6][counter6][1] = u;//u13
                    counter6+=1;
                    //keybits[9] = v;//v9
                    //keybits[29] = u;//u13
                    printf("v9: %d ", v);
                    printf("u13: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[7] == 1)  {
        printf("In SBOX7, \n");
        keybits[7]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x7 = ((y[7]^c1)<<3)^(y[54]<<2)^((y[37] ^u)<<1) ^ (y[20] ^ v);
                uint8_t x7_fault = ((y_fault[7]^c1)<<3)^(y_fault[54]<<2)^((y_fault[37] ^u)<<1) ^ (y_fault[20] ^ v);
                uint8_t s7 = inv_s_box[x7] ^ inv_s_box[x7_fault];
                if(s7 == 2) {
                    keybits[7][counter7] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[7][counter7][0] = v;//v5
                    keybits[7][counter7][1] = u;//u9
                    counter7+=1;                    
                    //keybits[5] = v;//v5
                    //keybits[25] = u;//u9
                    printf("v5: %d ", v);
                    printf("u9: %d \n", u);
                }
            }
        }
    }



    if(ActiveSbox_R28[8] == 1)  {
        printf("In SBOX8, \n");
        keybits[8]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x8 = (y[59]<<3)^(y[42]<<2)^((y[25] ^u)<<1) ^ (y[8] ^ v);
                uint8_t x8_fault = (y_fault[59]<<3)^(y_fault[42]<<2)^((y_fault[25] ^u)<<1) ^ (y_fault[8] ^ v);
                 uint8_t s8 = inv_s_box[x8] ^ inv_s_box[x8_fault];
                if(s8 == 4) {
                    keybits[8][counter8] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[8][counter8][0] = v;//v2
                    keybits[8][counter8][1] = u;//u6
                    counter8+=1;
                    //keybits[2] = v;//v2
                    //keybits[22] = u;//u6
                    printf("v2: %d ", v);
                    printf("u6: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[9] == 1)  {
        printf("In SBOX9, \n");
        keybits[9]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x9 = (y[43]<<3)^(y[26]<<2)^((y[9] ^u)<<1) ^ (y[56] ^ v);
                uint8_t x9_fault = (y_fault[43]<<3)^(y_fault[26]<<2)^((y_fault[9] ^u)<<1) ^ (y_fault[56] ^ v);
                uint8_t s9 = inv_s_box[x9] ^ inv_s_box[x9_fault];
                if(s9 == 0x04) {
                    keybits[9][counter9] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[9][counter9][0] = v;//v9
                    keybits[9][counter9][1] = u;//u13
                    counter9+=1;
                    //keybits[14] = v;//v14
                    //keybits[18] = u;//u2
                    printf("v14: %d ", v);
                    printf("u2: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[10] == 1)  {
        printf("In SBOX10, \n");
        keybits[10]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {
                uint8_t x10 = (y[27]<<3)^(y[10]<<2)^((y[57] ^u)<<1) ^ (y[40] ^ v);
                uint8_t x10_fault = (y_fault[27]<<3)^(y_fault[10]<<2)^((y_fault[57] ^u)<<1) ^ (y_fault[40] ^ v);
                uint8_t s10 = inv_s_box[x10] ^ inv_s_box[x10_fault];
                if(s10 == 0x04) {
                    keybits[10][counter10] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[10][counter10][0] = v;//v10
                    keybits[10][counter10][1] = u;//u14
                    counter10+=1;
                    //keybits[10] = v;//v10
                    //keybits[30] = u;//u14
                    printf("v10: %d ", v);
                    printf("u14: %d \n", u);
                }
            }
        }
    }


    if(ActiveSbox_R28[11] == 1)  {
        printf("In SBOX11, \n");
        keybits[11]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {     
            uint8_t x11 = ((y[11]^c2)<<3)^(y[58]<<2)^((y[41] ^u)<<1) ^ (y[24] ^ v);
            uint8_t x11_fault = ((y_fault[11]^c2)<<3)^(y_fault[58]<<2)^((y_fault[41] ^u)<<1) ^ (y_fault[24] ^ v);
            uint8_t s11 = inv_s_box[x11] ^ inv_s_box[x11_fault];
                if(s11 == 4) {
                    keybits[11][counter11] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[11][counter11][0] = v;//v6
                    keybits[11][counter11][1] = u;//u10
                    counter11+=1;
                    //keybits[6] = v;//v6
                    //keybits[26] = u;//u10
                    printf("v6: %d ", v);
                    printf("u10: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[12] == 1)  {
        printf("In SBOX12, \n");
        keybits[12]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {     
                uint8_t x12 = ((y[63]^1)<<3)^(y[46]<<2)^((y[29] ^u)<<1) ^ (y[12] ^ v);
                uint8_t x12_fault = ((y_fault[63]^1)<<3)^(y_fault[46]<<2)^((y_fault[29] ^u)<<1) ^ (y_fault[12] ^ v);
                uint8_t s12 = inv_s_box[x12] ^ inv_s_box[x12_fault];
                if(s12 == 8) {
                    keybits[12][counter12] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[12][counter12][0] = v;//v3
                    keybits[12][counter12][1] = u;//u7
                    counter12+=1;                    
                    //keybits[3] = v;//v3
                    //keybits[23] = u;//u7
                    printf("v3: %d ", v);
                    printf("u7: %d \n", u);
                }
            }
        }
    }


    if(ActiveSbox_R28[13] == 1)  {
        printf("In SBOX13, \n");
        keybits[13]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {     
                uint8_t x13 = (y[47]<<3)^(y[30]<<2)^((y[13] ^u)<<1) ^ (y[60] ^ v);
                uint8_t x13_fault = (y_fault[47]<<3)^(y_fault[30]<<2)^((y_fault[13] ^u)<<1) ^ (y_fault[60] ^ v);
                uint8_t s13 = inv_s_box[x13] ^ inv_s_box[x13_fault];
                if(s13 == 8) {
                    keybits[13][counter13] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[13][counter13][0] = v;//v15
                    keybits[13][counter13][1] = u;//u3
                    counter13+=1;                      
                    //keybits[15] = v;//v15
                    //keybits[19] = u;//u3
                    printf("v15: %d ", v);
                    printf("u3: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[14] == 1)  {
        printf("In SBOX14, \n");
        keybits[14]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {     
                uint8_t x14 = (y[31]<<3)^(y[14]<<2)^((y[61] ^u)<<1) ^ (y[44] ^ v);
                uint8_t x14_fault = (y_fault[31]<<3)^(y_fault[14]<<2)^((y_fault[61] ^u)<<1) ^ (y_fault[44] ^ v);
                uint8_t s14 = inv_s_box[x14] ^ inv_s_box[x14_fault];
                if(s14 == 8) {
                    keybits[14][counter14] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[14][counter14][0] = v;//v11
                    keybits[14][counter14][1] = u;//u15
                    counter14+=1;  
                    //keybits[11] = v;//v11
                    //keybits[31] = u;//u15
                    printf("v11: %d ", v);
                    printf("u15: %d \n", u);
                }
            }
        }
    }

    if(ActiveSbox_R28[15] == 1)  {
        printf("In SBOX15, \n");
        keybits[15]= (uint8_t **)malloc(4*sizeof(uint8_t *));
        for(u = 0; u <=1; u++) {
            for(v = 0; v <=1; v++) {     
                uint8_t x15 = ((y[15]^c3)<<3)^(y[62]<<2)^((y[45] ^u)<<1) ^ (y[28] ^ v);
                uint8_t x15_fault = ((y_fault[15]^c3)<<3)^(y_fault[62]<<2)^((y_fault[45] ^u)<<1) ^ (y_fault[28] ^ v);
                uint8_t s15 = inv_s_box[x15] ^ inv_s_box[x15_fault];
                if(s15 == 8) {
                    keybits[15][counter15] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[15][counter15][0] = v;//v7
                    keybits[15][counter15][1] = u;//u11
                    counter15+=1;  
                    //keybits[7] = v;//v7
                    //keybits[27] = u;//u11
                    printf("v7: %d ", v);
                    printf("u11: %d \n", u);
                }
            }
        }
    }
    return keybits;
}




int main() {
    //Test Vector 1
    // uint64_t plaintext1 = 0x0;
    // uint16_t key1[8] = {0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 };
    // printf("Plaintext: %"SCNx64 "\n", plaintext1);
    // printf("masterkey: ");
    // int k;
    // for(k = 0; k < 8; k++) {
    //     printf("%x " ,key1[k]);
    // }
    // printf("\n");
    

    //Test Vector 2
    // uint64_t plaintext2 = 0xfedcba9876543210;
    // uint16_t key2[8] = {0xfedc, 0xba98, 0x7654, 0x3210, 0xfedc, 0xba98, 0x7654, 0x3210 };
    // printf("Plaintext: %"SCNx64 "\n", plaintext2);
    // printf("masterkey: ");  
    // int k;
    // for(k = 0; k < 8; k++) {
    //     printf("%x " ,key2[k]);
    // }
    // printf("\n");

    //Test Vector 3
    //uint64_t plaintext3 = 0xc450c7727a9b8a7d;
    //uint64_t plaintext3 = 0xfedcba9876543210;
    uint64_t plaintext3 = 0x0;
    
    //uint16_t key3[8] = {0xbd91, 0x731e, 0xb6bc, 0x2713, 0xa1f9, 0xf6ff, 0xc750, 0x44e7};
    uint16_t key3[8] = {0xfedc, 0xba98, 0x7654, 0x3210, 0xfedc, 0xba98, 0x7654, 0x3210 };
    printf("Plaintext: %"SCNx64 "\n", plaintext3);
    printf("masterkey: ");  
    int k;
    for(k = 0; k < 8; k++) {
        printf("%x " ,key3[k]);
    }
    printf("\n");
    uint16_t ** W = (uint16_t **)malloc(28*sizeof(uint16_t *));
    W = KeySchedule(key3);
    printf("Roundkey28 extraction: \n");  
    uint16_t V = W[28-1][7]; //V
    uint16_t U = W[28-1][6]; //U
    for(k =0; k < 16; k++) {
        uint8_t bit = (V>>k) & 0x01;
        printf("v%d: %"SCNu8"\n", k, bit); 
    }
    for(k =0; k < 16; k++) {
        uint8_t bit = (U>>k) & 0x01;
        printf("u%d: %"SCNu8"\n", k, bit); 
    }

    uint64_t EncryptedValue = GIFT64_Encryption(plaintext3,key3);
    uint64_t FaultyEncryptedValue = GIFT64_Encryption_FaultyRound26(plaintext3,key3);
    printf("CipherText: %"SCNx64 "\n", EncryptedValue);
    printf("Faulty CipherText: %"SCNx64 "\n", FaultyEncryptedValue);
    

    //Assume Fault model on only the first nibble.
    uint8_t *** keybits = (uint8_t ***)malloc(16*sizeof(uint8_t**));
    keybits = GIFT64_Attack_Round26(EncryptedValue, FaultyEncryptedValue);

    
    //uint64_t DecryptedValue = GIFT64_Decryption(EncryptedValue,key3);
    //printf("DecryptedValue: %"SCNx64 "\n", DecryptedValue);

    
    return 0;
}




#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>

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
        //printf("%d: after SubCells: %"SCNx64 "\n", i-1, text);
        text = PermBits(text);
        //printf("%d: after PermBits : %"SCNx64 "\n", i-1, text);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        //printf("%d: after AddRoundKeys: %"SCNx64 "\n", i-1, text);
        //printf("%d current key: ");  
        // int k;
        // for(k = 0; k < 8; k++) {
        //      printf("%x " ,W[i-1][k]);
        // }
        // printf("\n");
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
        //printf("%d: after inverse AddRoundKeys: %"SCNx64 "\n", i-1, text);
        text = InvPermBits(text);
         //printf("%d: after inverse PermBits: %"SCNx64 " \n", i-1, text);
        //printf("Decrypted after Perm: %"SCNu64 " round: %d\n", text, i);
        text = InvSubCells(text);
        //printf("%d: after inverse SubCells: %"SCNx64 "\n", i-1, text);
        
    }
    //printf("\n");
    return text;
}


static uint8_t Random4Bits() {
	uint8_t x = rand() & 0xf;
	return x;
}

int HammingWeightofFault(uint64_t fault_pre) {
    int i = 0;
    int weight = 0;
    for(i =0; i < 64; i++) {
        if((fault_pre>> i) & 0x01 == 1) {
            weight += 1;
        }
    }
    return weight;
}

uint64_t GIFT64_Encryption_FaultyRound25(uint64_t plaintext, uint16_t * key, uint64_t fault) {
    uint64_t text = plaintext;
    uint16_t** W = KeySchedule(key);

    //Round 1-28
    int i;
    for(i = 1; i <=24; i++) {
        text = SubCells(text);
        text = PermBits(text);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
    }

    //Round 25
    //Fault Model: location nibble location 0. 
    text = SubCells(text);
    uint64_t faulty_text = text ^ fault;
    printf("fault: %"SCNx64 "\n", fault);
    text = PermBits(text);
    faulty_text = PermBits(faulty_text);
    uint64_t PermBit_Differential = text ^faulty_text; 
    printf("%d: PermBit Differential: %"SCNx64 "\n", 25, PermBit_Differential);
    uint16_t * Roundkey = W[25-1];
    text = AddRoundKey(text, Roundkey, 25);
    faulty_text = AddRoundKey(faulty_text, Roundkey, 25);
    uint64_t AddRoundKey_Differential = text ^faulty_text;
    printf("%d: AddRoundKey Differential: %"SCNx64 "\n", 25, AddRoundKey_Differential);

    //Round 26-28
    for(i = 26; i <=28 ; i++) {
        text = SubCells(text);
        faulty_text = SubCells(faulty_text);
        uint64_t Subcell_Differential = text ^faulty_text; 
        printf("%d: Subcell Differential: %"SCNx64 "\n", i, Subcell_Differential);
        text = PermBits(text);
        faulty_text = PermBits(faulty_text);
        uint64_t PermBit_Differential = text ^faulty_text; 
        printf("%d: PermBit Differential: %"SCNx64 "\n", i, PermBit_Differential);
        uint16_t * Roundkey = W[i-1];
        text = AddRoundKey(text, Roundkey, i);
        faulty_text = AddRoundKey(faulty_text, Roundkey, i);
        uint64_t AddRoundKey_Differential = text ^faulty_text; 
        printf("%d: AddRoundKey Differential: %"SCNx64 "\n", i, AddRoundKey_Differential);
    }
    //printf("CipherText: %"SCNx64 "\n", text);
    return faulty_text;
}


void Determine_ActiveSbox_R27(uint8_t * ActiveSbox_R27, uint64_t fault, int i) {
    
    if ((fault >> i) & 0x01 == 1) {
        //Sbox 0 -> Possible active Sbox in round 27 are Sbox 0,4,8,12.
        ActiveSbox_R27[0+i] =1;
        ActiveSbox_R27[4+i] =1;
        ActiveSbox_R27[8+i] =1;
        ActiveSbox_R27[12+i] =1;
    }
}

int Possible_HamWeight2(uint8_t* S, int c_Ham1){
    int c_Ham2 =0;
    int i,j;
    for(i=0; i < c_Ham1-1; i++) {
        for(j= i+1; j < c_Ham1; j++) {
            S[c_Ham1+c_Ham2] = S[i] ^ S[j];
            c_Ham2 += 1;
        }
    }
    return c_Ham2;
}

int Possible_HamWeight3(uint8_t* S, int c_Ham1, int c_Ham2){
    int c_Ham3 = 0;
    int i,j,k;
    for(i=0; i < c_Ham1-2; i++) {
        for(j= i+1; j < c_Ham1-1; j++) {
            for(k = j+1; k < c_Ham1-2;k++ ) {
                S[c_Ham1+c_Ham2 + c_Ham3] = S[i] ^ S[j] ^S[k];
                c_Ham3 += 1;
            }
        }
    }
    return c_Ham3;
}


void RecoverKeyBits(int Sbox_num, uint8_t*** keybits,uint8_t * y, uint8_t * y_fault, uint8_t ** S, int c_Ham1,int c_Ham2,int c_Ham3, int counter, int Constant) {
    int u,v,i;
    keybits[Sbox_num] = (uint8_t **)malloc(4*sizeof(uint8_t *));
    for(u = 0; u <=1; u++) {
        for(v = 0; v <=1; v++) {
            uint8_t x0 = ((y[PermTable[4*Sbox_num+3]]^Constant)<<3)^(y[PermTable[4*Sbox_num+2]]<<2)^((y[PermTable[4*Sbox_num+1]] ^u)<<1) ^ (y[PermTable[4*Sbox_num]] ^ v);
            uint8_t x0_fault = ((y_fault[PermTable[4*Sbox_num+3]]^Constant)<<3)^(y_fault[PermTable[4*Sbox_num+2]]<<2)^((y_fault[PermTable[4*Sbox_num+1]] ^u)<<1) ^ (y_fault[PermTable[4*Sbox_num]] ^ v);
            uint8_t s0 = inv_s_box[x0] ^ inv_s_box[x0_fault];
            for(i = 0; i < c_Ham1+c_Ham2+c_Ham3; i++) {
                uint8_t b = S[Sbox_num][i];
                if(s0 == b) {
                    keybits[Sbox_num][counter] = (uint8_t *)malloc(2*sizeof(uint8_t));
                    keybits[Sbox_num][counter][0] = v;//v0
                    keybits[Sbox_num][counter][1] = u;//u4
                    counter +=1;
                    //keybits[0] = v;//v0
                    //keybits[20] = v;//u4
                    printf("Sbox%d v: %d ",Sbox_num, v);
                    printf(" u: %d \n", u);
                }
            } 
        }
    }
}


uint8_t *** GIFT64_Attack_Round25(uint64_t ciphertext, uint64_t faulty_ciphertext, uint64_t fault) {
    int fault_weight = HammingWeightofFault(fault);
    uint8_t *** keybits = (uint8_t ***)malloc(16*sizeof(uint8_t**));
    if(fault_weight == 4) {
        printf("Hamming Weight of fault is too high.");
        return keybits;
    }
    int i,j;
    uint8_t * ActiveSbox_R27 = (uint8_t *)malloc(16*sizeof(uint8_t));
    for(i =0; i< 16; i++) {
         ActiveSbox_R27[i] = 0;
    }
    for(i =0; i <4 ; i++) {
        Determine_ActiveSbox_R27(ActiveSbox_R27, fault, i);
    }
    for(i =0; i <16 ; i++) {
        printf("Sbox num: %d Active: %d \n",i,ActiveSbox_R27[i]);
    }
    
    //1. group the sbox 
    //S is a double array that takes in first is the Sbox num and the possible input fault mask.
    uint8_t ** S = (uint8_t **)malloc(16*sizeof(uint8_t*));

    int c0_Ham1 =0 ,c4_Ham1 =0,c8_Ham1=0,c12_Ham1 = 0;
    S[0] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[4] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[8] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[12] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    int c1_Ham1=0,c5_Ham1=0,c9_Ham1=0,c13_Ham1 =0;
    S[1] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[5] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[9] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[13] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    int c2_Ham1=0,c6_Ham1=0,c10_Ham1=0,c14_Ham1 = 0;
    S[2] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[6] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[10] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[14] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    int c3_Ham1=0,c7_Ham1=0,c11_Ham1 =0 ,c15_Ham1 = 0;
    S[3] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[7] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[11] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));
    S[15] = (uint8_t *)malloc(pow(2,fault_weight)-1*sizeof(uint8_t));

    //uniquely due to the permutation - All the possible input fault mask of Hamming weight 1 for round r.
    int a,b,c,d;
    for(i =0; i < 4; i++) {
        a=0, b=0, c=0, d=0;
        if(ActiveSbox_R27[4*i+0] ==1) {
                S[i+4*0][a] = 0x01;
                S[i+4*1][b] = 0x02;
                S[i+4*2][c] = 0x04;
                S[i+4*3][d] = 0x08;
                a += 1;
                b += 1;
                c += 1;
                d += 1;
        }
        if(ActiveSbox_R27[4*i+1] ==1) {
                S[i+4*0][a] = 0x02;
                S[i+4*1][b] = 0x04;
                S[i+4*2][c] = 0x08;
                S[i+4*3][d] = 0x01;
                a += 1;
                b += 1;
                c += 1;
                d += 1;
        }
        if(ActiveSbox_R27[4*i+2] ==1) {
                S[i+4*0][a] = 0x04;
                S[i+4*1][b] = 0x08;
                S[i+4*2][c] = 0x01;
                S[i+4*3][d] = 0x02;
                a += 1;
                b += 1;
                c += 1;
                d += 1;
        }
        if(ActiveSbox_R27[4*i+3] ==1) {
                S[i+4*0][a] = 0x08;
                S[i+4*1][b] = 0x01;
                S[i+4*2][c] = 0x02;
                S[i+4*3][d] = 0x04;
                a += 1;
                b += 1;
                c += 1;
                d += 1;
        }

        if(i == 0) {
            c0_Ham1 = a;
            c4_Ham1 = b;
            c8_Ham1 = c;
            c12_Ham1 = d;
        }
        
        if(i == 1) {
            c1_Ham1 = a;
            c5_Ham1 = b;
            c9_Ham1 = c;
            c13_Ham1 = d;
        }
        
        if(i == 2) {
            c2_Ham1 = a;
            c6_Ham1 = b;
            c10_Ham1 = c;
            c14_Ham1 = d;
        }
        if(i == 3) {
            c3_Ham1 = a;
            c7_Ham1 = b;
            c11_Ham1 = c;
            c15_Ham1 = d;
        }
    }




    //2. All the possible input fault mask of Hamming weight 2 for round r.
    int c0_Ham2,c4_Ham2,c8_Ham2,c12_Ham2,c1_Ham2,c5_Ham2,c9_Ham2,c13_Ham2,c2_Ham2,c6_Ham2,c10_Ham2,c14_Ham2,c3_Ham2,c7_Ham2,c11_Ham2,c15_Ham2;
    c0_Ham2=0,c4_Ham2=0,c8_Ham2=0,c12_Ham2=0,c1_Ham2=0,c5_Ham2=0,c9_Ham2=0,c13_Ham2=0, c2_Ham2=0,c6_Ham2=0,c10_Ham2=0,c14_Ham2=0, c3_Ham2=0,c7_Ham2=0,c11_Ham2=0,c15_Ham2 = 0;
    if (fault_weight>=2) { 
        c0_Ham2 = Possible_HamWeight2(S[0], c0_Ham1);
        c1_Ham2 = Possible_HamWeight2(S[1], c1_Ham1);
        c2_Ham2 = Possible_HamWeight2(S[2], c2_Ham1);
        c3_Ham2 = Possible_HamWeight2(S[3], c3_Ham1);
        c4_Ham2 = Possible_HamWeight2(S[4], c4_Ham1);
        c5_Ham2 = Possible_HamWeight2(S[5], c5_Ham1);
        c6_Ham2 = Possible_HamWeight2(S[6], c6_Ham1);
        c7_Ham2 = Possible_HamWeight2(S[7], c7_Ham1);
        c8_Ham2 = Possible_HamWeight2(S[8], c8_Ham1);
        c9_Ham2 = Possible_HamWeight2(S[9], c9_Ham1);
        c10_Ham2 = Possible_HamWeight2(S[10], c10_Ham1);
        c11_Ham2 = Possible_HamWeight2(S[11], c11_Ham1);
        c12_Ham2 = Possible_HamWeight2(S[12], c12_Ham1);
        c13_Ham2 = Possible_HamWeight2(S[13], c13_Ham1);
        c14_Ham2 = Possible_HamWeight2(S[14], c14_Ham1);
        c15_Ham2 = Possible_HamWeight2(S[15], c15_Ham1);
    }
    //2. All the possible input fault mask of Hamming weight 3 for round r.
    int c0_Ham3,c4_Ham3,c8_Ham3,c12_Ham3,c1_Ham3,c5_Ham3,c9_Ham3,c13_Ham3,c2_Ham3,c6_Ham3,c10_Ham3,c14_Ham3,c3_Ham3,c7_Ham3,c11_Ham3,c15_Ham3;
    c0_Ham3=0,c4_Ham3=0,c8_Ham3=0,c12_Ham3=0,c1_Ham3=0,c5_Ham3=0,c9_Ham3=0,c13_Ham3=0, c2_Ham3=0,c6_Ham3=0,c10_Ham3=0,c14_Ham3=0, c3_Ham3=0,c7_Ham3=0,c11_Ham3=0,c15_Ham3 = 0;
    if (fault_weight == 3) {
        c0_Ham3 = Possible_HamWeight3(S[0], c0_Ham1, c0_Ham2);
        c1_Ham3 = Possible_HamWeight3(S[1], c1_Ham1, c1_Ham2);
        c2_Ham3 = Possible_HamWeight3(S[2], c2_Ham1, c2_Ham2);
        c3_Ham3 = Possible_HamWeight3(S[3], c3_Ham1, c3_Ham2);
        c4_Ham3 = Possible_HamWeight3(S[4], c4_Ham1, c4_Ham2);
        c5_Ham3 = Possible_HamWeight3(S[5], c5_Ham1, c5_Ham2);
        c6_Ham3 = Possible_HamWeight3(S[6], c6_Ham1, c6_Ham2);
        c7_Ham3 = Possible_HamWeight3(S[7], c7_Ham1, c7_Ham2);
        c8_Ham3 = Possible_HamWeight3(S[8], c8_Ham1, c8_Ham2);
        c9_Ham3 = Possible_HamWeight3(S[9], c9_Ham1, c9_Ham2);
        c10_Ham3 = Possible_HamWeight3(S[10], c10_Ham1, c10_Ham2);
        c11_Ham3 = Possible_HamWeight3(S[11], c11_Ham1, c11_Ham2);
        c12_Ham3 = Possible_HamWeight3(S[12], c12_Ham1, c12_Ham2);
        c13_Ham3 = Possible_HamWeight3(S[13], c13_Ham1, c13_Ham2);
        c14_Ham3 = Possible_HamWeight3(S[14], c14_Ham1, c14_Ham2);
        c15_Ham3 = Possible_HamWeight3(S[15], c15_Ham1, c15_Ham2);
    }

    //4. ReverseBack
    uint8_t * y = (uint8_t *)malloc(64*sizeof(uint8_t));  // Ciphertext arry
    uint8_t * y_fault = (uint8_t *)malloc(64*sizeof(uint8_t)); //Faulty Ciphertext array
    uint8_t * differentialArr = (uint8_t *)malloc(64*sizeof(uint8_t));
    uint64_t differential = ciphertext^faulty_ciphertext;
    //Converting unsigned int to array for ciphertext, faulty ciphertext and differential
    for(i =0; i<64; i++) {
        y[i] = (ciphertext >> i) & 0x01;
        y_fault[i] = (faulty_ciphertext >> i) & 0x01;
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
    uint64_t constant = Constant[28];
    uint64_t c0 = constant & 0x01;
    uint64_t c1 = (constant>>1) & 0x01;
    uint64_t c2 = (constant>>2) & 0x01;
    uint64_t c3 = (constant>>3) & 0x01;
    uint64_t c4 = (constant>>4) & 0x01;
    uint64_t c5 = (constant>>5) & 0x01;

    //Equations
    int counter0=0, counter1=0, counter2=0, counter3=0, counter4=0, counter5=0, counter6=0, counter7=0, counter8=0, counter9=0, counter10=0, counter11=0, counter12=0, counter13=0, counter14=0, counter15 = 0;
    int u,v;
    if(ActiveSbox_R28[0] == 1)  {
        RecoverKeyBits(0, keybits,y, y_fault, S, c0_Ham1, c0_Ham2, c0_Ham3, counter0, 0); 
    }
    
    if(ActiveSbox_R28[1] == 1)  {
        RecoverKeyBits(1, keybits,y, y_fault, S, c1_Ham1, c1_Ham2, c1_Ham3, counter1, 0); 
    }
    if(ActiveSbox_R28[2] == 1)  {
        RecoverKeyBits(2, keybits,y, y_fault, S, c2_Ham1, c2_Ham2, c2_Ham3, counter2, c4); 
    }

    if(ActiveSbox_R28[3] == 1)  {
        RecoverKeyBits(3, keybits,y, y_fault, S, c3_Ham1, c3_Ham2, c3_Ham3, counter3, c0); 
    }
    if(ActiveSbox_R28[4] == 1)  {
        RecoverKeyBits(4, keybits,y, y_fault, S, c4_Ham1, c4_Ham2, c4_Ham3, counter4, 0); 
    }
    
    if(ActiveSbox_R28[5] == 1)  {
        RecoverKeyBits(5, keybits,y, y_fault, S, c5_Ham1, c5_Ham2, c5_Ham3, counter5, 0); 
    }
    if(ActiveSbox_R28[6] == 1)  {
        RecoverKeyBits(6, keybits,y, y_fault, S, c6_Ham1, c6_Ham2, c6_Ham3, counter6, c5); 
    }

    if(ActiveSbox_R28[7] == 1)  {
        RecoverKeyBits(7, keybits,y, y_fault, S, c7_Ham1, c7_Ham2, c7_Ham3, counter7, c1); 
    }
    if(ActiveSbox_R28[8] == 1)  {
        RecoverKeyBits(8, keybits,y, y_fault, S, c8_Ham1, c8_Ham2, c8_Ham3, counter8, 0); 
    }
    if(ActiveSbox_R28[9] == 1)  {
        RecoverKeyBits(9, keybits,y, y_fault, S, c9_Ham1, c9_Ham2, c9_Ham3, counter9, 0); 
    }
    if(ActiveSbox_R28[10] == 1)  {
        RecoverKeyBits(10, keybits,y, y_fault, S, c10_Ham1, c10_Ham2, c10_Ham3, counter10, 0); 
    }
    if(ActiveSbox_R28[11] == 1)  {
        RecoverKeyBits(11, keybits,y, y_fault, S, c11_Ham1, c11_Ham2, c11_Ham3, counter11, c2); 
    }
    if(ActiveSbox_R28[12] == 1)  {
        RecoverKeyBits(12, keybits,y, y_fault, S, c12_Ham1, c10_Ham2, c12_Ham3, counter12, 1); 
    }
    if(ActiveSbox_R28[13] == 1)  {
        RecoverKeyBits(13, keybits,y, y_fault, S, c13_Ham1, c13_Ham2, c13_Ham3, counter13, 0); 
    }
    if(ActiveSbox_R28[14] == 1)  {
        RecoverKeyBits(14, keybits,y, y_fault, S, c14_Ham1, c14_Ham2, c14_Ham3, counter14, 0); 
    }
    if(ActiveSbox_R28[15] == 1)  {
        RecoverKeyBits(15, keybits,y, y_fault, S, c15_Ham1, c15_Ham2, c15_Ham3, counter15, c3); 
    }
    return keybits;
}  



int main(){

    //Fault Model: Location at Sbox 0 output. (if is input, we can actually use SCA to determine the fault mask output. Look at paper SCADFA)
    //Assume the attacker is able to determine the fault value of the fault injected. Assume Hamming weight of fault < 4. 

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
    printf("Roundkey 28 extraction: \n");  
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
    uint64_t fault = 0x08;
    uint64_t FaultyEncryptedValue = GIFT64_Encryption_FaultyRound25(plaintext3,key3, fault);
    printf("CipherText: %"SCNx64 "\n", EncryptedValue);
    printf("Faulty CipherText: %"SCNx64 "\n", FaultyEncryptedValue);
    //Round25 Attack
    uint8_t *** keybits = (uint8_t ***)malloc(16*sizeof(uint8_t**));
    keybits = GIFT64_Attack_Round25(EncryptedValue, FaultyEncryptedValue, fault);




    return 0;
}

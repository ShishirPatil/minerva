char b[] __attribute__((section (".container-data"))) = {'o', 'r', 'i', 'g', 'i', 'n', 'a', '2', 0};
char a[] __attribute__((section (".container-data"))) = {'n', 'e', 'w', 0};

__attribute__((section (".container-code")))
void predict_helper(void) {
    printf(a);
    char msg[32];
    msg[0]  = 87;
    msg[1]  = 101;
    msg[2]  = 32;
    msg[3]  = 97;
    msg[4]  = 114;
    msg[5]  = 101;
    msg[6]  = 32;
    msg[7]  = 105;
    msg[8]  = 110;
    msg[9]  = 115;
    msg[10] = 105;
    msg[11] = 100;
    msg[12] = 101;
    msg[13] = 32;
    msg[14] = 112;
    msg[15] = 114;
    msg[16] = 101;
    msg[17] = 100;
    msg[18] = 105;
    msg[19] = 99;
    msg[20] = 116;
    msg[21] = 95;
    msg[22] = 104;
    msg[23] = 101;
    msg[24] = 108;
    msg[25] = 112;
    msg[26] = 101;
    msg[27] = 100; 
    msg[28] = 13;
    msg[29] = 10;
    msg[30] = 0;
    printf(msg);
}

__attribute__((section (".container-code")))
void another_predict_helper(void) {
    printf(b);
    char msg[34]; 
    msg[0]  = 87;
    msg[1]  = 101;
    msg[2]  = 32;
    msg[3]  = 97;
    msg[4]  = 100;
    msg[5]  = 100;
    msg[6]  = 101;
    msg[7]  = 100;
    msg[8]  = 32;
    msg[9]  = 116;
    msg[10] = 104;
    msg[11] = 105;
    msg[12] = 115;
    msg[13] = 32;
    msg[14] = 112;
    msg[15] = 114;
    msg[16] = 101;
    msg[17] = 100;
    msg[18] = 105;
    msg[19] = 99;
    msg[20] = 116;
    msg[21] = 32;
    msg[22] = 102;
    msg[23] = 117;
    msg[24] = 110;
    msg[25] = 99;
    msg[26] = 116;
    msg[27] = 105;
    msg[28] = 111;
    msg[29] = 110;
    msg[30] = 33;
    msg[31] = 13;
    msg[32] = 10;
    msg[33] = 0;
    printf(msg);
}

__attribute__((section (".container-entry")))
void predict(void) {
    char msg[36]; 
    msg[0]  = 73;
    msg[1]  = 110;
    msg[2]  = 115;
    msg[3]  = 105;
    msg[4]  = 100;
    msg[5]  = 101;
    msg[6]  = 32;
    msg[7]  = 111;
    msg[8]  = 114;
    msg[9]  = 105;
    msg[10] = 103;
    msg[11] = 105;
    msg[12] = 110;
    msg[13] = 97;
    msg[14] = 108;
    msg[15] = 32;
    msg[16] = 112;
    msg[17] = 114;
    msg[18] = 101;
    msg[19] = 100;
    msg[20] = 105;
    msg[21] = 99;
    msg[22] = 116;
    msg[23] = 32;
    msg[24] = 102;
    msg[25] = 117;
    msg[26] = 110;
    msg[27] = 99;
    msg[28] = 116;
    msg[29] = 105;
    msg[30] = 111;
    msg[31] = 110;
    msg[32] = 13;
    msg[33] = 10;
    msg[34] = 0;
    printf(msg);
    predict_helper();
    another_predict_helper();
}

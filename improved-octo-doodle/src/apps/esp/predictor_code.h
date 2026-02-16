char a[] __attribute__((section (".container-data"))) = { 'c', 'o', 'n', 't', 'a', 'i', 'n', 'e', 'r', ' ', 'd', 'a', 't', 'a', 0 };

__attribute__((section (".container-code")))
void predict_helper(void) {
    char msg[] = { 'p', 'r', 'e', 'd', 'i', 'c', 't', 'o', 'r', ' ', 'h', 'e', 'l', 'p', 'e', 'r', '\r', '\n', };
    msg[1] = 0;
    printf(msg);
    printf(a);
}

__attribute__((section (".container-entry")))
void predict(void) {
    char msg[] = { 'o', 'r', 'i', 'g', 'i', 'n', 'a', 'l', ' ', 'p', 'r', 'e', 'd', 'i', 'c', 't', 'o', 'r', ' ', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '(', '3', ')', 0 };
    printf(msg);
    printf(msg);
    printf(msg);
    //predict_helper();
}

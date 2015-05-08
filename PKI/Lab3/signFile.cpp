#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/md5.h>

const char *PRIVATE_KEY_FILE = "rsa_pri.key";
const char *PHRASE = "JaySon";
const char *FILE_TO_SIGN = "signFile.cpp";
const char *FILE_SIGN = "signFile.sign";
// 编译命令:
// clang signFile.cpp -o signFile `pkg-config openssl --cflags --libs`

const int MAX_BUF_LENGTH = 1024*1024;
unsigned char file_buf[MAX_BUF_LENGTH];
unsigned char hash[MAX_BUF_LENGTH];
unsigned char sign[MAX_BUF_LENGTH];

int main(int argc, char const *argv[])
{
    // 必须添加
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    BIO *bin;
    bin = BIO_new_file(PRIVATE_KEY_FILE, "rb");
    RSA *pri_key;
    pri_key = PEM_read_bio_RSAPrivateKey(bin, &pri_key, NULL, (void*)PHRASE);

    // 获取文件内容和长度
    memset(file_buf, 0, MAX_BUF_LENGTH);
    FILE *fp = fopen(FILE_TO_SIGN, "rb");
    long file_length = fread(file_buf, sizeof(char), MAX_BUF_LENGTH, fp);
    fclose(fp);
    fprintf(stdout, "File `%s` size: %ld Bytes\n", FILE_TO_SIGN, file_length);

    memset(hash, 0, MAX_BUF_LENGTH);
    MD5(file_buf, file_length, hash);
    unsigned char *ch = hash;
    fprintf(stdout, "MD5 hash of `%s` = ", FILE_TO_SIGN);
    while (*ch != '\0'){
        printf("%02x", *ch++);
    }printf("\n");
    long hash_length = ch - hash;
    printf("hash length: %ld\n", hash_length);

    memset(sign, 0, MAX_BUF_LENGTH);
    unsigned int sign_length = 0;
    if (1 != RSA_sign(
        NID_md5, hash, hash_length,
        sign, &sign_length,
        pri_key)){
        fprintf(stderr, "RSA_sign failed!\n");
        exit(-1);
    }
    printf("sign of `%s`:\n", FILE_TO_SIGN);
    for (int i=0; i!=sign_length; ++i){
        fprintf(stdout, "%02x", sign[i]);
    }fprintf(stdout, "\n");

    fp = fopen(FILE_SIGN, "wb");
    fwrite(sign, sizeof(unsigned char), sign_length, fp);
    fclose(fp);
    fprintf(stdout, 
        "sign of `%s` has been save @ `%s`\n",
        FILE_TO_SIGN, FILE_SIGN);
    return 0;
}

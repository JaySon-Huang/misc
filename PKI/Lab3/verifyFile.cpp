#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/md5.h>

const char *PUBLIC_KEY_FILE = "rsa_pub.key";
const char *FILE_TO_SIGN = "signFile.cpp";
const char *FILE_SIGN = "signFile.sign";
// 编译命令:
// clang verifyFile.cpp -o verifyFile `pkg-config openssl --cflags --libs`

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

    // 获取公钥
    BIO *bin;
    bin = BIO_new_file(PUBLIC_KEY_FILE, "rb");
    RSA *pub_key;
    pub_key = PEM_read_bio_RSAPublicKey(bin, NULL, NULL, NULL);
    fprintf(stdout, "read sign from `%s`\n", PUBLIC_KEY_FILE);

    // 获取文件内容和长度
    memset(file_buf, 0, MAX_BUF_LENGTH);
    FILE *fp = fopen(FILE_TO_SIGN, "rb");
    long file_length = fread(file_buf, sizeof(char), MAX_BUF_LENGTH, fp);
    fclose(fp);
    fprintf(stdout, "File `%s` size: %ld Bytes\n", FILE_TO_SIGN, file_length);

    // 获取MD5哈希值
    memset(hash, 0, MAX_BUF_LENGTH);
    MD5(file_buf, file_length, hash);
    unsigned char *ch = hash;
    fprintf(stdout, "MD5 hash of `%s` = ", FILE_TO_SIGN);
    while (*ch != '\0'){
        printf("%02x", *ch++);
    }printf("\n");
    long hash_length = ch - hash;
    printf("hash length: %ld\n", hash_length);

    // 公钥解密签名
    memset(sign, 0, MAX_BUF_LENGTH);
    fp = fopen(FILE_SIGN, "rb");
    unsigned int sign_length = fread(sign, sizeof(char), MAX_BUF_LENGTH, fp);
    if (1 != RSA_verify(
        NID_md5, hash, hash_length,
        sign, sign_length, pub_key)){
        fprintf(stdout, "RSA_verify failed!\n");
        exit(-1);
    }else{
        fprintf(stdout, "RSA_verify OK!\n");
    }
    return 0;
}

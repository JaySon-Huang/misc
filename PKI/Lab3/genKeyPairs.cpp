#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

const int BIT_LENGTH = 2048;
const int E = 1000007;
const char *PRIVATE_KEY_OUTFILE = "rsa_pri.key";
const char *PUBLIC_KEY_OUTFILE = "rsa_pub.key";
const char *PHRASE = "JaySon";
// 编译命令:
// clang genKeyPairs.cpp -o genKeyPairs `pkg-config openssl --cflags --libs`

// PEM_write_bio_RSAPrivateKey 说明: http://www.openssl.org/docs/crypto/pem.html

int main(int argc, char const *argv[])
{
    // 必须添加
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    RSA *rsa, *pub_key, *pri_key;
    rsa = RSA_generate_key(BIT_LENGTH, E, NULL, NULL);
    
    pri_key = RSAPrivateKey_dup(rsa);
    BIO *out;
    out = BIO_new_file(PRIVATE_KEY_OUTFILE, "wb");
    int ret;
    ret = PEM_write_bio_RSAPrivateKey(
        out, pri_key, 
        EVP_des_ede3_cbc(), NULL, 0, 0, (void*)PHRASE
    );
    BIO_free(out);

    pub_key = RSAPublicKey_dup(rsa);
    out = BIO_new_file(PUBLIC_KEY_OUTFILE, "wb");
    ret = PEM_write_bio_RSAPublicKey(out, pub_key);

    return 0;
}

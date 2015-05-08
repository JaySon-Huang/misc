#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

// 编译命令:
// clang main.cpp -o verifyCert `pkg-config openssl --cflags --libs`

int main(int argc, char const *argv[])
{
    // 必须添加, 否则会验证出错
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    BIO *b;
    X509 *client_crt, *ca_crt;
    /** 载入client证书 **/
    /* PEM格式证书的读取 */
    // b = BIO_new_file("client.pem", "rb");
    // client_crt = PEM_read_bio_X509(b, NULL, NULL, NULL);
    // BIO_free(b);
    // b = BIO_new(BIO_s_file());
    // BIO_set_fp(b, stdout, BIO_NOCLOSE);
    // X509_print(b, client_crt);
    /* DER格式证书的读取 */
    b = BIO_new_file("client.der", "rb");
    client_crt = d2i_X509_bio(b, NULL);
    BIO_free(b);
    if (client_crt == NULL){
        fprintf(stderr, "d2i_X509_bio(`%s`) failed!\n", "client.crt");
        exit(-1);
    }
    b = BIO_new(BIO_s_file());
    BIO_set_fp(b, stdout, BIO_NOCLOSE);
    X509_print(b, client_crt);
    BIO_free(b);

    /** 载入CA证书 **/
    // b = BIO_new_file("ca.pem", "rb");
    // ca_crt = PEM_read_bio_X509(b, NULL, NULL, NULL);
    // BIO_free(b);
    // b = BIO_new(BIO_s_file());
    // BIO_set_fp(b, stdout, BIO_NOCLOSE);
    // X509_print(b, ca_crt);
    b = BIO_new_file("ca.der", "rb");
    ca_crt = d2i_X509_bio(b, NULL);
    BIO_free(b);
    if (ca_crt == NULL){
        fprintf(stderr, "d2i_X509_bio(`%s`) failed!\n", "ca.crt");
        exit(-1);
    }
    b = BIO_new(BIO_s_file());
    BIO_set_fp(b, stdout, BIO_NOCLOSE);
    X509_print(b, ca_crt);
    BIO_free(b);

    // 把CA证书放到存储区
    X509_STORE *ca_store = X509_STORE_new();
    if (X509_STORE_add_cert(ca_store, ca_crt) != 1){
        fprintf(stderr, "X509_STORE_add_cert failed!\n");
        exit(-1);
    }

    X509_STORE_CTX *ctx = X509_STORE_CTX_new();
    if (X509_STORE_CTX_init(ctx, ca_store, client_crt, NULL) != 1){
    // if (X509_STORE_CTX_init(ctx, ca_store, ca_crt, NULL) != 1){
        fprintf(stderr, "X509_STORE_CTX_init failed!\n");
        exit(-1);
    }

    // 验证
    int errorno;
    if (X509_verify_cert(ctx) != 1){
        errorno = X509_STORE_CTX_get_error(ctx);
        fprintf(stderr, 
            "X509_verify_cert failed!\n"
            " error: (%d) %s\n",
            errorno,
            X509_verify_cert_error_string(errorno)
        );
    }else{
        fprintf(stdout, "X509_verify_cert OK!\n");
    }
    return 0;
}

#ifndef TLS_CLIENT_OLD
#define TLS_CLIENT_OLD

#include <stdint.h>
#include <stdbool.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "TcpClient.h"

typedef struct
{
    uint8_t data[8 * 1024];
    int32_t size;
} read_buff_t;

typedef struct
{
    mbedtls_ssl_context *ssl;
    mbedtls_net_context *net;
    tcp::TcpClient client;

} io_ctx_t;

class SimpleTlsClient
{
public:
    bool Connect(const char *server_name);
    bool Request(const uint8_t *data, uint32_t size, read_buff_t *read_buf);
    void Close();
    void WaitData(read_buff_t *read_buf);   
    bool Read(read_buff_t *read_buf);
    bool Write(const uint8_t *data, uint32_t size);
    bool WebSocketHandshake(const std::string &path, read_buff_t *read_buf);
private:
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    io_ctx_t io_ctx;
};

#endif // TLS_CLIENT_OLD

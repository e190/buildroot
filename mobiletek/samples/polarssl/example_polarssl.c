#include <stdio.h>
#include "mbtk_log.h"
#include "ql_mcm_sim.h"
#include <sys/socket.h>
#include <polarssl/net.h>
#include <polarssl/ssl.h>
#include <polarssl/entropy.h>
#include <polarssl/ctr_drbg.h>
#include <polarssl/certs.h>
#include <polarssl/x509.h>
#include <polarssl/error.h>
#include <polarssl/debug.h>
#include <polarssl/config.h>

#define DFL_SERVER_NAME         "asr"
#define DFL_SERVER_ADDR         NULL
#define DFL_SERVER_PORT         4433
#define DFL_REQUEST_PAGE        "/"
#define DFL_REQUEST_SIZE        -1
#define DFL_DEBUG_LEVEL         0
#define DFL_NBIO                0
#define DFL_CA_FILE             "/ca.crt"
#define DFL_CA_PATH             "/ca.crt"
#define DFL_CRT_FILE            "/client.crt"
#define DFL_KEY_FILE            "/client.key"
#define DFL_PSK                 ""
#define DFL_PSK_IDENTITY        "Client_identity"
#define DFL_FORCE_CIPHER        0
#define DFL_RENEGOTIATION       SSL_RENEGOTIATION_DISABLED
#define DFL_ALLOW_LEGACY        SSL_LEGACY_NO_RENEGOTIATION
#define DFL_RENEGOTIATE         0
#define DFL_EXCHANGES           1
#define DFL_MIN_VERSION         SSL_MINOR_VERSION_3
#define DFL_MAX_VERSION         SSL_MINOR_VERSION_3
#define DFL_AUTH_MODE           SSL_VERIFY_REQUIRED
#define DFL_MFL_CODE            SSL_MAX_FRAG_LEN_NONE
#define DFL_TRUNC_HMAC          0
#define DFL_RECONNECT           0
#define DFL_RECO_DELAY          0
#define DFL_TICKETS             SSL_SESSION_TICKETS_ENABLED
#define DFL_ALPN_STRING         NULL

#define GET_REQUEST "GET %s HTTP/1.0\r\nExtra-header: "
#define GET_REQUEST_END "\r\n\r\n"

#define  CA_CERT \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDKjCCAhICCQCOewfZiRCiNjANBgkqhkiG9w0BAQUFADBXMQswCQYDVQQGEwJD\r\n" \
"TjEQMA4GA1UECBMHU2lDaHVhbjEVMBMGA1UEChMMTU9CSUxFVEVLLkNBMQswCQYD\r\n" \
"VQQLEwJJVDESMBAGA1UEAxMJTU9CSUxFVEVLMB4XDTE4MDkxODA4MDUzMloXDTMz\r\n" \
"MDkxOTA4MDUzMlowVzELMAkGA1UEBhMCQ04xEDAOBgNVBAgTB1NpQ2h1YW4xFTAT\r\n" \
"BgNVBAoTDE1PQklMRVRFSy5DQTELMAkGA1UECxMCSVQxEjAQBgNVBAMTCU1PQklM\r\n" \
"RVRFSzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOkdYJF1h1xjKbY0\r\n" \
"ipbl88G653PiCh8ZMjmIUYeiDKC8+0wtXZtHvQIl6AncOzBy9XHVOctbKn34exC8\r\n" \
"SEotMuo2T49vs9VtE8GYu2pOrf3m42NpLRnYAxfm9qw53CMHx+Jn7Oa9fnxa8haA\r\n" \
"pRc2BTVadWGoS8EEwoZFk0eNb7Z2Gc7U0c+GhISI4oVTTocGvGgMzkvduu5JJbbc\r\n" \
"BOcNFrii9sRO9vtOYQtqOEg01Uum2Dwp/o2bDLXNJEqAIh4WACiM4iPmmlRHWT2y\r\n" \
"NjQ3vcbEdrFwbHRtO46+Vw54HnSyCoFb3uCHMNMvXObZ/8AU9E3Cgat4j0sgEeB0\r\n" \
"hqA4MiMCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEAtEAjf0CjsLgG9ROdmp1qXYft\r\n" \
"+ndIT5l82KRK57ZQsfdFbnJOvALeF/ICKU0M2TXgJNiGOA5RxDi00YYdMbOIPwVZ\r\n" \
"JH4b87J/LYdLAGf+Q+kVI6gWH3hPm4Jzfzq/40KVrf3mpa54yWz6ZYtwfxBjrMgr\r\n" \
"IVe0O5SIJ99lsddgzgUkqYN2vWJW2zZ50xuXOAyo+pOnjzX0wuOcaBT3JCHWJRAb\r\n" \
"VhJCf9JbswDgnddJerqFtB8pnpAOdGokLCOoM06q3s3P9mhGX+72HXdX7G8CSAuG\r\n" \
"PVCGf6RaF0/G4B9R1c3du3lZRlQWfx2pxyU0LS86iFQFWqzqcWEXIcULVdcErQ==\r\n" \
"-----END CERTIFICATE-----\r\n"

const char ca1_cert[]= CA_CERT;


struct options
{
    const char *server_name;    /* hostname of the server (client only)     */
    const char *server_addr;    /* address of the server (client only)      */
    int server_port;            /* port on which the ssl service runs       */
    int debug_level;            /* level of debugging                       */
    int nbio;                   /* should I/O be blocking?                  */
    const char *request_page;   /* page on server to request                */
    int request_size;           /* pad request with header to requested size */
    const char *ca_file;        /* the file with the CA certificate(s)      */
    const char *ca_path;        /* the path with the CA certificate(s) reside */
    const char *crt_file;       /* the file with the client certificate     */
    const char *key_file;       /* the file with the client key             */
    const char *psk;            /* the pre-shared key                       */
    const char *psk_identity;   /* the pre-shared key identity              */
    int force_ciphersuite[2];   /* protocol/ciphersuite to use, or all      */
    int renegotiation;          /* enable / disable renegotiation           */
    int allow_legacy;           /* allow legacy renegotiation               */
    int renegotiate;            /* attempt renegotiation?                   */
    int renego_delay;           /* delay before enforcing renegotiation     */
    int exchanges;              /* number of data exchanges                 */
    int min_version;            /* minimum protocol version accepted        */
    int max_version;            /* maximum protocol version accepted        */
    int auth_mode;              /* verify mode for connection               */
    unsigned char mfl_code;     /* code for maximum fragment length         */
    int trunc_hmac;             /* negotiate truncated hmac or not          */
    int reconnect;              /* attempt to resume session                */
    int reco_delay;             /* delay in seconds before resuming session */
    int tickets;                /* enable / disable session tickets         */
    const char *alpn_string;    /* ALPN supported protocols                 */
} opt;


static sim_client_handle_type cli_handle;
int server_fd = -1;

static void my_debug( void *ctx, int level, const char *str )
{
    ((void) level);

    fprintf( (FILE *) ctx, "%s", str );
    fflush(  (FILE *) ctx  );
}


static int ssl_client_init()
{
    int ret = 0, len, tail_len, i, written, frags;
    unsigned char buf[SSL_MAX_CONTENT_LEN + 1];
    const char *pers = "ssl_client";

    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
    ssl_context ssl;
    ssl_session saved_session;
    x509_crt cacert;
    x509_crt clicert;
    pk_context pkey;

    memset( &ssl, 0, sizeof( ssl_context ) );
    memset( &saved_session, 0, sizeof( ssl_session ) );
    x509_crt_init( &cacert );
    x509_crt_init( &clicert );
    pk_init( &pkey );

    fflush( stdout );

     /*
     * 0. Initialize the RNG and the session data
     */

    entropy_init( &entropy );
    if( ( ret = ctr_drbg_init( &ctr_drbg, entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        printf( " failed\n  ! ctr_drbg_init returned -0x%x\n", -ret );
        goto exit;
    }

    printf( " ok\n" );

     /*
     * 1.1. Load the trusted CA
     */
    //ret = x509_crt_parse(&cacert,ca1_cert,strlen(ca1_cert));
    ret = x509_crt_parse_file( &cacert, opt.ca_path );
    if( ret < 0 )
    {
        printf( " failed\n  !  ca x509_crt_parse returned -0x%x\n\n", -ret );
        goto exit;
    }
    printf( " ok\n" );

     /*
     * 1.2. Load own certificate and private key
     *
     * (can be skipped if client authentication is not required)
     */

    ret = x509_crt_parse_file( &clicert, opt.crt_file );
    if( ret != 0 )
    {
        printf( " failed\n  !  crt x509_crt_parse returned -0x%x\n\n", -ret );
        goto exit;
    }

    ret = pk_parse_keyfile( &pkey, opt.key_file, NULL);
    if( ret != 0 )
    {
        printf( " failed\n  !  key x509_crt_parse returned -0x%x\n\n", -ret );
        goto exit;
    }

    printf( " ok\n" );

     /*
     * 2. Setup stuff
     */
    printf( "  . Setting up the SSL/TLS structure..." );
    fflush( stdout );

    if( ( ret = ssl_init( &ssl ) ) != 0 )
    {
        printf( " failed\n  ! ssl_init returned -0x%x\n\n", -ret );
        goto exit;
    }

    ssl_set_endpoint( &ssl, SSL_IS_CLIENT );
    ssl_set_authmode( &ssl, opt.auth_mode );

    ssl_set_rng( &ssl, ctr_drbg_random, &ctr_drbg );
    ssl_set_dbg( &ssl, my_debug, stdout );

    ssl_set_bio( &ssl, net_recv, &server_fd, net_send, &server_fd );

    ssl_set_renegotiation( &ssl, opt.renegotiation );
    ssl_legacy_renegotiation( &ssl, opt.allow_legacy );

    ssl_set_ca_chain( &ssl, &cacert, NULL, NULL );

    if( ( ret = ssl_set_own_cert( &ssl, &clicert, &pkey ) ) != 0 )
        {
            printf( " failed\n  ! ssl_set_own_cert returned %d\n\n", ret );
            goto exit;
        }
    if( opt.min_version != -1 )
        ssl_set_min_version( &ssl, SSL_MAJOR_VERSION_3, opt.min_version );
    if( opt.max_version != -1 )
        ssl_set_max_version( &ssl, SSL_MAJOR_VERSION_3, opt.max_version );
    printf( " ok\n" );
    /*
     * 3. Handshake
     */
    printf( "  . Performing the SSL/TLS handshake..." );
    fflush( stdout );

    while( ( ret = ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE )
        {
            printf( " failed\n  ! ssl_handshake returned -0x%x\n", -ret );
            if( ret == POLARSSL_ERR_X509_CERT_VERIFY_FAILED )
                printf(
                    "    Unable to verify the server's certificate. "
                        "Either it is invalid,\n"
                    "    or you didn't set ca_file or ca_path "
                        "to an appropriate value.\n"
                    "    Alternatively, you may want to use "
                        "auth_mode=optional for testing purposes.\n" );
            printf( "\n" );
            goto exit;
        }
    }

    printf( " ok\n    [ Protocol is %s ]\n    [ Ciphersuite is %s ]\n",
            ssl_get_version( &ssl ), ssl_get_ciphersuite( &ssl ) );

    /*
     * 4. Verify the server certificate
     */
    printf( "  . Verifying peer X.509 certificate..." );

    if( ( ret = ssl_get_verify_result( &ssl ) ) != 0 )
    {
        printf( " failed\n" );

        if( ( ret & BADCERT_EXPIRED ) != 0 )
            printf( "  ! server certificate has expired\n" );

        if( ( ret & BADCERT_REVOKED ) != 0 )
            printf( "  ! server certificate has been revoked\n" );

        if( ( ret & BADCERT_CN_MISMATCH ) != 0 )
            printf( "  ! CN mismatch (expected CN=%s)\n", opt.server_name );

        if( ( ret & BADCERT_NOT_TRUSTED ) != 0 )
            printf( "  ! self-signed or not signed by a trusted CA\n" );

        printf( "\n" );
    }
    else
        printf( " ok\n" );

    if( ssl_get_peer_cert( &ssl ) != NULL )
    {
        printf( "  . Peer certificate information    ...\n" );
        x509_crt_info( (char *) buf, sizeof( buf ) - 1, "      ",
                       ssl_get_peer_cert( &ssl ) );
        printf( "%s\n", buf );
    }
    /*
     * 5. Write the GET request
     */
    printf( "  > Write to server:" );
    fflush( stdout );

    len = snprintf( (char *) buf, sizeof(buf) - 1, GET_REQUEST,
                    opt.request_page );
    tail_len = strlen( GET_REQUEST_END );

    /* Add padding to GET request to reach opt.request_size in length */
    if( opt.request_size != DFL_REQUEST_SIZE &&
        len + tail_len < opt.request_size )
    {
        memset( buf + len, 'A', opt.request_size - len - tail_len );
        len += opt.request_size - len - tail_len;
    }

    strncpy( (char *) buf + len, GET_REQUEST_END, sizeof(buf) - len - 1 );
    len += tail_len;

    /* Truncate if request size is smaller than the "natural" size */
    if( opt.request_size != DFL_REQUEST_SIZE &&
        len > opt.request_size )
    {
        len = opt.request_size;

        /* Still end with \r\n unless that's really not possible */
        if( len >= 2 ) buf[len - 2] = '\r';
        if( len >= 1 ) buf[len - 1] = '\n';
    }

    for( written = 0, frags = 0; written < len; written += ret, frags++ )
    {
        while( ( ret = ssl_write( &ssl, buf + written, len - written ) ) <= 0 )
        {
            if( ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE )
            {
                printf( " failed\n  ! ssl_write returned -0x%x\n\n", -ret );
                goto exit;
            }
        }
    }

    buf[written] = '\0';
    printf( " %d bytes written in %d fragments\n\n%s\n", written, frags, (char *) buf );

    /*
     * 6. Read the HTTP response
     */
    printf( "  < Read from server:" );
    fflush( stdout );

    do
    {
        len = sizeof( buf ) - 1;
        memset( buf, 0, sizeof( buf ) );
        ret = ssl_read( &ssl, buf, len );

        if( ret == POLARSSL_ERR_NET_WANT_READ ||
            ret == POLARSSL_ERR_NET_WANT_WRITE )
            continue;

        if( ret <= 0 )
        {
            switch( ret )
            {
                case POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY:
                    printf( " connection was closed gracefully\n" );
                    ret = 0;
                    goto close_notify;

                case 0:
                case POLARSSL_ERR_NET_CONN_RESET:
                    printf( " connection was reset by peer\n" );
                    ret = 0;
                    goto exit;

                default:
                    printf( " ssl_read returned -0x%x\n", -ret );
                    goto exit;
            }
        }

        len = ret;
        buf[len] = '\0';
        printf( " %d bytes read\n\n%s", len, (char *) buf );

        /* End of message should be detected according to the syntax of the
         * application protocol (eg HTTP), just use a dummy test here. */
        if( ret > 0 && buf[len-1] == '\n' )
        {
            ret = 0;
            break;
        }
    }
    while( 1 );

    /*
     * 7. Done, cleanly close the connection
     */
close_notify:
        printf( "  . Closing the connection..." );

        while( ( ret = ssl_close_notify( &ssl ) ) < 0 )
        {
            if( ret == POLARSSL_ERR_NET_CONN_RESET )
            {
                printf( " ok (already closed by peer)\n" );
                ret = 0;
                goto exit;
            }

            if( ret != POLARSSL_ERR_NET_WANT_READ &&
                ret != POLARSSL_ERR_NET_WANT_WRITE )
            {
                printf( " failed\n  ! ssl_close_notify returned %d\n\n", ret );
                goto exit;
            }
        }

        printf( " ok\n" );
exit:
    if( server_fd )
        net_close( server_fd );

    x509_crt_free( &clicert );
    x509_crt_free( &cacert );
    pk_free( &pkey );
    ssl_session_free( &saved_session );
    ssl_free( &ssl );
    ctr_drbg_free( &ctr_drbg );
    entropy_free( &entropy );

    printf( " ok end\n" );
    return 0;
}

int main(int argc, char *argv[])
{
    printf("Start!\n");

    opt.server_name         = DFL_SERVER_NAME;
    opt.server_addr         = DFL_SERVER_ADDR;
    opt.server_port         = DFL_SERVER_PORT;
    opt.debug_level         = DFL_DEBUG_LEVEL;
    opt.nbio                = DFL_NBIO;
    opt.request_page        = DFL_REQUEST_PAGE;
    opt.request_size        = DFL_REQUEST_SIZE;
    opt.ca_file             = DFL_CA_FILE;
    opt.ca_path             = DFL_CA_PATH;
    opt.crt_file            = DFL_CRT_FILE;
    opt.key_file            = DFL_KEY_FILE;
    opt.psk                 = DFL_PSK;
    opt.psk_identity        = DFL_PSK_IDENTITY;
    opt.force_ciphersuite[0]= DFL_FORCE_CIPHER;
    opt.renegotiation       = DFL_RENEGOTIATION;
    opt.allow_legacy        = DFL_ALLOW_LEGACY;
    opt.renegotiate         = DFL_RENEGOTIATE;
    opt.exchanges           = DFL_EXCHANGES;
    opt.min_version         = DFL_MIN_VERSION;
    opt.max_version         = DFL_MAX_VERSION;
    opt.auth_mode           = DFL_AUTH_MODE;
    opt.mfl_code            = DFL_MFL_CODE;
    opt.trunc_hmac          = DFL_TRUNC_HMAC;
    opt.reconnect           = DFL_RECONNECT;
    opt.reco_delay          = DFL_RECO_DELAY;
    opt.tickets             = DFL_TICKETS;
    opt.alpn_string         = DFL_ALPN_STRING;


    if(argc < 3)
    {
        printf("input error \n example: mbtk_test ip prot\n");
        return -1;
    }
    opt.server_addr = argv[1];
    opt.server_port = atoi(argv[2]);

    int ret = -1;
    if( ( ret = net_connect( &server_fd, opt.server_addr,
                                         opt.server_port ) ) != 0 )
    {
        printf( " failed\n  ! net_connect returned -0x%x\n\n", -ret );
        return -1;
    }

    ret = net_set_nonblock( server_fd );
    if( ret != 0 )
    {
        printf( " failed\n  ! net_set_(non)block() returned -0x%x\n\n", -ret );
        return -1;
    }
    printf( " ok\n" );
    ret = ssl_client_init();
    printf("ret is %d\n",ret);
    printf("End!\n");
    return 0;
}

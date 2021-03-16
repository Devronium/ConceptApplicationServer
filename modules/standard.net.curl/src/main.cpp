//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <stdlib.h>
extern "C" {
#include <curl/curl.h>
}

typedef struct {
    struct curl_httppost *post;
    void                 *write_delegate;
    void                 *progress_delegate;
    void                 *read_delegate;
    void                 *sockopt_delegate;
    void                 *debug_delegate;
    void                 *conv_delegate;
    void                 *ssl_ctx_delegate;
    void                 *header_delegate;
    void                 *ioctl_delegate;

    int                  write_data;
    int                  read_data;
    int                  header_data;
} ConceptCURLHandle;

//=====================================================================================//
INVOKE_CALL InvokePtr = 0;
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    // DEFINE_ECONSTANT(HTTPPOST_FILENAME)
    // DEFINE_ECONSTANT(HTTPPOST_READFILE)
    // DEFINE_ECONSTANT(HTTPPOST_PTRNAME)
    // DEFINE_ECONSTANT(HTTPPOST_PTRCONTENTS)
    // DEFINE_ECONSTANT(HTTPPOST_BUFFER)
    // DEFINE_ECONSTANT(HTTPPOST_PTRBUFFER)

    DEFINE_ECONSTANT(CURL_READFUNC_ABORT)

    DEFINE_ECONSTANT(CURLIOE_OK)
    DEFINE_ECONSTANT(CURLIOE_UNKNOWNCMD)
    DEFINE_ECONSTANT(CURLIOE_FAILRESTART)
    DEFINE_ECONSTANT(CURLIOE_LAST)

    DEFINE_ECONSTANT(CURLIOCMD_NOP)
    DEFINE_ECONSTANT(CURLIOCMD_RESTARTREAD)
    DEFINE_ECONSTANT(CURLIOCMD_LAST)

    DEFINE_ECONSTANT(CURLINFO_TEXT)
    DEFINE_ECONSTANT(CURLINFO_HEADER_IN)
    DEFINE_ECONSTANT(CURLINFO_HEADER_OUT)
    DEFINE_ECONSTANT(CURLINFO_DATA_IN)
    DEFINE_ECONSTANT(CURLINFO_DATA_OUT)
    DEFINE_ECONSTANT(CURLINFO_SSL_DATA_IN)
    DEFINE_ECONSTANT(CURLINFO_SSL_DATA_OUT)
    DEFINE_ECONSTANT(CURLINFO_END)


    DEFINE_ECONSTANT(CURLE_OK)
    DEFINE_ECONSTANT(CURLE_UNSUPPORTED_PROTOCOL)
    DEFINE_ECONSTANT(CURLE_FAILED_INIT)
    DEFINE_ECONSTANT(CURLE_URL_MALFORMAT)
    DEFINE_ECONSTANT(CURLE_URL_MALFORMAT_USER)
    DEFINE_ECONSTANT(CURLE_COULDNT_RESOLVE_PROXY)
    DEFINE_ECONSTANT(CURLE_COULDNT_RESOLVE_HOST)
    DEFINE_ECONSTANT(CURLE_COULDNT_CONNECT)
    DEFINE_ECONSTANT(CURLE_FTP_WEIRD_SERVER_REPLY)
    DEFINE_ECONSTANT(CURLE_FTP_ACCESS_DENIED)
    DEFINE_ECONSTANT(CURLE_FTP_USER_PASSWORD_INCORRECT)
    DEFINE_ECONSTANT(CURLE_FTP_WEIRD_PASS_REPLY)
    DEFINE_ECONSTANT(CURLE_FTP_WEIRD_USER_REPLY)
    DEFINE_ECONSTANT(CURLE_FTP_WEIRD_PASV_REPLY)
    DEFINE_ECONSTANT(CURLE_FTP_WEIRD_227_FORMAT)
    DEFINE_ECONSTANT(CURLE_FTP_CANT_GET_HOST)
    DEFINE_ECONSTANT(CURLE_FTP_CANT_RECONNECT)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_SET_BINARY)
    DEFINE_ECONSTANT(CURLE_PARTIAL_FILE)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_RETR_FILE)
    DEFINE_ECONSTANT(CURLE_FTP_WRITE_ERROR)
    DEFINE_ECONSTANT(CURLE_FTP_QUOTE_ERROR)
    DEFINE_ECONSTANT(CURLE_HTTP_RETURNED_ERROR)
    DEFINE_ECONSTANT(CURLE_WRITE_ERROR)
    DEFINE_ECONSTANT(CURLE_MALFORMAT_USER)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_STOR_FILE)
    DEFINE_ECONSTANT(CURLE_READ_ERROR)
    DEFINE_ECONSTANT(CURLE_OUT_OF_MEMORY)
    DEFINE_ECONSTANT(CURLE_OPERATION_TIMEOUTED)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_SET_ASCII)
    DEFINE_ECONSTANT(CURLE_FTP_PORT_FAILED)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_USE_REST)
    DEFINE_ECONSTANT(CURLE_FTP_COULDNT_GET_SIZE)
    DEFINE_ECONSTANT(CURLE_HTTP_RANGE_ERROR)
    DEFINE_ECONSTANT(CURLE_HTTP_POST_ERROR)
    DEFINE_ECONSTANT(CURLE_SSL_CONNECT_ERROR)
    DEFINE_ECONSTANT(CURLE_BAD_DOWNLOAD_RESUME)
    DEFINE_ECONSTANT(CURLE_FILE_COULDNT_READ_FILE)
    DEFINE_ECONSTANT(CURLE_LDAP_CANNOT_BIND)
    DEFINE_ECONSTANT(CURLE_LDAP_SEARCH_FAILED)
    DEFINE_ECONSTANT(CURLE_LIBRARY_NOT_FOUND)
    DEFINE_ECONSTANT(CURLE_FUNCTION_NOT_FOUND)
    DEFINE_ECONSTANT(CURLE_ABORTED_BY_CALLBACK)
    DEFINE_ECONSTANT(CURLE_BAD_FUNCTION_ARGUMENT)
    DEFINE_ECONSTANT(CURLE_BAD_CALLING_ORDER)
    DEFINE_ECONSTANT(CURLE_INTERFACE_FAILED)
    DEFINE_ECONSTANT(CURLE_BAD_PASSWORD_ENTERED)
    DEFINE_ECONSTANT(CURLE_TOO_MANY_REDIRECTS)
    DEFINE_ECONSTANT(CURLE_UNKNOWN_TELNET_OPTION)
    DEFINE_ECONSTANT(CURLE_TELNET_OPTION_SYNTAX)
    DEFINE_ECONSTANT(CURLE_OBSOLETE)
    DEFINE_ECONSTANT(CURLE_SSL_PEER_CERTIFICATE)
    DEFINE_ECONSTANT(CURLE_GOT_NOTHING)
    DEFINE_ECONSTANT(CURLE_SSL_ENGINE_NOTFOUND)
    DEFINE_ECONSTANT(CURLE_SSL_ENGINE_SETFAILED)
    DEFINE_ECONSTANT(CURLE_SEND_ERROR)
    DEFINE_ECONSTANT(CURLE_RECV_ERROR)
    DEFINE_ECONSTANT(CURLE_SHARE_IN_USE)
    DEFINE_ECONSTANT(CURLE_SSL_CERTPROBLEM)
    DEFINE_ECONSTANT(CURLE_SSL_CIPHER)
    DEFINE_ECONSTANT(CURLE_SSL_CACERT)
    DEFINE_ECONSTANT(CURLE_BAD_CONTENT_ENCODING)
    DEFINE_ECONSTANT(CURLE_LDAP_INVALID_URL)
    DEFINE_ECONSTANT(CURLE_FILESIZE_EXCEEDED)
    DEFINE_ECONSTANT(CURLE_FTP_SSL_FAILED)
    DEFINE_ECONSTANT(CURLE_SEND_FAIL_REWIND)
    DEFINE_ECONSTANT(CURLE_SSL_ENGINE_INITFAILED)
    DEFINE_ECONSTANT(CURLE_LOGIN_DENIED)
    DEFINE_ECONSTANT(CURLE_TFTP_NOTFOUND)
    DEFINE_ECONSTANT(CURLE_TFTP_PERM)
    DEFINE_ECONSTANT(CURLE_TFTP_DISKFULL)
    DEFINE_ECONSTANT(CURLE_TFTP_ILLEGAL)
    DEFINE_ECONSTANT(CURLE_TFTP_UNKNOWNID)
    DEFINE_ECONSTANT(CURLE_TFTP_EXISTS)
    DEFINE_ECONSTANT(CURLE_TFTP_NOSUCHUSER)
    DEFINE_ECONSTANT(CURLE_CONV_FAILED)
    DEFINE_ECONSTANT(CURLE_CONV_REQD)
    DEFINE_ECONSTANT(CURLE_SSL_CACERT_BADFILE)
    DEFINE_ECONSTANT(CURL_LAST)

    DEFINE_ECONSTANT(CURLPROXY_HTTP)
    DEFINE_ECONSTANT(CURLPROXY_SOCKS4)
    DEFINE_ECONSTANT(CURLPROXY_SOCKS5)

    DEFINE_ECONSTANT(CURLAUTH_NONE)
    DEFINE_ECONSTANT(CURLAUTH_BASIC)
    DEFINE_ECONSTANT(CURLAUTH_DIGEST)
    DEFINE_ECONSTANT(CURLAUTH_GSSNEGOTIATE)
    DEFINE_ECONSTANT(CURLAUTH_NTLM)
    DEFINE_ECONSTANT(CURLAUTH_ANY)
    DEFINE_ECONSTANT(CURLAUTH_ANYSAFE)

    DEFINE_ECONSTANT(CURLE_ALREADY_COMPLETE);

    DEFINE_ECONSTANT(CURLE_FTP_PARTIAL_FILE);
    DEFINE_ECONSTANT(CURLE_FTP_BAD_DOWNLOAD_RESUME);

    DEFINE_ECONSTANT(CURL_ERROR_SIZE);



    DEFINE_ECONSTANT(CURLFTPSSL_NONE)
    DEFINE_ECONSTANT(CURLFTPSSL_TRY)
    DEFINE_ECONSTANT(CURLFTPSSL_CONTROL)
    DEFINE_ECONSTANT(CURLFTPSSL_ALL)
    DEFINE_ECONSTANT(CURLFTPSSL_LAST)

    DEFINE_ECONSTANT(CURLFTPAUTH_DEFAULT)
    DEFINE_ECONSTANT(CURLFTPAUTH_SSL)
    DEFINE_ECONSTANT(CURLFTPAUTH_TLS)
    DEFINE_ECONSTANT(CURLFTPAUTH_LAST)

    DEFINE_ECONSTANT(CURLFTPMETHOD_DEFAULT)
    DEFINE_ECONSTANT(CURLFTPMETHOD_MULTICWD)
    DEFINE_ECONSTANT(CURLFTPMETHOD_NOCWD)
    DEFINE_ECONSTANT(CURLFTPMETHOD_SINGLECWD)
    DEFINE_ECONSTANT(CURLFTPMETHOD_LAST)

    DEFINE_ECONSTANT(CURLOPTTYPE_LONG)
    DEFINE_ECONSTANT(CURLOPTTYPE_OBJECTPOINT)
    DEFINE_ECONSTANT(CURLOPTTYPE_FUNCTIONPOINT)
    DEFINE_ECONSTANT(CURLOPTTYPE_OFF_T)

    DEFINE_ECONSTANT(CURL_IPRESOLVE_WHATEVER)
    DEFINE_ECONSTANT(CURL_IPRESOLVE_V4)
    DEFINE_ECONSTANT(CURL_IPRESOLVE_V6)

    DEFINE_ECONSTANT(CURLOPT_WRITEDATA)
    DEFINE_ECONSTANT(CURLOPT_READDATA)
    DEFINE_ECONSTANT(CURLOPT_HEADERDATA)


    DEFINE_ECONSTANT(CURL_HTTP_VERSION_NONE)
    DEFINE_ECONSTANT(CURL_HTTP_VERSION_1_0)
    DEFINE_ECONSTANT(CURL_HTTP_VERSION_1_1)
    DEFINE_ECONSTANT(CURL_HTTP_VERSION_LAST)

    DEFINE_ECONSTANT(CURL_NETRC_IGNORED)
    DEFINE_ECONSTANT(CURL_NETRC_OPTIONAL)
    DEFINE_ECONSTANT(CURL_NETRC_REQUIRED)
    DEFINE_ECONSTANT(CURL_NETRC_LAST)

    DEFINE_ECONSTANT(CURL_SSLVERSION_DEFAULT)
    DEFINE_ECONSTANT(CURL_SSLVERSION_TLSv1)
    DEFINE_ECONSTANT(CURL_SSLVERSION_SSLv2)
    DEFINE_ECONSTANT(CURL_SSLVERSION_SSLv3)
    DEFINE_ECONSTANT(CURL_SSLVERSION_LAST)


    DEFINE_ECONSTANT(CURL_TIMECOND_NONE)
    DEFINE_ECONSTANT(CURL_TIMECOND_IFMODSINCE)
    DEFINE_ECONSTANT(CURL_TIMECOND_IFUNMODSINCE)
    DEFINE_ECONSTANT(CURL_TIMECOND_LASTMOD)
    DEFINE_ECONSTANT(CURL_TIMECOND_LAST)

    DEFINE_ECONSTANT(CURL_FORMADD_OK)
    DEFINE_ECONSTANT(CURL_FORMADD_MEMORY)
    DEFINE_ECONSTANT(CURL_FORMADD_OPTION_TWICE)
    DEFINE_ECONSTANT(CURL_FORMADD_NULL)
    DEFINE_ECONSTANT(CURL_FORMADD_UNKNOWN_OPTION)
    DEFINE_ECONSTANT(CURL_FORMADD_INCOMPLETE)
    DEFINE_ECONSTANT(CURL_FORMADD_ILLEGAL_ARRAY)
    DEFINE_ECONSTANT(CURL_FORMADD_DISABLED)
    DEFINE_ECONSTANT(CURL_FORMADD_LAST)

    DEFINE_ECONSTANT(CURLINFO_STRING)
    DEFINE_ECONSTANT(CURLINFO_LONG)
    DEFINE_ECONSTANT(CURLINFO_DOUBLE)
    DEFINE_ECONSTANT(CURLINFO_SLIST)
    DEFINE_ECONSTANT(CURLINFO_MASK)
    DEFINE_ECONSTANT(CURLINFO_TYPEMASK)

    DEFINE_ECONSTANT(CURLINFO_NONE)
    DEFINE_ECONSTANT(CURLINFO_EFFECTIVE_URL)
    DEFINE_ECONSTANT(CURLINFO_RESPONSE_CODE)
    DEFINE_ECONSTANT(CURLINFO_TOTAL_TIME)
    DEFINE_ECONSTANT(CURLINFO_NAMELOOKUP_TIME)
    DEFINE_ECONSTANT(CURLINFO_CONNECT_TIME)
    DEFINE_ECONSTANT(CURLINFO_PRETRANSFER_TIME)
    DEFINE_ECONSTANT(CURLINFO_SIZE_UPLOAD)
    DEFINE_ECONSTANT(CURLINFO_SIZE_DOWNLOAD)
    DEFINE_ECONSTANT(CURLINFO_SPEED_DOWNLOAD)
    DEFINE_ECONSTANT(CURLINFO_SPEED_UPLOAD)
    DEFINE_ECONSTANT(CURLINFO_HEADER_SIZE)
    DEFINE_ECONSTANT(CURLINFO_REQUEST_SIZE)
    DEFINE_ECONSTANT(CURLINFO_SSL_VERIFYRESULT)
    DEFINE_ECONSTANT(CURLINFO_FILETIME)
    DEFINE_ECONSTANT(CURLINFO_CONTENT_LENGTH_DOWNLOAD)
    DEFINE_ECONSTANT(CURLINFO_CONTENT_LENGTH_UPLOAD)
    DEFINE_ECONSTANT(CURLINFO_STARTTRANSFER_TIME)
    DEFINE_ECONSTANT(CURLINFO_CONTENT_TYPE)
    DEFINE_ECONSTANT(CURLINFO_REDIRECT_TIME)
    DEFINE_ECONSTANT(CURLINFO_REDIRECT_COUNT)
    DEFINE_ECONSTANT(CURLINFO_PRIVATE)
    DEFINE_ECONSTANT(CURLINFO_HTTP_CONNECTCODE)
    DEFINE_ECONSTANT(CURLINFO_HTTPAUTH_AVAIL)
    DEFINE_ECONSTANT(CURLINFO_PROXYAUTH_AVAIL)
    DEFINE_ECONSTANT(CURLINFO_OS_ERRNO)
    DEFINE_ECONSTANT(CURLINFO_NUM_CONNECTS)
    DEFINE_ECONSTANT(CURLINFO_SSL_ENGINES)
    DEFINE_ECONSTANT(CURLINFO_COOKIELIST)
    DEFINE_ECONSTANT(CURLINFO_LASTSOCKET)
    DEFINE_ECONSTANT(CURLINFO_FTP_ENTRY_PATH)

    DEFINE_ECONSTANT(CURLINFO_HTTP_CODE);

    DEFINE_ECONSTANT(CURLCLOSEPOLICY_NONE)

    DEFINE_ECONSTANT(CURLCLOSEPOLICY_OLDEST)
    DEFINE_ECONSTANT(CURLCLOSEPOLICY_LEAST_RECENTLY_USED)
    DEFINE_ECONSTANT(CURLCLOSEPOLICY_LEAST_TRAFFIC)
    DEFINE_ECONSTANT(CURLCLOSEPOLICY_SLOWEST)
    DEFINE_ECONSTANT(CURLCLOSEPOLICY_CALLBACK)

    DEFINE_ECONSTANT(CURLCLOSEPOLICY_LAST)

    DEFINE_ECONSTANT(CURL_GLOBAL_SSL)
    DEFINE_ECONSTANT(CURL_GLOBAL_WIN32)
    DEFINE_ECONSTANT(CURLCLOSEPOLICY_CALLBACK)
    DEFINE_ECONSTANT(CURL_GLOBAL_NOTHING)
    DEFINE_ECONSTANT(CURL_GLOBAL_DEFAULT)
    DEFINE_ECONSTANT(CURL_GLOBAL_ALL)

    DEFINE_ECONSTANT(CURL_LOCK_DATA_NONE)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_SHARE)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_COOKIE)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_DNS)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_SSL_SESSION)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_CONNECT)
    DEFINE_ECONSTANT(CURL_LOCK_DATA_LAST)

    DEFINE_ECONSTANT(CURL_LOCK_ACCESS_NONE)
    DEFINE_ECONSTANT(CURL_LOCK_ACCESS_SHARED)
    DEFINE_ECONSTANT(CURL_LOCK_ACCESS_SINGLE)
    DEFINE_ECONSTANT(CURL_LOCK_ACCESS_LAST)

    DEFINE_ECONSTANT(CURLSHE_OK)
    DEFINE_ECONSTANT(CURLSHE_BAD_OPTION)
    DEFINE_ECONSTANT(CURLSHE_IN_USE)
    DEFINE_ECONSTANT(CURLSHE_INVALID)
    DEFINE_ECONSTANT(CURLSHE_NOMEM)
    DEFINE_ECONSTANT(CURLSHE_LAST)

    DEFINE_ECONSTANT(CURLSHOPT_NONE)
    DEFINE_ECONSTANT(CURLSHOPT_SHARE)
    DEFINE_ECONSTANT(CURLSHOPT_UNSHARE)
    DEFINE_ECONSTANT(CURLSHOPT_LOCKFUNC)
    DEFINE_ECONSTANT(CURLSHOPT_UNLOCKFUNC)
    DEFINE_ECONSTANT(CURLSHOPT_USERDATA)
    DEFINE_ECONSTANT(CURLSHOPT_LAST)

    DEFINE_ECONSTANT(CURLVERSION_FIRST)
    DEFINE_ECONSTANT(CURLVERSION_SECOND)
    DEFINE_ECONSTANT(CURLVERSION_THIRD)
    DEFINE_ECONSTANT(CURLVERSION_LAST)

    DEFINE_ECONSTANT(CURL_VERSION_IPV6)
    DEFINE_ECONSTANT(CURL_VERSION_KERBEROS4)
    DEFINE_ECONSTANT(CURL_VERSION_SSL)
    DEFINE_ECONSTANT(CURL_VERSION_LIBZ)
    DEFINE_ECONSTANT(CURL_VERSION_NTLM)
    DEFINE_ECONSTANT(CURL_VERSION_GSSNEGOTIATE)
    DEFINE_ECONSTANT(CURL_VERSION_DEBUG)
    DEFINE_ECONSTANT(CURL_VERSION_ASYNCHDNS)
    DEFINE_ECONSTANT(CURL_VERSION_SPNEGO)
    DEFINE_ECONSTANT(CURL_VERSION_LARGEFILE)
    DEFINE_ECONSTANT(CURL_VERSION_IDN)
    DEFINE_ECONSTANT(CURL_VERSION_SSPI)
    DEFINE_ECONSTANT(CURL_VERSION_CONV)


    DEFINE_ECONSTANT(CURLOPT_FILE)
    DEFINE_ECONSTANT(CURLOPT_URL)
    DEFINE_ECONSTANT(CURLOPT_PORT)
    DEFINE_ECONSTANT(CURLOPT_PROXY)
    DEFINE_ECONSTANT(CURLOPT_USERPWD)
    DEFINE_ECONSTANT(CURLOPT_PROXYUSERPWD)
    DEFINE_ECONSTANT(CURLOPT_RANGE)
    DEFINE_ECONSTANT(CURLOPT_INFILE)
    DEFINE_ECONSTANT(CURLOPT_ERRORBUFFER)
    DEFINE_ECONSTANT(CURLOPT_WRITEFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_READFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_TIMEOUT)
    DEFINE_ECONSTANT(CURLOPT_INFILESIZE)
    DEFINE_ECONSTANT(CURLOPT_POSTFIELDS)
    DEFINE_ECONSTANT(CURLOPT_REFERER)
    DEFINE_ECONSTANT(CURLOPT_FTPPORT)
    DEFINE_ECONSTANT(CURLOPT_USERAGENT)
    DEFINE_ECONSTANT(CURLOPT_LOW_SPEED_LIMIT)
    DEFINE_ECONSTANT(CURLOPT_LOW_SPEED_TIME)
    DEFINE_ECONSTANT(CURLOPT_RESUME_FROM)
    DEFINE_ECONSTANT(CURLOPT_COOKIE)
    DEFINE_ECONSTANT(CURLOPT_HTTPHEADER)
    DEFINE_ECONSTANT(CURLOPT_HTTPPOST)
    DEFINE_ECONSTANT(CURLOPT_SSLCERT)
    DEFINE_ECONSTANT(CURLOPT_SSLCERTPASSWD)
    DEFINE_ECONSTANT(CURLOPT_SSLKEYPASSWD)
    DEFINE_ECONSTANT(CURLOPT_CRLF)
    DEFINE_ECONSTANT(CURLOPT_QUOTE)
    DEFINE_ECONSTANT(CURLOPT_WRITEHEADER)
    DEFINE_ECONSTANT(CURLOPT_COOKIEFILE)
    DEFINE_ECONSTANT(CURLOPT_SSLVERSION)
    DEFINE_ECONSTANT(CURLOPT_TIMECONDITION)
    DEFINE_ECONSTANT(CURLOPT_TIMEVALUE)
    DEFINE_ECONSTANT(CURLOPT_CUSTOMREQUEST)
    DEFINE_ECONSTANT(CURLOPT_POSTQUOTE)
    DEFINE_ECONSTANT(CURLOPT_WRITEINFO)
    DEFINE_ECONSTANT(CURLOPT_VERBOSE)
    DEFINE_ECONSTANT(CURLOPT_HEADER)
    DEFINE_ECONSTANT(CURLOPT_NOPROGRESS)
    DEFINE_ECONSTANT(CURLOPT_NOBODY)
    DEFINE_ECONSTANT(CURLOPT_FAILONERROR)
    DEFINE_ECONSTANT(CURLOPT_UPLOAD)
    DEFINE_ECONSTANT(CURLOPT_POST)
    DEFINE_ECONSTANT(CURLOPT_FTPLISTONLY)
    DEFINE_ECONSTANT(CURLOPT_FTPAPPEND)
    DEFINE_ECONSTANT(CURLOPT_NETRC)
    DEFINE_ECONSTANT(CURLOPT_FOLLOWLOCATION)
    DEFINE_ECONSTANT(CURLOPT_TRANSFERTEXT)
    DEFINE_ECONSTANT(CURLOPT_PUT)
    DEFINE_ECONSTANT(CURLOPT_PROGRESSFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_PROGRESSDATA)
    DEFINE_ECONSTANT(CURLOPT_AUTOREFERER)
    DEFINE_ECONSTANT(CURLOPT_PROXYPORT)
    DEFINE_ECONSTANT(CURLOPT_POSTFIELDSIZE)
    DEFINE_ECONSTANT(CURLOPT_HTTPPROXYTUNNEL)
    DEFINE_ECONSTANT(CURLOPT_INTERFACE)
    DEFINE_ECONSTANT(CURLOPT_KRB4LEVEL)
    DEFINE_ECONSTANT(CURLOPT_SSL_VERIFYPEER)
    DEFINE_ECONSTANT(CURLOPT_CAINFO)
    DEFINE_ECONSTANT(CURLOPT_MAXREDIRS)
    DEFINE_ECONSTANT(CURLOPT_FILETIME)
    DEFINE_ECONSTANT(CURLOPT_TELNETOPTIONS)
    DEFINE_ECONSTANT(CURLOPT_MAXCONNECTS)
    DEFINE_ECONSTANT(CURLOPT_CLOSEPOLICY)
    DEFINE_ECONSTANT(CURLOPT_FRESH_CONNECT)
    DEFINE_ECONSTANT(CURLOPT_RANDOM_FILE)
    DEFINE_ECONSTANT(CURLOPT_EGDSOCKET)
    DEFINE_ECONSTANT(CURLOPT_CONNECTTIMEOUT)
    DEFINE_ECONSTANT(CURLOPT_HEADERFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_HTTPGET)
    DEFINE_ECONSTANT(CURLOPT_SSL_VERIFYHOST)
    DEFINE_ECONSTANT(CURLOPT_COOKIEJAR)
    DEFINE_ECONSTANT(CURLOPT_SSL_CIPHER_LIST)
    DEFINE_ECONSTANT(CURLOPT_HTTP_VERSION)
    DEFINE_ECONSTANT(CURLOPT_FTP_USE_EPSV)
    DEFINE_ECONSTANT(CURLOPT_SSLCERTTYPE)
    DEFINE_ECONSTANT(CURLOPT_SSLKEY)
    DEFINE_ECONSTANT(CURLOPT_SSLKEYTYPE)
    DEFINE_ECONSTANT(CURLOPT_SSLENGINE)
    DEFINE_ECONSTANT(CURLOPT_SSLENGINE_DEFAULT)
    DEFINE_ECONSTANT(CURLOPT_DNS_USE_GLOBAL_CACHE)
    DEFINE_ECONSTANT(CURLOPT_DNS_CACHE_TIMEOUT)
    DEFINE_ECONSTANT(CURLOPT_PREQUOTE)
    DEFINE_ECONSTANT(CURLOPT_DEBUGFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_DEBUGDATA)
    DEFINE_ECONSTANT(CURLOPT_COOKIESESSION)
    DEFINE_ECONSTANT(CURLOPT_CAPATH)
    DEFINE_ECONSTANT(CURLOPT_BUFFERSIZE)
    DEFINE_ECONSTANT(CURLOPT_NOSIGNAL)
    DEFINE_ECONSTANT(CURLOPT_SHARE)
    DEFINE_ECONSTANT(CURLOPT_PROXYTYPE)
    DEFINE_ECONSTANT(CURLOPT_ENCODING)
    DEFINE_ECONSTANT(CURLOPT_PRIVATE)
    DEFINE_ECONSTANT(CURLOPT_HTTP200ALIASES)
    DEFINE_ECONSTANT(CURLOPT_UNRESTRICTED_AUTH)
    DEFINE_ECONSTANT(CURLOPT_FTP_USE_EPRT)
    DEFINE_ECONSTANT(CURLOPT_HTTPAUTH)
    DEFINE_ECONSTANT(CURLOPT_SSL_CTX_FUNCTION)
    DEFINE_ECONSTANT(CURLOPT_SSL_CTX_DATA)
    DEFINE_ECONSTANT(CURLOPT_FTP_CREATE_MISSING_DIRS)
    DEFINE_ECONSTANT(CURLOPT_PROXYAUTH)
    DEFINE_ECONSTANT(CURLOPT_FTP_RESPONSE_TIMEOUT)
    DEFINE_ECONSTANT(CURLOPT_IPRESOLVE)
    DEFINE_ECONSTANT(CURLOPT_MAXFILESIZE)
    DEFINE_ECONSTANT(CURLOPT_INFILESIZE_LARGE)
    DEFINE_ECONSTANT(CURLOPT_RESUME_FROM_LARGE)
    DEFINE_ECONSTANT(CURLOPT_MAXFILESIZE_LARGE)
    DEFINE_ECONSTANT(CURLOPT_NETRC_FILE)
    DEFINE_ECONSTANT(CURLOPT_FTP_SSL)
    DEFINE_ECONSTANT(CURLOPT_POSTFIELDSIZE_LARGE)
    DEFINE_ECONSTANT(CURLOPT_TCP_NODELAY)
    DEFINE_ECONSTANT(CURLOPT_FTPSSLAUTH)
    DEFINE_ECONSTANT(CURLOPT_IOCTLFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_IOCTLDATA)
    DEFINE_ECONSTANT(CURLOPT_FTP_ACCOUNT)
    DEFINE_ECONSTANT(CURLOPT_COOKIELIST)
    DEFINE_ECONSTANT(CURLOPT_IGNORE_CONTENT_LENGTH)
    DEFINE_ECONSTANT(CURLOPT_FTP_SKIP_PASV_IP)
    DEFINE_ECONSTANT(CURLOPT_FTP_FILEMETHOD)
    DEFINE_ECONSTANT(CURLOPT_LOCALPORT)
    DEFINE_ECONSTANT(CURLOPT_LOCALPORTRANGE)
    DEFINE_ECONSTANT(CURLOPT_CONNECT_ONLY)
    DEFINE_ECONSTANT(CURLOPT_CONV_FROM_NETWORK_FUNCTION)
    DEFINE_ECONSTANT(CURLOPT_CONV_TO_NETWORK_FUNCTION)
    DEFINE_ECONSTANT(CURLOPT_CONV_FROM_UTF8_FUNCTION)
    DEFINE_ECONSTANT(CURLOPT_MAX_SEND_SPEED_LARGE)
    DEFINE_ECONSTANT(CURLOPT_MAX_RECV_SPEED_LARGE)
    DEFINE_ECONSTANT(CURLOPT_FTP_ALTERNATIVE_TO_USER)
    DEFINE_ECONSTANT(CURLOPT_SOCKOPTFUNCTION)
    DEFINE_ECONSTANT(CURLOPT_SOCKOPTDATA)
    DEFINE_ECONSTANT(CURLOPT_SSL_SESSIONID_CACHE)

    DEFINE_ECONSTANT(CURLOPT_LASTENTRY)

    DEFINE_ECONSTANT(CURLFORM_NOTHING)
    DEFINE_ECONSTANT(CURLFORM_COPYNAME)
    DEFINE_ECONSTANT(CURLFORM_PTRNAME)
    DEFINE_ECONSTANT(CURLFORM_NAMELENGTH)
    DEFINE_ECONSTANT(CURLFORM_COPYCONTENTS)
    DEFINE_ECONSTANT(CURLFORM_PTRCONTENTS)
    DEFINE_ECONSTANT(CURLFORM_CONTENTSLENGTH)
    DEFINE_ECONSTANT(CURLFORM_FILECONTENT)
    DEFINE_ECONSTANT(CURLFORM_ARRAY)
    DEFINE_ECONSTANT(CURLFORM_OBSOLETE)
    DEFINE_ECONSTANT(CURLFORM_FILE)

    DEFINE_ECONSTANT(CURLFORM_BUFFER)
    DEFINE_ECONSTANT(CURLFORM_BUFFERPTR)
    DEFINE_ECONSTANT(CURLFORM_BUFFERLENGTH)

    DEFINE_ECONSTANT(CURLFORM_CONTENTTYPE)
    DEFINE_ECONSTANT(CURLFORM_CONTENTHEADER)
    DEFINE_ECONSTANT(CURLFORM_FILENAME)
    DEFINE_ECONSTANT(CURLFORM_END)
    DEFINE_ECONSTANT(CURLFORM_OBSOLETE2)

    DEFINE_ECONSTANT(CURLFORM_LASTENTRY)

    InvokePtr = Invoke;
    return 0;
}
//=====================================================================================//
struct curl_httppost *GetForm(ConceptCURLHandle *handle, void *arr, INVOKE_CALL _Invoke) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;

    if (handle->post) {
        curl_formfree(handle->post);
        handle->post = NULL;
    }
    struct curl_httppost *last = NULL;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            const char *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            const char *key = NULL;
            _Invoke(INVOKE_GET_ARRAY_KEY, arr, (INTEGER)i, &key);

            AnsiString temp((long)i);
            AnsiString temp2(nData);

            if (!key)
                key = temp.c_str();

            if (!szData)
                szData = temp2.c_str();

            curl_formadd(&handle->post, &last,
                         CURLFORM_COPYNAME, key,
                         CURLFORM_COPYCONTENTS, szData, CURLFORM_CONTENTSLENGTH, (long)nData, CURLFORM_END);
        }
    }
    return handle->post;
}

//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_init CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "curl_easy_init takes no parameters");

    CURL *curl = curl_easy_init();

    ConceptCURLHandle *handle = (ConceptCURLHandle *)malloc(sizeof(ConceptCURLHandle));
    memset(handle, 0, sizeof(ConceptCURLHandle));
    curl_easy_setopt(curl, CURLOPT_PRIVATE, handle);
    RETURN_NUMBER((SYS_INT)curl);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_global_init CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_global_init takes one parameter (flags)");

    LOCAL_INIT;

    NUMBER flags;
    GET_CHECK_NUMBER(0, flags, "curl_global_init: parameter 1 should be a number");

    RETURN_NUMBER(curl_global_init((int)flags));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_global_cleanup CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "curl_global_cleanup takes no parameters");

    curl_global_cleanup();

    RETURN_NUMBER(0);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_cleanup CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_easy_cleanup takes one parameter (the curl_easy_init returned handle)");

    LOCAL_INIT;

    NUMBER handle;
    GET_CHECK_NUMBER(0, handle, "curl_easy_cleanup: parameter 1 should be a number");

    ConceptCURLHandle *chandle = 0;
    curl_easy_getinfo((CURL *)(SYS_INT)handle, CURLINFO_PRIVATE, &chandle);
    if (chandle) {
        if (chandle->write_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->write_delegate);
        if (chandle->progress_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->progress_delegate);
        if (chandle->read_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->read_delegate);
        if (chandle->sockopt_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->sockopt_delegate);
        if (chandle->debug_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->debug_delegate);
        if (chandle->conv_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->conv_delegate);
        if (chandle->conv_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->conv_delegate);
        if (chandle->ssl_ctx_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->ssl_ctx_delegate);
        if (chandle->header_delegate)
            Invoke(INVOKE_FREE_VARIABLE, chandle->header_delegate);

        free(chandle);
    }

    curl_easy_cleanup((CURL *)(SYS_INT)handle);
    RETURN_NUMBER(0);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_reset CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_easy_reset takes one parameter (the curl_easy_init returned handle)");

    LOCAL_INIT;

    NUMBER handle;
    GET_CHECK_NUMBER(0, handle, "curl_easy_reset: parameter 1 should be a number");

    curl_easy_reset((CURL *)(SYS_INT)handle);
    RETURN_NUMBER(0);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_duphandle CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_easy_duphandle takes one parameter (the curl_easy_init returned handle)");

    LOCAL_INIT;

    NUMBER handle;
    GET_CHECK_NUMBER(0, handle, "curl_easy_duphandle: parameter 1 should be a number");

    RETURN_NUMBER((long)curl_easy_duphandle((CURL *)(SYS_INT)handle));

    return 0;
}
//=====================================================================================//

CONCEPT_DLL_API CONCEPT__curl_easy_strerror CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_easy_strerror takes one parameter (the error code)");

    LOCAL_INIT;

    NUMBER err_code;
    GET_CHECK_NUMBER(0, err_code, "curl_easy_strerror: parameter 1 should be a number");

    RETURN_STRING((char *)curl_easy_strerror((CURLcode)err_code));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_perform CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_easy_perform takes one parameter (the curl_easy_init returned handle)");

    LOCAL_INIT;

    NUMBER handle;
    GET_CHECK_NUMBER(0, handle, "curl_easy_perform: parameter 1 should be a number");

    RETURN_NUMBER((long)curl_easy_perform((CURL *)(SYS_INT)handle));

    ConceptCURLHandle *chandle = 0;
    curl_easy_getinfo((CURL *)(SYS_INT)handle, CURLINFO_PRIVATE, &chandle);

    if ((chandle) && (chandle->post)) {
        curl_formfree(chandle->post);
        chandle->post = NULL;
    }

    return 0;
}
//=====================================================================================//
//--------------------//
int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    if (!clientp)
        return 0;

    ConceptCURLHandle *chandle = 0;
    curl_easy_getinfo((CURL *)clientp, (CURLINFO)CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->progress_delegate, &RES, &EXCEPTION, (INTEGER)5, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)clientp, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dltotal, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dlnow, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ultotal, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ulnow);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);


    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
size_t write_callback(char *buffer, size_t size, size_t nitems, void *outstream) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER            ndata    = 0;
    char              *szdata  = 0;
    INTEGER           TYPE     = 0;
    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo((CURL *)outstream, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->write_delegate, &RES, &EXCEPTION, (INTEGER)4, (INTEGER)VARIABLE_STRING, buffer, (double)(long)size * nitems, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)size, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)nitems, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)chandle->write_data);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
size_t read_callback(char *buffer, size_t size, size_t nitems, void *outstream) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo((CURL *)outstream, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    void *buffer_var = 0;
    InvokePtr(INVOKE_CREATE_VARIABLE, &buffer_var);

    InvokePtr(INVOKE_SET_VARIABLE, buffer_var, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->read_delegate, &RES, &EXCEPTION, (INTEGER)4, (INTEGER)VARIABLE_UNDEFINED, buffer_var, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)size, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)nitems, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)chandle->read_data);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    InvokePtr(INVOKE_GET_VARIABLE, buffer_var, &TYPE, &szdata, &ndata);


    if (TYPE != VARIABLE_STRING) {
        InvokePtr(INVOKE_FREE_VARIABLE, RES);
        InvokePtr(INVOKE_FREE_VARIABLE, buffer_var);

        return 0;
    }
    int min = size * nitems;
    if ((long)ndata < min)
        min = (long)ndata;

    memcpy(buffer, szdata, min);

    InvokePtr(INVOKE_FREE_VARIABLE, buffer_var);


    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
int sockopt_callback(void *clientp, curl_socket_t curlfd, curlsocktype purpose) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo((CURL *)clientp, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->sockopt_delegate, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)clientp, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)curlfd, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)purpose);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);


    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
int ioctl_callback(CURL *handle, int cmd, void *clientp) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo(handle, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->ioctl_delegate, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)handle, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)cmd, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)clientp);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo(handle, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->debug_delegate, &RES, &EXCEPTION, (INTEGER)5, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)handle, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)type, (INTEGER)VARIABLE_STRING, (char *)data, (NUMBER)size, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)size, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)userptr);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
CURLcode conv_callback(char *buffer, size_t length) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, conv_callback, &RES, &EXCEPTION, (INTEGER)2, (INTEGER)VARIABLE_STRING, (char *)buffer, (double)(long)length, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)length);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);


    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return (CURLcode)result;
}

//--------------------//
CURLcode ssl_ctx_callback(CURL *curl, void *ssl_ctx, void *userptr) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo(curl, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return CURLE_USE_SSL_FAILED;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->ssl_ctx_delegate, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)curl, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)ssl_ctx, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)userptr);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return (CURLcode)result;
}

//--------------------//
size_t header_callback(char *buffer, size_t size, size_t nitems, void *outstream) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    ConceptCURLHandle *chandle = 0;

    curl_easy_getinfo((CURL *)outstream, CURLINFO_PRIVATE, &chandle);
    if (!chandle)
        return 0;

    InvokePtr(INVOKE_CALL_DELEGATE, chandle->header_delegate, &RES, &EXCEPTION, (INTEGER)4, (INTEGER)VARIABLE_STRING, buffer, (double)(long)size * nitems, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)size, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)nitems, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)chandle->header_data);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);


    InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

    switch (TYPE) {
        case VARIABLE_NUMBER:
            result = (int)ndata;
            break;

        case VARIABLE_STRING:
            result = AnsiString(szdata).ToInt();
            break;

        case VARIABLE_ARRAY:
        case VARIABLE_DELEGATE:
        case VARIABLE_CLASS:
            result = (long)szdata;
            break;
    }

    InvokePtr(INVOKE_FREE_VARIABLE, RES);

    return result;
}

//--------------------//
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_setopt CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "curl_easy_setopt takes three parameter (CURLhandle, CURLoption, parameter)");
    LOCAL_INIT;

    NUMBER _handle;
    NUMBER _option;

    CURL       *handle;
    CURLoption option;

    GET_CHECK_NUMBER(0, _handle, "curl_easy_setopt: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, _option, "curl_easy_setopt: parameter 2 should be a number");

    handle = (void *)(SYS_INT)_handle;
    if (!handle)
        return (void *)"Handle(first parameter) is null curl_easy_setopt";

    option = (CURLoption)(long)_option;

    // delegate
    char   *d_class = 0;
    NUMBER d_member = 0;
    int    res      = 0;

    ConceptCURLHandle *chandle = 0;
    curl_easy_getinfo((CURL *)(SYS_INT)handle, CURLINFO_PRIVATE, &chandle);
    switch (option) {
        case CURLOPT_WRITEDATA:
            if (chandle) {
                GET_CHECK_NUMBER(2, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a number");
                chandle->write_data = (int)d_member;
            }
            break;

        case CURLOPT_WRITEFUNCTION:
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->write_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->write_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->write_delegate);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, handle);
                res = (int)curl_easy_setopt(handle, option, write_callback);
            }
            break;

        case CURLOPT_READDATA:
            if (chandle) {
                GET_CHECK_NUMBER(2, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a number");
                chandle->read_data = (int)d_member;
            }
            break;

        case CURLOPT_READFUNCTION:
            //read_delegate=PARAMETER(2);
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->read_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->read_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->read_delegate);
                curl_easy_setopt(handle, CURLOPT_READDATA, handle);
                res = (int)curl_easy_setopt(handle, option, read_callback);
            }
            break;

        case CURLOPT_IOCTLFUNCTION:
            //ioctl_delegate=PARAMETER(2);
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");

            if (chandle) {
                if (chandle->ioctl_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->ioctl_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->ioctl_delegate);
                curl_easy_setopt(handle, CURLOPT_IOCTLDATA, handle);
                res = (int)curl_easy_setopt(handle, option, ioctl_callback);
            }
            break;

        case CURLOPT_SOCKOPTFUNCTION:
            //sockopt_delegate=PARAMETER(2);
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->sockopt_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->sockopt_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->sockopt_delegate);

                curl_easy_setopt(handle, CURLOPT_SOCKOPTDATA, handle);
                res = (int)curl_easy_setopt(handle, option, sockopt_callback);
            }
            break;

        case CURLOPT_PROGRESSFUNCTION:
            //progress_delegate=PARAMETER(2);
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->progress_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->progress_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->progress_delegate);

                curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, handle);
                res = (int)curl_easy_setopt(handle, option, progress_callback);
            }
            break;

        case CURLOPT_HEADERDATA:
            if (chandle) {
                GET_CHECK_NUMBER(2, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a number");
                chandle->header_data = (int)d_member;
            }
            break;

        case CURLOPT_HEADERFUNCTION:
            //header_delegate=PARAMETER(2);
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->header_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->header_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->header_delegate);
                curl_easy_setopt(handle, CURLOPT_HEADERDATA, handle);
                res = (int)curl_easy_setopt(handle, option, header_callback);
            }
            break;

        case CURLOPT_HTTPHEADER:
            {
                GET_CHECK_ARRAY(2, d_class, "curl_easy_setopt: for the given CURLoption, parameter 3 should be an array");
                struct curl_slist *header = NULL;

                char **elems = GetCharList(PARAMETER(2), Invoke);
                if (elems) {
                    int i = 0;
                    while (elems[i]) {
                        header = curl_slist_append(header, elems[i++]);
                    }
                }

                res = (int)curl_easy_setopt(handle, option, header);
            }
            break;

        case CURLOPT_HTTPPOST:
            {
                GET_ARRAY(2, d_class);
                //GET_CHECK_ARRAY(2,d_class,"curl_easy_setopt: for the given CURLoption, parameter 3 should be an array");
                if (TYPE == VARIABLE_ARRAY) {
                    struct curl_httppost *post = GetForm(chandle, PARAMETER(2), Invoke);
                    res = (int)curl_easy_setopt(handle, option, post);
                } else {
                    NUMBER post_value = 0;
                    GET_NUMBER(2, post_value);
                    if ((TYPE == VARIABLE_NUMBER) && (post_value != 0)) {
                        res = (int)curl_easy_setopt(handle, option, (struct curl_httppost *)(long)post_value);
                    } else
                        return (void *)"curl_easy_setopt: for the given CURLoption, parameter 3 should be a form created with curl_formnew or an array";
                }
            }
            break;

        /*case CURLOPT_WRITEHEADER:
             writeheader_delegate=PARAMETER(2);
             GET_CHECK_DELEGATE(2,d_class,d_member,"curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");

             res=(int)curl_easy_setopt(handle, option, writeheader_callback);
             break;*/
        case CURLOPT_DEBUGFUNCTION:
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->debug_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->debug_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->debug_delegate);

                res = (int)curl_easy_setopt(handle, option, debug_callback);
            }
            break;

        case CURLOPT_SSL_CTX_FUNCTION:
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->ssl_ctx_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->ssl_ctx_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->ssl_ctx_delegate);

                res = (int)curl_easy_setopt(handle, option, ssl_ctx_callback);
            }
            break;

        case CURLOPT_CONV_TO_NETWORK_FUNCTION:
        case CURLOPT_CONV_FROM_NETWORK_FUNCTION:
        case CURLOPT_CONV_FROM_UTF8_FUNCTION:
            GET_CHECK_DELEGATE(2, d_class, d_member, "curl_easy_setopt: for the given CURLoption, parameter 3 should be a delegate");
            if (chandle) {
                if (chandle->conv_delegate)
                    Invoke(INVOKE_FREE_VARIABLE, chandle->conv_delegate);
                DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), chandle->conv_delegate);

                res = (int)curl_easy_setopt(handle, option, conv_callback);
            }
            break;

        default:
            GET_DELEGATE(2, d_class, d_member);
            switch (TYPE) {
                case VARIABLE_NUMBER:
                    RETURN_NUMBER((long)curl_easy_setopt(handle, option, (long)d_member));
                    break;

                case VARIABLE_STRING:
                    RETURN_NUMBER((long)curl_easy_setopt(handle, option, (char *)d_class));
                    break;

                case VARIABLE_DELEGATE:
                    // TO DO !!!
                    break;

                case VARIABLE_CLASS:
                case VARIABLE_ARRAY:
                default:
                    return (void *)"curl_easy_setopt: parameter 3 can't be a class or array";
            }
            return 0;
    }
    RETURN_NUMBER((long)res);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_version CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "curl_version takes no parameters");

    RETURN_STRING(curl_version());

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_getenv CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_getenv takes one string parameter");

    LOCAL_INIT;

    char *data;

    GET_CHECK_STRING(0, data, "curl_getenv: parameter 1 should be a string");

    RETURN_STRING(curl_getenv(data));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_escape CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_escape takes one string parameter");

    LOCAL_INIT;

    char   *data;
    NUMBER len = 0;

    GET_CHECK_BUFFER(0, data, len, "curl_escape: parameter 1 should be a string");

    RETURN_STRING(curl_escape(data, (size_t)len));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_unescape CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_unescape takes one string parameter");

    LOCAL_INIT;

    char   *data;
    NUMBER len = 0;

    GET_CHECK_BUFFER(0, data, len, "curl_unescape: parameter 1 should be a string");

    char *ret = curl_unescape(data, (size_t)len);
    RETURN_STRING(ret);
    curl_free(ret);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_escape CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "curl_easy_escape takes 2 parameters");

    LOCAL_INIT;

    char   *data;
    NUMBER len = 0;
    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "curl_easy_escape: parameter 1 should be a number");
    GET_CHECK_BUFFER(1, data, len, "curl_easy_escape: parameter 2 should be a string");


    char *ret = curl_easy_escape((CURL *)(SYS_INT)handle, (const char *)data, (size_t)len);
    RETURN_STRING(ret);
    curl_free(ret);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_unescape CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "curl_easy_unescape takes 2 parameters");

    LOCAL_INIT;

    char   *data;
    NUMBER len = 0;
    NUMBER handle;

    GET_CHECK_NUMBER(0, handle, "curl_easy_unescape: parameter 1 should be a number");
    GET_CHECK_BUFFER(1, data, len, "curl_easy_unescape: parameter 2 should be a string");

    int len2 = 0;

    char *ret = curl_easy_unescape((CURL *)(SYS_INT)handle, (const char *)data, (size_t)len, &len2);
    RETURN_BUFFER(ret, len2);
    curl_free(ret);

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_strequal CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "curl_strequal takes 2 parameters");
    LOCAL_INIT;

    char *data1;
    char *data2;

    GET_CHECK_STRING(0, data1, "curl_strequal: parameter 1 should be a string");
    GET_CHECK_STRING(1, data2, "curl_strequal: parameter 2 should be a string");

    RETURN_NUMBER(curl_strequal(data1, data2));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_strnequal CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "curl_strequal takes 3 parameters");
    LOCAL_INIT;

    char   *data1;
    char   *data2;
    NUMBER size;

    GET_CHECK_STRING(0, data1, "curl_strnequal: parameter 1 should be a string");
    GET_CHECK_STRING(1, data2, "curl_strnequal: parameter 2 should be a string");
    GET_CHECK_NUMBER(2, size, "curl_strnequal: parameter 3 should be a number");

    RETURN_NUMBER(curl_strnequal(data1, data2, (int)size));

    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_easy_getinfo CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "curl_easy_getinfo takes 3 parameters");
    LOCAL_INIT;

    NUMBER handle;
    NUMBER info;

    GET_CHECK_NUMBER(0, handle, "curl_easy_getinfo: parameter 1 should be a number");
    GET_CHECK_NUMBER(1, info, "curl_easy_getinfo: parameter 2 should be a number");

    CURLcode res;
    CURL     *_handle = (CURL *)(SYS_INT)handle;
    CURLINFO _info    = (CURLINFO)(int)info;

    /*char    buffer[8192];
       buffer[0]=0;*/
    char   *buffer = 0;
    long   data    = 0;
    double fdata   = 0;

    struct curl_slist *list   = 0;
    struct curl_slist *list_f = 0;
    INTEGER           i       = 0;

    switch (_info) {
        case CURLINFO_EFFECTIVE_URL:
        case CURLINFO_CONTENT_TYPE:
        case CURLINFO_PRIVATE:
        case CURLINFO_FTP_ENTRY_PATH:
            res = curl_easy_getinfo(_handle, _info, &buffer);
            SET_STRING(2, buffer);
            break;

        case CURLINFO_FILETIME:
        case CURLINFO_HTTP_CONNECTCODE:
        case CURLINFO_REDIRECT_COUNT:
        case CURLINFO_HEADER_SIZE:
        case CURLINFO_REQUEST_SIZE:
        case CURLINFO_SSL_VERIFYRESULT:
        case CURLINFO_HTTPAUTH_AVAIL:
        case CURLINFO_PROXYAUTH_AVAIL:
        case CURLINFO_OS_ERRNO:
        case CURLINFO_NUM_CONNECTS:
        case CURLINFO_LASTSOCKET:
            res = curl_easy_getinfo(_handle, _info, &data);
            SET_NUMBER(2, data);
            break;

        case CURLINFO_TOTAL_TIME:
        case CURLINFO_NAMELOOKUP_TIME:
        case CURLINFO_CONNECT_TIME:
        case CURLINFO_PRETRANSFER_TIME:
        case CURLINFO_STARTTRANSFER_TIME:
        case CURLINFO_REDIRECT_TIME:
        case CURLINFO_SIZE_UPLOAD:
        case CURLINFO_SIZE_DOWNLOAD:
        case CURLINFO_SPEED_DOWNLOAD:
        case CURLINFO_SPEED_UPLOAD:
        case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
        case CURLINFO_CONTENT_LENGTH_UPLOAD:
            res = curl_easy_getinfo(_handle, _info, &fdata);
            SET_NUMBER(2, fdata);
            break;

        case CURLINFO_SSL_ENGINES:
        case CURLINFO_COOKIELIST:
            res = curl_easy_getinfo(_handle, _info, &list);
            Invoke(INVOKE_CREATE_ARRAY, PARAMETER(2));

            list_f = list;
            while (list) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(2), i, (INTEGER)VARIABLE_STRING, list->data, (double)0);
                i++;
                list = list->next;
            }
            curl_slist_free_all(list_f);
            break;

        default:
            return (void *)"curl_easy_getinfo: CURLINFO value not yet supported for parameter 2 !";
    }

    RETURN_NUMBER((long)res);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API CONCEPT__curl_getdate CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "curl_getdate takes one string parameter");

    LOCAL_INIT;

    char   *data;
    NUMBER len = 0;

    GET_CHECK_BUFFER(0, data, len, "curl_getdate: parameter 1 should be a string");

    RETURN_NUMBER(curl_getdate(data, 0));

    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(_curl_formnew, 1)
    T_ARRAY(_curl_formnew, 0)

    struct curl_httppost *mypost = NULL;
    struct curl_httppost *last   = NULL;
    AnsiString           temp;
    AnsiString           temp2;

    void *pData = PARAMETER(0);
    int  count  = Invoke(INVOKE_GET_ARRAY_COUNT, pData);
    for (int i = 0; i < count; i++) {
        const char  *key      = 0;
        void    *newpData = 0;
        char    *szData;
        INTEGER type;
        NUMBER  nData;

        Invoke(INVOKE_GET_ARRAY_KEY, pData, i, &key);
        Invoke(INVOKE_ARRAY_VARIABLE, pData, i, &newpData);
        if (!key) {
            temp2 = (long)i;
            key   = temp2.c_str();
        }

        Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

        if (key) {
            if (type == VARIABLE_STRING) {
                curl_formadd(&mypost, &last,
                             CURLFORM_COPYNAME, key,
                             CURLFORM_COPYCONTENTS, szData,
                             CURLFORM_CONTENTSLENGTH, (long)nData,
                             CURLFORM_END);
            } else
            if (type == VARIABLE_NUMBER) {
                temp = (NUMBER)nData;

                curl_formadd(&mypost, &last,
                             CURLFORM_COPYNAME, key,
                             CURLFORM_COPYCONTENTS, temp.c_str(),
                             CURLFORM_END);
            }
        }
    }
    RETURN_NUMBER((SYS_INT)mypost);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(_curl_formfree, 1)
    T_NUMBER(_curl_formfree, 0)

    if (PARAM_INT(0)) {
        curl_formfree((struct curl_httppost *)PARAM_INT(0));
    }

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(_curl_easy_send, 3)
    T_NUMBER(_curl_easy_send, 0)
    T_STRING(_curl_easy_send, 1)
    SET_NUMBER(2, 0);

    CURL *handle;

    handle = (void *)PARAM_INT(0);
    if (!handle)
        return (void *)"Handle(first parameter) is null curl_easy_send";


    size_t   n    = 0;
    CURLcode code = curl_easy_send(handle, PARAM(1), PARAM_LEN(1), &n);
    SET_NUMBER(2, n);
    RETURN_NUMBER(code);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(_curl_easy_recv, 4)
    T_NUMBER(_curl_easy_recv, 0)
    T_NUMBER(_curl_easy_recv, 2)
    SET_NUMBER(3, -1);
    SET_STRING(1, "");

    CURL *handle;

    handle = (void *)PARAM_INT(0);
    if (!handle)
        return (void *)"Handle(first parameter) is null curl_easy_send";

    void *ptr = malloc(PARAM_INT(2));
    if (ptr) {
        size_t   n    = 0;
        CURLcode code = curl_easy_recv(handle, ptr, PARAM_INT(2), &n);
        if (n > 0) {
            SET_BUFFER(1, ptr, n);
        }
        SET_NUMBER(3, n);
        RETURN_NUMBER(code);
        free(ptr);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//=====================================================================================//


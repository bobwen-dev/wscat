#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <curl/curl.h>
#include <ctype.h>

#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_PROTOCOL "wscat-protocol"

struct session_data {
    int fd;
};

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    struct session_data *data = (struct session_data *)userdata;
    size_t real_size = size * nmemb;
    write(data->fd, ptr, real_size);
    return real_size;
}

static size_t header_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;
    fprintf(stderr, "HTTP Header: %.*s", (int)real_size, (char *)ptr);
    return real_size;
}

void set_proxy(CURL *curl, const char *proxy_url) {
    if (proxy_url) {
        if (strncasecmp(proxy_url, "http://", 7) == 0) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url + 7);
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        } else if (strncasecmp(proxy_url, "https://", 8) == 0) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url + 8);
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        } else if (strncasecmp(proxy_url, "socks5://", 9) == 0) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url + 9);
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        } else if (strncasecmp(proxy_url, "socks5h://", 10) == 0) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url + 10);
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
        }
    }
}

int main(int argc, char **argv) {
    int opt;
    char *url = NULL;
    char *protocol_name = DEFAULT_PROTOCOL;
    int buffer_size = DEFAULT_BUFFER_SIZE;
    int timeout = 60;
    int dns_timeout = 30;
    int deflate = 0;
    int insecure = 0;
    const char *http_proxy = getenv("http_proxy");
    const char *https_proxy = getenv("https_proxy");

    static struct option long_options[] = {
        {"protocol", required_argument, 0, 'p'},
        {"buffer-size", required_argument, 0, 'b'},
        {"timeout", required_argument, 0, 't'},
        {"dns-timeout", required_argument, 0, 'd'},
        {"deflate", no_argument, 0, 'z'},
        {"insecure", no_argument, 0, 'k'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "p:b:t:d:zk", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                protocol_name = optarg;
                break;
            case 'b':
                buffer_size = atoi(optarg);
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            case 'd':
                dns_timeout = atoi(optarg);
                break;
            case 'z':
                deflate = 1;
                break;
            case 'k':
                insecure = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [options] <WebSocket URL>\n", argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        url = argv[optind];
    } else {
        fprintf(stderr, "Usage: %s [options] <WebSocket URL>\n", argv[0]);
        return 1;
    }

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    struct session_data data;
    char buffer[buffer_size];
    int n;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, dns_timeout);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    if (deflate) {
        headers = curl_slist_append(headers, "Accept-Encoding: deflate");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    if (insecure) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    // Determine the protocol and set the appropriate proxy
    if (strncasecmp(url, "ws://", 5) == 0) {
        set_proxy(curl, http_proxy);
    } else if (strncasecmp(url, "wss://", 6) == 0) {
        set_proxy(curl, https_proxy);
    }

    // Set the WebSocket protocol
    headers = curl_slist_append(headers, protocol_name);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    data.fd = fileno(stdin);

    while (1) {
        n = read(data.fd, buffer, sizeof(buffer));
        if (n <= 0) {
            break;
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            break;
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (headers) {
        curl_slist_free_all(headers);
    }
    return 0;
}

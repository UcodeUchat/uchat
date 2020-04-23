#include "uchat.h"

void mx_report_tls(struct tls * tls_ctx, char * host) {
    time_t t;
    const char *ocsp_url;

    fprintf(stderr, "\nTLS handshake negotiated %s/%s with host %s\n",
            tls_conn_version(tls_ctx), tls_conn_cipher(tls_ctx), host);
    fprintf(stderr, "Peer name: %s\n", host);
    if (tls_peer_cert_subject(tls_ctx))
        fprintf(stderr, "Subject: %s\n",
                tls_peer_cert_subject(tls_ctx));
    if (tls_peer_cert_issuer(tls_ctx))
        fprintf(stderr, "Issuer: %s\n",
                tls_peer_cert_issuer(tls_ctx));
    if ((t = tls_peer_cert_notbefore(tls_ctx)) != -1)
        fprintf(stderr, "Valid From: %s", ctime(&t));
    if ((t = tls_peer_cert_notafter(tls_ctx)) != -1)
        fprintf(stderr, "Valid Until: %s", ctime(&t));
    if (tls_peer_cert_hash(tls_ctx))
        fprintf(stderr, "Cert Hash: %s\n",
                tls_peer_cert_hash(tls_ctx));
    ocsp_url = tls_peer_ocsp_url(tls_ctx);
    if (ocsp_url != NULL)
        fprintf(stderr, "OCSP URL: %s\n", ocsp_url);
    switch (tls_peer_ocsp_response_status(tls_ctx)) {
        case TLS_OCSP_RESPONSE_SUCCESSFUL:
            fprintf(stderr, "OCSP Stapling: %s\n",
                    tls_peer_ocsp_result(tls_ctx) == NULL ?  "" :
                    tls_peer_ocsp_result(tls_ctx));
            fprintf(stderr,
                    "  response_status=%d cert_status=%d crl_reason=%d\n",
                    tls_peer_ocsp_response_status(tls_ctx),
                    tls_peer_ocsp_cert_status(tls_ctx),
                    tls_peer_ocsp_crl_reason(tls_ctx));
            t = tls_peer_ocsp_this_update(tls_ctx);
            fprintf(stderr, "  this update: %s",
                    t != -1 ? ctime(&t) : "\n");
            t =  tls_peer_ocsp_next_update(tls_ctx);
            fprintf(stderr, "  next update: %s",
                    t != -1 ? ctime(&t) : "\n");
            t =  tls_peer_ocsp_revocation_time(tls_ctx);
            fprintf(stderr, "  revocation: %s",
                    t != -1 ? ctime(&t) : "\n");
            break;
        case -1:
            break;
        default:
            fprintf(stderr, "OCSP Stapling:  failure - response_status %d (%s)\n",
                    tls_peer_ocsp_response_status(tls_ctx),
                    tls_peer_ocsp_result(tls_ctx) == NULL ?  "" :
                    tls_peer_ocsp_result(tls_ctx));
            break;

    }
}

void mx_print_curr_time(void) {
    time_t now;
    char *time_str;

    now = time(0);
    time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    printf("%s\t", time_str);
}


void mx_get_input(char *buffer) {
    int read;

    buffer[0] = 0;
    fgets(buffer, MAX_CLIENT_INPUT, stdin);
    read = strlen(buffer);
    if (read > 0)
        buffer[read - 1] = 0;
}


int mx_get_input2(char *buffer) {
    int read;

    buffer[0] = 0;
    fgets(buffer, MAX_CLIENT_INPUT, stdin);
    read = strlen(buffer);
    if (read > 0)
        buffer[read - 1] = 0;
    return read;
}

void mx_print_tid(const char *s) {
    pid_t       pid;
    pthread_t   tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %u tid %u (0x%x)\n", s,
            (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);
}

static void print_family(struct addrinfo *aip) {
	printf(" family ");
	switch (aip->ai_family) {
	case AF_INET:
		printf("inet");
		break;
	case AF_INET6:
		printf("inet6");
		break;
	case AF_UNIX:
		printf("unix");
		break;
	case AF_UNSPEC:
		printf("unspecified");
		break;
	default:
		printf("unknown");
	}
}

static void print_type(struct addrinfo *aip) {
	printf(" type ");
	switch (aip->ai_socktype) {
	case SOCK_STREAM:
		printf("stream");
		break;
	case SOCK_DGRAM:
		printf("datagram");
		break;
	case SOCK_SEQPACKET:
		printf("seqpacket");
		break;
	case SOCK_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_socktype);
	}
}

static void print_protocol(struct addrinfo *aip) {
	printf(" protocol ");
	switch (aip->ai_protocol) {
	case 0:
		printf("default");
		break;
	case IPPROTO_TCP:
		printf("TCP");
		break;
	case IPPROTO_UDP:
		printf("UDP");
		break;
	case IPPROTO_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_protocol);
	}
}

static void print_flags(struct addrinfo *aip) { 
	printf("flags");
	if (aip->ai_flags == 0) {
		printf(" 0");
	} else {
		if (aip->ai_flags & AI_PASSIVE)
			printf(" passive");
		if (aip->ai_flags & AI_CANONNAME)
			printf(" canon");
		if (aip->ai_flags & AI_NUMERICHOST)
			printf(" numhost");
		if (aip->ai_flags & AI_NUMERICSERV)
			printf(" numserv");
		if (aip->ai_flags & AI_V4MAPPED)
			printf(" v4mapped");
		if (aip->ai_flags & AI_ALL)
			printf(" all");
	}
}

int addr_socet_info(int argc, char *argv[]) {
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	struct sockaddr_in	*sinp;
	const char 			*addr;
	int 				err;
	char 				abuf[INET_ADDRSTRLEN];

	if (argc != 3)
		mx_printerr("usage: %s nodename service");
	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = 0;
	hint.ai_socktype = 0;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo("localhost", argv[1], &hint, &ailist)) != 0)
		printf("getaddrinfo error: %s", gai_strerror(err));
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		print_flags(aip);
		print_family(aip);
		print_type(aip);
		print_protocol(aip);
		printf("\n\thost %s", aip->ai_canonname?aip->ai_canonname:"-");
		if (aip->ai_family == AF_INET) {
			sinp = (struct sockaddr_in *)aip->ai_addr;
			addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf,
			    INET_ADDRSTRLEN);
			printf(" address %s", addr?addr:"unknown");
			printf(" port %d", ntohs(sinp->sin_port));
		}
		printf("\n");
	}
	return 0;
	// exit(0);
}



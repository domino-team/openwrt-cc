#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

struct nl_request {
	struct nlmsghdr  nh;
	struct ifinfomsg ifi;
	char attrbuf[4096];
};

static int use_syslog;

#define TIME_FORMAT "%F %T"
#define USE_SYSLOG(ident)                          \
    do {                                           \
        use_syslog = 1;                            \
        openlog((ident), LOG_CONS | LOG_PID, 0); } \
    while (0)

#define LOGI(format, ...)                                                        \
    do {                                                                         \
        if (use_syslog) {                                                        \
            syslog(LOG_INFO, format, ## __VA_ARGS__);                            \
        } else {                                                                 \
            time_t now = time(NULL);                                             \
            char timestr[20];                                                    \
            strftime(timestr, 20, TIME_FORMAT, localtime(&now));                 \
                fprintf(stdout, "%s INFO: " format "\n", timestr,               \
                        ## __VA_ARGS__);                                         \
        }                                                                        \
    }                                                                            \
    while (0)

#define LOGE(format, ...)                                                         \
    do {                                                                          \
        if (use_syslog) {                                                         \
            syslog(LOG_ERR, format, ## __VA_ARGS__);                              \
        } else {                                                                  \
            time_t now = time(NULL);                                              \
            char timestr[20];                                                     \
            strftime(timestr, 20, TIME_FORMAT, localtime(&now));                  \
                fprintf(stderr, "%s ERROR: " format "\n", timestr,               \
                        ## __VA_ARGS__);                                          \
        } }                                                                       \
    while (0)

void daemonize(const char *path)
{
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* If we got a good PID, then
     * we can exit the parent process. */
    if (pid > 0) {
        FILE *file = fopen(path, "w");
        if (file == NULL) {
			exit(EXIT_FAILURE);
        }

        fprintf(file, "%d", (int)pid);
        fclose(file);
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

/* Utility function for parse rtattr. */
static void 
netlink_parse_rtattr (struct rtattr **tb, int max, struct rtattr *rta, int len)
{
    while (RTA_OK (rta, len))
    {
	if (rta->rta_type <= max)
	    tb[rta->rta_type] = rta;
	rta = RTA_NEXT (rta, len);
    }
}

/* Recieving netlink message. */
void netlink_recv(int nsock) 
{
    char buf[4096];
    struct iovec iov = { buf, sizeof(buf) };
    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof(snl), &iov, 1, NULL, 0, 0};
    struct nlmsghdr *h;
    struct rtattr *tb[IFLA_MAX > IFA_MAX ? IFLA_MAX : IFA_MAX + 1];
    struct ifinfomsg *ifi;
    struct nlmsgerr *err;

    int status = recvmsg(nsock, &msg, 0);

	if(status < 0) {
		LOGI("recvmsg(nsock) fail");
		return;
	} else if (status == 0) {
		LOGI("recvmsg(nsock): received zero sizenetlink message");
		return;
	}

	if(msg.msg_namelen != sizeof(snl)){
		LOGI("recvmsg(nsock): received invalid netlink message");
		return;
	}

	if(msg.msg_flags & MSG_TRUNC){
		LOGI("recvmsg(nsock): received truncated netlink message");
		return;
	}

	for(h = (struct nlmsghdr *) buf; NLMSG_OK(h, status); h = NLMSG_NEXT (h, status)) {

		switch(h->nlmsg_type) {
			case NLMSG_DONE:
				return;
			case NLMSG_ERROR:
				err = (struct nlmsgerr *) NLMSG_DATA(h);
				if (h->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
					LOGI("received error message with invalid length");
					return;
				}
				return;
			case RTM_NEWLINK:
			case RTM_DELLINK:
				ifi = NLMSG_DATA(h);;

				int len = h->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifinfomsg));
				char ifname[IF_NAMESIZE+1];
				if(len < 0)
					continue;

				memset(tb, 0, sizeof(tb));
				netlink_parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

				if(tb[IFLA_IFNAME] == NULL)
					continue;

				strncpy(ifname, (char *)RTA_DATA(tb[IFLA_IFNAME]), IF_NAMESIZE);

				switch (h->nlmsg_type) {
					case RTM_NEWLINK:
						if (ifi->ifi_flags & IFF_RUNNING) {
							/* ifname up */
							/* TODO */
						} else {
							/* ifname down */
							if (!strncmp(ifname, "eth0.2", strlen("eth0.2"))) {
								system("killall -16 udhcpc");
							}
						}
#if 0
						if(ifi->ifi_flags & IFF_RUNNING){
							printf("interface %s(%d): UP\n", 
									ifname, ifi->ifi_index);
						} else {
							printf("interface %s(%d): DOWN\n", 
									ifname, ifi->ifi_index);
						}
						break;
					case RTM_DELLINK:
						printf("interface %s(%d): REMOVED\n", 
								ifname, ifi->ifi_index);
						break;
#endif
				}
				break;
			default:
				continue;
		}
	}
}

#if 0
/* Make the kernel send us an RTM_NEWLINK for all interfaces */
void netlink_getlink(int nsock) 
{
    struct nlmsghdr *n;
    struct ifinfomsg *ifi;
    u_int8_t req[sizeof(struct nlmsghdr) + sizeof(struct ifinfomsg) +
		 sizeof(struct ifaddrmsg) + 4096];

    memset(&req, 0, sizeof(req));
    n = (struct nlmsghdr*) req;
    n->nlmsg_len = NLMSG_LENGTH(sizeof(*ifi));
    n->nlmsg_type = RTM_GETLINK;
    n->nlmsg_seq = 1;
    n->nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    n->nlmsg_pid = 0;

    ifi = NLMSG_DATA(n);
    ifi->ifi_family = AF_UNSPEC;
    ifi->ifi_change = -1;

    if (send(nsock, n, n->nlmsg_len, 0) < 0) {
	LOGI("send");
	exit(1);
    }    
}
#endif

int main(int argc, char **argv) {
	/* netlink */
	struct nl_request req;
    struct sockaddr_nl nl_addr;
	struct rtattr *rta = NULL;
	unsigned int mtu = 1000;

	/* sock */
    int nsock = 0;

	/* select */
    fd_set r;
	
	/* other */
	int ret = 0;
	int c = 0;
	int pid_flags = 0;
    char *pid_path  = NULL;

    opterr = 0;

	while ((c = getopt(argc, argv, "f:")) != -1) {
		switch (c) {
		case 'f':
			pid_flags = 1;
			pid_path = optarg;
			break;
		case '?':
			LOGE("Unrecognized option: %s", optarg);
            opterr = 1;
			break;
		}
	}

    if (opterr) {
        exit(EXIT_FAILURE);
    }

	if (pid_flags) {
		/* run in background */
		USE_SYSLOG(argv[0]);
		daemonize(pid_path);
	}

    /* initialize netlink socket */
	nsock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if(nsock < 0) {
		LOGE("socket");
		return -1;
    }

    memset(&nl_addr, 0, sizeof(nl_addr));

    nl_addr.nl_family = AF_NETLINK;
    nl_addr.nl_groups = RTMGRP_LINK;
    nl_addr.nl_pid = getpid();

	ret = bind(nsock, (struct sockaddr *) &nl_addr, sizeof(nl_addr));
    if (ret < 0) {
		LOGE("bind");
		goto out;
    }

	memset(&req, 0, sizeof(req));

	/* man 3 rtnetlink for detail */
	req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.nh.nlmsg_flags = NLM_F_REQUEST;
	req.nh.nlmsg_type = RTM_NEWLINK;
	req.ifi.ifi_family = AF_UNSPEC;
	req.ifi.ifi_change = 0xffffffff;
	rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
	rta->rta_type = IFLA_MTU;
	rta->rta_len = RTA_LENGTH(sizeof(unsigned int));
	req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(mtu));
	memcpy(RTA_DATA(rta), &mtu, sizeof(mtu));

	ret = send(nsock, &req, req.nh.nlmsg_len, 0);
	if (ret < 0) {
		LOGE("send");
		goto out;
	}

	/*
    netlink_getlink(nsock);
    netlink_recv(nsock);
	*/
	while(1) {
		FD_ZERO(&r);
		FD_SET(nsock, &r);

		ret = select(nsock + 1, &r, NULL, NULL, NULL);
		switch (ret) {
			case -1:
				if (errno != EINTR) {
					LOGE("select");
					goto out;
				}
				continue;
			case 0:	 /* timeout */
				break;
			default:
				break;
		}

		if (FD_ISSET(nsock, &r))
			netlink_recv(nsock);
	}

    return 0;

out:
	close(nsock);
	return ret;
}

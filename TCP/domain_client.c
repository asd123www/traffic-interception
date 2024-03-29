#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/un.h>

#define MSG_SIZE 8192
#define WARMUP 100000
#define TOTAL_ROUND 2000000


char buff[1024 * 64];

long arr[TOTAL_ROUND + 5];
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
void queryStatistic(long a[]) {
    int n = a[0];
    qsort(a + 1, n, sizeof(a[0]), cmpfunc);
    double sum = 0.0;
    for(int i = 1; i <= n; ++i) sum += a[i];
    double var = 0.0;
    for(int i = 1; i <= n; ++i) var += ((double)a[i] * 0.001)*(double)(a[i] * 0.001);
    printf("Variance: %.3f\n", (var - (sum * 0.001) / n * (sum * 0.001)) / n);

    printf("Average latency: %.3f\n", sum / n * 0.001);
    printf("90th tail latency: %.3f\n", a[(long)(n * 0.9)] * 0.001);
    printf("99th tail latency: %.3f\n", a[(long)(n * 0.99)] * 0.001);
    printf("99.9th tail latency: %.3f\n", a[(long)(n * 0.999)] * 0.001);
}

int main() {
    int sockfd;
    struct sockaddr_un server_addr, client_addr;

    // Create a socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }

    // Specify the server address
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "asd123www_test_unix_socket");


    // Connect the socket to the server
    int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("connect failed!\n");
        exit(0);
    }

    puts("Successfully built a connection with server!");

    struct timespec start, end;

    for (int i = 0; i < TOTAL_ROUND; ++i) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        uint32_t write_len = write(sockfd, buff, MSG_SIZE);
        if (write_len != MSG_SIZE) {
            printf("Wrong: %d %d\n", write_len, MSG_SIZE);
            fflush(stdout);
        }
        uint32_t read_len = read(sockfd, buff, MSG_SIZE);
        if (read_len != MSG_SIZE) {
            printf("Wrong: %d %d\n", read_len, MSG_SIZE);
            fflush(stdout);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        if (i > WARMUP) {
            arr[++arr[0]] = end.tv_sec * 1000000000LL + end.tv_nsec - start.tv_sec * 1000000000LL - start.tv_nsec;
        }
    }

    // close the socket
    close(sockfd);

    // statistic
    queryStatistic(arr);
    
    return 0;
}

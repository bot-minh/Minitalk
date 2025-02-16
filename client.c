#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void send_char(pid_t server_pid, char c)
{
    for (int i = 7; i >= 0; i--) // Gửi từ bit cao xuống thấp
    {
        if ((c >> i) & 1)
            kill(server_pid, SIGUSR2); // Gửi bit 1
        else
            kill(server_pid, SIGUSR1); // Gửi bit 0
        usleep(100);                   // Delay để server có thể nhận tín hiệu
    }
}

void send_message(pid_t server_pid, char *message)
{
    while (*message)
        send_char(server_pid, *message++);
    send_char(server_pid, '\0'); // Gửi ký tự kết thúc chuỗi
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        write(2, "Usage: ./client <server_pid> <message>\n", 39);
        return 1;
    }

    pid_t server_pid = atoi(argv[1]);
    send_message(server_pid, argv[2]);

    return 0;
}

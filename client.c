#include <signal.h>
#include <unistd.h>
#include "ft_printf.h"
#include "libft/libft.h"

void send_char(int server_pid, char c)
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

void send_message(int server_pid, char *message)
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

    int server_pid = atoi(argv[1]);
    if (server_pid == 0)
    {
        ft_printf("Invalid PID: %s\n", argv[1]);
        return (1);
    }
    ft_printf("Sending message to PID: %d\n", server_pid);
    send_message(server_pid, argv[2]);
    return 0;
}

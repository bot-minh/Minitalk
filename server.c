#include <signal.h>
#include <unistd.h>
#include "ft_printf.h"
#include "libft/libft.h"

int buf_index = 0;

char *reallocate_buffer(char *buffer, int new_size, int buf_index)
{
    char *new_buffer;

    new_buffer = malloc(new_size);
    if (!new_buffer)
    {
        free(buffer);
        return NULL;
    }

    if (buffer)
    {
        ft_memcpy(new_buffer, buffer, buf_index);
        free(buffer);
    }
    return new_buffer;
}

// void handle_end_of_message(char **buffer)
// {
//     (*buffer)[buf_index] = '\0';
//     write(1, *buffer, buf_index);
//     write(1, "\n", 1);
//     free(*buffer);
//     *buffer = NULL;
//     buf_index = 0;
// }

void process_character(char **buffer, char c)
{
    char *new;
    new = reallocate_buffer(*buffer, buf_index + 1, buf_index);
    if (!new)
    {
        free(*buffer);
        exit(1);
    }
    *buffer = new;

    if (c == '\0')
    {
        (*buffer)[buf_index] = '\0';
        write(1, *buffer, buf_index);
        write(1, "\n", 1);
        free(*buffer);
        *buffer = NULL;
        buf_index = 0;
    }
    else
        (*buffer)[buf_index++] = c;
}

void store_bit(int sig)
{
    static int bit = 0;
    static int char_accum = 0;
    static char *buffer = NULL;

    char_accum |= (sig == SIGUSR2);
    bit++;

    if (bit == 8)
    {
        process_character(&buffer, (char)char_accum);
        bit = 0;
        char_accum = 0;
    }
    else
    {
        char_accum <<= 1;
    }
}

void handle_signal(int sig, siginfo_t *info, void *context)
{
    (void)info;
    (void)context;
    store_bit(sig);
}

// int main()
// {
//     struct sigaction sa;
//     sigemptyset(&sa.sa_mask);
//     sigaddset(&sa.sa_mask, SIGUSR1);
//     sigaddset(&sa.sa_mask, SIGUSR2);
//     sa.sa_flags = SA_RESTART | SA_SIGINFO;
//     sa.sa_sigaction = handle_signal;
//     sigaction(SIGUSR1, &sa, NULL);
//     sigaction(SIGUSR2, &sa, NULL);

//     ft_printf("Server running. PID: %d\n", getpid());
//     while (1)
//         pause();
//     return 0;
// }
// void sigint_handler(int sig, char **buffer)
// {
//     (void)sig;
//     if (buffer)
//         free(buffer);
//     exit(0);
// }

int main()
{
    struct sigaction sa;
    // struct sigaction sa_int;

    // Configure SIGUSR1 and SIGUSR2 handling
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    // Configure SIGINT(Ctrl + C) handling
    // sa_int.sa_handler = sigint_handler;
    // sigemptyset(&sa_int.sa_mask);
    // sa_int.sa_flags = 0;
    // sigaction(SIGINT, &sa_int, NULL);

    ft_printf("Server running. PID: %d\n", getpid());
    while (1)
        pause();
    return 0;
}
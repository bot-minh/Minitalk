#include <signal.h>
#include <unistd.h>
#include "ft_printf.h"
#include "libft/libft.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Global variables for message handling.
char *buffer = NULL;
int length = 0;    // Allocated size (number of characters stored)
int buf_index = 0; // Global index to track where the next character goes
int bit = 0;
int char_accum = 0;

void process_character(char c)
{
    if (c == '\0') // End-of-message detected
    {
        if (buffer) // Ensure buffer exists
        {
            buffer[buf_index] = '\0';    // Null-terminate the string
            write(1, buffer, buf_index); // Print the full message
            write(1, "\n", 1);
            free(buffer); // Free allocated memory
            buffer = NULL;
        }
        buf_index = 0;
        length = 0;
    }
    else
    {
        if (buf_index == 0) // First character received, allocate memory
        {
            length = 1;
            buffer = malloc(length + 1); // Allocate space for one char + null terminator
            if (!buffer)
            {
                free(buffer);
                exit(1);
            }
        }
        else
        {
            length++;
            char *new_buffer = realloc(buffer, length + 1);
            if (!new_buffer)
            {
                free(buffer);
                // write(1, "Memory reallocation failed\n", 29);
                exit(1);
            }
            buffer = new_buffer;
        }
        buffer[buf_index++] = c; // Store the received character and increment index
    }
}

void store_bit(int sig)
{
    // Set the bit in the character if SIGUSR2 is received.
    // (If SIGUSR1 is received, the bit remains 0.)
    char_accum |= (sig == SIGUSR2);
    bit++;

    if (bit == 8) // If 8 bits have been received, we have a complete character.
    {
        process_character(char_accum);
        bit = 0;
        char_accum = 0; // Reset for the next character.
    }
    else
    {
        char_accum <<= 1; // Shift left to prepare for the next bit.
    }
}

void handle_signal(int sig, siginfo_t *info, void *context)
{
    (void)info;
    (void)context;
    store_bit(sig);
}

int main()
{
    struct sigaction sa;

    // Clear the sigaction struct and set up the mask.
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);

    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = handle_signal;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    ft_printf("Server running. PID: %d\n", getpid());
    while (1)
        pause(); // Wait for signals from client

    return 0;
}

// #include <signal.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>

// /*
//  * This signal handler accumulates bits into a character.
//  * When 8 bits are received, it either appends the character
//  * to a dynamically allocated buffer or, if the character is '\0',
//  * prints the full message and frees the memory.
//  */
// void sigusr_handler(int signum, siginfo_t *info, void *context)
// {
//     // Static variables to preserve state between signals.
//     static int bit_count = 0;
//     static unsigned char character = 0;
//     static char *buffer = NULL;
//     static int index = 0;

//     (void)info;
//     (void)context;

//     // If SIGUSR2, set the corresponding bit.
//     if (signum == SIGUSR2)
//         character |= (1 << bit_count);
//     // For SIGUSR1, do nothing; the bit remains 0.

//     bit_count++;

//     // Once we have 8 bits, we have a complete character.
//     if (bit_count == 8)
//     {
//         if (character == '\0')
//         {
//             // End-of-message: reallocate to exactly fit the message (with null terminator)
//             char *final_msg = malloc(index + 1);
//             if (!final_msg)
//                 exit(1);
//             for (int i = 0; i < index; i++)
//                 final_msg[i] = buffer[i];
//             final_msg[index] = '\0';

//             // Print the complete message
//             write(1, final_msg, index);
//             write(1, "\n", 1);

//             // Clean up
//             free(final_msg);
//             free(buffer);
//             buffer = NULL;
//             index = 0;
//         }
//         else
//         {
//             // Append the character to the buffer.
//             char *new_buffer;
//             if (buffer == NULL)
//                 new_buffer = malloc(1);
//             else
//                 new_buffer = realloc(buffer, index + 1);
//             if (!new_buffer)
//                 exit(1);
//             buffer = new_buffer;
//             buffer[index] = character;
//             index++;
//         }
//         // Reset for the next character.
//         bit_count = 0;
//         character = 0;
//     }
// }

// int main(void)
// {
//     struct sigaction sa;

//     // Initialize the signal mask.
//     sigemptyset(&sa.sa_mask);
//     sigaddset(&sa.sa_mask, SIGUSR1);
//     sigaddset(&sa.sa_mask, SIGUSR2);

//     sa.sa_flags = SA_RESTART | SA_SIGINFO;
//     sa.sa_sigaction = sigusr_handler;

//     // Install the signal handlers.
//     if (sigaction(SIGUSR1, &sa, NULL) < 0 ||
//         sigaction(SIGUSR2, &sa, NULL) < 0)
//     {
//         perror("sigaction");
//         exit(1);
//     }

//     printf("Server running. PID: %d\n", getpid());

//     // Wait indefinitely for signals.
//     while (1)
//         pause();

//     return 0;
// }

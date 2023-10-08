#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

int typing_speed = 0;
struct timeval wpm_start, wpm_stop;
struct timeval program_start, program_stop;

int get_rand_int(int lower, int upper)
{ return (rand() % (upper - lower)) + lower; }

int get_words(char** buffer, int size)
{
    // Gets a chunk of words of size (size) from the file,
    // start line is a random line on the file, 
    // adds (size) to it to get the chunk

    FILE* file = fopen("words.txt", "r");

    int start_line = get_rand_int(0, 1000);

    // Checks if we go out of the file 
    if ((start_line + size) > 1000)
        start_line -= ((start_line + size) - 1000) - 1;

    int end_line = (start_line + size);
    int current_line = 0;
    char line[256];
    int i = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        current_line++;
        if (current_line >= start_line && current_line < end_line)
        {
            buffer[i] = (char*)malloc(((strlen(line) - 1) * sizeof(char)));
            memcpy(buffer[i], line, strlen(line) - 1);
            i++;
        }

        if (current_line >= end_line)
            break;
    }

    fclose(file);

    return 1;
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    int size = 15;

    if (argc > 1)
        size = atoi(argv[1]);

    char* buffer[size];
    memset(buffer, 0, size * sizeof(char*));

    int times[size];

    int misses = 0;
    float accuracy = 0;

    get_words(buffer, size);

    initscr();

    addstr("TYPING SPEED TEST!!!\nPRESS ANY KEY TO START...");
    getch();
    clear();
    gettimeofday(&program_start, NULL);

    noecho();

    for (int j = 0; j < size; j++)
    {
        char* rand_word = buffer[j];

        // Get maximum row and col
        int row, col;
        getmaxyx(stdscr, row, col);

        // This is to print the words

        start_color();
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        attron(A_BOLD | COLOR_PAIR(2));
        addstr(rand_word);
        attroff(A_BOLD | COLOR_PAIR(2));

        char* typing_speed_c = malloc(sizeof(char) * (log10(typing_speed) + 1));

        sprintf(typing_speed_c, "%d", typing_speed);

        mvaddstr(row - 1, 0, "WPM: ");
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);

        attron(COLOR_PAIR(1));
        mvaddstr(row - 1, 5, typing_speed_c);
        attroff(COLOR_PAIR(1));

        move(1, 0);

        gettimeofday(&wpm_start, NULL);
        for (int i = 0; i < strlen(rand_word);)
        {
            char c = getch();
            if (c == rand_word[i])
            {
                addch(c);
                i++;
            } else 
                misses++;
        }

        gettimeofday(&wpm_stop, NULL);

        typing_speed = (60 / (int)(wpm_stop.tv_sec - wpm_start.tv_sec));
        times[j] = typing_speed;

        clear();

        free(typing_speed_c);
    }

    gettimeofday(&program_stop, NULL);

    int time_total = (program_stop.tv_sec - program_start.tv_sec);

    endwin();

    int average;

    for (int i = 0; i < size; i++)
    {
        average += times[i];
    }

    average /= size;

    int tmp_size = 0;

    for (int i = 0; i < size; i++)
        tmp_size += strlen(buffer[i]);

    accuracy = ((float)tmp_size / (float)(tmp_size + misses)) * 100.0f;

    printf("WORDS: %d\n", size);
    printf("AVERAGE WPM: %d\n", average);
    printf("MISSES: %d\n", misses);
    printf("ACCURACY: %f\n", accuracy);
    printf("TIME: %d seconds\n", time_total);

    for (int i = 0; i < size; i++)
        free(buffer[i]);

    return 0;
}

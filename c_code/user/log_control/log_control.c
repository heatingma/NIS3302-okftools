#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "log_control.h"

#define TM_FMT "%Y-%m-%d %H:%M:%S"
#define LOG_FMT "%s %d %s %d %s %s %s %s %s\n"
#define LOG_SHOW_FMT "%s(%d) %s(%d) %s \"%s\" %s %s\n"
#define max(a, b) a > b ? a : b

volatile int com = 0;    //com = 1 pause
int next[256];
int malloc_size = 0;

typedef struct LOG
{
    char *username, *commandname, *logtime, *file_path, *opentype, *openresult;
    int uid, pid;
} log;

void help()
{
    printf("sort\n\tsort NUM: Sort the logs by the NUMth word\n");
    printf("search\n\tsearch WORD: Search for logs containing WORD\n");
    printf("merge\n\tmerge: Merge the same logs\n");
    printf("reload\n\treload: Reload the logs from the file\n");
    printf("pause\n\tpause: Pause Logging\n");
    printf("continue\n\tcontinue: Continue Logging\n");
    printf("clear\n\tclear: Clear the log file\n");
    printf("show\n\tshow: Show the logs\n");
    printf("help\n\thelp: Show this page\n");
    printf("exit\n\texit: Exit program\n");
}

void show(log *logs, int size, bool *limit)
{
    system("clear");
    if (limit)
    {
        for (int i = 0; i < size; ++i)
        {
            if (limit[i])
                printf(LOG_SHOW_FMT, logs[i].username, logs[i].uid, logs[i].commandname, logs[i].pid,
                    logs[i].logtime, logs[i].file_path, logs[i].opentype, logs[i].openresult);
        }
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            printf(LOG_SHOW_FMT, logs[i].username, logs[i].uid, logs[i].commandname, logs[i].pid,
                logs[i].logtime, logs[i].file_path, logs[i].opentype, logs[i].openresult);
        }
    }
}

void clear(void)
{
    FILE * fp = fopen("log", "w");
    fclose(fp);   
}

void log_var_init(log* logs)
{
    for (int i = 0; i < malloc_size; ++i)
    {
        logs[i].username = (char*)malloc(sizeof(char) * 30);
        logs[i].commandname = (char*)malloc(sizeof(char) * 30);
        logs[i].logtime = (char*)malloc(sizeof(char) * 20); //
        logs[i].file_path = (char*)malloc(sizeof(char) * 256);
        logs[i].opentype = (char*)malloc(sizeof(char) * 16);
        logs[i].openresult = (char*)malloc(sizeof(char) * 20);
    }
}

void release(log* logs, int size)
{
    for (int i = 0; i < size; ++i)
    {
        free(logs[i].username);
        free(logs[i].commandname);
        free(logs[i].logtime);
        free(logs[i].file_path);
        free(logs[i].opentype);     //debug:fail
        free(logs[i].openresult);
    }
    free(logs);
}

void count()
{
    FILE *logfile;
    logfile=fopen("./log", "r");
    
    malloc_size = 0;
    char line[1024];

    while (fgets(line, 1024, logfile) != NULL)
        ++malloc_size;

    fclose(logfile);
}

void open_log(log* logs) //read all logs and return the num of logs
{
    FILE *logfile;
    logfile=fopen("./log", "r");
    
    int i = 0;
    
    char tmp[20];


    for (int i = 0; i < malloc_size; ++i)
    {
        fscanf(logfile, LOG_FMT, logs[i].username, &logs[i].uid, logs[i].commandname,
            &logs[i].pid, logs[i].logtime, tmp, logs[i].file_path, logs[i].opentype, logs[i].openresult);
        logs[i].logtime[10] = ' ';
        logs[i].logtime[11] = '\0';
        strcat(logs[i].logtime, tmp);
    }
    fclose(logfile);
}

void swap(log *a, log *b)
{
    log tmp;
    tmp.commandname = a->commandname;
    tmp.file_path = a->file_path;
    tmp.logtime = a->logtime;
    tmp.openresult = a->openresult;
    tmp.opentype = a->opentype;
    tmp.username = a->username;
    tmp.pid = a->pid;
    tmp.uid = a->uid;

    a->commandname = b->commandname;
    a->file_path = b->file_path;
    a->logtime = b->logtime;
    a->openresult = b->openresult;
    a->opentype = b->opentype;
    a->username = b->username;
    a->pid = b->pid;
    a->uid = b->uid;

    b->commandname = tmp.commandname;
    b->file_path = tmp.file_path;
    b->logtime = tmp.logtime;
    b->openresult = tmp.openresult;
    b->opentype = tmp.opentype;
    b->username = tmp.username;
    b->pid = tmp.pid;
    b->uid = tmp.uid;
}

void quick_sort(log *begin, log *end, bool (*cmp)(log *a, log *b))
{
    int len = end - begin;
    if (len <= 1)
        return;
    
    log *i = begin, *j = end - 1;
    log *pivot = begin + ((len + 1) >> 1) - 1;
    while (j >= i)
    {
        while (cmp(i, pivot))
            i++;
        while (cmp(pivot, j))
            --j;
        if (j - i >= 0)
        {
            swap(i, j);
            ++i;
            --j;
        }
    }
    if (j - begin > 0)
        quick_sort(begin, i, cmp);
    if (end - i > 1)
        quick_sort(i, end, cmp);
}

bool cmp_username(log *a, log *b){ return strcmp(a->username, b->username) < 0; }

bool cmp_uid(log *a, log *b){ return a->uid < b->uid; }

bool cmp_commandname(log *a, log *b){ return strcmp(a->commandname, b->commandname) < 0; }

bool cmp_pid(log *a, log *b){ return a->pid < b->pid; }

bool cmp_logtime(log *a, log *b){ return strcmp(a->logtime, b->logtime) < 0; }

bool cmp_filepath(log *a, log *b){ return strcmp(a->file_path, b->file_path) < 0; }

bool cmp_opentype(log *a, log *b){ return strcmp(a->opentype, b->opentype) < 0; }

bool cmp_openresult(log *a, log *b){ return strcmp(a->openresult, b->openresult) < 0; }


bool cmp_username_rev(log *a, log *b){ return strcmp(a->username, b->username) > 0; }

bool cmp_uid_rev(log *a, log *b){ return a->uid > b->uid; }

bool cmp_commandname_rev(log *a, log *b){ return strcmp(a->commandname, b->commandname) > 0; }

bool cmp_pid_rev(log *a, log *b){ return a->pid > b->pid; }

bool cmp_logtime_rev(log *a, log *b){ return strcmp(a->logtime, b->logtime) > 0; }

bool cmp_filepath_rev(log *a, log *b){ return strcmp(a->file_path, b->file_path) > 0; }

bool cmp_opentype_rev(log *a, log *b){ return strcmp(a->opentype, b->opentype) > 0; }

bool cmp_openresult_rev(log *a, log *b){ return strcmp(a->openresult, b->openresult) > 0; }

int stoi(const char *str)
{
    int res = 0;
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
            res = (res << 3) + (res << 1) + str[i] - '0';
        else
            return 0;
    }
    return res;
}

bool KMP(const char *find, const char *in)
{
    int j = 0, k = -1;
    next[0] = -1;
    while (find[j])
    {
        if (k == -1 || find[j] == find[k])
        {
            ++j; ++k; next[j] = k;
        }
        else
            k = next[k];
    }
    int i = 0;
    j = 0;
    while (find[j] && in[i])
    {
        if (find[j] == in[i])
        {
            ++i; ++j;
        }
        else if (next[j] == -1)
            ++i;
        else
            j = next[j];
    }
    if (!find[j])
        return true;
    else
        return false;
}

//remember to free the res
bool *search(const log *logs, int size, const char *str)    //search all, put results in res and return size
{
    int num = stoi(str);
    bool *limit = (bool *)malloc(sizeof(bool) * size);
    if (num || str[0] == '0')  //search the num
    {
        for (int i = 0; i < size; ++i)
        {
            if (KMP(str, logs[i].username) || KMP(str, logs[i].commandname) || KMP(str, logs[i].logtime) || 
                KMP(str, logs[i].file_path) || KMP(str, logs[i].opentype) || KMP(str, logs[i].openresult))
                limit[i] = true;
            else if(logs[i].pid == num || logs[i].uid == num)
                limit[i] = true;
            else
                limit[i] = false;
        }
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            if (KMP(str, logs[i].username) || KMP(str, logs[i].commandname) || KMP(str, logs[i].logtime) || 
                KMP(str, logs[i].file_path) || KMP(str, logs[i].opentype) || KMP(str, logs[i].openresult))
                limit[i] = true;
            else
                limit[i] = false;
        }
    }
    return limit;
}

//return size of new logs. remember to update the size
int merge(log *logs, int size)
{
    bool *avai = (bool *)malloc(sizeof(bool) * size);
    for (int i = 0; i < size; ++i)
        avai[i] = true;
    quick_sort(logs, logs + size, cmp_logtime_rev);
    for (int i = 0; i < size; ++i)
    {
        if (avai[i])
            for (int j = i + 1; j < size; ++j)
            {
                if (avai[j])
                    avai[j] = strcmp(logs[i].commandname, logs[j].commandname) || strcmp(logs[i].file_path, logs[j].file_path) ||
                        strcmp(logs[i].opentype, logs[j].opentype) || logs[i].uid != logs[j].uid || logs[i].pid != logs[j].pid;
            }
    }
    int tmp = 0;
    for (int i = 0; i < size; ++i)
    {
        if (!avai[i])
        {
            tmp = max(tmp, i) + 1;
            for (; tmp < size; ++tmp)
            {
                if (avai[tmp])
                {
                    swap(logs + i, logs + tmp);
                    avai[i] = true;
                    avai[tmp] = false;
                    break;
                }
            }
            if (tmp == size)
            {
                free(avai);
                return i;
            }
        }
    }
    free(avai);
    return size;
}

void run()
{
    count();
    int size = malloc_size;
    log *logs= (log*)malloc(sizeof(log) * malloc_size);
    log_var_init(logs);
    char* command = (char *)malloc(sizeof(char) * 30);
    open_log(logs);

    while (1)
    {
        scanf("%s", command);
        if (!strcmp(command, "sort"))
        {
            int type;
            scanf("%d", &type);
            switch (type)
            {
                case 1:
                    quick_sort(logs, logs + size, cmp_username);
                    break;
                case 2:
                    quick_sort(logs, logs + size, cmp_uid);
                    break;
                case 3:
                    quick_sort(logs, logs + size, cmp_commandname);
                    break;
                case 4:
                    quick_sort(logs, logs + size, cmp_pid);
                    break;
                case 5:
                    quick_sort(logs, logs + size, cmp_logtime);
                    break;
                case 6:
                    quick_sort(logs, logs + size, cmp_filepath);
                    break;
                case 7:
                    quick_sort(logs, logs + size, cmp_opentype);
                    break;
                case 8:
                    quick_sort(logs, logs + size, cmp_openresult);
                    break;
                case -1:
                    quick_sort(logs, logs + size, cmp_username_rev);
                    break;
                case -2:
                    quick_sort(logs, logs + size, cmp_uid_rev);
                    break;
                case -3:
                    quick_sort(logs, logs + size, cmp_commandname_rev);
                    break;
                case -4:
                    quick_sort(logs, logs + size, cmp_pid_rev);
                    break;
                case -5:
                    quick_sort(logs, logs + size, cmp_logtime_rev);
                    break;
                case -6:
                    quick_sort(logs, logs + size, cmp_filepath_rev);
                    break;
                case -7:
                    quick_sort(logs, logs + size, cmp_opentype_rev);
                    break;
                case -8:
                    quick_sort(logs, logs + size, cmp_openresult_rev);
                    break;
                default:
                    printf("error\n");
                    type = 0;
            }
            if (type)
                show(logs, size, 0);
        }
        else if (!strcmp(command, "search"))
        {
            scanf("%s", command);
            bool *tmp = search(logs, size, command);
            show(logs, size, tmp);
            free(tmp);
        }
        else if (!strcmp(command, "merge"))
        {
            size = merge(logs, size);
            show(logs, size, 0);
        }
        else if (!strcmp(command, "reload"))
        {
            count();
            release(logs, size);
            size = malloc_size;

            logs= (log*)malloc(sizeof(log) * malloc_size);
            log_var_init(logs);
            open_log(logs);
            show(logs, size, 0);
        }
        else if (!strcmp(command, "pause"))
        {
            com = 1;
        }
        else if (!strcmp(command, "continue"))
        {
            com = 0;
        }
        else if (!strcmp(command, "clear"))
        {
            clear();
            release(logs, size);
            malloc_size = size = 0;

            logs = NULL;
            system("clear");
        }
        else if (!strcmp(command, "show"))
        {
            show(logs, size, 0);
        }
        else if (!strcmp(command, "help"))
        {
            help();
        }
        else if (!strcmp(command, "exit"))
        {
            break;
        }
        else if (!command[0])
            continue;
        else
        {
            printf("Wrong command! Please try again.\n");
        }
        command[0] = '\0';
    }

    release(logs, malloc_size);
    exit(0);
    return;
}

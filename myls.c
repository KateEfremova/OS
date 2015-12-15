#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>


struct ls_options
{
    bool l, a;
};


void print_rights(struct stat* file_info)
{
    char rights[2][10] = {{'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
                        {'d', 'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x'}};
    printf("%c", rights[((1 << 14) & (int)(file_info->st_mode)) > 0][0]);
    int i;
    for(i = 1; i < 10; i++)
    {
        printf("%c", rights[(((512 >> i) & ((int)(file_info->st_mode))) > 0)][i]);
    }
}


bool is_directory(struct stat* file_info)
{
    return(((1 << 14) & (int)(file_info->st_mode)) > 0);
}


void print_full_info(struct dirent* file, struct stat* file_info)
{
    print_rights(file_info);
    printf(" %s %s %lld %d-%d%d-%d%d %d:%d:%d %s\n",
        getgrgid(file_info->st_gid)->gr_name,
        getpwuid(file_info->st_uid)->pw_name,
        (long long)file_info->st_size,
        localtime(&(file_info->st_mtime))->tm_year + 1900,
        localtime(&(file_info->st_mtime))->tm_mon / 10,
        localtime(&(file_info->st_mtime))->tm_mon % 10 + 1,
        localtime(&(file_info->st_mtime))->tm_mday / 10,
        localtime(&(file_info->st_mtime))->tm_mday % 10,
        localtime(&(file_info->st_mtime))->tm_hour,
        localtime(&(file_info->st_mtime))->tm_min,
        localtime(&(file_info->st_mtime))->tm_sec,
        file->d_name);
}


void str_concatenate(char* x, char* y, char** destination)
{
    bool slashEnd = (x[strlen(x) - 1] == '/');
    *destination = malloc(sizeof(char) * (strlen(x) + strlen(y) + 2));
    strcpy(*destination, x);
    if(!slashEnd) {
        strcat(*destination, "/");
    }
    strcat(*destination, y);
    (*destination)[strlen(x) + strlen(y) + (1 - slashEnd)] = '\0';
}


void free_after_concatenate(char** destination)
{
    free(*destination);
}


void processing_of_file(char* full_name, struct dirent* file, struct ls_options options)
{
    struct stat file_info;
    stat(full_name, &file_info);
    if(options.l)
    {
        print_full_info(file, &file_info);
    }
    else
    {
        printf("%s ", file->d_name);
    }
}


void make_ls(char* path, struct ls_options options)
{
    DIR* stream_to_directory = opendir(path);
    if(stream_to_directory == NULL) {
    	printf("Error while processing %s dir\n", path);
    	return;
    }
    printf("%s\n", path);
    struct dirent* file = readdir(stream_to_directory);
    char* full_name;
    while(file != NULL)
    {
        if( file->d_name[0] != '.'  ||  options.a ) {
            str_concatenate(path, file->d_name, &full_name);
            processing_of_file(full_name, file, options);
            free_after_concatenate(&full_name);
        }
        file = readdir(stream_to_directory);
    }
    closedir(stream_to_directory);
    printf("\n\n");
}


int init_options(char* inc_param, struct ls_options* options)
{
    int i;
    for(i = 1; i < strlen(inc_param); i++)
        switch (inc_param[i])
        {
            case 'l':
                options->l = true;
                break;
            case 'a':
                options->a = true;
                break;
            case 0:
                break;
            default:
                return -1;
        }
}


int init_args(int argc, char** args, struct ls_options* options)
{
    int i;
    options->l = false;
    options->a = false;
    if(argc > 1)
    {
        for(i = 1; i < argc; i++)
        {
            if(args[i][0] == '-')
            {
                if(init_options(args[i], options) == -1)
                {
                    printf("Incorrect option\n");
                    return -1;
                }
            }
        }
    }
}


int main(int argc, char** args)
{
    struct ls_options options;
    if(init_args(argc, args, &options) == -1)
    {
        return -1;
    }
    int i;
    for( i = 1; i < argc; ++i ) {
        if( args[i][0] != '-') {
            make_ls(args[i], options);
        }
    }
    return 0;
}

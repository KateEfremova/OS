#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

void crpath(char buff[], char folder_name[], char file_name[]);

void copy(int argc, char* argv[]) {
	char buf[101];
	char path[1000];
	struct dirent* ent;
	struct stat st;
	if (argc == 3) {
		DIR* inp_dir = opendir(argv[1]);
		int err;
		if (inp_dir && ((err = mkdir(argv[2], 0777)) == 0 || errno == EEXIST)) {
			do {
				if (ent = readdir(inp_dir)) {
					crpath(path, argv[1], ent->d_name);
					stat(path, &st);
					if(!S_ISDIR(st.st_mode)) {
						int d_in = open(path, O_RDONLY);
						crpath(path, argv[2], ent->d_name);
						int d_out = creat(path, st.st_mode);
						if (d_in != -1 && d_out != -1) {

							int n;
							while ((n = read(d_in, buf, 100)) > 0) {
								write(d_out, buf, n);
							}
							printf("file copied: %s\n", ent->d_name);
						}
						else printf("%s\n", strerror(errno));
					}
					else if (strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0) {
						printf("%s\n", path);
						char* arg[3];
						arg[0] = argv[0];
						arg[1] = (char *) calloc(1000, sizeof(char));
						arg[2] = (char *) calloc(1000, sizeof(char));
						strcpy(arg[1], path);
						crpath(path, argv[2], ent->d_name);
						strcpy(arg[2], path);
						copy(3, arg);					}
				}
			} while (ent);
			stat(argv[1], &st);
			chmod(argv[2], st.st_mode);
		}
		else printf("%s\n", strerror(errno));
	}
	else printf("Needs two arguments\n");
}

int main(int argc, char* argv[]) {
	copy(argc, argv);
	return 0;
}


void crpath(char buff[], char folder_name[], char file_name[]) {
	buff[0] = '\0';
	strcat(buff, folder_name);
	strcat(buff, "/");
	strcat(buff, file_name);
}

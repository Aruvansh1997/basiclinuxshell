#define _GNU_SOURCE
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>

//Function Declarations
int halo_cd(char **args);
int halo_help(char **args);
int halo_exit(char **args);
int halo_ls(char **args);
int halo_delete(char **args);
int halo_mkdir(char **args);
int halo_display(char **args);
int halo_search(char **args);
char *self_strings[] = {
  "help",
  "cd",
  "exit",
"ls",
"delete",
"makedir",
"display",
"search"

};

int (*self_func[]) (char **) = {
  &halo_help,
  &halo_cd,
  &halo_exit,
  &halo_ls,
&halo_delete,
&halo_mkdir,
&halo_display,
&halo_search
};

int halo_inbuilt_func_num() {
  return sizeof(self_strings) / sizeof(char *);
}
int halo_search(char **args)
{
FILE *fp2;
int f=-1;
struct dirent *de;
DIR *dr=opendir(".");
if(dr==NULL)
return 0;
while((de=readdir(dr))!=NULL)
if(!strcmp(de->d_name,args[1]))
{
printf("%s\n","found");
f=1;
break;
}
int ch;
closedir(dr);
if(f==-1)
printf("the searched file is not present in this directory");
return 1;
}
int halo_display(char **args)
{
char ch;
   FILE *fp;
  
   fp = fopen(args[1], "r"); // read mode
 
   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
 
   printf("The contents of %s file are:\n",args[1]);
 
   while((ch = fgetc(fp)) != EOF)
      printf("%c", ch);
 
   fclose(fp);
   return 1;
}
 
int halo_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "halosh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("halosh");
    }
  }
  return 1;
}
int halo_mkdir(char **args)
{

FILE *fp1=fopen(args[1],args[2]);
fclose(fp1);
printf("the file has been created with the specified mode");
return 1;
}


int halo_ls(char **args)
{
DIR *dp;
struct dirent *ep;
dp=opendir("./");
if(dp!=NULL)  
{
while((ep=readdir(dp)))
puts(ep->d_name);
(void)closedir(dp);

}
return 1;

}
int halo_delete(char **args)
{
int status;
status=remove(args[1]);
if(status==1)
perror("halosh");
else
printf("successfully deleted the files");
return 1;

}

int halo_help(char **args)
{
  int i;
  printf("Basic shell use at your own risk \n");
  printf("Some in built functions are:\n");

  for (i = 0; i < halo_inbuilt_func_num(); i++) {
    printf("  %s\n", self_strings[i]);
  }
  return 1;
}

int halo_exit(char **args)
{
  return 0;
}

int halo_launch(char **args)
{
  pid_t pid;
  int halo_flag;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("halosh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("halosh");
  } else {
    do {
      waitpid(pid, &halo_flag, WUNTRACED);
    } while (!WIFEXITED(halo_flag) && !WIFSIGNALED(halo_flag));
  }

  return 1;
}

int halo_exec(char **args)
{
  int i,size_num;

  if (args[0] == NULL) {
    return 1;
  }

  size_num = halo_inbuilt_func_num();

  for (i = 0; i < size_num; i++) {
    if (strcmp(args[0], self_strings[i]) == 0) {
      return (*self_func[i])(args);
    }
  }

  return halo_launch(args);
}

char *halo_get_line(void)
{
  char *line = NULL;
  size_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
}

#define HALO_TOK_BUFSIZE 64
#define HALO_TOKEN_DELIMITER " \t\r\n\a"

char **halo_split_func(char *line)
{
  int bufsize = HALO_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "halosh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, HALO_TOKEN_DELIMITER);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += HALO_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "halosh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, HALO_TOKEN_DELIMITER);
  }
  tokens[position] = NULL;
  return tokens;
}

void halo_loop(void)
{
  time_t timer;
  char time_now[26];
  struct tm* tm_info;
  char *line;
  char **args;
  int halo_flag;
  char *path_name,*path_name_base;

  do {

    time(&timer);
    tm_info = localtime(&timer);
    strftime(time_now, 26, "%H:%M:%S", tm_info);
   getcwd(path_name,1024 );
    path_name_base = (char*)basename(path_name);
    printf("%s>%s> ",time_now,path_name_base);
    line = halo_get_line();
    args = halo_split_func(line);
    halo_flag = halo_exec(args);

    free(line);
    free(args);
  } while (halo_flag);
}

int main(int argc, char **argv)
{
  halo_loop();
  return EXIT_SUCCESS;
}


#include <SDL3/SDL_log.h> 
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#define BUF_SIZE 512

char *trim(char *src){
    if(SDL_strlen(src) == 0) return src;
    char *end;

    while(SDL_isspace((unsigned char)*src)){
        src++;
    }

    end = src + SDL_strlen(src) - 1;

    while(SDL_isspace((unsigned char)*end)){
        end--;
    }

    *(end+1) = 0;
    return src;
}

int read_config(){
    SDL_IOStream *config_stream = SDL_IOFromFile("config.ini", "a+");
    void *config_file = SDL_LoadFile_IO(config_stream, NULL, true);
    if(!config_file){
        //ERROR HANDLE HERE
        return 1;
    }

    char *newline;
    //char buf[BUF_SIZE];

    while(1){
        newline = SDL_strchr(config_file, '\n');
        if(newline){
            *newline = 0; 
        }

        char *val = SDL_strchr(config_file, '=');
        char *key = config_file;

        if(val){
            *val = 0;
            val++;
            val = trim(val);
            key = trim(key);
            SDL_Log("key: %s, val: %s", key,val);
        }
        config_file = newline+1;
        //handling files with no EOF. because I guess we should support bad text editors that have no terminating newline at EOF...
        if(!newline || *(newline+1) == 0) break;
    }
    return 0;
}

int main(){
    read_config();
}

#include "app.h"
#include "string.h"

int main(int argc, char* argv[]){
    if (argc != 3) {
        printf("Usage: grep [PATTERN] [FILE]\n");
        return -1;
    }

    char* pattern = argv[1];
    char* fname = argv[2];
    int pattern_len = strlen(pattern);
    if (pattern_len >= 1024) {
        printf("grep: pattern too long for buffer\n");
        return -1;
    }

    // get the inode of the file
    int inode = dir_lookup(workdir_ino, fname);
    if(inode < 0){
        printf("grep: cannot open %s\n", fname);
        return -1;
    }

    int offset = 0;  // how far we are in the file
    char buff[BLOCK_SIZE+1];  // loading BLOCK_SIZEE = 4 Kb chunks of data from file at a time
    char line_buff[2048];  // temporarily stroes the line which is currenlt search through
    int line_idx = 0; 

    // fetch blocks until file ends
    while(1){
        int fread = file_read(inode, offset, buff);

        if(fread < 0){
            printf("Error in reading file\n");
            return -1;
        }
        // End of file
        int is_empty = 1;
        for (int i = 0; i < BLOCK_SIZE; i++) {
            if (buff[i] != 0) {
                is_empty = 0;
                break;
            }
        }
        if(is_empty && offset > 0) {
            break;
        }
        // scan every char in the block
        for(int i=0; i<BLOCK_SIZE; i++){
            char c = buff[i];

            if(c == '\0'){ // End of file
                if(line_idx > 0) {
                    line_buff[line_idx] = '\0'; // terminate the buff, so that its a valid str

                    // check the pattern
                    if(strstr(line_buff, pattern) != NULL){
                        printf("%s\n", line_buff);
                    }
                    line_idx = 0;  // reset so done: won't print again
                }
                goto done;  //Exits both loops                
            }
            else if(c == '\n'){
                if(line_idx > 0) {
                    line_buff[line_idx] = '\0'; // terminate the buff, so that its a valid str

                    // check the pattern
                    if(strstr(line_buff, pattern) != NULL){
                        printf("%s\n", line_buff);
                    }
                }
                line_idx = 0; //reset it for next line 
            }
            else{
                if(line_idx < sizeof(line_buff) - 1) {
                    line_buff[line_idx++] = c;
                } 
                else {
                    // Buffer is full
                    line_buff[line_idx] = '\0';
                    
                    // keep the last (pattern_len - 1) characters
                    // in case pattern is split across buffer boundary
                    if(pattern_len > 1) {
                        int keep_chars = pattern_len - 1;
                        if(keep_chars > line_idx) {
                            keep_chars = line_idx;
                        }
                        
                        // Copy last few characters to beginning of buffer
                        for(int j = 0; j < keep_chars; j++){
                            line_buff[j] = line_buff[line_idx - keep_chars + j];
                        }
                        line_idx = keep_chars;
                        
                        // Add current character
                        line_buff[line_idx++] = c;
                    } 
                    else {
                        // Pattern is single character
                        line_buff[0] = c;
                        line_idx = 1;
                    }            
                }
            }
        }
        offset++; 
    }
    
done:  
    // if there's no "\n" but data in buffer, then check the pattern in that buffer
    if (line_idx > 0) {
        line_buff[line_idx] = '\0';
        if (strstr(line_buff, pattern)) {
            printf("%s\n", line_buff);
        }
    }
    return 0;
}
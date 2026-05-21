#include "app.h"
#include <string.h>

/*
 * wcl - Count total lines across multiple files.
 * Usage: wcl [FILE1] [FILE2] ...
 */
int main(int argc, char** argv) {
    // 1. Validate arguments
    if (argc < 2) {
        printf("Usage: wcl [FILE1] [FILE2] ...\n");
        return 0;
    }

    int total_lines = 0;
    char buf[BLOCK_SIZE + 1]; // +1 for null terminator

    // 2. Loop through ALL files provided in arguments
    for (int i = 1; i < argc; i++) {
        char *filename = argv[i];

        int file_ino = dir_lookup(workdir_ino, filename);
        if (file_ino < 0) {
            printf("wcl: cannot open %s\n", filename);
            continue; // Skip bad files, keep counting others
        }

        int offset = 0;
        
        // 3. Read file loop
        while (1) {
            // Clear buffer to ensure strlen works correctly
            memset(buf, 0, BLOCK_SIZE + 1);

            int status = file_read(file_ino, offset, buf);
            
            // Check for error
            if (status != 0) break;

            // Get actual data length
            int len = strlen(buf);
            if (len == 0) break; // EOF

            // Count newlines in this block
            for (int j = 0; j < len; j++) {
                if (buf[j] == '\n') {
                    total_lines++;
                }
            }

            // Stop if we read a partial block (end of file)
            if (len < BLOCK_SIZE) break;

            offset += BLOCK_SIZE;
        }
    }

    // 4. Print grand total
    printf("%d\n", total_lines);
    return 0;
}
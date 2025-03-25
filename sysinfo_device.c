/**
 * sysinfo_device.c
 * 
 * An API to interact with the sysinfo device /dev node.
 * 
 * @author Mikey Fennelly
 */

#include "./sysinfo_device.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SYSINFO_DEVICE "/dev/sysinfo"
#define READ_BUF_SIZE 256
#define SET_CIT_CPU _IOW('C',CPU, int)          // set the current_info_type to cpu
#define SET_CIT_MEM _IOW('M', MEMORY, int)      // set the current_info_type to memory
#define SET_CIT_DISK _IOW('D', DISK, int)       // // set the current_info_type to disk

int change_current_info_type(int dev_fd, int mode);
char* get_sysinfo(int info_type);
char* read_device(int fd);

/**
 * Get sysinfo based on 1 of 3 sysinfo types.
 * 
 * 1. CPU
 * 2. MEMORY
 * 3. DISK
 * 
 * @return - if sucessful, returns pointer to string on heap.
 *              else returns NULL.
 */
char*
get_sysinfo(int info_type)
{
    // if an invalid info_type is passed return NULL.
    if (!(info_type >= 1 && info_type <= 3))
    {
        return NULL;
    }

    // open the /dev node
    int fd = open(SYSINFO_DEVICE, O_RDONLY);
    if (fd < 0)
    {
        perror("Could not open /dev node\n");
        exit(EXIT_FAILURE);
    }
    
    // change the current_info_type of the 
    // device to the requested info_type
    int ret = change_current_info_type(fd, info_type);
    if (ret != 0)
    {
        return NULL;
    }

    // read the device
    char* sysinfo = read_device(fd);
    if (sysinfo == NULL)
    {
        return NULL;
    }

    close(fd);

    return sysinfo;
}

/**
 * Change the current_info_type of the device.
 * 
 * @return status code, indicating success of ioctl operation.
 */
int
change_current_info_type(int dev_fd,
                         int mode)
{
    if (!(mode >= 1 && mode <=3))
    {
        printf("ERROR: No corresponding device info mode for integer: %d\n", mode);
        return EXIT_FAILURE;
    }

    if (mode == CPU)
    {
        if (ioctl(dev_fd, SET_CIT_CPU, CPU) == -1) {
            perror("ioctl SET_CIT_CPU failed");
        }
    }
    if (mode == MEMORY)
    {
        if (ioctl(dev_fd, SET_CIT_MEM, MEMORY) == -1) {
            perror("ioctl SET_CIT_MEM failed");
        }
    }
    if (mode == DISK)
    {
        if (ioctl(dev_fd, SET_CIT_DISK, DISK) == -1) {
            perror("ioctl SET_CIT_DISK failed");
        }
    }

    return 0;
}

/**
 * Read the /dev/sysinfo node.
 * 
 * @return - the contents of the read if successful
 *              else NULL.
 */
char*
read_device(int fd)
{
    char* sysinfo = malloc(READ_BUF_SIZE);

    // read the file until EOF
    int ret;
    ssize_t bytes_read;
    while((bytes_read = read(fd, sysinfo, READ_BUF_SIZE - 1)) > 0)
    {
        sysinfo[bytes_read] = '\0';
        printf("%s", sysinfo);
    }

    if (bytes_read == -1)
    {
        perror("Error reading file\n");
        return NULL;
    }

    return sysinfo;
}

int main(void)
{
    char* cpu_contents = get_sysinfo(CPU);
    free(cpu_contents);

    char* mem_contents = get_sysinfo(MEMORY);    
    free(mem_contents);

    char* disk_contents = get_sysinfo(DISK);
    free(disk_contents);

    return EXIT_SUCCESS;
}
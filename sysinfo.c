/**
 * sysinfo.c
 * 
 * Read from the /dev/sysinfo file for a specified info type
 * 
 * @author Mikey Fennelly
 */

#include "./sysinfo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SYSINFO_DEVICE "/dev/sysinfo"
#define READ_BUF_SIZE 256

int change_current_info_type(int mode);
char* get_sysinfo(int info_type);
char* read_device(void);

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
char* get_sysinfo(int info_type)
{
    // if an invalid info_type is passed return NULL.
    if (!(info_type >= 1 && info_type <= 3))
    {
        return NULL;
    }
    
    // change the current_info_type of the 
    // device to the requested info_type
    int ret = change_current_info_type(info_type);
    if (ret != 0)
    {
        return NULL;
    }

    // read the device
    char* info = read_device();
    if (info == NULL)
    {
        return NULL;
    }
}

/**
 * Change the current_info_type of the device.
 * 
 * @return status code, indicating success of ioctl operation.
 */
int change_current_info_type(int mode)
{
    return 0;
}

/**
 * Read the /dev/sysinfo node.
 * 
 * @return - the contents of the read if successful
 *              else NULL.
 */
char* read_device(void)
{
    char* sysinfo = malloc(READ_BUF_SIZE);

    // open the device
    int dev_fd = open(SYSINFO_DEVICE, O_RDONLY);
    if (dev_fd < 0)
    {
        printf("Could not read %s\n", SYSINFO_DEVICE);
        return NULL;
    }

    // read the file until EOF
    int ret;
    ssize_t bytes_read;
    while((bytes_read = read(dev_fd, sysinfo, READ_BUF_SIZE - 1)) > 0)
    {
        sysinfo[bytes_read] = '\0';
        printf("%s", sysinfo);
    }

    if (bytes_read == -1)
    {
        perror("Error reading file\n");
        return NULL;
    }

    ret = close(dev_fd);
    if (ret < 0)
    {
        printf("Could not close the device: %s\n", SYSINFO_DEVICE);
        exit(EXIT_FAILURE);
    }

    return sysinfo;
}

int main(void)
{
    char* contents = read_device();
    if (contents == NULL)
    {
        printf("Contents are null\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
#ifndef SYSINFO_H
#define SYSINFO_H

#define CPU 1
#define MEMORY 2
#define DISK 3

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
char* get_sysinfo(int info_type);

#endif
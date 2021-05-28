#ifndef __UTILS_H__
#define __UTILS_H__
/*
 * Library for utility functions
 */

#include <stdlib.h>

/***************************  Utils  ******************************/
/**
 * \brief Reserve memory blocks of a given size or abort on failure.
 * \param count Memory blocks to reserve
 * \param size  Size of every memory block
 * \return      A pointer to the allocated memory block.
 */
void *xzalloc(size_t count, size_t size);

#endif // __UTILS_H__

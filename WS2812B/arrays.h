/** @file */
#ifndef ARRAYS_H
#define ARRAYS_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define ANY_OF(A) (A[rand() % sizeof(A)])
#define ANY_INDEX(A) (rand() % sizeof(A))

#endif

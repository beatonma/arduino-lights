/** @file */
#ifndef ARRAYS_H
#define ARRAYS_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define ANY_OF(A) (A[rand() % ARRAY_SIZE(A)])
#define ANY_INDEX(A) (rand() % ARRAY_SIZE(A))
#define NEXT_INDEX(A, index) ((index + 1) % ARRAY_SIZE(A))

#endif

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H


typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,
    STATUS_ERR_BAD_DEGREE,
    STATUS_ERR_NO_MEMORY
} Status;

Status calc_poly(const double eps, const double a, double **out, const int deg, ...);


#endif
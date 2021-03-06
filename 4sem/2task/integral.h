#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double integrate(double (*function)(double x), double a, double b);
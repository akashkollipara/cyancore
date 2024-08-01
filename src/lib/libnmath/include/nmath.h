/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: nmath.h
 * Description		: This file contains prototypes of neo-math function
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once
#include <stdbool.h>
unsigned int clog2(unsigned long num);
unsigned long gcd(unsigned long a, unsigned long b);
unsigned long lcd(const unsigned int *a, unsigned int n);
long mod(long a, long b);
long multiplicative_inverse(long base, long subject);
int abs(int x);
bool isPrime(long);

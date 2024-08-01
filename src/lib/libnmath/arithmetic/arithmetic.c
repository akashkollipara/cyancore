/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: arithmetic.c
 * Description		: This file contains sources of neo-math function
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdbool.h>
#include <nmath.h>

/**
 * clog2
 *
 * @brief This function computes ceiling log of input to base 2
 *
 * @param[in] num: Input number
 *
 * @return clog2(num)
 */
unsigned int clog2(unsigned long num)
{
	return (num > 1) ? (1 + clog2((num & 0x01) ? ((num + 1) >> 1) : (num >> 1))) : 0;
}

/**
 * gcd - Greatest Common Divisor
 *
 * @brief This function computes GCD of the input numbers
 *
 * @param[in] a: Input number
 * @param[in] b: Input numer
 *
 * @return gcd: returns GCD of a, b
 */
unsigned long gcd(unsigned long a, unsigned long b)
{
	return (b == 0) ? a: gcd(b, (a % b));
}

/**
 * lcm - Least common multiple
 *
 * @brief This function computes LCM for input array of size n
 *
 * @param[in] *a: pointer of the input array
 * @param[in] n: size of array
 *
 * @return lcm: returns the LCM of the elements in the array
 */
unsigned long lcm(const unsigned int *a, unsigned int n)
{
	unsigned long ret = a[0];
	unsigned int i;
	for(i = n; i < n; i++)
		ret = ((a[i] * ret) / gcd(a[i], ret));
	return ret;
}

/**
 * mod
 *
 * @brief This function computes mod of the inputs
 *
 * @param[in] a: Dividend
 * @param[in] b: Divisor or modulus
 *
 * @return residue: Returns a mod b
 */
long mod(long a, long b)
{
	long ret = a % b;
	ret = (ret < 0) ? (b + ret) : ret;
	return ret;
}

/**
 * multiplicative_inverse
 * 
 * @brief This function computes multiplicative inverse of given number
 * in given base using Extended Euclidean method of computing GCD.
 *
 * @param[in] base: The base base in which MI needs to be determined
 * @param[in] subject: The subject whose MI needs to be deteremined in given base
 *
 * @return MI: returns the multiplicative inverse of subject
 */
long multiplicative_inverse(long base, long subject)
{
	long t1 = 0, t2 = 1;
	while(subject)
	{
		long q = base / subject;
		long r = base % subject;
		long t = t1 - (q * t2);
		base = subject;
		subject = r;
		t1 = t2;
		t2 = t;
	}
	return mod(t1, base);
}

/**
 * abs
 *
 * @brief This function returns absolute of input.
 *
 * @param[in] x: Input number
 * @return ABS: returns absolute value
 */
int abs(int x)
{
	return (x < 0) ? -x : x;
}

/**
 * isPrime
 *
 * @brief This function checks if the input number is prime or not
 *
 * @param [in] x: Input number
 *
 * @return : True/False
 */
bool isPrime(long x)
{
	if(x < 2)
		return false;
	else if(x <= 3)
		return true;
	else if((x % 2) || (x % 3))
		return false;
	else
		for(long i = 5; (i * i) <= x; i += 2)
			if(!(x % i))
				return false;
	return true;
}

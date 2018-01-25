// SHA-1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <string>

#if BYTE_ORDER == LITTLE_ENDIAN

#endif

int leftRotate(unsigned variable, unsigned numberOfBits) {

	return (variable << numberOfBits) | (variable >> (32 - numberOfBits));

}

int main()
{

	std::string message;
	std::getline(std::cin, message);
	std::cout << message << std::endl;

	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;
	uint32_t h4 = 0xC3D2E1F0;

	uint64_t ml;

	//Pre-processing

	//append message

	//split into 512 bit chunks
	uint32_t word[80];
	uint32_t * wordPointer;

	wordPointer = word;

	for (int i = 0; i < 16; i++) {
		*wordPointer = 1;
		wordPointer++;

	}


	//Extend 16 32-bit words into 80 32-bit words

	for (int i = 16; i < 80; i++) {
		std::cout << i << std::endl;

		uint32_t temp = (word[i - 3] ^ word[i - 8] ^ word[i - 14] ^ word[i - 16]);
		uint32_t * tempPtr = &temp;

		uint32_t temp2 = leftRotate(*tempPtr, 1);

		word[i] = temp2;

		std::cout << temp << std::endl;
		std::cout << temp2 << std::endl;

	}

	//initialize hash value for this chunk
	uint32_t a = h0;
	uint32_t * aPtr = &a;
	uint32_t b = h1;
	uint32_t * bPtr = &b;
	uint32_t c = h2;
	uint32_t d = h3;
	uint32_t e = h4;
	uint32_t f;
	uint32_t k;

	//Main loop
	for (int i = 0; i < 80; i++) {

		if (0 <= i <= 19) {

			f = (b&c) | ((~b)&d);
			k = 0x5A827999;

		}
		else if (20 <= i <= 39) {

			f = b ^ c ^ d;
			k = 0x6ED9EBA1;

		}
		else if (40 <= i <= 59) {

			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;

		}
		else if (60 <= i <= 79) {

			f = b ^ c ^ d;
			k = 0xCA62C1D6;

		}

		int temp = leftRotate(*aPtr,5)+f + e + k + word[i];
		e = d;
		d = c;
		c = leftRotate(*bPtr,30);
		b = a;
		a = temp;

	}

	//Add this chunk's hash to the result so far:


	h0 = h0 + a;
	h1 = h1 + b;
	h2 = h2 + c;
	h3 = h3 + d;
	h4 = h4 + e;

	//Produce final hash value (big-endian) as a 160-bit number

	unsigned int hh = (h0 << 128) | (h1<< 96) | (h2 << 64) | (h3 << 32) | (h4);
	
	std::getchar();


	return 0;
}
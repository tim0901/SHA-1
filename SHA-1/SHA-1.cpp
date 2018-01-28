// SHA-1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <bitset>

#if BYTE_ORDER == LITTLE_ENDIAN

#endif

template <typename ValueType>
inline void print64(const char* name, ValueType value)
{
	std::cout << name << "= " << std::bitset <64>(value).to_string() << " = " << (long long)value << std::endl;
}

template <typename ValueType>
inline void print160(const char* name, ValueType value)
{
	std::cout << name << "= " << std::bitset <160>(value).to_string() << " = " << std::endl;
}


int leftRotate(unsigned variable, unsigned numberOfBits) {

	return (variable << numberOfBits) | (variable >> (32 - numberOfBits));

}
int bigEndian(uint32_t variable) {

	//Converts integer to be big-endian 

	uint32_t output = 0;
	uint32_t bE0, bE1, bE2, bE3;

	bE0 = (variable & 0xff) << 24; //least to most
	bE1 = (variable & 0xff00) << 8; //2nd least to 2nd most
	bE2 = (variable & 0xff0000) >> 8; //2nd most to 2nd least
	bE3 = (variable & 0xff000000) >> 24; //most to least

	output = (bE0 | bE1 | bE2 | bE3);

	return output;

}

std::string hash() {
	
	//Initialize variables

	uint64_t h0 = 0x67452301;
	uint64_t h1 = 0xEFCDAB89;
	uint64_t h2 = 0x98BADCFE;
	uint64_t h3 = 0x10325476;
	uint64_t h4 = 0xC3D2E1F0;

	uint64_t ml;

	//Pre-processing

	//append message

	//split into 512 bit chunks
	uint32_t word[80];
	uint32_t * wordPointer;
	wordPointer = word;

	//Break each chunk into 16 32-bit big-endian words

	for (int i = 0; i < 16; i++) {

		//sets the array wordPointer to all equal 1
		*wordPointer = 1;
		wordPointer++;

		*wordPointer = bigEndian(*wordPointer);

	}


	//Extend 16 32-bit words into 80 32-bit words

	for (int i = 16; i < 80; i++) {

		uint32_t temp = (word[i - 3] ^ word[i - 8] ^ word[i - 14] ^ word[i - 16]);
		uint32_t * tempPtr = &temp;

		uint32_t temp2 = leftRotate(*tempPtr, 1);

		word[i] = temp2;

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

		int temp = leftRotate(*aPtr, 5) + f + e + k + word[i];
		e = d;
		d = c;
		c = leftRotate(*bPtr, 30);
		b = a;
		a = temp;

	}

	//Add this chunk's hash to the result so far:


	h0 = h0 + a;
	h1 = h1 + b;
	h2 = h2 + c;
	h3 = h3 + d;
	h4 = h4 + e;


	print64("h0", h0 >> 0);


	std::cout << "h0: " << h0 << " h1: " << h1 << " h2: " << h2 << " h3: " << h3 << " h4: " << h4 << std::endl;

	//Produce final hash value (big-endian) as a 160-bit number

	uint64_t hh1;
	unsigned long long hh2;

	//uint64_t test = hash(message);



	std::bitset <160> hh;
	std::bitset <160> shiftingTemp;

	shiftingTemp = h0;
	hh = shiftingTemp << 128;
	shiftingTemp = h1;
	hh = hh |= shiftingTemp << 96;
	shiftingTemp = h2;
	hh = hh |= shiftingTemp << 64;
	shiftingTemp = h3;
	hh = hh |= shiftingTemp << 32;
	shiftingTemp = h4;
	hh = hh |= shiftingTemp;

	//hh = ((h0 << 128) | (h1 << 96) | (h2 << 64) | (h3 << 32) | (h4));

	//std::cout << "Digest in bits: " << hh.to_string() << std::endl;

	std::bitset<32> digestTemp;
	std::stringstream digestStream;

	for (int j = 4; j > (-1); j--) {

		for (int i = 31; i > (-1); i--) {
			
			digestTemp[i] = hh[j * 32 + i];

		}
		
		digestStream << std::hex << digestTemp.to_ulong();
		
	}

	std::string digest = digestStream.str();

	return digest;
}

int main()
{

	std::string message;

	//take console input

	std::getline(std::cin, message);

	std::cout << hash() << std::endl;

	std::getchar();


	return 0;
}
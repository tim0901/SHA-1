// SHA-1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>

//Global Variables - are initialized at the start of the hash
uint32_t h0;
uint32_t h1;
uint32_t h2;
uint32_t h3;
uint32_t h4;
int chunkNumber;

//Debug Tools to output the bitset of integers
template <typename ValueType>
inline void print64(const char* name, ValueType value){

	std::cout << name << "= " << std::bitset <64>(value).to_string() << " = " << (long long)value << std::endl;
}

template <typename ValueType>
inline void print160(const char* name, ValueType value){

	std::cout << name << "= " << std::bitset <160>(value).to_string() << " = " << std::endl;
}

//Shifts input left by numberOfBits
int leftRotate(unsigned variable, unsigned numberOfBits) {

	return (variable << numberOfBits) | (variable >> (32 - numberOfBits));
}

int bigEndian(uint32_t variable) {

	//Converts integer to be big-endian 
	uint32_t output = 0;
	uint32_t bE0, bE1, bE2, bE3 = 0;

	bE0 = (variable & 0xff) << 24; //least to most
	bE1 = (variable & 0xff00) << 8; //2nd least to 2nd most
	bE2 = (variable & 0xff0000) >> 8; //2nd most to 2nd least
	bE3 = (variable & 0xff000000) >> 24; //most to least

	output = (bE0 | bE1 | bE2 | bE3);

	return output;

}

int hash(std::bitset<512> chunk) {

	//Initialize variables
	
	//Iterates the chunk number
	chunkNumber++;

	std::bitset<32> wordBits = 0;

	uint32_t word[80];

	//Break each chunk into 16 32-bit big-endian words

	for (int i = 0; i < 16; i++) {

		wordBits.reset();

		for (int j = 0; j < 32; j++)
		{
			wordBits[31 - j] = chunk[511 - (i * 32 + j)];
			//std::cout << "wordBits " << j << " (" << (i*32) + j << "): " << wordBits.to_string() << std::endl;
		}

		word[i] = wordBits.to_ulong();

		//std::cout << "word "<< i << ": " << word[i] << std::endl;

	}

	//Extend 16 32-bit words into 80 32-bit words

	for (int i = 16; i < 80; i++) {

		uint32_t temp = (word[i - 3] ^ word[i - 8] ^ word[i - 14] ^ word[i - 16]);
		uint32_t * tempPtr = &temp;

		uint32_t temp2 = leftRotate(*tempPtr, 1);

		word[i] = temp2;

		//std::cout << "word " << i << ": " << word[i] << std::endl;

	}

	//Initialize hash value for this chunk
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
	for (int i = 0; i <= 79; i++) {

		if (0 <= i && i <= 19) {

			f = (b&c) | ((~b)&d);
			k = 0x5A827999;

		}
		else if (20 <= i && i <= 39) {

			f = b ^ c ^ d;
			k = 1859775393;

		}
		else if (40 <= i && i <= 59) {

			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;

		}
		else if (60 <= i && i <= 79) {

			f = b ^ c ^ d;
			k = 0xCA62C1D6;

		}

		int temp = leftRotate(*aPtr, 5) + f + e + k + word[i];

		e = d;
		d = c;
		c = leftRotate(*bPtr, 30);
		b = a;
		a = temp;

		//Outputs a-f after each loop of the hash
		//std::cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << " E: " << e << " after loop " << i << std::endl;

	}

	//Add this chunk's hash to the result so far:
	h0 = h0 + a;
	h1 = h1 + b;
	h2 = h2 + c;
	h3 = h3 + d;
	h4 = h4 + e;

	//Outputs values of the internal variables after each chunk has been added to the hash
	//std::cout << "h0: " << h0 << " h1: " << h1 << " h2: " << h2 << " h3: " << h3 << " h4: " << h4 << " after chunk " << chunkNumber << std::endl << std::endl;

	return 0;
}

std::string digest(){

	//Produce final hash value (big-endian) as a 160-bit number

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

	//Converts digest bitset into a string for export
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

std::string fileHash(std::string path) {

	//Initialise Variables

	//Resets to default values
	h0 = 0x67452301;
	h1 = 0xEFCDAB89;
	h2 = 0x98BADCFE;
	h3 = 0x10325476;
	h4 = 0xC3D2E1F0;
	chunkNumber = 0; //chunk number

	uint64_t ml = 0; //message length

	//Pre-processing

	//File name
	std::string fileName = path;

	//Open file
	std::ifstream fileInput;
	fileInput.open(fileName, std::ios::binary);

	std::bitset<512> messageLength;

	//Split into 512 bit chunks
	std::bitset<512> chunk = 0;
	const int bufferSize = 64;
	std::vector<char> buffer(bufferSize + 1, 0);

	//Validates file
	if (fileInput.good())
	{
		//Outputs if file exists
		//std::cout << fileName << " exists!" << std::endl << std::endl;

		while (1)
		{
			fileInput.read(buffer.data(), bufferSize);
			std::streamsize size = fileInput.gcount();

			//Outputs data in this chunk
			//std::cout << "Data read is: " << buffer.data() << std::endl << std::endl;
			
			buffer[size] = 0;
			//append message
			std::string output(buffer.begin(), buffer.end());
			chunk.reset();
			for (int i = 0; i < 64; i++)
			{
				chunk = chunk << 8;
				chunk = chunk |= std::bitset<512>(output.c_str()[i]);
			}

			if (!fileInput)
			{
				//This bit deals with the final chunk of the file

				//Confirms total size of the chunk
				//std::cout << "Last of file read. " << size << " characters read successfully." << std::endl << std::endl;
				chunk[511 - (size * 8)] = 1;

				//Appends chunk with 0s and the total length of the message. 
				if ((512 - size * 8) < 64)
				{
					for (int i = 0; i < 64; i++) {
						chunk[i] = 0;
					}
				}
				else {

					for (int i = 0; ((size * 8) + 2 + i) <= 511; i++) {
						chunk[511 - (size * 8) - 2 - i] = 0;
					}

					ml = chunkNumber * 512 + size * 8;

					messageLength = ml;
					chunk = chunk |= messageLength;
				}

				//Prints the chunk's bitset
				//std::cout << "appended chunk: " << chunk.to_string() << std::endl << std::endl;
				
				//Sends to the hash function
				hash(chunk);
			}
			else
			{
				//Prints the chunk's bitset
				//std::cout << "chunk " << chunkNumber + 1 << ": " << chunk.to_string() << std::endl << std::endl;

				//Sends to the hash function
				hash(chunk);

			}
			if (!fileInput) break;
		}
	}
	else
	{
		std::cout << "Invalid file" << std::endl;
		std::string badFile;
		return badFile;
	}
	fileInput.close();

	return digest();
}

int main(){
	std::string path;

	//take console input
	while (1){

		//Take input as a path to the file. Must be in directory of executable.
		std::getline(std::cin, path);

		//Hash file at the file provided, stores digest in 'output'
		std::string output = fileHash(path);
		
		//Outputs the 'output' string, but only if file was valid
		if (output != "") 
		{
			std::cout << "Digest: " << output << std::endl;
		}
	}

	return 0;
}
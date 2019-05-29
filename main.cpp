#include <cstdint>
#include <iostream>
#include <bitset>

#define BUFFER 1024
#define BITS_IN_BYTE 8

// Constants used by MD5
uint32_t DigestBlock[5] = {
	0x67452301,
	0xEFCDAB89,
	0x98BADCFE,
	0x10325476,
	0xC3D2E1F0
};

uint32_t k[4] = {
	0x5A827999,
	0x6ED9EBA1,
	0x8F1BBCDC,
	0xCA62C1D6
};

template<typename T>
void printInBinary(const T& a)
{
	const char* begin = reinterpret_cast<const char*>(&a);
	const char* end = begin + sizeof(a);

	while (begin != end)
		std::cout << std::bitset<CHAR_BIT>(*begin++) << ' ';

	std::cout << '\t';

}

uint32_t leftRotate(uint8_t numShifts, uint32_t value)
{
	uint32_t result = (value << numShifts) | (value >> (32 - numShifts));
	return result;
}

void showMessageDigest()
{
	std::cout << "h0: "; printInBinary(DigestBlock[0]);
	std::cout << std::endl;
	std::cout << "h1: "; printInBinary(DigestBlock[1]);
	std::cout << std::endl;
	std::cout << "h2: "; printInBinary(DigestBlock[2]);
	std::cout << std::endl;
	std::cout << "h3: "; printInBinary(DigestBlock[3]);
	std::cout << std::endl;
	std::cout << "h4: "; printInBinary(DigestBlock[4]);
	std::cout << std::endl;
}

void rounds(uint32_t* MDigest, uint32_t W, uint32_t K, int roundNumber)
{
	uint32_t temp;
	std::cout << std::endl;
	if (roundNumber < 20)
	{
		temp = leftRotate(5, MDigest[0]) +
			((MDigest[1] & MDigest[2]) | ((~MDigest[1]) & MDigest[3])) + MDigest[4] + W + K;
		MDigest[4] = MDigest[3];
		MDigest[3] = MDigest[2];
		MDigest[2] = leftRotate(30, MDigest[1]);
		MDigest[1] = MDigest[0];
		MDigest[0] = temp;
		std::cout << "Round: " << roundNumber << std::endl;
		showMessageDigest();
	}
	else if (roundNumber >= 20 && roundNumber < 40)
	{
		temp = leftRotate(5, MDigest[0]) + (MDigest[1] ^ MDigest[2] ^ MDigest[3]) + MDigest[4] + W + K;
		MDigest[4] = MDigest[3];
		MDigest[3] = MDigest[2];
		MDigest[2] = leftRotate(30, MDigest[1]);
		MDigest[1] = MDigest[0];
		MDigest[0] = temp;
		std::cout << "Round: " << roundNumber << std::endl;
		showMessageDigest();
	}
	else if (roundNumber >= 40 && roundNumber < 60)
	{
		temp = leftRotate(5, MDigest[0]) +
			((MDigest[1] & MDigest[2]) | (MDigest[1] & MDigest[3]) | (MDigest[2] & MDigest[3])) + MDigest[4] + W + K;
		MDigest[4] = MDigest[3];
		MDigest[3] = MDigest[2];
		MDigest[2] = leftRotate(30, MDigest[1]);
		MDigest[1] = MDigest[0];
		MDigest[0] = temp;
		std::cout << "Round: " << roundNumber << std::endl;
		showMessageDigest();
	}
	else if (roundNumber >= 60 && roundNumber < 80)
	{
		temp = leftRotate(5, MDigest[0]) + (MDigest[1] ^ MDigest[2] ^ MDigest[3]) + MDigest[4] + W + K;
		MDigest[4] = MDigest[3];
		MDigest[3] = MDigest[2];
		MDigest[2] = leftRotate(30, MDigest[1]);
		MDigest[1] = MDigest[0];
		MDigest[0] = temp;
		std::cout << "Round: " << roundNumber << std::endl;
		showMessageDigest();
	}
}
int main()
{
	unsigned char msg[BUFFER]; // = "The quick fox jumps over the lazy dog.";
	fflush(stdin);
	std::cout << "Put a message (press enter to finish): ";
	char c = getchar();
	uint64_t count = 0;
	while (c != '\n') {
		msg[count++] = c;
		c = getchar();
	}
	msg[count] = 0;
	const uint64_t msgLen = strlen((char*)msg);
	msg[msgLen] = '\x80';
	uint64_t newLen = msgLen + 1;

	// The same as for MD5 the new length should be equal to 448 Mod 512
	while (newLen % (512 / 8) != (448 / 8))
		newLen++;

	const uint64_t diff = newLen - msgLen;
	for (uint64_t i = 0; i < diff; i++)
	{
		msg[i + (uint64_t)1 + msgLen] = '\x00';
	}

	// we append the 64 bits string len
	const uint64_t messageLengthInBits = msgLen * BITS_IN_BYTE;
	msg[56] = (messageLengthInBits & 0xff00000000000000) >> 56;
	msg[57] = (messageLengthInBits & 0x00ff000000000000) >> 48;
	msg[58] = (messageLengthInBits & 0x0000ff0000000000) >> 40;
	msg[59] = (messageLengthInBits & 0x000000ff00000000) >> 32;

	msg[60] = (messageLengthInBits & 0x00000000ff000000) >> 24;
	msg[61] = (messageLengthInBits & 0x0000000000ff0000) >> 16;
	msg[62] = (messageLengthInBits & 0x000000000000ff00) >> 8;
	msg[63] = (messageLengthInBits & 0x00000000000000ff) >> 0;

	newLen += 8; // Update the newLen which is 8 bytes longer ( we just appended 8 bytes as messageLengthInBits)

	for (size_t i = 0; i < newLen; i++)
		printInBinary(msg[i]);


	uint64_t offset = 0;
	while (offset < newLen)
	{
		uint8_t temp[64];
		for (int i = 0; i < 64; i++)
		{
			temp[i] = msg[offset + i];
		}

		// Process block of 64 DWORD-S which is 512 bytes ( 64 * 8(each DWORD is 8 bytes long))

		uint32_t W[80];
		uint32_t
			A = DigestBlock[0],
			B = DigestBlock[1],
			C = DigestBlock[2],
			D = DigestBlock[3],
			E = DigestBlock[4];

		uint8_t t;
		for (t = 0; t < 16; t++)
		{
			W[t] = msg[offset + 4 * t + 0] << 24;
			W[t] |= msg[offset + 4 * t + 1] << 16;
			W[t] |= msg[offset + 4 * t + 2] << 8;
			W[t] |= msg[offset + 4 * t + 3] << 0;
		}

		for (t = 16; t < 80; t++)
		{
			W[t] = leftRotate(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
		}

		// 80 rounds

		for (t = 0; t < 20; t++)
		{
			rounds(DigestBlock, W[t], k[0], t);
		}

		for (t = 20; t < 40; t++)
		{
			rounds(DigestBlock, W[t], k[1], t);
		}

		for (t = 40; t < 60; t++)
		{
			rounds(DigestBlock, W[t], k[2], t);
		}

		for (t = 60; t < 80; t++)
		{
			rounds(DigestBlock, W[t], k[3], t);
		}

		//final addition
		DigestBlock[0] += A;
		DigestBlock[1] += B;
		DigestBlock[2] += C;
		DigestBlock[3] += D;
		DigestBlock[4] += E;
		std::cout << "Message Digest: " << std::endl;
		showMessageDigest();
		offset += 64;
	}

	std::cout << "Final Message Digest: " << std::endl;
	showMessageDigest();
	std::cout << "In hex: " << std::endl;
	for (int i = 0; i < 5; i++)
		std::cout << std::hex << DigestBlock[i];
	std::cout << std::endl;

	getc(stdin);
	return 0;
}
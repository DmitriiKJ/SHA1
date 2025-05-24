#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <iostream>
#include <vector>
#include <gtest/gtest.h>
using namespace std;

unsigned int circular_left_shift(unsigned int value, int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

string uint_to_hex(unsigned int value) 
{
	string res = "";

	for (int i = sizeof(int) * 2 - 1; i >= 0; i--) {
		unsigned int symbol = ((value >> i * 4) & 0xf);
		if (symbol < 10) {
			res += char(symbol + 48);
		}
		else {
			res += char(symbol + 55);
		}
	}

	return res;
}

string SHA_1(vector<char> message)
{
	unsigned int h0 = 0x67452301;
	unsigned int h1 = 0xEFCDAB89;
	unsigned int h2 = 0x98BADCFE;
	unsigned int h3 = 0x10325476;
	unsigned int h4 = 0xC3D2E1F0;

	unsigned long long length_before_padding = message.size() * 8;
	// Add 1 bit to the end of the message and pad with 0s until the length is 448 mod 512
	message.push_back((char)0x80);
	while (message.size() % 64 != 56)
	{
		message.push_back((char)0);
	}
	long length = message.size() * 8;

	// Add message size in bits to the end of the message
	for (int i = 7; i >= 0; i--)
	{
		message.push_back((char)((length_before_padding >> (8 * i) & 0xFF)));
	}

	// Divide the message into 512-bit blocks
	for (int i = 0; i < message.size() / (512 / 8); i++)
	{
		// Get 16 32-bit words from the 512-bit block
		vector<char> tmp = vector<char>(message.begin() + i * (512 / 8), message.begin() + (i + 1) * (512 / 8));
		vector<unsigned int> M;
		for (int j = 0; j <= 15; j++)
		{
			unsigned int value = 0;
			for (int k = 0; k < 4; k++) {
				if (j * 4 + k < tmp.size())
				{
					value = (value << 8) | (unsigned char)tmp[j * 4 + k];
				}
			}
			M.push_back(value);
		}

		// Extend the 16 32-bit words into 80 32-bit words
		vector<unsigned int> W = M;
		for (int j = 16; j <= 79; j++)
		{
			W.push_back(circular_left_shift(W[j - 3] ^ W[j - 8] ^ W[j - 14] ^ W[j - 16], 1));
		}

		auto a = h0;
		auto b = h1;
		auto c = h2;
		auto d = h3;
		auto e = h4;

		unsigned int f;
		unsigned int k;

		for (int j = 0; j <= 79; j++)
		{
			if (j >= 0 && j <= 19)
			{
				f = (b & c) | ((~b) & d);
				k = 0x5A827999;
			}
			else if (j >= 20 && j <= 39)
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			else if (j >= 40 && j <= 59)
			{
				f = (b & c) | (b & d) | (c & d);
				k = 0x8F1BBCDC;
			}
			else if (j >= 60 && j <= 79)
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}

			unsigned int temp = circular_left_shift(a, 5) + f + e + k + W[j];
			e = d;
			d = c;
			c = circular_left_shift(b, 30);
			b = a;
			a = temp;
		}

		h0 = h0 + a;
		h1 = h1 + b;
		h2 = h2 + c;
		h3 = h3 + d;
		h4 = h4 + e;
	}

	// Append the hash values to the result
	return uint_to_hex(h0) + uint_to_hex(h1) + uint_to_hex(h2) + uint_to_hex(h3)+ uint_to_hex(h4);
}

TEST(SHA1Test, TestVariousMessages) {
	string input = "MessageMySha1";
	string result = SHA_1(vector<char>(input.begin(), input.end()));
	EXPECT_EQ(result, "B4A451D43137596C92E4D913AD2F5745813B9FD9");

	input = "Very loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong message, to check if long messages are fine with this alghoritm";
	result = SHA_1(vector<char>(input.begin(), input.end()));
	EXPECT_EQ(result, "38D0836D51210A6C2396474E8F1C7E3EE19A1207");

	input = "Hello, world!";
	result = SHA_1(vector<char>(input.begin(), input.end()));
	EXPECT_EQ(result, "943A702D06F34599AEE1F8DA8EF9F7296031D699");
}

int main(int argc, char** argv)
{
	int way = 0;
	bool finish = false;
	while (!finish)
	{
		do {
			cout << "Options:\n1 - get hash from message\n2 - start tests\n3 - exit\nChoose option: ";
			cin >> way;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		} while (way != 1 && way != 2 && way != 3);

		switch (way)
		{
			case 1:
			{
				string input;
				cout << "Enter message: ";
				getline(cin, input);
				string result = SHA_1(vector<char>(input.begin(), input.end()));
				cout << "==================================================" << endl;
				cout << "Hash: " << result << endl;
				cout << "==================================================" << endl;
				break;
			}
			case 2:
			{
				::testing::InitGoogleTest(&argc, argv);
				return RUN_ALL_TESTS();
			}
			case 3:
			{
				cout << "Exit" << endl;
				finish = true;
				break;
			}
			default:
				break;
		}
	}
}

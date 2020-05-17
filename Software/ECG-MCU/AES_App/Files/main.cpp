#include <iostream>
#include <conio.h>
#include "boxes.h"

using namespace std;

void KeyExpansionCore(unsigned char* in, unsigned char i) 
{
	//Rotate Left
	unsigned int* q = (unsigned int*)in;
	*q = (*q >> 8) | ((*q & 0xff) << 24);

	//S-BOX 4Bytes
	in[0] = s_box[in[0]]; 
	in[1] = s_box[in[1]];
	in[2] = s_box[in[2]];
	in[3] = s_box[in[3]];

	//RCon
	in[0] ^= rcon[i];
}
void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys) 
{
	for (int i = 0; i < 16; i++)
		expandedKeys[i] = inputKey[i];

	int bytesGenerated = 16;
	int rconIteration = 1;
	unsigned char temp[4];

	while (bytesGenerated < 176)
	{
		for (int i = 0; i < 4; i++)
			temp[i] = expandedKeys[i + bytesGenerated - 4];

		if (bytesGenerated % 16 == 0)
		{
			KeyExpansionCore(temp, rconIteration);
			rconIteration++;
		}
			
		for (unsigned char a = 0; a < 4; a++)
		{
			expandedKeys[bytesGenerated]= expandedKeys[bytesGenerated - 16] ^ temp[a];
			bytesGenerated++;
		}
	}
}
void SubBytes(unsigned char* state) 
{
	for (int i = 0; i < 16; i++)
		state[i] = s_box[state[i]];
}
void inv_sub_bytes(unsigned char* state) {
	// Substitute each state value with another byte in the Rijndael S-Box
	for (int i = 0; i < 16; i++)
		state[i] = inv_s_box[state[i]];
}
void ShiftRows(unsigned char* state) 
{
	unsigned char tmp[16];

	tmp[0] = state[0];
	tmp[1] = state[5];
	tmp[2] = state[10];
	tmp[3] = state[15]; 

	tmp[4] = state[4];
	tmp[5] = state[9];
	tmp[6] = state[14];
	tmp[7] = state[3];

	tmp[8] = state[8];
	tmp[9] = state[13];
	tmp[10] = state[2];
	tmp[11] = state[7];

	tmp[12] = state[12];
	tmp[13] = state[1];
	tmp[14] = state[6];
	tmp[15] = state[11];

	for (int i = 0; i < 16; i++)
		state[i] = tmp[i];
}
void inv_shift_rows(unsigned char* state) {
	unsigned char tmp[16];

	// First row don't shift (idx = idx)
	tmp[0] = state[0];
	tmp[4] = state[4];
	tmp[8] = state[8];
	tmp[12] = state[12];

	// Second row shift right once (idx = (idx - 4) % 16)
	tmp[1] = state[13];
	tmp[5] = state[1];
	tmp[9] = state[5];
	tmp[13] = state[9];

	// Third row shift right twice (idx = (idx +/- 8) % 16)
	tmp[2] = state[10];
	tmp[6] = state[14];
	tmp[10] = state[2];
	tmp[14] = state[6];

	// Fourth row shift right three times (idx = (idx + 4) % 16)
	tmp[3] = state[7];
	tmp[7] = state[11];
	tmp[11] = state[15];
	tmp[15] = state[3];

	for (int i = 0; i < 16; i++)
		state[i] = tmp[i];
}
void MixColumns(unsigned char* state) 
{
	unsigned char tmp[16];

	tmp[0] = (unsigned char)(mul2[state[0]] ^ mul3[state[1]] ^ state[2] ^ state[3]);
	tmp[1] = (unsigned char)(state[0] ^ mul2[state[1]] ^ mul3[state[2]] ^ state[3]);
	tmp[2] = (unsigned char)(state[0] ^ state[1] ^ mul2[state[2]] ^ mul3[state[3]]);
	tmp[3] = (unsigned char)(mul3[state[0]] ^ state[1] ^ state[2] ^ mul2[state[3]]);

	tmp[4] = (unsigned char)(mul2[state[4]] ^ mul3[state[5]] ^ state[6] ^ state[7]);
	tmp[5] = (unsigned char)(state[4] ^ mul2[state[5]] ^ mul3[state[6]] ^ state[7]);
	tmp[6] = (unsigned char)(state[4] ^ state[5] ^ mul2[state[6]] ^ mul3[state[7]]);
	tmp[7] = (unsigned char)(mul3[state[4]] ^ state[5] ^ state[6] ^ mul2[state[7]]);

	tmp[8] = (unsigned char)(mul2[state[8]] ^ mul3[state[9]] ^ state[10] ^ state[11]);
	tmp[9] = (unsigned char)(state[8] ^ mul2[state[9]] ^ mul3[state[10]] ^ state[11]);
	tmp[10] = (unsigned char)(state[8] ^ state[9] ^ mul2[state[10]] ^ mul3[state[11]]);
	tmp[11] = (unsigned char)(mul3[state[8]] ^ state[9] ^ state[10] ^ mul2[state[11]]);

	tmp[12] = (unsigned char)(mul2[state[12]] ^ mul3[state[13]] ^ state[14] ^ state[15]);
	tmp[13] = (unsigned char)(state[12] ^ mul2[state[13]] ^ mul3[state[14]] ^ state[15]);
	tmp[14] = (unsigned char)(state[12] ^ state[13] ^ mul2[state[14]] ^ mul3[state[15]]);
	tmp[15] = (unsigned char)(mul3[state[12]] ^ state[13] ^ state[14] ^ mul2[state[15]]);

	for (int i = 0; i<16; i++)
		state[i] = tmp[i];

}
void inv_mix_columns(unsigned char* state) {
	unsigned char tmp[16];

	// Column 1
	tmp[0] = (unsigned char)(mul14[state[0]] ^ mul11[state[1]] ^ mul13[state[2]] ^ mul9[state[3]]);
	tmp[1] = (unsigned char)(mul9[state[0]] ^ mul14[state[1]] ^ mul11[state[2]] ^ mul13[state[3]]);
	tmp[2] = (unsigned char)(mul13[state[0]] ^ mul9[state[1]] ^ mul14[state[2]] ^ mul11[state[3]]);
	tmp[3] = (unsigned char)(mul11[state[0]] ^ mul13[state[1]] ^ mul9[state[2]] ^ mul14[state[3]]);

	// Column 2
	tmp[4] = (unsigned char)(mul14[state[4]] ^ mul11[state[5]] ^ mul13[state[6]] ^ mul9[state[7]]);
	tmp[5] = (unsigned char)(mul9[state[4]] ^ mul14[state[5]] ^ mul11[state[6]] ^ mul13[state[7]]);
	tmp[6] = (unsigned char)(mul13[state[4]] ^ mul9[state[5]] ^ mul14[state[6]] ^ mul11[state[7]]);
	tmp[7] = (unsigned char)(mul11[state[4]] ^ mul13[state[5]] ^ mul9[state[6]] ^ mul14[state[7]]);

	// Column 3
	tmp[8] = (unsigned char)(mul14[state[8]] ^ mul11[state[9]] ^ mul13[state[10]] ^ mul9[state[11]]);
	tmp[9] = (unsigned char)(mul9[state[8]] ^ mul14[state[9]] ^ mul11[state[10]] ^ mul13[state[11]]);
	tmp[10] = (unsigned char)(mul13[state[8]] ^ mul9[state[9]] ^ mul14[state[10]] ^ mul11[state[11]]);
	tmp[11] = (unsigned char)(mul11[state[8]] ^ mul13[state[9]] ^ mul9[state[10]] ^ mul14[state[11]]);

	// Column 4
	tmp[12] = (unsigned char)(mul14[state[12]] ^ mul11[state[13]] ^ mul13[state[14]] ^ mul9[state[15]]);
	tmp[13] = (unsigned char)(mul9[state[12]] ^ mul14[state[13]] ^ mul11[state[14]] ^ mul13[state[15]]);
	tmp[14] = (unsigned char)(mul13[state[12]] ^ mul9[state[13]] ^ mul14[state[14]] ^ mul11[state[15]]);
	tmp[15] = (unsigned char)(mul11[state[12]] ^ mul13[state[13]] ^ mul9[state[14]] ^ mul14[state[15]]);

	for (int i = 0; i < 16; i++)
		state[i] = tmp[i];
}
void AddRoundKey(unsigned char* state, unsigned char* roundKey) 
{
	for (int i = 0; i < 16; i++)
		state[i] ^= roundKey[i];
}
void AES_Encrypt(unsigned char* message, unsigned char* key)
{
	unsigned char state[16];
	for (int i = 0; i < 16; i++)
		state[i] = message[i];

	int numberOfRounds = 9;
	unsigned char expandedKey[176];
	//Initial Round
	KeyExpansion(key,expandedKey);
	AddRoundKey(state,key); 

	//Common Rounds
	for (int i = 0; i < numberOfRounds; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state,expandedKey+(16*(i+1)));
	}
	//Final Round
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state,expandedKey+160);

	for (int i = 0; i < 16; i++)
		message[i] = state[i];
}
void aes_decrypt(unsigned char* message, unsigned char* key) {
	
	unsigned char state[16];
	for (int i = 0; i < 16; i++)
		state[i] = message[i];

	const int round_cnt = 9;
	unsigned char expandedKey[176];
	//Initial Round
	KeyExpansion(key, expandedKey);
	AddRoundKey(state, expandedKey + 160);

	for (int i = round_cnt; i > 0; i--) {
		inv_shift_rows(state);
		inv_sub_bytes(state);
		AddRoundKey(state, expandedKey + (16 * i));
		inv_mix_columns(state);
	}
	inv_shift_rows(state);
	inv_sub_bytes(state);
	AddRoundKey(state, expandedKey);

	for (int i = 0; i < 16; i++)
		message[i] = state[i];
}
void printHex(unsigned char x)
{
	if (x / 16 < 10) cout << (char )((x / 16) + '0');
	if (x / 16 >= 10) cout << (char)((x / 16-10) + 'A');

	if (x % 16 < 10) cout << (char)((x % 16) + '0');
	if (x % 16 >= 10) cout << (char)((x % 16 - 10) + 'A');
}
int main()
{
	unsigned char message[] = "Universidad Santo Tomas encriptada con AES para la tesis de diseno y implementacion de un dispositivo de senales electrocardiograficas";
	cout <<"Mensaje: "<< message<<endl;
	unsigned char key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

	int originalLen = strlen((const char*)message);
	int lenOfPaddedMessage = originalLen;

	if (lenOfPaddedMessage % 16 != 0)
		lenOfPaddedMessage = (lenOfPaddedMessage / 16 + 1) * 16;

	unsigned char* paddedMessage = new unsigned char[lenOfPaddedMessage];
	for (int i = 0; i < lenOfPaddedMessage; i++)
	{
		if (i >= originalLen)paddedMessage[i] = 0;
		else paddedMessage[i] = message[i];
	}

	for(int i=0;i<lenOfPaddedMessage;i+=16)
		AES_Encrypt(paddedMessage+i,key);
	 
	cout << "Encripted Message" << endl;
	for (int i = 0; i < lenOfPaddedMessage; i++)
	{
		printHex(paddedMessage[i]);
		cout << " ";
	}
	_getch();
	for (int i = 0; i < lenOfPaddedMessage; i += 16)
		aes_decrypt(paddedMessage+i, key);
	
	cout << endl << "Decripted Message" << endl;
	cout << paddedMessage << endl;
	_getch();
	delete[] paddedMessage;

	return 0;
}
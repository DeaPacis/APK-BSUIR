#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

unsigned int notes[] = { 392,392,392,329,349,349,349,293,293 };		// íîòû äëÿ çâóêà
unsigned int note_delay = 400;

void PlaySound();		// ôóíêöèÿ âîñïðîèçâåäåíèÿ çâóêà
void StateWords();		// ôóíêöèÿ, ñ÷èòûâàþùàÿ ñëîâà ñîñòîÿíèÿ êàíàëîâ
void CharToBin(unsigned char state, char* str);		// ôóíêöèÿ ïåðåâîäà â äâîè÷íûé êîä
void TurnSpeaker(int isActive);		// ôóíêöèÿ ðàáîòû ñ ãðîìêîãîâîðèòåëåì
void SetCount(int iDivider);		// ôóíêöèÿ óñòàíîâêè çíà÷åíèÿ ñ÷åò÷èêà
void Menu();

int main() {
	Menu();
	return 0;
}

void Menu() {
	int choice = 0;
	while (1) {
		system("cls");
		printf("1 - Play sound");
		printf("\n2 - Print channels state words");
		printf("\n0 - Exit");

		printf("\n\nEnter choice: ");
		scanf("%d", &choice);
		if (choice >= 0 && choice <= 2) {
			switch (choice) {
			case 0:
				return;
			case 1:
				PlaySound();
				break;
			case 2:
				StateWords();
				printf("\n\nPress any key to continue: ");
				scanf("%d", &choice);
				break;
			}
		}
	}
}

// ôóíêöèÿ, ñ÷èòûâàþùàÿ ñëîâà ñîñòîÿíèÿ êàíàëîâ
void StateWords()
{
	char* bin_state;
	int iChannel;
	unsigned char state;
	bin_state = (char*)calloc(9, sizeof(char));
	if (bin_state == NULL)
	{
		printf("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	for (iChannel = 0; iChannel < 3; iChannel++)
	{
		switch (iChannel)
		{
		case 0:
		{
			outp(0x43, 0xE2); // çàíîñèì óïðàâëÿþùåå ñëîâî, 
			// ñîîòâåòñòâóþùåå êîìàíäå RBC (×òåíèå ñîñòîÿíèÿ êàíàëà) è íîìåðó êàíàëà 0
			state = inp(0x40); // ÷òåíèå ñëîâà ñîñòîÿíèÿ êàíàëà 0
			CharToBin(state, bin_state);
			printf("Channel 0x40 word: %s\n", bin_state);
			break;
		}
		case 1:
		{
			bin_state[0] = '\0';
			outp(0x43, 0xE4); // çàíîñèì óïðàâëÿþùåå ñëîâî, 
			// ñîîòâåòñòâóþùåå êîìàíäå RBC (×òåíèå ñîñòîÿíèÿ êàíàëà) è íîìåðó êàíàëà 1
			state = inp(0x41); // ÷òåíèå ñëîâà ñîñòîÿíèÿ êàíàëà 1
			CharToBin(state, bin_state);
			printf("Channel 0x41 word: %s\n", bin_state);
			break;
		}
		case 2:
		{
			bin_state[0] = '\0';
			outp(0x43, 0xE8); // çàíîñèì óïðàâëÿþùåå ñëîâî, 
			// ñîîòâåòñòâóþùåå êîìàíäå RBC (×òåíèå ñîñòîÿíèÿ êàíàëà) è íîìåðó êàíàëà 2
			state = inp(0x42); // ÷òåíèå ñëîâà ñîñòîÿíèÿ êàíàëà 2
			CharToBin(state, bin_state);
			printf("Channel 0x42 word: %s\n", bin_state);
			break;
		}
		}
	}
	free(bin_state);
	return;
}

// ôóíêöèÿ ïåðåâîäà â äâîè÷íûé êîä
void CharToBin(unsigned char state, char* str)
{
	int i, j;
	char temp;
	for (i = 7; i >= 0; i--)
	{
		temp = state % 2;
		state /= 2;
		str[i] = temp + '0';
	}
	str[8] = '\0';
}

// ôóíêöèÿ óñòàíîâêè çíà÷åíèÿ ñ÷åò÷èêà
void SetCount(int iDivider) {
	long base = 1193180; // ìàêñèìàëüíàÿ ÷àñòîòà
	long kd;
	outp(0x43, 0xB6); // 10110110 - êàíàë 2, îïåðàöèÿ 4, ðåæèì 3, ôîðìàò 0
	kd = base / iDivider;
	outp(0x42, kd % 256); // ìëàäøèé áàéò äåëèòåëÿ
	kd /= 256;
	outp(0x42, kd); // ñòàðøèé áàéò äåëèòåëÿ
	return;
}

// ôóíêöèÿ ðàáîòû ñ ãðîìêîãîâîðèòåëåì
void TurnSpeaker(int isActive) {
	if (isActive) {
		outp(0x61, inp(0x61) | 3); // óñòàíàâëèâàåì 2 ìëàäøèõ áèòà 11
		return;
	}
	else {
		outp(0x61, inp(0x61) & 0xFC); // óñòàíàâëèâàåì 2 ìëàäøèõ áèòà 00
		return;
	}
}

// ôóíêöèÿ âîñïðîèçâåäåíèÿ çâóêà
void PlaySound() {
	for (int i = 0; i < 9; i++) {
		SetCount(notes[i]);
		TurnSpeaker(1); // âêëþ÷àåì ãðîìêîãîâîðèòåëü
		delay(note_delay); // óñòàíàâëèâàåì äëèòåëüíîñòü ìñ
		TurnSpeaker(0); // âûêëþ÷àåì ãðîìêîãîâîðèòåëü
	}
}

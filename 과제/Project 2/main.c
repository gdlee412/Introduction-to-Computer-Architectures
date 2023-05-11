#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialize_data();
void free_data();
void instruction_format(unsigned char instruction[4], char *instruction_set);
void format_filter(char binary[33], int *address);
void r_format(char binary[33]);
void i_format(char binary[33], char opcode[8], int *address);
void s_format(char binary[33]);
void sb_format(char binary[33], int *address);
void u_format(char binary[33], int type, int *address);
void uj_format(char binary[33], int *address); 

int registers[32] = {0};
char *data_set[70000]; 

int main(int argc, char *argv[])
{
	FILE *fp, *data_fp;
	unsigned char instruction[4];
	int instruction_num;
	int index = 0;
	int i, j;
	char *instruction_set[1000];

	if(argc != 3 && argc != 4)
	{
		printf("Error! Enter 2 - 3 arguments.\n");
		exit(1);
	}

	if((fp = fopen(argv[1], "rb")) == NULL)
	{
		printf("Error! Cannot open file\n");
		exit(1);
	}

	if(argc == 4)
	{
		if((data_fp = fopen(argv[2], "rb")) == NULL)
		{
			printf("Error! Cannot open file\n");
			exit(1);
		}
		initialize_data();
		while(fread(instruction, sizeof(instruction), 1, data_fp) != 0)
        	{
                	data_set[index] = malloc(33 * sizeof(char));
                	instruction_format(instruction, data_set[index]);
			index++;
      		}
		instruction_num = atoi(argv[3]);
	}
	else
	{
		instruction_num = atoi(argv[2]);
	}

	index = 0;
	while(fread(instruction, sizeof(instruction), 1, fp) != 0)
	{
		instruction_set[index] = malloc(33 * sizeof(char)); 
		instruction_format(instruction, instruction_set[index]);
		index++;
	}

	j = 0;
	for(i = 0; i < instruction_num; i++)
	{
		int temp_index = j;
		format_filter(instruction_set[j], &j);

		if(j == temp_index)
		{
			j++;
		}
		
		if(j == index)
		{
			break;
		}
	}
	for(i = 0; i < 32; i++)
	{
		printf("x%d: 0x%08x\n", i, registers[i]);
	}

	for(i = 0; i < index; i++)
	{
		free(instruction_set[index]);
	}
	free_data();
}

void initialize_data()
{
	unsigned char data[4] = {0xff, 0xff, 0xff, 0xff};
	int i;
	for(i = 0; i < 70000; i++)
	{
		 data_set[i] = malloc(33 * sizeof(char));
                 instruction_format(data, data_set[i]);
	}
}

void free_data()
{
	for(int i = 0; i < 70000; i++)
	{
		free(data_set[i]);
	}
}

void instruction_format(unsigned char instruction[4], char *instruction_set)
{
	char binary[33];
	int i, j;
	int k = 0;

	for(i = 3; i >= 0; i--)
	{
		int hexval = instruction[i];
		for(j = 0; j < 8; j++)
		{
			binary[k + 7 - j] = hexval % 2 + '0';
			hexval /= 2;
		}
		k += 8;
	}
	binary[32] = '\0';

	strncpy(instruction_set, binary, 33);
}

void format_filter(char binary[33], int *address)
{
	char opcode[8];
	strncpy(opcode, &binary[25], 8);

	if(strncmp(opcode, "0110011", sizeof(opcode)) == 0)
	{
		r_format(binary);
	}
	else if(strncmp(opcode, "0010011", sizeof(opcode)) == 0)
	{
		i_format(binary, opcode, address);
	}
	else if(strncmp(opcode, "0100011", sizeof(opcode)) == 0)
	{
		s_format(binary);
	}
	else if(strncmp(opcode, "0000011", sizeof(opcode)) == 0)
	{
		i_format(binary, opcode, address);
	}
	else if(strncmp(opcode, "1100011", sizeof(opcode)) == 0)
	{
		sb_format(binary, address);
	}
	else if(strncmp(opcode, "0110111", sizeof(opcode)) == 0)
	{
		u_format(binary, 0, address);
	}
	else if(strncmp(opcode, "0010111", sizeof(opcode)) == 0)
	{
		u_format(binary, 1, address);
	}
	else if(strncmp(opcode, "1101111", sizeof(opcode)) == 0)
	{
		uj_format(binary, address);
	}
	else if(strncmp(opcode, "1100111", sizeof(opcode)) == 0)
	{
		i_format(binary, opcode, address);
	}
}

void r_format(char binary[33])
{
	char funct7[8];
	char temp[6];
	char funct3[4];
	int rs1, rs2, rd;
	int binaryconverter = 1;
	int i;

	strncpy(funct7, binary, 7);
	funct7[7] = '\0';
	strncpy(temp, &binary[7], 5);
	temp[5] = '\0';
	rs2 = atoi(temp);
	strncpy(temp, &binary[12], 5);
	rs1 = atoi(temp);
	strncpy(funct3, &binary[17], 3);
	funct3[3] = '\0';
	strncpy(temp,&binary[20], 5);
	rd = atoi(temp);

 	int rs1_decimal = 0, rs2_decimal = 0, rd_decimal = 0;
        for(i = 0; i < 5; i++)
        {
                rs1_decimal += (rs1 % 10) * binaryconverter;
                rs2_decimal += (rs2 % 10) * binaryconverter;
                rd_decimal += (rd % 10) * binaryconverter;

                rs1 /= 10;
                rs2 /= 10;
                rd /= 10;
                binaryconverter *= 2;
        }

	if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "000", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] + registers[rs2_decimal];	
	}
	else if(strncmp(funct7, "0100000", sizeof(funct7)) == 0 && strncmp(funct3, "000", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] - registers[rs2_decimal];
	}
	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "001", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] << (registers[rs2_decimal] % 32);
	}

	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "010", sizeof(funct3)) == 0)
	{
		if(registers[rs1_decimal] < registers[rs2_decimal])
		{
			registers[rd_decimal] = 1;
		}
		else
		{
			registers[rd_decimal] = 0;
		}
	}
	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "100", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] ^ registers[rs2_decimal];
	}
	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "101", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = (int)((unsigned)registers[rs1_decimal] >> (registers[rs2_decimal] % 32));
	}

	else if(strncmp(funct7, "0100000", sizeof(funct7)) == 0 && strncmp(funct3, "101", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] >> (registers[rs2_decimal] % 32);
	}
	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "110", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] | registers[rs2_decimal];
	}
	else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "111", sizeof(funct3)) == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] & registers[rs2_decimal];
	}
	
	registers[0] = 0;

}

void i_format(char binary[33], char opcode[8], int *address)
{
	char immediate[13];
	char *c;
	long immediate_bin;
	char temp[6];
	char funct3[4];
	int rs1, rd;
	int binaryconverter = 1, immediate_val = 12;
	int i;
	int type = 0; // i-format = 0, l = 1, j = 2

	if(strncmp(opcode, "0000011", 8) == 0)
	{
		type = 1;
	}
	else if(strncmp(opcode, "1100111", 8) == 0)
	{
		type = 2;
	}
	
	strncpy(immediate, binary, 12);
	immediate[12] = '\0';
	immediate_bin = strtol(immediate, &c, 10);
	strncpy(temp, &binary[12], 5);
	temp[5] = '\0';
	rs1 = atoi(temp);
	strncpy(funct3, &binary[17], 3);
	funct3[3] = '\0';
	strncpy(temp,&binary[20], 5);
	rd = atoi(temp);

	if(strncmp(funct3, "001", sizeof(funct3)) == 0 && type == 0)
        {
                immediate_val = 5;

        }
        else if(strncmp(funct3, "101", sizeof(funct3)) == 0 && type == 0)
        {
                immediate_val = 5;
        }

	int rs1_decimal = 0, rd_decimal = 0, immediate_decimal = 0;
        for(i = 0; i < 5; i++)
        {
                rs1_decimal += (rs1 % 10) * binaryconverter;
                rd_decimal += (rd % 10) * binaryconverter;
                immediate_decimal += (immediate_bin % 10) * binaryconverter;

                rs1 /= 10;
                rd /= 10;
                immediate_bin /= 10;
                binaryconverter *= 2;
        }

        for(i = 5; i < immediate_val; i++)
        {
                if(i == 11)
                {
                        immediate_decimal -= (immediate_bin % 10) * binaryconverter;
                }
                else
                {
                        immediate_decimal += (immediate_bin % 10) * binaryconverter;
                }
                immediate_bin /= 10;
                binaryconverter *= 2;
        }


	if(strncmp(funct3, "000", sizeof(funct3)) == 0 && type == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] + immediate_decimal;
	}
	else if(strncmp(funct3, "010", sizeof(funct3)) == 0 && type == 0)
	{
		if(registers[rs1_decimal] < immediate_decimal)
                {
                        registers[rd_decimal] = 1;
                }
                else
                {
                        registers[rd_decimal] = 0;
                }
	}
	else if(strncmp(funct3, "100", sizeof(funct3)) == 0 && type == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] ^ immediate_decimal;
	}
	else if(strncmp(funct3, "110", sizeof(funct3)) == 0 && type == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] | immediate_decimal;
	}
	else if(strncmp(funct3, "111", sizeof(funct3)) == 0 && type == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] & immediate_decimal;
	}	
	else if(strncmp(funct3, "001", sizeof(funct3)) == 0 && type == 0)
	{
		registers[rd_decimal] = registers[rs1_decimal] << immediate_decimal;
	}
	else if(strncmp(funct3, "101", sizeof(funct3)) == 0 && type == 0)
	{
		if(strncmp(immediate, "0000000", 7) == 0)
		{
			registers[rd_decimal] = (int)((unsigned int)registers[rs1_decimal] >> (immediate_decimal % 32));			
		}
		else if(strncmp(immediate, "0100000", 7) == 0)
		{
			registers[rd_decimal] = registers[rs1_decimal] >> (immediate_decimal % 32);
		}
	}
	else if(strncmp(funct3, "010", sizeof(funct3)) == 0 && type == 1)
	{
		int data_decimal = 0;
        	binaryconverter = 1;
		if((registers[rs1_decimal] - 0x10000000) < 0x10000000)
		{	
			int address = (registers[rs1_decimal] - 0x10000000) / 4;
	        	address += (immediate_decimal) / 4;
			for(i = 31; i >= 0; i--)
        		{
				int digit = *(data_set[address] + i) - '0';
   	        		data_decimal += digit * binaryconverter;
   	    	
           			binaryconverter *= 2;
        		}
			registers[rd_decimal] = data_decimal;	
		}
		else
		{
			int input;
			scanf("%d", &input);
			registers[rd_decimal] = input;		
		}
	}
	else if(strncmp(funct3, "000", sizeof(funct3)) == 0 && type == 2)
	{
		registers[rd_decimal] = (*(address) + 1) * 4;
		*address = (registers[rs1_decimal] >> immediate_decimal) / 4;
	}

	registers[0] = 0;
}

void s_format(char binary[33])
{
	char immediate[13];
	char *c;
	long immediate_bin;
	char temp[6];
	char funct3[4];
	int rs1, rs2;
	int binaryconverter = 1;
	int i;
	
	strncpy(immediate, binary, 7);
	strncpy(temp, &binary[7], 5);
	temp[5] = '\0';
	rs2 = atoi(temp);
	strncpy(temp, &binary[12], 5);
	rs1 = atoi(temp);
	strncpy(funct3, &binary[17], 3);
	funct3[3] = '\0';
	strncpy(&immediate[7], &binary[20], 5);
	immediate[12] = '\0';
	immediate_bin = strtol(immediate, &c, 10);
	
	int rs1_decimal = 0, rs2_decimal = 0, immediate_decimal = 0;
        for(i = 0; i < 5; i++)
        {
                rs1_decimal += (rs1 % 10) * binaryconverter;
                rs2_decimal += (rs2 % 10) * binaryconverter;
                immediate_decimal += (immediate_bin % 10) * binaryconverter;

                rs1 /= 10;
                rs2 /= 10;
                immediate_bin /= 10;
                binaryconverter *= 2;
        }

        for(i = 5; i < 12; i++)
        {
                if(i == 11)
                {
                        immediate_decimal -= (immediate_bin % 10) * binaryconverter;
                }
                else
                {
                        immediate_decimal += (immediate_bin % 10) * binaryconverter;
                }
                immediate_bin /= 10;
                binaryconverter *= 2;
        }	
	
	if((registers[rs1_decimal] - 0x10000000) < 0x10000000)
	{
		int address = (registers[rs1_decimal] - 0x10000000) / 4;
		address += immediate_decimal / 4;
	
		int currdata = registers[rs2_decimal];
		char currbin[32];
		for(int i = 31; i >= 0; i--)
		{
			int currdigit = currdata % 2;
			currbin[i] = currdigit + '0';
			currdata /= 2;
		}	
		strncpy(data_set[address], currbin, 32);
	}
	else
	{
		printf("%c", registers[rs2_decimal]);
	}
	registers[0] = 0;
}

void sb_format(char binary[33], int* address)
{
	char immediate[14];
	char *c;
	long immediate_bin;
	char temp[6];
	char funct3[4];
	int rs1, rs2;
	int binaryconverter = 1;
	int i;
	
	strncpy(immediate, binary, 1);
	strncpy(&immediate[2], &binary[1], 6);
	strncpy(temp, &binary[7], 5);
	temp[5] = '\0';
	rs2 = atoi(temp);
	strncpy(temp, &binary[12], 5);
	rs1 = atoi(temp);
	strncpy(funct3, &binary[17], 3);
	funct3[3] = '\0';
	strncpy(&immediate[8], &binary[20], 4);
	strncpy(&immediate[1], &binary[24], 1);
	immediate[12] = '0';
	immediate[13] = '\0';
	immediate_bin = strtol(immediate, &c, 10);

	int rs1_decimal = 0, rs2_decimal = 0, immediate_decimal = 0;
        for(i = 0; i < 5; i++)
        {
                rs1_decimal += (rs1 % 10) * binaryconverter;
                rs2_decimal += (rs2 % 10) * binaryconverter;
                immediate_decimal += (immediate_bin % 10) * binaryconverter;

                rs1 /= 10;
                rs2 /= 10;
                immediate_bin /= 10;
                binaryconverter *= 2;
        }

        for(i = 5; i < 13; i++)
        {
                if(i == 12)
                {
                        immediate_decimal -= (immediate_bin % 10) * binaryconverter;
                }
                else
                {
                        immediate_decimal += (immediate_bin % 10) * binaryconverter;
                }
                immediate_bin /= 10;
                binaryconverter *= 2;
        }
	
	if(strncmp(funct3, "000", sizeof(funct3)) == 0)
	{
		if(registers[rs1_decimal] == registers[rs2_decimal])
		{
			*(address) += immediate_decimal / 4;
		}
	}
	else if(strncmp(funct3, "001", sizeof(funct3)) == 0)
	{
		if(registers[rs1_decimal] != registers[rs2_decimal])
                {
                        *(address) += immediate_decimal / 4;
                }
	}
	else if(strncmp(funct3, "100", sizeof(funct3)) == 0)
	{
		if(registers[rs1_decimal] < registers[rs2_decimal])
                {
                        *(address) += immediate_decimal / 4;
                }
	}
	else if(strncmp(funct3, "101", sizeof(funct3)) == 0)
	{
		if(registers[rs1_decimal] >= registers[rs2_decimal])
                {
                        *(address) += immediate_decimal / 4;
                }
	}

}

void u_format(char binary[33], int type, int *address)
{
	char immediate[21];
	char *c;
	unsigned long long int immediate_bin;
	char rd_char[6];
	int rd;
	int binaryconverter = 1;
	int i;
	
	strncpy(immediate, binary, 20);
	immediate[20] = '\0';
	immediate_bin = strtoull(immediate, &c, 10);
	strncpy(rd_char, &binary[20], 5);
	rd_char[5] = '\0';
	rd = atoi(rd_char);

	int rd_decimal = 0, immediate_decimal = 0;
	for(i = 0; i < 20; i++)
	{
		rd_decimal += (rd % 10) * binaryconverter;
		immediate_decimal += (immediate_bin % 10) * binaryconverter;

		rd /= 10;
		immediate_bin /= 10;
		binaryconverter *= 2;
	}

	immediate_decimal <<= 12;

	if(type == 0)
	{
		registers[rd_decimal] = immediate_decimal;
	}
	else
	{
		registers[rd_decimal] = (*address) * 4 + immediate_decimal;	
	}

	registers[0] = 0;
}

void uj_format(char binary[33], int *address)
{
	char immediate[21];
	char *c;
	unsigned long long int immediate_bin;
	char rd_char[6];
	int rd;
	int binaryconverter = 1;
	int i;
	
	strncpy(immediate, binary, 1);
	strncpy(&immediate[10], &binary[1], 10);
	strncpy(&immediate[9], &binary[11], 1);
	strncpy(&immediate[1], &binary[12], 8);
	immediate[20] = '\0';
	immediate_bin = strtoull(immediate, &c, 10);
	strncpy(rd_char, &binary[20], 5);
	rd_char[5] = '\0';
	rd = atoi(rd_char);

	int rd_decimal = 0, immediate_decimal = 0;
	for(i = 0; i < 20; i++)
	{
		rd_decimal += (rd % 10) * binaryconverter;
		if(i == 19)
		{
			immediate_decimal -= (immediate_bin % 10) * binaryconverter;
		}
		else
		{
			immediate_decimal += (immediate_bin % 10) * binaryconverter;
		}
		
		rd /= 10;
		immediate_bin /= 10;
		binaryconverter *= 2;
	}
	immediate_decimal <<= 1;
	registers[rd_decimal] = (*(address) + 1) * 4;
	*(address) += immediate_decimal / 4;

	registers[0] = 0;
}

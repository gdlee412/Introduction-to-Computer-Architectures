#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void instruction_format(unsigned char instruction[4]);
void r_format(char binary[33]);
void i_format(char binary[33], char opcode[8]);
void s_format(char binary[33]);
void sb_format(char binary[33]);
void u_format(char binary[33]);
void uj_format(char binary[33]);

int main(int argc, char *argv[])
{
        FILE *fp;
        unsigned char instruction[4];
        int instruction_num = 0;
        int i;

        if(argc != 2)
        {
                printf("Error! Only input the file name.");
                exit(1);
        }

        if((fp = fopen(argv[1], "rb")) == NULL)
        {
                printf("Error! Cannot open file\n");
                exit(1);
		}

        while(fread(instruction, sizeof(instruction), 1, fp) != 0)
        {
                printf("inst %d: ", instruction_num);
                for(i = 3 ; i >= 0; i--)
                {
                        if(instruction[i] == 0)
                        {
                                printf("00");
                        }
                        else if(instruction[i] < 16)
                        {
                                printf("0");
                                printf("%x", instruction[i]);
                        }
                        else
                        {
                                printf("%x", instruction[i]);
                        }
                }
                printf(" ");
                instruction_num++;
                instruction_format(instruction);
                printf("\n");
        }
}

void instruction_format(unsigned char instruction[4])
	{
        char binary[33];
        char opcode[8];
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

        strncpy(opcode, &binary[25], 8);

        if(strncmp(opcode, "0110011", sizeof(opcode)) == 0)
        {
                r_format(binary);
        }
        else if(strncmp(opcode, "0010011", sizeof(opcode)) == 0)
        {
                i_format(binary, opcode);
        }
        else if(strncmp(opcode, "0100011", sizeof(opcode)) == 0)
		{
                s_format(binary);
        }
        else if(strncmp(opcode, "0000011", sizeof(opcode)) == 0)
        {
                i_format(binary, opcode);
        }
        else if(strncmp(opcode, "1100011", sizeof(opcode)) == 0)
        {
                sb_format(binary);
        }
        else if(strncmp(opcode, "0110111", sizeof(opcode)) == 0)
        {
                printf("lui ");
                u_format(binary);
        }
        else if(strncmp(opcode, "0010111", sizeof(opcode)) == 0)
        {
                printf("auipc ");
                u_format(binary);
        }
        else if(strncmp(opcode, "1101111", sizeof(opcode)) == 0)
        {
                printf("jal ");
                uj_format(binary);
        }
        else if(strncmp(opcode, "1100111", sizeof(opcode)) == 0)
        {
                i_format(binary, opcode);
		 }
        else
        {
                printf("unknown instruction");
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

        if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "000", sizeof(funct3)) == 0)
		 {
                printf("add ");
        }
        else if(strncmp(funct7, "0100000", sizeof(funct7)) == 0 && strncmp(funct3, "000", sizeof(funct3)) == 0)
        {
                printf("sub ");
        }
        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "001", sizeof(funct3)) == 0)
        {
                printf("sll ");
        }

        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "010", sizeof(funct3)) == 0)
        {
                printf("slt ");
        }
        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "011", sizeof(funct3)) == 0)
        {
                printf("sltu ");
        }

        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "100", sizeof(funct3)) == 0)
        {
                printf("xor ");
        }
        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "101", sizeof(funct3)) == 0)
        {
                printf("srl ");
        }
	else if(strncmp(funct7, "0100000", sizeof(funct7)) == 0 && strncmp(funct3, "101", sizeof(funct3)) == 0)
        {
                printf("sra ");
        }
        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "110", sizeof(funct3)) == 0)
        {
                printf("or ");
        }
        else if(strncmp(funct7, "0000000", sizeof(funct7)) == 0 && strncmp(funct3, "111", sizeof(funct3)) == 0)
        {
                printf("and ");
        }
        else
        {
                printf("unknown instruction");
                return;
        }

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

        printf("x%d, x%d, x%d", rd_decimal, rs1_decimal, rs2_decimal);

}

void i_format(char binary[33], char opcode[8])
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

        if(strncmp(funct3, "000", sizeof(funct3)) == 0 && type == 0)
        {
                printf("addi ");
        }
        else if(strncmp(funct3, "010", sizeof(funct3)) == 0 && type == 0)
        {
                printf("slti ");
        }
        else if(strncmp(funct3, "011", sizeof(funct3)) == 0 && type == 0)
        {
                printf("sltiu ");
        }
        else if(strncmp(funct3, "100", sizeof(funct3)) == 0 && type == 0)
        {
                printf("xori ");
        }
        else if(strncmp(funct3, "110", sizeof(funct3)) == 0 && type == 0)
        {
                printf("ori ");
        }
	 else if(strncmp(funct3, "111", sizeof(funct3)) == 0 && type == 0)
        {
                printf("andi ");
        }
        else if(strncmp(funct3, "001", sizeof(funct3)) == 0 && type == 0)
        {
                printf("slli ");
                immediate_val = 5;
        }
        else if(strncmp(funct3, "101", sizeof(funct3)) == 0 && type == 0)
        {
                if(strncmp(immediate, "0000000", 7) == 0)
                {
                        printf("srli ");
                }
                else if(strncmp(immediate, "0100000", 7) == 0)
                {
                        printf("srai ");
                }
                else
                {
                        printf("unknown instruction");
                        return;
                }
                immediate_val = 5;
        }
        else if(strncmp(funct3, "000", sizeof(funct3)) == 0 && type == 1)
        {
                printf("lb ");
		}
        else if(strncmp(funct3, "001", sizeof(funct3)) == 0 && type == 1)
        {
                printf("lh ");
        }
        else if(strncmp(funct3, "010", sizeof(funct3)) == 0 && type == 1)
        {
                printf("lw ");
        }
        else if(strncmp(funct3, "100", sizeof(funct3)) == 0 && type == 1)
        {
                printf("lbu ");
        }
        else if(strncmp(funct3, "101", sizeof(funct3)) == 0 && type == 1)
        {
                printf("lhu ");
        }
        else if(strncmp(funct3, "000", sizeof(funct3)) == 0 && type == 2)
        {
                printf("jalr ");
        }
        else
        {
                printf("unknown instruction");
                return;
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
        if(type == 0)
        {
                printf("x%d, x%d, %d", rd_decimal, rs1_decimal, immediate_decimal);
        }
        else
		{
                printf("x%d, %d(x%d)", rd_decimal, immediate_decimal, rs1_decimal);
        }
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

        if(strncmp(funct3, "000", sizeof(funct3)) == 0)
		{
                printf("sb ");
        }
        else if(strncmp(funct3, "001", sizeof(funct3)) == 0)
        {
                printf("sh ");
        }
        else if(strncmp(funct3, "010", sizeof(funct3)) == 0)
        {
                printf("sw ");
        }
        else
        {
                printf("unknown instruction");
                return;
        }

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



        printf("x%d, %d(x%d)", rs2_decimal, immediate_decimal, rs1_decimal);

}

void sb_format(char binary[33])
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


        if(strncmp(funct3, "000", sizeof(funct3)) == 0)
        {
                printf("beq ");
        }
        else if(strncmp(funct3, "001", sizeof(funct3)) == 0)
        {
                printf("bne ");
        }
        else if(strncmp(funct3, "100", sizeof(funct3)) == 0)
        {
                printf("blt ");
		 }
        else if(strncmp(funct3, "101", sizeof(funct3)) == 0)
        {
                printf("bge ");
        }
        else if(strncmp(funct3, "110", sizeof(funct3)) == 0)
        {
                printf("bltu ");
        }
        else if(strncmp(funct3, "111", sizeof(funct3)) == 0)
        {
                printf("bgeu ");
        }
        else
        {
                printf("unknown instruction");
                return;
        }

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



        printf("x%d, x%d, %d", rs1_decimal, rs2_decimal, immediate_decimal);


}

void u_format(char binary[33])
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


        printf("x%d, %d", rd_decimal, immediate_decimal);
}

void uj_format(char binary[33])
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


        printf("x%d, %d", rd_decimal, immediate_decimal);



}

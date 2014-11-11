#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

#define TAM_MAIOR_INSTRUCAO 8
#define MAX_VARS 5

//typedef int (*funcp) ();

typedef unsigned char byte;

int search(int* v, int n, int x)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(x==v[i])
		{
			return i;
		}
	}
	return -1;
}

void processOperation( char operation, int firstValue, int secondValue , byte * codigo, int * byte_atual )
{
	//movl firstValue,%eax
	codigo[*byte_atual + 1] = 0xb8;
	codigo[*byte_atual + 2] = firstValue;
	
	//movl secondValue,%ecx
	codigo[*byte_atual + 3] = 0xb9;
	codigo[*byte_atual + 4] = secondValue;
	
	*byte_atual += 4;
	
	if ( operation == '+' )
	{
		//addl %eax,%ecx
		codigo[*byte_atual+1] = 0x01;
		codigo[*byte_atual+2] = 0xc1;
		*byte_atual += 2;	
	}
	else  if ( operation == '-' )
	{
		// subl %eax,%ecx ---> checar se está ao contrário!
		codigo[*byte_atual+1] = 0x29;
		codigo[*byte_atual+2] = 0xc1;
		*byte_atual+= 2; 
	}
	else if (operation == '*')
	{
		//imull %eax,%ecx
		codigo[*byte_atual+1] = 0x0f;
		codigo[*byte_atual+2] = 0xaf;
		codigo[*byte_atual+3] = 0xc8;
		*byte_atual += 3;
	}
	else
	{
		printf("Unknown operation\n");
		exit(1);	
	}
	
	//push %ecx
	codigo[*byte_atual+1] = 0x51;
	*byte_atual += 1;
}
int get_value( char vOuSifrao , int valorLido , int * used_vs , int amount_used_vs , int * cache )
{
	if (vOuSifrao == '$')
			{
				return valorLido;	
			}
	else
	{
		if (vOuSifrao == 'v')
		{
			var_index = search(used_vs,amount_used_vs,valorLido);
			if (var_index == -1)
			{
			printf("Unknown variable\n");
			exit(1);
			}
			else
			{
				return cache[var_index];
			}
		}
		else 
		{
			printf("Unknown operation\n");
			exit(1);
		}
	}

}

void printCodigo(byte* codigo, int byte_atual, int size)
{
	int i;
	printf("Codigo de Maquina Gerado= ");
	for(i=byte_atual;i<byte_atual+size;i++)
	{
		printf("%02x ",codigo[i]);
	}
	printf("\n");
}

funcp geracod (FILE *f)
{
	byte* codigo;
	
	//int i;
	
	int used_vs[5];
	
	int cache[10]; //guarda o valor de V[0-9]
	
	int amount_used_vs = 0;
	
	char first_char;
	int byte_atual = 0;
	
	int var_num;
	int var_index;
	
	byte valor_subtrair_ebp;
	
	codigo = malloc(50*TAM_MAIOR_INSTRUCAO);
	if(codigo == NULL)
	{
		printf("Code Area Allocation Error\n");
		exit(1);
	}
	
	//pushl %ebp
	codigo[byte_atual] = 0x55;
	byte_atual+=1;
	
	//movl %esp, %ebp
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0xE5;
	byte_atual+=2;
	
	// subl $20,%esp;
	codigo[byte_atual] = 0x83;
	codigo[byte_atual+1] = 0xEC;
	codigo[byte_atual+2] = 0x14;
	byte_atual+=3;
	
	while(fscanf(f," %c",&first_char)==1)
	{
		char vOuSifrao;
		int valorLido;
		printf("Caracter Lido=%c\n",first_char);
		if(first_char == 'r')
		{
			// movl $valor,%eax
			codigo[byte_atual] = 0xB8;
			fscanf(f,"et $%d",(int*) &codigo[byte_atual+1]);
			printCodigo(codigo,byte_atual,5);
			byte_atual += 5;

			//movl %ebp, %esp
			codigo[byte_atual] = 0x89;
			codigo[byte_atual+1] = 0xEC;
			printCodigo(codigo,byte_atual,2);
			byte_atual+=2;
			
			//popl %ebp
			codigo[byte_atual] = 0x5D;
			printCodigo(codigo,byte_atual,1);
			byte_atual+=1;
			
			// ret
			codigo[byte_atual] = 0xC3;
			printCodigo(codigo,byte_atual,1);
			byte_atual += 1;
		}
		else if (first_char == 'v')
		{
			int firstValue,secondValue;
			char operation;
			fscanf(f,"%d",&var_num);
			var_index = search(used_vs,amount_used_vs,var_num);
			if(var_index == -1)
			{
				if(amount_used_vs == MAX_VARS)
				{
					printf("More than 5 vars used.\n");
					exit(1);
				}
				else
				{
					used_vs[amount_used_vs] = var_num;
					var_index=amount_used_vs;
					amount_used_vs++;
				}
			}
			valor_subtrair_ebp = ((byte)MAX_VARS - (byte)var_index)*4; // bytes
			
			// movl $valor,-valor_subtrair_ebp(%ebp)
			codigo[byte_atual] = 0xC7;
			codigo[byte_atual+1] = 0x45; 
			codigo[byte_atual+2] = ~valor_subtrair_ebp + 1;
			fscanf(f," := %c%d", vOuSifrao, valorLido);
			// codigo[byte_atual+3] = valorLido; //why? 
		
			firstValue = get_value( vOuSifrao , valorLido , used_vs , amount_used_vs , cache );
			
			fscanf(f," %c ", &operation);
			fscanf(f," %c%d", vOuSifrao, valorLido);
			
			secondValue = get_value( vOuSifrao , valorLido , used_vs , amount_used_vs , cache );
			
			processOperation( operation, firstValue, secondValue , codigo, byte_atual );

			//printCodigo(codigo,byte_atual,7);
			//byte_atual += 7;

		}
		else
		{
			printf("Unknown expression\n");
			exit(1);
		}
	}
	return (funcp) codigo;
}


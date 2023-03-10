#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tfw.h"

#define firmware_array  array_mp_8822b_fw_nic
#define firmware_length  array_length_mp_8822b_fw_nic

void writefile()
{
	FILE *p = fopen("firmware.bin", "w");
	int i;
	printf("firmware_length: %d \n", firmware_length);
	printf("firmware_sizeof: %d \n ", sizeof(firmware_array));

	if(firmware_length == sizeof(firmware_array))
	{	
		printf("firmware_length = sizeof(firmware_array): check pass! \n");
		for (i = 0; i < firmware_length ; i++)
		{
			fwrite( &firmware_array[i] , 1, 1, p);
		}
		fclose(p);
	}
	else
	{
		printf("firmware_length not equal to sizeof(firmware_array): check fail! \n");
		printf("Please check firmware_array and firmware_length ! \n");
	};
}


int main ()
{

	writefile();
	printf("write firmware.bin ok! \n");

	return 0;

};

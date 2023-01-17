#include "shell.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

uint8_t shell_check_cmd(char* cmd, char* str)
{
	return (!strcmp(cmd, str));
}

void shell_reset(int argc, char argv[8][16])
{
	NVIC_SystemReset();
}

void i2c_handler(int argc, char argv[8][16])
{
	I2C_HandleTypeDef hi2c;
	uint8_t count = 0;
	uint8_t addresses[10];

	if(shell_check_cmd(argv[1], "1")) {
		memcpy(&hi2c, &hi2c1, sizeof(I2C_HandleTypeDef));
	}
	else if(shell_check_cmd(argv[1], "2")) {
		memcpy(&hi2c, &hi2c2, sizeof(I2C_HandleTypeDef));
	}

	for(int i = 1; i < 0xFF; i+=2) {
		if(HAL_I2C_IsDeviceReady(&hi2c, i, 3, 100) == HAL_OK) {
			addresses[count]=i;
			count++;
		}
	}

	printf("[asus]Found %d device(s)\r\n", count);
	for(int i = 0; i < count; i++) {
		printf("[asus]Slave %d found at addr: %d\r\n", i, addresses[i]);
	}
}

uint8_t shell_process_cmd(char* cmd)
{
  int i=0;
  int j=0;
  int k=0;
  char bdown[8][16];

  while(1) {
    if (cmd[i] == '\0') {
      bdown[j][k] = '\0';
      break;
    }

    if (cmd[i] == ' ') {
      bdown[j][k] = '\0';
      j++;
      k=0;
    } else {
      bdown[j][k] = cmd[i];
      k++;
    }
    i++;
  }
  j++;

	if ((	shell_check_cmd(bdown[0], "reset") ||
				shell_check_cmd(bdown[0], "r"))) {
    shell_reset(j, bdown);
  }
	else if(shell_check_cmd(bdown[0], "q")) {
		return 1;
	}
	else if(shell_check_cmd(bdown[0], "i2c")) {
		i2c_handler(j, bdown);
	}

	return 0;
}

void shell_start(void)
{
  int j=0;
  int k=0;
  uint8_t c[2];
  char cmd[80]={0};

  while(1) {
    k = 0;
    for(j=0;j<2;j++) {
      c[j]='-';
    }
    for(j=0;j<80;j++) {
      cmd[j]='-';
    }

    printf("[asus]> ");
    fflush(stdout);
    while(1) {
			if(VCP_retrieveInputData(c, 1, 0xFFFF) == SUCCESS) {
			//if(c[0] != '-') {
				printf("%c",c[0]);
        fflush(stdout);
        if (c[0] == 0x0D) {
          cmd[k] = '\0';
          break;
        }
        if (c[0] == 0x08) {
          printf(" ");
          printf("%c", 0x08);
          fflush(stdout);
          k -= 1;
        } else {
          cmd[k] = c[0];
          k++;
        }
			}
    }

    printf("\r\n");


    if(shell_process_cmd(cmd) == 1) {
			return;
		}
  }
}

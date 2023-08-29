

#include "rfid.h"

long Get_Rfid_ID(int fd){
	int arr[10] = {0};
	char buf[13] = {0};
	long Card_Id = 0;
	int i=0;
	if(read(fd,buf,13)==-1){
			perror("read error");
			close(fd);
			return -1;
	}
	else{
		if(buf[0] == 0x02 && buf[12] == 0x03){
			for(i=0;i<8;i++){
				if(buf[i+3]<='9')
					arr[i] = buf[i+3]-'0';
				else if(buf[i+3]>'9')
 					arr[i] = buf[i+3]-'A'+10;
				Card_Id<<=4;
				Card_Id |= arr[i];
			}
			printf("read data:%d\n",Card_Id);
			return Card_Id;
		}
	}
	return -1;
}



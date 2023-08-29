
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/input.h>

//v4l2
#include "v4l2.h"
#include "convert.h"
#include "lcd.h"

#include "click.h"

//sqlite3
#include "MySqlite.h"

#define Rfid_File					"/dev/ttySAC1"
#define Click_File					"/dev/input/event0"

//SQL
#define DB_NAME						"MyDB"
#define CreateTableGrammer			"CREATE TABLE Rfid_Table("  \
									"ID INTEGER PRIMARY KEY NOT NULL," \
									"Name char(20) not NULL ," \
									"Build_Number  int NOT NULL," \
									"Room_Number int NOT NULL," \
									"Status char(5) CHECK(Status=='YES' OR Status=='NO'));"
									
#define InsertTableGrammer			"insert into Rfid_Table values(12487184,'zhangsan',1,1,'YES');"
#define SelectTableGrammer			"select * from Rfid_Table;"

long output = 0;
char buf[13] = {0};
int i=0;
int Rfid_Usart_fd = 0;
int arr[10] = {0};

int click_fd = 0;
sqlite3 *db;

//SQL  查询语句回调函数
int SelectCallBackFunc(void *data,int col_count,char **col_values,char **col_name){
    int i;
	if(col_count == 0){
		printf("The user information was not found\n");
	}
	else{
		printf("User information is \n");
		for(i=0;i<col_count;i++)
		{
			printf("%s=%s\n",col_name[i],col_values[i]==0?"NULL":col_values[i]);
		}
	}
    return 0;
}

//串口初始化
int uart1_init(){
	int uart1_fd= open("/dev/ttySAC1", O_RDWR);//打开串口1设备文件
	if (uart1_fd == -1)
	{
		perror("open error:"); 
		return -1;
	}

	struct termios myserial;
	//清空结构体
	memset(&myserial, 0, sizeof(myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//设置控制模式状态，本地连接，接受使能
	//设置 数据位
	myserial.c_cflag &= ~CSIZE;   //清空数据位
	myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
	myserial.c_cflag |= CS8;      //数据位:8

	myserial.c_cflag &= ~CSTOPB;//   //1位停止位
	myserial.c_cflag &= ~PARENB;  //不要校验

	cfsetospeed(&myserial, B9600);  //设置波特率,B9600是定义的宏
	cfsetispeed(&myserial, B9600);

	// 刷新输出队列,清楚正接受的数据 
	tcflush(uart1_fd, TCIFLUSH);

	// 改变配置
	tcsetattr(uart1_fd, TCSANOW, &myserial);
	return   uart1_fd;
}

//rfid读取线程
void *thread_of_readfunc(void *arg){
	long ID = 0;
	while(1){
		if(read(Rfid_Usart_fd,buf,13)==-1){
			perror("read error");
			close(Rfid_Usart_fd);
			break;
		}
		else{
			if(buf[0] == 0x02 && buf[12] == 0x03){
				for(i=0;i<8;i++){
					if(buf[i+3]<='9')
						arr[i] = buf[i+3]-'0';
					else if(buf[i+3]>'9')
	 					arr[i] = buf[i+3]-'A'+10;
					output<<=4;
					output |= arr[i];
				}
				printf("read data:%d\n",output);
				
				// 6.8 add  sql
				char BUFF[200];
				sprintf(BUFF,"select * from Rfid_Table where ID = %d;",output);
				printf("%s\n",BUFF);
				DataBaseSelectGrammer(db,BUFF,SelectCallBackFunc);
				
				break;
			}
		}
	}
}

//v4l2显示线程
void *thread_of_v4l2Func(void *arg){
	struct Frame frame;
	while(1){
		frame = v4l2_get_frame();
		char rgb888[frame.width * frame.height * 3];
		// yuv直接转rgb
		YUV422toRGB888(frame.width, frame.height, frame.addr, rgb888);
		//printf("fw: %d, fh: %d\n", frame.width, frame.height);

		// -------测试yuv转jpg, jpg转rgb--------
		//unsigned char *jpgbuffer = NULL;
		//unsigned long jpgsize = 0;
		//YUYV_JPEG_MEMORY(frame.addr, frame.width, frame.height, &jpgbuffer, &jpgsize);
		// printf("jpgbuffer: %p, jpgsize: %d\n", jpgbuffer, jpgsize);
		//JPEG_RGB_MEMORY(jpgbuffer, jpgsize, rgb888);
		// -------------------------------------------

		// 显示rgb图像到lcd屏幕
		lcd_draw_rgb888(rgb888, frame.width, frame.height, 0, 0);
		// 把帧缓冲区加入采集队列
		v4l2_enqueue(frame.index);
	}
	v4l2_uninit();
	lcd_uninit();
}

//向数据库中添加用户线程
void *thread_of_AddToSQL(void *arg){
	int ID = 0;
	char name[20] = {0};
	int Build_Number = 0;
	int Room_Number = 0;
	char BUFF[200];
	
	printf("please enter your ID number(int) \n");
	scanf("%d",&ID);
	printf("Please enter your username(char *) \n");
	scanf("%s",name);
	printf("Please enter User building unit(int) \n");
	scanf("%d",&Build_Number);
	printf("Please enter the user room number(int) \n");
	scanf("%d",&Room_Number);
	
	sprintf(BUFF,"insert into Rfid_Table values(%d,'%s',%d,%d,'YES');",ID,name,Build_Number,Room_Number);
	printf("%s\n",BUFF);
	DataBaseGrammer(db,BUFF);
}

//点击位置获取
void Get_ClickPos(int x,int y){
	if(x>680 && x<720){
		if(y>60 && y<96){//点击了注册按钮
			printf("first\n");
			
			//6.8 add
			pthread_t tid_AddIdToSql;
			int ret = 0;
			ret = pthread_create(&tid_AddIdToSql,NULL,thread_of_AddToSQL,NULL);//创建线程
			if(ret == 0){
				printf("create tid_AddIdToSql thread successful\n");
				printf("create tid_AddIdToSql thread id = %ld\n",tid_AddIdToSql);
			}
			else printf("create tid_AddIdToSql thread failed!\n");
		}
		
		else if(y>160 && y<196){//点击了检测按钮
			printf("second\n");
			//rfid read thread
			pthread_t tid_rfid;
			int ret = 0;
			ret = pthread_create(&tid_rfid,NULL,thread_of_readfunc,NULL);//创建线程
			if(ret == 0){
				printf("create rfid thread successful\n");
				printf("create rfid thread id = %ld\n",tid_rfid);
			}
			else printf("create rfid thread failed!\n");
		}
		
		else if(y>260 && y<296){//点击率第三个按钮
			printf("third\n");
			DataBaseSelectGrammer(db,SelectTableGrammer,SelectCallBackFunc);
		}
		
		else if(y>360 && y<396){//点击了第四个按钮
			printf("fourth\n");
		}
	}
}

//点击获取线程
void *thread_of_clickFunc(void *arg){
	int x=0,y=0;
	int ret = 0;
	while(1){
		ret = get_ClickPos(click_fd,&x,&y);
		if(ret==0){
			printf("click x:y=%d:%d\n",x,y);
			Get_ClickPos(x,y);
		}
	}
}

int stop = 0;

//信号处理函数：当ctrl+c时，退出
void handle_signal(int signo){
	if (signo == SIGINT)
	{
		stop = 1;
	}
}

void V4l2_Init(void){
	//signal(SIGINT, handle_signal);
	v4l2_init();
	lcd_init();
	LCD_DrawBMP(0,0,"1.bmp");
	printf("V4l2 Init successful\n");
}

int main(int argc,char **argv){	

	int ret = 0;
	CreateOrOpenDataBase(DB_NAME,&db);
	CreateTable(db,CreateTableGrammer);
	DataBaseGrammer(db,InsertTableGrammer);
	V4l2_Init();
	//获取串口文件描述符
	Rfid_Usart_fd = uart1_init();
	if(Rfid_Usart_fd < 0){
		printf("Rfid usart init error\n");
		return -1;
	}
	printf("Rfid usart init successful\n");
	
	//获取屏幕点击文件描述
	click_fd = open("/dev/input/event0",O_RDONLY);
	if(click_fd < 0){
		printf("click error\n");
		return -1;
	}
	printf("click successful\n");

/*	
	//rfid thread
	pthread_t tid_rfid;
	ret = pthread_create(&tid_rfid,NULL,thread_of_readfunc,NULL);//创建线程
	if(ret == 0){
	    printf("create rfid thread successful\n");
	}
	else printf("create rfid thread failed!\n");
	    printf("create rfid thread id = %ld\n",tid_rfid);
*/

	//v4l2 thread
	pthread_t tid_v4l2;
	ret = pthread_create(&tid_v4l2,NULL,thread_of_v4l2Func,NULL);//创建线程
	if(ret == 0){
	     printf("create v4l2 thread successful\n");
		 printf("create v4l2 thread id = %ld\n",tid_v4l2);
	}
	else printf("create v4l2 thread failed!\n");
		 
	//click thread
	pthread_t tid_click;
	ret = pthread_create(&tid_click,NULL,thread_of_clickFunc,NULL);//创建线程
	if(ret == 0){
	     printf("create click thread successful\n");
		 printf("create click thread id = %ld\n",tid_click);
	}
	else printf("create click thread failed!\n");
	     
	
	while(1){
/*
		if(read(Rfid_Usart_fd,buf,13)==-1){
			perror("read error");
			close(Rfid_Usart_fd);
			return -1;
		}
		else{
			if(buf[0] == 0x02 && buf[12] == 0x03){
				for(i=0;i<8;i++){
					if(buf[i+3]<='9')
						arr[i] = buf[i+3]-'0';
					else if(buf[i+3]>'9')
 						arr[i] = buf[i+3]-'A'+10;
					output<<=4;
					output |= arr[i];
				}
				printf("read data:%d\n",output);
				output = 0;
			}
		}
*/
		
	}
	return 0;
}

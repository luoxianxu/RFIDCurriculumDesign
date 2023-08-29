
#include "uart.h"

/*设置串口
	  波特率:115200
	  数据位:8
	  校验位:不要
	  停止位:1
	  数据流控制:无
*/
int uart_init(char *file_name)
{
	int   uart1_fd= open(file_name, O_RDWR);//打开串口1设备文件
	if (uart1_fd == -1)
	{
		perror("open error:");
		return -1;
	}
	struct termios myserial;
	//清空结构体
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//设置控制模式状态，本地连接，接受使能
	//设置 数据位
	myserial.c_cflag &= ~CSIZE;   //清空数据位
	myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
	myserial.c_cflag |= CS8;      //数据位:8

	myserial.c_cflag &= ~CSTOPB;//   //1位停止位
	myserial.c_cflag &= ~PARENB;  //不要校验

	cfsetospeed(&myserial, B115200);  //设置波特率,B9600是定义的宏
	cfsetispeed(&myserial, B115200);

	/* 刷新输出队列,清除正接受的数据 */
	tcflush(uart1_fd, TCIFLUSH);

	/* 改变配置 */
	tcsetattr(uart1_fd, TCSANOW, &myserial);
	return   uart1_fd;
}

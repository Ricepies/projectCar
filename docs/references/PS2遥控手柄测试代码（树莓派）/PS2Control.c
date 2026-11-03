#include <Arduino.h>
/*定义手柄按键*/
#define PSB_SELECT 1
#define PSB_L3 2
#define PSB_R3 3
#define PSB_START 4
#define PSB_PAD_UP 5
#define PSB_PAD_RIGHT 6
#define PSB_PAD_DOWN 7
#define PSB_PAD_LEFT 8
#define PSB_L2 9
#define PSB_R2 10
#define PSB_L1 11
#define PSB_R1 12
#define PSB_TRIANGLE 13
#define PSB_CIRCLE 14
#define PSB_CROSS 15
#define PSB_SQUARE 16

/*PS2引脚定义*/
#define PS2_DAT_PIN 12 // MOS
#define PS2_CMD_PIN 13 // MIS
#define PS2_SEL_PIN 6  // CS
#define PS2_CLK_PIN 14 // SCK

/*回发过来的后4个数据是摇杆的数据*/
#define PSS_RX 5 //右摇杆X轴数据
#define PSS_RY 6 //右摇杆Y轴数据
#define PSS_LX 7 //左摇杆X轴数据
#define PSS_LY 8 //右摇杆Y轴数据

unsigned char PS2_KEY;
unsigned char X1, Y1, X2, Y2;

//按键值
unsigned short MASK[] = {
	PSB_SELECT,
	PSB_L3,
	PSB_R3,
	PSB_START,
	PSB_PAD_UP,
	PSB_PAD_RIGHT,
	PSB_PAD_DOWN,
	PSB_PAD_LEFT,
	PSB_L2,
	PSB_R2,
	PSB_L1,
	PSB_R1,
	PSB_TRIANGLE,
	PSB_CIRCLE,
	PSB_CROSS,
	PSB_SQUARE};

unsigned short Handkey;
unsigned char scan[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/******************************************
 * Function       servo_pulse
 * @author        Danny
 * @date          2017.08.16
 * @brief         定义一个脉冲函数，用来模拟方式产生PWM值
 *                时基脉冲为20ms,该脉冲高电平部分在0.5-2.5ms
 *                控制0-180度
 * @param[in1]    ServPin:舵机控制引脚
 * @param[in2]    myangle:舵机转动指定的角度
 * @param[out]    void
 * @retval        void
 * @par History   无
 *****************************************/
void servo_pulse(int ServoPin, int myangle)
{
	int PulseWidth;					   //定义脉宽变量
	PulseWidth = (myangle * 11) + 500; //将角度转化为500-2480 的脉宽值
	digitalWrite(ServoPin, HIGH);	   //将舵机接口电平置高
	delayMicroseconds(PulseWidth);	   //延时脉宽值的微秒数
	digitalWrite(ServoPin, LOW);	   //将舵机接口电平置低
	delay(20 - PulseWidth / 1000);	   //延时周期内剩余时间
	return;
}

/******************************************
 * Function       PS2_Init
 * @author        Danny
 * @date          2017.08.16
 * @brief         PS2初始化
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   无
 *****************************************/
void PS2_Init()
{
	//初始化一个SPI通道，0代表SPI通道0,500000代表的是SPI时钟速度
	wiringPiSPISetup(0, 500000);
	pinMode(PS2_CMD_PIN, OUTPUT);
	pinMode(PS2_CLK_PIN, OUTPUT);
	pinMode(PS2_DAT_PIN, INPUT);
	pinMode(PS2_SEL_PIN, OUTPUT);

	digitalWrite(PS2_CLK_PIN, HIGH);
	digitalWrite(PS2_SEL_PIN, HIGH);
	digitalWrite(PS2_CMD_PIN, HIGH);
}

/*****************************************
 * Function       PS2_AnologData
 * @author        Danny
 * @date          2017.08.16
 * @brief         读取PS2摇杆的模拟值
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   无
 *******************************************/
//得到一个摇杆的模拟量范围0~255
unsigned char PS2_AnologData(unsigned char button)
{
	return Data[button];
}

/**
 * Function       PS2_ClearData
 * @author        Danny
 * @date          2017.08.16
 * @brief         清空接受PS2的数据
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   无
 */
void PS2_ClearData()
{
	memset(Data, 0, sizeof(Data) / sizeof(Data[0]));
	return;
}

/******************************************
 * Function       PS2_ReadData
 * @author        Danny
 * @date          2017.08.16
 * @brief         读取PS2的数据
 * @param[in]     command:
 * @param[out]    void
 * @retval        void
 * @par History   无
 *****************************************/
unsigned char PS2_ReadData(unsigned char command)
{
	unsigned char i, j = 1;
	unsigned char res = 0;
	for (i = 0; i <= 7; i++)
	{
		if (command & 0x01)
			digitalWrite(PS2_CMD_PIN, HIGH);
		else
			digitalWrite(PS2_CMD_PIN, LOW);
		command = command >> 1;
		delayMicroseconds(10);
		digitalWrite(PS2_CLK_PIN, LOW);
		delayMicroseconds(10);
		if (digitalRead(PS2_DAT_PIN) == HIGH)
			res = res + j;
		j = j << 1;
		digitalWrite(PS2_CLK_PIN, HIGH);
		delayMicroseconds(10);
	}
	digitalWrite(PS2_CMD_PIN, HIGH);
	delayMicroseconds(50);
	//   printf("res:%d\n",res);
	return res;
}

/******************************************
 * Function       PS2_DataKey
 * @author        Danny
 * @date          2017.08.16
 * @brief         PS2获取按键类型
 * @param[in]     void
 * @param[out]    void
 * @retval        void
 * @par History   无
 *****************************************/
//对读出来的 PS2 的数据进行处理
//按下为 0， 未按下为 1
unsigned char PS2_DataKey()
{
	unsigned char index = 0, i = 0;
	PS2_ClearData();
	digitalWrite(PS2_SEL_PIN, LOW);
	for (i = 0; i < 9; i++) //更新扫描按键
	{
		Data[i] = PS2_ReadData(scan[i]);
		//		printf("data[%d]:%d\n",i,Data[i]);
	}
	digitalWrite(PS2_SEL_PIN, HIGH);

	//这是16个按键，按下为0，未按下为1
	Handkey = (Data[4] << 8) | Data[3];
	for (index = 0; index < 16; index++)
	{
		if ((Handkey & (1 << (MASK[index] - 1))) == 0)
		{
			return index + 1;
		}
		//		printf("index:%d\n",index+1);
	}
	return 0; //没有任何按键按下
}

/******************************************
 * Function       main
 * @author        Danny
 * @date          2017.08.16
 * @brief         对手柄发过来的指令解析，并执行相应的指令
 * @param[in]     void
 * @retval        void
 * @par History   无
 *****************************************/
void main()
{
	wiringPiSetup(); // wiringPi初始化
	PS2_Init();		 //初始化PS2
	while (1)
	{

		PS2_KEY = PS2_DataKey(); //手柄按键捕获处理
		switch (PS2_KEY)
		{
		// select键
		case PSB_SELECT:
			puts("PSB_SELECT");
			break;
		// L3键
		case PSB_L3:
			puts("PSB_L3");
			break;
		// R3键
		case PSB_R3:
			puts("PSB_R3");
			break;
		// start键
		case PSB_START:
			puts("PSB_START");
			break;
		// UP键
		case PSB_PAD_UP:
			puts("PSB_PAD_UP");
			break;

		// RIGHT键
		case PSB_PAD_RIGHT:
			puts("PSB_PAD_RIGHT");
			break;
		// DOWN键按
		case PSB_PAD_DOWN:
			puts("PSB_PAD_DOWN");
			break;
		// LEFT键
		case PSB_PAD_LEFT:
			puts("PSB_PAD_LEFT");
			break;
		// L2按键
		case PSB_L2:
			puts("PSB_L2");
			break;
		// R2按键
		case PSB_R2:
			puts("PSB_R2");
			break;

		// L1按键
		case PSB_L1:
			puts("PSB_L1");
			break;
		// R1按键
		case PSB_R1:
			puts("PSB_R1");
			break;
		//三角形按键
		case PSB_TRIANGLE:
			puts("PSB_TRIANGLE");
			break;
		//圆形键
		case PSB_CIRCLE:
			puts("PSB_CIRCLE");
			break;
		//方形键
		case PSB_SQUARE:
			puts("PSB_SQUARE");
			break;

		// X按键
		case PSB_CROSS:
			puts("PSB_X");
			break;
		}

		//当L1或者R1按下时，读取摇杆数据的模拟值
		if (PS2_KEY == PSB_L1 || PS2_KEY == PSB_R1)
		{
			X1 = PS2_AnologData(PSS_LX);
			Y1 = PS2_AnologData(PSS_LY);
			X2 = PS2_AnologData(PSS_RX);
			Y2 = PS2_AnologData(PSS_RY);
		}

		//下面的延时是必须要的,主要是为了避免过于频繁的发送手柄指令造成的不断重启
		delay(50);
	}
}

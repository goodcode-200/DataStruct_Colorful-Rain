#include<graphics.h>
#include<stdio.h>
#include<time.h>
#include<conio.h>
#include<pthread.h>   //多线程
#include<stdlib.h>
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"pthreadVC2.lib")  //使用这个库时必须加上这句
  


POINT point;           //windows自定义的结构体  点
HWND hwnd;             //窗口句柄
int sleeptime;         //函数挂起时间


//这里是###接口或全局标记###定义位置，由这里定义更改各种全局参数
//---------------------------------
#define MAX 300	   //雨滴的总数
#define STEP 70   //雨滴的步进值（刷新时间间隔内雨滴前进的距离）
#define WID 1      //雨滴线的宽度（从左至右）
#define LEN  30    //雨滴线的长度（从上至下）
#define HZ  50    //控制打雷概率的数值（数值越大，打雷的概率越低）
int TAG = 0;       //初始化一个tag，判断雨是否已经初始化过
//---------------------------------

//这里是结构体定义位置
//---------------------------------
typedef struct Rain {
	int x0, y0;//雨滴的起始位置 
	int x, y;//雨滴的终止位置 
	int step;//步进 
	COLORREF color;//雨滴的颜色 
	//雨滴的绘制所需的参数 
	int left; 
	int top; 
	int right; 
	int bottom; 
	double stangle; 
	double endangle;
	int Drop_tag;
	int CLEAN_TG;      //判断是否应该清除雨   （有时是刚生成，就不必清除）
	int ID;            //判断雨线清除样式
}Rain;
Rain graph[MAX];
typedef struct Leaf{
	int left;
	int top;
	int right;
	int bottom;
	int x1,y1,x2,y2,x3,y3,x4,y4;
	COLORREF fillcolor;
	COLORREF linecolor;
}Leaf;
typedef struct Thunder
{
    int x0, y0;//顶点位置
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int x4, y4;
    int x5, y5;
    int x6, y6;
    int x7, y7;
    int x8, y8;
    int x9, y9;
}Thunder;


//荷叶部分 
void draw_leaf(int left, int top, int right, int bottom, COLORREF fillcolor, COLORREF linecolor) 
{ 
	//画荷叶的椭圆部分 
	setfillcolor(fillcolor); 
	setcolor(fillcolor); 
	fillellipse(left, top, right, bottom); 
	//画荷叶中的线条部分 
	setlinecolor(linecolor); 
	setlinestyle(PS_SOLID, 2); 
	int ox = (left + right) / 2; 
	int oy = (top + bottom) / 2; 
	line(ox - 20, oy - 10, ox + 20, oy + 10); 
	line(ox - 20, oy + 10, ox + 20, oy - 10); 
	setlinecolor(linecolor); 
}


//池塘部分 
void pool() 
{ 
	int R,G,B;
	R=0;
	G=0;
	B=108;
	setfillcolor(RGB(R,G,B));
	setcolor(RGB(R,G,B));
	fillrectangle(0, 650, 1000, 800);
	//绘制池塘中的荷叶 
	/*
	draw_leaf(450, 700, 550, 750, GREEN, BLACK);
	draw_leaf(380, 720, 480, 770, GREEN, BLACK); 
	*/
}

//Rain
void InitRain(int i)
{
	int R,G,B;
	graph[i].x0 = rand()%1000;
	if(TAG==0)
	{	
		graph[i].y0 = rand()%(650-STEP);
	}
	else	
		graph[i].y0 = rand()%STEP;
	graph[i].left = graph[i].x0;
	graph[i].top = graph[i].y0;
	graph[i].right = graph[i].left + WID;
	graph[i].bottom = graph[i].top + LEN;
	graph[i].stangle = 4.28;
	graph[i].endangle = 5.22;
	graph[i].step = STEP;
	R = rand()%255;
	G = rand()%255;
	B = rand()%255;
	graph[i].color = RGB(R,G,B);
	graph[i].Drop_tag = 0;//此标记说明雨未落入池塘
}

//不同时刻雨的位置函数
void Rain_quiet(int i)
{
	int width = 150-STEP;

	// 擦掉原来
	if(graph[i].CLEAN_TG)
	{
		if(graph[i].Drop_tag==1)     //说明上个雨滴是圈
		{
			int x1,x2,y1,y2;
			setcolor(RGB(0,0,108));
			x1 = graph[i].left-10;
			x2 = graph[i].left+10;
			y1 = graph[i].bottom-5;
			y2 = graph[i].bottom+5;
			ellipse (x1,y1,x2,y2);
		}
		else                         //上个雨滴是线,擦除时注意分部分
		{ 
			if(graph[i].ID==2)
			{
				setfillcolor(0);//填充随机颜色(雨滴内) 
				setlinecolor(0);//填充随机颜色(雨滴线) 
				fillpie(graph[i].left,graph[i].top,graph[i].right,651,graph[i].stangle,graph[i].endangle);
				setfillcolor(RGB(0,0,108));//填充随机颜色(雨滴内) 
				setlinecolor(RGB(0,0,108));//填充随机颜色(雨滴线) 
				fillpie(graph[i].left,650,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);
			}
			else if(graph[i].ID==1)
			{
				setfillcolor(RGB(0,0,108));//填充随机颜色(雨滴内) 
				setlinecolor(RGB(0,0,108));//填充随机颜色(雨滴线) 
				fillpie(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);
			}
			else 
			{
				setfillcolor(0);//填充随机颜色(雨滴内) 
				setlinecolor(0);//填充随机颜色(雨滴线) 
				fillpie(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);
			}
		}
 
		// 计算新位置
		if(graph[i].Drop_tag==1)
		{
			InitRain(i);
		}
		else
		{
			graph[i].top += graph[i].step;
			graph[i].bottom += graph[i].step;
		}
		

		if (graph[i].bottom >= 650+(rand()%width))   //落入池塘位置随机
		{
			graph[i].Drop_tag = 1 ;  //设置标记，入水与否
		}
		if(graph[i].bottom>=650) // 设置清除雨线标记
		{
			if(graph[i].top<650) graph[i].ID=2;  //分两段清除
			else if(graph[i].top>=650) graph[i].ID =1;  //只一段清除，用池塘色填充
			else graph[i].ID = 0;   //即清除雨线全部,用黑色填充
		}
		else graph[i].ID=0;
	}
	// 画new
	if(graph[i].Drop_tag==1)   //说明此时是入水状态，画圆圈
	{
		int x1,x2,y1,y2;
		setcolor(graph[i].color);
		x1 = graph[i].left-10;
		x2 = graph[i].left+10;
		y1 = graph[i].bottom-5;
		y2 = graph[i].bottom+5;
		ellipse (x1,y1,x2,y2);
	}
	else
	{
		setfillcolor(graph[i].color);//填充随机颜色(雨滴内) 
		setlinecolor(graph[i].color);//填充随机颜色(雨滴线) 
		fillpie(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);//绘制雨滴
	}
}

//Thunder
void thunder()
{
    int x = rand() % 500;// +20;
    int y = rand() % 100;//+10;
    int stepx = rand() % 2;
	int R,G,B;
    Thunder one;
    one.x0 = x + 0 * stepx; one.y0 = y + 0 * stepx;
    one.x1 = x + 120 * stepx; one.y1 = y + 80 * stepx;
    one.x2 = x + 100 * stepx; one.y2 = y + 120 * stepx;
    one.x3 = x + 140 * stepx; one.y3 = y + 160 * stepx;
    one.x4 = x + 100 * stepx; one.y4 = y + 200 * stepx;
    one.x5 = x + 160 * stepx; one.y5 = y + 280 * stepx;
    one.x6 = x + 80 * stepx; one.y6 = y + 200 * stepx;
    one.x7 = x + 100 * stepx; one.y7 = y + 160 * stepx;
    one.x8 = x + 60 * stepx; one.y8 = y + 140 * stepx;
    one.x9 = x + 80 * stepx; one.y9 = y + 80 * stepx;
    POINT thunder[] = {
        { one.x0,one.y0 },
        { one.x1,one.y1 },
        { one.x2,one.y2 },
        { one.x3,one.y3 },
        { one.x4,one.y4 },
        { one.x5,one.y5 },
        { one.x6,one.y6 },
        { one.x7,one.y7 },
        { one.x8,one.y8 },
        { one.x9,one.y9 },
    };
	
	R=255;                    //打雷时设计背景高亮
	G=255;
	B=255;
	setfillcolor(RGB(R,G,B));
	setcolor(RGB(R,G,B));
	fillrectangle(0,0,1000,650);
	R=0;                    //打雷时池塘高亮
	G=206;
	B=209;
	setfillcolor(RGB(R,G,B));
	setcolor(RGB(R,G,B));
	fillrectangle(0,650,1000,800);

	setpolyfillmode(WINDING);
    setfillcolor(RGB(205,173,0));
    solidpolygon(thunder, 10);//这个函数用于画填充的多边形（无边框）。void solidpolygon( const POINT *points, int num);参数：points 每个点的坐标，数组元素个数为 num。该函数会自动连接多边形首尾。num多边形顶点的个数。
	Sleep(200);
    clearpolygon(thunder, 10);
	clearrectangle(0,0,1000,650);
	R=0;                    //打雷后池塘恢复
	G=0;
	B=108;
	setfillcolor(RGB(R,G,B));
	setcolor(RGB(R,G,B));
	fillrectangle(0,650,1000,800);
}


// 主函数
int main()
{
	int i;

	srand((unsigned)time(NULL)); // 随机种子
	initgraph(1000, 800);	// 打开图形窗口

	pool();   //Draw a pool......

	// 初始化所有rain
	for(i=0; i<MAX; i++)
	{
		InitRain(i);
	}
	TAG = 1;         //标记所有雨滴第一次初始化过了
	// 绘制rain，按任意键退出
	//PlaySound (TEXT("C:/Users/asus/Desktop/2798.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );
	PlaySound (TEXT("Materier/10379.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );     //This is rain sound         

	while(!kbhit())
	{
		for(i=0; i<MAX; i++)  //绘制一帧雨的图片
		{
			Rain_quiet(i);
			graph[i].CLEAN_TG = 1;   //标记可以清除了
		}
		Sleep(30);
		if (rand()%HZ==1)  //判断是否应该打雷时来降低打雷的概率
		{
			thunder();
		}
		else if(rand()%HZ==2)  //雷声激烈 ，此伏彼起
		{ 
			thunder();
			thunder();
		}
	
	}
	closegraph();    // 关闭图形窗口
	return 0;
}






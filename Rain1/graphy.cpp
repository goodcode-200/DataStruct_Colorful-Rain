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
#define MAX 500	   //雨滴的总数
#define STEP 70   //雨滴的步进值（刷新时间间隔内雨滴前进的距离）
#define WID 1      //雨滴线的宽度（从左至右）
#define LEN  16    //雨滴线的长度（从上至下）
#define HZ  500   //控制打雷概率的数值（数值越大，打雷的概率越低）
int TAG = 0;       //初始化一个tag，判断雨是否已经初始化过
#define NUM 200  //转变风向后持续绘制的帧数，数值越大，风向改变的频率越低
int direction = 1;     //初始化雨的降落方向，即雨的偏移方向（0,1,2）
//---------------------------------

//这里是结构体定义位置
//---------------------------------
typedef struct Rain { 
	COLORREF color;//雨滴的颜色 
	//雨滴的绘制所需的参数 
	int left; 
	int top; 
	int right; 
	int bottom; 
	int Drop_tag;
	int CLEAN_TG;      //判断是否应该清除雨   （有时是刚生成，就不必清除）
	int ID;            //判断雨线清除样式
	int times;         //雨滴入水后泛起涟漪的大小（倍数）
	int direction;     //雨的撒落方向，取（0：左倾，1：直下，2：右倾）
	int trans;         //控制标记，标记此雨滴绘制后，下次位置是否转向
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
}

//Rain
void InitRain(int i, int de ,int tr)
{
	int R,G,B;
	graph[i].left = rand()%1000;
	if(TAG==0)
	{	
		graph[i].top = rand()%(650-STEP);
	}
	else	
		graph[i].top = rand()%STEP;
	if(tr==1)
	{
		if(de == 0)
		{
			graph[i].right = graph[i].left;
			graph[i].direction = 1;
		}
		else if(de == 1)
		{
			graph[i].right = graph[i].left + LEN;
			graph[i].direction = 2;
		}
		else if(de == 2)
		{
			graph[i].right = graph[i].left - LEN;
			graph[i].direction = 0;
		}
	}
	else if(tr==0)
	{
		if(de == 0)
			graph[i].right = graph[i].left - LEN;
		else if(de == 1)
			graph[i].right = graph[i].left;
		else if(de == 2)
			graph[i].right = graph[i].left + LEN;
	}
	graph[i].bottom = graph[i].top + LEN;
	R = rand()%255;
	G = rand()%255;
	B = rand()%255;
	graph[i].color = RGB(R,G,B);
	graph[i].Drop_tag = 0;//此标记说明雨未落入池塘
}

//不同时刻雨的位置函数
void Rain_quiet(int i)
{
	int width = 140-STEP;
	int midx,midy;   //擦除跨界雨滴时的中间值
	int st;
	// 擦掉原来
	if(graph[i].CLEAN_TG)
	{
		int de,tr;
		if(graph[i].Drop_tag==1)     //说明上个雨滴是圈
		{
			int x1,x2,y1,y2;
			setcolor(RGB(0,0,108));
			x1 = graph[i].right-6*graph[i].times;
			x2 = graph[i].right+6*graph[i].times;
			y1 = graph[i].bottom-3*graph[i].times;
			y2 = graph[i].bottom+3*graph[i].times;
			ellipse (x1,y1,x2,y2);
		}
		else                         //上个雨滴是线,擦除时注意分部分
		{ 
			if(graph[i].ID==2)
			{
				int next;
				st = 649-graph[i].top;
				midy = 649;
				if(graph[i].direction==0)
				{
					midx = graph[i].left -st;
					next = midx-1;
				}
				else if(graph[i].direction == 1)
				{
					midx = graph[i].left;
				    next = midx;
				}
				else if(graph[i].direction == 2)
				{
					midx = graph[i].left + st;
					next = midx +1;
				}
				setcolor(0);
				line(graph[i].left,graph[i].top,midx,midy); 
				setcolor(RGB(0,0,108)); 
				line(next,midy+1,graph[i].right,graph[i].bottom);
			}
			else if(graph[i].ID==1)
			{
				setcolor(RGB(0,0,108));
				line(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom);
			}
			else 
			{ 
				setcolor(0);
				line(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom);
			}
		}
 
		// 计算新位置
		if(graph[i].Drop_tag==1)
		{
			de = graph[i].direction;
			tr = graph[i].trans;
			InitRain(i,de,tr);
		}
		else if(graph[i].trans==1)
		{   
			if(graph[i].direction==0)
			{
				graph[i].right = graph[i].left;
				graph[i].direction = 1;
			}
			else if(graph[i].direction==1)
			{
				graph[i].right = graph[i].left + LEN;
				graph[i].direction = 2;
			}
			else if(graph[i].direction==2)
			{
				graph[i].right = graph[i].left - LEN;
				graph[i].direction = 0;
			}
		}
		else if(graph[i].trans == 0)
		{
			graph[i].top += STEP;
			graph[i].bottom += STEP;
			if(graph[i].direction == 0)
			{
				graph[i].left = graph[i].left-STEP;
				graph[i].right = graph[i].right-STEP;
			}
			else if(graph[i].direction == 2)
			{
				graph[i].left += STEP;
				graph[i].right += STEP;
			}	
		}
		

		if (graph[i].bottom >= 660+(rand()%width))   //落入池塘位置随机
		{
			graph[i].Drop_tag = 1 ;  //设置标记，入水与否
		}
		if(graph[i].right<0)
		{
			graph[i].left += 1000;
			graph[i].right = graph[i].left - LEN;
		}
		else if(graph[i].right>1000)
		{
			graph[i].left += -1000;
			graph[i].right = graph[i].left + LEN;
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
		int times = rand()%3+1;
		graph[i].times = times;
		setcolor(graph[i].color);
		x1 = graph[i].right-6*times;
		x2 = graph[i].right+6*times;
		y1 = graph[i].bottom-3*times;
		y2 = graph[i].bottom+3*times;
		ellipse (x1,y1,x2,y2);
	}
	else
	{
		setfillcolor(graph[i].color);//填充随机颜色(雨滴内) 
		setlinecolor(graph[i].color);//填充随机颜色(雨滴线) 
		line(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom);
	}
}

//Thunder
void thunder()
{
    int x = rand() % 700;// +20;
    int y = rand() % 60;//+10;
    int stepx = rand() % 2+1;
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
	Sleep(100);
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
	int i,j;
	int times;
	srand((unsigned)time(NULL)); // 随机种子
	initgraph(1000, 800);	// 打开图形窗口

	pool();   //Draw a pool......

	// 初始化所有rain
	for(i=0; i<MAX; i++)
	{
		InitRain(i,0,1);
		graph[i].direction = direction;
	}
	TAG = 1;         //标记所有雨滴第一次初始化过了
	// 绘制rain，按任意键退出
	//PlaySound (TEXT("C:/Users/asus/Desktop/2798.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );
	PlaySound (TEXT("Materier/10379.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );     //This is rain sound         
	
	while(!kbhit())
	{
		for(j=0; j<NUM; j++)
		{  
			
			for(i=0; i<MAX; i++)  //绘制一帧雨的图片
			{
				if (j == NUM-1)  //这是雨滴下落路径的转折点，此时，下一个绘制底部坐标只增加一个横向偏移量
					graph[i].trans = 1;
				else
					graph[i].trans = 0;
				Rain_quiet(i);     //把这滴雨绘制出来
				graph[i].CLEAN_TG = 1;   //标记可以清除了
			}
				Sleep(20);
			if (rand()%HZ==1)  //判断是否应该打雷时来降低打雷的概率
			{
				thunder();
			}
			else if(rand()%HZ==2)  //雷声激烈 ，此伏彼起
			{ 
				thunder();
				thunder();
			}
			else if(rand()%HZ==3)  //雷声更激烈了
			{
				thunder();
				thunder();
				thunder();
			}
		}
	}
	closegraph();    // 关闭图形窗口
	return 0;
}






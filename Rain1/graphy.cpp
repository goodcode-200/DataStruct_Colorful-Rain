#include<graphics.h>
#include<stdio.h>
#include<time.h>
#include<conio.h>
#include<process.h>    //多线程
#include<stdlib.h>
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define MAX 100	
#define STEP 60
POINT point;           //windows自定义的结构体  点
HWND hwnd;             //窗口句柄
int sleeptime;         //函数挂起时间

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
	R=151;
	G=255;
	B=255;
	setfillcolor(RGB(R,G,B));
	setcolor(RGB(R,G,B));
	fillrectangle(0, 500, 600, 800);
	//绘制池塘中的荷叶 
	draw_leaf(450, 550, 550, 600, GREEN, BLACK); 
	draw_leaf(380, 570, 480, 620, GREEN, BLACK); 
}

//Rain
void InitRain(int i)
{
	int R,G,B;
	graph[i].x0 = rand()%600;
	graph[i].y0 = rand()%453;
	graph[i].left = graph[i].x0;
	graph[i].top = graph[i].y0;
	graph[i].right = graph[i].left + 1;
	graph[i].bottom = graph[i].top +45;
	graph[i].stangle = 4.28;
	graph[i].endangle = 5.22;
	graph[i].step = STEP;
	R = rand()%255;
	G = rand()%255;
	B = rand()%255;
	graph[i].color = RGB(R,G,B);
}

void DropRain(int i)
{
	// 擦掉原来
	setfillcolor(0);//填充随机颜色(雨滴内) 
	setlinecolor(0);//填充随机颜色(雨滴线) 
	fillpie(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);//绘制雨滴
 
	// 计算新位置
	graph[i].top += graph[i].step;
	graph[i].bottom += graph[i].step;
	if (graph[i].bottom > 500)	InitRain(i);
 
	// 画new
	setfillcolor(graph[i].color);//填充随机颜色(雨滴内) 
	setlinecolor(graph[i].color);//填充随机颜色(雨滴线) 
	fillpie(graph[i].left,graph[i].top,graph[i].right,graph[i].bottom,graph[i].stangle,graph[i].endangle);//绘制雨滴
	
}

//Thunder
void thunder()
{
    int x = rand() % 500;// +20;
    int y = rand() % 100;//+10;
    double stepx = rand() % 2;
    Thunder one;
    one.x0 = x + 0 * stepx; one.y0 = y + 0 * stepx;
    one.x1 = x + 60 * stepx; one.y1 = y + 40 * stepx;
    one.x2 = x + 50 * stepx; one.y2 = y + 60 * stepx;
    one.x3 = x + 70 * stepx; one.y3 = y + 80 * stepx;
    one.x4 = x + 50 * stepx; one.y4 = y + 100 * stepx;
    one.x5 = x + 80 * stepx; one.y5 = y + 140 * stepx;
    one.x6 = x + 40 * stepx; one.y6 = y + 100 * stepx;
    one.x7 = x + 50 * stepx; one.y7 = y + 80 * stepx;
    one.x8 = x + 30 * stepx; one.y8 = y + 70 * stepx;
    one.x9 = x + 40 * stepx; one.y9 = y + 40 * stepx;
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
	setpolyfillmode(WINDING);
    setfillcolor(YELLOW);
    solidpolygon(thunder, 10);//这个函数用于画填充的多边形（无边框）。void solidpolygon( const POINT *points, int num);参数：points 每个点的坐标，数组元素个数为 num。该函数会自动连接多边形首尾。num多边形顶点的个数。
    Sleep(200);
    clearpolygon(thunder, 10);
}

// 主函数
void main()
{
	int i;
	srand((unsigned)time(NULL)); // 随机种子
	initgraph(600, 800);	// 打开图形窗口
	
	pool();   //Draw a pool......
	// 初始化所有rain
	for(i=0; i<MAX; i++)
	{
		InitRain(i);
	}
 
	// 绘制rain，按任意键退出
	
	PlaySound (TEXT("Materier/2798.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );
	//PlaySound (TEXT("Materier/10379.wav"),NULL,SND_FILENAME | SND_ASYNC| SND_LOOP );     //This is rain sound         
	while(!kbhit())
	{
		for(i=0; i<MAX; i++)
			DropRain(i);
		thunder();
		Sleep(20);
	}
 
	closegraph();    // 关闭图形窗口
}



/*
void CreateRain(Rain* rain)
{
	Rain *p,*q;
	int i;
	p = ra;
	for(i=0;i<40;i++)
	{
		q = (Rain *)malloc(sizeof(Rain));
		q->next = NULL;
		InitRain(q);
		p->next = q;
		p->n = i;
		p = q;
	}
	p->next = ra;
}

void rainMove(Rain *r) { GetCursorPos(&point); // 获取鼠标指针位置（屏幕坐标） 
	ScreenToClient(hwnd, &point); // 将鼠标指针位置转换为窗口坐标 
	//判断鼠标是否在窗口之内 
	if (point.x >= 0 && point.x <= 640 && point.y >= 0 && point.y <= 480) 
	{ //雨滴的x随风而改变 //雨滴的y增大更多 
		if (point.x >= 320) 
		{ 
			r->x0 += 20; r->y0 += (r->step + 10); 
		} 
		else { 
			r->x0 -= 20; 
			r->y0 += (r->step + 10); 
		} 
		sleeptime = 130;//修改Sleep时间，加快雨滴(模拟狂风骤雨)
	} 
	else { 
		r->y0 = r->y0 + r->step; 
		sleeptime = 250;//修改Sleep时间，减缓雨滴(模拟和风细雨) 
	}
}

//檫除雨线中雨滴的上一个位置 
void cleanRain(Rain *r) 
{ 
	setfillcolor(Getcolor(r->x0, r->y0));
	//填充背景颜色(雨滴内) 
	setlinecolor(Getcolor(r->x0, r->y0));
	//填充背景色(雨滴线) 
	fillpie(r->x0, r->y0, r->x0 + 20, r->y0 + 25, r->stangle, r->endangle);//绘制雨滴 
}
	
//绘制雨滴 
void drowRain(Rain *r) 
{ 
	setfillcolor(r->color);//填充随机颜色(雨滴内) 
	setlinecolor(r->color);//填充随机颜色(雨滴线) 
	fillpie(r->x0, r->y0, r->x0 + 20, r->y0 + 25, r->stangle, r->endangle);//绘制雨滴 
}

//整个雨滴的动画效果 
void rainDrop(Rain *r) 
{ 
	if (r->y0 >= (300 + rand() % 180)) 
	{ 
		cleanrain(r); //檫除雨滴 
		r->x = r->x0; 
		r->y = r->y0; 
		m->rain = r; 
		_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))drawwave,(music*)m, 0, 0); 
		//多线程 
		InitRain(r); 
		//刷新雨滴 
	} 
	else 
	{
		cleanrain(r);
	//擦除雨滴 
	rainmove(r);
	//计算雨滴下一个位置        
	drowrain(r);//绘制雨滴
    }
}
*/


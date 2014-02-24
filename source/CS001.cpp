#include <Windows.h>
#include <stdio.h>

#define CS_QUAD_SIZE 40
#define CS_COUNT_MAX 25
#define CS_FIXATION_MSEC 200

char char_buf[4096];

// Глобальные переменные, которые могут потребоваться везде
char*		CSAppName="Измеряем скорость курсора";
HINSTANCE	CSInst;
HWND		CShwnd;
HBRUSH green_brush, red_brush;
LONG  screen_x, screen_y;


POINT p1={100,200},p2={200,100};
DWORD popali_time,first_time;

bool popali=false;
int count=0;
//====================================================================================
// Оконная процедура
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd,
						UINT message,
						WPARAM wparam,
						LPARAM lparam)
{
	RECT r;
	int xPos, yPos;

	switch (message)
	{
		case WM_CREATE:
			break;

		case WM_RBUTTONDOWN:
			count=0;
			KillTimer(hwnd,1);
			popali=false;
			InvalidateRect(hwnd,NULL,TRUE);
			break;

		case WM_DESTROY:	// Завершение программы
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			screen_x=LOWORD(lparam);
			screen_y=HIWORD(lparam);
			break;


		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc=BeginPaint(hwnd,&ps);
			
			r.left=p1.x-CS_QUAD_SIZE/2; r.top=p1.y-CS_QUAD_SIZE/2;
			r.right=p1.x+CS_QUAD_SIZE/2; r.bottom=p1.y+CS_QUAD_SIZE/2;
			FillRect(hdc,&r,red_brush);
			
			r.left=p2.x-CS_QUAD_SIZE/2; r.top=p2.y-CS_QUAD_SIZE/2;
			r.right=p2.x+CS_QUAD_SIZE/2; r.bottom=p2.y+CS_QUAD_SIZE/2;
			FillRect(hdc,&r,green_brush);

			sprintf(char_buf,"Шаг: %02d Время: %4.2f",count, (popali_time-first_time)/1000.0f);

			if(0==count) TextOut(hdc,20,20,"Приготовились!  ",14);
			else 
			TextOut(hdc,20,20,char_buf,strlen(char_buf));

			EndPaint(hwnd,&ps);
			break;

		case WM_MOUSEMOVE:
			xPos=LOWORD(lparam);
			yPos=HIWORD(lparam);
			
			if(CS_COUNT_MAX<=count) {break;}

			// Мы внутри нужного квадратика? квадратика?
			if((abs(p1.x-xPos)<CS_QUAD_SIZE/2)&&(abs(p1.x-xPos)<CS_QUAD_SIZE/2))
			{
				if(!popali) // Приехали в квадратик, запомним время
				{
					popali=true;
					popali_time=timeGetTime();
					if(0==count) first_time=popali_time;
					
					SetTimer(hwnd,1,CS_FIXATION_MSEC,NULL); // Выждем 1/5 секунды
					
				}
			}
			else
			{
				if(popali) // Уехали из квадратика, который должны были ждать
				{
					popali=false;
					KillTimer(hwnd,1);
				}
			}
			break;

		case WM_TIMER:
			if(timeGetTime()-popali_time>CS_FIXATION_MSEC) // 1/5 секунды
			{
				// Время пришло!
				KillTimer(hwnd,1);
				if(popali)
				{
					//следующую точку давай!
					p1=p2;
					p2.x=50+rand()*(screen_x-100)/RAND_MAX;
					p2.y=50+rand()*(screen_y-150)/RAND_MAX;
					count++;
					InvalidateRect(hwnd,NULL,TRUE);
				}
			}
			break;


		default: // Сообщения обрабатываются системой
			return DefWindowProc(hwnd,message,wparam,lparam);
	}

return 0; // сами обработали
}


int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR cline,INT)
// Командную строку не обрабатываем
{
	ATOM aresult; // Для всяких кодов возврата
	BOOL boolresult;
	MSG msg; // Сообщение
	char *CSWindowCName="CSpeed10";


	// Делаем hInst доступной для всех	
	CSInst=hInst;

	// Найдём размер экрана
	screen_x=GetSystemMetrics(SM_CXSCREEN);
	screen_y=GetSystemMetrics(SM_CYSCREEN);

	// Кисти
	green_brush=CreateSolidBrush(RGB(100,255,100));
	red_brush=CreateSolidBrush(RGB(227,20,2));

	// Регистрация класса окна
	WNDCLASS wcl={CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                          //LoadIcon( hInst, MAKEINTRESOURCE(IDI_ICON1)),
						  LoadIcon( NULL, IDI_APPLICATION),
                          LoadCursor(NULL, IDC_ARROW), 
                          (HBRUSH)GetStockObject(WHITE_BRUSH), 
						  //NULL,
						  NULL,
						  //MAKEINTRESOURCE(IDR_MENU1),
						  TEXT(CSWindowCName) };

	aresult=RegisterClass(&wcl);

	if (aresult==0)
		return (1);

	
	//Создание главного окна
	CShwnd=CreateWindow( TEXT(CSWindowCName),
		TEXT(CSAppName),
		//WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		//rect.right-rect.left, rect.bottom-rect.top,
		0L, 0L,
		hInst, 0L );

	if (CShwnd==NULL)
	{
		//BKBReportError(__FILE__,"CreateWindow",__LINE__);
		return (1);
	}

	// Показываем окно
    ShowWindow( CShwnd, SW_MAXIMIZE );

	boolresult=UpdateWindow( CShwnd ); 
	if(boolresult==0)
	{
		//CSReportError(__FILE__,"UpdateWindow",__LINE__);
		return (1);
	}
		
	//Цикл обработки сообщений
	while(GetMessage(&msg,NULL,0,0)) 
    {
		TranslateMessage( &msg );
        DispatchMessage( &msg );
	}// while !WM_QUIT

	// Кисти
	DeleteObject(green_brush);
	DeleteObject(red_brush);

	return 0;
}

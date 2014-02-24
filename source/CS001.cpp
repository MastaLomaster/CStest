#include <Windows.h>
#include <stdio.h>

#define CS_QUAD_SIZE 40
#define CS_COUNT_MAX 25
#define CS_FIXATION_MSEC 200

char char_buf[4096];

// ���������� ����������, ������� ����� ������������� �����
char*		CSAppName="�������� �������� �������";
HINSTANCE	CSInst;
HWND		CShwnd;
HBRUSH green_brush, red_brush;
LONG  screen_x, screen_y;


POINT p1={100,200},p2={200,100};
DWORD popali_time,first_time;

bool popali=false;
int count=0;
//====================================================================================
// ������� ���������
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

		case WM_DESTROY:	// ���������� ���������
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

			sprintf(char_buf,"���: %02d �����: %4.2f",count, (popali_time-first_time)/1000.0f);

			if(0==count) TextOut(hdc,20,20,"�������������!  ",14);
			else 
			TextOut(hdc,20,20,char_buf,strlen(char_buf));

			EndPaint(hwnd,&ps);
			break;

		case WM_MOUSEMOVE:
			xPos=LOWORD(lparam);
			yPos=HIWORD(lparam);
			
			if(CS_COUNT_MAX<=count) {break;}

			// �� ������ ������� ����������? ����������?
			if((abs(p1.x-xPos)<CS_QUAD_SIZE/2)&&(abs(p1.x-xPos)<CS_QUAD_SIZE/2))
			{
				if(!popali) // �������� � ���������, �������� �����
				{
					popali=true;
					popali_time=timeGetTime();
					if(0==count) first_time=popali_time;
					
					SetTimer(hwnd,1,CS_FIXATION_MSEC,NULL); // ������ 1/5 �������
					
				}
			}
			else
			{
				if(popali) // ������ �� ����������, ������� ������ ���� �����
				{
					popali=false;
					KillTimer(hwnd,1);
				}
			}
			break;

		case WM_TIMER:
			if(timeGetTime()-popali_time>CS_FIXATION_MSEC) // 1/5 �������
			{
				// ����� ������!
				KillTimer(hwnd,1);
				if(popali)
				{
					//��������� ����� �����!
					p1=p2;
					p2.x=50+rand()*(screen_x-100)/RAND_MAX;
					p2.y=50+rand()*(screen_y-150)/RAND_MAX;
					count++;
					InvalidateRect(hwnd,NULL,TRUE);
				}
			}
			break;


		default: // ��������� �������������� ��������
			return DefWindowProc(hwnd,message,wparam,lparam);
	}

return 0; // ���� ����������
}


int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR cline,INT)
// ��������� ������ �� ������������
{
	ATOM aresult; // ��� ������ ����� ��������
	BOOL boolresult;
	MSG msg; // ���������
	char *CSWindowCName="CSpeed10";


	// ������ hInst ��������� ��� ����	
	CSInst=hInst;

	// ����� ������ ������
	screen_x=GetSystemMetrics(SM_CXSCREEN);
	screen_y=GetSystemMetrics(SM_CYSCREEN);

	// �����
	green_brush=CreateSolidBrush(RGB(100,255,100));
	red_brush=CreateSolidBrush(RGB(227,20,2));

	// ����������� ������ ����
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

	
	//�������� �������� ����
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

	// ���������� ����
    ShowWindow( CShwnd, SW_MAXIMIZE );

	boolresult=UpdateWindow( CShwnd ); 
	if(boolresult==0)
	{
		//CSReportError(__FILE__,"UpdateWindow",__LINE__);
		return (1);
	}
		
	//���� ��������� ���������
	while(GetMessage(&msg,NULL,0,0)) 
    {
		TranslateMessage( &msg );
        DispatchMessage( &msg );
	}// while !WM_QUIT

	// �����
	DeleteObject(green_brush);
	DeleteObject(red_brush);

	return 0;
}

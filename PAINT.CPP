#include<graphics.h>
#include<dos.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>

union REGS i, o;
int topcolor[17];
int lefticon[7];

int disp_opcode[16];
int disp_identifier[16];
int disp_arguments[16];

int mem_identifier[200];
int mem_argx[200];
int mem_argy[200];

//initialize arrays to all 0s

int mem_ptr=0;
int disp_ptr=0;
int identifier=1;

int current_color=0;
int selected_layer=0;

//Used in stack for Seed Fill

void interpret();//prototype defined

void push(int arr[],int v,int *top)
	{	(*top)++;
		if(*top==5000)
		{	closegraph();
			printf("Stack Overflow");
			getch();
			exit(1);
		}
			arr[*top]=v;
	}

	
int pop(int arr[],int *top)
	{	
		int temp;
		temp=arr[*top];
		(*top)--;
		return temp;
	}

	
void memory_init()
{
int i;
		for(i=0;i<=15;i++)
		{
		disp_opcode[i]=0;
		disp_identifier[i]=0;
		disp_arguments[i]=0;
		}

		for(i=0;i<=199;i++)
		{
		mem_argx[i]=0;
		mem_identifier[i]=0;
		mem_argy[i]=0;
		}
}


int get_key()
{
   union REGS i,o;
 
   i.h.ah = 0;
   int86(22,&i,&o);
 
   return ( o.h.ah );
}


void showmouseptr()
{
   i.x.ax = 1;
   int86(0x33,&i,&o);
}
 
 
void hidemouseptr()
{
   i.x.ax = 2;
   int86(0x33,&i,&o);
}
 
 
void restrictmouseptr( int x1, int y1, int x2, int y2)
{
   i.x.ax = 7;
   i.x.cx = x1;
   i.x.dx = x2;
   int86(0x33,&i,&o);
 
   i.x.ax = 8;
   i.x.cx = y1;
   i.x.dx = y2;
   int86(0x33,&i,&o);
}
 
 
void getmousepos(int *button,int *x,int *y)
{
   i.x.ax = 3;
   int86(0x33,&i,&o);
 
   *button = o.x.bx;
   *x = o.x.cx;
   *y = o.x.dx;
}


void draw_color_panel()
{
   int left, top, c, color;

   left = getmaxx()-30;
   top = 10;
   //store original color
   color = getcolor();
   setcolor(DARKGRAY);
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   bar(left-5,0,getmaxx(),getmaxy());
   setcolor(WHITE);
   settextstyle(SANS_SERIF_FONT, VERT_DIR, 1);
   outtextxy(getmaxx()-15,getmaxy()-50,"Colour");

   for( c = 0 ; c <= 15 ; c++ )
   {
      setfillstyle(SOLID_FILL, c);
      bar3d(left, top, left+16, top+16,10,1);
      topcolor[c] = top;
      top=top+26;
   }
   topcolor[c]=top;
   //load original color
   setcolor(color);
}


void draw_layers_panel()
{
int left, top,i;
left = getmaxx()-85;
top = 10;

setfillstyle(SOLID_FILL,DARKGRAY);
bar(left,0,getmaxx()-35,getmaxy());

setcolor(WHITE);
   settextstyle(SANS_SERIF_FONT, VERT_DIR, 1);

   outtextxy(getmaxx()-55,getmaxy()-50,"Layers");

   setfillstyle(SOLID_FILL, LIGHTGRAY);

for(i=0;i<=15;i++)
{
bar(left+5,top,left+40,top+16);
top=top+26;
}

}


void write_layers()
{
int dptr,color,top=18;
draw_layers_panel();
color=getcolor();
setcolor(BLACK);
settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
for(dptr=0;dptr<=15;dptr++)
{
	if(disp_opcode[dptr]==1)//Line
	{outtextxy(getmaxx()-78,top,"Line");
	top=top+26;
	}
	else if(disp_opcode[dptr]==2)//Circle
	{outtextxy(getmaxx()-78,top,"Circ");
	top=top+26;
	}
	else if(disp_opcode[dptr]==3)//Rectangle
	{outtextxy(getmaxx()-78,top,"Rect");
	top=top+26;
	}
	else if(disp_opcode[dptr]==4)//Fill
	{outtextxy(getmaxx()-78,top,"Fill");
	top=top+26;
	}
	else if(disp_opcode[dptr]==5)//Colour
	{setcolor(disp_arguments[dptr]);
	outtextxy(getmaxx()-78,top,"Colr");
	top=top+26;
	}
	
}

setcolor(color);

}


void draw_help_screen()
{
int xc=(getmaxx()-90)/2;
int yc=40;

settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
setfillstyle(SOLID_FILL,LIGHTGRAY);
bar(20,20,getmaxx()-110,getmaxy()-50);
setcolor(BLACK);
rectangle(20,20,getmaxx()-110,getmaxy()-50);
setcolor(WHITE);
rectangle(40,40,getmaxx()-130,getmaxy()-70);
outtextxy(xc,getmaxy()-90,"Press Q to go back to canvas");
bar(xc-20,yc-5,xc+20,yc+5);
setcolor(BLACK);
outtextxy(xc-15,yc-2,"HELP");
setcolor(DARKGRAY);
outtextxy(80,yc+30,"F5");
outtextxy(120,yc+30,"Save");
outtextxy(80,yc+50,"F6");
outtextxy(120,yc+50,"Load");
outtextxy(80,yc+70,"ESC");
outtextxy(120,yc+70,"Exit");

yc=yc+70;

yc=yc+50;
outtextxy(80,yc,"Translate:");
outtextxy(200,yc,"ARROW KEYS");

yc=yc+50;
outtextxy(80,yc,"Rotate wrt end point:");
outtextxy(280,yc,"Z(acw)   X(cw)");

yc=yc+30;
outtextxy(80,yc,"Rotate wrt mid point:");
outtextxy(280,yc,"C(acw)   V(cw)");

yc=yc+50;
outtextxy(80,yc,"Scale uniformly:");
outtextxy(280,yc,"Q(dec)   E(inc)");

yc=yc+30;
outtextxy(80,yc,"Scale in X direction:");
outtextxy(280,yc,"A(dec)   D(inc)");

yc=yc+30;
outtextxy(80,yc,"Scale in Y direction:");
outtextxy(280,yc,"W(dec)   S(inc)");


	while(1)
	{
				if(kbhit())
				{
				
					 if( get_key() == 16 )
					 {	delay(250);
						break;
					 }
					 
				}
	}
}


void store_color(int col)
{
if(disp_ptr==16) //If more than 16 instructions added,override last instruction
{disp_ptr=15;}

current_color=col;
disp_opcode[disp_ptr]=5;
disp_identifier[disp_ptr]=0;
disp_arguments[disp_ptr]=col;


disp_ptr++;

write_layers();

}


void change_color(int x, int y)
{	
   int c,selected;
   for (c=0;c<=15;c++)
   {
   if(y>topcolor[c] && y<topcolor[c+1])
   {//setcolor(c);
	selected=c;
   }
   }
   
   if(current_color!=selected)
   {
   store_color(selected);
   }
   
   return;
   

}


void select_layer(int x, int y)
{
   int c;
   for (c=0;c<=15;c++)
   {
   if(y>topcolor[c] && y<topcolor[c+1])
   {selected_layer=c;
   break;}
   
   }

 
}


int select_icon(int x, int y)
{
   int select,d;
   for (d=0;d<=5;d++)
   {
   if(x>lefticon[d] && x<lefticon[d+1])
   {select=d;}
   }


	return select;

}


void draw_icon_panel()
{
   int left, top, d, color,textalign=23;
 
   left = 20;
   top = getmaxy()-30;
   setfillstyle(SOLID_FILL, LIGHTGRAY);
   bar(0,top,getmaxx()-85,getmaxy());

 setfillstyle(SOLID_FILL, DARKGRAY);
   for( d = 0 ; d <= 5 ; d++ )
   {
   bar(left, top+5, left+50, top+25);
    lefticon[d] = left;
    left =left+70;
   }
   lefticon[d] = left;
	setcolor(WHITE);
   settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
   outtextxy(textalign,getmaxy()-17,"Rect..");
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"Circle");
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"Line");
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"Fill");
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"Clear");
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"Delete");
   
   setcolor(DARKGRAY);
   textalign+=70;
   outtextxy(textalign,getmaxy()-17,"F1: Help Menu");
   
}


void store_line(int x1,int y1,int x2,int y2)
{
if(disp_ptr==16) //If more than 16 instructions added,override last instruction
{disp_ptr=15;}


disp_opcode[disp_ptr]=1;
disp_identifier[disp_ptr]=identifier;
disp_arguments[disp_ptr]=0;


mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x1;
mem_argy[mem_ptr]=y1;


mem_ptr++;


mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x2;
mem_argy[mem_ptr]=y2;


mem_ptr++;

disp_ptr++;
identifier++;
}


void store_rectangle(int x1,int y1,int x2,int y2)
{
if(disp_ptr==16) //If more than 16 instructions added,override last instruction
{disp_ptr=15;}


disp_opcode[disp_ptr]=3;
disp_identifier[disp_ptr]=identifier;
disp_arguments[disp_ptr]=0;


mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x1;
mem_argy[mem_ptr]=y1;


mem_ptr++;


mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x2;
mem_argy[mem_ptr]=y2;


mem_ptr++;

disp_ptr++;
identifier++;
}


void store_circle(int x1,int y1,int x2,int y2)
{

if(disp_ptr==16) //If more than 16 instructions added,override last instruction
{disp_ptr=15;}



disp_opcode[disp_ptr]=2;
disp_identifier[disp_ptr]=identifier;
disp_arguments[disp_ptr]=(int)(sqrt((pow((x2-x1), 2))+(pow((y2-y1), 2))));


mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x1;
mem_argy[mem_ptr]=y1;


mem_ptr++;


disp_ptr++;
identifier++;
}


void store_fill(int x1,int y1)
{

if(disp_ptr==16) //If more than 16 instructions added,override last instruction
{disp_ptr=15;}

disp_opcode[disp_ptr]=4;
disp_identifier[disp_ptr]=identifier;
disp_arguments[disp_ptr]=0;



mem_identifier[mem_ptr]=identifier;
mem_argx[mem_ptr]=x1;
mem_argy[mem_ptr]=y1;

mem_ptr++;

disp_ptr++;
identifier++;
}


void SeedFill(int seedx, int seedy,int bg_color)
{		
		int fill_color=getcolor();
		int topx=-1;
		int topy=-1;
		int x1,y1;
		int arrx[10000],arry[10000];

//To get border color
x1=seedx;
y1=seedy;
while(getpixel(x1,y1)==bg_color)
{
x1++;
}
int border_color=getpixel(x1,y1);

		
	push(arrx,seedx,&topx);
	push(arry,seedy,&topy);
	
		while(topx!=-1 && topy!=-1)
		{		x1=pop(arrx,&topx);
				y1=pop(arry,&topy);
		
				
					putpixel(x1,y1,fill_color);
					
					if(getpixel(x1-1,y1)!=border_color && getpixel(x1-1,y1)!=fill_color && getpixel(x1-1,y1)==bg_color)
					{
					push(arrx,x1-1,&topx);
					push(arry,y1,&topy);
					}
					
					if(getpixel(x1+1,y1)!=border_color && getpixel(x1+1,y1)!=fill_color && getpixel(x1+1,y1)==bg_color)
					{
					push(arrx,x1+1,&topx);
					push(arry,y1,&topy);
					}
					
					if(getpixel(x1,y1-1)!=border_color && getpixel(x1,y1-1)!=fill_color && getpixel(x1,y1-1)==bg_color)
					{
					push(arrx,x1,&topx);
					push(arry,y1-1,&topy);
					}
					
					if(getpixel(x1,y1+1)!=border_color && getpixel(x1,y1+1)!=fill_color && getpixel(x1,y1+1)==bg_color)
					{
					push(arrx,x1,&topx);
					push(arry,y1+1,&topy);
					}
					
				
			
		}
		 

	
}


void delete_layer(int layer)
{
			int dptr,a,b,c;
			
			if(disp_opcode[layer]!=0)
			{
						if(layer==15)
						{	disp_opcode[15]=0;
							disp_identifier[15]=0;
							disp_arguments[15]=0;
						}
						
						else if(layer<15)
						{
							for(dptr=layer;dptr<15;dptr++)
							{
							a=disp_opcode[dptr+1];
							disp_opcode[dptr]=a;
							
							b=disp_identifier[dptr+1];
							disp_identifier[dptr]=b;
							
							c=disp_arguments[dptr+1];
							disp_arguments[dptr]=c;
							}
							disp_opcode[15]=0;
							disp_identifier[15]=0;
							disp_arguments[15]=0;
							
						}
			}

}


void dda(int xa,int ya,int xb,int yb)
{
float x1=xa;
float y1=ya;
float x2=xb;
float y2=yb;

int c=getcolor();

 float delx,dely,adelx,adely;
 float len;
 float X,Y;
 int i;

 delx=x2-x1;
 dely=y2-y1;

 adelx=abs(delx);
 adely=abs(dely);

 if(adelx>=adely)
  len=adelx;
 else
  len=adely;


 delx=delx/len;
 dely=dely/len;
 if(delx==0)
 X= x1;
 else
 X= x1 + 0.5 * (abs(delx)/delx);  // sign
 if(dely==0)
 Y = y1;
 else
 Y= y1 + 0.5 * (abs(dely)/dely);
 i=0;

 while(i<=len)
 {
  putpixel((int)X,(int)Y,c);
  X+=delx;
  Y+=dely;
  i++;

 }
}


void save_file()
{
FILE * pFile;
   int n,a,b,c,dptr,mptr;

   pFile = fopen ("savefile.dat","w");
   fprintf (pFile, "%d %d %d\n",disp_ptr, mem_ptr,identifier);
   
   for(dptr=0;dptr<=15;dptr++)
	{
		a=disp_opcode[dptr];
		b=disp_identifier[dptr];
		c=disp_arguments[dptr];
		fprintf (pFile, "%d %d %d\n",a,b,c);
	}
	
	for(mptr=0;mptr<=199;mptr++)
	{
		a=mem_identifier[mptr];
		b=mem_argx[mptr];
		c=mem_argy[mptr];
		fprintf (pFile, "%d %d %d\n",a,b,c);
	}
	
	fclose (pFile);
	
	c=getcolor();
	setcolor(LIGHTGRAY);
	settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
	outtextxy(10,10,"File Saved");
	setcolor(c);
}


void load_file()
{
FILE * pFile;
   int n,a,b,c,dptr,mptr;

   pFile = fopen ("savefile.dat","r");
   
   fscanf (pFile, "%d %d %d\n",&a,&b,&c);
   disp_ptr=a;
   mem_ptr=b;
   identifier=c;
   
   for(dptr=0;dptr<=15;dptr++)
	{
		fscanf (pFile, "%d %d %d\n",&a,&b,&c);
		disp_opcode[dptr]=a;
		disp_identifier[dptr]=b;
		disp_arguments[dptr]=c;
	}


	
	for(mptr=0;mptr<=199;mptr++)
	{
		fscanf (pFile, "%d %d %d\n",&a,&b,&c);
		mem_identifier[mptr]=a;
		mem_argx[mptr]=b;
		mem_argy[mptr]=c;
	}
	

	fclose (pFile);
	
	interpret();
	
	c=getcolor();
	setcolor(LIGHTGRAY);
	settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
	outtextxy(10,10,"File Loaded");
	setcolor(c);
}


void interpret()
{
int mptr,dptr,id,x1,x2,y1,y2,color,rad;

setfillstyle(SOLID_FILL,WHITE);
bar(0,0,getmaxx()-85,getmaxy()-30);

setcolor(BLACK);
rectangle(0,0,getmaxx()-86,getmaxy()-31);




for(dptr=0;dptr<=15;dptr++)
{
	if(disp_opcode[dptr]==1)//Line
	{
		id=disp_identifier[dptr];
			
		for(mptr=0;mptr<=199;mptr++)
		{
		if(mem_identifier[mptr]==id)
			{		
			x1=mem_argx[mptr];
			y1=mem_argy[mptr];
			mptr++;
			x2=mem_argx[mptr];
			y2=mem_argy[mptr];
			dda(x1,y1,x2,y2);
			break;
			}
		}	
	}
	
	else if(disp_opcode[dptr]==2)
	{
		id=disp_identifier[dptr];
		rad=disp_arguments[dptr];
		
		for(mptr=0;mptr<=199;mptr++)
		{
		if(mem_identifier[mptr]==id)
			{
			x1=mem_argx[mptr];
			y1=mem_argy[mptr];
			circle(x1,y1,rad);
			break;
			}
		}
	}
	
	else if(disp_opcode[dptr]==3)//Rectangle
	{
		id=disp_identifier[dptr];
		
		for(mptr=0;mptr<=199;mptr++)
		{
		if(mem_identifier[mptr]==id)
			{		
			x1=mem_argx[mptr];
			y1=mem_argy[mptr];
			mptr++;
			x2=mem_argx[mptr];
			y2=mem_argy[mptr];
			rectangle(x1,y1,x2,y2);
			break;
			}
		}	
	}
	
		
	else if(disp_opcode[dptr]==4)
	{
		id=disp_identifier[dptr];
	
		for(mptr=0;mptr<=199;mptr++)
		{
		if(mem_identifier[mptr]==id)
			{
			x1=mem_argx[mptr];
			y1=mem_argy[mptr];
			y2=getpixel(x1,y1);
			SeedFill(x1,y1,y2);
			break;
			}
		}
	}
	
	else if(disp_opcode[dptr]==5)
	{
		color=disp_arguments[dptr];
		setcolor(color);
		setfillstyle(SOLID_FILL,color);
			
	}
	
}

write_layers();

}


void translate (int layer, int choice)
{	
					int x1,y1,x2,y2,id,mptr=0;
					if(disp_opcode[layer]==1)//Line
					{
						id=disp_identifier[layer];
						
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{		
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
								if(choice==0)//right move
								x1++;
								else if(choice==1)//leftmove
								x1--;
								else if(choice==2)//up move
								y1++;
								else if(choice==3)//downmove
								y1--;
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							
							mptr++;
							x2=mem_argx[mptr];
							y2=mem_argy[mptr];
								if(choice==0)//right move
								x2++;
								else if(choice==1)//leftmove
								x2--;
								else if(choice==2)//up move
								y2++;
								else if(choice==3)//downmove
								y2--;
							mem_argx[mptr]=x2;
							mem_argy[mptr]=y2;
							break;
							}
						}	
					}
					
					else if(disp_opcode[layer]==2)//circle
					{
						id=disp_identifier[layer];
						
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
								if(choice==0)//right move
								x1++;
								else if(choice==1)//leftmove
								x1--;
								else if(choice==2)//up move
								y1++;
								else if(choice==3)//downmove
								y1--;
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							break;
							}
						}
					}
					
					else if(disp_opcode[layer]==3)//Rectangle
					{
						id=disp_identifier[layer];
						
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{		
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
								if(choice==0)//right move
								x1++;
								else if(choice==1)//leftmove
								x1--;
								else if(choice==2)//up move
								y1++;
								else if(choice==3)//downmove
								y1--;
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							
							mptr++;
							x2=mem_argx[mptr];
							y2=mem_argy[mptr];
								if(choice==0)//right move
								x2++;
								else if(choice==1)//leftmove
								x2--;
								else if(choice==2)//up move
								y2++;
								else if(choice==3)//downmove
								y2--;
							mem_argx[mptr]=x2;
							mem_argy[mptr]=y2;
							break;
							}
						}	
					}

}


void rotate (int layer, int choice)
{	
	int x1,y1,x2,y2,id,xa,ya,xb,yb,xc,yc,mptr=0;

	float cos_factor=cos(5*3.14/180);
	float sin_factor=sin(5*3.14/180);
	float x_transa,y_transa,x_transb,y_transb;
	
					if(disp_opcode[layer]==1 || disp_opcode[layer]==3)//Line OR Rectangle
					{
						id=disp_identifier[layer];
						
						//printf("%d has line",dptr);
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{		
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
							x2=mem_argx[mptr+1];
							y2=mem_argy[mptr+1];
								if(choice==0)//right move wrt first point
								{
								yb=y2-y1;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
								xb=x2-x1;
								x_transb=(xb*cos_factor)-(yb*sin_factor);
								y_transb=(xb*sin_factor)+(yb*cos_factor);
								x2=(int)(x_transb+x1);//shifting back to old system
								y2=(int)(y_transb+y1);
								}
								else if(choice==1)//leftmove wrt first point
								{
								yb=y2-y1;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
								xb=x2-x1;
								x_transb=(xb*cos_factor)+(yb*sin_factor);
								y_transb=(yb*cos_factor)-(xb*sin_factor);
								x2=(int)(x_transb+x1);//shifting back to old system
								y2=(int)(y_transb+y1);
								}
								
								if(choice==2)//right move wrt center
								{
									xc=(x1+x2)/2;//Calculate centre
									yc=(y1+y2)/2;
									
									//Shifting origins to centre point
									ya=y1-yc;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
									xa=x1-xc;
									yb=y2-yc;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
									xb=x2-xc;
									
									//Rotations Ans Shift back to old origin
									x_transa=(xa*cos_factor)-(ya*sin_factor);
									y_transa=(xa*sin_factor)+(ya*cos_factor);
									x1=(int)(x_transa+xc);//shifting back to old system
									y1=(int)(y_transa+yc);
									
									x_transb=(xb*cos_factor)-(yb*sin_factor);
									y_transb=(xb*sin_factor)+(yb*cos_factor);
									x2=(int)(x_transb+xc);//shifting back to old system
									y2=(int)(y_transb+yc);
								}
								
								else if(choice==3)//leftmove wrt center
								{
									xc=(x1+x2)/2;//Calculate centre
									yc=(y1+y2)/2;
									
									//Shifting origins to centre point
									ya=y1-yc;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
									xa=x1-xc;
									yb=y2-yc;//Y_new=Y_old - Y_neworigin: shifting origin to x1,y1
									xb=x2-xc;
									
									//Rotations Ans Shift back to old origin
									x_transa=(xa*cos_factor)+(ya*sin_factor);
									y_transa=(ya*cos_factor)-(xa*sin_factor);
									x1=(int)(x_transa+xc);//shifting back to old system
									y1=(int)(y_transa+yc);
									
									x_transb=(xb*cos_factor)+(yb*sin_factor);
									y_transb=(yb*cos_factor)-(xb*sin_factor);
									x2=(int)(x_transb+xc);//shifting back to old system
									y2=(int)(y_transb+yc);
								}
								
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							mem_argx[mptr+1]=x2;
							mem_argy[mptr+1]=y2;
							break;
							}
						}	
					}
					
					
}


void scale (int layer, int choice)
{	
					int x1,y1,x2,y2,id,mptr=0;
					if(disp_opcode[layer]==1)//Line
					{
						id=disp_identifier[layer];
						
						//printf("%d has line",dptr);
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{		
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
								if(choice==0)//right move
								x1=x1*1.1;
								else if(choice==1)//leftmove
								x1=x1*0.9;
								else if(choice==2)//up move
								y1=y1*1.1;
								else if(choice==3)//downmove
								y1=y1*0.9;
								else if(choice==4)//incease size
								{y1=y1*1.1;
								x1=x1*1.1;}
								else if(choice==5)//decrease size
								{y1=y1*0.9;
								x1=x1*0.9;}
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							
							mptr++;
							x2=mem_argx[mptr];
							y2=mem_argy[mptr];
								if(choice==0)//right move
								x2=x2*1.1;
								else if(choice==1)//leftmove
								x2=x2*0.9;
								else if(choice==2)//up move
								y2=y2*1.1;
								else if(choice==3)//downmove
								y2=y2*0.9;
								else if(choice==4)//incease size
								{y2=y2*1.1;
								x2=x2*1.1;}
								else if(choice==5)//decrease size
								{y2=y2*0.9;
								x2=x2*0.9;}
							mem_argx[mptr]=x2;
							mem_argy[mptr]=y2;
							break;
							}
						}	
					}
					
					else if(disp_opcode[layer]==2)//circle
					{
						id=disp_identifier[layer];
						
						y2=disp_arguments[layer];
								if(choice==0)//right move
								y2=y2*1.1;
								else if(choice==1)//leftmove
								y2=y2*0.9;
								else if(choice==2)//up move
								y2=y2*1.1;
								else if(choice==3)//downmove
								y2=y2*0.9;
								else if(choice==4)//incease size
								y2=y2*1.1;
								else if(choice==5)//decrease size
								y2=y2*0.9;
						disp_arguments[layer]=y2;
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{
							x2=mem_argx[mptr];
							y2=mem_argy[mptr];
								if(choice==0)//right move
								x2=x2*1.1;
								else if(choice==1)//leftmove
								x2=x2*0.9;
								else if(choice==2)//up move
								y2=y2*1.1;
								else if(choice==3)//downmove
								y2=y2*0.9;
								else if(choice==4)//incease size
								{y2=y2*1.1;
								x2=x2*1.1;}
								else if(choice==5)//decrease size
								{y2=y2*0.9;
								x2=x2*0.9;}
							mem_argx[mptr]=x2;
							mem_argy[mptr]=y2;
							break;
							}
						}
					}
					
					else if(disp_opcode[layer]==3)//Rectangle
					{
						id=disp_identifier[layer];
						
						//printf("%d has line",dptr);
						
						for(mptr=0;mptr<=199;mptr++)
						{
						if(mem_identifier[mptr]==id)
							{		
							x1=mem_argx[mptr];
							y1=mem_argy[mptr];
								if(choice==0)//right move
								x1=x1*1.1;
								else if(choice==1)//leftmove
								x1=x1*0.9;
								else if(choice==2)//up move
								y1=y1*1.1;
								else if(choice==3)//downmove
								y1=y1*0.9;
								else if(choice==4)//incease size
								{y1=y1*1.1;
								x1=x1*1.1;}
								else if(choice==5)//decrease size
								{y1=y1*0.9;
								x1=x1*0.9;}
							mem_argx[mptr]=x1;
							mem_argy[mptr]=y1;
							
							mptr++;
							x2=mem_argx[mptr];
							y2=mem_argy[mptr];
								if(choice==0)//right move
								x2=x2*1.1;
								else if(choice==1)//leftmove
								x2=x2*0.9;
								else if(choice==2)//up move
								y2=y2*1.1;
								else if(choice==3)//downmove
								y2=y2*0.9;
								else if(choice==4)//incease size
								{y2=y2*1.1;
								x2=x2*1.1;}
								else if(choice==5)//decrease size
								{y2=y2*0.9;
								x2=x2*0.9;}
							mem_argx[mptr]=x2;
							mem_argy[mptr]=y2;
							break;
							}
						}	
					}

}


void clear_canvas()
{
memory_init();
mem_ptr=0;
disp_ptr=0;
identifier=1;
current_color=0;
setfillstyle(SOLID_FILL,WHITE);
bar(0,0,getmaxx()-85,getmaxy()-30);

setcolor(BLACK);
rectangle(0,0,getmaxx()-86,getmaxy()-31);
draw_layers_panel();
}


void main()
{
   int gd = DETECT,gm;

   int maxx,maxy,x,y,button,prevx,prevy,temp1,temp2,key,color=0;
   int select=2;            // default line tool is selected
	memory_init();
	int layer_selected=2;

   initgraph(&gd,&gm,"C:\\TC\\BGI");

   maxx = getmaxx();
   maxy = getmaxy();

   setcolor(BLACK);
   rectangle(0,0,maxx,maxy);


   draw_color_panel();
   draw_layers_panel();
   draw_icon_panel();


	setviewport(1,1,maxx-1,maxy-1,1);
	restrictmouseptr(1,1,maxx-1,maxy-1);
	showmouseptr();



	setviewport(1,1,maxx-35,maxy-30,1);

	
	clear_canvas();
	
   while(1)
   {
				  if(kbhit())
				  {
					 key = get_key();

					 if( key == 1 )
					 {	
						closegraph();
						exit(0);
					 }
					 else if( key == 75)//left translate
					 {
						translate(selected_layer,1);

						interpret();
						continue;
					 }
					 else if( key == 77)//right translate
					 {
						translate(selected_layer,0);

						interpret();
						continue;
					 }
					 else if( key == 72)//up arrow
					 {
						translate(selected_layer,3);
						interpret();
						continue;
					 }
					 else if( key == 80)//down arrow
					 {
						translate(selected_layer,2);
						interpret();
						continue;
					 }
					 else if( key == 17)//w arrow scale y increment
					 {
						scale(selected_layer,3);
						interpret();
						continue;
					 }
					 else if( key == 30)//a arrow scale x decrement
					 {
						scale(selected_layer,1);
						interpret();
						continue;
					 }
					 else if( key == 31)//s arrow scale y decrement
					 {
						scale(selected_layer,2);
						interpret();
						continue;
					 }
					 else if( key == 32)//d arrow scale x increment
					 {
						scale(selected_layer,0);
						interpret();
						continue;
					 }
					 else if( key == 16)//q arrow scale down uniform
					 {
						scale(selected_layer,5);
						interpret();
						continue;
					 }
					 else if( key == 18)//e arrow scale up uniform
					 {
						scale(selected_layer,4);
						interpret();
						continue;
					 }
					 else if( key == 44)//z Rotate Left wrt end point
					 {
						rotate(selected_layer,1);
						interpret();
						continue;
					 }
					 else if( key == 45)//x Rotate Right wrt end point
					 {
						rotate(selected_layer,0);
						interpret();
						continue;
					 }
					 else if( key == 46)//c Rotate Left wrt center
					 {
						rotate(selected_layer,3);
						interpret();
						continue;
					 }
					 else if( key == 47)//v Rotate Right wrt center
					 {
						rotate(selected_layer,2);
						interpret();
						continue;
					 }
					 else if( key == 63)//F5 Save File
					 {	delay(250);
						save_file();
						continue;
					 }
					 else if( key == 64)//F6 Load File
					 {	delay(250);
						load_file();
						continue;
					 }
					 else if( key == 59)//F1 Help Screen
					 {	delay(250);
						draw_help_screen();
						interpret();
						continue;
					 }
					 
				  }

    getmousepos(&button,&x,&y);

				  if( button == 1 )
				  {
				 if( x > (maxx-35) && x < maxx && y > 10 && y < 417 )
					{
					change_color( x, y );

					}
					
				else if( x > (maxx-80) && x < (maxx-40) && y > 10 && y < 417 )
					select_layer( x, y );
					
					
				else if( x < (maxx-85) && y > (maxy-25) && y < (maxy-5) )
					{
					select=select_icon( x, y );
						if(select==4)
						{	clear_canvas();
							select=2;
						}
						else if(select==5)
						{	delay(250);
							delete_layer(selected_layer);
							interpret();
							select=2;
						}

					}
				else
				{


		temp1 = x ;
		temp2 = y ;

		while( button == 1)
	    getmousepos(&button,&x,&y);

	 if(select==2)//Line
	    {
		store_line(temp1,temp2,x,y);
		interpret();
		}
	else if(select==1)//Circle
		{
		store_circle(temp1,temp2,x,y);
		interpret();
		}
	else if(select==3)//Fill
		{
		store_fill(x,y);
		interpret();
		}
	else if(select==0)//Rectangle
		{
		store_rectangle(temp1,temp2,x,y);
		interpret();
		}



		
   }
   }
   }
   getch();
   closegraph();

}

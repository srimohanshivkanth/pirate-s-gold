#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#define H_Extreme 150
#define V_Extreme 45
#define Bomb_Max 4
#define G_Max 10

double randm();
time_t seed;
int flag = 0;
double t;
double cm;
int grid[V_Extreme][H_Extreme],yryt=22,ylft=21,ppost=25,pposb=26,score=0;

struct goldC
{
	int ori_x,ori_y,ban;
	double x,y;
	double coefficient[5];
};

struct bomb
{
	int pos;
	struct goldC coins[G_Max];
	long int loops;
	int remain;
};

struct bomb b[Bomb_Max];

double randm()
{
    if(flag == 0)   //First time seeding
   { seed = time(NULL);
     t  = (double) seed/CLOCKS_PER_SEC;
     t= t*100;
     flag++;
    }

    double y= (double)3*sin(t); //Convergence point for firsts series

     int i,j=1,k=2,N =1000;
	double close = 0,*a;
	double sum =0;
	a = (double*)calloc(N + 1,sizeof(double));

	for(i=1;i<N;)
	{
	   for(;close <= y ;j+=2)
	   {
	     close += (double)1/j;
	     a[i-1] = (double)j;
	     i++;
	   }
	   for(;close >= y ;k+=2)
	   {
	    close += (double)-1/j;
	    a[i-1] = (double)-j;
	    i++;
	   }

	}


int con2; //Array index
con2 = abs(floor((size_t)1000*sin(y)));
if(con2>=999)
    cm = 1/a[con2%998]; //Conergence point for second series
else
    cm = -1/a[con2];
//printf("\n%lf",cm);
close =0;
j=1; k=2;
double x;

for(i=1 ; i<N;)
	{
		for (;close>=cm;j+=2)
		 {    x =  -log(j)/j;
			  close = close+x;
			  a[i-1] *= x;
			  i++;
		 }
		 for (;close<=cm; k+=2)
		  {
			  x = log(k)/k;
			  close = close+x;
			  a[i-1] *= x; //Segementation fault waala line
			  i++;
		  }



}

//Summing up
for (i=1 ; i<N; i++)
{

sum = sum + a[i];
}

t = t+sum; //incrementing value of t
return sum;
}



int kbhit(void)
{

	struct termios oldt,newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO,&oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&newt);
	oldf = fcntl(STDIN_FILENO,F_GETFL,0);
	fcntl(STDIN_FILENO,F_SETFL,oldf | O_NONBLOCK);
	ch = getc(stdin);
	tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
	fcntl(STDIN_FILENO,F_SETFL,oldf);

	if(ch != EOF)
	{
		ungetc(ch,stdin);
		return 1;
	}
	return 0;
}

void Pirate_put(int d)
{
	if(d==0)
	{
		grid[ppost][yryt]=0;
        grid[pposb][yryt]=0;
        grid[ppost][ylft]=0;
        grid[pposb][ylft]=0;
	}
	else if(d==1)
	{
		grid[ppost][yryt]=1;
    	grid[pposb][yryt]=1;
	    grid[ppost][ylft]=1;
    	grid[pposb][ylft]=1;
	}
}

void makebomb(int i)
{
	double ranVal = randm();
	int j,k,l,m;
	if(ranVal < 0)
		ranVal *= -1;
	b[i].loops = 125 ;
	b[i].remain = (int)((ranVal - (int)ranVal%1500)*1500)%G_Max;
	b[i].pos = (int)((ranVal - (int)ranVal%1500)*1500)% H_Extreme;
	grid[V_Extreme-1][b[i].pos]=3;
	//printf("\n\n%d\n ",b[i].remain);
	for(j=0;j<b[i].remain;j++)
    {
        for(k=0;k<5;k++)//Making coefficients here
        {
            ranVal = randm();
            b[i].coins[j].coefficient[k] = (ranVal - (int)ranVal%1500)*13;
            //printf("%d  %d  %d  %lf\n",i,j,k,b[i].coins[j].coefficient[k]);
        }
        ranVal = randm();
        for(m=0,k=V_Extreme-3,l=b[i].pos-1;m<b[i].remain;m++,l++)
        {
            if(l>b[i].pos+2)
            {
                l=b[i].pos-2;
                k=V_Extreme-2;
            }
            b[i].coins[m].ori_x = k;
            b[i].coins[m].ori_y = l;
            b[i].coins[m].x = 0;
            b[i].coins[m].y = 0;
            grid[k][l] = 2;
        }
    }

}

void defusebomb(i)
{
	int j,k,l,m;
	grid[V_Extreme-1][b[i].pos]=0;
	for(j=0;j<b[i].remain;j++)
    {
        for(k=0;k<5;k++)
            b[i].coins[j].coefficient[k] = 0;
        grid[(int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x][(int)floor(b[i].coins[j].y + 0.5) + b[i].coins[j].ori_y] = 0;
        b[i].coins[j].x = 0;
        b[i].coins[j].y = 0;
        b[i].coins[j].ban = 0;
        for(m=0,k=V_Extreme-3,l=b[i].pos-1;m<b[i].remain;m++,l++)
        {
            if(l>b[i].pos+2)
            {
                l=b[i].pos-2;
                k=V_Extreme-2;
            }
            grid[k][l] = 0;
        }
	}
}


void path()
{
	double xt,yt;
	int i,j,k;
	for(i=0;i<Bomb_Max;i++)
	{
		//printf("%lf,%lf\n\n",xt,yt);
		for(j=0;j<b[i].remain;j++)
		{
			if(b[i].coins[j].ban!=1)
			{
				grid[(int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x][(int)floor(b[i].coins[j].y + 0.5) + b[i].coins[j].ori_y] = 0;
				b[i].coins[j].x += -0.35;
				xt = -1*b[i].coins[j].x/40;
				for(k=0;k<5;k++)
					yt += pow(-1*xt,5-k)*b[i].coins[j].coefficient[k];
				b[i].coins[j].y = yt;
			}
		}
	}
}

void putcoins()
{
	int i,j,k;
	for(i=0;i<Bomb_Max;i++)
	{
		for(j=0;j<b[i].remain;j++)
		{
			if(b[i].coins[j].ban != 1)
			{
				if(grid[(int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x][(int)floor(b[i].coins[j].y + 0.5) + b[i].coins[j].ori_y] == 1)
				{
					b[i].coins[j].ban = 1;
					score++;
				}
				else
				{
					if((int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x >= V_Extreme)
						b[i].coins[j].ban = 1;
					else if((int)floor(b[i].coins[j].y + 0.5) + b[i].coins[j].ori_y >= H_Extreme)
						b[i].coins[j].ban = 1;
					else if((int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x < 4)
						b[i].coins[j].ban = 1;
					else
						grid[(int)floor(b[i].coins[j].x + 0.5) + b[i].coins[j].ori_x][(int)floor(b[i].coins[j].y + 0.5) + b[i].coins[j].ori_y] = 2;
				}
			}
		}
	}

}
int main()
{

    int i=0,j,movmnt,m=0;
    char ch;
    Pirate_put(1);
	for(i=0;i<Bomb_Max;i++)
		makebomb(i);

    while(m<1)
    {
        for(i=0;i<3500;i++)
            for(j=0;j<4000;j++)
           	;//printf("pr");
        for(i=0;i<4;i++)
        	if(b[i].loops <= 0 || b[i].remain == 0)
            {
            	defusebomb(i);
                 makebomb(i);
             }
        if(kbhit())
        {
            movmnt = getc(stdin);
            switch(movmnt)
            {
                case 97:	if(ylft!=1)
                        	{
                            	Pirate_put(0);
                                yryt-=1;
            			        ylft-=1;
			                    //score+=CatchCoin();
            			        Pirate_put(1);
            			   	}
            			    break;
                case 100:	if(yryt!=148)
                			{
			    	        	Pirate_put(0);
	        				    yryt+=1;
    	    				    ylft+=1;
    	    				    //score+=CatchCoin();
    	    				    Pirate_put(1);
    	    				}
            				break;
                case 115:	if(pposb!=43)
				            {
                				Pirate_put(0);
                				ppost+=1;
	            				pposb+=1;
                				//score+=CatchCoin();
    	        				Pirate_put(1);
    	        	        }
          			        break;
                case 119:	if(ppost!=2)
				    		{
								Pirate_put(0);
                	        	ppost-=1;
                	            pposb-=1;
								//score+=CatchCoin();
                		        Pirate_put(1);
                	        }
                	     	break;
               	case 27: 	return 0;
               				break;
                default:  	break;
            }
        }
        path();
        putcoins();
        //printf("\nmn\n");
        system("clear");

        printf("\n%d\n",score);
        for(i=0;i<45;i++)
        {
            for(j=0;j<150;j++)
            	//printf("%d",grid[i][j]);
               if(grid[i][j]==1)
                	printf("%c",'#');
                else if(grid[i][j]==2)
                	printf("*");
                else if(grid[i][j]==3)
                	printf("O");
                else
                    printf(" ");
            printf("\n");
        }


        for(i=0;i<Bomb_Max;i++)
        {
        	b[i].loops--;
        }

    }
 }
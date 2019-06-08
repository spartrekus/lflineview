
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
#include <stdio.h>
#define PATH_MAX 2500
#if defined(__linux__) //linux
#define MYOS 1
#elif defined(_WIN32)
#define MYOS 2
#elif defined(_WIN64)
#define MYOS 3
#elif defined(__unix__) 
#define MYOS 4  // freebsd
#define PATH_MAX 2500
#else
#define MYOS 0
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <time.h>

char string[PATH_MAX];

#define ESC "\033"
#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		printf(ESC "[2J") //clear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

static struct termios oldt;

void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void enable_waiting_for_enter(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
    struct termios newt;

    /* Make terminal read 1 char at a time */
    tcgetattr(0, &oldt);  /* Save terminal settings */
    newt = oldt;  /* Init new settings */
    newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
    tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
    atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
}



void nsystem( char *mycmd )
{
   //printf( "<SYSTEM>\n" );
   //printf( " >> CMD:%s\n", mycmd );
   //printf( "CMD:%s\n", mycmd );
   system( mycmd );
   //printf( "</SYSTEM>\n");
}



void nrunwith( char *cmdapp, char *filesource )
{
           char cmdi[PATH_MAX];
           strncpy( cmdi , "  " , PATH_MAX );
           strncat( cmdi , cmdapp , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi ,  filesource , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
           nsystem( cmdi ); 
}





int user_line_sel = 1;



///////////////////////////////////////////
int filelinecount( char *filesource )
{
   FILE *source; 
   int ch ; 
   int file_linemax = 0;
   source = fopen( filesource , "r");
   while( ( ch = fgetc(source) ) != EOF )
   {
         //printf( "%c", ch );
         if ( ch == '\n' )
           file_linemax++;
   }
   fclose(source);
}







///////////////////////////////////////////
int stringline( char *filesource , int userselline)
{
   FILE *source; 
   int ch ; 
   int file_linemax = 1; 
   int readit = 1;
   int pcc = 0; char lline[PATH_MAX];
   strncpy( lline, "", PATH_MAX );
   strncpy( string, "", PATH_MAX );
   source = fopen( filesource , "r");
   while(   ( readit == 1 ) &&  ( ch = fgetc(source) ) != EOF )
   {
         if ( file_linemax == userselline )
            printf( "%c", ch );

         if ( ch != '\n' )
            lline[pcc++]=ch;

         else if ( ch == '\n' ) 
         {
             lline[pcc++]='\0';
             if ( file_linemax == userselline )
             {
               printf( "Line: %s\n",   lline );
               strncpy( string, lline, PATH_MAX );
             }
             lline[0]='\0';
             pcc = 0;
             file_linemax++;
         }

         if ( file_linemax >= userselline+1 )
           readit = 0; 

   }
   fclose(source);
}







///////////////////////////////////////////
int printline( char *filesource , int userselline)
{
   FILE *source; 
   int ch ; 
   int file_linemax = 1; int readit = 1;
   source = fopen( filesource , "r");
   while(   ( readit == 1 ) &&  ( ch = fgetc(source) ) != EOF )
   {
         if ( file_linemax == userselline )
            printf( "%c", ch );

         if ( ch == '\n' )
           file_linemax++;

         if ( file_linemax >= userselline+1 )
           readit = 0; 

   }
   fclose(source);
}




int main( int argc, char *argv[])
{
    int i ; 
    if ( argc >= 2)
    {
          for( i = 1 ; i < argc ; i++) 
          {
	    printf( "%d/%d: %s\n", i, argc-1 ,  argv[ i ] );
	    printline( argv[ i ] , user_line_sel );
          }
    }

    int ch;
    int gameover = 0;
    FILE *fptt; 
    char strpath[PATH_MAX];
    char cwd[PATH_MAX];

    if ( argc >= 2)
    while( gameover == 0  ) 
    {
        if (   user_line_sel <= 0 ) user_line_sel = 0; 
        disable_waiting_for_enter();
        ch = getchar();
        printf( "[%d]",        user_line_sel );
        printline( argv[ 1 ] , user_line_sel );

        if      ( ch == 'Q' )         gameover = 1;
        else if ( ch == 'g' )         user_line_sel = 1; 
        else if ( ch == 'j' )         user_line_sel++;
        else if ( ch == 'k' )         user_line_sel--;
        else if ( ch == 'd' )         user_line_sel = user_line_sel +15;
        else if ( ch == 'u' )         user_line_sel = user_line_sel  -15;
        else if ( ch == 4 )           user_line_sel = user_line_sel +100;
        else if ( ch == 21 )          user_line_sel = user_line_sel -100;

        else if ( ch == 's' )         stringline( argv[ 1 ] , user_line_sel );
        else if ( ch == 'p' )         printline(  argv[ 1 ] , user_line_sel );

       else if ( ch == 'r' )
       {
          printline(  argv[ 1 ] , user_line_sel );
          stringline( argv[ 1 ] , user_line_sel );
          char cmdi[PATH_MAX];
          strncpy( cmdi , "  " , PATH_MAX );
          strncat( cmdi , " less " , PATH_MAX - strlen( cmdi ) -1 );
          strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
          strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
          strncat( cmdi ,  string , PATH_MAX - strlen( cmdi ) -1 );
          strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
          system( cmdi ); 
          printline(  argv[ 1 ] , user_line_sel );
       }

       else if ( ch == 'o' )
       {
         stringline( argv[ 1 ] , user_line_sel );
         // copy a line to clipboard
         //fputs( string , fptt );
         printf( "Running the selection.\n" );
         nrunwith( " pkill mupdf ; export DISPLAY=:0 ; screen -d -m mupdf " , string ); 
       }

       else if ( ch == 'y' )
       {
         stringline( argv[ 1 ] , user_line_sel );
         // copy a line to clipboard
         strncpy( strpath , getcwd( cwd, PATH_MAX ), PATH_MAX );
         chdir( getenv( "HOME" ) );
         fptt = fopen( ".clipboard", "wb+" );
         fputs( string , fptt );
         fputs( "\n" , fptt );
         fclose( fptt );
         chdir( strpath );
         printf( "Copying to clipboard the selection.\n" );
       }
    }

    return 0;
}




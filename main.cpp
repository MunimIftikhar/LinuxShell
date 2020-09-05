#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

extern char **environ;
extern int errno;
int findLength(string line);
char** fillArray(string theLine,int argc);
int checkPreFixes(char **argv,int argc);
int findPath();
char** splitArray1(char** argv,int argc,int &size1);
char** splitArray2(char** argv,int argc,int size1,int &size2);
char** breakArray(char** argv,int & argc,char*& file);

int main( void )
{
    string theLine;
    int argc; char** argv;char  ** argv2;int argc2=argc; char* file;int size1;int size2;char ** argv3;
    // Initialization stuff
    while (true)
    {
        cout << "Munim> ";
        getline(cin,theLine);
        cout << "Command was: " << theLine << endl;
        if ((theLine == "exit") || cin.eof())
        {
            exit(0);
        }
        argc = findLength(theLine);
        argv=fillArray(theLine, argc);
        int opt=checkPreFixes(argv,argc);
        if(opt==2)
        {
           argv2=breakArray(argv,argc2,file);
        }
        if(opt==3)
        {   
            argv2=splitArray1(argv,argc,size1);
            argv3=splitArray2(argv,argc,size1,size2);
        }
       int index=findPath();
        int pid=fork();
        if(pid==0) 
        {
            if(argv[argc-1][0]=='&')// here's problem B
            {
                argv[argc-1]=NULL;
                argc--;
                pid= fork();
                if(pid>0)exit(0);
                else
                {
                    execvp(argv[0],argv);
                    exit(1);
                }
            }
            else if(opt==2)
            {
                int array[2];
                pipe(array);
                
                pid=fork(); 
                if(pid==0)
                {
                   
                    close(array[0]);
                    int newstdout = open(file,O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
                    close(1);
                    dup(newstdout);
                    close(newstdout);
                    execvp(argv2[0],argv2);
                }
                else{ 
                    close(array[1]);
                    int newstdin = open(file,O_RDONLY);
                    close(0);
                    dup(newstdin);
                    close(newstdin);
                    int status=0;
                    wait(&status);
                    cout << "Child 1 exited with status of " << status << endl;
                    
                }
            }   
            else if (opt==3)
            {
                int array[2];
                pipe(array);
                
                pid=fork(); 
                if(pid==0)
                {
                   pid=fork();
                    if(pid==0)
                    {
                         close(array[1]);
                        int newstdin = open("file.txt",O_RDONLY);
                        close(0);
                        dup(newstdin);
                        close(newstdin);
                        execvp(argv3[0],argv3);
                    }
                    else
                    {
                        int status=0;
                        wait(&status);
                        cout << "Child 1 exited with status of " << status << endl;
                        
                    }
                }
                else{ 
                    pid=fork();
                    if(pid==0)
                    {
                        close(array[0]);
                        int newstdout = open("file.txt",O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
                        close(1);
                        dup(newstdout);
                        close(newstdout);
                        execvp(argv2[0],argv2);
                    }
                    else
                    {
                        int status=0;
                        wait(&status);
                        cout << "Child 2 exited with status of " << status << endl;
                        
                    }
                    int status=0;
                    wait(&status);
                    cout << "Child 3 exited with status of " << status << endl;
                }
            }
            else
            {  
                execvp(argv[0],argv);
            }
        }
        else if(pid>0)
        {
            int status=0;
            wait(&status);
            cout << "Child exited with status of " << status << endl;
        }
        else
        {
            cout << strerror(errno) << endl;
        }
    }
}
int findPath()
{
    int index;
    string temp="PATH";
    for(int i=0;environ[i];i++)
    {
        bool check=true;
        for(int j=0;j<temp.length();j++)
        {
            if(environ[i][j]!=temp[j])
            {
                check=false;
                break;
            }
        }
        if(check==true)
        {
            index=i;
            break;
        }
    }
    
    return index;
}
int findLength(string line)
{
    int size=line.length();int lnth=1;
    for(int i=0;i<size;i++)
    {
        if(line[i]==' ')
        {
            lnth++;
        }
    }
    return lnth;
}

char** fillArray(string theLine,int argc)
{

    char ** argv=new char*[argc+1];int i;int m=0;
    for(i=0;i<argc;i++)
    {
        int j;
        argv[i]=new char[100];
        for( j=0;m<theLine.length();j++)
        {
            if(theLine[m]!=' ')
            {
                argv[i][j]=theLine[m];
                m++;
            }
            else
            {
                m++;
                break;
            }
            
        }
        argv[i][j]='\0';
    }
    argv[i]=NULL;
    return argv;
}

int checkPreFixes(char **argv,int argc)
{
    int opt=0;
    for(int i=1;i<argc;i++)
    {
        for(int j=0;j<strlen(argv[i]);j++)
        {
            if(argv[i][j]=='<')
            {
                opt=1;
            }
            else if(argv[i][j]=='>')
            {
                opt=2;
            }
            else if(argv[i][j]=='|')
            {
                opt=3;
            }
        }
    }
    return opt;
}

char** splitArray1(char** argv,int argc,int &size1)
{
   int index=0;
    for(int i=0;i<argc;i++)
    {
        if(argv[i][0]!='|')
        {
            index++;
        }
        else
        {
            break;
        }
    }
     char ** arr=new char*[index+1];int i;
    for( i=0;i<index;i++)
    {
        arr[i]=new char[100];
        for(int j=0;j<=(strlen(argv[i]));j++)
        {
            arr[i][j]=argv[i][j];
        }
    }
    arr[i]=NULL;
    size1=index;
    return arr;
}

char** splitArray2(char** argv,int argc,int size1,int &size2)
{
    
    int size=0;
    for(int i=size1+1;i<argc;i++)
    {
        size++;
    }
     char ** argv2=new char*[size+1];int i;int m=size1+1;
    for( i=0;i<size;i++)
    {
        argv2[i]=new char[100];
        for(int j=0;j<=(strlen(argv[m]));j++)
        {
            argv2[i][j]=argv[m][j];
        }
        m++;
        
    }
    size2=size;
    argv2[i]=NULL;
    return argv2;
}

char** breakArray(char** argv,int & argc, char *& file)
{
    int size=0;
    for(int i=0;i<argc;i++)
    {
        if(argv[i][0]!='>')
        {
            size++;
        }
        else
        {
            break;
        }
    }
    char** arr=new char*[size+1];int i;
    for( i=0;i<size;i++)
    {
        arr[i]=new char[100];
            for(int j=0;j<=strlen(argv[i]);j++)
            {
                arr[i][j]=argv[i][j];
            }
            
    }
    arr[i]=NULL;
    file=new char[100];
    for(int j=0;j<=strlen(argv[i+1]);j++)
    {
        file[j]=argv[i+1][j];
    }
    cout<<file<<endl;
    size=argc;
    return arr;
}
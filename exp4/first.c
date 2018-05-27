#include <stdio.h>

#define BLOCKSIZE 1024//磁盘块大小
#define SIZE      1024000//虚拟磁盘空间大小
#define END      65535//FAT中的文件结束标志
#define FREE      0//FAT中盘块空闲标志
#define ROOTBLOCKNUM      2//根目录初始所占盘块总数
#define MAXOPENFILE 10//最多同时打开文件个数

//用于记录文件的描述和控制信息，每个文件设置一个FCB，它也是文件的目录项的内容
typedef struct FCB//仿照FAT16设置的
{
    char filename[8];//文件名
    char exname[3];//文件扩展名
    unsigned char attribute;//文件属性字段，0：目录文件；1：数据文件
    unsigned short time;//文件创建时间 
    unsigned short data;//文件创建日期
    unsigned short first;//文件起始盘块号
    unsigned long length;//文件长度（字节数）
    char free;//表示目录项是否为空，若值为0表示空，值为1表示已分配
}fcb;

typedef struct FAT//文件分配表
{
    unsigned short id;//FREE表示空闲，END表示是最后一个磁盘块，其他值表示是下一个磁盘块的盘号
}fat;

typedef struct USEROPEN//用户打开文件表
{
    char filename[8];//文件名
    char exname[3];//文件扩展名
    unsigned char attribute;//文件属性字段，0：目录文件；1：数据文件
    unsigned short time;//文件创建时间 
    unsigned short data;//文件创建日期
    unsigned short first;//文件起始盘块号
    unsigned long length;//文件长度（字节数）
    char free;//表示目录项是否为空，若值为0表示空，值为1表示已分配
    //上面内容是文件夹FCB中的内容，下面是文件使用中的动态信息
    char dir[80];//打开文件所在路径，以便快速检查指定文件是否已经打开
    int count;//读写指针的位置
    char fcbstate;//文件的FCB是否被修改，如果修改了为1,否则为0
    char topenfile;//打开表项是否为空，若值为0,表示为空，否则表示已被占用

}useropen;

typedef struct BLOCK0
{
    char information[200];//存储一些描述信息，如磁盘块大小，磁盘块数量等
    unsigned short root;//根目录文件的起始盘块号
    unsigned char *startblock;//虚拟磁盘上数据区开始位置
}block0;
unsigned char *myvhard; //指向虚拟磁盘的起始地址
useropen openfilelist[MAXOPENFILE]; //用户打开文件表数组
int curdir; //当前目录的文件描述符fd
char currentdir[80];//记录当前无怒路的目录名（包括目录的路径）
unsigned char* startp; //记录虚拟磁盘上数据区开始位置
void my_exitsys()
{
    
}

int do_read(int fd,int len,char *text)
{

}

int my_read(int fd,int len)
{

}

int do_write(int fd,char *text,int len,char wstyle)
{

}

void my_write(int fd)
{
    
}

void my_close(int fd)
{

}

int my_open(char *filename)
{

}

void my_rm(char *filename)
{

}
int my_create(char *filename)
{

}

void my_ls()
{

}

void my_rmdir(char *dirname)
{

}
void my_mkdir(char *dirname)
{

}
void mycd(char *dirname)
{

}
void my_format()
{

}
void startsys()
{
    //申请虚拟磁盘空间
    //判断是否有文件系统，没有调用format
    //读入磁盘上的文件系统内容到虚拟磁盘中
    //初始化用户打开表，将表项0分配给根目录文件，并设置根目录为当前目录
}
int main()
{
    startsys();
    return 0;
}
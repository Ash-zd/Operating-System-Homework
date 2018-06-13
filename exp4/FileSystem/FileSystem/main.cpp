#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BLOCKSIZE	 1024    //磁盘块大小
#define BLOCKNUM     1000    //磁盘块数量
#define SIZE         1024000 //虚拟磁盘空间大小
#define END          65535   //FAT中的文件结束标志
#define FREE         0       //FAT中盘块空闲标志
#define ROOTBLOCKNUM 2       //根目录初始所占盘块总数
#define MAXOPENFILE  10      //最多同时打开文件个数

//用于记录文件的描述和控制信息，每个文件设置一个FCB，它也是文件的目录项的内容
//文件控制块FCB
typedef struct FCB {
	char           filename[8]; //文件名
	char           exname[3];   //文件扩展名
	unsigned char  attribute;   //文件属性字段，0：目录文件；1：数据文件
	unsigned short time;        //文件创建时间 
	unsigned short data;        //文件创建日期
	unsigned short first;       //文件起始盘块号
	unsigned long  length;      //文件长度（字节数）
	char           free;        //表示目录项是否为空，若值为0表示空，值为1表示已分配
} fcb;

//文件分配表FAT
typedef struct FAT {
	unsigned short id; //FREE表示空闲，END表示是最后一个磁盘块，其他值表示是下一个磁盘块的盘号
} fat;

//用户打开文件表USEROPEN
typedef struct USEROPEN {
	fcb            useropen_fcb;//fcb内容
	int            dirno;       //打开文件项在父目录文件中的盘块号
	int            diroff;      //打开文件箱在父目录文件中dirno盘块号的起始位置
	char           dir[80];     //打开文件所在路径，以便快速检查指定文件是否已经打开
	int            count;       //读写指针的位置
	char           fcbstate;    //文件的FCB是否被修改，如果修改了为1,否则为0
	char           topenfile;   //打开表项是否为空，若值为0,表示为空，否则表示已被占用
} useropen;

//引导块
typedef struct BLOCK0 {
	char           information[200]; //存储一些描述信息，如磁盘块大小，磁盘块数量等
	unsigned short root;             //根目录文件的起始盘块号
	unsigned char  *startblock;      //虚拟磁盘上数据区开始位置
} block0;

//全局变量定义
unsigned char      *myvhard;                  //指向虚拟磁盘的起始地址
useropen           openfilelist[MAXOPENFILE]; //用户打开文件表数组
int                curdir;                    //当前目录的文件描述符fd
char               currentdir[80];            //记录当前目录的目录名（包括目录的路径）
unsigned char      *startp;                   //记录虚拟磁盘上数据区开始位置
unsigned char      *blockAddr[BLOCKNUM];      //虚拟磁盘各个块对应地址指针
block0             initBlock;
fat                fat1[BLOCKNUM], fat2[BLOCKNUM];

void my_format();
void startsys();
void my_exitsys();
int  my_create(char *filename);
void my_rm(char *filename);
int  my_open(char *filename);
void my_close(int fd);
void my_mkdir(char *dirname);
void my_rmdir(char *dirname);
void my_cd(char *dirname);
void my_ls();
int  my_write(int fd);
int  do_write(int fd, char *text, int len, char wstyle);
int  my_read(int fd, int len);
int  do_read(int fd, int len, char *text);

//utils
void useropen_init(useropen *openfile, int dirno, int diroff, const char* dir);
void fcb_init(fcb *new_fcb, const char* filename, unsigned short first, unsigned char attribute);
int check_fd(int fd);

int main()
{
	//初始化全局变量
	myvhard = (unsigned char *)malloc(SIZE);
	if (myvhard == NULL) {
		printf("虚拟磁盘初始化失败.\n");
		exit(-1);
	}
	for (int i = 0; i < BLOCKNUM; i++) {
		blockAddr[i] = i * BLOCKSIZE + myvhard;
	}
	for (int i = 0; i < MAXOPENFILE; i++) {
		openfilelist->topenfile = 0;
	}
	startsys();
	printf("root@localhost:%s$ ", openfilelist[curdir].dir);
	char cmd[20];
	while (scanf("%s", cmd) != EOF) {
		if (strcmp("format", cmd) == 0) {
			my_format();
		}
		else if (strcmp("mkdir", cmd) == 0) {

		}
		else if (strcmp("rmdir", cmd) == 0) {

		}
		else if (strcmp("ls", cmd) == 0) {

		}
		else if (strcmp("cd", cmd) == 0) {

		}
		else if (strcmp("create", cmd) == 0) {

		}
		else if (strcmp("open", cmd) == 0) {

		}
		else if (strcmp("close", cmd) == 0) {

		}
		else if (strcmp("write", cmd) == 0) {

		}
		else if (strcmp("read", cmd) == 0) {

		}
		else if (strcmp("rm", cmd) == 0) {

		}
		else if (strcmp("exit", cmd) == 0) {
			my_exitsys();
			return 0;
		}
		else {
			printf("%s: Command not found!\n", cmd);
		}
		printf("root@localhost:%s$ ", openfilelist[curdir].dir);
	}
	my_exitsys();
	return 0;
}

//------------------↓锅1 START↓-------------------
/**
 * 锅1中间函数
 */
void useropen_init(useropen *openfile, int dirno, int diroff, const char* dir) {
	openfile->dirno = dirno;
	openfile->diroff = diroff;
	strcpy(openfile->dir, dir);
	openfile->fcbstate = 0;
	openfile->topenfile = 1;
	openfile->count = openfile->useropen_fcb.length;
}

void fcb_init(fcb *new_fcb, const char* filename, unsigned short first, unsigned char attribute) {
	strcpy(new_fcb->filename, filename);
	new_fcb->first = first;
	new_fcb->attribute = attribute;
	new_fcb->free = 0;
	if (attribute) new_fcb->length = 0;
	else new_fcb->length = 2 * sizeof(fcb);
}
int check_fd(int fd) {
	if (!(0 <= fd && fd < MAXOPENFILE)) {
		return 0;
	}
	return 1;
}
/**
* @功能：对虚拟磁盘进行格式化，布局虚拟磁盘，建立根目录文件
* @主要完成的工作有：按照
* 1块   |2块 |2块 |995块
* 引导块|FAT1|FAT2|数据区
* 布局磁盘内容，初始化两张FAT表，初始化根目录
*/
void my_format() {
	strcpy(initBlock.information, "10101010");
	initBlock.root = 5;
	initBlock.startblock = blockAddr[5];
	for (int i = 0; i < 5; ++i) {
		fat1[i].id = END;
	}
	for (int i = 5; i < BLOCKNUM; ++i) {
		fat1[i].id = FREE;
	}
	for (int i = 0; i < BLOCKNUM; ++i) {
		fat2[i].id = fat1[i].id;
	}
	fat1[5].id = END;
	fcb root;
	fcb_init(&root, ".", 5, 0);
	memcpy(blockAddr[5], &root, sizeof(fcb));
	strcpy(root.filename, "..");
	memcpy(blockAddr[5] + sizeof(fcb), &root, sizeof(fcb));
	printf("格式化完成\n");
}

/**
 * @功能：初始化所建立的文件系统，以供用户使用
 * @主要完成的工作有：申请虚拟磁盘空间，读入磁盘上的文件系统内容到虚拟磁盘中
 * 若还没有创建文件系统，则调用format()创建。
 * 初始化用户打开文件表，将表项0分配给根目录文件，并设置根目录为当前目录。
 */
void startsys() {
	FILE *fp = fopen("myfssys", "rb");
	int need_format = 0;
	if (fp == NULL) {
		need_format = 1;
	}
	else {
		//读入虚拟磁盘信息
		unsigned char *buf = (unsigned char *)malloc(SIZE);
		fread(buf, 1, SIZE + 1, fp);
		memcpy(myvhard, buf, SIZE);
		memcpy(&initBlock, blockAddr[0], sizeof(block0));
		if (strcmp("10101010", initBlock.information) != 0) {
			need_format = 1;
		}
		free(buf);
		fclose(fp);
	}
	if (need_format == 1) {
		printf("文件系统不存在，开始进行初始化文件系统...\n");
		my_format();
	}
	else {
		memcpy(fat1, blockAddr[1], sizeof(fat1));
		memcpy(fat2, blockAddr[3], sizeof(fat2));
	}
	curdir = 0;//当前目录置为根目录0
	memcpy(&openfilelist[curdir].useropen_fcb, blockAddr[5], sizeof(fcb));
	useropen_init(&openfilelist[curdir], 5, 0, "/");
}

/**
* @功能：退出文件系统
* @主要完成的工作有：将虚拟磁盘上的内容全部写到磁盘上的指定文件中；
* 释放用户打开文件表及虚拟磁盘空间。
*/
void my_exitsys() {
	// 先关闭所有打开文件项
	for (int i = 0; i < MAXOPENFILE; ++i) {
		my_close(i);
	}
	memcpy(blockAddr[0], &initBlock, sizeof(initBlock));
	memcpy(blockAddr[1], fat1, sizeof(fat1));
	memcpy(blockAddr[3], fat1, sizeof(fat1));
	FILE *fp = fopen("myfssys", "wb");
	fwrite(myvhard, BLOCKSIZE, BLOCKNUM, fp);
	free(myvhard);
	fclose(fp);
}

/**
* @功能：创建名为filename的新文件
* @主要完成的工作有：在父目录中间差新文件是否重名；
* 为新文件分配一个空间目录项；
* 在用户打开文件表中分配一个空闲表项；
* 为新文件分配一个空闲磁盘块，建立目录项。
*/
int my_create(char *filename) {
	return 0;
}

/**
* @功能：删除名为filename的文件
* @主要完成的工作有：检查欲删除文件是否存在；
* 若存在，则回收其磁盘块，并从父目录文件中删除其目录项。
*/
void my_rm(char *filename) {

}

/**
* @功能：打开当前目录或指定目录下名为filename的文件
* @主要完成的工作有：检查制定文件是否已经打开、是否存在；
* 若没有打开且存在，则分配一个空闲打开文件表项并填写相关内容
* 表项序号即为文件描述符fd
*/
int my_open(char *filename) {
	return 0;
}

/**
* @功能：关闭之前有my_open()打开的文件描述符为df的文件
* @主要完成的工作有：检查fd的有效性；检查其FCB是否更改过
* 如果是则将修改保存到父目录文件中；清空其用户打开文件表表项。
*/
void my_close(int fd) {
	if (!check_fd(fd))
		return;
	if (openfilelist[fd].topenfile == 0)
		return;

	if (openfilelist[fd].fcbstate == 1) {

	}
	openfilelist[fd].topenfile = 0;
	return;

}
//------------------↑锅1 END↑-----------------

//------------------↓锅2 START↓---------------
/**
* @功能：在当前目录或指定目录下创建名为dirname的子目录
* @主要完成的工作有：读入当前目录或指定目录文件内容到内存，检查新建目录文件是否重名
* 若没有重名，则分配磁盘空间，建立目录项
* 最后初始化新建的目录文件，在其中建立"."和".."两个特殊目录项。
*/
void my_mkdir(char *dirname) {

}

/**
* @功能：在当前目录或指定目录下删除名为dirname的子目录
* @主要完成的工作有：在当前目录或指定目录文件中检查欲删除目录文件是否存在；
* 若存在，再检查其是否为空，若为空则回售该目录文件所占据的磁盘块
* 删除其目录项，左后修改其父目录文件相关内容
*/
void my_rmdir(char *dirname) {

}

/**
 * @功能：改变当前目录到指定的名为dirname的目录
 * @主要完成的工作有：打开病毒如新的当前目录文件到内存，关闭原当前目录文件
 * 将curdir设置为新当前目录文件的fd，并更新currentdir[]中内容
 */
void my_cd(char *driname) {

}

/**
 * @功能：显示当前目录的内容（子目录和文件信息）
 * @主要完成的工作有：将当前目录文件所有内容，按照一定格式显示到屏幕上。
 */
void my_ls() {

}
//------------------↑锅2 END↑-----------------


//------------------↓锅3 END↓-----------------
/**
 * @功能：将用户通过建涅槃输入的内容写到fd所指定的文件中
 * 磁盘文件的读写操作都不许以完整的数据块为单位进行。
 * 在写操作时，先将数据写在缓冲区中，缓冲区的大小与磁盘块的大小相同，然后再将缓冲区中的数据一次性写到磁盘块中
 * 读出时先讲一个磁盘块中的内容读到缓冲区中，然后再传送到用户区。
 */
int my_write(int fd) {
	return 0;
}

/**
 * @功能：将缓冲区中的内容写到指定文件中
 * @主要完成的工作有：将读写指针转化为逻辑块块号和块内偏移off，并进一步得到其磁盘号；
 * 申请空闲缓冲区与磁盘块一样大，将临时存储区中的数据转存到缓冲区，将缓冲区的内容写到相应的磁盘块中
 * 若写入内容超出一个块，则重复上述过程，直到写完。
 */
int do_write(int fd, char *text, int len, char wstyle) {
	return 0;
}

/**
 * @功能：读出指定文件中从读写指针开始的长度为len的内容到用户空间中
 * @主要完成的工作有：检查fd的有效性，调用do_read()完成实际读操作
 */
int my_read(int fd, int len) {
	return 0;
}

/**
 * @功能：读出指定文件种从读写指针开始的长度为len的内容到用户空间的text中
 * @主要完成的工作有：申请空闲缓冲区，将读写指针转化为逻辑块块号及块内偏移量off，
 * 并进一步得到其磁盘块号；将该磁盘块整块内容读入缓冲区中；再将从off开始的缓冲区的内容复制到text[]中
 * 若读入内容超出一个块，则重复前面过程直到读完。
 */
int do_read(int fd, int len, char *text) {
	return 0;
}
//------------------↑锅3 END↑-----------------
#include <iostream>
#include <string>

#include "../zlib/zlib.h"
#include "../zlib/GZipHelper.h"


#define GZ_BUF_SIZE 1024

bool gzLoad(char* gzfn, std::string &out)
{
	//open .gz file
	gzFile gzfp = gzopen(gzfn,"rb");
	if(!gzfp)
	{
		return false;
	}

	//read and add it to out
	unsigned char buf[GZ_BUF_SIZE];
	int have;
	while( (have = gzread(gzfp,buf,GZ_BUF_SIZE)) > 0)
	{
		out.append((const char*)buf,have);
	}

	//close .gz file
	gzclose(gzfp);
	return true;
}

int main(int argc,char** argv)
{
// 	if(argc!=2)
// 	{
// 		::std::cout<<"Usage: ./test xxx.gz"<<::std::endl;
// 		return -1;
// 	}

	std::string out;
	GZIP buff[260] = {0};
	//if(gzLoad(argv[1],out))
	char szpath[] = "D:\\0000000\\251715124.js.gz\0";
	//gzFile gzfp = gzopen(szpath,"wb");
	//gzwrite((gzFile)buff, "testtest", strlen("testtest"));
	//gzclose(gzfp);
	CA2GZIP aa("testtest");

	CGZIP2A a(aa.pgzip, aa.Length);
	size_t count=strlen(a.psz);

	if(gzLoad(szpath,out))
	{
		::std::cout<<out<<::std::endl;
	}
	else
	{
		::std::cout<<"gzLoad Error"<<::std::endl;
	}
	return 0;
}
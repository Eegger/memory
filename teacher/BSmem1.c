/*
 基本思路：
 1.完成所有内存页面的分配
 2.以单个页面（每个页面大小为4096kb）为单位,写入内容temp*(j%256);
 3.连续读取所有页面，一共读取60遍，在每遍的读取中,如果有出错页面，就记录出错页面地址，然后继续读取，获得最后一个出错页面的地址，从而确定在整个地址范围中，出错页面的地址范围。
 4.如果在某一遍的读取中，有错误出现，FirstAdress不为0，那么输出错误首页的内容，并且中断读取。
*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>
//实验室环境下的地址范围
#define StartAddress 4294967296 //十六进制为0x100000000.设置宏定义，记录要测试的第一页的地址，十进制形式。
#define EndAddress 6442450944   //0x180000000 标识要测试的最后一页地址
//模拟器下的地址范围
//#define StartAddress 1157627904//0x45000000
//#define EndAddress 1207959552 //0x48000000

EFI_STATUS EFIAPI
ReadAfterAllocateAndWrite(){

	EFI_STATUS Status = 0;

    EFI_PHYSICAL_ADDRESS pages = StartAddress;//赋初始值为StartAddress，表示要操作内存的起始物理地址。
	int j = 0;
	
    //分配指定地址范围内的页面并且写入内容要页面中，从每个页面的起始地址开始写入，每个字节写入的内容相同
    while(pages<EndAddress){//EndAddress表示物理地址的最大范围。   
		//分配指定起始地址的一个页面
    	Status = gBS->AllocatePages(AllocateAddress, EfiBootServicesData, 1, &pages);
    	if (Status == 0)
    	{
    		//页面分配成功，向页面写入内容
    		UINT64* data;
            data = (UINT64*) pages;
            j=0;
            unsigned long long temp = 0x0101010101010101;
            while(j<512){
                data[j] = temp*(j%256);
                j++;
            }
    	}
    	else{
    		Print(L"AllocateAddress failed!:%r %lx\n", Status, pages);
    	}
    	pages+=4096;
    }

    Print(L"Allocate and write finished!\n");
    
    Print(L"Test Address 0x%lx~0x%lx\n",StartAddress,EndAddress);
    //读取页面信息，输出测试结果
    int k = 0;
    int readTimes = 1;
    EFI_PHYSICAL_ADDRESS FirstAddress = 0;
    EFI_PHYSICAL_ADDRESS LastAddress = 0;
   
    Print(L"Test Result Is Following:\n");
    while(readTimes<=60){//连续读取60遍，测试读到的内容是否正确
        pages = StartAddress;//赋初始值为StartAddress，表示要内存的起始物理地址。
        while(pages<EndAddress){//表示物理地址的最大范围。  
            if(Status == 0){
                
                UINT64* data;
                data = (UINT64*)pages;
                
                unsigned long long temp = 0x0101010101010101;
                
                k = 0;
                while(k<512){
                    if(data[k] != (temp*(k%256))){
            
                        if(FirstAddress==0)
                        //记录有读写错误的第一页的页面地址,只被写入一次，记录第一页
                        FirstAddress = pages;
                        //记录有读写错误的最后一页的页面地址，连续被覆盖，最终记录最后一页
                        LastAddress = pages;
                    }
                    k++;
                }
            }
            pages+=4096;
        }

        Print(L"The %2d time     ",readTimes);
        //输出
        if (FirstAddress!=0)
        {
            //输出有出错内容的页面范围
            Print(L"FirstAddress 0x%lx\n",FirstAddress);
            Print(L"LastAddress 0x%lx\n",LastAddress);
        
            //输出第一个出错页面的内容
            UINT64* str1;
            str1 = (UINT64*)FirstAddress;
            k = 1;
            while(k<=512){
            
                Print(L"%016lx ",str1[k-1]);
                if(k%4==0){
                    Print(L"\n");
                }
                k++;
            }
            break;//不再继续读取
        }
        else{
            Print(L"right\n");
            readTimes++;//继续下一遍的测试
        }

    }
    
    //用来测试，输出第一页的内容，实际实验时候，要注释掉
    // Print(L"\nPrint Address 0x%lx\n",StartAddress);
    // pages = StartAddress;
    // UINT64* data;
    // data = (UINT64*) pages;
    // j=1;
    // while(j<=512){
    //     Print(L"%016lx ",data[j-1]);
    //     if (j%4==0)//每行输出4个
    //     {
    //         Print(L"\n");
    //     }
    //     j++;
    // }
    
    //释放内存页面
    pages = StartAddress;//赋初始值为StartAddress，表示要内存的起始物理地址。
    while(pages<EndAddress){
        Status = gBS->FreePages(pages,1);
        pages+=4096;
    }

	return Status;
}

//主函数
EFI_STATUS EFIAPI
UefiMain (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
 
    EFI_STATUS Status = 0;
    Status = ReadAfterAllocateAndWrite();
    return Status;
}

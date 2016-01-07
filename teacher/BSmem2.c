/*
 * 为了保证输出的是第一次读取到的内容，设置临时数组，保存读取到的内容，最后输出临时数组中的内容。
 * 思路：用临时数组保存第一页读到的内容，然后输出临时数组中的内容。
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
    int i = 0; 
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
    
   //用临时数组Test保存读到的内容 
    j=0;
    UINT64 Test[512];
    //初始化数组
    while(j<512){
        Test[j] = 0;
        j++;
    }
    //读取内存页面，并赋值，并复制给临时变量。
    pages = StartAddress;//输出第一页
    UINT64* data;
    data = (UINT64*) pages;
    j = 0;
    while(j<512){
        
        Test[j] = data[j];
        j++;
    }

    Print(L"Test Address 0x%lx\n",pages);
    //打印输出临时数组Test中的内容
    i=1;
    while(i<=512){
        Print(L"%016lx ",Test[i-1]);

        if(i%4==0){
            Print(L"\n");
        }
        i++;
    }
   
   //释放分配的内存页面
    pages = StartAddress;
    while(pages<EndAddress){

        Status = gBS->FreePages(pages,1);
        pages+=4096;
    } 

	return Status;
}

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

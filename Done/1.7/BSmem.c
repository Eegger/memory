/*
 * 为了保证输出的是第一次读取到的内容，设置临时数组，保存读取到的内容，最后输出临时数组中的内容。
 * 下面的程序给出思路：
 * 1.根据地址范围分配大小为4kb的内存页面。
 * 2.将每个页面看成大小是4096字节的数组，并向页面中写入内容
 * 3.开始进行测试，预计将所有内存页连续读取60遍
 * 4.在读取的过程中，如果发现某个页面有出错的内容，停止继续读取，记录下第一次出错的页面的地址
 * 5.对于每次对所有页面的测试，在判断是否有错误之前，将该页面的全部内容读入到临时数组变量中保存，临时数组中的内容对于每个页面都会被覆盖，知道该页面有错误出现时候才会停止。
 * 6.最后输出临时数组中的内容，也就是出现错误的第一个页面的内容。
*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>

//实验室环境下的地址范围
//#define StartAddress 4294967296 //十六进制为0x100000000.设置宏定义，记录要测试的第一页的地址，十进制形式。
//#define EndAddress 6442450944   //0x180000000 标识要测试的最后一页地址
//模拟器下的地址范围
#define StartAddress 1157627904//0x45000000
#define EndAddress 1207959552 //0x48000000

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
    
    Print(L"Test: Address 0x%lx~0x%lx\n",StartAddress,EndAddress);
    //读取页面信息，输出测试结果
    int k = 0;
    int readTimes = 1;
    EFI_PHYSICAL_ADDRESS FirstAddress = 0;
    //EFI_PHYSICAL_ADDRESS LastAddress = 0;
   
   //初始化临时数组
    UINT64 Test[512];
    k = 0;
    while(k<512){
        Test[k] = 0;
        k++;
    }
    
    Print(L"Test Result Is Following:\n");
    while(readTimes<=60){//连续读取60遍，测试读到的内容是否正确
        pages = StartAddress;//赋初始值为StartAddress，表示要内存的起始物理地址。
        int index = 0;
        while(pages<EndAddress){//表示物理地址的最大范围。  
            if(Status == 0){
                
                UINT64* data;
                data = (UINT64*)pages;
                
                unsigned long long temp = 0x0101010101010101;
                k = 0;
                while(k<512){

                    Test[k] = data[k];//每次读取判断之前，都将读到的内容读入到临时数组中
                    if(data[k] != (temp*(k%256))){
            
                        if(FirstAddress==0)
                        //记录有读写错误的第一页的页面地址,只被写入一次，记录第一页
                        FirstAddress = pages;
                        //记录有读写错误的最后一页的页面地址，连续被覆盖，最终记录最后一页
                        //LastAddress = pages;
                    }
                    k++;
                }
                
            }


            if(FirstAddress!=0){
                index = 1;
                Print(L"FirstAddress 0x%lx\n",FirstAddress);//输出出错的页面地址
                break;//中断继续向后遍历页面
            }
            else{
                pages+=4096;//继续下一页
            }
        }

        if(index!=0){
             break;//如果已经找到FirstAddress,index不是初始值，则不再进行下一遍的测试
        }
        else{
            Print(L"The  %2d time is right\n",readTimes);
            readTimes++;//继续进行下一遍                 
        }
    }

    //当存在出错页面的时候，临时数组Test中保存的正好是第一个出错页面的内容，打印输出
    if(FirstAddress!=0){
        i=1;
        while(i<=512){
            Print(L"%016lx ",Test[i-1]);
             if(i%4==0){
                 Print(L"\n");
            }
            i++;
        }
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

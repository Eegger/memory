#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>

EFI_STATUS EFIAPI
ReadAfterAllocateAndWrite(){

	EFI_STATUS Status = 0;

	EFI_PHYSICAL_ADDRESS pages = 1157627904;//0x45000000
    //EFI_PHYSICAL_ADDRESS pages = 6442450944;//赋初始值为0x180000000，表示要操作内存的起始物理地址。
    //EFI_PHYSICAL_ADDRESS pages = 4294967296;//赋初始值为0x100000000，表示要操作内存的起始物理地址。
	//int NumberOfPage = 0;
	int j = 0;
	
    //分配指定地址范围内的页面并且写入内容要页面中，从每个页面的起始地址开始写入，每个字节写入的内容相同
    //while(NumberOfPage<10){
    //while(pages<6442450944){//0x180000000，表示物理地址的最大范围。   
    while(pages<1207959552){//0x48000000，表示物理地址的最大范围。

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

            //Status = gBS->FreePages(pages,1);
            //Print(L"Test Str:%c\n", str[0]);
            //if not Free the page,allocate pages will fail!
            //Status = gBS->FreePages(pages,1);
    	}
    	else{
    		Print(L"\nAllocateAddress failed!:%r %lx\n", Status, pages);
    	}

    	pages+=4096;
    	//NumberOfPage++;
    }

    Print(L"Allocate and write finished!\n");
    // Print(L"Address 0x100000000\n");
    // pages = 4294967296;
    // UINT64* data;
    // data = (UINT64*) pages;
    // j=1;
    // while(j<=160){

    //     Print(L"%lx ",data[j-1]);
    //     if (j%4==0)
    //     {
    //         Print(L"\n");
    //     }
    //     j++;
    // }

    
    Print(L"Test Address 0x100000000~0x180000000\n");
    Print(L"Test Address 0x45000000~0x48000000\n");
    //读取页面信息，输出测试结果
    //int NumberOfTest = 0;
    int k = 0;
    //int ReadWrongNumber = 0;
    int readTimes = 1;
    EFI_PHYSICAL_ADDRESS FirstAddress = 0;
    EFI_PHYSICAL_ADDRESS LastAddress = 0;
    
    //while(NumberOfTest<10){
    while(readTimes<=60){

       // pages = 4294967296;//赋初始值为0x180000000，表示要内存的起始物理地址。
        pages = 1157627904;//0x45000000
        //while(pages<6442450944){//0x280000000，表示物理地址的最大范围。  
        while(pages<1207959552){//0x4800000
            if(Status == 0){
                INT64* data;
                data = (INT64*)pages;
                
                unsigned long long temp = 0x0101010101010101;
                //BeginAddress
                k = 0;
                while(k<512){
                    if(data[k] != (temp*(k%256))){
                        //ReadWrongNumber++;
                        if(FirstAddress==0)
                        //记录有读写错误的第一页的页面地址
                        FirstAddress = pages;
                        //记录有读写错误的最后一页的页面地址
                        LastAddress = pages;
                    }
                    k++;
                }
            }
            pages+=4096;
            // NumberOfTest++;
        }

        Print(L"read %d times\n",readTimes);
        readTimes++;

        Print(L"Test Result is Following:\n");

        FirstAddress = 1157627904;
        if (FirstAddress!=0)
        {
            Print(L"FirstAddress 0x%lx\n",FirstAddress);
            Print(L"LastAddress 0x%lx\n",LastAddress);
            //Print(L"The Different times is %d\n",ReadWrongNumber);
           
            UINT64* str1;
            
            str1 = (UINT64*)FirstAddress;
            k = 1;
            while(k<=160){
            
                Print(L"%016lx ",str1[k-1]);
                if(k%4==0){
                    Print(L"\n");
                }
                k++;
            }

            break;
        }
        else{
            Print(L"Amazing!everything is right!");
        }

    }
    
    //pages = 4294967296;//赋初始值为0x180000000，表示要内存的起始物理地址。
    pages = 1157627904;//0x4500000
    //while(pages<6442450944){
    while(pages<1207959552) {

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

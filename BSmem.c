#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>

EFI_STATUS EFIAPI
ReadAfterAllocateAndWrite(){

	EFI_STATUS Status = 0;

	//EFI_PHYSICAL_ADDRESS pages = 1157627904;//0x45000000
    EFI_PHYSICAL_ADDRESS pages = 6442450944;//赋初始值为0x180000000，表示要操作内存的起始物理地址。
	//int NumberOfPage = 0;
	int j = 0;
	//allocate pages and write into all the pages
    //while(NumberOfPage<10){
    while(pages<10737418240){//0x280000000，表示物理地址的最大范围。   

		//Allocate Address
    	Status = gBS->AllocatePages(AllocateAddress, EfiBootServicesData, 1, &pages);
    	if (Status == 0)
    	{
    		//write into the page space
    		CHAR8* str;
            str = (CHAR8*) pages;
            j=0;
            while(j<4096){

                str[j] = 'A'+j%26;
                j++;
            }

            //Status = gBS->FreePages(pages,1);
            //Print(L"Test Str:%c\n", str[0]);
            //if not Free the page,allocate pages will fail!
            //Status = gBS->FreePages(pages,1);
    	}
    	else{
    		Print(L"AllocateAddress failed!:%r %lx\n", Status, pages);
    	}

    	pages+=4096;
    	//NumberOfPage++;
    }

    Print(L"Allocate and write finished!\n");
    //allocate pages and test
    //int NumberOfTest = 0;
    int k = 0;
    int ReadWrongNumber = 0;

    

    int readTimes = 1;
    EFI_PHYSICAL_ADDRESS ResultAddress = 0;
    //while(NumberOfTest<10){
    while(readTimes<=6){

        pages = 6442450944;//赋初始值为0x180000000，表示要内存的起始物理地址。

        while(pages<10737418240){//0x280000000，表示物理地址的最大范围。  

            if(Status == 0){
                CHAR8* str;
                str = (CHAR8*)pages;
                k = 0;
                while(k<4096){
                    if(str[k] != ('A'+k%26)){
                        ReadWrongNumber++;
                        ResultAddress = pages+k+1;
                        break;
                    }
                    // else{
                    //     Print(L"Read Test Str:%c\n", str[1]);
                    // }
                    k++;
                }

           // Status = gBS->FreePages(pages,1);
            }
            pages+=4096;
            // NumberOfTest++;
        }


        Print(L"read %d times\n",readTimes);
        readTimes++;

        if (ReadWrongNumber!=0)
        {
            break;
        }
    }
    
    pages = 6442450944;//赋初始值为0x180000000，表示要内存的起始物理地址。
    while(pages<10737418240){

        Status = gBS->FreePages(pages,1);
        pages+=4096;
    }

    //FreePages 
    // while(NumberOfTest<10){

    // 	Status = gBS->AllocatePages(AllocateAddress,EfiBootServicesData,1,&pagesForTest);
    // 	if (Status == 0)
    // 	{
    // 		CHAR8* str;
    //         str = (CHAR8*) pages;
    //         k = 0;

    //         Print(L"Test Str:%c\n", str[0]);

    //         while(k<4096){

    //         	if(str[k] != ('A'+k%26)){
    //         		ReadWrongNumber++;
    //         	}
    //         	k++;
    //         }

    //         //Free the page which have been test!
    //         Status = gBS->FreePages(pagesForTest,1);
    // 	}
    // 	else{
    // 		Print(L"Read AllocateAddress failed!:%r %lx\n", Status, pagesForTest);
    // 	}

    // 	pagesForTest+=4096;
    // 	NumberOfTest++;
    // }

    
    //Print(L"After Read %d times!\n",(readTimes-1));

    Print(L"Test Result is Following:\n");
    if(ReadWrongNumber == 0){
    	Print(L"Amazing!everything is right!");
    }
    else{
        Print(L"ResultAddress %lx\n",ResultAddress);
    	Print(L"The Different times is %d\n",ReadWrongNumber);
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
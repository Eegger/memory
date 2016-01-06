#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>

EFI_STATUS EFIAPI
ReadAfterAllocateAndWrite(){

	EFI_STATUS Status = 0;

	//EFI_PHYSICAL_ADDRESS pages = 1157627904;//0x45000000
    //EFI_PHYSICAL_ADDRESS pages = 6442450944;//赋初始值为0x180000000，表示要操作内存的起始物理地址。
    EFI_PHYSICAL_ADDRESS pages = 4294967296;//赋初始值为0x100000000，表示要操作内存的起始物理地址。
	//int NumberOfPage = 0;
	int j = 0;
	//allocate pages and write into all the pages
    //while(NumberOfPage<10){
    while(pages<6442450944){//0x180000000，表示物理地址的最大范围。   

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
    		Print(L"\nAllocateAddress failed!:%r %lx\n", Status, pages);
    	}

    	pages+=4096;
    	//NumberOfPage++;
    }

    Print(L"Allocate and write finished!\n");
    Print(L"Address 0x100000000\n");
    pages = 4294967296;
    CHAR8* str;
    str = (CHAR8*) pages;
    j=1;
    while(j<=520){

        Print(L"%c ",str[j-1]);
        if (j%26==0)
        {
            Print(L"\n");
        }
        j++;
    }

    
    Print(L"Test Address 0x100000000~0x180000000\n");
    //allocate pages and test
    //int NumberOfTest = 0;
    int k = 0;
    //sint ReadWrongNumber = 0;
    int readTimes = 1;
    EFI_PHYSICAL_ADDRESS FirstAddress = 0;
    EFI_PHYSICAL_ADDRESS LastAddress = 0;
    //while(NumberOfTest<10){
    while(readTimes<=60){

        pages = 4294967296;//赋初始值为0x180000000，表示要内存的起始物理地址。

        while(pages<6442450944){//0x280000000，表示物理地址的最大范围。  

            if(Status == 0){
                CHAR8* str;
                str = (CHAR8*)pages;
                
                //BeginAddress
                k = 0;
                while(k<4096){
                    if(str[k] != ('A'+k%26)){
                        //ReadWrongNumber++;
                        if(FirstAddress==0)
                        FirstAddress = pages;

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

        FirstAddress = 4294967296;
        if (FirstAddress!=0)
        {
            Print(L"FirstAddress 0x%lx\n",FirstAddress);
            Print(L"LastAddress 0x%lx\n",LastAddress);
            //Print(L"The Different times is %d\n",ReadWrongNumber);
           
            CHAR8* str1;
            
            str1 = (CHAR8*)FirstAddress;
            k = 1;
            while(k<=520){
            
                Print(L"%c ",str1[k-1]);
                if(k%26==0){
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
    
    pages = 4294967296;//赋初始值为0x180000000，表示要内存的起始物理地址。
    while(pages<6442450944){

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


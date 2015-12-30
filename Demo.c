#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <stdio.h>
#include <stdlib.h>

EFI_STATUS EFIAPI
ReadAfterAllocateAndWrite(){

	EFI_STATUS Status = 0;

	EFI_PHYSICAL_ADDRESS pages = 1157627904;//0x45000000
	int NumberOfPage = 0;
	int j = 0;
	//allocate pages and write into all the pages
    while(NumberOfPage<10){

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
    	}
    	else{
    		Print(L"AllocateAddress failed!:%r %lx\n", Status, pages);
    	}

    	pages+=4096;
    	NumberOfPage++;
    }

    //allocate pages and test
    int NumberOfTest = 0;
    EFI_PHYSICAL_ADDRESS pagesForTest = 1157627904;//0x45000000
    int k = 0;
    int ReadWrongNumber = 0;

    while(NumberOfTest<10){

    	Status = gBS->AllocatePages(AllocateAddress,EfiBootServicesData,1,&pagesForTest);
    	if (Status == 0)
    	{
    		CHAR8* str;
            str = (CHAR8*) pages;
            k = 0;
            while(k<4096){

            	if(str[k] != ('A'+k%26)){
            		ReadWrongNumber++;
            	}
            	k++;
            }
    	}
    	else{
    		Print(L"AllocateAddress failed!:%r %lx\n", Status, pagesForTest);
    	}
    	pagesForTest+=4096;
    	NumberOfTest++;
    }

    Print(L"\nTest Result is Following:\n");
    if(ReadWrongNumber == 0){
    	Print(L"Write and Read is total Same");
    }
    else{
    	Print(L"Different times is %d\n",ReadWrongNumber);
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
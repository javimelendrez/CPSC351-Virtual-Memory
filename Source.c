#include <stdio.h>
//function to get page
size_t getPage(size_t x, size_t len, size_t n) {
    return x >> (len - n);
}
//function to get offset
size_t getOffset(size_t x, size_t len, size_t n) {
    size_t mask = ~(~0 << n);
    return x & mask;
}
#pragma warning (disable : 4996) //needed this for windows for fopen said it was unsafe so had to disable the error code
unsigned int pageTable[256]; //page table
//i have to arrays to represent the physical addresss and the value to the physical address
unsigned physicalMemoryAddress[256]; //physical address are stored here
signed char physicalMemoryValue[256]; //physical address memory values are stored here
//unsigned int TLB[16][2];
//initialize the page
void initializePageTable() {
    for (size_t i = 0; i < 256; i++)
    {
        pageTable[i] = -1; //all indexes must be initialized to -1
        //might as well the address
        physicalMemoryAddress[i] = 0;
        //physicalMemoryValue[i] = 0;
    }
}
//main
int main(int argc, const char * argv[]) {
    initializePageTable();
    //open and read from file
    FILE* file = fopen("addresses.txt", "r");
    
    if(file == 0) {
        perror("fopen");
        exit(1);
    }
    FILE* fileBin = fopen("BACKING_STORE.bin", "rb");
    int i = 0; //wild hold the addresss
    int page = 0, offset = 0; //will hold the page and offset
    int frameNumber = 0;
    int physicalAddress = 0; //physical address
    float pageFault = 0; //hold amount of page faults and then the percentage
    fscanf(file, "%d", &i);
    //loop to read in the file
    while (!feof(file))
    {
        //get the page number and the offset
        page = getPage(i, 16, 8);
        offset = getOffset(i, 16, 8);
        //with the given page number go into the page table and assign the next free
        //frame #
        if (pageTable[page] == -1)
        {
            
            pageTable[page] = frameNumber;
            //read from backing store since it was page fault
            //fseek(fp, 0, SEEK_END);
            fseek(fileBin, page*256, SEEK_SET);
            //store it in physical memory
            fread(physicalMemoryValue,sizeof(signed char),256,fileBin);
            //save physical address
            physicalAddress = frameNumber * 256 + offset;
            physicalMemoryAddress[frameNumber] = physicalAddress;
            
            printf("logical: %5u (page:%3u, offset:%3u) ---> physical: %5u value: %d\n", i, page, offset, physicalAddress, physicalMemoryValue[frameNumber]);
            
            //increase the next available frameNumber
            frameNumber++;
            pageFault++;
        } //not page fault no need for getting value from the bin
        else if (pageTable[page] != -1) {
            //get the location of the physical address
            int tempFrame = pageTable[page]; //get that frame number that was for that entry
            int tempValue = physicalMemoryValue[tempFrame]; //go into the physical address and get the value that was already signed for that location
            physicalAddress = tempFrame * 256 + offset;
            printf("logical: %5u (page:%3u, offset:%3u) ---> physical: %5u value: %d\n", i, page, offset, physicalAddress, physicalMemoryValue[tempFrame]);
        }
        
        fscanf(file, "%d", &i); //scan again
    }
    pageFault = (pageFault / 1000) *100; //divide by thousand because amount of
    //close the file
    fclose(file);
    fclose(fileBin);
    printf("Page Fault Rate = %.2f percent\n", pageFault);
    //system("pause"); //use to pause the terminal
    
    return 0;
}

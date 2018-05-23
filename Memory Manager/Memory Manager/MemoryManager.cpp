    #include "MemoryManager.h"
#include <stdio.h>
#include <iostream>
namespace MemoryManager
{
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
  //
  // This is the only static memory that you may use, no other global variables may be
  // created, if you need to save data make it fit in MM_pool
  //
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT


  const int MM_POOL_SIZE = 65536;
  char MM_pool[MM_POOL_SIZE];
    
    
    int getLeftDistanceFromFlag(char* aPointer)
    {//set aPointer to beginning of Flag and it returns the distance to the left nearest flag.
        int number = 0;
        for (int i = 0; i < 5; i++){
            int tenPower = 1;
            for( int k = i + 1; k <5; k++){
                tenPower *= 10;
            }
            int intDig = (int)*aPointer - 48;
            aPointer++;
            number += intDig * tenPower;
        }
        return  number;
        
    }
    char getLeftSymbolFromFlag(char* aPointer)
    {//set aPointer to the beginning of Flag and it returns the status of files to the left
        aPointer += 5;
        char symbol;
        symbol = *aPointer;
        return symbol;
    }
    
    int getRightDistanceFromFlag(char* aPointer)
    {//set aPointer to beginning of Flag and it returns the distance to the right nearest flag.
        aPointer += 6;
        int number = 0;
        for (int i = 0; i < 5; i++){
            int tenPower = 1;
            for( int k = i + 1; k <5; k++){
                tenPower *= 10;
            }
            int intDig = (int) *aPointer - 48;
            aPointer++;
            number += intDig * tenPower;
        }
        return  number;
        
    }
    
    char getRightSymbolFromFlag(char* aPointer)
    {//set aPointer to the beginning of Flag and it returns the status of files to the right
        aPointer += 11;
        char symbol;
        symbol = *aPointer;
        return symbol;
    }

    void writeNumber(int number, int number_Scale, char* aPointer)
    {//writes (number) into (number_Scale) digits at (aPointer)
        char digits[5];
        for (int i = 0; i < number_Scale; i++){
            int tenPower = 1;
            for(int k = i + 1; k < number_Scale; k++){
                tenPower *= 10;
            }
            int intDig = ((number - (number % tenPower)) / tenPower) % 10;
            digits[i] = intDig + 48;
            *aPointer = digits[i];
            aPointer++;
        }
    }
    
    void* locate(int size)
    {//looks at each right flag to see if it is available and if it is big enough to fit (size)
        if (size == -1)
        {
            return &MM_pool[0];
        }
        char* aPointer;
        aPointer= &MM_pool[0];
        for(int i = 0; i < 100; i++)
        {
            int distance = getRightDistanceFromFlag(aPointer);
            
            if(distance - 12 > size & getRightSymbolFromFlag(aPointer) == 'a')
            {
                return aPointer + 12;
                break;
            }
            else{
                aPointer += distance;
            }
        }
        return &MM_pool[0];
    }
    
    
    void createFlag(int left, char left_Status, int right, char right_Status, char* aPointer)
    {//writes flag at point in ######a######a format
        //if left_Status or right_Status is not b,e,u, or a, then ERROR
        //number_Scale determines how many digits each number consists of.  (4 with 3 digits is '004')
        int number_Scale = 5;
        //if left or right are greater than 10^number_Scale then ERROR
        //change 2 to 5 for final
        writeNumber(left, number_Scale, aPointer);
        aPointer += number_Scale;
        *aPointer = left_Status;
        aPointer ++;
        writeNumber(right, number_Scale, aPointer);
        aPointer += number_Scale;
        *aPointer = right_Status;
    }
    
    // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {//places a flag at the beginning and end of the MMpool
      int flagSize = 12;
      char* aPointer;
      aPointer = &MM_pool[0];
      createFlag(0, 'b', MM_POOL_SIZE - flagSize, 'a', aPointer);
      aPointer += MM_POOL_SIZE - flagSize;
      createFlag(MM_POOL_SIZE - flagSize, 'a', 0, 'e', aPointer);
  }
    

  // return a pointer inside the memory pool
  // If no chunk can accommodate aSize call onOutOfMemory()
  char* allocate(int aSize)
  {//Runs locate to find a suitable place to put an allocation and 2 flags.
      int flagSize =12;
      char* aPointer;
      aPointer = (char*) locate(aSize + 24);
      //here we find how much space is between the two flags before we put our new flag in between them
      char* second_Pointer;
      second_Pointer = aPointer - flagSize;
      int previousdistance = getLeftDistanceFromFlag(second_Pointer);
      int distance = getRightDistanceFromFlag(second_Pointer);
      //here we store the "left" flag's leftdistance so we can reprint that flag
      char leftSymbol = getLeftSymbolFromFlag(second_Pointer);
      //now we rewrite the "left" flag
      createFlag(previousdistance, leftSymbol, flagSize, 'u', second_Pointer);
      //now we write a left flag for our new alloccation
      createFlag(flagSize,'u', flagSize+aSize, 'u', aPointer);
      char* allocationPointer = aPointer;
      //now we go to the right side of our allocation
      aPointer += flagSize + aSize;
      //now we write a right flag for our new allocation
      createFlag(flagSize + aSize, 'u', distance - 2*flagSize - aSize, 'a', aPointer);
      aPointer += distance - 2*flagSize - aSize;
      previousdistance = getRightDistanceFromFlag(aPointer);
      char rightSymbol = getRightSymbolFromFlag(aPointer);
      createFlag(distance - 2* flagSize - aSize, 'a', previousdistance, rightSymbol, aPointer);
      //now we move stringpointer to the beginning and return it inside the flags
      return allocationPointer;
  }

  // Free up a chunk previously allocated
  void deallocate(void* vPointer)
  {//Given vPointer at the beginning of a allocated block, will fix surrounding flags to skip over allocated block
      char* aPointer = (char*) vPointer;
      char* ftrPointer1 = aPointer;
      //read flag and store distance to nearest left flag and distance to right flag
      int distanceToLeftFlag = getLeftDistanceFromFlag(aPointer);
      int deallocationSize = getRightDistanceFromFlag(aPointer);
      char* rightPointer;
      rightPointer = aPointer + deallocationSize;
      int distanceToRightFlag = getRightDistanceFromFlag(rightPointer);
      rightPointer += distanceToRightFlag;
      char* ftrPointer2 = aPointer;
      aPointer -= distanceToLeftFlag;
      int leftFlagLeftInt = getLeftDistanceFromFlag(aPointer);
      char leftFlagLeftSym = getLeftSymbolFromFlag(aPointer);
      int rightFlagRightInt = getRightDistanceFromFlag(rightPointer);
      char rightFlagRightSym = getRightSymbolFromFlag(rightPointer);
      createFlag(leftFlagLeftInt, leftFlagLeftSym, deallocationSize + distanceToLeftFlag+ distanceToRightFlag, 'a', aPointer);
      createFlag(deallocationSize + distanceToLeftFlag + distanceToRightFlag, 'a', rightFlagRightInt, rightFlagRightSym, rightPointer);
      
  }

  //---
  //--- support routines

  //---



  //---

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void)
  {
      int freeSpace = 0;
      char* aPointer;
      aPointer= &MM_pool[0];
      for(int i = 0; i < 1000; i++)
      {
          int distance = getRightDistanceFromFlag(aPointer);
          if(getRightSymbolFromFlag(aPointer) == 'a')
          {
              freeSpace += getRightDistanceFromFlag(aPointer);
          }else if (getRightSymbolFromFlag(aPointer) == 'e')
          {
              return freeSpace;
          }
            aPointer += distance;
          }
      return freeSpace;
      }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
      int largestSpace = 0;
      char* aPointer;
      aPointer= &MM_pool[0];
      for(int i = 0; i < 1000; i++)
      {
          int distance = getRightDistanceFromFlag(aPointer);
          if(getRightSymbolFromFlag(aPointer) == 'a' && distance >= largestSpace)
          {
              largestSpace += getRightDistanceFromFlag(aPointer);
          }else if (getRightSymbolFromFlag(aPointer) == 'e')
          {
              return largestSpace;
          }
          aPointer += distance;
      }
      return largestSpace;
  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
      int freeSpace = -1;
      char* aPointer;
      aPointer= &MM_pool[0];
      for(int i = 0; i < 1000; i++)
      {
          int distance = getRightDistanceFromFlag(aPointer);
          if(getRightSymbolFromFlag(aPointer) == 'a' && (freeSpace == -1 or distance < freeSpace))
          {
              freeSpace += getRightDistanceFromFlag(aPointer);
          }else if (getRightSymbolFromFlag(aPointer) == 'e')
          {
              return freeSpace;
          }
          aPointer += distance;
      }
      return freeSpace;
  }
 }

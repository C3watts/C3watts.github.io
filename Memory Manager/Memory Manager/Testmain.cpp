//
//  Testmain.cpp
//  Memory Manager
//
//  Created by Chas Watts on 5/21/18.
//  Copyright © 2018 Chas Watts. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include "MemoryManager.h"
#include "Testmain.hpp"
namespace Testmain
{
    int testMain(void){
        using namespace MemoryManager;
        int number = 100;
        char* firstSpot = allocate(number);
        deallocate(firstSpot);
        char* thirdSpot = allocate(number + 100);
        return(0);
        }
}


/*
Source: ingroup.c
By    : George Milliken & John Silva
What  : Checks to see if user X is in group Y in the Netware bindery
Date  : 03/28/94
Version 1.00
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <assert.h>

#ifndef FAR
   #define FAR far
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAP_DRIVE 14       // drive N:

#include <nwcalls.h>
#include <nwbindry.h>

#ifdef __cplusplus
}
#endif


int main(int argc, char *argv[])
{
   WORD connHandle;               // global for atexit()
   WORD wNWError;                 // error return code
   WORD connNumber;               // connection number
   long bDataBuffer[32];
   int  iBufferCount;
   int  bMoreFlag;
   long lTargetObjectID;
   int x;


   if (argc != 3) {
      puts("INGROUP.EXE Copyright (c) 1994 Wells Fargo Bank");
      puts("John & George's excellent novbat replacement");
      puts("Missing commandline option(s)");
      puts("INGROUP groupname username");
      exit(255);
   }

   /* init the NW system */
   if (wNWError = NWCallsInit(NULL, NULL)){
      puts("\nNWCallsInit: failed");
      exit(255);
   }

   if (wNWError = NWGetDefaultConnectionID(&connHandle)) {
      puts("\nCannot find your connection handle");
      exit(255);
   }

   if (wNWError = NWGetConnectionNumber(connHandle, &connNumber)) {
      puts("\nCannot find your connection number");
      exit(255);
   }

   // get target group'd ID 
   if (wNWError = NWGetObjectID(connHandle, strupr(argv[1]), OT_USER_GROUP, &lTargetObjectID)) {
      puts("\nGroup doesn't exist");
      exit(0);
   }

   iBufferCount = 1;
   bMoreFlag = 1;

   while (bMoreFlag) {
      memset(bDataBuffer, 0xAE, sizeof(bDataBuffer));
      wNWError = NWReadPropertyValue(connHandle, strupr(argv[2]), OT_USER, "GROUPS_I'M_IN", iBufferCount++, bDataBuffer, &bMoreFlag, NULL);
      if (wNWError != 0 && wNWError != 0x89ec) {
        puts("\nUser doesn't exist or is brain damaged");
        exit(0);
      }

      for (x = 0; x < 32; x++) {
        if (bDataBuffer[x] == lTargetObjectID) {
          puts("User in group.\n");
          exit(1);
        }
      }
   }

   puts("User not in group.\n");
   return(0);
}


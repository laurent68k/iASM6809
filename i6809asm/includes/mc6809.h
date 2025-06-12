//
//  mc6809.h
//  i6809asm
//
//  Created by Laurent Favard on 12/06/2025.
//  Copyright © 2025 Laurent. All rights reserved.
//

#ifndef     __MC6809__
#define     __MC6809__  __MC6809__

/*
 *      MC6809 specific processing
 */

#define PAGE2   0x10
#define PAGE3   0x11
#define IPBYTE  0x9F    /* extended indirect postbyte */
#define SWI     0x3F

/* register names */

#define RD      0
#define RX      1
#define RY      2
#define RU      3
#define RS      4
#define RPC     5
#define RA      8
#define RB      9
#define RCC     10
#define RDP     11
#define RPCR    12

/* addressing modes */
#define IMMED   0       /* immediate */
#define IND     1       /* indexed */
#define INDIR   2       /* indirect */
#define OTHER   3       /* NOTA */
    
#endif


//
//  directives.h
//  i6809asm
//
//  Created by Laurent Favard on 12/06/2025.
//  Copyright © 2025 Laurent. All rights reserved.
//

#ifndef     __DIRECTIVES__
#define     __DIRECTIVES__      __DIRECTIVES__

#define RMB     0       /* Reserve Memory Bytes         */
#define FCB     1       /* Form Constant Bytes          */
#define FDB     2       /* Form Double Bytes (words)    */
#define FCC     3       /* Form Constant Characters     */
#define ORG     4       /* Origin                       */
#define EQU     5       /* Equate                       */
#define ZMB     6       /* Zero memory bytes            */
#define FILL    7       /* block fill constant bytes    */
#define OPT     8       /* assembler option             */
#define NULL_OP 9       /* null pseudo op               */
#define PAGE    10      /* new page                     */
#define SPACETO  11      /* ROM Origin                   */


#endif


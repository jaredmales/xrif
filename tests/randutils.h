/* This file is part of the xrif library.

Copyright (c) 2020 The Arizona Board of Regents on behalf of The
University of Arizona

All rights reserved.

Developed by: The Arizona Board of Regents on behalf of the
University of Arizona.

Redistribution and use for noncommercial purposes in source and
binary forms, with or without modification, are permitted provided
that the following conditions are met:

1. The software is used solely for noncommercial purposes.

2. Redistributions of source code must retain the above copyright
notice, terms and conditions specified herein and the disclaimer
specified in Section 4 below.

3. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

4. Neither the name of the Arizona Board of Regents, the University
of Arizona nor the names of other contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

Arizona Required Clauses:

1.1. Arbitration. The parties agree that if a dispute arises
between them concerning this Agreement, the parties may be required
to submit the matter to arbitration pursuant to Arizona law.

1.2. Applicable Law and Venue. This Agreement shall be interpreted
pursuant to the laws of the State of Arizona. Any arbitration or
litigation between the Parties shall be conducted in Pima County,
ARIZONA, and LICENSEE hereby submits to venue and jurisdiction in
Pima County, ARIZONA.

1.3. Non-Discrimination. The Parties agree to be bound by state and
federal laws and regulations governing equal opportunity and non-
discrimination and immigration.

1.4. Appropriation of Funds. The Parties recognize that performance
by ARIZONA may depend upon appropriation of funds by the State
Legislature of ARIZONA. If the Legislature fails to appropriate the
necessary funds, or if ARIZONA’S appropriation is reduced during
the fiscal year, ARIZONA may cancel this Agreement without further
duty or obligation. ARIZONA will notify LICENSEE as soon as
reasonably possible after it knows of the loss of funds.

1.5. Conflict of Interest. This Agreement is subject to the
provisions of A.R.S. 38-511 and other conflict of interest
regulations. Within three years of the EFFECTIVE DATE, ARIZONA may
cancel this Agreement if any person significantly involved in
initiating, negotiating, drafting, securing, or creating this
Agreement for or on behalf of ARIZONA becomes an employee or
consultant in any capacity of LICENSEE with respect to the subject
matter of this Agreement.

*/

#include <stdlib.h>
#include <limits.h>

//prngs for producing more than 31 bits.
//I got this technique from stackoverflow.  Haven't tested it much
//Probably more than good enough for fuzz testing...
unsigned 
rand256()
{
    static unsigned const limit = RAND_MAX - RAND_MAX % 256;
    unsigned result = rand();
    while ( result >= limit ) {
        result = rand();
    }
    return result % 256;
}

uint32_t
rand32bits()
{
    uint32_t results = 0ULL;
    for ( int count = 4; count > 0; --count ) {
        results = 256U * results + rand256();
    }
    return results;
}

uint64_t
rand64bits()
{
    uint64_t results = 0ULL;
    for ( int count = 8; count > 0; -- count ) {
        results = 256U * results + rand256();
    }
    return results;
}

//Fill a 16-bit buffer with 14 bit signed white noise
int fill_int14_white( int16_t * buffer,
                      size_t size 
                    )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = -8192 + (((double) rand())/RAND_MAX)*(8191-(-8182));
   }
   
   return 0;
}

//Fill a 16-bit buffer with 14 bit unsigned white noise
int fill_uint14_white( int16_t * buffer,
                       size_t size 
                     )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = (((double) rand())/RAND_MAX)*(16383);
   }
   
   return 0;
}

//Fill a 16-bit buffer with 16-bit signed white noise
int fill_int16_white( int16_t * buffer,
                      size_t size 
                    )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = -32768 + (((double) rand())/RAND_MAX)*(32767-(-32768));
   }
   
   return 0;
}

//Fill a 16-bit buffer with 16-bit unsigned white noise
int fill_uint16_white( uint16_t * buffer,
                       size_t size 
                     )
{
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = (((double) rand())/RAND_MAX)*(65535);
   }
   
   return 0;
}

//Fill a 32-bit buffer with 32-bit signed white noise -- need a better prng for this
int fill_int32_white( int32_t * buffer,
                      size_t size 
                    )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = -2147483648LL + rand32bits();
   }
   
   return 0;
}

//Fill a 32-bit buffer with 32-bit unsigned white noise -- need a better prng for this
int fill_uint32_white( uint32_t * buffer,
                       size_t size 
                     )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = rand32bits();//  (((double) rand())/4294967296)*(4294967296);
   }
   
   return 0;
}

//Fill a 64-bit buffer with 64-bit signed white noise
int fill_int64_white( int64_t * buffer,
                      size_t size 
                    )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = -9223372036854775807-1 + rand64bits(); //-1 one trick to avoid compiler errror due to parsing order of - and number
   }
   
   return 0;
}

//Fill a 64-bit buffer with 32-bit unsigned white noise
int fill_uint64_white( uint64_t * buffer,
                       size_t size 
                     )
{
   
   if(buffer == NULL) return -1;
   if(size < 1) return -1;
   
   for( size_t i = 0; i < size; ++i ) 
   {
      buffer[i] = rand64bits();//  (((double) rand())/4294967296)*(4294967296);
   }
   
   return 0;
}

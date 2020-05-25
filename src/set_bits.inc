/* This file is part of the xrif library.

Copyright (c) 2019, 2020, The Arizona Board of Regents on behalf of The
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

static const unsigned char set_bits[] = 
{
/*0*/   0, 0, 0, 0, 0, 0, 0, 0, 0,
/*1*/   1, 0, 0, 0, 0, 0, 0, 0, 0,
/*2*/   1, 1, 0, 0, 0, 0, 0, 0, 0,
/*3*/   2, 0, 1, 0, 0, 0, 0, 0, 0,
/*4*/   1, 2, 0, 0, 0, 0, 0, 0, 0,
/*5*/   2, 0, 2, 0, 0, 0, 0, 0, 0,
/*6*/   2, 1, 2, 0, 0, 0, 0, 0, 0,
/*7*/   3, 0, 1, 2, 0, 0, 0, 0, 0,
/*8*/   1, 3, 0, 0, 0, 0, 0, 0, 0,
/*9*/   2, 0, 3, 0, 0, 0, 0, 0, 0,
/*10*/   2, 1, 3, 0, 0, 0, 0, 0, 0,
/*11*/   3, 0, 1, 3, 0, 0, 0, 0, 0,
/*12*/   2, 2, 3, 0, 0, 0, 0, 0, 0,
/*13*/   3, 0, 2, 3, 0, 0, 0, 0, 0,
/*14*/   3, 1, 2, 3, 0, 0, 0, 0, 0,
/*15*/   4, 0, 1, 2, 3, 0, 0, 0, 0,
/*16*/   1, 4, 0, 0, 0, 0, 0, 0, 0,
/*17*/   2, 0, 4, 0, 0, 0, 0, 0, 0,
/*18*/   2, 1, 4, 0, 0, 0, 0, 0, 0,
/*19*/   3, 0, 1, 4, 0, 0, 0, 0, 0,
/*20*/   2, 2, 4, 0, 0, 0, 0, 0, 0,
/*21*/   3, 0, 2, 4, 0, 0, 0, 0, 0,
/*22*/   3, 1, 2, 4, 0, 0, 0, 0, 0,
/*23*/   4, 0, 1, 2, 4, 0, 0, 0, 0,
/*24*/   2, 3, 4, 0, 0, 0, 0, 0, 0,
/*25*/   3, 0, 3, 4, 0, 0, 0, 0, 0,
/*26*/   3, 1, 3, 4, 0, 0, 0, 0, 0,
/*27*/   4, 0, 1, 3, 4, 0, 0, 0, 0,
/*28*/   3, 2, 3, 4, 0, 0, 0, 0, 0,
/*29*/   4, 0, 2, 3, 4, 0, 0, 0, 0,
/*30*/   4, 1, 2, 3, 4, 0, 0, 0, 0,
/*31*/   5, 0, 1, 2, 3, 4, 0, 0, 0,
/*32*/   1, 5, 0, 0, 0, 0, 0, 0, 0,
/*33*/   2, 0, 5, 0, 0, 0, 0, 0, 0,
/*34*/   2, 1, 5, 0, 0, 0, 0, 0, 0,
/*35*/   3, 0, 1, 5, 0, 0, 0, 0, 0,
/*36*/   2, 2, 5, 0, 0, 0, 0, 0, 0,
/*37*/   3, 0, 2, 5, 0, 0, 0, 0, 0,
/*38*/   3, 1, 2, 5, 0, 0, 0, 0, 0,
/*39*/   4, 0, 1, 2, 5, 0, 0, 0, 0,
/*40*/   2, 3, 5, 0, 0, 0, 0, 0, 0,
/*41*/   3, 0, 3, 5, 0, 0, 0, 0, 0,
/*42*/   3, 1, 3, 5, 0, 0, 0, 0, 0,
/*43*/   4, 0, 1, 3, 5, 0, 0, 0, 0,
/*44*/   3, 2, 3, 5, 0, 0, 0, 0, 0,
/*45*/   4, 0, 2, 3, 5, 0, 0, 0, 0,
/*46*/   4, 1, 2, 3, 5, 0, 0, 0, 0,
/*47*/   5, 0, 1, 2, 3, 5, 0, 0, 0,
/*48*/   2, 4, 5, 0, 0, 0, 0, 0, 0,
/*49*/   3, 0, 4, 5, 0, 0, 0, 0, 0,
/*50*/   3, 1, 4, 5, 0, 0, 0, 0, 0,
/*51*/   4, 0, 1, 4, 5, 0, 0, 0, 0,
/*52*/   3, 2, 4, 5, 0, 0, 0, 0, 0,
/*53*/   4, 0, 2, 4, 5, 0, 0, 0, 0,
/*54*/   4, 1, 2, 4, 5, 0, 0, 0, 0,
/*55*/   5, 0, 1, 2, 4, 5, 0, 0, 0,
/*56*/   3, 3, 4, 5, 0, 0, 0, 0, 0,
/*57*/   4, 0, 3, 4, 5, 0, 0, 0, 0,
/*58*/   4, 1, 3, 4, 5, 0, 0, 0, 0,
/*59*/   5, 0, 1, 3, 4, 5, 0, 0, 0,
/*60*/   4, 2, 3, 4, 5, 0, 0, 0, 0,
/*61*/   5, 0, 2, 3, 4, 5, 0, 0, 0,
/*62*/   5, 1, 2, 3, 4, 5, 0, 0, 0,
/*63*/   6, 0, 1, 2, 3, 4, 5, 0, 0,
/*64*/   1, 6, 0, 0, 0, 0, 0, 0, 0,
/*65*/   2, 0, 6, 0, 0, 0, 0, 0, 0,
/*66*/   2, 1, 6, 0, 0, 0, 0, 0, 0,
/*67*/   3, 0, 1, 6, 0, 0, 0, 0, 0,
/*68*/   2, 2, 6, 0, 0, 0, 0, 0, 0,
/*69*/   3, 0, 2, 6, 0, 0, 0, 0, 0,
/*70*/   3, 1, 2, 6, 0, 0, 0, 0, 0,
/*71*/   4, 0, 1, 2, 6, 0, 0, 0, 0,
/*72*/   2, 3, 6, 0, 0, 0, 0, 0, 0,
/*73*/   3, 0, 3, 6, 0, 0, 0, 0, 0,
/*74*/   3, 1, 3, 6, 0, 0, 0, 0, 0,
/*75*/   4, 0, 1, 3, 6, 0, 0, 0, 0,
/*76*/   3, 2, 3, 6, 0, 0, 0, 0, 0,
/*77*/   4, 0, 2, 3, 6, 0, 0, 0, 0,
/*78*/   4, 1, 2, 3, 6, 0, 0, 0, 0,
/*79*/   5, 0, 1, 2, 3, 6, 0, 0, 0,
/*80*/   2, 4, 6, 0, 0, 0, 0, 0, 0,
/*81*/   3, 0, 4, 6, 0, 0, 0, 0, 0,
/*82*/   3, 1, 4, 6, 0, 0, 0, 0, 0,
/*83*/   4, 0, 1, 4, 6, 0, 0, 0, 0,
/*84*/   3, 2, 4, 6, 0, 0, 0, 0, 0,
/*85*/   4, 0, 2, 4, 6, 0, 0, 0, 0,
/*86*/   4, 1, 2, 4, 6, 0, 0, 0, 0,
/*87*/   5, 0, 1, 2, 4, 6, 0, 0, 0,
/*88*/   3, 3, 4, 6, 0, 0, 0, 0, 0,
/*89*/   4, 0, 3, 4, 6, 0, 0, 0, 0,
/*90*/   4, 1, 3, 4, 6, 0, 0, 0, 0,
/*91*/   5, 0, 1, 3, 4, 6, 0, 0, 0,
/*92*/   4, 2, 3, 4, 6, 0, 0, 0, 0,
/*93*/   5, 0, 2, 3, 4, 6, 0, 0, 0,
/*94*/   5, 1, 2, 3, 4, 6, 0, 0, 0,
/*95*/   6, 0, 1, 2, 3, 4, 6, 0, 0,
/*96*/   2, 5, 6, 0, 0, 0, 0, 0, 0,
/*97*/   3, 0, 5, 6, 0, 0, 0, 0, 0,
/*98*/   3, 1, 5, 6, 0, 0, 0, 0, 0,
/*99*/   4, 0, 1, 5, 6, 0, 0, 0, 0,
/*100*/   3, 2, 5, 6, 0, 0, 0, 0, 0,
/*101*/   4, 0, 2, 5, 6, 0, 0, 0, 0,
/*102*/   4, 1, 2, 5, 6, 0, 0, 0, 0,
/*103*/   5, 0, 1, 2, 5, 6, 0, 0, 0,
/*104*/   3, 3, 5, 6, 0, 0, 0, 0, 0,
/*105*/   4, 0, 3, 5, 6, 0, 0, 0, 0,
/*106*/   4, 1, 3, 5, 6, 0, 0, 0, 0,
/*107*/   5, 0, 1, 3, 5, 6, 0, 0, 0,
/*108*/   4, 2, 3, 5, 6, 0, 0, 0, 0,
/*109*/   5, 0, 2, 3, 5, 6, 0, 0, 0,
/*110*/   5, 1, 2, 3, 5, 6, 0, 0, 0,
/*111*/   6, 0, 1, 2, 3, 5, 6, 0, 0,
/*112*/   3, 4, 5, 6, 0, 0, 0, 0, 0,
/*113*/   4, 0, 4, 5, 6, 0, 0, 0, 0,
/*114*/   4, 1, 4, 5, 6, 0, 0, 0, 0,
/*115*/   5, 0, 1, 4, 5, 6, 0, 0, 0,
/*116*/   4, 2, 4, 5, 6, 0, 0, 0, 0,
/*117*/   5, 0, 2, 4, 5, 6, 0, 0, 0,
/*118*/   5, 1, 2, 4, 5, 6, 0, 0, 0,
/*119*/   6, 0, 1, 2, 4, 5, 6, 0, 0,
/*120*/   4, 3, 4, 5, 6, 0, 0, 0, 0,
/*121*/   5, 0, 3, 4, 5, 6, 0, 0, 0,
/*122*/   5, 1, 3, 4, 5, 6, 0, 0, 0,
/*123*/   6, 0, 1, 3, 4, 5, 6, 0, 0,
/*124*/   5, 2, 3, 4, 5, 6, 0, 0, 0,
/*125*/   6, 0, 2, 3, 4, 5, 6, 0, 0,
/*126*/   6, 1, 2, 3, 4, 5, 6, 0, 0,
/*127*/   7, 0, 1, 2, 3, 4, 5, 6, 0,
/*128*/   1, 7, 0, 0, 0, 0, 0, 0, 0,
/*129*/   2, 0, 7, 0, 0, 0, 0, 0, 0,
/*130*/   2, 1, 7, 0, 0, 0, 0, 0, 0,
/*131*/   3, 0, 1, 7, 0, 0, 0, 0, 0,
/*132*/   2, 2, 7, 0, 0, 0, 0, 0, 0,
/*133*/   3, 0, 2, 7, 0, 0, 0, 0, 0,
/*134*/   3, 1, 2, 7, 0, 0, 0, 0, 0,
/*135*/   4, 0, 1, 2, 7, 0, 0, 0, 0,
/*136*/   2, 3, 7, 0, 0, 0, 0, 0, 0,
/*137*/   3, 0, 3, 7, 0, 0, 0, 0, 0,
/*138*/   3, 1, 3, 7, 0, 0, 0, 0, 0,
/*139*/   4, 0, 1, 3, 7, 0, 0, 0, 0,
/*140*/   3, 2, 3, 7, 0, 0, 0, 0, 0,
/*141*/   4, 0, 2, 3, 7, 0, 0, 0, 0,
/*142*/   4, 1, 2, 3, 7, 0, 0, 0, 0,
/*143*/   5, 0, 1, 2, 3, 7, 0, 0, 0,
/*144*/   2, 4, 7, 0, 0, 0, 0, 0, 0,
/*145*/   3, 0, 4, 7, 0, 0, 0, 0, 0,
/*146*/   3, 1, 4, 7, 0, 0, 0, 0, 0,
/*147*/   4, 0, 1, 4, 7, 0, 0, 0, 0,
/*148*/   3, 2, 4, 7, 0, 0, 0, 0, 0,
/*149*/   4, 0, 2, 4, 7, 0, 0, 0, 0,
/*150*/   4, 1, 2, 4, 7, 0, 0, 0, 0,
/*151*/   5, 0, 1, 2, 4, 7, 0, 0, 0,
/*152*/   3, 3, 4, 7, 0, 0, 0, 0, 0,
/*153*/   4, 0, 3, 4, 7, 0, 0, 0, 0,
/*154*/   4, 1, 3, 4, 7, 0, 0, 0, 0,
/*155*/   5, 0, 1, 3, 4, 7, 0, 0, 0,
/*156*/   4, 2, 3, 4, 7, 0, 0, 0, 0,
/*157*/   5, 0, 2, 3, 4, 7, 0, 0, 0,
/*158*/   5, 1, 2, 3, 4, 7, 0, 0, 0,
/*159*/   6, 0, 1, 2, 3, 4, 7, 0, 0,
/*160*/   2, 5, 7, 0, 0, 0, 0, 0, 0,
/*161*/   3, 0, 5, 7, 0, 0, 0, 0, 0,
/*162*/   3, 1, 5, 7, 0, 0, 0, 0, 0,
/*163*/   4, 0, 1, 5, 7, 0, 0, 0, 0,
/*164*/   3, 2, 5, 7, 0, 0, 0, 0, 0,
/*165*/   4, 0, 2, 5, 7, 0, 0, 0, 0,
/*166*/   4, 1, 2, 5, 7, 0, 0, 0, 0,
/*167*/   5, 0, 1, 2, 5, 7, 0, 0, 0,
/*168*/   3, 3, 5, 7, 0, 0, 0, 0, 0,
/*169*/   4, 0, 3, 5, 7, 0, 0, 0, 0,
/*170*/   4, 1, 3, 5, 7, 0, 0, 0, 0,
/*171*/   5, 0, 1, 3, 5, 7, 0, 0, 0,
/*172*/   4, 2, 3, 5, 7, 0, 0, 0, 0,
/*173*/   5, 0, 2, 3, 5, 7, 0, 0, 0,
/*174*/   5, 1, 2, 3, 5, 7, 0, 0, 0,
/*175*/   6, 0, 1, 2, 3, 5, 7, 0, 0,
/*176*/   3, 4, 5, 7, 0, 0, 0, 0, 0,
/*177*/   4, 0, 4, 5, 7, 0, 0, 0, 0,
/*178*/   4, 1, 4, 5, 7, 0, 0, 0, 0,
/*179*/   5, 0, 1, 4, 5, 7, 0, 0, 0,
/*180*/   4, 2, 4, 5, 7, 0, 0, 0, 0,
/*181*/   5, 0, 2, 4, 5, 7, 0, 0, 0,
/*182*/   5, 1, 2, 4, 5, 7, 0, 0, 0,
/*183*/   6, 0, 1, 2, 4, 5, 7, 0, 0,
/*184*/   4, 3, 4, 5, 7, 0, 0, 0, 0,
/*185*/   5, 0, 3, 4, 5, 7, 0, 0, 0,
/*186*/   5, 1, 3, 4, 5, 7, 0, 0, 0,
/*187*/   6, 0, 1, 3, 4, 5, 7, 0, 0,
/*188*/   5, 2, 3, 4, 5, 7, 0, 0, 0,
/*189*/   6, 0, 2, 3, 4, 5, 7, 0, 0,
/*190*/   6, 1, 2, 3, 4, 5, 7, 0, 0,
/*191*/   7, 0, 1, 2, 3, 4, 5, 7, 0,
/*192*/   2, 6, 7, 0, 0, 0, 0, 0, 0,
/*193*/   3, 0, 6, 7, 0, 0, 0, 0, 0,
/*194*/   3, 1, 6, 7, 0, 0, 0, 0, 0,
/*195*/   4, 0, 1, 6, 7, 0, 0, 0, 0,
/*196*/   3, 2, 6, 7, 0, 0, 0, 0, 0,
/*197*/   4, 0, 2, 6, 7, 0, 0, 0, 0,
/*198*/   4, 1, 2, 6, 7, 0, 0, 0, 0,
/*199*/   5, 0, 1, 2, 6, 7, 0, 0, 0,
/*200*/   3, 3, 6, 7, 0, 0, 0, 0, 0,
/*201*/   4, 0, 3, 6, 7, 0, 0, 0, 0,
/*202*/   4, 1, 3, 6, 7, 0, 0, 0, 0,
/*203*/   5, 0, 1, 3, 6, 7, 0, 0, 0,
/*204*/   4, 2, 3, 6, 7, 0, 0, 0, 0,
/*205*/   5, 0, 2, 3, 6, 7, 0, 0, 0,
/*206*/   5, 1, 2, 3, 6, 7, 0, 0, 0,
/*207*/   6, 0, 1, 2, 3, 6, 7, 0, 0,
/*208*/   3, 4, 6, 7, 0, 0, 0, 0, 0,
/*209*/   4, 0, 4, 6, 7, 0, 0, 0, 0,
/*210*/   4, 1, 4, 6, 7, 0, 0, 0, 0,
/*211*/   5, 0, 1, 4, 6, 7, 0, 0, 0,
/*212*/   4, 2, 4, 6, 7, 0, 0, 0, 0,
/*213*/   5, 0, 2, 4, 6, 7, 0, 0, 0,
/*214*/   5, 1, 2, 4, 6, 7, 0, 0, 0,
/*215*/   6, 0, 1, 2, 4, 6, 7, 0, 0,
/*216*/   4, 3, 4, 6, 7, 0, 0, 0, 0,
/*217*/   5, 0, 3, 4, 6, 7, 0, 0, 0,
/*218*/   5, 1, 3, 4, 6, 7, 0, 0, 0,
/*219*/   6, 0, 1, 3, 4, 6, 7, 0, 0,
/*220*/   5, 2, 3, 4, 6, 7, 0, 0, 0,
/*221*/   6, 0, 2, 3, 4, 6, 7, 0, 0,
/*222*/   6, 1, 2, 3, 4, 6, 7, 0, 0,
/*223*/   7, 0, 1, 2, 3, 4, 6, 7, 0,
/*224*/   3, 5, 6, 7, 0, 0, 0, 0, 0,
/*225*/   4, 0, 5, 6, 7, 0, 0, 0, 0,
/*226*/   4, 1, 5, 6, 7, 0, 0, 0, 0,
/*227*/   5, 0, 1, 5, 6, 7, 0, 0, 0,
/*228*/   4, 2, 5, 6, 7, 0, 0, 0, 0,
/*229*/   5, 0, 2, 5, 6, 7, 0, 0, 0,
/*230*/   5, 1, 2, 5, 6, 7, 0, 0, 0,
/*231*/   6, 0, 1, 2, 5, 6, 7, 0, 0,
/*232*/   4, 3, 5, 6, 7, 0, 0, 0, 0,
/*233*/   5, 0, 3, 5, 6, 7, 0, 0, 0,
/*234*/   5, 1, 3, 5, 6, 7, 0, 0, 0,
/*235*/   6, 0, 1, 3, 5, 6, 7, 0, 0,
/*236*/   5, 2, 3, 5, 6, 7, 0, 0, 0,
/*237*/   6, 0, 2, 3, 5, 6, 7, 0, 0,
/*238*/   6, 1, 2, 3, 5, 6, 7, 0, 0,
/*239*/   7, 0, 1, 2, 3, 5, 6, 7, 0,
/*240*/   4, 4, 5, 6, 7, 0, 0, 0, 0,
/*241*/   5, 0, 4, 5, 6, 7, 0, 0, 0,
/*242*/   5, 1, 4, 5, 6, 7, 0, 0, 0,
/*243*/   6, 0, 1, 4, 5, 6, 7, 0, 0,
/*244*/   5, 2, 4, 5, 6, 7, 0, 0, 0,
/*245*/   6, 0, 2, 4, 5, 6, 7, 0, 0,
/*246*/   6, 1, 2, 4, 5, 6, 7, 0, 0,
/*247*/   7, 0, 1, 2, 4, 5, 6, 7, 0,
/*248*/   5, 3, 4, 5, 6, 7, 0, 0, 0,
/*249*/   6, 0, 3, 4, 5, 6, 7, 0, 0,
/*250*/   6, 1, 3, 4, 5, 6, 7, 0, 0,
/*251*/   7, 0, 1, 3, 4, 5, 6, 7, 0,
/*252*/   6, 2, 3, 4, 5, 6, 7, 0, 0,
/*253*/   7, 0, 2, 3, 4, 5, 6, 7, 0,
/*254*/   7, 1, 2, 3, 4, 5, 6, 7, 0,
/*255*/   8, 0, 1, 2, 3, 4, 5, 6, 7
};

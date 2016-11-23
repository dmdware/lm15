

#ifndef LOCALMAP_H
#define LOCALMAP_H

#include "../platform.h"


//memory localizing hash map with changing hash function cipher key

//just keep a counter and rearrange its bits to give a non-linear result
//basically a sequence with a period of 2^32
//to get a different sequence, change which bits are switched


#if 0

110 -> 000
	010 -> 001
	111 -> 010
	001 -> 011

	truth table boolean
	i2	i1	i0		o2	o1	o0
	0	0	0		1	1	1
	...
	0	0	1		0	1	1
	1	1	0		0	0	0
	0	1	0		0	0	1
	1	1	1		0	1	0

	//hash fun
	o = (i << 1) | (i >> 31)
	o = o ^ ~(i & m)
	i = o
	//repeat 32

	o0[0] = i[2] ^ ~( i[0] & m0[0] )
	o0[1] = i[0] ^ ~( i[1] & m0[1] )
	o0[2] = i[1] ^ ~( i[2] & m0[2] )
	o1[0] = o0[2] ^ ~( o0[0] & m1[0] )
	o1[1] = o0[0] ^ ~( o0[1] & m1[1] )
	o1[2] = o0[1] ^ ~( o0[2] & m1[2] )
	o2[0] = o1[2] ^ ~( o1[0] & m2[0] )
	o2[1] = o1[0] ^ ~( o1[1] & m2[1] )
	o2[2] = o1[1] ^ ~( o1[2] & m2[2] )

	truth table t ^ ~(i & m)
	t	i	m			t2
	0	0	0			1
	0	0	1			1
	0	1	0			1
	0	1	1			0
	1	0	0			0
	1	0	1			0
	1	1	0			0
	1	1	1			1

	train for 110 -> 000
	o2[0] = 0
	0 = o1[2] ^ ~( o1[0] & m2[0] )
	so o1[2]=0 and ~( o1[0] & m2[0] )=0
	o1[2]=0
	so o1[0]=1 and m2[0]=1
	so o0[2]=0 and ~( o0[0] & m1[0] )=1
	so o0[2]=0 and o0[0]=0 and m1[0]=0
	or o0[2]=0 and o0[0]=1 and m1[0]=0
	or o0[2]=0 and o0[0]=0 and m1[0]=1
	or o0[2]=1 and ~( o0[0] & m1[0] )=0
	so o0[2]=1 and o0[2]=1 and o0[0]=1 and m1[0]=1
	o0[2]=1  and o0[0]=1
	so o0[2]=1 and i[2]=1 and ~( i[0] & m0[0] )=0
	so o0[2]=1 and i[0]=1 and m0[0]=1
	or o0[2]=1 and i[2]=0 and ~( i[0] & m0[0] )=1
	so o0[2]=1 and i[0]=0 and m0[0]=0
	or o0[2]=1 and i[0]=1 and m0[0]=0
	or o0[2]=1 and i[0]=0 and m0[0]=1
	or o1[2]=1 and ~( o1[0] & m2[0] )=1
	so o1[2]=1 and o1[0]=0 and m2[0]=0
	o1[2]=1 and o1[0]=0
	so o1[2]=1 and o0[2]=1 && ~( o0[0] & m1[0] )=1
	or o1[2]=1 and o0[2]=0 && ~( o0[0] & m1[0] )=0
	or o1[2]=1 and o1[0]=1 and m2[0]=0
	o1[2]=1 and o1[0]=1
	so 
	or o1[2]=1 and o1[0]=0 and m2[0]=1
	o1[2]=1 and o1[0]=0
	so o1[2]=1 and o0[2]=1 && ~( o0[0] & m1[0] )=1
	or o1[2]=1 and o0[2]=0 && ~( o0[0] & m1[0] )=0
	etc.

	just pick first one given restrictions, then pick for second applying the new restrictions, 
	and if it doesn't work, back up the last step and try different, and so on
#endif


	//work backwards from last bracket level
	bool HashFun(uint32_t mval[32], uint32_t* mup, uint32_t mset[32], uint32_t oval[32], uint32_t oset[32], uint32_t in, uint32_t inin=0, int8_t level=31, int32_t* skip=NULL)
{
	uint32_t tempmval[32], tempoval[32], tempmset[32], temposet[32];
	memcpy(tempmval, mval, sizeof(uint32_t) * 32);
	memcpy(tempoval, oval, sizeof(uint32_t) * 32);
	memcpy(tempmset, mset, sizeof(uint32_t) * 32);
	memcpy(temposet, oset, sizeof(uint32_t) * 32);

	//o2[0] = 0
	//0 = o1[2] ^ ~( o1[0] & m2[0] )

	uint32_t mdown[32];
	uint32_t olevel = oval[level];
	uint32_t olevelset = oset[level];

	//get matching that are set:
	// ( ((mval & mvaldown) | (~mval & ~mvaldown)) & mvalset ) | ( ~mvalset )
	// ones = ( mvaldown & mvalset )
	// zeros = ( ~mvaldown & mvalset ) 
	//
	// mvalset |= 
	// just watch out when looping around for bin-1 value set by other end

	//todo send param u32 restriction for oval(level-1) and mval(level-1) is not needed (not set at level)

	//todo have a set of *ins and *outs for required values
	//and narrow down mval's at each level that match for all and pick first one that works
	//oval's will be diff for each in/out pair so keep *ovals

	//todo use bit logic on multiple bits at once to get m nd if o(level-1)[] is set etc
	//todo each level depends only on level-1
	//and at each level-1, [bit-1] and [bit] of oval(level-1) are needed (the only possible conflict)
	//masks are not shared between any (level,bin) sets
	//todo optimize using this
	for(uint8_t bin=0; bin<32; ++bin)
	{
		const uint8_t binmin = (bin+33)%32;
		//const uint8_t binmax = (bin);

		//should be set from higher level
		if(olevel&(1<<bin))
		{
			//olevel[bin] = 1

			//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
			//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1
			if(level > 0)	
			{	
				//check oset(level-1)[#-1]
				//todo use bit logic to figure what m is and if o(level-1)[] is set etc
				if( (oset[level-1]&(1<<binmin)) && (oval[level-1]&(1<<binmin)) )
				{
					//i[#-1]=1 and ~( i[#] & m0[#] )=0
					//so i[#]=1 and m0[#]=1

					//todo m(level)[#] can never be set by anything other than (level)[#] so dont check
					if(mset[level]&(1<<bin))
					{
						if(mval[level]&(1<<bin))
						{
							//already set
							//m0[#]=1
						}
						else
						{
							//m0[#]=0
							return false;
						}
					}
					else
					{
						mset[level] |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					if( (oset[level-1]&(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[#]=1
					}
					//check oset(level-1)[#+1]
					else if( (oset[level-1]&(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//i[#]=0
						return false;
					}
					//!oset(level-1)[#]
					else
					{
						//i[#]=1

						oset[level-1] |= (1<<bin);
						oval[level-1] |= (1<<bin);
					}
				}
				//else i[#-1]=0
				else if( (oset[level-1]&(1<<binmin)) && !(oval[level-1]&(1<<binmin)) )
				{
					//i[#-1]=0 and ~( i[#] & m0[#] )=1

					//so i[#]=0 and m0[#]=0
					//or i[#]=1 and m0[#]=0
					//or i[#]=0 and m0[#]=1

					//check oset(level-1)[#]
					if( (oset[level-1]&(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//i[#]=1

						//todo check already mset
						//so m0[#]=0
						mset[0] |= (1<<bin);
						mval[0] &= ~(1<<bin);
					}
					else if( (oset[level-1]&(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//i[#]=0

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset[level] |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip
					}
					//!oset(level-1)[#]
					else
					{
						//todo

						//choice
						//so i[#]=1 and m0[#]=0
						//or i[#]=0 and m0[#]=0
						//or i[#]=0 and m0[#]=1

						oset[level-1] |= (1<<bin);
						oval[level-1] &= ~(1<<bin);

						mset[level] |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//they can't both be 1 and set
					//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
				}
				//so i[#-1]=1 and ~( i[#] & m0[#] )=0
				//or i[#-1]=0 and ~( i[#] & m0[#] )=1
				else if( !(oset[level-1]&(1<<binmin)) )
				{
					//todo
					//choice

					//check oset(level-1)[#]
					if( (oset[level-1]&(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//i[#]=1

						//todo check already mset
						//so m0[#]=0
						mset[0] |= (1<<bin);
						mval[0] &= ~(1<<bin);

						//so i[#-1]=0
						oset[level-1] |= (1<<binmin);
						oval[level-1] &= ~(1<<binmin);
					}
					else if( (oset[level-1]&(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//i[#]=0

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset[level] |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip

						//so i[#-1]=0
						oset[level-1] |= (1<<binmin);
						oval[level-1] &= ~(1<<binmin);
					}
					//!oset(level-1)[#]
					else
					{
						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=1

						oset[level-1] |= (1<<bin);
						oval[level-1] &= ~(1<<bin);

						mset[level] |= (1<<bin);
						mval[level] |= (1<<bin);

						oset[level-1] |= (1<<binmin);
						oval[level-1] &= ~(1<<binmin);
					}
				}
			}
			//o(0)[#] = i[#-1] ^ ~( i[#] & m0[#] ) = 1
			//so i[#-1]=1 and ~( i[#] & m0[#] )=0
			//or i[#-1]=0 and ~( i[#] & m0[#] )=1
			else	//level = 0
			{	
				//check oset(level-1)[#-1]
				//todo use bit logic to figure what m is and if o(level-1)[] is set etc
				if(in&(1<<binmin))
				{
					//i[#-1]=1 and ~( i[#] & m0[#] )=0
					//so i[#]=1 and m0[#]=1

					if(mset[0]&(1<<bin))
					{
						if(mval[0]&(1<<bin))
						{
							//already set
							//m0[#]=1
						}
						else
						{
							//m0[#]=0
							return false;
						}
					}
					else
					{
						mset[0] |= (1<<bin);
						mval[0] |= (1<<bin);
					}

					if(in&(1<<bin))
					{
						//i[#]=1
					}
					//check oset(level-1)[#+1]
					else
					{
						//i[#]=0
						return false;
					}
				}
				//else i[#-1]=0
				else
				{
					//i[#-1]=0 and ~( i[#] & m0[#] )=1

					//so i[#]=0 and m0[#]=0
					//or i[#]=1 and m0[#]=0
					//or i[#]=0 and m0[#]=1

					//check oset(level-1)[#-1]
					if(in&(1<<bin))
					{
						//i[#]=1

						//todo check already mset
						//so m0[#]=0
						mset[0] |= (1<<bin);
						mval[0] &= ~(1<<bin);
					}
					else
					{
						//i[#]=0

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset[0] |= (1<<bin);
						mval[0] |= (1<<bin);
						//todo or other choice skip
					}

					//they can't both be 1 and set
					//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
				}
			}
		}
		else
		{
			//olevel[bin] = 0

			//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
			//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

			if(level > 0)
			{
				if( (oset[level-1]&(1<<binmin)) && (oval[level-1]&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=1
					// so ~( o(level-1)[bin] & m(level)[bin] )=1

					//so o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[bin]=0 and m(level)[bin]=1

					if( (oset[level-1]*(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						//so m(level)[bin]=0

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							return false;
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					else if( (oset[level-1]*(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so m(level)[bin]=0
						//or m(level)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset[level]&(1<<bin))
						else
						{
							//choice
							mset[level] |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					// !(oset[level-1]*(1<<bin))
					else
					{
						//choice
						//so o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[bin]=0 and m(level)[bin]=1
					}
				}
				else if( (oset[level-1]&(1<<binmin)) && !(oval[level-1]&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=0
					// so ~( o(level-1)[bin] & m(level)[bin] )=0

					//so o(level-1)[bin]=1 and m(level)[bin]=1

					if( (oset[level-1]*(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					else if( (oset[level-1]*(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=0
						return false;
					}
					// !(oset[level-1]*(1<<bin))
					else
					{
						//choice
						oset[level-1] |= (1<<bin);
						oval[level-1] |= (1<<bin);

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
				// !(oset[level-1]&(1<<binmin))
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					if( (oset[level-1]*(1<<bin)) && (oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=1
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=0

							oset[level-1] |= (1<<binmin);
							oval[level-1] &= ~(1<<binmin);
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);
						}
						// !(mset[level]&(1<<bin))
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=0 and m(level)[bin]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] &= ~(1<<binmin);

							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					else if( (oset[level-1]*(1<<bin)) && !(oval[level-1]&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);
						}
						// !(mset[level]&(1<<bin))
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);

							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
					// !(oset[level-1]*(1<<bin))
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//oset[level-1] |= (1<<bin);
						//oval[level-1] |= (1<<bin);

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
							//choice

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);

							oset[level-1] |= (1<<bin);
							oval[level-1] &= ~(1<<bin);
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//choice

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);

							oset[level-1] |= (1<<bin);
							oval[level-1] &= ~(1<<bin);
						}
						// !(mset[level]&(1<<bin))
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

							oset[level-1] |= (1<<binmin);
							oval[level-1] |= (1<<binmin);

							oset[level-1] |= (1<<bin);
							oval[level-1] &= ~(1<<bin);

							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
			}
			//level=0
			else
			{
				if( in&(1<<binmin) )
				{
					// o(level-1)[(bin-1)%nb]=1
					// so ~( o(level-1)[bin] & m(level)[bin] )=1

					//so o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[bin]=0 and m(level)[bin]=1

					if( (in&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						//so m(level)[bin]=0

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							return false;
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					else if( !(in&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so m(level)[bin]=0
						//or m(level)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset[level]&(1<<bin))
						else
						{
							//choice
							mset[level] |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					// !(oset[level-1]*(1<<bin))
					else
					{
						//choice
						//so o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[bin]=0 and m(level)[bin]=1
					}
				}
				else if( !(in&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=1
					// so ~( o(level-1)[bin] & m(level)[bin] )=1

					// o(level-1)[(bin-1)%nb]=0
					// so ~( o(level-1)[bin] & m(level)[bin] )=0

					//so o(level-1)[bin]=1 and m(level)[bin]=1

					if( (in&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					else if( !(in&(1<<bin)) )
					{
						//o(level-1)[bin]=0
						return false;
					}
					// !(oset[level-1]*(1<<bin))
					else
					{
						//choice
						oset[level-1] |= (1<<bin);
						oval[level-1] |= (1<<bin);

						if( (mset[level]&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset[level]&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset[level]&(1<<bin))
						else
						{
							mset[level] |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
			}
		}
	}

	//HasFun(level-1)
}

//work backwards from last bracket level
bool HashFun2(uint32_t mval[32], uint32_t ovalup, uint32_t in, uint32_t* skip, int8_t level=31)
{
	//o2[0] = 0
	//0 = o1[2] ^ ~( o1[0] & m2[0] )

	uint32_t mset = 0;
	uint32_t ovaldown = 0;
	uint32_t osetdown = 0;

	if(!level)
	{
		osetdown = -1;
		ovaldown = in;
	}

	//get matching that are set:
	// ( ((mval & mvaldown) | (~mval & ~mvaldown)) & mvalset ) | ( ~mvalset )
	// ones = ( mvaldown & mvalset )
	// zeros = ( ~mvaldown & mvalset ) 
	//
	// mvalset |= 
	// just watch out when looping around for bin-1 value set by other end

	//todo send param u32 restriction for oval(level-1) and mval(level-1) is not needed (not set at level)

	//todo have a set of *ins and *outs for required values
	//and narrow down mval's at each level that match for all and pick first one that works
	//oval's will be diff for each in/out pair so keep *ovals

	//todo use bit logic on multiple bits at once to get m nd if o(level-1)[] is set etc
	//todo each level depends only on level-1
	//and at each level-1, [bit-1] and [bit] of oval(level-1) are needed (the only possible conflict)
	//masks are not shared between any (level,bin) sets
	//todo optimize using this
	for(uint8_t bin=0; bin<32; ++bin)
	{
		const uint8_t binmin = (bin+33)%32;
		//const uint8_t binmax = (bin);

		//should be set from higher level
		if(ovalup&(1<<bin))
		{
			//ovalup[bin] = 1

			//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
			//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1

			//check oset(level-1)[#-1]
			//todo use bit logic to figure what m is and if o(level-1)[] is set etc
			if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
			{
				//i[#-1]=1 and ~( i[#] & m0[#] )=0
				//so i[#]=1 and m0[#]=1

				//todo m(level)[#] can never be set by anything other than (level)[#] so dont check
				if(mset&(1<<bin))
				{
					if(mval[level]&(1<<bin))
					{
						//already set
						//m0[#]=1
					}
					else
					{
						//m0[#]=0
						return false;
					}
				}
				else
				{
					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}

				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//o(level-1)[#]=1
				}
				//check oset(level-1)[#+1]
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//i[#]=0
					return false;
				}
				//!oset(level-1)[#]
				else
				{
					//i[#]=1

					osetdown |= (1<<bin);
					ovaldown |= (1<<bin);
				}
			}
			//else i[#-1]=0
			else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
			{
				//i[#-1]=0 and ~( i[#] & m0[#] )=1

				//so i[#]=0 and m0[#]=0
				//or i[#]=1 and m0[#]=0
				//or i[#]=0 and m0[#]=1

				//check oset(level-1)[#]
				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//i[#]=1

					//todo check already mset
					//so m0[#]=0
					mset |= (1<<bin);
					mval[level] &= ~(1<<bin);
				}
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//i[#]=0

					//todo check already mset
					//so m0[#]=0
					//or m0[#]=1
					mset |= (1<<bin);
					mval[level] |= (1<<bin);
					//todo or other choice skip
				}
				//!oset(level-1)[#]
				else
				{
					//todo

					//choice
					//so i[#]=1 and m0[#]=0
					//or i[#]=0 and m0[#]=0
					//or i[#]=0 and m0[#]=1

					osetdown |= (1<<bin);
					ovaldown &= ~(1<<bin);

					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}

				//they can't both be 1 and set
				//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
			}
			//so i[#-1]=1 and ~( i[#] & m0[#] )=0
			//or i[#-1]=0 and ~( i[#] & m0[#] )=1
			else if( !(osetdown&(1<<binmin)) )
			{
				//todo
				//choice

				//check oset(level-1)[#]
				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//i[#]=1

					//todo check already mset
					//so m0[#]=0
					mset |= (1<<bin);
					mval[level] &= ~(1<<bin);

					//so i[#-1]=0
					osetdown |= (1<<binmin);
					ovaldown &= ~(1<<binmin);
				}
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//i[#]=0

					//todo check already mset
					//so m0[#]=0
					//or m0[#]=1
					mset |= (1<<bin);
					mval[level] |= (1<<bin);
					//todo or other choice skip

					//so i[#-1]=0
					osetdown |= (1<<binmin);
					ovaldown &= ~(1<<binmin);
				}
				//!oset(level-1)[#]
				else
				{
					//choice
					//so i[#-1]=1 and i[#]=1 and m0[#]=0
					//or i[#-1]=1 and i[#]=0 and m0[#]=0
					//or i[#-1]=1 and i[#]=0 and m0[#]=1
					//or i[#-1]=0 and i[#]=1 and m0[#]=1

					osetdown |= (1<<bin);
					ovaldown &= ~(1<<bin);

					mset |= (1<<bin);
					mval[level] |= (1<<bin);

					osetdown |= (1<<binmin);
					ovaldown &= ~(1<<binmin);
				}
			}
		}
		else
		{
			//ovalup[bin] = 0

			//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
			//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

			if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
			{
				// o(level-1)[(bin-1)%nb]=1
				// so ~( o(level-1)[bin] & m(level)[bin] )=1

				//so o(level-1)[bin]=0 and m(level)[bin]=0
				//or o(level-1)[bin]=1 and m(level)[bin]=0
				//or o(level-1)[bin]=0 and m(level)[bin]=1

				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=1

					//so m(level)[bin]=0

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
						return false;
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
					}
					// !(mset&(1<<bin))
					else
					{
						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);
					}
				}
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=0

					//choice
					//so m(level)[bin]=0
					//or m(level)[bin]=1

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
					}
					// !(mset&(1<<bin))
					else
					{
						//choice
						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);
					}
				}
				// !(osetdown&(1<<bin))
				else
				{
					//choice
					//so o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[bin]=0 and m(level)[bin]=1
				}
			}
			else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
			{
				// o(level-1)[(bin-1)%nb]=0
				// so ~( o(level-1)[bin] & m(level)[bin] )=0

				//so o(level-1)[bin]=1 and m(level)[bin]=1

				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=1

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
						return false;
					}
					// !(mset&(1<<bin))
					else
					{
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//2^32 values
					//(2^32)^32 assignments
					//
				}
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=0
					return false;
				}
				// !(osetdown&(1<<bin))
				else
				{
					//choice
					osetdown |= (1<<bin);
					ovaldown |= (1<<bin);

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
						return false;
					}
					// !(mset&(1<<bin))
					else
					{
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}
				}
			}
			// !(osetdown&(1<<binmin))
			else
			{
				//choice
				//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
				//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

				if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=1
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
						//so o(level-1)[(bin-1)%nb]=0

						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
						//so o(level-1)[(bin-1)%nb]=1

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);
					}
					// !(mset&(1<<bin))
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=0 and m(level)[bin]=1

						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//2^32 values
					//(2^32)^32 assignments
					//
				}
				else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
				{
					//o(level-1)[bin]=0

					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
						//so o(level-1)[(bin-1)%nb]=1

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
						//so o(level-1)[(bin-1)%nb]=1

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);
					}
					// !(mset&(1<<bin))
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=1

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}
				}
				// !(osetdown&(1<<bin))
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					//osetdown |= (1<<bin);
					//ovaldown |= (1<<bin);

					if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=1
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
						//choice

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);
					}
					else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
					{
						//m(level)[bin]=0
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//choice

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);
					}
					// !(mset&(1<<bin))
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						osetdown |= (1<<binmin);
						ovaldown |= (1<<binmin);

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}
				}
			}
		}
	}

	if(level)
		HashFun2(mval, ovaldown, in, skip, level-1);
}


#if 0
https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive

Counting bits set, in parallel

	unsigned int v; // count bits set in this (32-bit value)
unsigned int c; // store the total here
static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

c = v - ((v >> 1) & B[0]);
c = ((c >> S[1]) & B[1]) + (c & B[1]);
c = ((c >> S[2]) + c) & B[2];
c = ((c >> S[3]) + c) & B[3];
c = ((c >> S[4]) + c) & B[4];
The B array, expressed as binary, is:
B[0] = 0x55555555 = 01010101 01010101 01010101 01010101
	B[1] = 0x33333333 = 00110011 00110011 00110011 00110011
	B[2] = 0x0F0F0F0F = 00001111 00001111 00001111 00001111
	B[3] = 0x00FF00FF = 00000000 11111111 00000000 11111111
	B[4] = 0x0000FFFF = 00000000 00000000 11111111 11111111
	We can adjust the method for larger integer sizes by continuing with the patterns for the Binary Magic Numbers, B and S. If there are k bits, then we need the arrays S and B to be ceil(lg(k)) elements long, and we must compute the same number of expressions for c as S or B are long. For a 32-bit v, 16 operations are used.
	The best method for counting bits in a 32-bit integer v is the following:

v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
The best bit counting method takes only 12 operations, which is the same as the lookup-table method, but avoids the memory and potential cache misses of a table. It is a hybrid between the purely parallel method above and the earlier methods using multiplies (in the section on counting bits with 64-bit instructions), though it doesn't use 64-bit instructions. The counts of bits set in the bytes is done in parallel, and the sum total of the bits set in the bytes is computed by multiplying by 0x1010101 and shifting right 24 bits.

	A generalization of the best bit counting method to integers of bit-widths upto 128 (parameterized by type T) is this:

	v = v - ((v >> 1) & (T)~(T)0/3);                           // temp
v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);      // temp
v = (v + (v >> 4)) & (T)~(T)0/255*15;                      // temp
c = (T)(v * ((T)~(T)0/255)) >> (sizeof(T) - 1) * CHAR_BIT; // count
See Ian Ashdown's nice newsgroup post for more information on counting the number of bits set (also known as sideways addition). The best bit counting method was brought to my attention on October 5, 2005 by Andrew Shapira; he found it in pages 187-188 of Software Optimization Guide for AMD Athlon™ 64 and Opteron™ Processors. Charlie Gordon suggested a way to shave off one operation from the purely parallel version on December 14, 2005, and Don Clugston trimmed three more from it on December 30, 2005. I made a typo with Don's suggestion that Eric Cole spotted on January 8, 2006. Eric later suggested the arbitrary bit-width generalization to the best method on November 17, 2006. On April 5, 2007, Al Williams observed that I had a line of dead code at the top of the first method.


	Count bits set (rank) from the most-significant bit upto a given position

	The following finds the the rank of a bit, meaning it returns the sum of bits that are set to 1 from the most-signficant bit downto the bit at the given position.
	uint64_t v;       // Compute the rank (bits set) in v from the MSB to pos.
unsigned int pos; // Bit position to count bits upto.
uint64_t r;       // Resulting rank of bit at pos goes here.

// Shift out bits after given position.
r = v >> (sizeof(v) * CHAR_BIT - pos);
// Count set bits in parallel.
// r = (r & 0x5555...) + ((r >> 1) & 0x5555...);
r = r - ((r >> 1) & ~0UL/3);
// r = (r & 0x3333...) + ((r >> 2) & 0x3333...);
r = (r & ~0UL/5) + ((r >> 2) & ~0UL/5);
// r = (r & 0x0f0f...) + ((r >> 4) & 0x0f0f...);
r = (r + (r >> 4)) & ~0UL/17;
// r = r % 255;
r = (r * (~0UL/255)) >> ((sizeof(v) - 1) * CHAR_BIT);
Juha Järvi sent this to me on November 21, 2009 as an inverse operation to the computing the bit position with the given rank, which follows.
	Select the bit position (from the most-significant bit) with the given count (rank)

	The following 64-bit code selects the position of the rth 1 bit when counting from the left. In other words if we start at the most significant bit and proceed to the right, counting the number of bits set to 1 until we reach the desired rank, r, then the position where we stop is returned. If the rank requested exceeds the count of bits set, then 64 is returned. The code may be modified for 32-bit or counting from the right.
	uint64_t v;          // Input value to find position with rank r.
unsigned int r;      // Input: bit's desired rank [1-64].
unsigned int s;      // Output: Resulting position of bit with rank r [1-64]
uint64_t a, b, c, d; // Intermediate temporaries for bit count.
unsigned int t;      // Bit count temporary.

// Do a normal parallel bit count for a 64-bit integer,                     
// but store all intermediate steps.                                        
// a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
a =  v - ((v >> 1) & ~0UL/3);
// b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
b = (a & ~0UL/5) + ((a >> 2) & ~0UL/5);
// c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
c = (b + (b >> 4)) & ~0UL/0x11;
// d = (c & 0x00ff...) + ((c >> 8) & 0x00ff...);
d = (c + (c >> 8)) & ~0UL/0x101;
t = (d >> 32) + (d >> 48);
// Now do branchless select!                                                
s  = 64;
// if (r > t) {s -= 32; r -= t;}
s -= ((t - r) & 256) >> 3; r -= (t & ((t - r) >> 8));
t  = (d >> (s - 16)) & 0xff;
// if (r > t) {s -= 16; r -= t;}
s -= ((t - r) & 256) >> 4; r -= (t & ((t - r) >> 8));
t  = (c >> (s - 8)) & 0xf;
// if (r > t) {s -= 8; r -= t;}
s -= ((t - r) & 256) >> 5; r -= (t & ((t - r) >> 8));
t  = (b >> (s - 4)) & 0x7;
// if (r > t) {s -= 4; r -= t;}
s -= ((t - r) & 256) >> 6; r -= (t & ((t - r) >> 8));
t  = (a >> (s - 2)) & 0x3;
// if (r > t) {s -= 2; r -= t;}
s -= ((t - r) & 256) >> 7; r -= (t & ((t - r) >> 8));
t  = (v >> (s - 1)) & 0x1;
// if (r > t) s--;
s -= ((t - r) & 256) >> 8;
s = 65 - s;
If branching is fast on your target CPU, consider uncommenting the if-statements and commenting the lines that follow them.
	Juha Järvi sent this to me on November 21, 2009.
#endif


inline uint8_t CountSet(uint32_t v)
{
	uint8_t c;
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count

	return c;
}

//position from rank
inline uint8_t SetPos(uint8_t r)
{
	/*
	The following 64-bit code selects the position of the rth 1 bit when counting from the left. 
	In other words if we start at the most significant bit and proceed to the right, counting the 
	number of bits set to 1 until we reach the desired rank, r, then the position where we stop 
	is returned. If the rank requested exceeds the count of bits set, then 64 is returned. The 
	code may be modified for 32-bit or counting from the right.
	*/

	//uint64_t v;          // Input value to find position with rank r.
	uint32_t v;
	//unsigned int r;      // Input: bit's desired rank [1-64].
	unsigned int s;      // Output: Resulting position of bit with rank r [1-64]
	//uint64_t a, b, c, d; // Intermediate temporaries for bit count.
	uint32_t a, b, c, d;
	unsigned int t;      // Bit count temporary.

	//static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};
	
	// Do a normal parallel bit count for a 64-bit integer,                     
	// but store all intermediate steps.                                        
	a = (v & 0x55555555) + ((v >> 1) & 0x55555555);
	//a =  v - ((v >> 1) & ~0UL/3);
	b = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	//b = (a & ~0UL/5) + ((a >> 2) & ~0UL/5);
	c = (b & 0x0F0F0F0F) + ((b >> 4) & 0x0F0F0F0F);
	//c = (b + (b >> 4)) & ~0UL/0x11;
	d = (c & 0x00FF00FF) + ((c >> 8) & 0x00FF00FF);
	//d = (c + (c >> 8)) & ~0UL/0x101;
	//t = (d >> 32) + (d >> 48);
	t = (d >> 16) + (d >> 24);
	// Now do branchless select!                                                
	//s  = 64;
	s = 32;
	// if (r > t) {s -= 32; r -= t;}
	s -= ((t - r) & 256) >> 3; r -= (t & ((t - r) >> 8));
	t  = (d >> (s - 16)) & 0xff;
	// if (r > t) {s -= 16; r -= t;}
	s -= ((t - r) & 256) >> 4; r -= (t & ((t - r) >> 8));
	t  = (c >> (s - 8)) & 0xf;
	// if (r > t) {s -= 8; r -= t;}
	s -= ((t - r) & 256) >> 5; r -= (t & ((t - r) >> 8));
	t  = (b >> (s - 4)) & 0x7;
	// if (r > t) {s -= 4; r -= t;}
	s -= ((t - r) & 256) >> 6; r -= (t & ((t - r) >> 8));
	t  = (a >> (s - 2)) & 0x3;
	// if (r > t) {s -= 2; r -= t;}
	s -= ((t - r) & 256) >> 7; r -= (t & ((t - r) >> 8));
	t  = (v >> (s - 1)) & 0x1;
	// if (r > t) s--;
	s -= ((t - r) & 256) >> 8;
	//s = 65 - s;
	s = 33 - s;
	
	return s;
}

#if 0

//http://bits.stephan-brumme.com/lowestBitNotSet.html

unsigned int lowestBitNotSet(unsigned int x)
{
	return ~x & (x + 1);
}

unsigned int lowestBitNotSetSimple(unsigned int x)
{
	// to be consistent with lowestBitNotSet
	if (x == ~0)
		return 0;

	// shift right until finding a zero
	unsigned int result = 1;
	while ((x & result) != 0)
		result <<= 1; 
	return result;
}

// http://bits.stephan-brumme.com/lowestBitSet.html

 const unsigned int MultiplyDeBruijnBitPosition[32] =
02: {
03:   // precomputed lookup table
04:   0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
05:   31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9
06: };
07: 
08: unsigned int lowestBitSet(unsigned int x)
09: {
10:   // only lowest bit will remain 1, all others become 0
11:   x  &= -int(x);
12:   // DeBruijn constant
13:   x  *= 0x077CB531;
14:   // the upper 5 bits are unique, skip the rest (32 - 5 = 27)
15:   x >>= 27;
16:   // convert to actual position
17:   return MultiplyDeBruijnBitPosition[x];
18: }
19: 
20: unsigned int lowestBitSetSimple(unsigned int x)
21: {
22:   // x=0 is not properly handled by while-loop
23:   if (x == 0)
24:     return 0;
25: 
26:   unsigned int result = 0;
27:   while ((x & 1) == 0)
28:   {
29:     x >>= 1;
30:     result++;
31:   }
32: 
33:   return result;
34: }

//http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer

int NumberOfSetBits(int i)
{
     // Java: use >>> instead of >>
     // C or C++: use uint32_t
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// http://bits.stephan-brumme.com/countBits.html

01: unsigned int countBits(unsigned int x)
02: {
03:   // count bits of each 2-bit chunk
04:   x  = x - ((x >> 1) & 0x55555555);
05:   // count bits of each 4-bit chunk
06:   x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
07:   // count bits of each 8-bit chunk
08:   x  = x + (x >> 4);
09:   // mask out junk
10:   x &= 0xF0F0F0F;
11:   // add all four 8-bit chunks
12:   return (x * 0x01010101) >> 24;
13: }
14: 
15: unsigned int countBitsLoop(unsigned int x)
16: {
17:   unsigned int result = 0;
18:   // strip one set bit per iteration
19:   while (x != 0)
20:   {
21:     x &= x-1;
22:     result++;
23:   }
24:   return result;
25: }

#endif

// http://bits.stephan-brumme.com/countBits.html

inline uint32_t CtSet(uint32_t x)
{
	// count bits of each 2-bit chunk
	x  = x - ((x >> 1) & 0x55555555);
	// count bits of each 4-bit chunk
	x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	// count bits of each 8-bit chunk
	x  = x + (x >> 4);
	// mask out junk
	x &= 0xF0F0F0F;
	// add all four 8-bit chunks
	return (x * 0x01010101) >> 24;
}

//http://bits.stephan-brumme.com/lowestBitNotSet.html

uint32_t LowestNotSet(uint32_t x)
{
	return ~x & (x + 1);
}

unsigned int lowestBitNotSetSimple(unsigned int x)
{
	// to be consistent with lowestBitNotSet
	if (x == ~0)
		return 0;

	// shift right until finding a zero
	unsigned int result = 1;
	while ((x & result) != 0)
		result <<= 1; 
	return result;
}

#define LOWEST0(x)			(~x & (x + 1))

bool Ahead(uint32_t current[32], uint32_t test[32])
{
	for(uint8_t i=31; i>=0; --i)
	{	
		if(test[i] > current[i])
			return true;
		if(test[i] < current[i])
			return false;
	}

	return false;
}

uint32_t TwoPow(uint32_t pow)
{
	uint32_t r = 1;

	for(uint32_t i=0; i<pow; ++i)
		r <<= 1;

	return r;
}

uint32_t Pow(uint32_t base, uint32_t pow)
{
	uint32_t r = 1;

	for(uint32_t i=0; i<pow; ++i)
		r *= base;

	return r;
}

//fit combos
void FitComb(uint32_t startm)
{
}

struct ComboDef
{
	////vector for each possible combo.
	//then a vector for each possible direction number (e.g. 2 or 3) 
	//that are mixed according to direction op (e.g. "&= ~m" or "|= m").
	std::vector<uint32_t> m;
};

struct Combo
{
	uint32_t mval;
	uint32_t mset;
	uint32_t ovaldown;
	uint32_t osetdown;
	std::vector<uint32_t> chbs;
	std::list<ComboDef> cds;
};

#if 0
	Combo cm0;
	cm0.mset = mset;
	cm0.mval = mval[level];
	cm0.ovaldown = ovaldown;
	combs.push_back(cm0);

	std::list<ComboDef> combdefs2;
	uint8_t dirs = 3;

	for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
	{
		Combo* cms = &*cmit;
		
		uint32_t ch = ovalupshiftnotdownset & (~osetdown);
		uint32_t nch = ~ch;
		uint32_t chb = 0;

		while(chb = LOWEST0(nch))
		{
			cms->chbs.push_back(~chb);
			nch |= chb;
		}
		
		std::vector<uint32_t>* chbs = &cms->chbs;

		uint32_t startm = mval[level];
		//num of combos
		//uint32_t nc = TwoPow(chbs.size());
		//combo index

		//uint32_t m = startm;	//combo mask
		//uint32_t mod = 1;	//next bit to add

		{
			ComboDef cd0;
			for(uint8_t din=0; din<dirs; ++din)
				cd0.m.push_back(startm);
			combdefs2.push_back(cd0);
		}

		//auto cbit = chbs.begin();	//cycle through all change bits
		//cbin change bit index
		for(uint32_t cbin=0; cbin<chbs->size(); ++cbin)
		{
			std::list<ComboDef> combdefs3;

			/*
			for each combodef in combdefs2,
			spawn three more,
			with a chbit applied to a dif dir.
			*/

			for(auto cd2it=combdefs2.begin(); cd2it!=combdefs2.end; cd2it++)
			{
				ComboDef* cd = &*cd2it;

				for(uint8_t din=0; din<dirs; ++din)
				{
					ComboDef cd2; 
					uint32_t m = cd->m[din];
					m |= (*chbs)[cbin];
					cd2.m.push_back(m);
					combdefs3.push_back(cd2);
				}
			}

			combdefs2 = combdefs3;
		}

		cms->cds = combdefs2;

		//Combo cm;
		//cm.mset = cms->mset | m;
	}

	std::vector<Combo> combs2;

	for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
	{
		for(auto cdit=cmit->cds.begin(); cdit!=cmit->cds.end(); cdit++)
		{
			Combo cm2;
			cm2.mset  = cmit->mset;
			cm2.mval = cmit->mval;
			cm2.ovaldown = cmit->ovaldown;



			combs2.push_back(cm2);
		}
	}

	combs = combs2;
#endif

//bit effects
void Effect(uint32_t ch, std::vector<Combo>& combs, uint8_t dirs)
{
	uint32_t sch = (ch << 1) | (ch >> 31);
	//uint32_t ch = ovalupshiftnotdownset & (~osetdown);
	uint32_t nch = ~ch;
	uint8_t nset = CtSet(ch);
	uint32_t chb = 0;
	//uint8_t dirs = 3;

	//Combo* newc = (uint32_t*)malloc(nset * dirs * sizeof(Combo));
	uint32_t curlim = combs.size();
	uint32_t max = nset * dirs * combs.size();
	combs.resize(max);
	//newc[0] = 

	while(chb = LOWEST0(nch))
	{
		//std::vector<Combo> combs2;
		//for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
		//for(uint32_t cin=curlim-1; cin>=0; --cin)
		for(uint32_t cin=0; cin<curlim; ++cin)
		{
			Combo* cd = &combs[cin];
			Combo* newcd;
			//0
			{
				newcd = &combs[curlim * 2 + cin];
				*newcd = *cd;
				newcd->ovaldown |= chb;
				newcd->mval &= ~chb;
				newcd->mset |= chb;
				newcd->osetdown |= chb;
				//combs2.push_back(newcd);
				//combs[curlim * 2 + cin] = newcd;
			}
			//1
			{
				newcd = &combs[curlim * 1 + cin];
				*newcd = *cd;
				newcd->ovaldown &= ~chb;
				newcd->mval &= ~chb;
				newcd->mset |= chb;
				newcd->osetdown |= chb;
				//combs[curlim * 1 + cin] = newcd;
			}
			//2
			{
				newcd = &combs[curlim * 0 + cin];
				//*newcd = *cd;
				newcd->ovaldown |= chb;
				newcd->mval |= chb;
				newcd->mset |= chb;
				newcd->osetdown |= chb;
				//combs[curlim * 0 + cin] = newcd;
			}
		}

		curlim *= dirs;
		//combs = combs2;
		nch |= chb;
	}
}

//work backwards from last bracket level
bool HashFun3(uint32_t mval[32], uint32_t ovalup, uint32_t in, int8_t level=31)
{
	//o2[0] = 0
	//0 = o1[2] ^ ~( o1[0] & m2[0] )

	std::vector<Combo> combs;
	uint32_t mvaltry;

	uint32_t mset = 0;
	uint32_t ovaldown = 0;
	uint32_t osetdown = 0;

	if(!level)
	{
		osetdown = -1;
		ovaldown = in;
	}

	//get matching that are set:
	// ( ((mval & mvaldown) | (~mval & ~mvaldown)) & mvalset ) | ( ~mvalset )
	// ones = ( mvaldown & mvalset )
	// zeros = ( ~mvaldown & mvalset ) 
	//
	// mvalset |= 
	// just watch out when looping around for bin-1 value set by other end

	//todo send param u32 restriction for oval(level-1) and mval(level-1) is not needed (not set at level)

	//todo have a set of *ins and *outs for required values
	//and narrow down mval's at each level that match for all and pick first one that works
	//oval's will be diff for each in/out pair so keep *ovals

	//todo use bit logic on multiple bits at once to get m nd if o(level-1)[] is set etc
	//todo each level depends only on level-1
	//and at each level-1, [bit-1] and [bit] of oval(level-1) are needed (the only possible conflict)
	//masks are not shared between any (level,bin) sets
	//todo optimize using this

	//const uint8_t binmin = (bin+33)%32;
	//const uint8_t binmax = (bin);

	uint32_t ovaldownset = osetdown & ovaldown;
	uint32_t ovalshiftdownset = (ovaldownset << 1) | (ovaldownset >> 31);

	uint32_t ovalupshiftdownset = ovalup & ovalshiftdownset;

	uint32_t ovalnotdownset = (~osetdown) & ovaldown;
	uint32_t ovalshiftnotdownset = (ovalnotdownset << 1) | (ovalnotdownset >> 31);

	uint32_t ovalupshiftnotdownset = ovalup & ovalshiftnotdownset;

	uint32_t ovalupshiftdownsetmval = ovalupshiftdownset & mval[level];
	uint32_t ovalupshiftnotdownsetmval = ovalupshiftnotdownset & mval[level];

	uint32_t ovalupshiftdownsetnotmval = ovalupshiftdownset & (~mval[level]);
	uint32_t ovalupshiftnotdownsetnotmval = ovalupshiftnotdownset & (~mval[level]);

	if(mset & ovalupshiftdownset)
		return false;

	mval[level] |= ovalupshiftdownset;
	mset |= ovalupshiftdownset;

	//uint32_t ovalnotdownset = osetdown & (~ovaldown);
	//uint32_t ovalupnotdownset = ovalup & osetdown & (~ovaldown);

	//invalid value?
	if(ovalupshiftdownset & osetdown & (~ovaldown))
		return false;

	ovaldown |= ovalupshiftdownset & (~osetdown);
	osetdown |= ovalupshiftdownset & (~osetdown);
	
	ovaldownset = osetdown & ovaldown;
	ovalshiftdownset = (ovaldownset << 1) | (ovaldownset >> 31);
	ovalupshiftdownset = ovalup & ovalshiftdownset;
	//ovalnotdownset = (~osetdown) & ovaldown;

	if(ovalupshiftnotdownset & osetdown & ovaldown & mset & mval[level]) 
		return false;

	mset |= ovalupshiftnotdownset & osetdown & ovaldown;
	mval[level] &= ~(ovalupshiftnotdownset & osetdown & ovaldown);

	if(ovalupshiftnotdownset & osetdown & (~ovaldown) & mset & (~mval[level])) 
		return false;

	mset |= ovalupshiftnotdownset & osetdown & (~ovaldown);
	mval[level] |= ovalupshiftnotdownset & osetdown & (~ovaldown);

	//
	std::vector<Combo> combs2;
	for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
	//uint32_t oosz = combs.size();
	//for(uint32_t cmin=0; cmin<oosz; ++cmin)
	{
		ovaldownset = cmit->osetdown & cmit->ovaldown;
		ovalshiftdownset = (ovaldownset << 1) | (ovaldownset >> 31);
		ovalupshiftdownset = ovalup & ovalshiftdownset;

		uint32_t ch = ovalupshiftnotdownset & (~osetdown);
		uint32_t nch = ~ch;
		uint8_t nset = CtSet(ch);
		uint32_t chb = 0;
		uint8_t dirs = 3;

		//Combo* newc = (uint32_t*)malloc(nset * dirs * sizeof(Combo));
		//uint32_t curlim = combs.size();
		//uint32_t max = nset * dirs * combs.size();
		uint32_t max = Pow(dirs, nset);
		uint32_t osz = combs2.size();
		combs2.resize(osz + max);
		//newc[0] = 
		uint8_t bin = 0;
		uint32_t curlim = 1;

		combs2[osz] = *cmit;

		while(chb = LOWEST0(nch))
		{
			//std::vector<Combo> combs2;
			//for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
			//for(uint32_t cin=curlim-1; cin>=0; --cin)
			for(uint32_t cin=0; cin<curlim; ++cin)
			//for(uint32_t cin=0; cin<max; ++cin)
			{
				Combo* cd = &combs2[osz + cin];
				//Combo* cd = &*cmit;
				Combo* newcd;
				//0
				{
					//newcd = &combs[curlim * 2 + cin];
					newcd = &combs2[osz + curlim * 2 + cin];
					*newcd = *cd;
					newcd->ovaldown |= chb;
					newcd->mval &= ~chb;
					newcd->mset |= chb;
					newcd->osetdown |= chb;
					//combs2.push_back(newcd);
					//combs[curlim * 2 + cin] = newcd;
				}
				//1
				{
					//newcd = &combs[curlim * 1 + cin];
					newcd = &combs2[osz + curlim * 1 + cin];
					*newcd = *cd;
					newcd->ovaldown &= ~chb;
					newcd->mval &= ~chb;
					newcd->mset |= chb;
					newcd->osetdown |= chb;
					//combs[curlim * 1 + cin] = newcd;
				}
				//2
				{
					//newcd = &combs[curlim * 0 + cin];
					//*newcd = *cd;
					newcd = &combs2[osz + curlim * 0 + cin];
					newcd->ovaldown |= chb;
					newcd->mval |= chb;
					newcd->mset |= chb;
					newcd->osetdown |= chb;
					//combs[curlim * 0 + cin] = newcd;
				}
			}

			curlim *= dirs;
			//combs = combs2;
			nch |= chb;
		}
	}
	//free(newc);
	combs = combs2;
	combs2.clear();
	//

	//0
	//ovaldown |= ovalupshiftnotdownset & (~osetdown);
	//mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
	//1
	//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
	//mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
	//2
	//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
	//mval[level] |= ovalupshiftnotdownset & (~osetdown);
	//.
	//mset |= ovalupshiftnotdownset & (~osetdown);
	//osetdown |= ovalupshiftnotdownset & (~osetdown);

	uint32_t osetshiftdown = (osetdown << 1) | (osetdown >> 31);

	uint32_t b = ovalup & (~osetshiftdown) & osetdown & ovaldown;
	mval[level] &= ~b;
	mset |= b;
	uint32_t sb = (b << 1) | (b >> 31);
	osetdown |= sb;
	ovaldown &= ~sb;
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);

	b = ovalup & (~osetshiftdown) & osetdown & (~ovaldown);
	sb = (b << 1) | (b >> 31);
	//0
	mval[level] &= ~b;
	//1
	//mval |= b;
	//.
	mset |= b;
	osetdown |= sb;
	ovaldown &= ~sb;
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);

	b = ovalup & (~osetshiftdown) & (~osetdown);
	sb = (b << 1) | (b >> 31);
	//0
	ovaldown |= b;
	ovaldown |= sb;
	mval[level] &= ~b;
	//1
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//mval[level] &= ~b;
	//2
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//mval[level] |= b;
	//3
	//ovaldown |= b;
	//ovaldown &= ~sb;
	//mval[level] |= b;
	//.
	osetdown |= b;
	osetdown |= sb;
	mset |= b;

	uint32_t ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);

	if(~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & mset & mval[level])
		return false;

	mval[level] &= ~(~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset);
	mset |= ~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset;

	//0
	mval[level] |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;
	//1
	//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset);
	//.
	mset |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;

	// o(level-1)[bin]=0 and m(level)[bin]=1
	~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval[level];
	ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval[level];
	osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval[level];
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;
	//up all

	// o(level-1)[bin]=0 and m(level)[bin]=0
	// o(level-1)[bin]=1 and m(level)[bin]=0
	//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval
	//0
	ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval[level]);
	//1
	ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval[level];
	//.
	osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval[level];

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	// o(level-1)[bin]=0 and m(level)[bin]=0
	// o(level-1)[bin]=1 and m(level)[bin]=0
	// o(level-1)[bin]=0 and m(level)[bin]=1
	//0
	ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
	mval[level] &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
	//1
	//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
	//mval[level] &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
	//2
	//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
	//mval |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
	//.
	osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
	mset |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	if(~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & mset & ~mval[level])
		return false;

	mval[level] |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
	mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;

	//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ~ovaldown ret f
	if(~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ~ovaldown)
		return false;

	ovaldown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;
	osetdown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	if(~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & mset & ~mval[level])
		return false;

	mval[level] |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;
	mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;

	//

	ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval[level]);
	osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval[level];

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval[level];
	osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval[level];

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	//0
	ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
	mval[level] &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
	//1
	//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
	//mval |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
	//.
	osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
	mset |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval[level];
	sb = (b << 1) | (b >> 31);
	ovaldown |= sb;
	osetdown |= sb;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval[level];
	sb = (b << 1) | (b >> 31);
	ovaldown |= sb;
	osetdown |= sb;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & ~mset;
	sb = (b << 1) | (b >> 31);
	//0
	ovaldown |= sb;
	//1
	//ovaldown &= ~sb;
	//.
	osetdown |= sb;
	mval[level] |= sb;
	mset |= sb;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & ~osetdown & mset & mval[level];
	sb = (b << 1) || (b >> 31);
	//0
	ovaldown |= b;
	ovaldown &= ~sb;
	//1
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//.
	osetdown |= b;
	osetdown |= sb;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval[level];
	sb = (b << 1) || (b >> 31);
	//0
	ovaldown |= b;
	ovaldown |= sb;
	//1
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//.
	osetdown |= b;
	osetdown |= sb;

	//up
	osetshiftdown = (osetdown << 1) | (osetdown >> 31);
	ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
	ovalshiftdownset = osetshiftdown & ovalshiftdown;

	b = ~ovalup & ~osetshiftdown & ~osetdown & ~mset;
	sb = (b << 1) || (b >> 31);
	//0
	ovaldown |= b;
	ovaldown |= sb;
	mval[level] &= ~b;
	//1
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//mval[level] &= ~b;
	//2
	//ovaldown |= b;
	//ovaldown &= ~sb;
	//mval[level] |= b;
	//3
	//ovaldown &= ~b;
	//ovaldown |= sb;
	//mval[level] |= b;
	//.
	osetdown |= b;
	osetdown |= sb;
	mset |= b;

	//28

	//should be set from higher level
	//ovalup
	if(ovalup&(1<<bin))
	{
		//ovalup[bin] = 1

		//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
		//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1

		//check oset(level-1)[#-1]
		//todo use bit logic to figure what m is and if o(level-1)[] is set etc

		//ovalupshiftdownset
		if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
		{
			//i[#-1]=1 and ~( i[#] & m0[#] )=0
			//so i[#]=1 and m0[#]=1

			//todo m(level)[#] can never be set by anything other than (level)[#] so dont check
			//0 later last bit
			//mset & ovalupshiftdownset
			if(mset&(1<<bin))
			{
				//ovalupshiftdownset & mval
				if(mval[level]&(1<<bin))
				{
					//already set
					//m0[#]=1
				}
				//ovalupshiftdownset & ~mval
				else
				{
					//m0[#]=0
					return false;
				}
			}
			//mval |= ovalupshiftdownset
			//mset |= ovalupshiftdownset
			else
			{
				mset |= (1<<bin);
				mval[level] |= (1<<bin);
			}

			//ovaldownset
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//o(level-1)[#]=1
			}
			//check oset(level-1)[#+1]
			//ovalnotdownset
			//(ovalnotdownset & ovalupshiftdownset)
			//ovalupshiftdownset & osetdown & (~ovaldown)
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//i[#]=0
				return false;
			}
			//!oset(level-1)[#]
			else
			{
				//i[#]=1

				//osetdown |= ovalupshiftdownset & (~osetdown);
				//ovaldown |= ovalupshiftdownset & (~osetdown);

				osetdown |= (1<<bin);
				ovaldown |= (1<<bin);
			}
		}
		//else i[#-1]=0
		//ovalupshiftnotdownset
		else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
		{
			//i[#-1]=0 and ~( i[#] & m0[#] )=1

			//so i[#]=0 and m0[#]=0
			//or i[#]=1 and m0[#]=0
			//or i[#]=0 and m0[#]=1

			//check oset(level-1)[#]
			//ovalupshiftnotdownset & osetdown & ovaldown
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//i[#]=1

				//todo check already mset
				//so m0[#]=0

				//if(ovalupshiftnotdownset & osetdown & ovaldown & mset & mval) return false;

				//mset |= ovalupshiftnotdownset & osetdown & ovaldown;
				//mval &= ~(ovalupshiftnotdownset & osetdown & ovaldown);

				mset |= (1<<bin);
				mval[level] &= ~(1<<bin);
			}
			//ovalupshiftnotdownset & osetdown & (~ovaldown)
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//i[#]=0


				//if(ovalupshiftnotdownset & osetdown & (~ovaldown) & mset & ~mval) return false;

				//mset |= ovalupshiftnotdownset & osetdown & (~ovaldown);
				//mval |= ovalupshiftnotdownset & osetdown & (~ovaldown);

				//todo check already mset
				//so m0[#]=0
				//or m0[#]=1
				mset |= (1<<bin);
				mval[level] |= (1<<bin);
				//todo or other choice skip
			}
			//!oset(level-1)[#]
			//ovalupshiftnotdownset & (~osetdown)
			else
			{
				//todo

				//choice
				//so i[#]=1 and m0[#]=0
				//or i[#]=0 and m0[#]=0
				//or i[#]=0 and m0[#]=1

#if 0
				//0
				ovaldown |= ovalupshiftnotdownset & (~osetdown);
				mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
				//1
				//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
				//mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
				//2
				//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
				//mval[level] |= ovalupshiftnotdownset & (~osetdown);
				//.
				mset |= ovalupshiftnotdownset & (~osetdown);
				osetdown |= ovalupshiftnotdownset & (~osetdown);

				if(!*skip)
				{

				}
				else
				{
					--(*skip);

					if(!*skip)
					{
					}
					else
					{
						--(*skip);

						if(!*skip)
						{
						}
						else
							--(*skip);
					}
				}
#endif

				osetdown |= (1<<bin);
				ovaldown &= ~(1<<bin);

				mset |= (1<<bin);
				mval[level] |= (1<<bin);
			}

			//they can't both be 1 and set
			//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
		}
		//so i[#-1]=1 and ~( i[#] & m0[#] )=0
		//or i[#-1]=0 and ~( i[#] & m0[#] )=1
		//ovalupshiftnotdownset
		//ovalup & (~osetshiftdown)
		else if( !(osetdown&(1<<binmin)) )
		{
			//todo
			//choice

			//check oset(level-1)[#]
			//ovalup & (~osetshiftdown) & osetdown & ovaldown
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//i[#]=1

#if 0
				uint32_t b = ovalup & (~osetshiftdown) & osetdown & ovaldown;
				mval &= ~b;
				mset |= b;
				uint32_t sb = (b << 1) | (b >> 31);
				osetdown |= sb;
				ovaldown &= ~sb;
				osetshiftdown = (osetdown << 1) | (osetdown >> 31);
#endif

				//todo check already mset
				//so m0[#]=0
				mset |= (1<<bin);
				mval[level] &= ~(1<<bin);

				//so i[#-1]=0
				osetdown |= (1<<binmin);
				ovaldown &= ~(1<<binmin);
			}
			//ovalup & (~osetshiftdown) & osetdown & (~ovaldown)
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//i[#]=0

#if 0
				uint32_t b = ovalup & (~osetshiftdown) & osetdown & (~ovaldown);
				uint32_t sb = (b << 1) | (b >> 31);
				//0
				mval &= ~b;
				//1`
				mval |= b;
				//.
				mset |= b;
				osetdown |= sb;
				ovaldown &= ~sb;
				osetshiftdown = (osetdown << 1) | (osetdown >> 31);
#endif

				//todo check already mset
				//so m0[#]=0
				//or m0[#]=1
				mset |= (1<<bin);
				mval[level] |= (1<<bin);
				//todo or other choice skip

				//so i[#-1]=0
				osetdown |= (1<<binmin);
				ovaldown &= ~(1<<binmin);
			}
			//!oset(level-1)[#]
			//ovalup & (~osetshiftdown) & (~osetdown)
			else
			{
#if 0
				b = ovalup & (~osetshiftdown) & (~osetdown);
				sb = (b << 1) | (b >> 31);
				//0
				ovaldown |= b;
				ovaldown |= sb;
				mval[level] &= ~b;
				//1
				ovaldown &= ~b;
				ovaldown |= sb;
				mval[level] &= ~b;
				//2
				ovaldown &= ~b;
				ovaldown |= sb;
				mval[level] |= b;
				//
				ovaldown |= b;
				ovaldown &= ~sb;
				mval[level] |= b;
				//.
				osetdown |= b;
				osetdown |= sb;
				mset |= b;
#endif


				//choice
				//so i[#-1]=1 and i[#]=1 and m0[#]=0
				//or i[#-1]=1 and i[#]=0 and m0[#]=0
				//or i[#-1]=1 and i[#]=0 and m0[#]=1
				//or i[#-1]=0 and i[#]=1 and m0[#]=1

				osetdown |= (1<<bin);
				ovaldown &= ~(1<<bin);

				mset |= (1<<bin);
				mval[level] |= (1<<bin);

				osetdown |= (1<<binmin);
				ovaldown &= ~(1<<binmin);
			}
		}
	}
	else
	{
		//ovalup[bin] = 0
		//~ovalup

		//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
		//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

		//~ovalup & osetshiftdown & ovalshiftdown
		if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
		{
			// o(level-1)[(bin-1)%nb]=1
			// so ~( o(level-1)[bin] & m(level)[bin] )=1

			//so o(level-1)[bin]=0 and m(level)[bin]=0
			//or o(level-1)[bin]=1 and m(level)[bin]=0
			//or o(level-1)[bin]=0 and m(level)[bin]=1

			//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=1

				//so m(level)[bin]=0

				//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & mset & mval
				//retu fal
				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
					return false;
				}
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
				}
				// !(mset&(1<<bin))
				//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset
				else
				{
					//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset;
					//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset);
					mset |= (1<<bin);
					mval[level] &= ~(1<<bin);
				}
			}
			//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=0

				//choice
				//so m(level)[bin]=0
				//or m(level)[bin]=1

				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
				}
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
				}
				// !(mset&(1<<bin))
				//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset
				else
				{
					//0
					//mval |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;
					//1
					//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset);
					//.
					//mset |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;

					//choice
					mset |= (1<<bin);
					mval[level] &= ~(1<<bin);
				}
			}
			// !(osetdown&(1<<bin))
			//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown
			else
			{
				//choice
				//so o(level-1)[bin]=0 and m(level)[bin]=0
				//or o(level-1)[bin]=1 and m(level)[bin]=0
				//or o(level-1)[bin]=0 and m(level)[bin]=1

				// o(level-1)[bin]=0 and m(level)[bin]=1
				//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval
				//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
				//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
				//osetshiftdown = (osetdown << 1) | (osetdown >> 31);
				//ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
				//ovalshiftdownset = osetshiftdown & ovalshiftdown;
				//up all

				// o(level-1)[bin]=0 and m(level)[bin]=0
				// o(level-1)[bin]=1 and m(level)[bin]=0
				//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval
				//0
				//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval);
				//1
				//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;
				//.
				//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;

				// o(level-1)[bin]=0 and m(level)[bin]=0
				// o(level-1)[bin]=1 and m(level)[bin]=0
				// o(level-1)[bin]=0 and m(level)[bin]=1
				//0
				//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
				//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
				//1
				//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
				//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
				//2
				//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
				//mval |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
				//.
				//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
				//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
			}
		}
		//2
		//~ovalup & osetshiftdown & ~ovalshiftdown
		else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
		{
			// o(level-1)[(bin-1)%nb]=0
			// so ~( o(level-1)[bin] & m(level)[bin] )=0

			//so o(level-1)[bin]=1 and m(level)[bin]=1

			//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=1

				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
				}
				//
				//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & mset & ~mval -> ret f
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
					return false;
				}
				// !(mset&(1<<bin))
				//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset
				else
				{
					//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
					//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}

				//2^32 values
				//(2^32)^32 assignments
				//
			}
			//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ~ovaldown ret f
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=0
				return false;
			}
			// !(osetdown&(1<<bin))
			//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown
			else
			{
				//choice
				osetdown |= (1<<bin);
				ovaldown |= (1<<bin);

				//ovaldown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;
				//osetdown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;

				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
				}
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & mset & ~mval -> ret f

					//m(level)[bin]=0
					return false;
				}
				// !(mset&(1<<bin))
				else
				{
					//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;
					//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;

					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}
			}
		}
		//3
		// !(osetdown&(1<<binmin))
		//~ovalup & ~osetshiftdown
		else
		{
			//choice
			//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
			//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
			//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
			//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

			//~ovalup & ~osetshiftdown & osetdown & ovaldown
			if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=1
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

				//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval
				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
					//so o(level-1)[(bin-1)%nb]=0

					//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval);
					//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval;

					osetdown |= (1<<binmin);
					ovaldown &= ~(1<<binmin);
				}
				//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
					//so o(level-1)[(bin-1)%nb]=1

					//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;
					//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);
				}
				// !(mset&(1<<bin))
				//~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=0 and m(level)[bin]=1

					//0
					//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
					//mval &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
					//1
					//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
					//mval |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
					//.
					//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
					//mset |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;

					osetdown |= (1<<binmin);
					ovaldown &= ~(1<<binmin);

					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}

				//2^32 values
				//(2^32)^32 assignments
				//
			}
			//~ovalup & ~osetshiftdown & osetdown & ~ovaldown
			else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
			{
				//o(level-1)[bin]=0

				//choice
				//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1

				//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval
				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
					//so o(level-1)[(bin-1)%nb]=1

					//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval;
					//bs = (b << 1) | (b >> 31);
					//ovaldown |= bs;
					//osetdown |= bs;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);
				}
				//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
					//so o(level-1)[(bin-1)%nb]=1

					//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval;
					//bs = (b << 1) | (b >> 31);
					//ovaldown |= bs;
					//osetdown |= bs;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);
				}
				// !(mset&(1<<bin))
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=1

					//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown;
					//bs = (b << 1) | (b >> 31);
					//ovaldown |= bs;
					//osetdown |= bs;

					//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & ~mset;
					//bs = (b << 1) | (b >> 31);
					//0
					//ovaldown |= bs;
					//1
					//ovaldown &= ~bs;
					//.
					//osetdown |= bs;
					//mval |= bs;
					//mset |= bs;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);

					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}
			}
			// !(osetdown&(1<<bin))
			//4
			//~ovalup & ~osetshiftdown & ~osetdown
			else
			{
				//choice
				//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
				//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
				//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

				//osetdown |= (1<<bin);
				//ovaldown |= (1<<bin);

				//~ovalup & ~osetshiftdown & ~osetdown & mset & mval
				if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=1
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
					//choice

					//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & mval;
					//sb = (b << 1) || (b >> 31);
					//0
					//ovaldown |= b;
					//ovaldown &= ~sb;
					//1
					//ovaldown &= ~b;
					//ovaldown |= sb;
					//.
					//osetdown |= b;
					//osetdown |= sb;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);

					osetdown |= (1<<bin);
					ovaldown &= ~(1<<bin);
				}
				//~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval
				else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
				{
					//m(level)[bin]=0
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//choice

					//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval;
					//sb = (b << 1) || (b >> 31);
					//0
					//ovaldown |= b;
					//ovaldown |= sb;
					//1
					//ovaldown &= ~b;
					//ovaldown |= sb;
					//.
					//osetdown |= b;
					//osetdown |= sb;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);

					osetdown |= (1<<bin);
					ovaldown &= ~(1<<bin);
				}
				// !(mset&(1<<bin))
				//~ovalup & ~osetshiftdown & ~osetdown & ~mset
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					//b = ~ovalup & ~osetshiftdown & ~osetdown & ~mset;
					//sb = (b << 1) || (b >> 31);
					//0
					//ovaldown |= b;
					//ovaldown |= sb;
					//mval &= ~b;
					//1
					//ovaldown &= ~b;
					//ovaldown |= sb;
					//mval &= ~b;
					//2
					//ovaldown |= b;
					//ovaldown &= ~sb;
					//mval |= b;
					//3
					//ovaldown &= ~b;
					//ovaldown |= sb;
					//mval |= b;
					//.
					//osetdown |= b;
					//osetdown |= sb;
					//mset |= b;

					osetdown |= (1<<binmin);
					ovaldown |= (1<<binmin);

					osetdown |= (1<<bin);
					ovaldown &= ~(1<<bin);

					mset |= (1<<bin);
					mval[level] |= (1<<bin);
				}
			}
		}
	}

	if(level)
		HashFun3(mval, ovaldown, in, level-1);
}

//work backwards from last bracket level
bool HashFun4(uint32_t mval[32], uint32_t ovalup, uint32_t in, bool nolim, int8_t level=31)
{
	//o2[0] = 0
	//0 = o1[2] ^ ~( o1[0] & m2[0] )

	std::vector<Combo> combs;

	uint32_t mset = 0;
	uint32_t ovaldown = 0;
	uint32_t osetdown = 0;

	//	for mval[level+1] no change -> nolim=false
	//low = mval[level]
	//high = -1
	//	for mval[level+1]+ -> nolim=true
	//low = 0
	//high = -1

	uint32_t mlow;
	uint32_t mhigh = -1;

#if 1
	if(nolim)
		mlow = 0;
	else
		mlow = mval[level];
#endif
	
	uint32_t mcur = mlow;

	if(!level)
	{
		osetdown = -1;
		ovaldown = in;

		//todo
		return;
	}

	Combo cm0;
	cm0.mset = mset;
	cm0.mval = mval[level];
	cm0.osetdown = osetdown;
	cm0.ovaldown = ovaldown;
	combs.push_back(cm0);

	const uint32_t first = (-1) & ~(1<<31);

	const uint32_t oup0 = first & ~ovalup;
	const uint32_t oup1 = first & ovalup;
	const uint32_t oupboth = first;

	//oupboth
	std::vector<Combo> combs2;
	for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
	//uint32_t oosz = combs.size();
	//for(uint32_t cmin=0; cmin<oosz; ++cmin)
	{
		uint32_t ch = oupboth;
		uint32_t nch = ~ch;
		//uint8_t nset = CtSet(ch);
		uint8_t nset = 31;
		uint32_t chb = 0;
		uint8_t dirs = 2;

		//Combo* newc = (uint32_t*)malloc(nset * dirs * sizeof(Combo));
		//uint32_t curlim = combs.size();
		//uint32_t max = nset * dirs * combs.size();
		//uint32_t max = Pow(dirs, nset);
		uint32_t max = TwoPow(nset);
		uint32_t osz = combs2.size();
		combs2.resize(osz + max);
		//newc[0] = 
		uint8_t bin = 0;
		uint32_t curlim = 1;

		combs2[osz] = *cmit;

		while(chb = LOWEST0(nch))
		{
			uint32_t schb = (chb << 1) | (chb >> 31);

			//std::vector<Combo> combs2;
			//for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
			//for(uint32_t cin=curlim-1; cin>=0; --cin)
			for(uint32_t cin=0; cin<curlim; ++cin)
			//for(uint32_t cin=0; cin<max; ++cin)
			{
				Combo* cd = &combs2[osz + cin];
				//Combo* cd = &*cmit;
				Combo* newcd;
				//2
				//or i[#-1]=1 and i[#]=0 and m0[#]=1
				{
					//newcd = &combs[curlim * 0 + cin];
					newcd = &combs2[osz + curlim * 1 + cin];
					*newcd = *cd;
					newcd->ovaldown |= schb;
					newcd->ovaldown &= ~chb;
					newcd->mval |= chb;
					newcd->mset |= chb;
					newcd->osetdown |= chb | schb;
					//combs[curlim * 0 + cin] = newcd;
				}
				//3
				//or i[#-1]=0 and i[#]=1 and m0[#]=1
				{
					//newcd = &combs[curlim * 0 + cin];
					newcd = &combs2[osz + curlim * 0 + cin];
					//*newcd = *cd;
					newcd->ovaldown &= ~schb;
					newcd->ovaldown |= chb;
					newcd->mval |= chb;
					newcd->mset |= chb;
					newcd->osetdown |= chb | schb;
					//combs[curlim * 0 + cin] = newcd;
				}
			}

			curlim *= dirs;
			//combs = combs2;
			nch |= chb;
		}
	}
	//free(newc);
	combs = combs2;
	combs2.clear();
	//

#if 0
		//2
			//or i[#-1]=1 and i[#]=0 and m0[#]=1
			{
				cm0 = *cmit;

				cm0.osetdown |= (1<<binmin);
				cm0.ovaldown |= (1<<binmin);

				cm0.osetdown |= (1<<bin);
				cm0.ovaldown &= ~(1<<bin);

				cm0.mset |= (1<<bin);
				cm0.mval |= (1<<bin);

				combs2.push_back(cm0);
			}
			//3
			//or i[#-1]=0 and i[#]=1 and m0[#]=1
			{
				cm0 = *cmit;

				cm0.osetdown |= (1<<binmin);
				cm0.ovaldown &= ~(1<<binmin);

				cm0.osetdown |= (1<<bin);
				cm0.ovaldown |= (1<<bin);

				cm0.mset |= (1<<bin);
				cm0.mval |= (1<<bin);

				combs2.push_back(cm0);
			}
#endif

	//combs.push_back(cm0);

	//28

	//1 up
	for(uint8_t bin=0; bin<31; ++bin)
	{
		//0:1:1
		//1:10:2
		//2:100:4
		//3:1000:8
		//4:10000:16
		//5:100000:32
		//6:1000000:64
		uint8_t binmin = (bin+31)&(31);
		std::vector<Combo> combs2;

		for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
		{
			osetdown = cmit->osetdown;
			ovaldown = cmit->ovaldown;
			mval[level] = cmit->mval;
			mset = cmit->mset;

			//should be set from higher level
			//ovalup
			if(ovalup&(1<<bin))
			{
				//ovalup[bin] = 1

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1

				//check oset(level-1)[#-1]
				//todo use bit logic to figure what m is and if o(level-1)[] is set etc
				
				//can't be set unless level 0 or bin 0 at end
				//if( !(osetdown&(1<<binmin)) )
				{
					//todo
					//choice
					//!oset(level-1)[#]
					//ovalup & (~osetshiftdown) & (~osetdown)
					// !(osetdown(1<<bin))
					//can't be set unless level 0 or bin 0 at end
					//else
					{
		#if 0
						b = ovalup & (~osetshiftdown) & (~osetdown);
						sb = (b << 1) | (b >> 31);
						//0
						ovaldown |= b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//1
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//2
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] |= b;
						//
						ovaldown |= b;
						ovaldown &= ~sb;
						mval[level] |= b;
						//.
						osetdown |= b;
						osetdown |= sb;
						mset |= b;
		#endif


						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0

						
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//0
						//so i[#-1]=1 and i[#]=1 and m0[#]=0
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown |= (1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval &= ~(1<<bin);

							combs2.push_back(cm0);
						}
						//1
						//or i[#-1]=1 and i[#]=0 and m0[#]=0
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown &= ~(1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval &= ~(1<<bin);

							combs2.push_back(cm0);
						}
#if 0
						//2
						//or i[#-1]=1 and i[#]=0 and m0[#]=1
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown &= ~(1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval |= (1<<bin);

							combs2.push_back(cm0);
						}
						//3
						//or i[#-1]=0 and i[#]=1 and m0[#]=1
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown &= ~(1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown |= (1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval |= (1<<bin);

							combs2.push_back(cm0);
						}
#endif
					}
				}
			}
			//!ovalup[bin]
			else
			{
				//ovalup[bin] = 0
				//~ovalup

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

				//3
				// !(osetdown&(1<<binmin))
				//~ovalup & ~osetshiftdown
				//can't be set unless level 0 or bin 0 at end
				//else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					// !(osetdown&(1<<bin))
					//4
					//~ovalup & ~osetshiftdown & ~osetdown
					//can't be set unless level 0 or bin 0 at end
					//else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//osetdown |= (1<<bin);
						//ovaldown |= (1<<bin);
						
						// !(mset&(1<<bin))
						//~ovalup & ~osetshiftdown & ~osetdown & ~mset
						//can't be set unless level 0 or bin 0 at end
						//else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

							//b = ~ovalup & ~osetshiftdown & ~osetdown & ~mset;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown |= sb;
							//mval &= ~b;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval &= ~b;
							//2
							//ovaldown |= b;
							//ovaldown &= ~sb;
							//mval |= b;
							//3
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval |= b;
							//.
							//osetdown |= b;
							//osetdown |= sb;
							//mset |= b;

							//osetdown |= (1<<binmin);
							//ovaldown |= (1<<binmin);

							//osetdown |= (1<<bin);
							//ovaldown &= ~(1<<bin);

							//mset |= (1<<bin);
							//mval[level] |= (1<<bin);
						}
						
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
						
						//0
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown &= ~(1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval &= ~(1<<bin);

							combs2.push_back(cm0);
						}
						//1
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown |= (1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval &= ~(1<<bin);

							combs2.push_back(cm0);
						}
#if 0
						//2
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown |= (1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown &= ~(1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval |= (1<<bin);

							combs2.push_back(cm0);
						}
						//3
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
						{
							cm0 = *cmit;
							
							cm0.osetdown |= (1<<binmin);
							cm0.ovaldown &= ~(1<<binmin);

							cm0.osetdown |= (1<<bin);
							cm0.ovaldown |= (1<<bin);

							cm0.mset |= (1<<bin);
							cm0.mval |= (1<<bin);

							combs2.push_back(cm0);
						}
#endif
					}
				}
			}

			//2
			//or i[#-1]=1 and i[#]=0 and m0[#]=1
			{
				cm0 = *cmit;

				cm0.osetdown |= (1<<binmin);
				cm0.ovaldown |= (1<<binmin);

				cm0.osetdown |= (1<<bin);
				cm0.ovaldown &= ~(1<<bin);

				cm0.mset |= (1<<bin);
				cm0.mval |= (1<<bin);

				combs2.push_back(cm0);
			}
			//3
			//or i[#-1]=0 and i[#]=1 and m0[#]=1
			{
				cm0 = *cmit;

				cm0.osetdown |= (1<<binmin);
				cm0.ovaldown &= ~(1<<binmin);

				cm0.osetdown |= (1<<bin);
				cm0.ovaldown |= (1<<bin);

				cm0.mset |= (1<<bin);
				cm0.mval |= (1<<bin);

				combs2.push_back(cm0);
			}
		}
		
		combs = combs2;
	}

	//2 down
	{
		uint8_t bin = 31;
		uint8_t binmin = 30;

		std::vector<Combo> combs2;

		for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
		{
			osetdown = cmit->osetdown;
			ovaldown = cmit->ovaldown;
			mval[level] = cmit->mval;
			mset = cmit->mset;

			//should be set from higher level
			//ovalup
			if(ovalup&(1<<bin))
			{
				//ovalup[bin] = 1

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1

				//check oset(level-1)[#-1]
				//todo use bit logic to figure what m is and if o(level-1)[] is set etc

				//ovalupshiftdownset
				if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
				{
					//i[#-1]=1 and ~( i[#] & m0[#] )=0
					//so i[#]=1 and m0[#]=1

					//todo m(level)[#] can never be set by anything other than (level)[#] so dont check
					//0 later last bit
					//mset & ovalupshiftdownset
					if(mset&(1<<bin))
					{
						//ovalupshiftdownset & mval
						if(mval[level]&(1<<bin))
						{
							//already set
							//m0[#]=1
						}
						//ovalupshiftdownset & ~mval
						else
						{
							//m0[#]=0
							return false;
						}
					}
					//mval |= ovalupshiftdownset
					//mset |= ovalupshiftdownset
					else
					{
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//ovaldownset
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[#]=1
					}
					//check oset(level-1)[#+1]
					//ovalnotdownset
					//(ovalnotdownset & ovalupshiftdownset)
					//ovalupshiftdownset & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0
						return false;
					}
					//!oset(level-1)[#]
					else
					{
						//i[#]=1

						//osetdown |= ovalupshiftdownset & (~osetdown);
						//ovaldown |= ovalupshiftdownset & (~osetdown);

						osetdown |= (1<<bin);
						ovaldown |= (1<<bin);
					}
				}
				//else i[#-1]=0
				//ovalupshiftnotdownset
				else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
				{
					//i[#-1]=0 and ~( i[#] & m0[#] )=1

					//so i[#]=0 and m0[#]=0
					//or i[#]=1 and m0[#]=0
					//or i[#]=0 and m0[#]=1

					//check oset(level-1)[#]
					//ovalupshiftnotdownset & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//i[#]=1

						//todo check already mset
						//so m0[#]=0

						//if(ovalupshiftnotdownset & osetdown & ovaldown & mset & mval) return false;

						//mset |= ovalupshiftnotdownset & osetdown & ovaldown;
						//mval &= ~(ovalupshiftnotdownset & osetdown & ovaldown);

						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);
					}
					//ovalupshiftnotdownset & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0


						//if(ovalupshiftnotdownset & osetdown & (~ovaldown) & mset & ~mval) return false;

						//mset |= ovalupshiftnotdownset & osetdown & (~ovaldown);
						//mval |= ovalupshiftnotdownset & osetdown & (~ovaldown);

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip
					}
					//!oset(level-1)[#]
					//ovalupshiftnotdownset & (~osetdown)
					else
					{
						//todo

						//choice
						//so i[#]=1 and m0[#]=0
						//or i[#]=0 and m0[#]=0
						//or i[#]=0 and m0[#]=1

		#if 0
						//0
						ovaldown |= ovalupshiftnotdownset & (~osetdown);
						mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
						//1
						//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
						//mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
						//2
						//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
						//mval[level] |= ovalupshiftnotdownset & (~osetdown);
						//.
						mset |= ovalupshiftnotdownset & (~osetdown);
						osetdown |= ovalupshiftnotdownset & (~osetdown);

						if(!*skip)
						{

						}
						else
						{
							--(*skip);

							if(!*skip)
							{
							}
							else
							{
								--(*skip);

								if(!*skip)
								{
								}
								else
									--(*skip);
							}
						}
		#endif

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//they can't both be 1 and set
					//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
				}
				//so i[#-1]=1 and ~( i[#] & m0[#] )=0
				//or i[#-1]=0 and ~( i[#] & m0[#] )=1
				//ovalupshiftnotdownset
				//ovalup & (~osetshiftdown)
				else if( !(osetdown&(1<<binmin)) )
				{
					//todo
					//choice

					//check oset(level-1)[#]
					//ovalup & (~osetshiftdown) & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//i[#]=1

		#if 0
						uint32_t b = ovalup & (~osetshiftdown) & osetdown & ovaldown;
						mval &= ~b;
						mset |= b;
						uint32_t sb = (b << 1) | (b >> 31);
						osetdown |= sb;
						ovaldown &= ~sb;
						osetshiftdown = (osetdown << 1) | (osetdown >> 31);
		#endif

						//todo check already mset
						//so m0[#]=0
						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);

						//so i[#-1]=0
						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
					//ovalup & (~osetshiftdown) & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0

		#if 0
						uint32_t b = ovalup & (~osetshiftdown) & osetdown & (~ovaldown);
						uint32_t sb = (b << 1) | (b >> 31);
						//0
						mval &= ~b;
						//1`
						mval |= b;
						//.
						mset |= b;
						osetdown |= sb;
						ovaldown &= ~sb;
						osetshiftdown = (osetdown << 1) | (osetdown >> 31);
		#endif

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip

						//so i[#-1]=0
						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
					//!oset(level-1)[#]
					//ovalup & (~osetshiftdown) & (~osetdown)
					else
					{
		#if 0
						b = ovalup & (~osetshiftdown) & (~osetdown);
						sb = (b << 1) | (b >> 31);
						//0
						ovaldown |= b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//1
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//2
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] |= b;
						//
						ovaldown |= b;
						ovaldown &= ~sb;
						mval[level] |= b;
						//.
						osetdown |= b;
						osetdown |= sb;
						mset |= b;
		#endif


						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=1

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);

						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
				}
			}
			else
			{
				//ovalup[bin] = 0
				//~ovalup

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

				//~ovalup & osetshiftdown & ovalshiftdown
				if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=1
					// so ~( o(level-1)[bin] & m(level)[bin] )=1

					//so o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[bin]=0 and m(level)[bin]=1

					//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						//so m(level)[bin]=0

						//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & mset & mval
						//retu fal
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							return false;
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset
						else
						{
							//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset;
							//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset);
							mset |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so m(level)[bin]=0
						//or m(level)[bin]=1

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset
						else
						{
							//0
							//mval |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;
							//1
							//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset);
							//.
							//mset |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;

							//choice
							mset |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					// !(osetdown&(1<<bin))
					//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown
					else
					{
						//choice
						//so o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[bin]=0 and m(level)[bin]=1

						// o(level-1)[bin]=0 and m(level)[bin]=1
						//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
						//osetshiftdown = (osetdown << 1) | (osetdown >> 31);
						//ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
						//ovalshiftdownset = osetshiftdown & ovalshiftdown;
						//up all

						// o(level-1)[bin]=0 and m(level)[bin]=0
						// o(level-1)[bin]=1 and m(level)[bin]=0
						//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval
						//0
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval);
						//1
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;
						//.
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;

						// o(level-1)[bin]=0 and m(level)[bin]=0
						// o(level-1)[bin]=1 and m(level)[bin]=0
						// o(level-1)[bin]=0 and m(level)[bin]=1
						//0
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//1
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//2
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//mval |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//.
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
					}
				}
				//2
				//~ovalup & osetshiftdown & ~ovalshiftdown
				else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=0
					// so ~( o(level-1)[bin] & m(level)[bin] )=0

					//so o(level-1)[bin]=1 and m(level)[bin]=1

					//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						//
						//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & mset & ~mval -> ret f
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset
						else
						{
							//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
							//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ~ovaldown ret f
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0
						return false;
					}
					// !(osetdown&(1<<bin))
					//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown
					else
					{
						//choice
						osetdown |= (1<<bin);
						ovaldown |= (1<<bin);

						//ovaldown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;
						//osetdown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & mset & ~mval -> ret f

							//m(level)[bin]=0
							return false;
						}
						// !(mset&(1<<bin))
						else
						{
							//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;
							//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
				//3
				// !(osetdown&(1<<binmin))
				//~ovalup & ~osetshiftdown
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					//~ovalup & ~osetshiftdown & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=0

							//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval);
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval;

							osetdown |= (1<<binmin);
							ovaldown &= ~(1<<binmin);
						}
						//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						// !(mset&(1<<bin))
						//~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=0 and m(level)[bin]=1

							//0
							//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//mval &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
							//1
							//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
							//mval |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//.
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//mset |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;

							osetdown |= (1<<binmin);
							ovaldown &= ~(1<<binmin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					//~ovalup & ~osetshiftdown & osetdown & ~ovaldown
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1

						//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						// !(mset&(1<<bin))
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & ~mset;
							//bs = (b << 1) | (b >> 31);
							//0
							//ovaldown |= bs;
							//1
							//ovaldown &= ~bs;
							//.
							//osetdown |= bs;
							//mval |= bs;
							//mset |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
					// !(osetdown&(1<<bin))
					//4
					//~ovalup & ~osetshiftdown & ~osetdown
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//osetdown |= (1<<bin);
						//ovaldown |= (1<<bin);

						//~ovalup & ~osetshiftdown & ~osetdown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
							//choice

							//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & mval;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown &= ~sb;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//.
							//osetdown |= b;
							//osetdown |= sb;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);
						}
						//~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//choice

							//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown |= sb;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//.
							//osetdown |= b;
							//osetdown |= sb;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);
						}
						// !(mset&(1<<bin))
						//~ovalup & ~osetshiftdown & ~osetdown & ~mset
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

							//b = ~ovalup & ~osetshiftdown & ~osetdown & ~mset;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown |= sb;
							//mval &= ~b;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval &= ~b;
							//2
							//ovaldown |= b;
							//ovaldown &= ~sb;
							//mval |= b;
							//3
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval |= b;
							//.
							//osetdown |= b;
							//osetdown |= sb;
							//mset |= b;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
			}
		}

		combs = combs2;
	}

	//3 down
	{
		uint8_t bin = 0;
		uint8_t binmin = 31;

		std::vector<Combo> combs2;

		for(auto cmit=combs.begin(); cmit!=combs.end(); cmit++)
		{
			osetdown = cmit->osetdown;
			ovaldown = cmit->ovaldown;
			mval[level] = cmit->mval;
			mset = cmit->mset;

			//should be set from higher level
			//ovalup
			if(ovalup&(1<<bin))
			{
				//ovalup[bin] = 1

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m2[bin] )=0
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m2[bin] )=1

				//check oset(level-1)[#-1]
				//todo use bit logic to figure what m is and if o(level-1)[] is set etc

				//ovalupshiftdownset
				if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
				{
					//i[#-1]=1 and ~( i[#] & m0[#] )=0
					//so i[#]=1 and m0[#]=1

					//todo m(level)[#] can never be set by anything other than (level)[#] so dont check
					//0 later last bit
					//mset & ovalupshiftdownset
					if(mset&(1<<bin))
					{
						//ovalupshiftdownset & mval
						if(mval[level]&(1<<bin))
						{
							//already set
							//m0[#]=1
						}
						//ovalupshiftdownset & ~mval
						else
						{
							//m0[#]=0
							return false;
						}
					}
					//mval |= ovalupshiftdownset
					//mset |= ovalupshiftdownset
					else
					{
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//ovaldownset
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[#]=1
					}
					//check oset(level-1)[#+1]
					//ovalnotdownset
					//(ovalnotdownset & ovalupshiftdownset)
					//ovalupshiftdownset & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0
						return false;
					}
					//!oset(level-1)[#]
					else
					{
						//i[#]=1

						//osetdown |= ovalupshiftdownset & (~osetdown);
						//ovaldown |= ovalupshiftdownset & (~osetdown);

						osetdown |= (1<<bin);
						ovaldown |= (1<<bin);
					}
				}
				//else i[#-1]=0
				//ovalupshiftnotdownset
				else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
				{
					//i[#-1]=0 and ~( i[#] & m0[#] )=1

					//so i[#]=0 and m0[#]=0
					//or i[#]=1 and m0[#]=0
					//or i[#]=0 and m0[#]=1

					//check oset(level-1)[#]
					//ovalupshiftnotdownset & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//i[#]=1

						//todo check already mset
						//so m0[#]=0

						//if(ovalupshiftnotdownset & osetdown & ovaldown & mset & mval) return false;

						//mset |= ovalupshiftnotdownset & osetdown & ovaldown;
						//mval &= ~(ovalupshiftnotdownset & osetdown & ovaldown);

						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);
					}
					//ovalupshiftnotdownset & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0


						//if(ovalupshiftnotdownset & osetdown & (~ovaldown) & mset & ~mval) return false;

						//mset |= ovalupshiftnotdownset & osetdown & (~ovaldown);
						//mval |= ovalupshiftnotdownset & osetdown & (~ovaldown);

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip
					}
					//!oset(level-1)[#]
					//ovalupshiftnotdownset & (~osetdown)
					else
					{
						//todo

						//choice
						//so i[#]=1 and m0[#]=0
						//or i[#]=0 and m0[#]=0
						//or i[#]=0 and m0[#]=1

		#if 0
						//0
						ovaldown |= ovalupshiftnotdownset & (~osetdown);
						mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
						//1
						//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
						//mval[level] &= ~(ovalupshiftnotdownset & (~osetdown));
						//2
						//ovaldown &= ~(ovalupshiftnotdownset & (~osetdown));
						//mval[level] |= ovalupshiftnotdownset & (~osetdown);
						//.
						mset |= ovalupshiftnotdownset & (~osetdown);
						osetdown |= ovalupshiftnotdownset & (~osetdown);

						if(!*skip)
						{

						}
						else
						{
							--(*skip);

							if(!*skip)
							{
							}
							else
							{
								--(*skip);

								if(!*skip)
								{
								}
								else
									--(*skip);
							}
						}
		#endif

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);
					}

					//they can't both be 1 and set
					//todo if( (mset & oset & i & mval & (1<<bin)) ) return false; 
				}
				//so i[#-1]=1 and ~( i[#] & m0[#] )=0
				//or i[#-1]=0 and ~( i[#] & m0[#] )=1
				//ovalupshiftnotdownset
				//ovalup & (~osetshiftdown)
				else if( !(osetdown&(1<<binmin)) )
				{
					//todo
					//choice

					//check oset(level-1)[#]
					//ovalup & (~osetshiftdown) & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//i[#]=1

		#if 0
						uint32_t b = ovalup & (~osetshiftdown) & osetdown & ovaldown;
						mval &= ~b;
						mset |= b;
						uint32_t sb = (b << 1) | (b >> 31);
						osetdown |= sb;
						ovaldown &= ~sb;
						osetshiftdown = (osetdown << 1) | (osetdown >> 31);
		#endif

						//todo check already mset
						//so m0[#]=0
						mset |= (1<<bin);
						mval[level] &= ~(1<<bin);

						//so i[#-1]=0
						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
					//ovalup & (~osetshiftdown) & osetdown & (~ovaldown)
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//i[#]=0

		#if 0
						uint32_t b = ovalup & (~osetshiftdown) & osetdown & (~ovaldown);
						uint32_t sb = (b << 1) | (b >> 31);
						//0
						mval &= ~b;
						//1`
						mval |= b;
						//.
						mset |= b;
						osetdown |= sb;
						ovaldown &= ~sb;
						osetshiftdown = (osetdown << 1) | (osetdown >> 31);
		#endif

						//todo check already mset
						//so m0[#]=0
						//or m0[#]=1
						mset |= (1<<bin);
						mval[level] |= (1<<bin);
						//todo or other choice skip

						//so i[#-1]=0
						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
					//!oset(level-1)[#]
					//ovalup & (~osetshiftdown) & (~osetdown)
					else
					{
		#if 0
						b = ovalup & (~osetshiftdown) & (~osetdown);
						sb = (b << 1) | (b >> 31);
						//0
						ovaldown |= b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//1
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] &= ~b;
						//2
						ovaldown &= ~b;
						ovaldown |= sb;
						mval[level] |= b;
						//
						ovaldown |= b;
						ovaldown &= ~sb;
						mval[level] |= b;
						//.
						osetdown |= b;
						osetdown |= sb;
						mset |= b;
		#endif


						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=0
						//or i[#-1]=1 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=1

						osetdown |= (1<<bin);
						ovaldown &= ~(1<<bin);

						mset |= (1<<bin);
						mval[level] |= (1<<bin);

						osetdown |= (1<<binmin);
						ovaldown &= ~(1<<binmin);
					}
				}
			}
			else
			{
				//ovalup[bin] = 0
				//~ovalup

				//so o(level-1)[(bin-1)%nb]=1 and ~( o(level-1)[bin] & m(level)[bin] )=1
				//or o(level-1)[(bin-1)%nb]=0 and ~( o(level-1)[bin] & m(level)[bin] )=0

				//~ovalup & osetshiftdown & ovalshiftdown
				if( (osetdown&(1<<binmin)) && (ovaldown&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=1
					// so ~( o(level-1)[bin] & m(level)[bin] )=1

					//so o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[bin]=0 and m(level)[bin]=1

					//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						//so m(level)[bin]=0

						//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & mset & mval
						//retu fal
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							return false;
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset
						else
						{
							//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset;
							//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ovaldownset & ~mset);
							mset |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so m(level)[bin]=0
						//or m(level)[bin]=1

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset
						else
						{
							//0
							//mval |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;
							//1
							//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset);
							//.
							//mset |= ~ovalup & osetshiftdown & ovalshiftdown & osetdown & ~ovaldown & ~mset;

							//choice
							mset |= (1<<bin);
							mval[level] &= ~(1<<bin);
						}
					}
					// !(osetdown&(1<<bin))
					//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown
					else
					{
						//choice
						//so o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[bin]=0 and m(level)[bin]=1

						// o(level-1)[bin]=0 and m(level)[bin]=1
						//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & mval;
						//osetshiftdown = (osetdown << 1) | (osetdown >> 31);
						//ovalshiftdown = (ovaldown << 1) | (ovaldown >> 31);
						//ovalshiftdownset = osetshiftdown & ovalshiftdown;
						//up all

						// o(level-1)[bin]=0 and m(level)[bin]=0
						// o(level-1)[bin]=1 and m(level)[bin]=0
						//~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval
						//0
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval);
						//1
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;
						//.
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & mset & ~mval;

						// o(level-1)[bin]=0 and m(level)[bin]=0
						// o(level-1)[bin]=1 and m(level)[bin]=0
						// o(level-1)[bin]=0 and m(level)[bin]=1
						//0
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//1
						//ovaldown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//mval &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//2
						//ovaldown &= ~(~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset);
						//mval |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//.
						//osetdown |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
						//mset |= ~ovalup & osetshiftdown & ovalshiftdown & ~osetdown & ~mset;
					}
				}
				//2
				//~ovalup & osetshiftdown & ~ovalshiftdown
				else if( (osetdown&(1<<binmin)) && !(ovaldown&(1<<binmin)) )
				{
					// o(level-1)[(bin-1)%nb]=0
					// so ~( o(level-1)[bin] & m(level)[bin] )=0

					//so o(level-1)[bin]=1 and m(level)[bin]=1

					//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						//
						//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & mset & ~mval -> ret f
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							return false;
						}
						// !(mset&(1<<bin))
						//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset
						else
						{
							//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
							//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ovaldown & ~mset;
							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					//~ovalup & osetshiftdown & ~ovalshiftdown & osetdown & ~ovaldown ret f
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0
						return false;
					}
					// !(osetdown&(1<<bin))
					//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown
					else
					{
						//choice
						osetdown |= (1<<bin);
						ovaldown |= (1<<bin);

						//ovaldown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;
						//osetdown |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown;

						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
						}
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & mset & ~mval -> ret f

							//m(level)[bin]=0
							return false;
						}
						// !(mset&(1<<bin))
						else
						{
							//mval |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;
							//mset |= ~ovalup & osetshiftdown & ~ovalshiftdown & ~osetdown & ~mset;

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
				//3
				// !(osetdown&(1<<binmin))
				//~ovalup & ~osetshiftdown
				else
				{
					//choice
					//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
					//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
					//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

					//~ovalup & ~osetshiftdown & osetdown & ovaldown
					if( (osetdown&(1<<bin)) && (ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=1
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=0

							//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval);
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & mval;

							osetdown |= (1<<binmin);
							ovaldown &= ~(1<<binmin);
						}
						//~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & mset & ~mval;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						// !(mset&(1<<bin))
						//~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=0 and m(level)[bin]=1

							//0
							//ovaldown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//mval &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
							//1
							//ovaldown &= ~(~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset);
							//mval |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//.
							//osetdown |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;
							//mset |= ~ovalup & ~osetshiftdown & osetdown & ovaldown & ~mset;

							osetdown |= (1<<binmin);
							ovaldown &= ~(1<<binmin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}

						//2^32 values
						//(2^32)^32 assignments
						//
					}
					//~ovalup & ~osetshiftdown & osetdown & ~ovaldown
					else if( (osetdown&(1<<bin)) && !(ovaldown&(1<<bin)) )
					{
						//o(level-1)[bin]=0

						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1

						//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & mval;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						//~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & mset & ~mval;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);
						}
						// !(mset&(1<<bin))
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and m(level)[bin]=1

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown;
							//bs = (b << 1) | (b >> 31);
							//ovaldown |= bs;
							//osetdown |= bs;

							//b = ~ovalup & ~osetshiftdown & osetdown & ~ovaldown & ~mset;
							//bs = (b << 1) | (b >> 31);
							//0
							//ovaldown |= bs;
							//1
							//ovaldown &= ~bs;
							//.
							//osetdown |= bs;
							//mval |= bs;
							//mset |= bs;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
					// !(osetdown&(1<<bin))
					//4
					//~ovalup & ~osetshiftdown & ~osetdown
					else
					{
						//choice
						//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
						//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
						//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

						//osetdown |= (1<<bin);
						//ovaldown |= (1<<bin);

						//~ovalup & ~osetshiftdown & ~osetdown & mset & mval
						if( (mset&(1<<bin)) && (mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=1
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
							//choice

							//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & mval;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown &= ~sb;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//.
							//osetdown |= b;
							//osetdown |= sb;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);
						}
						//~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval
						else if( (mset&(1<<bin)) && !(mval[level]&(1<<bin)) )
						{
							//m(level)[bin]=0
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//choice

							//b = ~ovalup & ~osetshiftdown & ~osetdown & mset & ~mval;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown |= sb;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//.
							//osetdown |= b;
							//osetdown |= sb;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);
						}
						// !(mset&(1<<bin))
						//~ovalup & ~osetshiftdown & ~osetdown & ~mset
						else
						{
							//choice
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

							//b = ~ovalup & ~osetshiftdown & ~osetdown & ~mset;
							//sb = (b << 1) || (b >> 31);
							//0
							//ovaldown |= b;
							//ovaldown |= sb;
							//mval &= ~b;
							//1
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval &= ~b;
							//2
							//ovaldown |= b;
							//ovaldown &= ~sb;
							//mval |= b;
							//3
							//ovaldown &= ~b;
							//ovaldown |= sb;
							//mval |= b;
							//.
							//osetdown |= b;
							//osetdown |= sb;
							//mset |= b;

							osetdown |= (1<<binmin);
							ovaldown |= (1<<binmin);

							osetdown |= (1<<bin);
							ovaldown &= ~(1<<bin);

							mset |= (1<<bin);
							mval[level] |= (1<<bin);
						}
					}
				}
			}
		}

		combs = combs2;
	}

	if(level)
		HashFun4(mval, ovaldown, in, level-1);
}

struct Mapping
{
	uint32_t in;
	uint32_t out;
};

//propagate up to expected result
void Verify(uint32_t mval[32], uint32_t *mid, uint32_t* know, int8_t level, uint8_t bin, uint32_t oval, uint8_t knowns, uint8_t scale)
{
	bin = (bin+1)&31;
	//uint8_t binmin = (bin+1)&31;
	knowns--;
	uint32_t knowmask = (1<<knowns)-1;
	knowmask = (knowmask << bin) | (knowmask >> (32-bin));

	uint32_t shifto = (oval << 1) | (oval >> 31);
	uint32_t ovalup = shifto ^ ~( oval & mval[level] );
	
	//if(level == 31)
	if(level == scale)
	{
		*know = knowmask;

		//if( (mid & knowmask) != (oval & knowmask) )
		//	return false;
		//return true;

		*mid = ovalup;

		return;
	}
	
	Verify(mval, mid, know, level+1, bin, ovalup, knowns, scale);
}

//elimate 1 bit with each level+
//1 less know
//add 2^(32-level) possibs
//elim 6 bits jump by 2^6

//propagate down to gotten input
bool Verify2(uint32_t mval[32], uint32_t mid, uint32_t know, int8_t level, uint32_t oval, uint8_t scale)
{
	//l31-> 2 b0 b-4 b
	//l30-> 4 b1 b-3 b
	//l29-> 8 b2 b-2 b
	//l28-> 16 b3 b-1 b
	//l27-> 32 b4 b0 b 
	//l26-> 64 b5 b1 b28
	//if(level == 26)
	if(level == scale)
	{
		if(mid != oval & know)
			return false;
		return true;
		//todo
		//return Verify();
	}

						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0
					
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

	uint32_t oshift = (oval << 31) | (oval >> 1);
	uint32_t ovaldown = 0;

	for(uint8_t bin=0; bin<31; ++bin)
	{
		//uint8_t binmin = (bin+31)&31;

		//up1 o0 m0 = 0
		//up1 o0 m1 = 0
		//up1 o1 m0 = 0
		//up1 o1 m1 = 1
		//up0 o0 m0 = 1
		//up0 o0 m1 = 1
		//up0 o1 m0 = 1
		//up0 o1 m1 = 0
		
		uint32_t bm = 1<<bin;
		uint32_t b = ovaldown & bm;
		uint32_t m = mval[level] & bm;
		uint32_t o = oval & bm;
		uint32_t sb = (o & (b & m)) | ~(o | (b & m));
		sb = (sb << 1) || (sb >> 31);
		ovaldown |= sb;
	}

	{
		uint32_t bm = 1<<31;
		uint32_t b = ovaldown & bm;
		uint32_t m = mval[level] & bm;
		uint32_t o = oval & bm;
		uint32_t sb = (o & (b & m)) | ~(o | (b & m));
		sb = (sb << 1) || (sb >> 31);
		//uint8_t binmin = 30;
		bm = 1<<0;
		if(sb != ovaldown & bm)
			goto next;
		if(!Verify2(mval, mid, know, level-1, ovaldown, scale))
			goto next;
		return true;
	}

next:
	ovaldown = 1;
	
	for(uint8_t bin=0; bin<31; ++bin)
	{
		//uint8_t binmin = (bin+31)&31;

		//up1 o0 m0 = 0
		//up1 o0 m1 = 0
		//up1 o1 m0 = 0
		//up1 o1 m1 = 1
		//up0 o0 m0 = 1
		//up0 o0 m1 = 1
		//up0 o1 m0 = 1
		//up0 o1 m1 = 0
		
		uint32_t bm = 1<<bin;
		uint32_t b = ovaldown & bm;
		uint32_t m = mval[level] & bm;
		uint32_t o = oval & bm;
		uint32_t sb = (o & (b & m)) | ~(o | (b & m));
		sb = (sb << 1) || (sb >> 31);
		ovaldown |= sb;
	}

	{
		uint32_t bm = 1<<31;
		uint32_t b = ovaldown & bm;
		uint32_t m = mval[level] & bm;
		uint32_t o = oval & bm;
		uint32_t sb = (o & (b & m)) | ~(o | (b & m));
		sb = (sb << 1) || (sb >> 31);
		//uint8_t binmin = 30;
		bm = 1<<0;
		if(sb != ovaldown & bm)
			return false;
		if(!Verify2(mval, mid, know, level-1, ovaldown, scale))
			return false;
		return true;
	}
}

bool GetMask(uint32_t in, uint32_t oval, uint32_t low, uint32_t high, uint32_t* mask)
{
	//up1 n0 i0 = 0 or 1
	//up1 n0 i1 = 0
	//up1 n1 i0 = ---
	//up1 n1 i1 = 1
	//up0 n0 i0 = ---
	//up0 n0 i1 = 1
	//up0 n1 i0 = 1 or 0
	//up0 n1 i1 = 0

	uint32_t sin = (in << 1) | (in >> 31);

	if(oval & sin & ~in)
		return false;
	if(~oval & ~sin & ~in)
		return false;

	uint32_t stone = ((oval & sin) | ~(oval | sin)) & in;
	uint32_t choice = ((oval & ~sin) | (~oval & sin)) & ~in;

	//uint32_t max = stone | choice;

	//if(max < low)
	//	return false;

	//set the biggest matched choice bit
	for(uint8_t bin=31; bin>=0; --bin)
	{
		uint32_t bm = 1<<bin;

		if(~(low & bm))
		{
			if(stone & bm)
				goto r;

			continue;
		}

		if(~(choice & bm))
		{
			for(++bin; bin&31; ++bin)
			{
				bm = 1<<bin;
				if(~choice & bm)
				{
					stone &= ~bm;
					continue;
				}
				stone |= bm;
				choice &= ~bm;
				goto r;
			}

			return false;
		}

		if(choice & bm)
		{
			stone |= bm;
			choice &= ~bm;
			//break;
		}
	}

r:

	if(stone > high)
		return false;

	*mask = stone;

	return true;
}

#if 0
//https://graphics.stanford.edu/~seander/bithacks.html

Find the log base 2 of an N-bit integer in O(lg(N)) operations

unsigned int v;  // 32-bit value to find the log2 of 
const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
const unsigned int S[] = {1, 2, 4, 8, 16};
int i;

register unsigned int r = 0; // result of log2(v) will go here
for (i = 4; i >= 0; i--) // unroll for speed...
{
  if (v & b[i])
  {
    v >>= S[i];
    r |= S[i];
  } 
}


// OR (IF YOUR CPU BRANCHES SLOWLY):

unsigned int v;	         // 32-bit value to find the log2 of 
register unsigned int r; // result of log2(v) will go here
register unsigned int shift;

r =     (v > 0xFFFF) << 4; v >>= r;
shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                        r |= (v >> 1);


// OR (IF YOU KNOW v IS A POWER OF 2):

unsigned int v;  // 32-bit value to find the log2 of 
static const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 
                                 0xFF00FF00, 0xFFFF0000};
register unsigned int r = (v & b[0]) != 0;
for (i = 4; i > 0; i--) // unroll for speed...
{
  r |= ((v & b[i]) != 0) << i;
}
#endif

uint8_t log2(uint32_t v)
{
	// OR (IF YOU KNOW v IS A POWER OF 2):

	//unsigned int v;  // 32-bit value to find the log2 of 
	static const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 
									 0xFF00FF00, 0xFFFF0000};
	register unsigned int r = (v & b[0]) != 0;
	for (uint8_t i = 4; i > 0; i--) // unroll for speed...
	{
	  r |= ((v & b[i]) != 0) << i;
	}

	return r;
}

bool Pyramid()
{
	return false;
}

//get level 1
bool GetMid1(uint32_t in, uint32_t oval, uint32_t low, uint32_t high, bool nolim, uint32_t mask[32], uint8_t level)
{
	uint32_t down = 0;
	uint32_t trymask = low;
	
	uint32_t sin = (in << 1) | (in >> 31);

	do
	{
		//mask from oval and down
		//what gives lowest mask (in oval)
		//preference priority
		//n i don't matter, only mask matters at level 1
		//going from least significant to most, with given up and m
	//up1 m0 = n0 i0 or //up1 m1 = n0 i0
	//up0 m0 = n1 i0 or //up0 m1 = n1 i0
	//up1 m0 = n0 i1 or //up0 m1 = n0 i1
	//up1 m1 = n1 i1 or //up0 m0 = n1 i1
		//prefer up1 overall to give all 0
		//given mask passed down and oval constructed based on higher oval and mask,
		//constructing down preference priority to pick mask bits.

	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0

		
	//up1 n0 i0 = 0 or 1
	//up1 n0 i1 = 0
	//up1 n1 i0 = ---
	//up1 n1 i1 = 1
	//up0 n0 i0 = ---
	//up0 n0 i1 = 1
	//up0 n1 i0 = 1 or 0
	//up0 n1 i1 = 0


		uint32_t trysubmask;

		down = 0;

		for(uint8_t bin=31; bin>=0; --bin)
		{
			//get lower level lowest mask given i n and down
			//actually try to get a down for a target mask

			//if(

#if 0
			if(down & sin & ~in)
				break;
			if(~down & ~sin & ~in)
				break;

			uint32_t stone = ((down & sin) | ~(down | sin)) & in;
			uint32_t choice = ((down & ~sin) | (~down & sin)) & ~in;
#endif
		}

		bool works = false;

		if(works)
		{
			mask[level-1] = trysubmask;
			mask[level] = trymask;
			return true;
		}

#if 0
		if(GetMask(in, down, nolim ? 0 : mask[level-1], &mask[level-1]))
		{
			mask[level] = trymask;
			return true;
		}
#endif
	}while(trymask);

	return false;
}


bool GetMid2(uint32_t in, uint32_t* ovalup, uint32_t low, uint32_t high, bool nolim, bool* nolimdown, uint32_t mask[32], uint8_t level)
{

}

bool GetMid(uint32_t in, uint32_t oval, uint32_t low, uint32_t high, bool nolim, bool* nolimdown, uint32_t mask[32], uint8_t level)
{
	//for when mask fails
	//nolim = true

	//up1 n0 i0 = 0 or 1
	//up1 n0 i1 = 0
	//up1 n1 i0 = ---
	//up1 n1 i1 = 1
	//up0 n0 i0 = ---
	//up0 n0 i1 = 1
	//up0 n1 i0 = 1 or 0
	//up0 n1 i1 = 0
	
						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0
					
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

	//given mask, upper
	
	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0

	if(!level)
		return GetMask(in, oval, low, high, &mask[level]);
	//if(level == 1)
	//	return GetMid1(in, oval, low, high, nolim, mask, level);

	uint32_t mtry = low;
	mask[level] = high;
	bool works = false;

	do
	{
		//must give o
		
		//unknowns allowed = 31-level
		//knowns required = level+1

		//guaranteed match in decent time

		//2^32 split going down
		//any mask can given o

		uint32_t higher = 0;

		//given mask get desired down
		//desired down is obtained from masks desired below and 

		//mask from oval and down
		//what gives lowest mask (in oval)
		//preference priority
		//n i don't matter, only mask matters at level 1
	//up1 m0 = n0 i0 or //up1 m1 = n0 i0
	//up0 m0 = n1 i0 or //up0 m1 = n1 i0
	//up1 m0 = n0 i1 or //up0 m1 = n0 i1
	//up1 m1 = n1 i1 or //up0 m0 = n1 i1
		//prefer up1 overall to give all 0
		//given mask passed down and oval constructed based on higher oval and mask,
		//constructing down preference priority to pick mask bits.


	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0

#if 1
#define CASE_NOTSHIFT	0
#define CASE_SHIFT		1

		uint32_t cases[4];

		cases[0] = oval & ~mtry;
		cases[1] = oval & mtry;
		cases[2] = ~oval & ~mtry;
		cases[3] = ~oval & mtry;

		std::vector<uint8_t> casesb;
		casesb.resize(32);

#if 1
		for(uint8_t bin=0; bin&31; ++bin)
		{
			uint8_t b = 0;
			const const uint32_t bm = 1<<bin;
			
			b |= (!!(cases[0] & bm))<<0;	//0
			b |= (!!(cases[1] & bm))<<1;	//1
			b |= (!!(cases[2] & bm))<<2;	//2
			b |= (!!(cases[3] & bm))<<3;	//3

			casesb[bin] = b;
		}
#else
		for(uint8_t cin=0; cin&3; ++cin)
		{
			uint32_t nch = ~cases[cin];
			uint32_t chb;

			while(chb = LOWEST0(nch))
			{
				uint32_t lb2 = log2(chb);
				casesb[lb2] = cin;
				nch |= chb;
			}
		}
#endif
#endif
	
#if 0
		uint32_t casea = oval & ~mtry;
		uint32_t caseb = oval & mtry;
		uint32_t casec = ~oval & ~mtry;
		uint32_t cased = ~oval & mtry;

		uint32_t scasea = (casea << 1) || (casea >> 31);
		uint32_t scaseb = (caseb << 1) || (caseb >> 31);
		uint32_t scasec = (casec << 1) || (casec >> 31);
		uint32_t scased = (cased << 1) || (cased >> 31);
#endif

		do
		{
#if 1
			uint32_t down = 0;

			{
				const uint8_t bin=1;
				const uint8_t binmin = (bin+31)&31;
				const uint8_t cb = casesb[bin];
				const uint32_t bm = 1<<bin;
				const uint32_t bmm = 1<<binmin;
				
	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0
				
				const uint32_t hbm = higher & bm;
				const uint32_t nhbm = ~higher & bm;
				
				down |= ( (hbm && ((cb == 1<<0) || (cb == 1<<1) || (cb == 1<<2))) << bin );
				down |= ( (nhbm && ( (cb == 1<<3) )) << bin );
				
				//const uint32_t hbmm = higher & bmm;
				//const uint32_t nhbmm = ~higher & bmm;
				
				down |= ( (hbm && ((cb == 1<<1) || (cb == 1<<2) || (cb == 1<<3))) << binmin );
				down |= ( (nhbm && ( (cb == 1<<2) )) << binmin );

				//down |= ( (hbm && ((cb == 1<<1) || (cb == 1<<2) || (cb == 1<<3))) << binmin );
				//down |= ( (nhbm && ( (cb == 1<<2) )) << binmin );

#if 0
				if(higher & bm)
				{
					if(cb == 0)
					{
						down |= bm;
					}
					else if(cb == 1)
					{
						down |= bm | bmm;
					}
					else if(cb == 2)
					{
						down |= bm | bmm;
					}
					else
					{
						down |= bmm;
					}
				}
				else
				{
					if(cb == 0)
					{
					}
					else if(cb == 1)
					{
					}
					else if(cb == 2)
					{
						down |= bmm;
					}
					else
					{
						down |= bm;
					}
				}
#endif
			}
		
			for(uint8_t bin=2; bin&31; ++bin)
			{
				const uint8_t binmin = (bin+31)&31;
				const uint8_t cb = casesb[bin];
				const uint32_t bm = 1<<bin;
				const uint32_t bmm = 1<<binmin;
				
	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0
				
				uint32_t e = 0;
				const uint32_t hbm = higher & bm;
				
				e |= (hbm && (~down & bmm)) && (cb == 1<<0);
				e |= (hbm && (~down & bmm)) && (cb == 1<<1);
				e |= (hbm && (~down & bmm)) && (cb == 1<<2);
				e |= (hbm && (down & bmm)) && (cb == 1<<3);

				const uint32_t nhbm = ~higher & bm;
				
				e |= (nhbm && (down & bmm)) && (cb == 1<<0);
				e |= (nhbm && (down & bmm)) && (cb == 1<<1);
				e |= (nhbm && (down & bmm)) && (cb == 1<<2);
				e |= (nhbm && (~down & bmm)) && (cb == 1<<3);

				if(e)
					goto next;
				
				//const uint32_t hbm = higher & bm;
				//const uint32_t nhbm = ~higher & bm;

				down |= ( (hbm && ((cb == 1<<0) || (cb == 1<<1) || (cb == 1<<2))) << bin );
				down |= ( (nhbm && ( (cb == 1<<3) )) << bin );

				//down |= ( (hbm && ((cb == 1<<1) || (cb == 1<<2) || (cb == 1<<3))) << binmin );
				//down |= ( (nhbm && ( (cb == 1<<2) )) << binmin );

#if 0
				if(higher & bm)
				{
					if(cb == 1<<0)
					{
						if(down & bmm) goto next;
						down |= bm;
					}
					else if(cb == 1<<1)
					{
						if(~down & bmm)	goto next;
						down |= bm /* | bmm */;
					}
					else if(cb == 1<<2)
					{
						if(~down & bmm)	goto next;
						down |= bm /* | bmm */;
					}
					else if(cb == 1<<3)
					{
						if(~down & bmm)	goto next;
						//down |= bmm;
					}
				}
				else
				{
					if(cb == 0)
					{
						if(down & bmm)	goto next;
					}
					else if(cb == 1)
					{
						if(down & bmm)	goto next;
					}
					else if(cb == 2)
					{
						if(~down & bmm)	goto next;
						//down |= bmm;
					}
					else
					{
						if(down & bmm)	goto next;
						//down |= bm;
					}
				}
#endif
				//down |= cases[0] & 
			}

			{
				const uint8_t bin=0;
				const uint8_t binmin = (bin+31)&31;
				const uint8_t cb = casesb[bin];
				const uint32_t bm = 1<<bin;
				const uint32_t bmm = 1<<binmin;
				
	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0
				
				uint32_t e = 0;
				const uint32_t hbm = higher & bm;
				
				e |= (hbm && (~down & bmm)) && (cb == 1<<0);
				e |= (hbm && (~down & bmm)) && (cb == 1<<1);
				e |= (hbm && (~down & bmm)) && (cb == 1<<2);
				e |= (hbm && (down & bmm)) && (cb == 1<<3);

				const uint32_t nhbm = ~higher & bm;
				
				e |= (nhbm && (down & bmm)) && (cb == 1<<0);
				e |= (nhbm && (down & bmm)) && (cb == 1<<1);
				e |= (nhbm && (down & bmm)) && (cb == 1<<2);
				e |= (nhbm && (~down & bmm)) && (cb == 1<<3);
				
				e |= (hbm & ~down) && (cb == 1<<0);
				e |= (hbm & ~down) && (cb == 1<<1);
				e |= (hbm & ~down) && (cb == 1<<2);
				e |= (hbm & down) && (cb == 1<<3);
				
				e |= (nhbm & down) && (cb == 1<<0);
				e |= (nhbm & down) && (cb == 1<<1);
				e |= (nhbm & down) && (cb == 1<<2);
				e |= (nhbm & ~down) && (cb == 1<<3);

				if(e)
					goto next;

				//down |= ( (hbm && ((cb == 1<<0) || (cb == 1<<1) || (cb == 1<<2))) << bin );
				//down |= ( (nhbm && ( (cb == 1<<3) )) << bin );
				
				//down |= ( (hbm && ((cb == 1<<1) || (cb == 1<<2) || (cb == 1<<3))) << binmin );
				//down |= ( (nhbm && ( (cb == 1<<2) )) << binmin );
				
#if 0
				if(higher & bm)
				{
					if(cb == 1<<0)
					{
						if(down & bmm) goto next;
						down |= bm;
					}
					else if(cb == 1<<1)
					{
						if(~down & bmm)	goto next;
						down |= bm /* | bmm */;
					}
					else if(cb == 1<<2)
					{
						if(~down & bmm)	goto next;
						down |= bm /* | bmm */;
					}
					else if(cb == 1<<3)
					{
						if(~down & bmm)	goto next;
						//down |= bmm;
					}
				}
				else
				{
					if(cb == 0)
					{
						if(down & bmm)	goto next;
					}
					else if(cb == 1)
					{
						if(down & bmm)	goto next;
					}
					else if(cb == 2)
					{
						if(~down & bmm)	goto next;
						//down |= bmm;
					}
					else
					{
						if(down & bmm)	goto next;
						//down |= bm;
					}
				}
#endif
			}
#endif

#if 0
			uint32_t down = 0;
			uint32_t downset = 0;

			{
				uint32_t caseset = cases[0];
				caseset = caseset | (caseset << 1) | (caseset >> 31);
				uint32_t caseval = 
			}
#endif

			//uint32_t highset[2];

			//highset[CASE_NOTSHIFT] = higher;
		//	highset[CASE_SHIFT] = (higher << 31) | (higher >> 1);

			//uint32_t sh = (higher << 31) || (higher >> 1);

#if 0
#define CASE_LOW		0
#define CASE_HIGH		1

			uint32_t casesvals[4][2][2];
			
	//up1 m0 = n0 i0 or n0 i1
	//up1 m1 = n0 i0 or n1 i1
	//up0 m0 = n1 i0 or n1 i1
	//up0 m1 = n0 i1 or n1 i0

			//up = 

			for(uint8_t cin=0; cin<4; ++cin)
			{
				//for(uint8_t sh=0; sh<2; sh<<=1)
				{
					//for(uint8_t hi=0; hi<2; hi<<=1)
					{
						casesvals[cin][CASE_NOTSHIFT][CASE_LOW] = cases[cin] & higher;
						casesvals[cin][CASE_NOTSHIFT][CASE_HIGH] = cases[cin] & ~higher;


						
						casesvals[cin][CASE_SHIFT][CASE_LOW] = (casesvals[cin][CASE_NOTSHIFT][CASE_LOW] << 1) | (casesvals[cin][CASE_NOTSHIFT][CASE_LOW] >> 31);
						casesvals[cin][CASE_SHIFT][CASE_HIGH] = (casesvals[cin][CASE_NOTSHIFT][CASE_HIGH] << 1) | (casesvals[cin][CASE_NOTSHIFT][CASE_HIGH] >> 31);
					}
				}
			}
#endif

#if 0
			uint32_t caseal = 0;	//~higher & casea & ;
			uint32_t scaseal = 0;
			uint32_t caseah = higher & casea;
			uint32_t scaseah = 0;
			uint32_t casebl = 0;
			uint32_t scasebl = 0;
			uint32_t casebh = higher & caseb;
			uint32_t scasebh = sh & scaseb;
			uint32_t casecl = 0;
			uint32_t scasecl = ~sh & scasec;
			uint32_t casech = higher & casec;
			uint32_t scasech = sh & scasec;
			uint32_t casedl = ~higher & cased;
			uint32_t scasedl = 0;
			uint32_t casedh = 0;
			uint32_t scasedh = sh & cased;
#endif
			
#if 0
			uint32_t down = 0;
			uint32_t downset = 0;

			downset |= ((scasea) & sh) |
				((casea | caseb | casec) & ~higher) |
				((scasea | scaseb | scased) & ~sh) |
				((cased) & higher);


			if(down & downset & casea & higher != caseah)
				goto next;

			down |= caseah;
			downset |= casea & higher;


			if(down & downset & caseb & higher != casebh)
				goto next;

			down |= casebh;
			downset |= caseb & higher;
			

			if(down & downset & scaseb & sh != scasebh)
				goto next;

			down |= scasebh;
			downset |= scaseb & sh;
			

			if(down & downset & scasec & ~sh != scasecl)
				goto next;

			down |= scasecl;
			downset |= scasec & ~sh;
#endif

			//if there's no limit, low=0 and high=-1 at start then high=mask found and we try to find a lower mask
			//if there's a limit at first, low=mask (advanced by lowest 1) and high=-1 until one works
			if(GetMid(in, down, (nolim | works) ? 0 : mask[level-1], works ? mask[level-1] : -1, nolim, nolimdown, mask, level-1))
			{
				if(mtry <= mask[level] /* || !works */)
				{
#if 0
					if(mtry < mask[level] && 
						*nolimdown == level)
					{
						*nolimdown--;
					}
#endif

					mask[level] = mtry;
					works = true;
					nolim = true;

					//if(!nolim && /* !*nolimdown && */ works && mtry <= low)
					//	break;
				}
			}

			next:
				++higher;
		}while(higher);

		if(works)
			break;

		++mtry;
		
		//if(!*nolimdown)
		//if(!nolim)
		{
			nolim = true;
			*nolimdown = level;
			//continue;
		}
	}while(mtry && mtry <= high);

	return works;
}

//propagate up to expected output
//assumes lower levels are more significant masks and so counting changes higher levels more frequently
//but little changes from higher levels won't be propagated to all bits
//better to treat higher levels as more significant
//so change lower levels more frequently and try to keep higher levels as small as possible
//by "more significant" I meant the higher levels change less frequently, but the lower levels really have more impact on others
bool Try3(uint32_t mval[32], bool nolim, uint32_t out, int8_t level, uint32_t oval, uint32_t low /*, uint32_t high */, uint32_t* unmatch)
{
	if(level == 32)
	{
		*unmatch = oval ^ out;
		return oval == out;
	}

						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0
					
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1
	
	//bool nolim = false;
	bool nolimup = false;

	uint32_t mtry = low;
	const uint32_t shifto = (oval << 1) | (oval >> 31);
	
again:

	do
	{
		const uint32_t ovalup = shifto ^ ~(oval & mtry);
		
		//const uint32_t uplow = nolimup ? 0 : mval[(level+1)&31];
		const uint32_t uplow = mval[(level+1)&31];
		//const uint32_t uphigh = -1;
		
#if 0
		uint32_t ovalup2;

		//in the backward case it will not be more than 2^32 to get a desired i
		//in the forward case it might take more

		if(GetMask(oval, ovalup2, uplow, -1, &mtry))
		{

		}
#endif

		if(Try3(mval, nolimup, out, level+1, ovalup, uplow /*, uphigh */, unmatch))
		{
			mval[level] = mtry;
			return true;
		}

		//++mtry;

		if(!nolim)
			break;

		//if(mtry)
		{
			//analyse
			uint32_t nch = ~(*unmatch);
			uint32_t chb = LOWEST0(nch);

			mtry += chb;

			//if(!chb)
		}

	}while(mtry /* && mtry != low */ /* && mtry <= high */);

	if(nolim /* && !nolimup */)
	{
		uint8_t l = level+1;

		while(l < 32 && !(++mval[l])) ++l;

		if( l == 32 || (l == 31 && !mval[l]) )
			return false;

		//nolimup = true;
		mtry = 0;
		goto again;
	}

	*unmatch = ((*unmatch) << 31) | ((*unmatch) >> 1);

	return false;
}

//try low and high
//propagate down to gotten input
bool Try2(uint32_t mval[32], bool nolim, uint32_t in, int8_t level, uint32_t oval, uint8_t scale, uint32_t low, uint32_t high)
{
	//l31-> 2 b0 b-4 b
	//l30-> 4 b1 b-3 b
	//l29-> 8 b2 b-2 b
	//l28-> 16 b3 b-1 b
	//l27-> 32 b4 b0 b 
	//l26-> 64 b5 b1 b28
	//if(level == 26)
#if 1
	if(level==(int8_t)-1)
	{
		//up1 n0 i0 = 0 or 1
		//up1 n0 i1 = 0
		//up1 n1 i0 = ---
		//up1 n1 i1 = 1
		//up0 n0 i0 = ---
		//up0 n0 i1 = 1
		//up0 n1 i0 = 1 or 0
		//up0 n1 i1 = 0

		if(oval != in)
			return false;

		return true;
		//todo
		//return Verify();
	}
#endif

#if 0
	if(!level)
	{
		if(GetMask(in, oval, 0, -1, &mval[level]))
			return true;
	}
#endif

	if(level == scale)
	{
		//GetMid...
	}

						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0
					
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

	uint32_t mval2[32], mval3[32];

	memcpy(mval2, mval, sizeof(uint32_t) * 32);
	memcpy(mval3, mval, sizeof(uint32_t) * 32);

	uint32_t mtry = low;
	
	uint32_t oshift = (oval << 31) | (oval >> 1);

	do
	{
		bool got2 = false;
		bool got3 = false;

		uint32_t ovaldown = 0;

		for(uint8_t bin=0; bin<31; ++bin)
		{
			//uint8_t binmin = (bin+31)&31;

			//up1 o0 m0 = 0
			//up1 o0 m1 = 0
			//up1 o1 m0 = 0
			//up1 o1 m1 = 1
			//up0 o0 m0 = 1
			//up0 o0 m1 = 1
			//up0 o1 m0 = 1
			//up0 o1 m1 = 0
		
			uint32_t bm = 1<<bin;
			uint32_t b = ovaldown & bm;
			uint32_t m = mtry & bm;
			uint32_t o = oval & bm;
			uint32_t sb = (o & (b & m)) | ~(o | (b & m));
			sb = (sb << 1) || (sb >> 31);
			ovaldown |= sb;
		}

		{
			uint32_t bm = 1<<31;
			uint32_t b = ovaldown & bm;
			uint32_t m = mtry & bm;
			uint32_t o = oval & bm;
			uint32_t sb = (o & (b & m)) | ~(o | (b & m));
			sb = (sb << 1) || (sb >> 31);
			//uint8_t binmin = 30;
			bm = 1<<0;
			if(sb != ovaldown & bm)
				goto next;
			if(!Verify2(mval, mid, know, level-1, ovaldown, scale))
				goto next;
			got2 = true;
			//memcpy(mval, mval2, sizeof(uint32_t) * 32);
			//return true;
		}

	next:
		ovaldown = 1;
	
		for(uint8_t bin=0; bin<31; ++bin)
		{
			//uint8_t binmin = (bin+31)&31;

			//up1 o0 m0 = 0
			//up1 o0 m1 = 0
			//up1 o1 m0 = 0
			//up1 o1 m1 = 1
			//up0 o0 m0 = 1
			//up0 o0 m1 = 1
			//up0 o1 m0 = 1
			//up0 o1 m1 = 0
		
			uint32_t bm = 1<<bin;
			uint32_t b = ovaldown & bm;
			uint32_t m = mtry & bm;
			uint32_t o = oval & bm;
			uint32_t sb = (o & (b & m)) | ~(o | (b & m));
			sb = (sb << 1) || (sb >> 31);
			ovaldown |= sb;
		}

		{
			uint32_t bm = 1<<31;
			uint32_t b = ovaldown & bm;
			uint32_t m = mtry & bm;
			uint32_t o = oval & bm;
			uint32_t sb = (o & (b & m)) | ~(o | (b & m));
			sb = (sb << 1) || (sb >> 31);
			//uint8_t binmin = 30;
			bm = 1<<0;
			if(sb != ovaldown & bm)
				goto end;
			if(!Verify2(mval, mid, know, level-1, ovaldown, scale))
				goto end;
			got3 = true;
		}

	end:
		if(got2 && got3)
		{
			if(Ahead(mval2, mval3))
				memcpy(mval, mval2, sizeof(uint32_t) * 32);
			else
				memcpy(mval, mval3, sizeof(uint32_t) * 32);
		}
		else if(got2)
			memcpy(mval, mval2, sizeof(uint32_t) * 32);
		else if(got3)
			memcpy(mval, mval3, sizeof(uint32_t) * 32);
		else
		{
			++mtry;
			continue;
		}
		
		return true;

	}while(mtry && mtry <= high);

	return false;
}

bool HashFun5(uint32_t mval[32], uint32_t ovalup, uint32_t in, bool nolim, int8_t level=31)
{
	//o2[0] = 0
	//0 = o1[2] ^ ~( o1[0] & m2[0] )

	std::vector<Combo> combs;

	uint32_t mset = 0;
	uint32_t ovaldown = 0;
	uint32_t osetdown = 0;

	//	for mval[level+1] no change -> nolim=false
	//low = mval[level]
	//high = -1
	//	for mval[level+1]+ -> nolim=true
	//low = 0
	//high = -1

	uint32_t mlow;
	uint32_t mhigh = -1;

#if 1
	if(nolim)
		mlow = 0;
	else
		mlow = mval[level];
#endif
	
	uint32_t mcur = mlow;

	if(!level)
	{
		osetdown = -1;
		ovaldown = in;

		//todo
		return;
	}

	Combo cm0;
	cm0.mset = mset;
	cm0.mval = mval[level];
	cm0.osetdown = osetdown;
	cm0.ovaldown = ovaldown;
	combs.push_back(cm0);

	const uint32_t first = (-1) & ~(1<<31);

	const uint32_t oup0 = first & ~ovalup;
	const uint32_t oup1 = first & ovalup;

						//choice
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0

						
							//choice down
							//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
							//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
							//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1



	//

	if(level)
		HashFun5(mval, ovaldown, in, nolim, level-1);
}

void HashFun(uint32_t* ins, uint32_t nin, uint32_t mval[32], uint32_t* mup, uint32_t mset[32])
{	
	std::list<uint32_t[32]> listmval;
	std::list<uint32_t[32]> listoval;
	std::list<uint32_t[32]> listmset;
	std::list<uint32_t[32]> listoset;

#if 1
	for(uint32_t inin=0; inin<nin; ++inin)
	{
		uint32_t mdown[32];
		uint32_t in = *(ins + inin);
		uint32_t out = inin;
		uint32_t oval[33];
		uint32_t oset[33];

		oset[32] = -1;
		oval[32] = out;
		//oset[0] = -1;
		//oval[0] = i;
		//uint32_t topout = out;

		//o2[0] = 0
		//0 = o1[2] ^ ~( o1[0] & m2[0] )

		uint32_t tempmval[32], tempmset[32];
		memcpy(tempmval, mval, sizeof(uint32_t) * 32);
		memcpy(tempmset, mset, sizeof(uint32_t) * 32);

		while(!HashFun(tempmval, mup, tempmset, oval, oset, in) && listmval.size())
		{

		}
	}
#endif
}

#if 0

uint3 mset[3] <- list kept for future updates?
	uint3 mval[3] <- list kept for future updates
	uint3 oset[3] <- passed down recursion, local copy
	uint3 oval[3] <- passed down recursion, local copy

	uint32 mset[32] <- list kept for future updates?
	uint32 mval[32] <- list kept for future updates
	uint32 oset[32]	<- passed down recursion, local copy
	uint32 oval[32]	<- passed down recursion, local copy
#endif


#if 0

	0110 -> 0000
	1010 -> 0001
	1111 -> 0010
	0001 -> 0011

	//0-3 = right-left
	o[0] = (!i[0] && i[1] && !i[2] && i[3]) || (i[0] && !i[1] && !i[2] && !i[3]) = 
	o[1] = (i[0] && i[1] && i[2] && i[3])
	o[2] = 0
	o[3] = 0

	what makes an out bit 1? can ignore what 0000?

	a xor b = (a && !b) || (!a && b) = !(a && b) && (a || b)

	how to turn only &&'s into a xor?

	and || and !

	when updating hash fun, ignore/leave unchanged all values before addition

	truth table boolean
	i3	i2	i1	i0		o3	o2	o1	o0
	0	0	0	0		1	1	1	1
	...
	0	0	0	1		0	0	1	1
	0	1	1	0		0	0	0	0
	1	0	1	0		0	0	0	1
	1	1	1	1		0	0	1	0

	o[0] = !(i[0] && m0[0]) && !(i[1] && m1[1]) && !(i[2] && m2[2]) && !(i[3] && m3[3])

	o[n] = (i[(n-1)%4] && !(i ) || (!i[(n-1)%4] && (i)

	mn[n] = effect of nth bit of input on [n]th bit of output

	0 changes result if input bit is 1 and previous result 0
	0 changes result if input bit is 0 and previous result 0
	1 changes result if input bit is 0 and previous result 0

	1 changes result if input bit is 0 and previous result 1
	1 changes result if input bit is 1 and previous result 1
	0 changes result if input bit is 1 and previous result 1

	1 does not change result if input bit is 1 and previous result 1
	1 does not change result if input bit is 0 and previous result 0

	sometimes want always 0
	sometimes want always 1
	sometimes want only when xth bit and yth bit is 0 etc.

	goal from any two bits, any result

	truth table t ^ ~(i & m)
	t	i	m			t2
	0	0	0			1
	0	0	1			1
	0	1	0			1
	0	1	1			0
	1	0	0			0
	1	0	1			0
	1	1	0			0
	1	1	1			1

	given two bits, can decide what third mask bit will be given desired result

	o[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) & m2[0] )

	~(a&b) = ~a | ~b
	o[0] = ( ( i[0] ^ ( ~i[1] | ~m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) & m2[0] )

	a^b = (!a & b) | (a & !b)
	a ^ ~(b&c) = (~a & (~b | ~c)) | (a & ~(~b | ~c)) = 

	a^b = !(a & b) & (a | b)
	a ^ ~(b&c) = !(a & (~b | ~c)) & (a | (~b | ~c))

	o[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) & m2[0] )

	o[0] = (!i[0] && i[1] && !i[2] && i[3]) || (i[0] && !i[1] && !i[2] && !i[3])

	train 
	work backwards from end required result
	first level of brackets, iteration 32 of getting hash, working backwards
	t ^ ~(i & m) = o1[2] ^ ~( o1[0] & m2[0] )
	o2[0] should equal 1 (hash item #0001 from external ID)
	if m2[0] is set, then set either o1[0] and/or o1[2], and so on
		if they have not been set, set to always give 1 o2[0], etc.
			mark which bits set {bool oset[32][32] mset[32][32]} during each train
			this works because there will always be a var you can set(?)
			keep mset[32][32] between trains for updating hash fun

			o 32 * 32 * m 32 * 32 = 1,048,576

			3-bit
			o[0] = ((-1 * m0[0]) * i[0] + (-1 * m1[0]) * i[1] + (-1 * m2[0]) * i[2]) || () || () || m4[0]
		o[0] = 
			m[n] says if i[n] is involved in && term, or if its positive in it

			o[0] = (!i[0] && i[1] && !i[2] && i[3]) || (i[0] && !i[1] && !i[2] && !i[3])
			how to combine the terms?

			maybe
			t = ~(t & n) ^ ~(i & m)

			how to lets say have 0th bit affect 3rd bit?

			3-bit
			o[0] = i[2] ^ ~( i[0] & m0[0] )
			o[1] = i[0] ^ ~( i[1] & m0[1] )
			o[2] = i[1] ^ ~( i[2] & m0[2] )
			i[0] = o[0]
		i[1] = o[1]
		i[2] = o[2]
		o[0] = i[2] ^ ~( i[0] & m1[0] )
			o[1] = i[0] ^ ~( i[1] & m1[1] )
			o[2] = i[1] ^ ~( i[2] & m1[2] )
			i[0] = o[0]
		i[1] = o[1]
		i[2] = o[2]
		o[0] = i[2] ^ ~( i[0] & m2[0] )
			o[1] = i[0] ^ ~( i[1] & m2[1] )
			o[2] = i[1] ^ ~( i[2] & m2[2] )
			i[0] = o[0]
		i[1] = o[1]
		i[2] = o[2]

		or
			o0[0] = i[2] ^ ~( i[0] & m0[0] )
			o0[1] = i[0] ^ ~( i[1] & m0[1] )
			o0[2] = i[1] ^ ~( i[2] & m0[2] )
			o1[0] = o0[2] ^ ~( o0[0] & m1[0] )
			o1[1] = o0[0] ^ ~( o0[1] & m1[1] )
			o1[2] = o0[1] ^ ~( o0[2] & m1[2] )
			o2[0] = o1[2] ^ ~( o1[0] & m2[0] )
			o2[1] = o1[0] ^ ~( o1[1] & m2[1] )
			o2[2] = o1[1] ^ ~( o1[2] & m2[2] )

			therefore
			o2[0] = o1[2] ^ ~( o1[0] & m2[0] )
			o2[1] = o1[0] ^ ~( o1[1] & m2[1] )
			o2[2] = o1[1] ^ ~( o1[2] & m2[2] )

			therefore
			o2[0] = ( o0[1] ^ ~( o0[2] & m1[2] ) ) ^ ~( o1[0] & m2[0] )
			o2[1] = ( o0[2] ^ ~( o0[0] & m1[0] ) ) ^ ~( o1[1] & m2[1] )
			o2[2] = ( o0[0] ^ ~( o0[1] & m1[1] ) ) ^ ~( o1[2] & m2[2] )

			therefore
			o2[0] = ( o0[1] ^ ~( o0[2] & m1[2] ) ) ^ ~( ( o0[2] ^ ~( o0[0] & m1[0] ) ) & m2[0] )
			o2[1] = ( o0[2] ^ ~( o0[2] & m1[2] ) ) ^ ~( ( o0[0] ^ ~( o0[1] & m1[1] ) ) & m2[1] )
			o2[2] = ( o0[0] ^ ~( o0[1] & m1[1] ) ) ^ ~( ( o0[1] ^ ~( o0[2] & m1[2] ) ) & m2[2] )

			therefore
			o2[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( o0[2] & m1[2] ) ) ^ ~( ( o0[2] ^ ~( o0[0] & m1[0] ) ) & m2[0] )
			o2[1] = ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( o0[0] & m1[0] ) ) ^ ~( ( o0[0] ^ ~( o0[1] & m1[1] ) ) & m2[1] )
			o2[2] = ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( o0[1] & m1[1] ) ) ^ ~( ( o0[1] ^ ~( o0[2] & m1[2] ) ) & m2[2] )

			therefore
			o2[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( o0[2] ^ ~( o0[0] & m1[0] ) ) & m2[0] )
			o2[1] = ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) ^ ~( ( o0[0] ^ ~( o0[1] & m1[1] ) ) & m2[1] )
			o2[2] = ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) ^ ~( ( o0[1] ^ ~( o0[2] & m1[2] ) ) & m2[2] )

			therefore
			o2[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( o0[0] & m1[0] ) ) & m2[0] )
			o2[1] = ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) ^ ~( ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( o0[1] & m1[1] ) ) & m2[1] )
			o2[2] = ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) ^ ~( ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( o0[2] & m1[2] ) ) & m2[2] )

			therefore
			o2[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) & m2[0] )
			o2[1] = ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[2] ) ) ^ ~( ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) & m2[1] )
			o2[2] = ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) ^ ~( ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) & m2[2] )

			and
			o[0] = ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) ^ ~( ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) & m2[0] )
			o[1] = ( ( i[1] ^ ~( i[2] & m0[2] ) ) ^ ~( ( i[2] ^ ~( i[0] & m0[0] ) ) & m1[0] ) ) ^ ~( ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) & m2[1] )
			o[2] = ( ( i[2] ^ ~( i[0] & m0[0] ) ) ^ ~( ( i[0] ^ ~( i[1] & m0[1] ) ) & m1[1] ) ) ^ ~( ( ( i[0] ^ ~( i[1] & m0[1] ) ) ^ ~( ( i[1] ^ ~( i[2] & m0[2] ) ) & m1[2] ) ) & m2[2] )

			in general, for jth iteration result

			dont do this for all each time add new hash item
			calc hash then check if item external id matches and if in range
			if not, item has not been added

				isolate variable m

http://www.davdata.nl/math/boole.html

		For AND operations, the associative rule applies: A.(B.C) = (A.B).C
			Also the communicative rule applies: A.B = B.A

			For OR operations, the associative rule applies: A+(B+C) = (A+B)+C
			Also the communicative rule applies: A+B = B + A

			A XOR B XOR B = A



			t = (i << 1) | (i >> 31)
			//t = t ^ (i & m2)
			t = t ^ ~(i & m)
			i = t
			//repeat 32

			//must rotate for all input bits to effect all out bits
			//t = i ^ m
			t = (i << 1) | (i >> 31)
			//t = t ^ (i & m2)
			t = t & !(i & m)
			i = t
			t = (i << 1) | (i >> 31)
			//etc.

			Because the NAND function has functional completeness all logic 
			systems can be converted into NAND gates. This is also true of NOR gates. 
			In principle, any combinatorial logic function can be realized with enough NAND gates.

https://en.wikipedia.org/wiki/NAND_logic

		const uint32_t bit00mask = 1 << 0;
		const uint32_t bit01mask = 1 << 1;
		const uint32_t bit02mask = 1 << 2;
		const uint32_t bit03mask = 1 << 3;
		const uint32_t bit04mask = 1 << 4;
		const uint32_t bit05mask = 1 << 5;
		const uint32_t bit06mask = 1 << 6;
		const uint32_t bit07mask = 1 << 7;
		const uint32_t bit08mask = 1 << 8;
		const uint32_t bit09mask = 1 << 9;
		const uint32_t bit10mask = 1 << 10;
		const uint32_t bit11mask = 1 << 11;
		const uint32_t bit12mask = 1 << 12;
		const uint32_t bit13mask = 1 << 13;
		const uint32_t bit14mask = 1 << 14;
		const uint32_t bit15mask = 1 << 15;
		const uint32_t bit16mask = 1 << 16;
		const uint32_t bit17mask = 1 << 17;
		const uint32_t bit18mask = 1 << 18;
		const uint32_t bit19mask = 1 << 19;
		const uint32_t bit20mask = 1 << 20;
		const uint32_t bit21mask = 1 << 21;
		const uint32_t bit22mask = 1 << 22;
		const uint32_t bit23mask = 1 << 23;
		const uint32_t bit24mask = 1 << 24;
		const uint32_t bit25mask = 1 << 25;
		const uint32_t bit26mask = 1 << 26;
		const uint32_t bit27mask = 1 << 27;
		const uint32_t bit28mask = 1 << 28;
		const uint32_t bit29mask = 1 << 29;
		const uint32_t bit30mask = 1 << 30;
		const uint32_t bit31mask = 1 << 31;

		uint32_t oldbit00 = ( counter & bit00mask ) >> 0;
		uint32_t oldbit01 = ( counter & bit01mask ) >> 0;
		uint32_t oldbit02 = ( counter & bit02mask ) >> 0;
		uint32_t oldbit03 = ( counter & bit03mask ) >> 0;
		uint32_t oldbit04 = ( counter & bit04mask ) >> 0;
		uint32_t oldbit05 = ( counter & bit05mask ) >> 0;
		uint32_t oldbit06 = ( counter & bit06mask ) >> 0;
		uint32_t oldbit07 = ( counter & bit07mask ) >> 0;
		uint32_t oldbit08 = ( counter & bit08mask ) >> 0;
		uint32_t oldbit09 = ( counter & bit09mask ) >> 0;
		uint32_t oldbit10 = ( counter & bit10mask ) >> 0;
		uint32_t oldbit11 = ( counter & bit11mask ) >> 0;
		uint32_t oldbit12 = ( counter & bit12mask ) >> 0;
		uint32_t oldbit13 = ( counter & bit13mask ) >> 0;
		uint32_t oldbit14 = ( counter & bit14mask ) >> 0;
		uint32_t oldbit15 = ( counter & bit15mask ) >> 0;
		uint32_t oldbit16 = ( counter & bit16mask ) >> 0;
		uint32_t oldbit17 = ( counter & bit17mask ) >> 0;
		uint32_t oldbit18 = ( counter & bit18mask ) >> 0;
		uint32_t oldbit19 = ( counter & bit19mask ) >> 0;
		uint32_t oldbit20 = ( counter & bit20mask ) >> 0;
		uint32_t oldbit21 = ( counter & bit21mask ) >> 0;
		uint32_t oldbit22 = ( counter & bit22mask ) >> 0;
		uint32_t oldbit23 = ( counter & bit23mask ) >> 0;
		uint32_t oldbit24 = ( counter & bit24mask ) >> 0;
		uint32_t oldbit25 = ( counter & bit25mask ) >> 0;
		uint32_t oldbit26 = ( counter & bit26mask ) >> 0;
		uint32_t oldbit27 = ( counter & bit27mask ) >> 0;
		uint32_t oldbit28 = ( counter & bit28mask ) >> 0;
		uint32_t oldbit29 = ( counter & bit29mask ) >> 0;
		uint32_t oldbit30 = ( counter & bit30mask ) >> 0;
		uint32_t oldbit31 = ( counter & bit31mask ) >> 0;

		counter++;

		uint32_t newbit00 = oldbit00 << 0;
		uint32_t newbit01 = oldbit00 * 2;
		uint32_t newbit02 = oldbit00 << 2;
		uint32_t newbit03 = oldbit00 << 3;
		uint32_t newbit04 = oldbit00 << 4;
		uint32_t newbit05 = oldbit00 << 5;
		uint32_t newbit06 = oldbit00 << 6;
		uint32_t newbit07 = oldbit00 << 7;
		uint32_t newbit08 = oldbit00 << 8;
		uint32_t newbit09 = oldbit00 << 9;
		uint32_t newbit10 = oldbit00 << 10;
		uint32_t newbit11 = oldbit00 << 11;
		uint32_t newbit12 = oldbit00 << 12;
		uint32_t newbit13 = oldbit00 << 13;
		uint32_t newbit14 = oldbit00 << 14;
		uint32_t newbit15 = oldbit00 << 15;
		uint32_t newbit16 = oldbit00 << 16;
		uint32_t newbit17 = oldbit00 << 17;
		uint32_t newbit18 = oldbit00 << 18;
		uint32_t newbit19 = oldbit00 << 19;
		uint32_t newbit20 = oldbit00 << 20;
		uint32_t newbit21 = oldbit00 << 21;
		uint32_t newbit22 = oldbit00 << 22;
		uint32_t newbit23 = oldbit00 << 23;
		uint32_t newbit24 = oldbit00 << 24;
		uint32_t newbit25 = oldbit00 << 25;
		uint32_t newbit26 = oldbit00 << 26;
		uint32_t newbit27 = oldbit00 << 27;
		uint32_t newbit28 = oldbit00 << 28;
		uint32_t newbit29 = oldbit00 << 29;
		uint32_t newbit30 = oldbit00 << 30;
		uint32_t newbit31 = oldbit00 << 31;

		uint32_t r = 0;

		r = r | newbit00;

#endif


#endif
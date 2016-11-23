

#include "../utils.h"
#include "../debug.h"
#include "localmap.h"


void TestHash()
{
	uint32_t m[32];
	memset(m, 0, 32 * sizeof(uint32_t));
	
	uint32_t unmatch;
	uint32_t expect;

	uint64_t t = GetTicks();

	struct Map
	{
		uint32_t i;
		uint32_t o;
	};

	std::list<Map> map;

	for(uint32_t i=0; i<1; ++i)
	{
		//uint32_t o = 1024-i;
		//uint32_t o = i;
		uint32_t o = i+1;
		Map add;
		add.i = i;
		add.o = o;
		map.push_back(add);
	}

	auto mit = map.begin();

	while(mit != map.end())
	{
		uint32_t old[32];
		memcpy(old, m, sizeof(uint32_t) * 32);

		if(Try3(m, true, mit->o, 0, mit->i, m[0], &unmatch, &expect))
			;//InfoMess("s","s");
		else
			InfoMess("f","f");
		
		break;

		if(Ahead(old, m) || Ahead(m, old))
			mit = map.begin();
		else
			mit++;
	}

	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	InfoMess(ms,ms);
}

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

//propagate up to expected output
//assumes lower levels are more significant masks and so counting changes higher levels more frequently
//but little changes from higher levels won't be propagated to all bits
//better to treat higher levels as more significant
//so change lower levels more frequently and try to keep higher levels as small as possible
//by "more significant" I meant the higher levels change less frequently, but the lower levels really have more impact on others
bool Try3(uint32_t mval[32], bool nolim, uint32_t out, int8_t level, uint32_t oval, uint32_t low /*, uint32_t high */, uint32_t* unmatch, uint32_t* expect)
{
	if(level == 32)
	{
		//bits ahead won't affect it unless loops > 32 levels
		*unmatch = oval ^ out;
		//*expect = out;
		*expect = 0;
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
		
		Log("try3 l%d mtry%u", (int)level, mtry);

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

		if(Try3(mval, nolimup, out, level+1, ovalup, uplow /*, uphigh */, unmatch, expect))
		{
			mval[level] = mtry;
			return true;
		}

		//++mtry;

		if(!nolim)
		{
			//if changing oval bits wouldn't affect ovalup result bits, then this level's mask must be changed

#if 0
			if(*expect < level)
			{
				uint32_t nch = ~(*unmatch);
				uint32_t chb = LOWEST0(nch);

				uint32_t oval2 = oval ^ ((oval & ~chb) | (~oval & chb));
				uint32_t shifto2 = (oval2 << 1) | (oval2 >> 31);
				uint32_t ovalup2 = shifto2 ^ ~(oval2 & mtry);

#if 0
				uint32_t oval3 = oval;
				uint32_t shifto3 = shifto + chb;
				uint32_t ovalup3 = shifto3 ^ ~(oval3 & mtry);
			
				uint32_t oval4 = oval + chb;
				uint32_t shifto4 = shifto + chb;
				uint32_t ovalup4 = shifto4 ^ ~(oval4 & mtry);
#endif

				if( ( ovalup2 & chb == ovalup & chb ) /* &&
					( ovalup3 & chb == ovalup & chb ) &&
					( ovalup4 & chb == ovalup & chb ) */ )
				{
					*expect = level;
					return false;
				}
			}
			else
				return false;
#endif

#if 0
			uint32_t unexpect = ((*expect) ^ ovalup) & (*unmatch);

			if( unexpect )
			{
				//get oval given mask up

				//uint32_t exp2 = *expect;

				//*expect = ;
				//*unmatch = (*expect) ^ oval;

				//up1 m0 s i
				//up1 m0 s i
				//up1 m1 s i
				//up1 m1 s i
				//up0 m0 s i
				//up0 m0 s i
				//up0 m1 s i
				//up0 m1 s i
				
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

			}
			else
#endif		
			//*expect = 
			*unmatch = ((*unmatch) << 31) | ((*unmatch) >> 1);
			return false;
		}

#if 1
		//if(mtry)
		{
			if(*expect > level)
			{
				uint8_t l = level+1;

				while(l < *expect) { mval[l]=0; ++l; }
				
				while(l < 32 && !(++mval[l])) ++l;

				if( l == 32 || (l == 31 && !mval[l]) )
				{
					*unmatch = 0;
					return false;
				}

				//nolimup = true;
				mtry = 0;
				goto again;
			}
			else
			{
				//analyse
				uint32_t nch = ~(*unmatch);
				uint32_t chb = LOWEST0(nch);
				//uint32_t chb = *unmatch;

				if(mtry + chb <= mtry)
					break;

				mtry += chb;
			}
#if 0
			uint32_t unmatch2;

			if(Try3(mval, nolimup, out, level+1, ovalup, uplow /*, uphigh */, &unmatch2))
			{
				mval[level] = mtry;
				return true;
			}
			
			uint32_t nch2 = ~(unmatch2);
			uint32_t chb2 = LOWEST0(nch2);
			
			uint8_t l = level+1;
			uint32_t chb3 = chb;

			while(chb == chb2)
			{
				chb3 = (chb3 << 1) || (chb3 >> 31);


				//while(l < 32 && !(++mval[l])) ++l;

				if( l == 32 || (l == 31 && !mval[l]) )
				{
					*unmatch = 0;
					return false;
				}

				//nolimup = true;
				mtry = 0;
				++l;
				//goto again;
			}
#endif
			//if(!chb)
		}
#else
		++mtry;
#endif
	}while(mtry /* && mtry != low */ /* && mtry <= high */);

	//if(nolim /* && !nolimup */)
	{
		uint8_t l = level+1;

		while(l < 32 && !(++mval[l])) ++l;

		if( l == 32 || (l == 31 && !mval[l]) )
		{
			*unmatch = 0;
			return false;
		}

		//nolimup = true;
		mtry = 0;
		goto again;
	}

	//*unmatch = ((*unmatch) << 31) | ((*unmatch) >> 1);

	//return false;
}
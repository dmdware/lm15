

#include "../utils.h"
#include "../debug.h"
#include "localmap.h"


void TestHash()
{
#if 0
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

	for(uint32_t i=0; i<2; ++i)
	{
		uint32_t o = 1024-i;
		//uint32_t o = i;
		//uint32_t o = i+1;
		//uint32_t o = i+2;
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
		
		//break;

		if((Ahead(old, m) || Ahead(m, old)) && mit != map.begin())
			mit = map.begin();
		else
			mit++;
	}

	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	InfoMess(ms,ms);
#endif
}

#if 1

uint32_t Hash(uint32_t mval[32], uint32_t oval, uint8_t level)
{
	if(level >= 32)
		return oval;

	uint32_t shifto = (oval << 1) | (oval >> 31);
	const uint32_t ovalup = shifto ^ ~(oval & mval[level]);
	
		Log("level%d up%u", (int)level, ovalup);

	return Hash(mval, ovalup, level+1);
}

bool NextSide()
{
	return false;
}

void Propagate(uint32_t mval[32], uint32_t ovals[34], int8_t level)
{
	for(; level<33; ++level)
	{
		const uint32_t oval = ovals[level];
		const uint32_t shifto = (oval << 1) | (oval >> 31);
		const uint32_t ovalup = shifto ^ ~(oval & mval[level]);
		ovals[level+1] = ovalup;
	}
}

//try all row combos behind changed bit
bool CombosUp(uint32_t mval[32], uint32_t ovals[33], uint32_t effect[33], int8_t mainbin, int8_t workbase, int8_t startl, int8_t startbin, uint32_t ovalin)
{
	ovals[startl] = ovalin;

	if(startl == 32)
	{
		return true;
	}

	for(int8_t l=startl; l<32; ++l)
	{
		//const uint32_t oval = ovals[l];
		//const uint32_t shifto = (oval << 1) | (oval >> 31);
		//const uint32_t ovalup = shifto ^ ~(oval & mval[l]);

		//int8_t w = 32-(l-startl);
		//int8_t maxb = mainbin + 

		int8_t lup = l-startl;
		int8_t tobin = startbin + 32 - lup;
		//int8_t backbin = tobin & 31;

		for(int8_t binoff=0; binoff<=lup; ++binoff)
		{
			int8_t fullbin = ((startbin + 32 - lup)&31) + binoff;
			//reverse pyramid that gets bigger upward
			int8_t bin = fullbin&31;

			int8_t diagedge = mainbin + (31 - l);

			//greater than diagonal edge?
			if(fullbin > diagedge)
				break;

			int8_t backbin = (bin + 31)&31;
			uint32_t bm = 1 << bin;
			uint32_t bbm = 1 << backbin;

			mval[l] &= ~bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

			mval[l] |= bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

			//todo mod lower significance first, if some ahead

			//don't check behind downward pyramid, i.e. pyramid of bits that effect bit (32,mainbin)
			if(bin == mainbin)
				break;
		}

		//ovals[l+1] = ovalup;
	}

	return false;
}

//check how far up effect is and try changing bits forward
bool Propagate2(uint32_t mval[32], uint32_t ovals[33], uint32_t effect[33], int8_t mainbin, int8_t workbase, int8_t startl, int8_t startbin, uint32_t ovalin)
{
	ovals[startl] = ovalin;

	if(startl == 32)
	{
		return true;
	}

	for(int8_t l=startl; l<32; ++l)
	{
		//const uint32_t oval = ovals[l];
		//const uint32_t shifto = (oval << 1) | (oval >> 31);
		//const uint32_t ovalup = shifto ^ ~(oval & mval[l]);

		//int8_t w = 32-(l-startl);
		//int8_t maxb = mainbin + 

		int8_t lup = l-startl;
		int8_t tobin = startbin + 32 - lup;
		//int8_t backbin = tobin & 31;

		for(int8_t binoff=0; binoff<=lup; ++binoff)
		{
			int8_t fullbin = ((startbin + 32 - lup)&31) + binoff;
			//reverse pyramid that gets bigger upward
			int8_t bin = fullbin&31;

			int8_t diagedge = mainbin + (31 - l);

			//greater than diagonal edge?
			if(fullbin > diagedge)
				break;

			int8_t backbin = (bin + 31)&31;
			uint32_t bm = 1 << bin;
			uint32_t bbm = 1 << backbin;

			mval[l] &= ~bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

			mval[l] |= bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

#if 0
			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			//or if on mainline, check vertically
			if((ovals[l+1] & bin) == (ovalup & bin))
			{
				//if we're behind the modified bit, i.e., we can safely decrement any bits here and still be forward going
				if(l != startl || bin != startbin)
				{

				}

				//no change
				//effect[l] |= bin;
			}
#endif

			//todo mod lower significance first, if some ahead

			//don't check behind downward pyramid, i.e. pyramid of bits that effect bit (32,mainbin)
			if(bin == mainbin)
				break;
		}

		//ovals[l+1] = ovalup;
	}

	return false;
}

bool Changed(uint32_t mval[32], uint32_t ovals[33], uint32_t oldo[33], int8_t level, int8_t bin)
{
}

//check how far up effect is and try changing bits forward
bool Propagate3(uint32_t mval[32], uint32_t ovals[33], uint32_t effect[33], int8_t mainbin, int8_t workbase, int8_t startl, int8_t startbin, uint32_t ovalin)
{
	ovals[startl] = ovalin;

	if(startl == 32)
	{
		return true;
	}

	for(int8_t l=startl; l<32; ++l)
	{
		//const uint32_t oval = ovals[l];
		//const uint32_t shifto = (oval << 1) | (oval >> 31);
		//const uint32_t ovalup = shifto ^ ~(oval & mval[l]);

		//int8_t w = 32-(l-startl);
		//int8_t maxb = mainbin + 

		int8_t lup = l-startl;
		int8_t tobin = startbin + 32 - lup;
		//int8_t backbin = tobin & 31;

		for(int8_t binoff=0; binoff<=lup; ++binoff)
		{
			int8_t fullbin = ((startbin + 32 - lup)&31) + binoff;
			//reverse pyramid that gets bigger upward
			int8_t bin = fullbin&31;

			int8_t diagedge = mainbin + (31 - l);

			//greater than diagonal edge?
			if(fullbin > diagedge)
				break;

			int8_t backbin = (bin + 31)&31;
			uint32_t bm = 1 << bin;
			uint32_t bbm = 1 << backbin;

			mval[l] &= ~bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

			mval[l] |= bm;

			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			if(binoff)
			{
				mval[l] &= ~bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}

				mval[l] |= bbm;

				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bbm) != (ovalup & bbm))
				{
					//change detected
					//ovals[l+1] = ovalup;
					if(Propagate2(mval, ovals, effect, mainbin, bin + 31, l+1, backbin, ovalup))
						return true;
				}

				//or if below diagonal edge, check vertically
				if(l<startl)
				{
					if(l<startl && ((ovals[l+1] & bm) != (ovalup & bm))) //extra 4th bit since we're below diagonal edge
					{
						//change detected
						//ovals[l+1] = ovalup;
						if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
							return true;
					}
				}
			}
			//or if on mainline, check vertically
			else
			{
				uint32_t oval = ovals[l];
				uint32_t shifto = (1 << oval) | (oval >> 31);
				uint32_t ovalup = shifto ^ ~(oval & mval[l+1]);

				if((ovals[l+1] & bm) != (ovalup & bm)) //extra 4th bit since we're below diagonal edge
				{
					//change detected

					if(Propagate2(mval, ovals, effect, mainbin, bin, l+1, bin, ovalup))
						return true;
				}
			}

#if 0
			//check 3-bit pyramid / 4-bit block for change, try all combos, trying 0's first
			//or if on mainline, check vertically
			if((ovals[l+1] & bin) == (ovalup & bin))
			{
				//if we're behind the modified bit, i.e., we can safely decrement any bits here and still be forward going
				if(l != startl || bin != startbin)
				{

				}

				//no change
				//effect[l] |= bin;
			}
#endif

			//todo mod lower significance first, if some ahead

			//don't check behind downward pyramid, i.e. pyramid of bits that effect bit (32,mainbin)
			if(bin == mainbin)
				break;
		}

		continue;

tryall:
		//try all row combos going up before mod bit



		//ovals[l+1] = ovalup;
	}

	return false;
}

//increment side(s) trying to match output bit
bool UpSide(uint32_t mval[32], uint32_t out, uint32_t in, uint32_t ovals[33], uint32_t effect[33], uint8_t mainbin, int8_t level, bool* ch)
{
	//uint32_t oldo[33];
	//memcpy(oldo, ovals, sizeof(uint32_t) * 33);

	int8_t maxb = mainbin+32;
	uint32_t mm = 1<<mainbin;

	for(int8_t workbase=mainbin; workbase<maxb; ++workbase)
	{
		int8_t bin = workbase&31;
		uint32_t bm = 1<<bin;
		int8_t startl = 31-(mainbin-workbase);

		//no effect possible
		//if(effect[startl] & bm)
		//	continue;

		for(int8_t l=startl; l>=0; ++l)
		{
			//only add 1's until effective
			if(mval[l] & bm)
				continue;
			
			//no effect possible
			if( workbase == mainbin &&
				(effect[l] & bm) )
				break;

			mval[l] |= bm;

			uint32_t oval = ovals[l+1];
			uint32_t shifto = (oval << 1) | (oval >> 31);
			uint32_t ovalup = shifto ^ ~(oval & mval[l]);

			ovals[l+1] = ovalup;

			int8_t backbin = (bin+31)&31;
			uint32_t bbm = 1<<backbin;

			//significant change?
			if( (((workbase == mainbin) || (l > startl)) && ((ovalup & bm) != (ovals[l+1] & bm))) ||
				((workbase > mainbin) && ((ovalup & bbm) != (ovals[l+1] & bbm))))
			{
				Propagate2(mval, ovals, effect, mainbin, workbase, l, bin, ovalup);
				
				if((out & mm) == (ovals[32] & mm))
					return true;
				else
				{

				}
			}
		}
	}

	return false;
}

/*

tried column

0
0
0
0
0
0
0
0

next col
effect 

00
00
00
00
00
00
00
01

combos

00
01

10
01

00
11

10
11

01
01

11
01

01
11

11
11

etc


effect

10
00

or

01
00

or

11
00


don't have to try
previous cols
only last two
and upward pyramid from mod bit 1
















if tried

...
00

and

...
10

and

...
01

and 

...
11

and effect is not

10
00

nor

11
00

then mark ineffective

10
00


then in the future don't try

...
00

nor

...
10

nor

...
01

nor 

...
11


actually if tried

..
0.

and

..
1.

and result effect is not

1.
..

or maybe

1..
...





*/

bool TryBit(uint32_t mval[32], uint8_t bin, int8_t level, uint8_t bval)
{

}

bool MatchBit(uint32_t mval[32], uint32_t out, uint32_t in, uint8_t bin, bool* ch)
{
#if 0
	if(level == 31)
	{
		//todo
		return (out & (1<<bin)) == (oval & (1<<bin));
	}
#endif

	uint32_t effect[32];

	memset(effect, 0, sizeof(uint32_t) * 32);

	uint32_t ovals[34];

	ovals[0] = in;
	ovals[33] = out;

	Propagate(mval, ovals, 0);

	uint32_t oval = ovals[32];

	if((out & (1<<bin)) != (oval & (1<<bin)))
	{
		if(!UpSide(mval, out, in, ovals, effect, bin, 31, ch))
			return false;
	}

	if(!MatchBit(mval, out, ovalup, bin, level+1, ch))
	{
		if(level == 0)
		//todo
			UpSide(mval, out, oval, effect, bin, 31, ch);
	}

	return false;
}

//propagate up to expected output
//assumes lower levels are more significant masks and so counting changes higher levels more frequently
//but little changes from higher levels won't be propagated to all bits
//better to treat higher levels as more significant
//so change lower levels more frequently and try to keep higher levels as small as possible
//by "more significant" I meant the higher levels change less frequently, but the lower levels really have more impact on others
bool Try4(uint32_t mval[32], bool nolim, uint32_t out, uint32_t oval)
{
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
	
	uint8_t bin = 31;
	uint32_t effect[32];

	memset(effect, 0, sizeof(uint32_t) * 32);

	while(bin >= 0)
	{
		bool ch = false;

		//assume level=0
		if(!MatchBit(mval, out, oval, bin, 0, &ch))
			return false;

		if(ch)
			bin = 31;
		else
			--bin;
	}
}

bool Try5(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;

	while(true)
	{
		bin = 0;

		while(bin < 32)
		{
			bm = 1 << bin;
			l = 31;

			while(l >= 0)
			{
				if(!(mval[l] & bm))
				{
					mval[l] |= bm;
					goto back;
				}

				--l;
			}

			++bin;
		}
		
		return false;

back:
		if(!l)
		{
			if(bin)
			{
				--bin;
				l = 31;
			}
			else
				goto prop;
		}
		else
			--l;

		while(bin >= 0)
		{
			bm = 1 << bin;

			while(l >= 0)
			{
				mval[l] &= ~bm;
				--l;
			}

			--bin;
			l = 31;
		}

prop:

		uint32_t oval = in;

		for(uint8_t i=0; i<32; ++i)
		{
			uint32_t shifto = (oval << 1) | (oval >> 31);
			oval = shifto ^ ~(oval & mval[i]);
		}

		if(oval == out)
			return true;
	}
}

bool Check(uint32_t mval[32], uint32_t in, uint32_t out, uint8_t checkbin)
{
	uint32_t oval = in;

	for(uint8_t i=0; i<32; ++i)
	{
		uint32_t shifto = (oval << 1) | (oval >> 31);
		oval = shifto ^ ~(oval & mval[i]);
	}

	uint32_t checkbm = 1 << checkbin;

	return ((oval & checkbm) == (out & checkbm));
}

//opt 1
bool Try6(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 0;
	uint32_t ovals[33];

	while(true)
	{
		bin = 0;

		while(bin < 32)
		{
			bm = 1 << bin;
			l = 31;

			while(l >= 0)
			{
				if(!(mval[l] & bm))
				{
					mval[l] |= bm;
					
					goto back;
				}

				--l;
			}

			++bin;
		}

		return false;
		
back:
		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		if(!Check(mval, in, out, checkbin))
			continue;

		bool next = false;

		for(uint8_t i=0; i<checkbin; ++i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				next = true;
				break;
			}
		}

		if(next)
			continue;

#if 0
		oval = in;

		for(uint8_t i=0; i<32; ++i)
		{
			uint32_t shifto = (oval << 1) | (oval >> 31);
			oval = shifto ^ ~(oval & mval[i]);
		}

		if(oval == out)
			return true;
#endif

		++checkbin;

		if(checkbin >= 33)
			return true;
	}
}


//opt 2
bool Try7(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 0;
	uint32_t ovals[33];

	while(true)
	{
		uint8_t first = checkbin;
		uint8_t lastfull = checkbin + 32;
		uint8_t second = 32;

		if(lastfull >= 32)
			second = 0;

		uint8_t last = lastfull & 31;

		bin = 0;

		bool ch = false;

		while(!ch)
		{
			while(bin < 32)
			{
				//only modify check bits
				if( !((bin >= checkbin && bin < lastfull) || (bin >= second && bin < last)))
					continue;

				bm = 1 << bin;
				l = 31;

				while(l >= 0)
				{
					if(!(mval[l] & bm))
					{
						mval[l] |= bm;
					
						goto back;
					}

					--l;
				}

				++bin;
			}
		}

		return false;
		
back:
		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		if(!Check(mval, in, out, checkbin))
			continue;

		bool next = false;

		for(uint8_t i=0; i<checkbin; ++i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				next = true;
				break;
			}
		}

		if(next)
			continue;

#if 0
		oval = in;

		for(uint8_t i=0; i<32; ++i)
		{
			uint32_t shifto = (oval << 1) | (oval >> 31);
			oval = shifto ^ ~(oval & mval[i]);
		}

		if(oval == out)
			return true;
#endif

		++checkbin;

		if(checkbin >= 33)
			return true;
	}
}

void ResetBits(uint32_t mval[32], uint8_t tobin, uint8_t tolevel)
{
	for(uint8_t bin=0; bin<=tobin; ++bin)
	{
		uint32_t bm = 1 << bin;

		int8_t bintol = -1;

		if(bin == tobin)
			bintol = tolevel;

		for(int8_t l=31; l>bintol; --l)
		{
			mval[l] &= ~bm;
		}
	}
}

bool UpBits(uint32_t mval[32], uint8_t frombin, uint8_t tobin)
{
	for(uint8_t bin=frombin; bin<=tobin; ++bin)
	{
		uint32_t bm = 1 << bin;

		for(uint8_t l=31; l>=0; --l)
		{
			if(mval[l] & bm)
				continue;

			mval[l] |= bm;
			ResetBits(mval, bin, l);
			return true;
		}
	}

	return false;
}

//opt 3
bool Try8(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 0;
	uint32_t ovals[33];

	while(true)
	{
		uint8_t first = checkbin;
		uint8_t lastfull = checkbin + 32;
		uint8_t second = 32;

		if(lastfull >= 32)
			second = 0;

		uint8_t last = lastfull & 31;

		bin = 0;

		if(UpBits(mval, second, last))
			goto back;

		if(UpBits(mval, last+1, first))
			goto back;

		if(UpBits(mval, first, 31))
			goto back;

		return false;
		
back:
		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		if(!Check(mval, in, out, checkbin))
			continue;

		bool next = false;

		for(uint8_t i=0; i<checkbin; ++i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				next = true;
				break;
			}
		}

		if(next)
			continue;

#if 0
		oval = in;

		for(uint8_t i=0; i<32; ++i)
		{
			uint32_t shifto = (oval << 1) | (oval >> 31);
			oval = shifto ^ ~(oval & mval[i]);
		}

		if(oval == out)
			return true;
#endif

		++checkbin;

		if(checkbin >= 33)
			return true;
	}
}


uint32_t TwoPow(uint32_t pow)
{
	uint32_t r = 1;

	for(uint32_t i=0; i<pow; ++i)
		r <<= 1;

	return r;
}

//opt 4
bool Try9(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 0;
	uint32_t ovals[33];
	uint32_t midcycle = 0;
	uint32_t cyval[32];
	uint32_t midcycles;

	memcpy(cyval, mval, sizeof(uint32_t) * 32);

	while(true)
	{
		uint8_t first = checkbin;
		uint8_t lastfull = checkbin + 32;
		uint8_t second = 32;

		if(lastfull >= 32)
			second = 0;

		uint8_t last = lastfull & 31;
		
		uint8_t midcols = first - (lastfull+1);

		midcycles = TwoPow(midcols);

		bin = 0;

		if(UpBits(mval, second, last))
			goto back;

		if(midcycle < midcycles)
		{
			if(UpBits(mval, first, 31))
			{
				++midcycle;
				goto back;
			}
		}
		else
		{
			if(UpBits(mval, last+1, first))
				goto back;
		}

		return false;
		
back:
		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		if(!Check(mval, in, out, checkbin))
			continue;

		bool next = false;

		for(uint8_t i=0; i<checkbin; ++i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				next = true;
				break;
			}
		}

		if(next)
			continue;

		++checkbin;
		midcycle = 0;

		if(checkbin >= 33)
			return true;
	}
}










//opt 5
bool Try10(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 0;
	uint32_t ovals[33];
	uint32_t firstcycle = 0;
	//uint32_t cyval[32];
	uint32_t firstcycles;
	uint32_t seccycle = 0;
	uint32_t seccycles;

	//memcpy(cyval, mval, sizeof(uint32_t) * 32);

	while(true)
	{
		uint8_t first = checkbin;
		uint8_t lastfull = checkbin + 32;
		uint8_t second = 32;

		uint8_t firstcols = lastfull - second;
		uint8_t seccols = 32 - first;

		if(lastfull >= 32)
		{
			firstcols = 32 - second;
			second = 0;
		}

		uint8_t last = lastfull & 31;

		firstcycles = TwoPow(32 * (uint32_t)firstcols);
		seccycles = TwoPow(32 * (uint32_t)seccols);

		bin = 0;
		
		if(firstcycle < firstcycles)
		{
			if(UpBits(mval, second, last))
			{
				++firstcycle;
				goto back;
			}

			if(UpBits(mval, last+1, first))
			{
				goto back;
			}
		}
		else if(seccycle < seccycles)
		{
			if(UpBits(mval, first, 31))
			{
				firstcycle = 0;
				++seccycle;
				goto back;
			}
		}

		return false;
		
back:
		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		if(!Check(mval, in, out, checkbin))
			continue;

		bool next = false;

		for(uint8_t i=0; i<checkbin; ++i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				next = true;
				break;
			}
		}

		if(next)
			continue;

		++checkbin;
		firstcycle = 0;
		seccycle = 0;

		if(checkbin >= 33)
			return true;
	}
}









//increment bit below diagonal edge
//set behind to off
bool UpDiag()
{
}

//increment side until change happens effect
//stay in column
bool UpSide2()
{
}

//increment change below level l
bool UpSide3()
{
}

//opt 6
bool Try11(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint8_t bin;
	uint32_t bm;
	uint8_t l;
	uint32_t oval;
	uint8_t bin2;
	uint32_t bm2;
	uint8_t l2;
	uint8_t checkbin = 31;
	//uint32_t ovals[33];
	uint32_t firstcycle = 0;
	//uint32_t cyval[32];
	uint32_t firstcycles;
	uint32_t seccycle = 0;
	uint32_t seccycles;
	
	uint32_t both[32];
	//uint32_t bottleneck = 0; (1<<last-1)-(1<<second-1)
	/*
	reset same row previous bits when advanced next bit
	and only reset above bits when add new column below (upside down triangle)
	rather when reached diagonal edge, don't need to reset above, if combos tried
	if all combos 111 for given column including edge, no further action possible
	reset below in truncated pyramid with base down
	*/
	memset(both, 0, sizeof(uint32_t) * 32);

	//memcpy(cyval, mval, sizeof(uint32_t) * 32);

	//todo i8min

	while(true)
	{
next:
		uint8_t first = checkbin;
		uint8_t lastfull = checkbin + 32;
		uint8_t second = 32;

		uint8_t firstcols = lastfull - second;
		uint8_t seccols = 32 - first;

		if(lastfull >= 32)
		{
			firstcols = 32 - second;
			second = 0;
		}

		uint8_t last = lastfull & 31;

		firstcycles = TwoPow(32 * (uint32_t)firstcols);
		seccycles = TwoPow(32 * (uint32_t)seccols);

		bin = 0;
		
		if(firstcycle < firstcycles)
		{
			if(UpBits(mval, second, last))
			{
				++firstcycle;
				goto back;
			}

			if(UpBits(mval, last+1, first))
			{
				goto back;
			}
		}
		else if(seccycle < seccycles)
		{
			if(UpBits(mval, first, 31))
			{
				firstcycle = 0;
				++seccycle;
				goto back;
			}
		}

		return false;
		
back:
		//reset previous bits

		bm2 = bm;
		l2 = l;
		bin2 = bin;

		if(!l2)
		{
			if(bin2)
			{
				--bin2;
				l2 = 31;
			}
			else
				goto prop;
		}
		else
			--l2;

		while(bin2 >= 0)
		{
			bm2 = 1 << bin2;

			while(l2 >= 0)
			{
				mval[l2] &= ~bm2;
				--l2;
			}

			--bin2;
			l2 = 31;
		}

prop:
		//propagate changes

		if(!Check(mval, in, out, checkbin))
			continue;
		
		firstcycle = 0;
		seccycle = 0;

		for(uint8_t i=31; i!=(uint8_t)-1; --i)
		{
			if(!Check(mval, in, out, i))
			{
				checkbin = i;
				goto next;
			}
		}

		--checkbin;

		if(checkbin == (uint8_t)-1)
			return true;
	}
}



#endif
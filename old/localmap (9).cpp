

#include "../utils.h"
#include "../debug.h"
#include "localmap.h"


void TestHash()
{
#if 1
	uint64_t t = GetTicks();

	//for(int times=0; times<10; ++times)
	{
		HASHINT m[HASHLEVELS];
		memset(m, 0, HASHLEVELS * sizeof(HASHINT));

		struct Map
		{
			HASHINT i;
			HASHINT o;
		};

		std::list<Map> map;

		uint32_t r = GetTicks();
		
		//for(HASHINT o=0; o<pow(2,HASHBITS)-1; ++o)
		//for(HASHINT o=0; o<pow(2,HASHBITS); ++o)
		//for(HASHINT o=0; o<pow(2,HASHBITS-2)-1; ++o)
		//for(HASHINT o=0; o<HASHBITS-1; ++o)
		for(HASHINT o=0; o<HASHVALS; ++o)
		//for(HASHINT o=0; o<0; ++o)
		{
			while(true)
			{
				r *= 0x12345;
				r += 98765;

				//HASHINT i = 12;
				//HASHINT i = 23;
				//HASHINT i = 74;
				//HASHINT i = 37235;
				//HASHINT i = 1024-o;
				HASHINT i = (1024-o) ^ r;
				//HASHINT i = o;
			//	HASHINT i = HASHCROP( o+1 );
				//HASHINT i = o * MAXJUMP;

				//i &= ((1<<1) | (1<<3) | (1<<4) | (1<<5));

				//i = (3-o)&(4-1);

				Map add;
				add.i = HASHCROP(i);
				add.o = HASHCROP(o);

				//if(o>1)
				//	add.o = HASHCROP( -1 );

#if 1
				bool have = false;

				for(auto mit=map.begin(); mit!=map.end(); mit++)
				{
					if(mit->i != add.i)
						continue;

					have = true;
					break;
				}

				if(have)
					continue;
#endif
				map.push_back(add);
				break;
			}
		}

#if 0
				Map add;
				add.i = HASHCROP(8);
				add.o = HASHCROP(0);
				map.push_back(add);
				
				add.i = HASHCROP(10);
				add.o = HASHCROP(1);
				map.push_back(add);
#endif
		//Log("start");

		auto mit = map.begin();

		std::list<Mask>* masks = new std::list<Mask> [map.size()];
		std::list<Tried>* tried = new std::list<Tried> [map.size() * HASHLEVELS];
		HASHINT mci = 0;
		
		HASHINT* inval = new HASHINT[map.size()];
		HASHINT* inmask = new HASHINT[map.size()];
		HASHINT* in = new HASHINT[map.size()];
		memset(inval, 0, sizeof(HASHINT)*map.size());
		memset(inmask, 0, sizeof(HASHINT)*map.size());
		memset(in, 0, sizeof(HASHINT)*map.size());

		//if(0)
		while(mit != map.end())
		{
			//HASHINT old[HASHLEVELS];
			//HASHINT m2[HASHLEVELS];
			//memcpy(old, m, sizeof(HASHINT) * HASHLEVELS);
			//memcpy(m2, m, sizeof(HASHINT) * HASHLEVELS);

			//Log("%u->%u", (int)mit->i, (int)mit->o);

			//std::list<Mask>* masks = ;
			
			//Log("answer2:");
			Mask mask;
			mask.ovaldown = mit->o;
			mask.mask = 0;
			masks[mci].push_back(mask);

			Up(mit->i, &inval[mci], &inmask[mci]);

			in[mci] = mit->i;

			++mci;
			++mit;
		}

		if(0)
		{
			//HASHINT old[HASHLEVELS];
			//HASHINT m2[HASHLEVELS];
			//memcpy(old, m, sizeof(HASHINT) * HASHLEVELS);
			//memcpy(m2, m, sizeof(HASHINT) * HASHLEVELS);

			//Log("%u->%u", (int)mit->i, (int)mit->o);

			//std::list<Mask>* masks = ;
			
			//Log("answer2:");
			Mask mask;
			mask.ovaldown = 0;
			mask.mask = 0;
			masks[mci].push_back(mask);

			Up(mit->i, &inval[mci], &inmask[mci]);

			in[mci] = 8;

			++mci;
			++mit;
		}if(0)
		{
			//HASHINT old[HASHLEVELS];
			//HASHINT m2[HASHLEVELS];
			//memcpy(old, m, sizeof(HASHINT) * HASHLEVELS);
			//memcpy(m2, m, sizeof(HASHINT) * HASHLEVELS);

			//Log("%u->%u", (int)mit->i, (int)mit->o);

			//std::list<Mask>* masks = ;
			
			//Log("answer2:");
			Mask mask;
			mask.ovaldown = 1;
			mask.mask = 0;
			masks[mci].push_back(mask);

			Up(mit->i, &inval[mci], &inmask[mci]);

			in[mci] = 10;

			++mci;
			++mit;
		}

	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0000
	//0101
	//1010

	//0000
	//0000
	//1111

	//0000
	//1111
	//1010

	//0001
	//0101
	//1011

	//1000
	//0101
	//0011

	//1010
	//0101
	//0101

		{
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				Log("start %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
		}

		{
			std::list<HASHINT>* up = new std::list<HASHINT> [map.size() * HASHLEVELS];
			
			auto mit2 = map.begin();
			auto i=0;
			for(; mit2!=map.end(); mit2++)
			{
				//UpFill(mit2->i, &up[HASHLEVELS*i]);
				++i;
			}
			delete [] up;
		}
		Log("up)");

		{

			HASHINT d = 0;

			if(Down(masks, m, &d, HASHLEVELS-1, in, true, inval, inmask, map.size(), tried))
			//if(Try12(m, mit->i, mit->o))
				;//InfoMess("s","s");
			else
			{
				//InfoMess("f","f");
				
				mit = map.begin();

				for(; mit!=map.end(); mit++)
				{
					HASHINT o = Hash(m, mit->i, 0);
					Log("fail gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
				}
				
			Log("answer:");

#if 0
				if(Try12(m2, mit->i, mit->o))
				{
					mit = map.begin();

					for(; mit!=map.end(); mit++)
					{
						HASHINT o = Hash(m2, mit->i, 0);
						Log("win gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
					}
				
					for(SHASHADDR x=0; x<HASHLEVELS; ++x)
					{
						Log("\t cmask[%d] = %u", (int)x, (int)m2[x]);
					}
				}
				else
					Log("nor");

					for(SHASHADDR x=0; x<HASHLEVELS; ++x)
					{
						Log("\t wmask[%d] = %u", (int)x, (int)m[x]);
					}
#endif

				uint64_t p = GetTicks() - t;

				char ms[123];
				sprintf(ms, "fail %llu on %d", p, (int)mit->o);
				MessageBox(NULL, ms, ms, NULL);

				delete [] masks;

				return;
			}
			
			delete [] masks;

			mit = map.begin();
			
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			++mit;
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			
#if 1
			
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				Log("gives %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
			//break;
			
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %llu", (int)x, m[x]);
			}
#endif

#if 0
			if((Ahead(old, m) || Ahead(m, old)) && mit != map.begin())
				mit = map.begin();
			else
#endif
			//	mit++;
		}

		mit = map.begin();

		for(; mit!=map.end(); mit++)
		{
			HASHINT o = Hash(m, mit->i, 0);
			Log("gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
		}

		//__int128 i = 123ull;

			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %llu", (int)x, m[x]);
			}
	}

	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	MessageBox(NULL, ms, ms, NULL);
	//InfoMess(ms,ms);
#endif
}

#if 1

bool UniqueMask(Mask a, Mask b)
{
	return (a.ovaldown == b.ovaldown);
}

bool CompareMask(Mask a, Mask b)
{
	return ( (a.mask < b.mask) && (a.maskup == b.maskup) ) || ( a.maskup < b.maskup );
}

void Up(HASHINT in, HASHINT* inval, HASHINT* inmask)
{
	
	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	HASHINT shift = HASHROTL(in,1);

	*inval |= HASHCROP( ~shift & ~in );
	*inmask |= *inval;

	*inmask |= HASHCROP( shift & ~in );
}

bool UniqueUp(HASHINT a, HASHINT b)
{
	return (a==b);
}

void UpFill(HASHINT in, std::list<HASHINT>* invals, HASHINT inval, HASHINT inmask)
{
	if(!inmask)
		Up(in, &inval, &inmask);
	
	HASHINT low0 = HASHCROP( LOWEST0( inmask ) );

	if(!low0)
	{
		for(auto uit=invals->begin(); uit!=invals->end(); ++uit)
			if(*uit == inval)
				return;

		invals->push_back(inval);
		return;
	}

	inmask |= low0;

	inval &= HASHCROP( ~low0 );
	UpFill(in, invals, inval, inmask);
	inval |= low0;
	UpFill(in, invals, inval, inmask);
}

void UpFill(HASHINT in, std::list<HASHINT> upvals[HASHLEVELS])
{
	UpFill(in, &upvals[0], 0, 0);
	//upvals[0].unique(UniqueUp);

	for(HASHINT l=1; l<HASHLEVELS; ++l)
	{
		Log("up%d %d", (int)l, (int)upvals[l-1].size());
		for(auto uit=upvals[l-1].begin(); uit!=upvals[l-1].end(); uit++)
			UpFill(*uit, &upvals[l], 0, 0);

		//upvals[l].unique(UniqueUp);
	}
}

void Narrow(std::list<Mask>* a, std::list<Mask>* b)
{
	auto ait=a->begin();
	auto bit=b->begin();

	//Log("narrow %d %d", (int)a->size(), (int)b->size());
	
	//for(auto ait2=a->begin(); ait2!=a->end(); ait2++)
	//	Log("a %d", (int)ait2->mask);
	//for(auto bit2=b->begin(); bit2!=b->end(); bit2++)
	//	Log("b %d", (int)bit2->mask);

	while(true)
	{
		HASHINT c;

		if(bit == b->end())
		{
			while(ait != a->end())
				ait = a->erase(ait);
			//goto check;
			return;
		}
		
		if(ait == a->end())
		{
			while(bit != b->end())
				bit = b->erase(bit);
			//goto check;
			return;
		}

		if(bit->mask < ait->mask)
		{
			c = ait->mask;

			while(bit != b->end() &&
				bit->mask < c)
				bit = b->erase(bit);

			if(bit != b->end() &&
				bit->mask == c)
				goto up;

			while(ait != a->end() &&
				ait->mask == c)
				ait = a->erase(ait);
		}
		else
		{
			c = bit->mask;

			while(ait != a->end() &&
				ait->mask < c)
				ait = a->erase(ait);
			
			if(ait != a->end() &&
				ait->mask == c)
				goto up;

			while(bit != b->end() &&
				bit->mask == c)
				bit = b->erase(bit);
		}

up:

		//Log("narrow %d", (int)c);
		
		while(bit != b->end() &&
			bit->mask == c)
			bit++;

		while(ait != a->end() &&
			ait->mask == c)
			ait++;
	}

check:

	;
	//if(a->size() != b->size() &&
	//	(a->size() == 0 || b->size() == 0))
	//	Log("disc");
}

void Fill(HASHINT oval, HASHINT ovaldown, HASHINT oset, HASHADDR fullbin, HASHADDR setbin, 
		  HASHINT mlow, HASHINT mval, std::list<Mask>* mask, HASHINT mset, bool check, HASHINT in, HASHINT inmask, HASHINT high, HASHINT maskup,
		  bool lowlim, std::list<Tried>* tried)
{
#if 0
	//if((int)oval == 21847)
	Log("ovaldown%d oval%d mset%d oset%d q%d mlow%d inmask%d h%d !HASHCROP(~oset)%d !HASHCROP(~mset)%d HASHCROP(~oset)%d HASHCROP(~mset)%d ~oset%d ~mset%d MSHHASH%d (int)HASHBITS%d, (int)(1<<HASHBITS)%d, (int)((1<<HASHBITS)-1)%d", 
		(int)ovaldown, (int)oval,
		(int)mset, (int)oset, (int)(!HASHCROP(~oset) && !HASHCROP(~mset)),
		(int)mlow, (int)inmask, (int)high,
		(int)(!HASHCROP(~oset)), (int)(!HASHCROP(~mset)), (int)(!!(int)HASHCROP(~oset)), (int)(!!HASHCROP(~mset)), (int)(!!(int)(~oset)), (int)(~mset), (int)MAXHASH, (int)HASHBITS, (int)(1<<HASHBITS), (int)((1<<HASHBITS)-1));
#endif

	//if( ((int)(!HASHCROP(~oset))) == ((int)(!!HASHCROP(~oset))) )
	//if( ((!HASHCROP(~oset))) == ((HASHCROP(~oset))) )
	//if(!HASHCROP(~oset) && !HASHCROP(~mset))
	//if( ((int)(!!(int)HASHCROP(~oset))) != (int)(!!(int)(~oset)) )
	//	InfoMess("!","!");

	if( ((mset & mval) < (mset & mlow)) || 
		((inmask & oset & ovaldown) != (inmask & oset & in)) ||
		((mset & mval) > (mset & high)) )
		return;

	//if(!lowlim && DidTry(tried, ovaldown, oset))
	//	return;

	HASHADDR bin = fullbin%HASHBITS;
	HASHADDR backbin = (bin+HASHBITS-1)%HASHBITS;
	//HASHADDR backbin2 = rightbin%HASHBITS;
	//HASHADDR bin2 = (backbin2+1)%HASHBITS;

	//if(backbin == setbin)
	//	return;

#if 0
	Log("start l%d b%d bb%d d%d o%d l%d %d %dm", 
		(int)0, (int)bin, (int)backbin, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset);
#endif

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;
	//HASHINT bm2 = ((HASHINT)1)<<bin2;
	//HASHINT bbm2 = ((HASHINT)1)<<backbin2;
	
	HASHINT shifto = HASHROTL(ovaldown,1);
	HASHINT shiftoset = HASHROTL(oset,1);
	
	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0000
	//1111
	//0101

	//0000
	//1111
	//1010

	//0000
	//0101
	//0101

	//0000
	//0111
	//0101

	//0000
	//0011
	//1110

	////////////////////

	//0001
	//1111
	//0110

	//0001
	//0101
	//1101

	//0001
	//0111
	//1101
	
	//0000
	//0011
	//0110

	HASHINT case0 = HASHCROP( ~oval & (~shifto & bm & shiftoset) );	//0?
	HASHINT case1 = HASHCROP( ~oval & (~ovaldown & bm & oset) );	//?0
	HASHINT case2 = HASHCROP( ~oval & (shifto & ovaldown & bm & shiftoset & oset) );	//11
	HASHINT error0 = HASHCROP( ~oval & (~shifto & ~ovaldown & bm & shiftoset & oset) );
	HASHINT error1 = HASHCROP( case0 & mset & ~mval );
	HASHINT error2 = ( case2 & mset & mval );

	HASHINT case3 = ( oval & (shifto & bm & shiftoset) );	//1?
	HASHINT error3 = HASHCROP( case3 & oset & ~ovaldown );
	HASHINT error4 = HASHCROP( case3 & mset & ~mval );
	HASHINT case4 = HASHCROP( oval & (oset & shiftoset & ~shifto & ovaldown & bm ) );	//01
	HASHINT error5 = ( case4 & mset & mval );
	HASHINT case5 = ( oval & (oset & ovaldown & bm) );	//?1
	HASHINT error6 = HASHCROP( oval & (shiftoset & shifto & bm & oset & ~ovaldown) );

	HASHINT case6 = case0 | case2 | case4 | case3 | (oset & ovaldown & bm);	//?1
	HASHINT case7 = case1 | (oset & ~ovaldown & bm);	//?0
	
	HASHINT case8 = case0 | case4 | (shiftoset & ~shifto & bm);	//0?
	HASHINT case9 = ( oval & (oset & shiftoset & ovaldown & shifto & bm) );	//11
	HASHINT case10 = case1 | case9 | case3 | (shiftoset & shifto & bm);	//1?

	ovaldown |= case6;
	ovaldown &= HASHCROP( ~case7 );

	ovaldown |= HASHROTR(case10,1);
	ovaldown &= HASHCROP( ~HASHROTR(case8,1) );

	HASHINT case11 = case2 | case4 | (mset & bm & ~mval);	//case m 0
	HASHINT case12 = case0 | case9 | case3 | (mset & bm & mval);	//case m 1

	HASHINT case13 = (case10 & (case11 | case7)) | (case8 & case6 & case12) | (~oval & bm);	//up0
	HASHINT case14 = (case10 & case6 & case12) | (case8 & (case11 | case7)) | (oval & bm);	//up1

	mval |= case12;
	mval &= HASHCROP( ~case11 );
	
#if 0
	Log("mid l%d b%d bb%d c0%d c1%d c2%d 3%d 4%d 5%d 6%d 7%d 8%d 9%d 10%d 11%d 12%d 13%d 14%d e%d 1%d 2%d 3%d 4%d 5%d %d d%d o%d l%d %d %dm %d", 
		(int)0, (int)bin, (int)backbin, (int)case0, (int)case1, (int)case2, (int)case3, (int)case4, (int)case5, (int)case6, (int)case7, (int)case8, (int)case9, (int)case10, (int)case11,
		(int)case12, (int)case13, (int)case14, (int)error0, (int)error1, (int)error2, (int)error3, (int)error4, (int)error5, (int)error6, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset, (int)shiftoset);
#endif

	if(error0 || error1 || error2 || error3 || error4 || error5 || error6)
		return;

	if((case11 && case12) || (case8 && case10) || (case6 && case7) | (case13 && case14))
		return;

	if(!HASHCROP(~oset) && !HASHCROP(~mset))
	{
		//if(mval >= mlow)
		{
			Mask m;
			m.mask = mval;
			m.ovaldown = ovaldown;
			m.maskup = maskup;
			mask->push_back(m);

			//if(!lowlim)
			//	AddTried(tried, ovaldown);
	
			//Log("%d for %d->%d", (int)mval, (int)ovaldown, (int)oval);

#if 0
			HASHINT s = HASHROTL(ovaldown,1);
			HASHINT o = HASHCROP( s ^ ~(mval & ovaldown) );

			if(o != oval)
			{
				char ms[123];
				sprintf(ms, "er %u->%u->%u != %u", (int)ovaldown, (int)mval, (int)o, (int)oval);
				InfoMess(ms,ms);
			}
#endif		
#if 0
			Log("add %u->%u->%u", (int)ovaldown, (int)mval, (int)oval);

			if(oval == 21847)
			{
				
			Log("addadd %u->%u->%u", (int)ovaldown, (int)mval, (int)oval);
			}
#endif
		
#if 0
			if( !(mask->size() % 100) )
			{
				mask->sort(CompareMask);
				mask->unique(UniqueMask);
			}
#endif
		}
		return;
	}

	oset |= bm;
	oset |= bbm;
	mset |= bm;

#if 0
	Log("downfill l%d b%d bb%d c0%d c1%d c2%d 3%d 4%d 5%d 6%d 7%d 8%d 9%d 10%d 11%d 12%d e%d 1%d 2%d 3%d 4%d 5%d %d d%d o%d l%d %d %dm %d", 
		(int)0, (int)bin, (int)backbin, (int)case0, (int)case1, (int)case2, (int)case3, (int)case4, (int)case5, (int)case6, (int)case7, (int)case8, (int)case9, (int)case10, (int)case11,
		(int)case12, (int)error0, (int)error1, (int)error2, (int)error3, (int)error4, (int)error5, (int)error6, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset, (int)shiftoset);
#endif
	
	HASHADDR nextbin = backbin;
	//HASHADDR nextbin = bin+(HASHADDR)!check;

#if 0
	if((!case11 && !case12) ||
		(!case8 && !case10) ||
		(!case6 && !case7))
#endif
	if(check)
		nextbin = bin;

	check = !check;
	
	for(HASHINT r=0; r<2; ++r)
	{
		for(HASHINT l=0; l<2; ++l)
		{
			for(HASHINT m=0; m<2; ++m)
			{
				Fill(oval, ovaldown, oset, nextbin, setbin, mlow, mval, mask, mset, check, in, inmask, high, maskup, lowlim, tried);

				if(!case11 && !case12 && !check)
					mval ^= (((HASHINT)1)<<bin);
				else
					break;
			}
		
			if(!case8 && !case10 && !check)
				ovaldown ^= (((HASHINT)1)<<backbin);
			else
				break;
		}
	
		if(!case6 && !case7 && !check)
			ovaldown ^= (((HASHINT)1)<<bin);
		else
			break;
	}

#if 0
	HASHINT m = 1<<bin;
	HASHINT bm = 1<<backbin;

	HASHINT ib = HASHROTR( (ovaldown&bm), 1 );
	HASHINT i0 = (~oval & );
	HASHINT i1 = (~oval & );

	if(oval && ib && !(i0 || i1))
		return;

	oval |= i0;

	Fill(oval, ovaldown, oset, bin+1, setbin, mval, mask, mbin, tried);

	if(i0 == i1)
		return;

	ovaldown &= ~bm;
	ovaldown |= i1;
	Fill(oval, ovaldown, oset, bin+1, setbin, mval, mask, mbin, tried);
#endif

	//choice 1
	//so i[#-1]=1 and i[#]=1 and m0[#]=1
	//or i[#-1]=0 and i[#]=0 and m0[#]=0
	//or i[#-1]=0 and i[#]=0 and m0[#]=1
	//or i[#-1]=0 and i[#]=1 and m0[#]=0

	//choice down 0
	//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
	//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
	//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
	//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0 0 1 0 0 1 1 1    0 1 0 1 0 1 1 1    0 0 1 0 0 1 0 1    0 1 0 1 0 1 0 1
	//0 1 1 0 1 1 1 1
	//0 1 1 1 0 0 0 0
	//0 1 1 1 0 0 0 1
	//0 1 1 1 0 0 1 1
	//0 1 1 1 1 1 1 1
	//1 0 0 1 0 0 0 0
	//1 0 0 1 0 0 0 1
	//1 0 0 1 0 0 1 1
	//1 0 0 1 0 1 1 1
	//1 0 0 1 1 1 1 1
	//1 0 1 0 1 1 1 1
	//1 0 1 1 0 0 0 0
	//1 0 1 1 0 0 0 1
	//1 0 1 1 0 0 1 1
	//1 0 1 1 0 1 1 1
	//1 0 1 1 1 1 1 1
	//1 1 1 0 1 1 1 1
	//1 1 1 1 0 0 0 0
	//1 1 1 1 0 0 0 1
	//1 1 1 1 0 0 1 1
	//1 1 1 1 0 1 1 1
	//1 1 1 1 1 1 1 1

	//0 0 1 0
	//0 1 1 1
	//1 0 0 1
	//1 0 1 0
	//1 0 1 1
	//1 1 1 0
	//1 1 1 1

	//0 1 1 1
	//0 0 0 0
	//0 0 0 1
	//0 0 1 1
	//0 1 1 1
	//1 1 1 1

	//0 1 0 1
	//0 0 1 1
	//0 1 0 0
	//0 1 0 1
	//0 1 1 1
	//1 1 0 0
	//1 1 0 1
	//1 1 1 1
}

void Merge(std::list<Tried>* t)
{
	return;
again:
	for(auto t1=t->begin(); t1!=t->end(); t1++)
	{
		auto t2=t->begin();
		for(t2++; t2!=t->end(); t2++)
		{
			//if(t2==t1)
			//	continue;
			
			HASHINT d = (HASHCROP(~t1->mask) & t1->tried) ^ 
				(HASHCROP(~t2->mask) & t2->tried);


			HASHINT l1 = HASHCROP( LOWEST1(d) );

			if(!l1)
			{
				//0m
				//mm
				//
				//0m
				//m0
				d = t1->mask ^ t2->mask;

				if(d)
				{
					l1 = HASHCROP( LOWEST1(d) );
					d ^= l1;

					if(!d)
					{
						//one dif
						if(t1->mask < t2->mask)
							t->erase(t1);
						else
							t->erase(t2);

						goto again;
					}

					continue;
				}

				t->erase(t2);
				goto again;
			}

			d ^= l1;

			if( HASHCROP( LOWEST1(d) ) )
				continue;

			//one

			if(t1->mask != t2->mask)
				continue;

			t1->mask |= l1;
			t->erase(t2);
			
			//0m
			//m1

			//00
			//01

			//01
			//11
			
			goto again;
		}
	}
}

void AddTried(std::list<Tried>* t, HASHINT s)
{
#if 0
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		HASHINT d = (HASHCROP(~tit->mask) & tit->tried) ^ 
			(HASHCROP(~tit->mask) & s);

		HASHINT l1 = HASHCROP( LOWEST1(d) );

		Log("t %d,%d,%d", (int)tit->tried, (int)tit->mask, (int)s);

		if(!l1)
		{
			Log("ret %d %d m%d", (int)s, (int)tit->tried, (int)tit->mask);
			return;	//must be one or more
		}

		d ^= l1;

		if( HASHCROP( LOWEST1(d) ) )
		{
			Log("cona %d %d m%d", (int)s, (int)tit->tried, (int)tit->mask);
			continue;	//must be single
		}

		if(!tit->mask)
			tit->mask |= l1;

		Log("add a (s%d,m%d)(%d) merge", (int)tit->tried, (int)tit->mask,
			(int)s);

#if 0
		for(auto tit2=t->begin(); tit2!=t->end(); ++tit2)
		{
			if(tit2 == tit)
				continue;

			if((HASHCROP(~tit->mask) & tit->tried) != 
				(HASHCROP(~tit2->mask) & tit2->tried))
				continue;

			d = tit->mask ^ tit2->mask;

			l1 = HASHCROP( LOWEST1(d) );

			if(!l1)
				continue;

			//merge

			d ^= l1;

			if( HASHCROP( LOWEST1(d) ) )
				continue;	//must be single

			Log("add b (s%d,m%d)(%d,%d) merge erase min", (int)tit->tried, (int)tit->mask,
				(int)tit2->tried, (int)tit2->mask);

			if(tit->mask < tit2->mask)
				t->erase(tit);
			else
				t->erase(tit2);

			return;
		}
#endif
		goto merge;
	}
#endif
	Tried st;
	st.tried = s;
	st.mask = 0;
	t->push_back(st);

merge:

	Merge(t);
	
		Log("add add (%d) merge", 
			(int)s);
}

bool DidTry(std::list<Tried>* t, HASHINT s)
{
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		if((HASHCROP(~tit->mask) & tit->tried) == 
			(HASHCROP(~tit->mask) & s))
			return true;
	}

	return false;
}


bool DidTry(std::list<Tried>* t, HASHINT s, HASHINT setmask)
{
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		if(((HASHCROP(~tit->mask) & tit->tried) == (HASHCROP(~tit->mask) & s)) &&
			!(HASHCROP(~tit->mask) & HASHCROP(~setmask)))
		{
			Log("true tit%d,s%d, s%d,sm%d ((%d=(%d==%d)) && (%d=!(%d&%d)))", 
				(int)tit->mask, (int)tit->tried,
				(int)s, (int)setmask,
				(int)((HASHCROP(~tit->mask) & tit->tried) == (HASHCROP(~tit->mask) & s)),
				(int)(HASHCROP(~tit->mask) & tit->tried), 
				(int)(HASHCROP(~tit->mask) & s),
				(int)!(HASHCROP(~tit->mask) & HASHCROP(~setmask)),
				(int)HASHCROP(~tit->mask),
				(int)HASHCROP(~setmask));
			return true;
		}
	}

	return false;
}

//propagate down to gotten input
bool Down(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried)
{
	Log("lev%d m%d mask%u to%u lowlim%d od%llu", (int)level, (int)masksup->size(), (int)masksup->begin()->mask, (int)masksup->rbegin()->mask, (int)lowlim, masksup->begin()->ovaldown);

	//if(masksup->size() != masksup[1].size())
	//	Log("disc");

	if(level == (HASHADDR)-1)
	{
#if 0
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			std::list<Mask>* mu = masksup[mci];

			for(auto mit=mu->begin(); mit!=mu->end(); mit++)
			{
			
		//Log("upmask=%u in=%u", (int)mit->mask, (int)mit->ovaldown);

				if (mit->ovaldown != in)
					continue;

			
		//Log("upmask=%u in=%u", (int)mit->mask, (int)mit->ovaldown);

				*maskup = mit->mask;
				goto next;
			}

			return false;

next:
		}
#else
		*maskup = masksup[0].begin()->mask;
#endif
		return true;
	}

	//HASHINT m = mval[level];
	//HASHINT mset = HASHCROP( (HASHINT)-1 );
	HASHINT mset = 0;
	HASHINT oval = 0;
	//HASHINT mlow = mval[level];

	if(!lowlim)
		mval[level] = 0;

	//HASHADDR upbin=0;
	HASHADDR upbin=HASHBITS-1;

	//if(!lowlim)
	//	upbin = HASHBITS-1;

	HASHINT* zeromask = new HASHINT[nmc];
	memset(zeromask, 0, sizeof(HASHINT)*nmc);

	HASHINT* inmask = zeromask;
	
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);

	if(!level)
	{
		//inmask = (HASHINT)-1;
		//memset(zeromask, (HASHINT)-1, sizeof(HASHINT)*nmc);
		memset(zeromask, -1, sizeof(HASHINT)*nmc);

		
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);
	}

	HASHINT* inval = in;

	if(level==1)
	{
		inval = upin;
		inmask = upinmask;
	}

	
					//Log("inmask[mci]%d", (int)inmask[0]);
					//Log("zeromask[mci]%d", (int)zeromask[0]);
					//Log("in[mci]%d", (int)in[0]);

	HASHINT high = mval[level];
	HASHINT lowest = mval[level];

	if(!high)
		high = 1;

	//high = (HASHINT)-1;

	//Log("low %llu", lowest);

	//for(; upbin<HASHBITS;)
	//while(high)
	
	std::list<Mask>* masks = new std::list<Mask>[nmc];
	HASHINT upmask = 0;
	HASHINT mci = 0;

	while(mci<nmc)
	{
skipmc:
		//Log("skipmc");

		std::list<Mask>* mu = &masksup[mci];
		std::list<Mask>* m = &masks[mci];

		auto muit = mu->begin();

nextmu:
		//Log("nextmu");
		
		std::list<Tried>* tlc = &tried[nmc*level+mci];

		//BinTree tried;
		//HASHINT tried[2] = {0,0};

		while(muit!=mu->end() && muit->mask < upmask)
			muit++;
		
		auto start = muit;

		while(muit!=mu->end())
		{
			//Log("while(muit!=mu->end())");

			upmask = muit->mask;
			HASHINT oval = muit->ovaldown;
			//HASHINT oshift = HASHROTR(oval,1);

			if(upmask > *maskup && lowlim)
			{
				mval[level] = 0;
				lowest = 0;
				high = 1;
				lowlim = false;
				//upbin = HASHBITS-1;
				//Log("upmask > *maskup && lowlim");
			}

			//Log("lev%d m%d continue mask%u", (int)level, (int)masksup->size(), (int)upmask);

			//for(HASHADDR setbin=0; setbin<HASHBITS; ++setbin)
			const HASHADDR setbin=upbin;
			{
				//HASHINT bm = 1<<setbin;

				//if(!mci)
				//for(HASHADDR v=0; v<2; ++v)
				{
					HASHINT ovaldown = 0;
					HASHINT oset = 0;
					//HASHINT ovaldown = ((HASHINT)v)<<setbin;
					//HASHINT oset = ((HASHINT)1)<<setbin;
					//HASHINT mset = HASHCROP( ~((((HASHINT)1)<<(upbin+1))-1) );
					//mset = 1<<setbin;
					//
					//Log("inmask[mci]%d", (int)inmask[mci]);
					//Log("zeromask[mci]%d", (int)zeromask[mci]);
					//Log("inmask[mci]%d", (int)inmask[0]);
					//Log("zeromask[mci]%d", (int)zeromask[0]);
					Fill(oval, ovaldown, oset, setbin, setbin, mval[level], 0, m, mset, true, inval[mci], inmask[mci], high, upmask, lowlim, tlc);
				}
#if 0
				else
				{
					std::list<Mask>* ncmasks = &masks[0];
					for(auto ncmit=ncmasks->begin(); ncmit!=ncmasks->end(); ncmit++)
					{
						HASHINT ovaldown = 0;
						HASHINT oset = 0;
						HASHINT mv = ncmit->mask;
						HASHINT ms = HASHCROP( ~(HASHINT)1 );
						Fill(oval, ovaldown, oset, setbin, setbin, mval[level], mv, m, ms, true, inval[mci], inmask[mci], high, upmask, lowlim, tlc);
					}
				}
#endif
			}

			muit++;

			if(muit == mu->end() || muit->mask > upmask)
			{
				m->sort(CompareMask);
				//m->unique(UniqueMask);	//only for 1 ch

				for(HASHINT mci2=0; mci2<mci; ++mci2)
					Narrow(m, &masks[mci2]);

				//Log("muit == mu->end() || muit->mask > upmask");

				//if(!m->size())
				//	goto fail;
#if 0
				//HASHINT maskdown;

				std::list<Mask> submasks;

				auto smit=masks.rbegin();

				while(smit!=masks.rend() && smit->maskup >= upmask)
					smit++;

				//smit--;
				//auto fsmit = --(smit.base());
				auto fsmit = (smit.base());

				while(fsmit!=masks.end())
				{
					//Log("add upmask %llu %llu %llu", fsmit->maskup, fsmit->mask, fsmit->ovaldown);
					submasks.push_back(*fsmit);
					fsmit++;
				}
#endif

#if 0
				for(auto cit=submasks.begin(); cit!=submasks.end(); cit++)
				{
					Log("(%u) %u->%u->%u", (int)cit->maskup, (int)cit->ovaldown, (int)cit->mask, (int)oval);
				}
				Log("e");
#endif
				//Log("up %u", (int)upmask);
				//Log("m%d", (int)submasks.size());

				if(mci == nmc-1)
				{
					//Log("nmc-1");

					//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
					if(m->size() && Down(masks, mval, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried))
					{
						//Log("m from%llu to %llu", submasks.begin()->maskup, submasks.rbegin()->maskup);

						//Log("set mask[%d] = %u", (int)level, (int)mval[level]);
						*maskup = upmask;

						for(HASHINT mci2=0; mci2<nmc; ++mci2)
						{
							
							std::list<Mask>* mu2 = &masksup[mci2];

							for(muit=mu2->begin(); muit!=mu2->end() && muit->mask<upmask; ++muit)
								;
#if 0
							if(level>0)
							Log("lev%d tried(%dup)=%d ch%dm%d", 
							(int)level, (int)muit->ovaldown, 
							(int)DidTry(&tried[nmc*(level-1)+mci], muit->ovaldown,
							(HASHINT)-1),
							(int)mci,
							(int)muit->mask);
							else
								Log("top");
#endif
						}

						delete [] masks;
						delete [] zeromask;
						
						//Log("success");

						return true;
					}
#if 0
					else if(m->size())
						Log("upmask%d lev%d end%d high%d", 
						(int)upmask, (int)level, (int)(muit==mu->end()),
						(int)high);
					
					if(!lowlim)
					{
						for(HASHINT mci2=0; mci2<nmc; ++mci2)
						{
							std::list<Mask>* mu2 = &masks[mci2];
							std::list<Tried>* tlc2 = &tried[nmc*level+mci2];
							for(auto mit=mu2->begin(); mit!=mu2->end(); mit++)
							{
								AddTried(tlc2, mit->ovaldown);
							}
						}
					}
#endif

					for(; mci!=(HASHINT)-1; --mci)
						masks[mci].clear();

					//muit = start;

					mval[level] = high;

					HASHINT low0 = HASHCROP( LOWEST0( high ) );
					high |= low0;

					//high <<= 1;

					//high = HASHCROP( high );

					if(low0)
					{
						muit = start;

						//Log("muit = start");

					}
					else
					{
						if(muit == mu->end())
						{
							Log("low0 fail lev%d", (int)level);
							goto fail;
						}

						//Log("low0 else lev%d low0=%d muit->mask%d, upmask%d", 
						//	(int)level, (int)low0, (int)muit->mask, (int)upmask);

						start = muit;
						high = lowest;

						if(!high)
							high = 1;

						//lowlim = false;
						mval[level] = lowest;
						upmask = muit->mask;
					}

					mci = 0;
					goto skipmc;
				}
				else
				{
					muit = start;
					//Log("muit = start;");
				}

				goto nextmc;

#if 0
				if(!lowlim && muit == masksup->end())
					return false;
				else if(upmask > *maskup && lowlim)
				{
					lowlim = false;
					upbin = HASHBITS-1;
					mval[level] = 0;
					goto next;
				}
#endif
#if 0
				for(; upbin<HASHBITS; ++upbin)
				{
					HASHINT mm = ((HASHINT)1)<<upbin;

					if(mval[level] & mm)
					{
						mval[level] &= HASHCROP( ~mm );
						continue;
					}

					mval[level] |= mm;
					muit = start;
					goto next;
				}

				if(muit!=masks.end())
				{
					upbin = 0;
					mval[level] = 0;
					lowlim = false;
					start = muit;
					continue;
				}

				return false;
#endif
				//if(muit == masksup->end())
				//	return false;
			}
		}

		//mval[level] = high;
		//high <<= 1;
		
nextmc:
		++mci;
		//Log("nextmc");
	}

fail:

	delete [] masks;
	delete [] zeromask;

	return false;
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

						//choice 1
						//so i[#-1]=1 and i[#]=1 and m0[#]=1
						//or i[#-1]=0 and i[#]=0 and m0[#]=0
						//or i[#-1]=0 and i[#]=0 and m0[#]=1
						//or i[#-1]=0 and i[#]=1 and m0[#]=0
					
							//choice down 0
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

HASHINT Hash(HASHINT mval[HASHLEVELS], HASHINT oval, HASHADDR level)
{
	if(level >= HASHLEVELS)
		return oval;

	HASHINT shifto = HASHROTL(oval,1);
	const HASHINT ovalup = HASHCROP( shifto ^ ~(oval & mval[level]) );

	Log("level%d up%u o%u os%u", (int)level, (int)ovalup, (int)oval, (int)shifto);

	return Hash(mval, ovalup, level+1);
}

void Flush(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], SHASHADDR level)
{
	HASHINT oval = ovals[level];

	for(; level<HASHLEVELS; ++level)
	{
		const HASHINT shifto = HASHROTL(oval,1);
		oval = HASHCROP( shifto ^ ~(oval & mval[level]) );

		//Log("level %d", (int)level);
		//g_applog.flush();

		if(ovals[level+1] == oval)
			break;

		ovals[level+1] = oval;
	}
}

void FullFlush(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], SHASHADDR level)
{
	HASHINT oval = ovals[level];

	for(; level<HASHLEVELS; ++level)
	{
		const HASHINT shifto = HASHROTL(oval,1);
		oval = HASHCROP( shifto ^ ~(oval & mval[level]) );
		ovals[level+1] = oval;
	}
}

bool Check(HASHINT mval[HASHLEVELS], HASHINT in, HASHINT out, HASHADDR checkbin)
{
	HASHINT oval = in;

	for(HASHADDR i=0; i<HASHLEVELS; ++i)
	{
		HASHINT shifto = HASHROTL(oval,1);
		oval = HASHCROP( shifto ^ ~(oval & mval[i]) );
	}

	HASHINT checkbm = 1 << checkbin;

	return ((oval & checkbm) == (out & checkbm));
}

bool UpBuffer(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], HASHADDR checkbit, HASHADDR bin)
{
	HASHADDR decline = bin - checkbit;

	if(bin < checkbit)
		decline = ((HASHBITS-1)-checkbit)+bin;
	
	SHASHADDR height = (HASHLEVELS-1) - decline;

	height = imax(0, height);

	HASHINT bm = 1 << bin;

	for(SHASHADDR l=(HASHLEVELS-1); l>=height; --l)
	{
		if(mval[l] & bm)
			continue;

		mval[l] |= bm;

		for(HASHADDR l2=l+1; l2<HASHLEVELS; ++l2)
			mval[l2] &= HASHCROP( ~bm );

		Flush(mval, ovals, l);

		return true;
	}

	//InfoMess("!upb","!upb");

	return false;
}

//increment side until change happens effect
//stay in column
bool UpSide2(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], HASHINT out, HASHADDR checkbit, HASHADDR bin)
{
	HASHADDR decline = bin - checkbit;

	if(bin < checkbit)
		decline = ((HASHBITS-1)-checkbit)+bin;
	
	SHASHADDR height = (HASHLEVELS-1) - decline;

	height = imax(0, height);

	HASHINT bm = 1 << bin;

	for(SHASHADDR l=height; l>=0; --l)
	{
		if(mval[l] & bm)
		{
			mval[l] &= HASHCROP( ~bm );
			Flush(mval, ovals, l);
			continue;
		}

		mval[l] |= bm;

		HASHINT oval = ovals[l];
		HASHINT shifto = HASHROTL(oval,1);
		oval = shifto ^ ~(oval & mval[l]);

		HASHADDR stretch = (HASHLEVELS-1) - l;
		HASHINT effect = 0;
		effect |= ((1<<(checkbit+stretch+1))-1);
		effect &= ~((1<<checkbit)-1);
		effect |= ((1<<(((checkbit+stretch)&(HASHBITS-1))+1))-1);
		effect = HASHCROP(effect);

		if( (oval & effect) != (ovals[l+1] & effect) )
		{
			Flush(mval, ovals, l);
			return true;
		}

		mval[l] &= HASHCROP( ~bm );
	}

	//InfoMess("!ups","!ups");

	Flush(mval, ovals, 0);
	return false;
}

//reset unused 'buffer' bits
void ResetBuffer(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], HASHINT out, HASHADDR checkbit, HASHADDR bin)
{
	HASHINT bm = 1 << bin;

	HASHADDR decline = bin - checkbit;

	if(bin < checkbit)
		decline = ((HASHBITS-1)-checkbit)+bin;

	SHASHADDR height = (HASHLEVELS-1) - decline;

	height = imax(0, height);

	//Log("hl%d,   h %d    d %d     b%d    c%d", (int)HASHLEVELS, (int)height, (int)decline, (int)bin, (int)checkbit);
	//g_applog.flush();

	for(SHASHADDR l=(HASHLEVELS-1); l>height; --l)
		mval[l] &= HASHCROP( ~bm );

	Flush(mval, ovals, height);
}

bool MatchBit2(HASHINT mval[HASHLEVELS], HASHINT ovals[HASHLEVELS+1], HASHINT out, HASHADDR checkbit)
{
	uint64_t full = 0;
	HASHINT cbm = 1 << checkbit;

	HASHADDR bin=0;
	ResetBuffer(mval, ovals, out, checkbit, bin);

	//send pulse
	while(true)
	{
		HASHINT bm = 1 << bin;

		if(UpSide2(mval, ovals, out, checkbit, bin))
		{
			if( (ovals[HASHLEVELS] & cbm) == (out & cbm) )
				return true;
			
			bin = 0;

			continue;
		}

		
		//if(GetTicks() % 1000 == 0)
		if(0)
		{
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %u", (int)x, mval[x]);
			}

			Log("matching bit%d %u-%u=%u", (int)checkbit, ovals[HASHLEVELS], out, ovals[HASHLEVELS]-out);
		}

		if(bin+1 >= HASHBITS)
		{
			//MessageBox(NULL, "!>hb", "!>hb", NULL);

#if 0
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t >mask[%d] = %u", (int)x, mval[x]);
			}

			Log("no bit%d %u-%u=%u", (int)checkbit, ovals[HASHLEVELS], out, ovals[HASHLEVELS]-out);
#endif

			return false;
		}

		if( !(full & bm) && UpBuffer(mval, ovals, checkbit, bin+1) )
		{
			full |= bm;
			Flush(mval, ovals, 0);
			bin = 0;
			continue;
		}

		++bin;

		ResetBuffer(mval, ovals, out, checkbit, bin);
	}
}

//opt 7
bool Try12(HASHINT mval[HASHLEVELS], HASHINT in, HASHINT out)
{
	HASHINT ovals[HASHLEVELS+1];
	ovals[0] = in;
	HASHINT oval = ovals[0];

	for(HASHADDR l=0; l<HASHLEVELS; ++l)
	{
		HASHINT shifto = HASHROTL(oval,1);
		oval = HASHCROP( shifto ^ ~(oval & mval[l]) );
		ovals[l+1] = oval;
	}

	//Log("start");

#if 1
	for(SHASHADDR bin=(HASHBITS-1); bin>=0; --bin)
	{
		//Log("bin%d", (int)bin);

		HASHINT bm = 1 << bin;

		if( (ovals[HASHLEVELS] & bm) == (out & bm) )
		{
			//Log("skip match %u-%u=%u", ovals[32], out, ovals[32]-out);
			continue;
		}

		if(!MatchBit2(mval, ovals, out, bin))
		{
			//Log("matched bit%d %u-%u=%u", (int)bin, ovals[32], out, ovals[32]-out);
			return false;
		}

		//if(GetTicks() % 1000 == 0)
		if(0)
		{
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %u", (int)x, mval[x]);
			}

			Log("matched bit%d %u-%u=%u", (int)bin, ovals[HASHLEVELS], out, ovals[HASHLEVELS]-out);
		}

		bin = (HASHBITS-1);
	}
#else

	for(HASHADDR bin=0; bin<HASHBITS; ++bin)
	{
		//Log("bin%d", (int)bin);

		HASHINT bm = 1 << bin;

		if( (ovals[HASHLEVELS] & bm) == (out & bm) )
		{
			//Log("match %u-%u=%u", ovals[32], out, ovals[32]-out);
			continue;
		}

		if(!MatchBit2(mval, ovals, out, bin))
			return false;

		//Log("matched bit%d %u-%u=%u", (int)bin, ovals[32], out, ovals[32]-out);

		bin = 0;
	}
#endif

	//Log("fin");

	return true;
}

#endif
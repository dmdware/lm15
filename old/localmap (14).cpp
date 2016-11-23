

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

		//Log("start");

		auto mit = map.begin();

		while(mit != map.end())
		{
			HASHINT old[HASHLEVELS];
			HASHINT m2[HASHLEVELS];
			memcpy(old, m, sizeof(HASHINT) * HASHLEVELS);
			memcpy(m2, m, sizeof(HASHINT) * HASHLEVELS);

			//Log("%u->%u", (int)mit->i, (int)mit->o);

			std::list<Mask> masks;
			Mask mask;
			mask.ovaldown = mit->o;
			mask.mask = 0;
			masks.push_back(mask);
			
			//Log("answer2:");

			HASHINT inval = 0;
			HASHINT inmask = 0;

			Up(mit->i, &inval, &inmask);

			if(Down(&masks, m, &mask.mask, HASHLEVELS-1, mit->i, true, inval, inmask))
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

					Log("nor");

					for(SHASHADDR x=0; x<HASHLEVELS; ++x)
					{
						Log("\t wmask[%d] = %u", (int)x, (int)m[x]);
					}

				uint64_t p = GetTicks() - t;

				char ms[123];
				sprintf(ms, "fail %llu on %d", p, (int)mit->o);
				MessageBox(NULL, ms, ms, NULL);

				return;
			}

			//Log("success %u->%u", (int)mit->i, (int)mit->o);

			uint64_t p = GetTicks() - t;

			char ms[123];
			sprintf(ms, "passt %llu", p);
			Log("%s", ms);

			t = GetTicks();
			
#if 0
			
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

#if 1
			if((Ahead(old, m) || Ahead(m, old)) && mit != map.begin())
				mit = map.begin();
			else
#endif
				mit++;
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

void Fill(HASHINT oval, HASHINT ovaldown, HASHINT oset, HASHADDR fullbin, HASHADDR setbin, 
		  HASHINT mlow, HASHINT mval, std::list<Mask>* mask, HASHINT mset, bool check, HASHINT in, HASHINT inmask, HASHINT high, HASHINT maskup)
{
#if 0
	Log("ovaldown%d oval%d mset%d oset%d q%d mlow%d inmask%d h%d !HASHCROP(~oset)%d !HASHCROP(~mset)%d HASHCROP(~oset)%d HASHCROP(~mset)%d ~oset%d ~mset%d MSHHASH%d (int)HASHBITS%d, (int)(1<<HASHBITS)%d, (int)((1<<HASHBITS)-1)%d", 
		(int)ovaldown, (int)oval,
		(int)mset, (int)oset, (int)(!HASHCROP(~oset) && !HASHCROP(~mset)),
		(int)mlow, (int)inmask, (int)high,
		(int)(!HASHCROP(~oset)), (int)(!HASHCROP(~mset)), (int)(!!(int)HASHCROP(~oset)), (int)(!!HASHCROP(~mset)), (int)(!!(int)(~oset)), (int)(~mset), (int)MAXHASH, (int)HASHBITS, (int)(1<<HASHBITS), (int)((1<<HASHBITS)-1));
#endif

	if( ((mset & mval) < (mset & mlow)) || 
		((inmask & oset & ovaldown) != (inmask & oset & in)) ||
		((mset & mval) > (mset & high)) )
		return;
	
#if 0
	if(!lowlim && DidTry(tried, ovaldown, oset))
		return false;
	if(!lowlim && wtried && DidTry(wtried, ovaldown, oset))
		return true;
#endif

	HASHADDR bin = fullbin%HASHBITS;
	HASHADDR backbin = (bin+HASHBITS-1)%HASHBITS;

#if 0
	Log("start l%d b%d bb%d d%d o%d l%d %d %dm", 
		(int)0, (int)bin, (int)backbin, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset);
#endif

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;
	
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
		Mask m;
		m.mask = mval;
		m.ovaldown = ovaldown;
		m.maskup = maskup;
		mask->push_back(m);

		//if(!lowlim)
		//	AddTried(tried, ovaldown);

		//Log("%d for %d->%d", (int)mval, (int)ovaldown, (int)oval);

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
				Fill(oval, ovaldown, oset, nextbin, setbin, mlow, mval, mask, mset, check, in, inmask, high, maskup);

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

//propagate down to gotten input
bool Down(std::list<Mask>* masksup, HASHINT mval[HASHLEVELS], HASHINT* maskup, HASHADDR level, HASHINT in, bool lowlim, HASHINT upin, HASHINT upinmask)
{
	//Log("lev%d m%d mask%u to%u lowlim%d od%llu", (int)level, (int)masksup->size(), (int)masksup->begin()->mask, (int)masksup->rbegin()->mask, (int)lowlim, masksup->begin()->ovaldown);

	if(level == (HASHADDR)-1)
	{
		for(auto muit=masksup->begin(); muit!=masksup->end(); muit++)
		{

			//Log("upmask=%u in=%u", (int)muit->mask, (int)muit->ovaldown);

			if (muit->ovaldown != in)
				continue;


			//Log("upmask=%u in=%u", (int)muit->mask, (int)muit->ovaldown);

			*maskup = muit->mask;
			return true;
		}

		return false;
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

	HASHINT inmask = 0;

	if(!level)
		inmask = (HASHINT)-1;

	HASHINT inval = in;

	if(level==1)
	{
		inval = upin;
		inmask = upinmask;
	}

	HASHINT high = mval[level];
	HASHINT lowest = mval[level];

	if(!high)
		high = 1;

	//high = (HASHINT)-1;

	//Log("low %llu", lowest);

	//for(; upbin<HASHBITS;)
	//while(high)
	{
		auto muit = masksup->begin();
		auto start = muit;
next:
		std::list<Mask> masks;
		//BinTree tried;

		while(muit!=masksup->end())
		{
			HASHINT upmask = muit->mask;
			HASHINT oval = muit->ovaldown;
			//HASHINT oshift = HASHROTR(oval,1);

			if(upmask > *maskup && lowlim)
			{
				mval[level] = 0;
				lowest = 0;
				high = 1;
				lowlim = false;
				//upbin = HASHBITS-1;
			}

			//Log("lev%d m%d continue mask%u", (int)level, (int)masksup->size(), (int)upmask);

			//for(HASHADDR setbin=0; setbin<HASHBITS; ++setbin)
			HASHADDR setbin=upbin;
			{
				//HASHINT bm = 1<<setbin;

				//for(HASHADDR v=0; v<2; ++v)
				{
					HASHINT ovaldown = 0;
					HASHINT oset = 0;
					//HASHINT ovaldown = ((HASHINT)v)<<setbin;
					//HASHINT oset = ((HASHINT)1)<<setbin;
					//HASHINT mset = HASHCROP( ~((((HASHINT)1)<<(upbin+1))-1) );
					//mset = 1<<setbin;
					//
					Fill(oval, ovaldown, oset, setbin, setbin, mval[level], 0, &masks, mset, true, inval, inmask, high, upmask);
				}
			}

			muit++;

			if(muit == masksup->end() || muit->mask > upmask)
			{
				masks.sort(CompareMask);
				masks.unique(UniqueMask);

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

				//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
				if(masks.size() && Down(&masks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
				{
					//Log("m from%llu to %llu", submasks.begin()->maskup, submasks.rbegin()->maskup);

					//Log("set mask[%d] = %u", (int)level, (int)mval[level]);
					*maskup = upmask;

					return true;
				}

				masks.clear();

				//muit = start;

				mval[level] = high;

				HASHINT low0 = HASHCROP( LOWEST0( high ) );
				high |= low0;

				//high <<= 1;

				//high = HASHCROP( high );

				if(low0)
					muit = start;
				else
				{
					if(muit == masksup->end())
					{
						//Log("low0 fail lev%d", (int)level);
						return false;
					}

					start = muit;
					high = lowest;

					if(!high)
						high = 1;

					//lowlim = false;
					mval[level] = lowest;
					upmask = muit->mask;
				}

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
	}

	return false;
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

#endif
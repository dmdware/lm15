

#include "../utils.h"
#include "../debug.h"
#include "localmap.h"


void TestHash()
{
#if 1
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

		if(Try12(m, mit->i, mit->o))
			;//InfoMess("s","s");
		else
		{
			InfoMess("f","f");
			break;
		}

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

uint8_t Flush(uint32_t mval[32], uint32_t ovals[33], int8_t level)
{
	uint32_t oval = ovals[level];

	for(; level<33; ++level)
	{
		const uint32_t shifto = (oval << 1) | (oval >> 31);
		oval = shifto ^ ~(oval & mval[level]);

		if(ovals[level+1] == oval)
			break;

		ovals[level+1] = oval;
	}

	return level;
}

void FullFlush(uint32_t mval[32], uint32_t ovals[33], int8_t level)
{
	uint32_t oval = ovals[level];

	for(; level<33; ++level)
	{
		const uint32_t shifto = (oval << 1) | (oval >> 31);
		oval = shifto ^ ~(oval & mval[level]);
		ovals[level+1] = oval;
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

bool UpBuffer(uint32_t mval[32], uint32_t ovals[33], uint8_t checkbit, uint8_t bin)
{
	uint8_t decline = bin - checkbit;

	if(bin < checkbit)
		decline = (31-checkbit)+bin;

	uint8_t height = 31 - decline;

	uint32_t bm = 1 << bin;

	for(int8_t l=31; l>=height; --l)
	{
		if(mval[l] & bm)
			continue;

		mval[l] |= bm;

		for(uint8_t l2=l+1; l2<32; ++l2)
			mval[l2] &= ~bm;

		Flush(mval, ovals, l);

		return true;
	}

	return false;
}

//increment side until change happens effect
//stay in column
bool UpSide2(uint32_t mval[32], uint32_t ovals[33], uint32_t out, uint8_t checkbit, uint8_t bin, uint8_t start, uint8_t* left)
{
	uint8_t decline = bin - checkbit;

	if(bin < checkbit)
		decline = (31-checkbit)+bin;

	uint8_t height = 31 - decline;

	uint32_t bm = 1 << bin;

	for(int8_t l=imin(height,start); l>=0; --l)
	{
		if(mval[l] & bm)
		{
			mval[l] &= ~bm;
			Flush(mval, ovals, l);
			continue;
		}

		mval[l] |= bm;

		uint32_t oval = ovals[l];
		uint32_t shifto = (oval << 1) | (oval >> 31);
		oval = shifto ^ ~(oval & mval[l]);

		uint8_t stretch = 31 - l;
		uint32_t effect = 0;
		effect |= ((1<<(checkbit+stretch+1))-1);
		effect &= ~((1<<checkbit)-1);
		effect |= ((1<<(((checkbit+stretch)&31)+1))-1);

		if( (oval & effect) != (ovals[l+1] & effect) )
		{
			*left = Flush(mval, ovals, l);
			return true;
		}

		mval[l] &= ~bm;
	}

	Flush(mval, ovals, 0);
	return false;
}

//reset unused 'buffer' bits
void ResetBuffer(uint32_t mval[32], uint32_t ovals[33], uint32_t out, uint8_t checkbit, uint8_t bin)
{
	uint32_t bm = 1 << bin;

	uint8_t decline = bin - checkbit;

	if(bin < checkbit)
		decline = (31-checkbit)+bin;

	uint8_t height = 31 - decline;

	for(int8_t l=31; l>height; --l)
		mval[l] &= ~bm;

	Flush(mval, ovals, height);
}

bool MatchBit2(uint32_t mval[32], uint32_t ovals[33], uint32_t out, uint8_t checkbit)
{
	uint32_t full = 0;
	uint32_t cbm = 1 << checkbit;

	uint8_t bin=0;
	ResetBuffer(mval, ovals, out, checkbit, bin);

	uint8_t start = 31;
	uint8_t left = 31;

	bool front = false;

	//send pulse
	while(true)
	{
		uint32_t bm = 1 << bin;

		if(UpSide2(mval, ovals, out, checkbit, bin, start, &left))
		{
			if( (ovals[32] & cbm) == (out & cbm) )
				return true;
			
			start = imax(start,left);

			//if(bin)
			//	start = left;

			bin = 0;

			if(front)
			{
				start = left;
				front = false;
			}

			continue;
		}

		if(bin+1 >= 32)
			return false;

		start = 31;

		if( !(full & bm) && UpBuffer(mval, ovals, checkbit, bin+1) )
		{
			full |= bm;
			Flush(mval, ovals, 0);
			bin = 0;
			continue;
		}

		front = true;

		++bin;

		ResetBuffer(mval, ovals, out, checkbit, bin);
	}
}

//opt 7
bool Try12(uint32_t mval[32], uint32_t in, uint32_t out)
{
	uint32_t ovals[33];
	ovals[0] = in;
	uint32_t oval = ovals[0];

	for(uint8_t l=0; l<32; ++l)
	{
		uint32_t shifto = (oval << 1) | (oval >> 31);
		oval = shifto ^ ~(oval & mval[l]);
		ovals[l+1] = oval;
	}

	Log("start");

#if 1
	for(int8_t bin=31; bin>=0; --bin)
	{
		//Log("bin%d", (int)bin);

		uint32_t bm = 1 << bin;

		if( (ovals[32] & bm) == (out & bm) )
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
		{
			for(int8_t x=0; x<32; ++x)
			{
				Log("\t mask[%d] = %u", (int)x, mval[x]);
			}

			Log("matched bit%d %u-%u=%u", (int)bin, ovals[32], out, ovals[32]-out);
		}

		bin = 31;
	}
#else

	for(uint8_t bin=0; bin<32; ++bin)
	{
		//Log("bin%d", (int)bin);

		uint32_t bm = 1 << bin;

		if( (ovals[32] & bm) == (out & bm) )
		{
			//Log("match %u-%u=%u", ovals[32], out, ovals[32]-out);
			continue;
		}

		if(!MatchBit2(mval, ovals, out, bin))
			return false;

		Log("matched bit%d %u-%u=%u", (int)bin, ovals[32], out, ovals[32]-out);

		bin = 0;
	}
#endif

	Log("fin");

	return true;
}

#endif


#include "utils.h"
#include "localmap.h"
#include "procvid.h"

//HASHINT segments[SEGPARTS][HASHLEVELS];
HASHINT segments[MAXSEGIS];

int main(int argc, char** argv)
{
	OpenLog("log.txt", 0);
	TestHash();
	system("pause");
	return 0;
}

		//HASHINT* m;
		//HASHINT* m2;

std::list<Map> mappings[MAXSEGIS];

bool TeachSeg(bool reset, HASHINT* masks, HASHINT* out, HASHINT* in, HASHINT nmc)
{
	HASHINT masks2[HASHLEVELS];

	//if(reset)
	//	return (Down4(out, in, masks, HASHLEVELS, nmc));
	return false;
}

HASHINT Hash(HASHINT inval, HASHINT shinval, HASHINT mval)
{
	HASHINT oval = HASHCROP( ~(mval & ~(inval & shinval)) );
	return oval;
}
class LevelCase;

	class SegCase
	{
	public:
		HASHINT ivals[2];
		HASHINT mval;
		HASHINT oval;
		SegCase* up;
		LevelCase* uplc;
		LevelCase* lc;
		int refs;
	};

	class LevelCase
	{
	public:
		LevelCase* uplc;
		std::vector<std::list<SegCase>> sc;
		//[slot][case]
	};

#if 0
void Narrow(int Bi, LevelCase* plc, int level, int levelsegs)
{
	int shiftamt = level*2+1;
	int segi=Bi%levelsegs; 
//over:
	segi=Bi%levelsegs; 
	bool repeat = false;
over:
	while(segi<FITRESX*FITRESY*3 && segi<=Bi)
	{
		std::vector<SegCase>::iterator sgit=plc->sc[Bi].begin();

		if(Bi==segi)
			goto next1;

		while(sgit!=plc->sc[Bi].end())
		{
			bool found = false;

			std::vector<SegCase>::iterator sgit2=plc->sc[segi].begin();
			while(sgit2!=plc->sc[segi].end())
			{
				if(sgit2->mval == sgit->mval)
				{
					found = true;
					break;
				}

				//Log("ner f%d ls%d", (int)plc->sc[segi].size(), (int)levelsegs);
				++sgit2;
			}

			if(!found)
			{
				//Log("er %d ls%d", (int)plc->sc[segi].size(), (int)levelsegs);
				sgit = plc->sc[Bi].erase(sgit);
				repeat = true;
				//++sgit;
				//goto over;
				continue;
			} 
				//Log("ner %d ls%d", (int)plc->sc[segi].size(), (int)levelsegs);
			++sgit;
		}

next1:
			if(rand()%10001 == 1)
		Log("seg1i%d", (int)segi);
		segi+=(levelsegs);
	}
	if(repeat)
	{
		repeat = false;
		segi=Bi%levelsegs; 
		goto over;
	}

	segi=Bi%levelsegs;
	//int shiftamt = level*2+1;
	repeat = false;
over2:
	while(segi<FITRESX*FITRESY*3 && segi<=Bi)
	{
		std::vector<SegCase>::iterator sgit=plc->sc[segi].begin();

		if(Bi==segi)
			break;

		while(sgit!=plc->sc[segi].end())
		{
			HASHINT i0 = sgit->ivals[0];
			HASHINT i1 = sgit->ivals[1];

			int bi2 = (segi*8 + shiftamt)%(FITRESX*FITRESY*3*8);
			int Bi2 = bi2/8;
			int bi22 = bi2%8;
			int Bi22 = (bi2+HASHBITS-1)/8;
			int bi222 = bi2+HASHBITS-1;

			if(Bi2 >= Bi)
				goto next;

			bool found = false;

			std::vector<SegCase>::iterator sgit2=plc->sc[Bi2].begin();
			while(sgit2!=plc->sc[Bi2].end())
			{
				//if(sgit2->mval == sgit->mval)
				if((sgit2->ivals[0]&~((-1)<<bi22))==(sgit->ivals[1]&((-1)>>(8-bi22))))
				{
				//Log("ner %d fls%d", (int)plc->sc[Bi2].size(), (int)levelsegs);
					found = true;
					break;
				}

				++sgit2;
			}

			if(!found)
			{
				sgit = plc->sc[segi].erase(sgit);
				repeat = true;
				//sgit++;
				//goto over2;
				continue;
			}

			bool found2 = false;

			sgit2=plc->sc[Bi2].begin();
			while(sgit2!=plc->sc[Bi2].end())
			{
				//if(sgit2->mval == sgit->mval)
				if((sgit2->ivals[0]&~((-1)<<bi22))==(sgit->ivals[1]&((-1)>>(8-bi22))))
				{
				//Log("ner %d ffls%d", (int)plc->sc[Bi2].size(), (int)levelsegs);
					found2 = true;
					break;
				}

				++sgit2;
			}

			if(!found2)
			{
				sgit = plc->sc[segi].erase(sgit);
				//Log("nnfer %d ls%d", (int)plc->sc[segi].size(), (int)levelsegs);
				repeat = true;
				//sgit++;
				//goto over2;
				continue;
			}
next:
				//Log("nfer %d ls%d", (int)plc->sc[segi].size(), (int)levelsegs);
			++sgit;
		}

			if(rand()%10001 == 1)
		Log("segi%d", (int)segi);
		segi+=(levelsegs);
	}
	if(repeat)
	{
		segi=Bi%levelsegs;
		repeat = false;
		goto over2;
	}
}

void InitCases(std::vector<int>* vlevelsegs,
			   int level,
			   int y,
			   int ymax,
			   int ymin,
			   std::vector<std::vector<LevelCase>>* levelscases,
			   LoadedTex* outtex)
{
	(*levelscases)[level].push_back(LevelCase());
	LevelCase* plc = &*(*levelscases)[level].rbegin();
	plc->sc.resize(FITRESX*FITRESY*3);
	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		SegCase sc;
		sc.refs = 0;
		sc.up = NULL;
		sc.oval = outtex->data[Bi];
		std::list<Mask> masks;
		Fill(0, 0, 0, HASHBITS-1, masks, 0, sc.oval, 0);
			if(rand()%10001 == 1)
		Log("f%d o%d i%d s%d", (int)masks.size(), (int)sc.oval, (int)0, (int)0);
		HASHINT lastmask = -1;
		masks.sort(CompareMask);

		for(std::list<Mask>::iterator mit=masks.begin();
			mit!=masks.end();
			++mit)
		{
			//new mask list?
			if(mit!=masks.begin() && mit->mask != lastmask)
			{
				//add new
				sc.mval = mit->mask;
				lastmask = mit->mask;
				sc.ivals[0] = mit->ovaldown[0];
				sc.ivals[1] = mit->ovaldown[1];
				plc->sc[Bi].push_back(sc);
			}
			//append to mask?
			else
			{
				sc.ivals[0] = mit->ovaldown[0];
				sc.ivals[1] = mit->ovaldown[1];
				plc->sc[Bi].push_back(sc);
			}

			Narrow(Bi, plc, level, (*vlevelsegs)[level]);
			if(rand()%10001 == 1)
			Log("af n %d", (int)plc->sc[Bi].size());
		}

		if(plc->sc[Bi].size() <= 0)
		{
			//(*levelscases)[level].erase( ((*levelscases)[level].end()--) );
			(*levelscases)[level].clear(  );
			return;
		}
	}
}

#if 0
bool CompareSeg(SegCase& a, SegCase& b)
{
	return (a.mval < b.mval) || (a.mval == b.mval && a.ivals);
}
#endif

bool VerifySplit(int level, LevelCase* lc)
{
	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		if(!lc->sc[Bi].size())
			return false;
	}
	return true;
}

void Clear(SegCase* sgp,
		   std::vector<std::vector<LevelCase>>* levelscases,
		   int level,
		   std::list<SegCase>::iterator sgit)
{
	level++;
	SegCase* usgp;
	while(sgp && level<nlevels)
	{
		usgp = sgp->up;

		//(*levelscases)[level].begin()->sc.erase( (*levelscases)[level].begin() + (sgp - (*levelscases)[level][0]) );

		level++;
		sgp = usgp;
	}
}

void DoCases(std::vector<int>* vlevelsegs,
			   int level,
			   int y,
			   int ymax,
			   int ymin,
			   std::vector<LevelCase>* levelcases,
			   std::vector<std::vector<LevelCase>>* levelscases,
			   LoadedTex* intex,
			   LoadedTex* outtex)
{
	std::vector<int> mival;
	mival.resize(FITRESX*FITRESY*3);

	for(int i=0; i<FITRESX*FITRESY*3; ++i)
		mival[i]=0;

	//std::vector<LevelCase> lcd;
	std::vector<LevelCase>::iterator ulcit = levelcases->begin();
	//LevelCase* ulc = &*levelcases->rbegin();
	if(ulcit == levelcases->end())
		return;
	LevelCase* ulc = &*ulcit;

again:
	
	//
	std::vector<LevelCase>* dlevcases = &(*levelscases)[level];
	dlevcases->push_back(LevelCase());
	LevelCase* lc = &*dlevcases->rbegin();
	lc->sc.resize(FITRESX*FITRESY*3);

	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		//[slot][case]
/*		for(std::vector<SegCase>::iterator sgit=ulc->sc[Bi].begin()+mival[Bi];
			sgit!=ulc->sc[Bi].end() && 
			sgit->mval == ulc->sc[Bi][mival[Bi]].mval && 
			sgit->ivals[0] == ulc->sc[Bi][mival[Bi]].ivals[0] && 
			sgit->ivals[1] == ulc->sc[Bi][mival[Bi]].ivals[1];
			++sgit)
				*/
		for(std::vector<SegCase>::iterator sgit=ulc->sc[Bi].begin();
			sgit!=ulc->sc[Bi].end();
			++sgit)
		{
			std::list<Mask> masks;
			Fill(sgit->ivals[0], sgit->ivals[1], 0, HASHBITS-1, masks, 0, sgit->oval, 0);

			for(std::list<Mask>::iterator mit=masks.begin();
				mit!=masks.end();
				++mit)
			{
				SegCase sc;
				sc.up = &*sgit;
				sc.ivals[0] = mit->ovaldown[0];
				sc.ivals[1] = mit->ovaldown[1];
				sc.mval = mit->mask;
				sc.oval = sgit->ivals[0];
				lc->sc[Bi].push_back(sc);
			}

			//if(!lc->sc[Bi].size())
			//	return;
			Narrow(Bi, lc, level, (*vlevelsegs)[level]);
			if(rand()%10001 == 1)
		Log("af n2 %d", (int)lc->sc[Bi].size());

			if(!masks.size())
			{
				Clear(&*sgit, levelscases, level, sgit);
			}
		}

		//mival[Bi]++;
/*
		if(!lc->sc[Bi].size())
		{
			dlevcases->erase((dlevcases->end()--));
			//goto nextulcit;
			goto advmival;
		}
		*/

		if(!lc->sc[Bi].size())
		{
			dlevcases->erase((dlevcases->end()--));
			goto nextulcit;
			//goto advmival;
		}
	}

	if(!VerifySplit(level, &*dlevcases->rbegin()))
	{
		(*levelscases)[level].erase( ((*levelscases)[level].end()--) );
	}

	goto nextulcit;

	//(*levelscases)[level].sc.push_back(lcd);
/*
advmival:
	
	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		if(ulc->sc[Bi].size() > mival[Bi]+1)
		{
			mival[Bi]++;
			goto again;
		}

		mival[Bi] = 0;
		//[slot][case]
		//for(std::vector<SegCase>::iterator sgit=ulc->sc[Bi].begin()+mival[Bi];
		{
		}
	}
*/
nextulcit:
	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		mival[Bi] = 0;
	}
	ulcit++;
	//LevelCase* ulc = &*levelcases->rbegin();
	if(ulcit == levelcases->end())
		return;
	ulc = &*ulcit;
	goto again;
}

void DoCases(std::vector<int>* vlevelsegs,
			   int level,
			   int y,
			   int ymax,
			   int ymin,
			   std::vector<std::vector<LevelCase>>* levelscases,
			   LoadedTex* intex,
			   LoadedTex* outtex)
{
	for(std::vector<std::vector<LevelCase>>::iterator lcit=levelscases->begin();
		lcit!=levelscases->end();
		++lcit)
	{
		DoCases(vlevelsegs,
			level,
			y,
			ymax,
			ymin,
			&(*levelscases)[level+1],
			levelscases,
			intex,
			outtex);
	}
}

void QuickTeach()
{
	
}



bool SyncMasks(std::vector<LevelCase>::iterator lcit, int Bi, int levelsegs, std::vector<int> mivals)
{
	for(int Bi2=(Bi%levelsegs); Bi2<Bi; Bi2+=levelsegs)
	{
		if((lcit->sc[Bi2].begin()+mivals[Bi2])->mval!=(lcit->sc[Bi].begin()+mivals[Bi])->mval)
			return false;
	}
	return true;
}

bool TryMatch(LoadedTex* ltex, int Bi,
			  std::vector<int>& mivals,
			  std::vector<LevelCase>::iterator& lcit3,
			  LoadedTex* shiftltex, int levelsegs)
{
	//matching set of masks for level segs?
	for(int Bi2=(Bi%levelsegs); Bi2<FITRESX*FITRESY*3; Bi2+=levelsegs)
	{
		if((lcit3->sc[Bi2].begin()+mivals[Bi2])->mval !=
			(lcit3->sc[Bi].begin()+mivals[Bi])->mval)
			return false;

		if((lcit3->sc[Bi2].begin()+mivals[Bi2])->ivals[0] !=
			ltex->data[Bi2])
			return false;
		
		if((lcit3->sc[Bi2].begin()+mivals[Bi2])->ivals[1] !=
			shiftltex->data[Bi2])
			return false;
	}

	return true;
}

bool Upmival(std::vector<int>& mivals,
			  std::vector<LevelCase>::iterator& lcit3,
			  LoadedTex* ltex, LoadedTex* shiftltex)
{
	for(int Bi=0; Bi<FITRESX*FITRESY*3; Bi++)
	{
		if(mivals[Bi]+1 >= lcit3->sc[Bi].size())
		{
			mivals[Bi] = 0;
			continue;
		}

			if(rand()%10001 == 1)
		Log("upm[%d] %d i%d,%d!=%d,%d", (int)Bi,
			(int)mivals[Bi],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[0],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[1],
			(int)ltex->data[0],
			(int)shiftltex->data[0]);

		while(mivals[Bi]+1 < lcit3->sc[Bi].size() &&
			( (lcit3->sc[Bi].begin()+mivals[Bi])->ivals[0] != ltex->data[Bi] ||
			 (lcit3->sc[Bi].begin()+mivals[Bi])->ivals[1] != shiftltex->data[Bi] ) )
		{

			if(rand()%10001 == 1)
		Log("upm1[%d] %d i%d,%d!=%d,%d", (int)Bi,
			(int)mivals[Bi],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[0],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[1],
			(int)ltex->data[0],
			(int)shiftltex->data[0]);
			mivals[Bi]++;
		Log("upm11[%d] %d i%d,%d!=%d,%d", (int)Bi,
			(int)mivals[Bi],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[0],
			(int)(lcit3->sc[Bi].begin()+mivals[Bi])->ivals[1],
			(int)ltex->data[0],
			(int)shiftltex->data[0]);
		}
		
		if(mivals[Bi]+1 >= lcit3->sc[Bi].size())
		{
			//mivals[Bi] = 0;
			//continue;
			return true;
		}

		return true;
	}

	return false;
}

bool TryMatch(std::vector<LevelCase>* lcases,
			  std::vector<LevelCase>::iterator& lcit3,
			  int shiftamt,
			  LoadedTex* shiftltex,
			  LoadedTex* ltex,
			  std::vector<int>& mivals, int levelsegs)
{
	for(int i=0; i<FITRESX*FITRESY*3; ++i)
		mivals[i] = 0;

	for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	{
		if(!TryMatch(ltex, Bi, mivals, lcit3, shiftltex, levelsegs))
		{
			if(Upmival(mivals, lcit3, ltex, shiftltex))
				return false;
			Bi=-1;
			continue;
		}
	}
	return true;
}

bool TryMatch(std::vector<LevelCase>* lcases,
			  int shiftamt,
			  LoadedTex* shiftltex,
			  LoadedTex* ltex,
			  std::vector<int>& mivals, int levelsegs)
{
	std::vector<LevelCase>::iterator lcit3 = lcases->begin();

	while(lcit3 != lcases->end())
	{
		if(TryMatch(lcases, lcit3, shiftamt, shiftltex, ltex, mivals, levelsegs))
			return true;
		lcit3++;
	}

	return false;
}

#endif
void Clear(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex, SegCase* sc)
{
	if(!sc)
		return;

	//Clear(Bi, level+1, levelsegs/2, levelscases,
	//	outtex, ltex, sc->up);

	std::list<SegCase>::iterator sgit;

	for(sgit=(*levelscases)[level].begin()->sc[Bi].begin();
		sgit!=(*levelscases)[level].begin()->sc[Bi].end();
		sgit++)
		if(&*sgit == sc)
		{
			if(sc->up)
			{
				sc->up->refs--;
				if(sc->up->refs <= 0)
					Clear(Bi, level+1, levelsegs/2, levelscases,
						outtex, ltex, sc->up);
			}
			sc->up = NULL;
			sgit = (*levelscases)[level].begin()->sc[Bi].erase(sgit);
			break;
		}
}

void ClearSides(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex, SegCase* sc)
{
	if(!sc)
		return;

	HASHINT mval = sc->mval;

	std::list<SegCase>::iterator sgit;

	for(int Bi2=Bi%levelsegs; Bi2<=Bi; Bi2+=levelsegs)
	{
		for(sgit=(*levelscases)[level].begin()->sc[Bi2].begin();
			sgit!=(*levelscases)[level].begin()->sc[Bi2].end();
			sgit++)
			if(&*sgit == sc)
			//if(sgit->mval == mval)
			{
				//sc->up = NULL;
				//sgit->up
				//sgit = (*levelscases)[level].begin()->sc[Bi2].erase(sgit);
				//break;
				Clear(Bi2, level, levelsegs,
					levelscases, outtex, ltex,
					&*sgit);
				//if(Bi2==Bi)
				break;
			}
	}
}

bool CheckUp(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex, SegCase* sc, int Bi2//, SegCase* sc2
			  )
{
	if(!sc)
		return true;
	//if(sc->mval == 

	std::list<SegCase>::iterator sgit;

	for(sgit=(*levelscases)[level].begin()->sc[Bi2].begin();
		sgit!=(*levelscases)[level].begin()->sc[Bi2].end();
		sgit++)
	{
		if(sgit->mval == sc->mval)
		{
			SegCase* sc2 = &*sgit;
			SegCase* sc3 = sc;

			while(true)
			{
				if(sc2 && !sc3)
					return false;
				if(!sc2 && sc3)
					return false;
				if(!sc2 && !sc3)
					return true;
				if(sc2->mval == sc3->mval)
				{
					sc2 = sc2->up;
					sc3 = sc3->up;
					continue;
				}
				break;
				//return false;
			}
			/*
			if(CheckUp(Bi, level+1, levelsegs/2, levelscases,
				outtex, ltex, sgit->up, Bi2))
				return true;
			*/
			//else
			//	goto clear;
		}
	}

	return false;

clear:
	Clear(Bi, level, levelsegs,
		levelscases, outtex, ltex, &*sgit);

	return false;
}

bool CheckSides(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex, SegCase* sc)
{
	if(!sc)
		return false;

	if(Bi%levelsegs < Bi)
	{
		for(int Bi2=Bi%levelsegs; Bi2<Bi; Bi2++)
		{
			///*
			if(CheckUp(Bi, level, levelsegs,
				levelscases, outtex, ltex,
				sc, Bi2))
				continue;
			//	*/
			ClearSides(Bi, level, levelsegs, levelscases,
				outtex, ltex, sc);
			return false;
		}
	}

	return true;
}

bool CheckShift(int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
				LoadedTex* outtex, LoadedTex* ltex, SegCase* sgp, int Bi)
{
	/*
	Shift(
	int amtB = amt/8;
	int amtb = amt%8;

	char* temp = (char*)malloc(tex->sizex*tex->sizey*3);
	memset(temp, 0, tex->sizex*tex->sizey*3);

	for(int bi=0; bi<tex->sizex*tex->sizey*3*8; ++bi)
	{
		int fromb = bi%8;
		int fromB = bi/8;
		int tob = (bi+amt)%(tex->sizex*tex->sizey*3*8);
		int toB = (tob/8);
		tob = tob%8;

		temp[toB] |= ((((tex->data[fromB])&(1<<fromb))>>fromb)<<tob);
	}
	*/

	if(level == 0)
	{
#if 0
		//for(int Bi2=(Bi%levelsegs); Bi2<Bi; Bi2+=levelsegs)
		{
			if(sgp->ivals[0] != ltex->data[Bi])
			{	
				Clear(Bi, level, levelsegs,
					levelscases, outtex, ltex, sgp);
				return false;
			}

			if((sgp->ivals[1]&1) != ((ltex->data[Bi]&2)>>1))
			{	
				Clear(Bi, level, levelsegs,
					levelscases, outtex, ltex, sgp);
				return false;
			}

			if((sgp->ivals[1]&~1&~(1<<7)) != ((ltex->data[Bi]&~2)>>1))
			{	
				Clear(Bi, level, levelsegs,
					levelscases, outtex, ltex, sgp);
				return false;
			}

			if((sgp->ivals[1]>>7) != ((ltex->data[Bi+1]&1)))
			{	
				Clear(Bi, level, levelsegs,
					levelscases, outtex, ltex, sgp);
				return false;
			}
		}
#else
		LoadedTex shiftltex;
		AllocTex(&shiftltex, FITRESX, FITRESY, 3);
		memcpy(shiftltex.data, ltex->data, FITRESX*FITRESY*3);
		Shift(&shiftltex, level*2+1);

		if(sgp->ivals[0]!=ltex->data[Bi] || sgp->ivals[1]!=shiftltex.data[Bi])
			{	
				//Log("sgp%d,%d!=%d,%d", (int)sgp->ivals[0], (int)sgp->ivals[1],
				//	(int)ltex->data[Bi], (int)shiftltex.data[Bi]);
				Clear(Bi, level, levelsegs,
					levelscases, outtex, ltex, sgp);
				return false;
			}
#endif
	}
	else
	{
	}

	return true;
clear:
	return false;
}

void MidCase(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex);

void LowCase(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex);

void InitCase(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex)
{
	std::list<Mask> masks;
//	Fill(0, 0, 0, HASHBITS-1, masks, 0, outtex->data[Bi], 0);

	if((*levelscases)[level].size() == 0)
		(*levelscases)[level].push_back(LevelCase());
	LevelCase* lc = &*(*levelscases)[level].rbegin();
	lc->sc.resize(FITRESX*FITRESY*3);

	for(std::list<Mask>::iterator mit=masks.begin();
		mit!=masks.end();
		++mit)
	{
		SegCase sc;
		sc.refs = 0;
		sc.up = NULL;
		sc.uplc = NULL;
		sc.lc = lc;
//		sc.ivals[0] = mit->ovaldown[0];
//		sc.ivals[1] = mit->ovaldown[1];
		sc.mval = mit->mask;
		sc.oval = mit->oval;
		lc->sc[Bi].push_back(sc);

		if(!CheckSides(Bi, level, levelsegs, levelscases, outtex, ltex, &*lc->sc[Bi].rbegin()))
		{
			//Log("ci r%d", (int)lc->sc[Bi].size());
			continue;
		}

		if(level-1 > 0)
			MidCase(Bi, level-1, levelsegs*2, levelscases, outtex, ltex);
		else
			LowCase(Bi, level-1, levelsegs*2, levelscases, outtex, ltex);
	}
}

void MidCase(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex)
{
	
	if((*levelscases)[level].size() == 0)
		(*levelscases)[level].push_back(LevelCase());
	LevelCase* lc = &*(*levelscases)[level].rbegin();
	lc->sc.resize(FITRESX*FITRESY*3);

	//for(std::list<SegCase>::iterator sgitup=(*levelscases)[level+1].begin()->sc[Bi].begin();
	//	sgitup!=(*levelscases)[level+1].begin()->sc[Bi].begin();
	//	sgitup++)
					Log("aa rem%d", (int)lc->sc[Bi].size());
	if((*levelscases)[level+1].begin()->sc[Bi].size() == 0)
		return;
	std::list<SegCase>::iterator sgitup=((*levelscases)[level+1].begin()->sc[Bi].end());
	sgitup--;
	//SegCase* sgitup=&*((*levelscases)[level+1].begin()->sc[Bi].rbegin());
	{
		std::list<Mask> masks;

//		Fill(0, 0, 0, HASHBITS-1, masks, 0, sgitup->ivals[0], 0);

		if((*levelscases)[level].size() == 0)
			(*levelscases)[level].push_back(LevelCase());
		LevelCase* lc = &*(*levelscases)[level].rbegin();
		lc->sc.resize(FITRESX*FITRESY*3);

		for(std::list<Mask>::iterator mit=masks.begin();
			mit!=masks.end();
			++mit)
		{
			SegCase sc;
			sc.refs = 0;
			sc.up = &*sgitup;
			sc.uplc = NULL;
			sc.lc = lc;
//			sc.ivals[0] = mit->ovaldown[0];
//			sc.ivals[1] = mit->ovaldown[1];
			sc.mval = mit->mask;
			sc.oval = mit->oval;
			lc->sc[Bi].push_back(sc);

			if(!CheckSides(Bi, level, levelsegs, levelscases, outtex, ltex, &*lc->sc[Bi].rbegin()))
			{
					Log("clears rem%d", (int)lc->sc[Bi].size());
				continue;
			}

			sgitup->refs++;

			if(level-1 > 0)
				MidCase(Bi, level-1, levelsegs*2, levelscases, outtex, ltex);
			else
				LowCase(Bi, level-1, levelsegs*2, levelscases, outtex, ltex);
		}
	}
}

void LowCase(int Bi, int level, int levelsegs, std::vector<std::vector<LevelCase>>* levelscases,
			  LoadedTex* outtex, LoadedTex* ltex)
{

	if((*levelscases)[level].size() == 0)
		(*levelscases)[level].push_back(LevelCase());
	LevelCase* lc = &*(*levelscases)[level].rbegin();
	lc->sc.resize(FITRESX*FITRESY*3);

	//Log("l s%d", (int)(*levelscases)[0].begin()->sc[Bi].size());

	if((*levelscases)[level+1].begin()->sc[Bi].size() == 0)
	{
		//Log("!s");
		return;
	}
	//else if(Bi==0)
	//	Log("lcc");

//Log("lcc122");
	std::list<SegCase>::iterator sgitup=((*levelscases)[level+1].begin()->sc[Bi].end());
	sgitup--;

//Log("lcc12");
	if(sgitup == (*levelscases)[level+1].begin()->sc[Bi].end())
		return;
//Log("lcc122");
	{
		std::list<Mask> masks;

//		Fill(0, 0, 0, HASHBITS-1, masks, 0, sgitup->ivals[0], 0);

		if((*levelscases)[level].size() == 0)
			(*levelscases)[level].push_back(LevelCase());
		LevelCase* lc = &*(*levelscases)[level].rbegin();
		lc->sc.resize(FITRESX*FITRESY*3);

		for(std::list<Mask>::iterator mit=masks.begin();
			mit!=masks.end();
			++mit)
		{
			SegCase sc;
			sc.refs = 0;
			sc.up = &*sgitup;
			sc.uplc = NULL;
			sc.lc = lc;
//			sc.ivals[0] = mit->ovaldown[0];
//			sc.ivals[1] = mit->ovaldown[1];
			sc.mval = mit->mask;
			sc.oval = mit->oval;
			lc->sc[Bi].push_back(sc);
//Log("lcc1");
			if(!CheckSides(Bi, level, levelsegs, levelscases, outtex, ltex, &*lc->sc[Bi].rbegin()))
				continue;
//Log("lcc2");
			SegCase* psc = &*lc->sc[Bi].rbegin();

			//if(psc->ivals[0] != ltex->data[Bi])
			//{
			//	Clear(Bi, level, levelsegs, levelscases, outtex, ltex, psc);
			//	break;
			//}

			//if(Bi>=FITRESX*FITRESY*3-levelsegs)	//last few?
			{
				if(!CheckShift(level, levelsegs,
				levelscases, outtex, ltex, psc, Bi))
				{
			//		Log("clear rem%d", (int)lc->sc[Bi].size());
					//Clear(Bi, level, levelsegs, levelscases, outtex, ltex, psc);
					continue;
				}
			}

		//Log("lcccc");
			sgitup->refs++;
			//int shiftamt = level*2+1;
				//if((sgit2->ivals[0]&~((-1)<<bi22))==(sgit->ivals[1]&((-1)>>(8-bi22))))
			//if((psc->ivals[1]&~((-1)<<shiftamt)) != (((ltex->data[Bi])<<shiftamt)&))
		}
	}
}

void Misbits(HASHINT oval[FITRESX*FITRESY*3],
			 HASHINT oval2[FITRESX*FITRESY*3],
			 HASHINT* misbits,
			int levelsegs)
{
	for(int i=0; i<FITRESX*FITRESY*3; ++i)
	{
		misbits[i] = (oval[i]^oval2[i]);
	}
}

bool Less(HASHINT a[FITRESX*FITRESY*3],
		  HASHINT b[FITRESX*FITRESY*3],
		  int levelsegs)
{
	for(int i=FITRESX*FITRESY*3-1; i>=0; --i)
	{
		if(a[i]<b[i])
			return true;
		else if(a[i]>b[i])
			return false;
	}

	return false;
}

void DownProp(HASHINT* downprop,
			  HASHINT* reqoval,
			  LoadedTex* shifttex,
			  HASHINT* oval,
			  int level)
{

}

void ResetMask(int level, HASHINT* mvals)
{
	for(int B=0; B<FITRESX*FITRESY*3; ++B)
		mvals[level*FITRESX*FITRESY*3+B] = 0;
}

bool IncBits(int level,
			 HASHINT* propdownmisbits, HASHINT* mvals)
{
	//int spread = level;
	//spread<=level
	for(int b=0; b<FITRESX*FITRESY*3*8; ++b)
	{
		int B = b/8;
		int b0 = b%8;

		if((propdownmisbits[B]&b0)&~mvals[level*FITRESX*FITRESY*3+B])
		{
			if(B)
			Log("incB%d:%d->%d", (int)B, (int)(mvals[level*FITRESX*FITRESY*3+B]), (int)(mvals[level*FITRESX*FITRESY*3+B]|b0));
			mvals[level*FITRESX*FITRESY*3+B] |= b0;
			return true;
		}
		else
		{
			mvals[level*FITRESX*FITRESY*3+B] &= ~((1<<b0)-1);
		}
	}
	return false;
}

bool Check(int nlevels,
		   LoadedTex* outtex,
		   LoadedTex* ltex,
		   HASHINT* mvals,
		   LoadedTex* shifttex,
		   HASHINT* oval)
{
	{	
		for(int level2=0; level2<nlevels; level2++)
		{
			//Log("levss%d", (int)level); 
			if(level2==0)
			{
				AllocTex(shifttex, FITRESX, FITRESY, 3);
				memcpy(shifttex->data, ltex->data, FITRESX*FITRESY*3);
				Shift(shifttex, (!level2)?1:level2*2);

				Hash(&oval[level2*FITRESX*FITRESY*3],
					&mvals[level2*FITRESX*FITRESY*3],
					ltex->data,
					shifttex->data,
					1);
			}
			else
			{
				AllocTex(shifttex, FITRESX, FITRESY, 3);
				memcpy(shifttex->data, &oval[level2*FITRESX*FITRESY*3], FITRESX*FITRESY*3);
				Shift(shifttex, (!level2)?1:level2*2);

				Hash(&oval[level2*FITRESX*FITRESY*3],
					&mvals[level2*FITRESX*FITRESY*3],
					&oval[(level2-1)*FITRESX*FITRESY*3],
					shifttex->data,
					1);
			}
		}
	}

	HASHINT h[FITRESX*FITRESY*3] = {0};

	for(int level=0; level<nlevels; ++level)
	{
		HASHINT h2[FITRESX*FITRESY*3] = {0};

		LoadedTex sh;
		AllocTex(&sh, FITRESX, FITRESY, 3);
		if(level==0)
			memcpy(sh.data, ltex->data, FITRESX*FITRESY*3);
		else
			memcpy(sh.data, h, FITRESX*FITRESY*3);
		Shift(&sh, (level)?(level*2):1);

		if(level==0)
			Hash(h2, &mvals[level*FITRESX*FITRESY*3], ltex->data, sh.data, level);
		else
			Hash(h2, &mvals[level*FITRESX*FITRESY*3], h, sh.data, level);

		memcpy(h, h2, FITRESX*FITRESY*3);
	}

	for(int B=0; B<FITRESX*FITRESY*3; ++B)
	{
		if(h[B] != outtex->data[B])
			return false;
	}

	return true;
}

void TeachSeg(LoadedTex* prevltex, LoadedTex* ltex, int fi, LoadedTex* outtex)
{
						printf("start");
						Log("start");
						fflush(stdout);
	int layer = 0;
	int segp = 0;
	int bitwidth = 8;

	int level = 0;
	int levelsegs = FITRESX*FITRESY*3;
	
	std::vector<int> vlevelsegs;
	
	int y;

	//x=1;
	//for(int y=0; y<SEGPARTS; ++y)
	for(y=0; y<SEGIS && levelsegs>0 && !(levelsegs%2); y+=levelsegs, level++, levelsegs>>=1)
	{
		vlevelsegs.push_back(levelsegs);
	}

	Log("levs%d", (int)level); 
	//int уровень = 0;

	y-=levelsegs;
	int nlevels = level;
	--level;

	std::vector<std::vector<LevelCase>> levelscases;	//for each [level], for each [case2]
	levelscases.resize(nlevels);

	HASHINT* mvals = new HASHINT [ FITRESX*FITRESY*3 * nlevels ];
	memset(mvals, 0, sizeof(HASHINT)*FITRESX*FITRESY*3);

	HASHINT* oval = new HASHINT [FITRESX*FITRESY*3*nlevels];
	memset(oval, 0, sizeof(HASHINT)*FITRESX*FITRESY*3);

	HASHINT downprop[FITRESX*FITRESY*3];
	memset(downprop, 0, FITRESX*FITRESY*3);
	
	HASHINT propdownmisbits[FITRESX*FITRESY*3];
	memset(propdownmisbits, 0, FITRESX*FITRESY*3);

	HASHINT reqoval[FITRESX*FITRESY*3];
	memset(reqoval, 0, FITRESX*FITRESY*3);
	
	LoadedTex shifttex;
	AllocTex(&shifttex, FITRESX, FITRESY, 3);
	memset(shifttex.data, 0, FITRESX*FITRESY*3);

	Check(nlevels,
	outtex,
	ltex,
	mvals,
	&shifttex,
	oval);

	Log("ddd");

	int lastlevel = level+1;

	for(; level>=0; level--)
	{
		Log("level%d", level);
		if(level < lastlevel)
		{
			lastlevel = level;
			for(int B=0; B<FITRESX*FITRESY*3; ++B)
			{
				//oval^propdownmisbits=1=need invert
				//reqoval = requirments for value of oval[level] and changeof mval[level]
				if(level==0)
				{
					reqoval[B] = ltex->data[B] ^ propdownmisbits[B];
					memcpy(shifttex.data, ltex->data, FITRESX*FITRESY*3);
				}
				else
				{
					reqoval[B] = oval[(level-1)*FITRESX*FITRESY*3+B] ^ propdownmisbits[B];
					memcpy(shifttex.data, &oval[(level-1)*FITRESX*FITRESY*3], FITRESX*FITRESY*3);
				}
			}
		}

		HASHINT h[FITRESX*FITRESY*3] = {0};
		memset(h, 0, FITRESX*FITRESY*3);
		if(level>0)
			Hash(h, &mvals[(level)*FITRESX*FITRESY*3], &oval[(level-1)*FITRESX*FITRESY*3], shifttex.data, 0);
		else
			memcpy(h, ltex->data, FITRESX*FITRESY*3);

		HASHINT misbits[FITRESX*FITRESY*3] = {0};
		memset(misbits, 0, FITRESX*FITRESY*3);

		bool havemis = false;

		for(int B=0; B<FITRESX*FITRESY*3; ++B)
		{
			char ms1[1234]={0};
			pb(ms1, mvals[level*FITRESX*FITRESY*3+B]);
			Log("mvals[level%d,B%d]=%s", (int)level, (int)B, ms1);
			//Log("h%d,ro%d,m%d", (int)h[B], (int)reqoval[B], (int)mvals[level*FITRESX*FITRESY*3+B]);
			misbits[B] = h[B] ^ reqoval[B];
			///if(misbits[B])
			///	Log("mb%d=%d", (int)B, (int)misbits[B]);
			havemis = havemis || misbits[B];
		}
		
		if(havemis)
		{
			///Log("hm");

			if(level==nlevels-1)
				for(int B=0; B<FITRESX*FITRESY*3; ++B)
				{
					propdownmisbits[B] |= misbits[B];
				}

			//increment at level
			if(IncBits(level, propdownmisbits, mvals))
			{
				Log("inc2");
				//level++;
				level=nlevels;
				memset(reqoval, 0, FITRESX*FITRESY*3);
				goto checkfromtop;
			}
			else
			{
				Log("reset");
				//set mask inverts
				//AddPropBits(mvals, level, 
				ResetMask(level, mvals);

				if(level == 0)
					return;

				goto continuedownprop;
			}
		}
		else	//!havemis
		{
			Log("succ");
			if(Check(nlevels,
				outtex,
				ltex,
				mvals,
				&shifttex,
				oval))	
			{
				Log("succ22");
				return;
			}
			return;
		}
		Log("fail");
		return;
checkfromtop:
		Log("checkfromtop");
		if(Check(nlevels, outtex, ltex, mvals, &shifttex, oval))	
		{
			Log("succ2");
			return;
		}
		//level=nlevels;
continuedownprop:
		Log("continuedownprop");
		Check(nlevels,
			outtex,
			ltex,
			mvals,
			&shifttex,
			oval);
		//if(level==0)
		{
		//	memset(reqoval, 0, FITRESX*FITRESY*3);
		//	level=nlevels;
		}
	}

		Log("2fail");
		//return;
#if 0
down:
	for(; level>=0; level--)
	{
		HASHINT misbits[FITRESX*FITRESY*3];
		HASHINT oval2[FITRESX*FITRESY*3];

		LoadedTex shifttex;
		AllocTex(&shifttex, FITRESX, FITRESY, 3);
		memcpy(shifttex.data, &oval[level*FITRESX*FITRESY*3], sizeof(HASHINT)*FITRESX*FITRESY*3);
		Shift(&shifttex, (level)?(level*2):1);

		Hash(oval2, &mvals[level*FITRESX*FITRESY*3, 
			&oval[level*FITRESX*FITRESY*3],
			&shifttex.data[level*FITRESX*FITRESY*3],
			level);

		Misbits(&oval[level*FITRESX*FITRESY*3], 
			oval2, 
			misbits,
			levelsegs);

		bool have = false;

		memcpy(shifttex.data, misbits, sizeof(HASHINT)*FITRESX*FITRESY*3);
		Shift(&shifttex, FITRESX*FITRESY*3 - ((level)?(level*2):1));

		for(int b=0; b<FITRESX*FITRESY*3; ++b)
		{
			downprop[b] |= misbits[b];
			downprop[b] |= shifttex.data[b];

			if(misbits[b])
			{
				have = true;
		//		break;
			}
		}

		if(have)
		{
			goto tryincup;
		}
	}

	Log("succ");
	return;

tryincup:

	bool upped = false;

	if(upped)
	{
		level=0;
		goto down;
	}
	else
	{
		goto down;
	}

up:

	for(; level<nlevels; level++)
	{
		
	}
#endif
#if 0
	//for(int Bi=0; Bi<FITRESX*FITRESY*3; ++Bi)
	int Bi = 0;
	{
		Log("B%d", Bi);
		//for(levelsegs=vlevelsegs[level]; level>=0 && y>=0; y-=levelsegs, --level, levelsegs=vlevelsegs[level])
		{
			int levelsegs = FITRESX*FITRESY*3 / 2;
			//001110->010010
			HASHINT* misbits =  new HASHINT [levelsegs];

			//for(Bi=0; Bi<FITRESX*FITRESY*3; Bi++)
			{
				misbits[Bi] = -1;
				std::list<Mask> masks;
				Fill(0,0,0, HASHBITS-1, &masks, 0, outtex->data[Bi], 0);
				
				bool f = false;

				for(std::list<Mask>::iterator mit=masks.begin();
					mit!=masks.end();
					++mit)
				{
					HASHINT misbit = mit->oval ^ outtex->data[Bi];

					if(misbit < misbits[Bi])
					{
						f = true;
						misbits[Bi] = misbit;
						mvals[level * FITRESX*FITRESY*3 + Bi] = mit->mask;
					}
				}

				if(!f)
				{
					//reset top, adj below forw
					if(!masks.size())
						exit(0);

					mvals[level * FITRESX*FITRESY*3 + Bi] = masks.begin()->mask;
				}
			}

			delete [] misbits;
		}
		Log("lc[level]sc[%d]sz%d",  Bi, (int)levelscases[level].begin()->sc[Bi].size());
	}
#endif

Log("asd");

	delete [] mvals;
	delete [] oval;
}

int CSegis()
{
	int levelsegs = FITRESX*FITRESY*3;

	int c = levelsegs;

	for(int i=0; i<SEGLEVELS && levelsegs>0 && !(levelsegs%2) && c<MAXSEGIS; ++i)
		c += (levelsegs<<=1);

	return imin(MAXSEGIS,c);
}

int segis = CSegis();

void Shift(LoadedTex* tex, int amt)
{
	int amtB = amt/8;
	int amtb = amt%8;

	char* temp = (char*)malloc(tex->sizex*tex->sizey*3);
	memset(temp, 0, tex->sizex*tex->sizey*3);

	for(int bi=0; bi<tex->sizex*tex->sizey*3*8; ++bi)
	{
		int fromb = bi%8;
		int fromB = bi/8;
		int tob = (bi+amt)%(tex->sizex*tex->sizey*3*8);
		int toB = (tob/8);
		tob = tob%8;

		temp[toB] |= ((((tex->data[fromB])&(1<<fromb))>>fromb)<<tob);
	}
	
	memcpy(tex->data, temp, tex->sizex*tex->sizey*3);
	free(temp);
}

//get output from two prev frames based on masks
void FeedOut(LoadedTex* prevltex, LoadedTex* ltex, int fi, LoadedTex* outtex)
{
#if 0	//not all seg parts are on individual levels
	int segi = 0;
	for(uint32_t seglevel=0; seglevel<SEG1LEVELS*2; ++seglevel)
	//for(uint32_t seglevel=0; seglevel<4; ++seglevel)
	{
		FeedOutSub(prevltex, ltex, fi, outtex, seglevel, &segi);

		//char n[123];
		//sprintf(n, "out/l%d.png", seglevel);
		//SavePNG(n, outtex);
	}
#endif

	LoadedTex midtex;
	AllocTex(&midtex, outtex->sizex, outtex->sizey, outtex->channels);

	memcpy(&outtex->data[0], ltex->data, outtex->sizex*outtex->sizey*outtex->channels/2);
	memcpy(&outtex->data[outtex->sizex*outtex->sizey*outtex->channels/2], prevltex->data, outtex->sizex*outtex->sizey*outtex->channels/2);

	//SEGPARTS	(SEG1INPARTS*2+SEG2INPARTS)
	int segp=0;
	//	char fn2[123];
	//	sprintf(fn2, "out/levelasd%0000006d.png", segp);
	//	SavePNG(fn2, outtex);

	int levelsegs = FITRESX*FITRESY*3;

	LoadedTex next;
	AllocTex(&next, FITRESX, FITRESY, 3);
	memcpy(next.data, ltex->data, ltex->sizex*ltex->sizey*3);

	//for(int level=0; level<SEGLEVELS, levelsegs>=2, segp<SEGIS; ++level, segp+=levelsegs, levelsegs>>=1)//wrong
	for(int level=0; level<SEGLEVELS && levelsegs>0 && !(levelsegs%2) && segp<SEGIS; ++level, segp+=levelsegs, levelsegs>>=1)
	{
		LoadedTex shifttex[2];

		AllocTex(&shifttex[0], FITRESX, FITRESY, 3);
		AllocTex(&shifttex[1], FITRESX, FITRESY, 3);

		memcpy(shifttex[0].data, next.data, FITRESX*FITRESY*3);
		memcpy(shifttex[1].data, next.data, FITRESX*FITRESY*3);

		//Shift(&shifttex[1], level*2+1);
		Shift(&shifttex[1], (level)?(level*2):1);

		LoadedTex next2;
		AllocTex(&next2, FITRESX, FITRESY, 3);

		if(levelsegs <= 0)
			break;


		for(int i=0; i<FITRESX*FITRESY*3; ++i)
		{
			Hash(&next2.data[i], segments[segp+(i%levelsegs)], shifttex[0].data[i], shifttex[1].data[i], 999999);
		}

//	Hash(next2.data, &segments[segp], shifttex[0].data, shifttex[1].data, 999999);
		
		memcpy(next.data, next2.data, FITRESX*FITRESY*3);
		next2.destroy();
	}

	SavePNG2("map.png", &next);

#if 0
	for(; segp<SEG1INPARTS*2; segp+=2*12*HASHLEVELS)
	{
		memcpy(midtex.data, outtex->data, outtex->sizex*outtex->sizey*outtex->channels);

		memset(outtex->data, 0, outtex->sizex*outtex->sizey*outtex->channels);

		printf("o%d/%d,", segp, 2*SEG1INPARTS+SEG2INPARTS);
		fflush(stdout);
		for(int pix=0; pix<outtex->sizex*outtex->sizey/2; ++pix)
		{
			for(int segbit=0; segbit<12; ++segbit)
			{
				//HASHINT *m0 = &segments[HASHLEVELS*(12*2*segp+segbit)];
				//HASHINT *m1 = &segments[HASHLEVELS*(12*2*segp+segbit+12)];
				HASHINT *m0 = &segments[segp+segbit*HASHLEVELS];
				HASHINT *m1 = &segments[segp+segbit*HASHLEVELS+12*HASHLEVELS];

				HASHINT i0a = HASHCROP( (midtex.data[(pix)*3+segbit/4]&(3<<((segbit*2)%8))) >> ((segbit*2)%8) );
				HASHINT i1a = HASHCROP( (midtex.data[(pix)*3+segbit/4+outtex->sizex*outtex->sizey*3/2]&(3<<((segbit*2)%8))) >> ((segbit*2)%8) );

				HASHINT i0 = (i0a&1)|(i1a&2);
				HASHINT i1 = (i1a&1)|(i0a&2);

				HASHINT o0 = Hash(m0, i0, 0, false);
				HASHINT o1 = Hash(m1, i1, 0, false);

				int b0 = ((segbit*2)%8);
				int b1 = ((segbit*2+1)%8);
				int B0 = (segbit/4);
				int B1 = ((segbit)/4)+1;
				int Bs = (outtex->sizex*outtex->sizey*3);

				outtex->data[(pix)*3+B0] |= (o0<<(b0+1));

				//if(segbit % 4 == 3)
				if(b0+1+1>7)
					//outtex->data[((pix)*3+B1)%(Bs)] |= (o0>>(8-b0+1));
					outtex->data[((pix)*3+B1)%(Bs)] |= (o0>>(1));

				outtex->data[((pix)*3+B0 + outtex->sizex*outtex->sizey*3/2)%Bs] |= (o1<<(b0+1));

				//if(segbit % 4 == 3)
				if(b0+1+1>7)
					//outtex->data[((pix)*3+B1 + outtex->sizex*outtex->sizey*3/2)%Bs] |= (o1>>(8-b0+1));
					outtex->data[((pix)*3+B1 + outtex->sizex*outtex->sizey*3/2)%Bs] |= (o1>>(1));
				
				if(segbit == 11)
				{
					outtex->data[((pix)*3+B1)%Bs] |= (o0>>(1));
					outtex->data[((pix)*3+B1 + outtex->sizex*outtex->sizey*3/2)%Bs] |= (o1>>(1));
				}
			}
		}

		//char fn[123];
		//sprintf(fn, "out/f/level%00000016d.png", segp);
		//SavePNG(fn, outtex);
	}

	//segp+=SEG1INPARTS;

	for(; segp-2*SEG1INPARTS<SEG2INPARTS; segp+=1*12*HASHLEVELS)
	{

		printf("o%d/%d,", segp, 2*SEG1INPARTS+SEG2INPARTS);
		fflush(stdout);
		memcpy(midtex.data, outtex->data, outtex->sizex*outtex->sizey*outtex->channels);
		memset(outtex->data, 0, outtex->sizex*outtex->sizey*outtex->channels/2);

		for(int pix=0; pix<outtex->sizex*outtex->sizey/2; ++pix)
		{
			for(int segbit=0; segbit<12; ++segbit)
			{
				//HASHINT *m0 = &segments[HASHLEVELS*(12*2*segp+segbit)];
				//HASHINT *m1 = &segments[HASHLEVELS*(12*2*segp+segbit+12)];
				HASHINT *m0 = &segments[segp+segbit*HASHLEVELS];
				//HASHINT *m1 = &segments[segp+segbit+12];

				HASHINT i0 = HASHCROP( (midtex.data[(pix)*3+segbit/4]&(3<<((segbit*2)%8))) >> ((segbit*2)%8) );
				//HASHINT i1 = HASHCROP( midtex.data[(pix)*3+segbit/3+outtex->sizex*outtex->sizey/2*3]&(3<<(segbit*2)) );

				HASHINT o0 = Hash(m0, i0, 0, false);
				//HASHINT o1 = Hash(m1, i1, 0, false);

				int b0 = ((segbit*2)%8);
				int b1 = ((segbit*2+1)%8);
				int B0 = (segbit/4);
				int B1 = ((segbit)/4)+1;
				int Bs = (outtex->sizex*outtex->sizey*3);

				outtex->data[(pix)*3+B0] |= (o0<<(b0+1));

				//if(segbit % 4 == 3)
				if(b0+1+1>7)
					//outtex->data[((pix)*3+B1)%Bs] |= (o0>>(8-b0+1));
					outtex->data[((pix)*3+B1)%Bs] |= (o0>>1);
				//outtex->data[(pix)*3+segbit/4] |= (o0<<((segbit*2)%8)+1);
				//outtex->data[(pix)*3+segbit/3 + outtex->sizex*outtex->sizey/2*3] |= HASHCROP(o1<<(segbit*2+1));
				
				if(segbit == 11)
				{
					outtex->data[((pix)*3+(segbit)/4+1)%Bs] |= (o0>>(1));
					//outtex->data[((pix)*3+(segbit+1)/3 + outtex->sizex*outtex->sizey/2*3)%(outtex->sizex*outtex->sizey/1*3)] |= HASHCROP(o1>>(1));
				}
			}
		}
		//char fn[123];
		//sprintf(fn, "out/f/level%00000016d.png", segp);
		//SavePNG(fn, outtex);
	}
#endif
}

void TestHash()
{
	//m = new HASHINT [HASHLEVELS];
	//m2 = new HASHINT [HASHLEVELS];
	//	memset(m, 0, HASHLEVELS * sizeof(HASHINT));
	//	memset(m2, 0, HASHLEVELS * sizeof(HASHINT));

#if 1
	uint64_t t = GetTicks();


	srand(GetTicks());
	memset(segments, 0, sizeof(HASHINT)*MAXSEGIS);

	//FillUp();

	//for(int x=0; x<HASHLEVELS; ++x)
	{
		int seglevel = 0;
		int levelsegs = FITRESX*FITRESY*3;
		//x=1;
		//for(int y=0; y<SEGPARTS; ++y)
		for(int y=0; y<SEGIS && levelsegs>0 && !(levelsegs%2); y+=1, seglevel++)
		{
			static int lasty = 0;

			if(y-lasty >= levelsegs)
			{
				lasty=y;
				levelsegs>>=1;
				if(levelsegs <= 0)
					break;
			}

			//x = y%HASHLEVELS;
			//segments[y][x] = (rand()%100000 == 1) ? 3 : 1;
			//segments[y][x] = 3-(rand()%4 + rand()%4)%2;
			segments[y] = (uint32_t)(y*HASHLEVELS)%400;
			//avoid 11 ie 3

			//segments[y][x] = 2-(rand()%4 + rand()%4)%2;

			//segments[y] = 2-(rand()%4 + rand()%4)%2;
#if 0
			{
				std::vector<HASHINT> outs;
				std::vector<HASHINT> ins;
				std::list<Map> map2;

				Map maps[4];

				for(int mi=0; mi<4; ++mi)
				{
					again:
					int i = mi;
					int o = rand()%(1+MAXHASH);
					//o = (i ^ 3 + 2)%(MAXHASH+1);
					if(y/12%3==0)
						o = (i ^ 2 + y)%(MAXHASH+1);

					else if(y/12%3==1)
						o = (i ^ 1 + 3+y)%(MAXHASH+1);

					else
					for(int mi2=0; mi2<mi; ++mi2)
						if(maps[mi2].o == o)
							goto again;

					maps[mi].i = i;
					maps[mi].o = o;

					map2.push_back(maps[mi]);

					outs.push_back(o);
					ins.push_back(i);
				}

				printf("ms%d", (int)map2.size());

				//if(Down4(out, in, m, HASHLEVELS, map.size())
				//Down4(&outs[0], &ins[0], &segments[y], HASHLEVELS, map2.size());
			}
#endif
#if 0

			if(y%12 < 3)
			{
				std::vector<HASHINT> outs;
				std::vector<HASHINT> ins;
				std::list<Map> map2;

				Map m0,m1,m2,m3;
				m0.i = 0;
				m0.o = 1;

				m0.i = 1;
				m0.o = 3;

				m0.i = 3;
				m0.o = 2;

				m0.i = 0;
				m0.o = 1;

				//if(Down4(out, in, m, HASHLEVELS, map.size())
				Down4(&outs[0], &ins[0], segments[y], map2.size());
			}
			else if(y%12 < 6)
			{
			}
			else
			{
			}
#endif

#if 1
			//do
			{
			segments[y] = rand()%(1+MAXHASH);
			//segments[y] = 1;
			//segments[y] = ((1<<0)|(1<<2)|(1<<4)|(1<<6))<<1;
			//	if(y%3 == 0)
			//		segments[y]=-1;
			//	if(y%3==1)
			//		segments[y]=127;
			//	if(y%3==2)
			//		segments[y]=0;
			}
			//while(segments[y] <= 30  || !segments[y]);

			//segments[y] |= (segments[y]<<2);
			//segments[y] = ~(y%(MAXHASH+1));
	
			//segments[y] = (int)sqrt( (double)(y/levelsegs)*(y/levelsegs) + 
			//	(y%levelsegs)*(y%levelsegs) 
				//+ (seglevel)*(seglevel)
				//+ ((y%levelsegs+FITRESX*3/2)%(FITRESX*3))*((y%levelsegs+FITRESX*3/2)%(FITRESX*3))
				//+ ((y%levelsegs)/(FITRESX*3))*((y%levelsegs)/(FITRESX*3))
				//+ ((y%levelsegs)/(FITRESX*3))*((y%levelsegs)/(FITRESX*3))
			//	)%(MAXHASH+1);

			//segments[y] |= ~(1<<(y%8));
			//segments[y] |= ( ~~(1<<((y%levelsegs)%HASHBITS)) /* & ~(1<<(((y<<1)%levelsegs)%HASHBITS)) */ );
#endif
#if 0
			if(y<SEGTHIRD)
				segments[y][x] = 3;

			if(y<SEGTHIRD*2)
				segments[y][x] = 0;

			if(y>=2*SEGTHIRD)
				segments[y][x] = 1;
#endif
#if 0
			if(y%50==0)
				segments[y][x] = 2;
			if(y%50==1)
				segments[y][x] = 2;
			if(y%50==2)
				segments[y][x] = 0;
			if(y%50==3)
				segments[y][x] = 2;
			if(y%50==4)
				segments[y][x] = 1;
#endif
#if 0
			if(time(0)%100==1)
				srand(time(0));

			if(segments[y][x] == 3)
				segments[y][x] = (rand()%100000 == 1) ? 2 : 1;
#endif
		}
		//break;
	}

#if 0
	segments[0][0]=1;
	segments[0][1]=3;
	segments[1][0]=2;
	segments[1][1]=2;
#endif

	//if(g_invideo.size() > 0)
	{
		//char fullpath[DMD_MAX_PATH+1];
		//std::string relpath = MakeRelative(g_invideo.c_str());
		//FullPath(relpath.c_str(), fullpath);
		/////ProcVid("../move.mov");
		//g_mode = APPMODE_PROCVID;

		LoadedTex* intex1 = LoadTexture("in1.png");
		LoadedTex* intex2 = LoadTexture("in2.png");
		LoadedTex* outtex = LoadTexture("out.png");

		LoadedTex intex12;
		LoadedTex intex22;
		LoadedTex outtex2;

		Resample(intex1, &intex12, Vec2i(FITRESX,FITRESY));
		Resample(intex2, &intex22, Vec2i(FITRESX,FITRESY));
		Resample(outtex, &outtex2, Vec2i(FITRESX,FITRESY));

		Log("Asd");

		FeedOut(&intex12, &intex22, 0, &outtex2);
		//TeachSeg(&intex12, &intex22, 0, &outtex2);
	}



	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	Log("%s", ms);
#endif


#if 0
	uint64_t t = GetTicks();

	//for(int times=0; times<10; ++times)
	{

		struct Map
		{
			HASHINT i;
			HASHINT o;
		};

		std::list<Map> map;
		
		uint32_t r = GetTicks();

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
				//add.o = HASHCROP(o * (1024-o) ^ r);

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


		auto mit = map.begin();

		{
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				Log("start %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
		}

		Log("up)");


		Log("up)");

		{
		
			HASHINT* out = new HASHINT[map.size()];
			HASHINT* in = new HASHINT[map.size()];

			mit = map.begin();

			for(HASHINT mci=0; mci<map.size(); ++mci, ++mit)
			{
				out[mci] = mit->o;
				in[mci] = mit->i;
			}

			HASHINT subi = 0;

			FillUp();

			//while(true)
			{
#if 0
				std::vector<HASHINT> subout;
				std::vector<HASHINT> subin;
				subout.push_back(out[(subi+0)%map.size()]);
				subout.push_back(out[(subi+1)%map.size()]);
				//subout.push_back(out[(subi+2)%map.size()]);
				subin.push_back(in[(subi+0)%map.size()]);
				subin.push_back(in[(subi+1)%map.size()]);
				//subin.push_back(in[(subi+2)%map.size()]);
				subi = (subi+2)%map.size();

				Log("subi%d", (int)subi);
				
#endif

				//m[10]=4;
				//if(Down(masks, m, m2, &d, HASHLEVELS-1, in, true, inval, inmask, map.size(), tried, 0, wtried, passin, mp, maps_ovals, maps_ivals, HASHLEVELS-1))
				//if(Down4(&subout[0], &subin[0], m, HASHLEVELS, 2))
				if(Down4(out, in, m, HASHLEVELS, map.size()))
				//if(Try12(m, mit->i, mit->o))
					;//InfoMess("s","s");
				else
				{
					//InfoMess("f","f");
				
					//memcpy(m, m2, sizeof(HASHINT) * HASHLEVELS);

					mit = map.begin();

					for(; mit!=map.end(); mit++)
					{
						Hash(m, mit->i, 0, true);
						HASHINT o = Hash(m, mit->i, 0);
						Log("fail gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
					}
				
				Log("answer:");

					uint64_t p = GetTicks() - t;

					char ms[123];
					sprintf(ms, "fail %llu on %u", p, (int)mit->o);
					Log("%s", ms);

					//delete [] masks;
				
					for(HASHINT min=0; min<map.size(); ++min)
					{
					//	delete [] (maps_ovals[min]);
					}

					//delete [] passin;

					return;
				}

#if 0
				Log("chek");

				//check all
				for(HASHINT mci=0; mci<map.size(); ++mci)
				{
					if(out[mci] != Hash(m, in[mci], 0, true))
						goto goforw;
				}

				break;

goforw:
				;
#endif
			}

#if 1
			auto mlit=map.begin();
			for(HASHINT mci=0; mci<map.size(); ++mci, ++mlit)
			{
				HASHINT o = mlit->i;
				for(HASHINT lev=0; lev<HASHLEVELS; ++lev)
				{
					HASHINT i = o;
					HASHINT s = HASHROTL(i,1);
					o = HASHCROP( s ^ ~(m[lev] & i) );

					Log("lev%dmc%u o%u i%u m%u", (int)lev, (int)mci, (int)o, (int)i, (int)m[lev]);
				}
			}


#endif
			
		//	delete [] mp;

			//delete [] masks;
			
		//	delete [] passin;

			for(HASHINT min=0; min<map.size(); ++min)
			{
				//delete [] (maps_ovals[min]);
			}
			
			//memcpy(m, m2, sizeof(HASHINT) * HASHLEVELS);

			mit = map.begin();
			
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			++mit;
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			
#if 1
			
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				//Log("gives %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
			//break;
			
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				//Log("\t mask[%u] = %llu", (int)x, m[x]);
			}
#endif
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
				Log("\t mask[%u] = %llu", (int)x, m[x]);
			}
	}

	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	Log("%s", ms);
	fflush(stdout);
	//InfoMess(ms,ms);
#endif
}

void UpFill2(HASHINT* mvals, HASHINT* ovals, HASHINT inval, HASHINT level, HASHINT stoplevel)
{
	if(level == stoplevel)
		return;

	HASHINT ovaldown;

	if(level == 0)
	{
		ovaldown = inval;
	}
	else
	{
		ovaldown = ovals[level-1];
	}
	
	HASHINT shift = HASHROTL(ovaldown,1);
	ovals[level] = HASHCROP( shift ^ ~(ovaldown & mvals[level]) );

	UpFill2(mvals, ovals, inval, level+1, stoplevel);
}

//void DownFill(HASHINT* mvals, HASHINT maps_ivals[HASHLEVELS+1][HASHVALS], HASHINT inval, HASHINT level, HASHINT stoplevel, HASHINT mci)
void DownFill(HASHINT* mvals, std::list<HASHINT>* maps_ivals, HASHINT inval, HASHINT level, HASHINT stoplevel, HASHINT mci, HASHINT nmc, HASHINT nlevels)
{
	//maps_ivals[level][mci] = inval;
	//maps_ivals[level*nmc+mci].clear();
	maps_ivals[level*nmc+mci].push_back(inval);

	if(level == stoplevel)
		return;
}

#if 0

bool UniqueMask(Mask a, Mask b)
{
	return (a.ovaldown[0] == b.ovaldown[0] &&
		a.ovaldown[1] == b.ovaldown[1] &&
		a.mask == b.mask);
}

bool UniqueMask2(CaseMask a, CaseMask b)
{
	return (a.ovaldown == b.ovaldown);
}

#if 0
bool CompareMask(Mask& a, Mask& b)
{
	//return ( (a.mask < b.mask) && (a.maskup == b.maskup) ) || ( a.maskup < b.maskup );
	//return ( (a.mask < b.mask) && ((!a.maskupp && !b.maskupp) || (a.maskupp->mask == b.maskupp->mask)) ) || ( a.maskupp && b.maskupp && a.maskupp->mask < b.maskupp->mask );
	//return ( (a.mask < b.mask) && ((a.maskupp->mask == b.maskupp->mask)) ) || ( a.maskupp->mask < b.maskupp->mask );
	return (a.mask < b.mask && a.ovaldown[0] == b.ovaldown[0]) ||
		(a.ovaldown[0] < b.ovaldown[0]);
}

bool CompareMask2(Mask& a, Mask& b)
{
	//return ( (a.mask < b.mask) && (a.maskup == b.maskup) ) || ( a.maskup < b.maskup );
	//return ( (a.mask < b.mask) && ((!a.maskupp && !b.maskupp) || (a.maskupp->mask == b.maskupp->mask)) ) || ( a.maskupp && b.maskupp && a.maskupp->mask < b.maskupp->mask );
	//return ( (a.mask < b.mask) && ((a.maskupp->mask == b.maskupp->mask)) ) || ( a.maskupp->mask < b.maskupp->mask );
	return (a.mask < b.mask);
}
#endif

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
		for(std::list<HASHINT>::iterator uit=invals->begin(); uit!=invals->end(); ++uit)
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

bool CheckFor(HASHINT m, HASHINT mup, std::list<Mask>* c)
{
	for(std::list<Mask>::iterator mit=c->begin(); mit!=c->end(); ++mit)
		//if(mit->mask == m && mit->maskup == mup)
		//if(mit->mask == m && mit->maskupp->mask == mup)
			return true;

	return false;
}

#if 1
bool Narrow(std::list<Mask>* a, std::list<Mask>* b)
{
	std::list<Mask>::iterator ait=a->begin();
	std::list<Mask>::iterator bit=b->begin();

	//Log("narrow %d %d", (int)a->size(), (int)b->size());
	
	//for(auto ait2=a->begin(); ait2!=a->end(); ait2++)
	//	Log("a %d", (int)ait2->mask);
	//for(auto bit2=b->begin(); bit2!=b->end(); bit2++)
	//	Log("b %d", (int)bit2->mask);

	bool ch = false;

	while(true)
	{
		HASHINT c;
		//HASHINT cup;

		//check in order for same ->mask

		//mod 2016/06/22 to include repeats
		if(bit == b->end())
		{
			ch |= (ait != a->end());
			while(ait != a->end())
			{
				ait = a->erase(ait);
			}
			//goto check;
			return ch;
		}
		
		if(ait == a->end())
		{
			ch |= (bit != b->end());
			while(bit != b->end())
			{
				bit = b->erase(bit);
			}
			//goto check;
			return ch;
		}

		//next c is smallest next ->mask of ->a and b

		if(bit->mask < ait->mask)
		{
			c = ait->mask;

			while(bit != b->end() &&
				bit->mask < c)
			{
				bit = b->erase(bit);
				ch = true;
			}
		}
		else
		{
			c = bit->mask;

			while(ait != a->end() &&
				ait->mask < c)
			{
				ait = a->erase(ait);
				ch = true;
			}
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

	return ch;
	//if(a->size() != b->size() &&
	//	(a->size() == 0 || b->size() == 0))
	//	Log("disc");
}
#endif

//oldfill
#if 0
void Fill(HASHINT oval, HASHINT ovaldown, HASHINT iset, HASHADDR fullbin, HASHINT mval, std::list<Mask>* mask, HASHINT mset,
	CaseMask* maskupp, bool check)
{
	HASHADDR bin = fullbin%HASHBITS;
	HASHADDR backbin = (bin+HASHBITS-1)%HASHBITS;

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;
	
	HASHINT shifti = HASHROTL(ovaldown,1);
	HASHINT shiftiset = HASHROTL(iset,1);
	
	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	HASHINT case0 = HASHCROP( ~oval & (~shifti & bm & shiftiset) );	//0?
	HASHINT case1 = HASHCROP( ~oval & (~ovaldown & bm & iset) );	//?0
	HASHINT case2 = HASHCROP( ~oval & (shifti & ovaldown & bm & shiftiset & iset) );	//11
	HASHINT error0 = HASHCROP( ~oval & (~shifti & ~ovaldown & bm & shiftiset & iset) );
	HASHINT error1 = HASHCROP( case0 & mset & ~mval );
	HASHINT error2 = ( case2 & mset & mval );

	HASHINT case3 = ( oval & (shifti & bm & shiftiset) );	//1?
	HASHINT error3 = HASHCROP( case3 & iset & ~ovaldown );
	HASHINT error4 = HASHCROP( case3 & mset & ~mval );
	HASHINT case4 = HASHCROP( oval & (iset & shiftiset & ~shifti & ovaldown & bm ) );	//01
	HASHINT error5 = ( case4 & mset & mval );
	HASHINT case5 = ( oval & (iset & ovaldown & bm) );	//?1
	HASHINT error6 = HASHCROP( oval & (shiftiset & shifti & bm & iset & ~ovaldown) );

	HASHINT case6 = case0 | case2 | case4 | case3 | (iset & ovaldown & bm);	//?1
	HASHINT case7 = case1 | (iset & ~ovaldown & bm);	//?0
	
	HASHINT case8 = case0 | case4 | (shiftiset & ~shifti & bm);	//0?
	HASHINT case9 = ( oval & (iset & shiftiset & ovaldown & shifti & bm) );	//11
	HASHINT case10 = case1 | case9 | case3 | (shiftiset & shifti & bm);	//1?

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

	//greatest first
	//mval &= HASHCROP( ~case11 );
	//mval |= case12;
	//mval |= (~case12 & ~case11);

	if(error0 || error1 || error2 || error3 || error4 || error5 || error6)
	{

		return;
	}

	if((case11 && case12) || (case8 && case10) || (case6 && case7) | (case13 && case14))
	{

		return;
	}

	if(!HASHCROP(~iset) && !HASHCROP(~mset))
	{
		//((mset & mval) < (mset & mlow))

		Mask m;
		m.mask = mval;
		m.ovaldown = ovaldown;
		//m.maskup = maskup;
		m.oval = oval;
		m.maskupp = maskupp;
		//mask->push_back(m);
		mask->push_front(m);

		
							HASHINT ival = ovaldown;
							HASHINT shift = HASHROTL(ival,1);
							HASHINT oval2 = HASHCROP( shift ^ ~(mval & ival) );

							if(oval != oval2)
							{
								Log("i=%d o%d!=expgiv%d m%d c0=%d,1=%d,2=%d,3=%d,4=%d,5=%d,6=%d,7=%d,8=%d,9=%d,10=%d,11=%d,12=%d,13=%d,14=%d bit=%d", (int)ival, (int)oval2, (int)oval, (int)mval,
									(int)case0, (int)case1, (int)case2, (int)case3, (int)case4,
									(int)case5, (int)case6, (int)case7, (int)case8, (int)case9,
									(int)case10, (int)case11, (int)case12, (int)case13, (int)case14,
									(int)bin);
								system("pause");
								exit(0);
							}

		return;
	}

	iset |= bm;
	iset |= bbm;
	mset |= bm;
	
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
	
	//if(nextbin == setbin &&
	//	!check)
	//	return;

#if 1
	for(HASHINT r=0; r<2; ++r)
	{
		for(HASHINT l=0; l<2; ++l)
		{
			for(HASHINT m=0; m<2; ++m)
			{
				Fill(oval, ovaldown, iset, nextbin, mval, mask, mset, maskupp, check);

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
#else

	//greatest firts
	
	for(HASHINT m=0; m<2; ++m)
	{
		if(!case11 && !case12 && !check)
			mval |= (((HASHINT)1)<<bin);

		for(HASHINT l=0; l<2; ++l)
		{
			for(HASHINT r=0; r<2; ++r)
			{
				Fill(oval, ovaldown, iset, nextbin, setbin, mlow, mval, mask, mset, check, in, inmask, high, maskup, lowlim, mtried, mtried2, maskupp);
				
				if(!case6 && !case7 && !check)
					ovaldown ^= (((HASHINT)1)<<bin);
				else
					break;
			}
		
			if(!case8 && !case10 && !check)
				ovaldown ^= (((HASHINT)1)<<backbin);
			else
				break;
		}

		if(!case11 && !case12 && !check)
			mval ^= (((HASHINT)1)<<bin);
		else
			break;
	}

#endif

	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below
}
#endif
//newfill

class UplookM
{
public:
	//HASHINT o;
	//list of inputs accepted by this class instance UplookM (same mask case given output)
	std::list<HASHINT> i;
};


class UplookO
{
public:
	//HASHINT o;
	//based on given mask HASHINT, get UplookM list of inputs
	std::map<HASHINT,UplookM> um;
};

//based on output, get masks' Uplook0 instance
std::map<HASHINT,UplookO> uplooko;

#if 0
bool CompareUplook(Uplook& a, Uplook& b)
{
	return true;
	//return (a.o < b.o);
}
#endif

//std::vector<std::list<TABINT>> tablookupbyoi[MAXHASH];

#if 0
void FillUp()
{
	Log("filling cache");

	HASHINT o = 0;

	//return;

	HASHINT oval = 0;
	HASHINT mval = 0;

	do
	{
		//tablookupbyoi[o].resize(MAXHASH);

		oval = 0;
		do
		{
			std::list<Mask> m;
			Fill(oval, 0, 0, 0, mval, &m, 0);
	
			UplookO u;
			//u.o = oval;

			m.unique(UniqueMask);
			m.sort(CompareMask);

			for(std::list<Mask>::iterator mit=m.begin(); mit!=m.end(); ++mit)
			{
				//u.um.push_back(*mit);
				//UplookM um;
				//um.i.push_back(mit->ovaldown);
				u.um[mit->mask].i.push_back(mit->ovaldown);
				//mask->push_back(*mit);

				////tablookupbyoi[o][mit->ovaldown].push_back(
			}

			uplooko[oval] = u;

			oval++;

		}while(HASHCROP(oval)!=0 && HASHCROP(oval)==oval);

		mval++;

	}while(HASHCROP(mval)!=0 && HASHCROP(mval)==mval);
}
#endif
#if 0
void Fill2(HASHINT oval, HASHINT ovaldown, HASHINT iset, HASHADDR fullbin, HASHINT mval, std::list<Mask>* mask, HASHINT mset)
{
r:
	std::map<HASHINT,UplookO>::iterator uit = uplooko.find(oval);

	if(uit != uplooko.end())
	{
		if(!mset)
		{
			std::map<HASHINT,UplookM>::iterator mit=uit->second.um.begin();
			while(mit!=uit->second.um.end())
			{
				//if((mset&mit->mask)==(mset&mval))
				{
					Mask m;
					m.mask = mit->first;

					for(std::list<HASHINT>::iterator iit=mit->second.i.begin(); iit!=mit->second.i.end(); ++iit)
					{
						m.ovaldown = *iit;
						mask->push_back(m);
					}
				}
				++mit;
			}
		}
#if 1
		else
		{
			std::map<HASHINT,UplookM>::iterator mit=uit->second.um.begin();
			while(mit!=uit->second.um.end())
			{
				//if((mset&mit->mask)==(mset&mval))
				if((mset&mit->first)==(mset&mval))
				{
					Mask m;
					m.mask = mit->first;

					for(std::list<HASHINT>::iterator iit=mit->second.i.begin(); iit!=mit->second.i.end(); ++iit)
					{
						m.ovaldown = *iit;
						mask->push_back(m);
					}
				}
				++mit;
			}
		}
#endif
		return;
	}

#if 0
	std::list<Mask> m;
	Fill(oval, ovaldown, iset, fullbin, mval, &m, 0);
	
	UplookO u;
	//u.o = oval;

	m.unique(UniqueMask);
	m.sort(CompareMask);

	for(auto mit=m.begin(); mit!=m.end(); ++mit)
	{
		//u.um.push_back(*mit);
		//UplookM um;
		//um.i.push_back(mit->ovaldown);
		u.um[mit->mask].i.push_back(mit->ovaldown);
		//mask->push_back(*mit);
	}

	uplooko[oval] = u;
#endif

	goto r;
}
#endif

#if 0
void Fill(HASHINT ival0[FITRESX*FITRESY*3], 
		  HASHINT ival1[FITRESX*FITRESY*3], 
		  HASHINT iset[FITRESX*FITRESY*3], 
		  int fullbin, 
		  std::list<Mask>& masks, 
		  HASHINT mset[FITRESX*FITRESY*3], 
		  HASHINT oval[FITRESX*FITRESY*3], 
		  HASHINT mval[FITRESX*FITRESY*3],
		  int levelsegs)
{
	HASHADDR bin = fullbin%HASHBITS;
	HASHADDR fullbackbin = (fullbin+FITRESX*FITRESY*3-1)%HASHBITS;
	HASHADDR backbin = fullbackbin%HASHBITS;
	HASHADDR backBin = fullbackbin/HASHBITS;
	HASHADDR Bin = fullbin/HASHBITS;


	//if(ival0 == 207)
	//	Log("ii207 b%d", (int)fullbin);

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;

	HASHINT shifti = (ival1[Bin]);
	HASHINT shiftiset = (iset[Bin]);
/*
	HASHINT oval = HASHCROP( ~(mval & ~(inval & shinval)) );
	return oval;

	u	m	i	s
	1	0	0	0
	1	0	0	1
	1	0	1	0
	1	0	1	1
	0	1	0	0
	0	1	0	1
	0	1	1	0
	1	1	1	1
	*/

	HASHINT case0 = ( bm & iset[Bin] & ~ival0[Bin] );
	HASHINT case1 = ( bm & iset[Bin] & ival0[Bin] );

	HASHINT case2 = ( bm & shiftiset & ~shifti );
	HASHINT case3 = ( bm & shiftiset & shifti );

	HASHINT case4 = ( bm & mval[Bin] & mset[Bin] );
	HASHINT case5 = ( bm & ~mval[Bin] & mset[Bin] );

	HASHINT case6 = case4&case3&oval[Bin];//i1
	HASHINT case7 = HASHCROP(case4&case1&~oval[Bin]);//s1

	case1 |= case6;
	case3 |= case7;

	ival0[Bin] |= bm&case1;
	ival1[Bin] |= bm&case3;

	HASHINT h = Hash(ival0, ival1, mval, levelsegs);

	if((h&iset)!=(oval&iset))
		return;

	if((case0&case1)|(case2&case3)|(case4&case5))
	{
		return;
	}

	//if(ival0 == 207)
	//	Log("i207");

	//if(!HASHCROP(~iset) && !HASHCROP(~mset))
	//if((!HASHCROP(~iset) && !HASHCROP(~mset)) && bin == HASHBITS-1 )
	if(fullbin == 0)
	{

		if((h)!=(oval[Bin]))
			return;

		Mask m;
		memcpy(m.mask, mval, sizeof(HASHINT)*levelsegs);
		memcpy(m.ovaldown[0], ival0, sizeof(HASHINT)*levelsegs);
		memcpy(m.ovaldown[1], ival1, sizeof(HASHINT)*levelsegs);
		memcpy(m.oval, oval, sizeof(HASHINT)*levelsegs);
		//m.mask = mval;
		//m.ovaldown[0] = ival0;
		//m.ovaldown[1] = ival1;
		//m.oval = oval;
///		m.maskupp = maskupp;
////		m.oval = oval;
		int osz = masks.size();
		masks.push_back(m);
		masks.unique(UniqueMask);

		if(osz != masks.size() && rand()%10001 == 1)
		Log("a m%d i%d,%d o%d h%d is%d", 
		(int)mval, (int)ival0, (int)ival1, 
		(int)oval, (int)h,
			(int)iset);

		return;
	}
	
	//HASHADDR nextbin = (bin+HASHBITS-!!(bm&iset&shiftiset&mset))%HASHBITS;
	HASHADDR nextbin = (fullbin+HASHBITS-1)%(levelsegs*HASHBITS);

	iset |= bm;
	//iset |= bbm;
	mset |= bm;

	for(HASHINT r=0; r<2; ++r)
	{
		for(HASHINT l=0; l<2; ++l)
		{
			for(HASHINT m=0; m<2; ++m)
			{
				Fill(ival0, ival1, iset, nextbin, masks, mset, oval, mval, levelsegs);

				//if((case4|case5)&bm)
				//	break;
				mval ^= bm;
			}

			//if((case2|case3)&bm)
			//	break;
			ival1 ^= bm;
		}

		//if((case0|case1)&bm)
		//	break;
		ival0 ^= bm;
	}
}
#endif

#if 0
void Fill(HASHINT oval, HASHINT ovaldown, HASHINT iset, HASHADDR fullbin, HASHINT mval, std::list<Mask>* mask, HASHINT mset)
{

#if 0
	if((mset & low) > (mset & mval) ||
		(mset & high) < (mset & mval))
		return;
#endif

	HASHADDR bin = fullbin;
	HASHADDR backbin = (bin+HASHBITS-1)%HASHBITS;

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;

	HASHINT shifti = HASHROTL(ovaldown,1);
	HASHINT shiftiset = HASHROTL(iset,1);

	//levout[outoff1+mci] = HASHCROP( ~( mval[level] & ~(oval & shifti) ) );
	//up1 m0 i0 s0	//	0	2	5	6	11
	//up1 m0 i0 s1	//	0	3	5	7	13
	//up1 m0 i1 s0	//	1	2	5	8	15
	//up1 m0 i1 s1	//	1	3	5	9	17
	//up0 m1 i0 s0	//	0	2	4	6	10
	//up0 m1 i0 s1	//	0	3	4	7	12
	//up0 m1 i1 s0	//	1	2	4	8	14
	//up1 m1 i1 s1	//	1	3	4	9	16

	//i1 = m1&s1&u1
	//s1 = m1&i1&u1

	HASHINT case0 = ( bm & iset & ~ovaldown );
	HASHINT case1 = ( bm & iset & ovaldown );

	HASHINT case2 = ( bm & shiftiset & ~shifti );
	HASHINT case3 = ( bm & shiftiset & shifti );

	HASHINT case4 = ( bm & mval & mset );
	HASHINT case5 = ( bm & ~mval & mset );

	HASHINT case6 = case4&case3&oval;//i1
	HASHINT case7 = case4&case1&oval;//s1

	case1 |= case6;
	case3 |= case7;

	ovaldown |= bm&case1;
	
	if((case0&case1)|(case2&case3)|(case4&case5))
	{
		return;
	}

	//if(!HASHCROP(~iset) && !HASHCROP(~mset))
	if((!HASHCROP(~iset) && !HASHCROP(~mset)) && bin == 0)
	{
		Mask m;
		m.mask = mval;
		m.ovaldown[0] = ival0;
		m.ovaldown[1] = ival1;
///		m.maskupp = maskupp;
////		m.oval = oval;
		mask->push_back(m);

		return;
	}
	
	HASHADDR nextbin = (bin+HASHBITS-!!(bm&iset&shiftiset&mset))%HASHBITS;
	
	iset |= bm;
	//iset |= bbm;
	mset |= bm;

	for(HASHINT r=0; r<2; ++r)
	{
	//	for(HASHINT l=0; l<2; ++l)
	//	{
			for(HASHINT m=0; m<2; ++m)
			{
				Fill(oval, ovaldown, iset, nextbin, mval, mask, mset);

				if((case4|case5)&bm)
					break;
				mval ^= bm;
			}

		//	if((case4|case5)&bm)
		//		break;
		//	ovaldown ^= bbm;
		//}

		if((case0|case1)&bm)
			break;
		ovaldown ^= bm;
	}
}
#endif
#endif

void pb(char* c, HASHINT i)
{
	char mc[HASHBITS+1];
	mc[HASHBITS]=0;

	for(int in=0; in<HASHBITS; ++in, i>>=1)
		//strcat(c, (i%2==0)?"0":"1");
		mc[HASHBITS-in-1] = ((i%2==0)?'0':'1');

	strcat(c,mc);
}

void pb22(char* c, HASHINT i)
{
	char mc[HASHBITS*2+1];
	mc[HASHBITS*2]=0;

	for(int in=0; in<HASHBITS*2; ++in, i>>=1)
		//strcat(c, (i%2==0)?"0":"1");
		mc[HASHBITS*2-in-1] = ((i%2==0)?'0':'1');

	strcat(c,mc);
}

void pb2(char* c, HASHINT t, HASHINT m)
{
	char mc[HASHBITS+1];
	mc[HASHBITS]=0;

	for(int in=0; in<HASHBITS; ++in, t>>=1, m>>=1)
		//strcat(c, (i%2==0)?"0":"1");
		mc[HASHBITS-in-1] = (m%2==1)?('m') : ((t%2==0)?'0':'1');

	strcat(c,mc);
}

#if 0
#if 0

bool Limit(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT mvalnew[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried,
		  HASHADDR downmci,
		  std::list<Mask>* mu,
		  HASHINT upmask,
		  HASHADDR mci,
		  bool original,
		  std::list<Tried>* wtried,
		  std::list<Tried>* mtried2)
{
	if(level == (HASHADDR)-1)
		return true;

	Log("lim lev%u ms%u", (int)level, (int)masksup->size());

	//if(lowlim)
	//	return;

	HASHINT inmask = 0;
	
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);

	if(!level)
	{
		inmask =  HASHCROP((HASHINT)-1);
	}

	HASHINT inval = in[mci];

	if(level==1)
	{
		inval = upin[mci];
		inmask = upinmask[mci];
	}

	HASHINT lowest = 0;
	HASHINT mhighlimit = 1;
	
	std::list<Tried>* tlc = &tried[nmc*level+mci];
	std::list<Tried>* wtlc = &wtried[nmc*level+mci];

	std::list<Mask> masks;
	std::list<Mask>* m = &masks;

	//BinTree tried;
	//HASHINT tried[2] = {0,0};

	auto muit = mu->begin();

//	while(muit!=mu->end() && muit->mask < upmask)
		//muit++;
	/////////	muit = mu->erase(muit);

	mu->unique(UniqueMask);	//only for 1 ch

	auto start = muit;

	bool r = false;

#if 0
	if(!lowlim)
	{
		if(level <= nmc)
		{
			lowest = 0;
			mhighlimit = MAXHASH;
		}
	}
#endif

	while(muit!=mu->end() &&
		(!original || muit->mask == upmask))
	{
		//Log("while(muit!=mu->end())");

		upmask = muit->mask;
		HASHINT oval = muit->ovaldown;
#if 0
		if(DidTry(wtlc, oval))
		{
			return true;
			r = true;
			continue;
		}
#endif

		//Log("lev%u m%u continue mask%u", (int)level, (int)masksup->size(), (int)upmask);

		const HASHADDR setbin = HASHBITS-1;
		const HASHINT ovaldown = 0;
		const HASHINT iset = 0;
		//
		//Log("inmask[mci]%u", (int)inmask[mci]);
		//Log("zeromask[mci]%u", (int)zeromask[mci]);
		//Log("inmask[mci]%u", (int)inmask[0]);
		//Log("zeromask[mci]%u", (int)zeromask[0]);
		bool skipw;

	//////	Fill(oval, ovaldown, iset, setbin, setbin, &lowest, 0, m, 0, true, inval, inmask, &mhighlimit, upmask, false, NULL, mtried2, NULL);

		muit++;

		if(!level && m->size())
			return true;

		//m->sort(CompareMask);
		m->unique(UniqueMask);	//only for 1 ch
		//m->sort(CompareMask);

		//for(HASHINT mci2=0; mci2<mci; ++mci2)
		//	Narrow(m, &masks[mci2]);

		//Mask* mp = new Mask[nmc];

		//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
		if(m->size() && Limit(&masks, mval, mvalnew, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried, mci,
			&masks, 0, mci, false, wtried, mtried2))
		{
			//Log("m from%llu to %llu", submasks.begin()->maskup, submasks.rbegin()->maskup);

			//Log("set mask[%u] = %u", (int)level, (int)mval[level]);
			//*maskup = upmask;

			//Log("success");

#if 0
			//if(!original)
			if(level < HASHLEVELS-1)
			{
				std::list<Tried>* wtlcup = &wtried[nmc*(level+1)+mci];
				AddTried(wtlcup, oval);
			}
#endif

			return true;
			r = true;
		}
		else if(!skipw);
		{
			//for(auto mit=m->begin(); mit!=m->end(); ++mit)
			//	AddTried(tlc, mit->ovaldown);
#if 0
	if(level < HASHLEVELS-1)
	{
			if(!r)
			{
				std::list<Tried>* tlcup = &tried[nmc*(level+1)+mci];
				AddTried(tlcup, oval);
			}
	}
#endif
		}

		m->clear();

		if(muit!=mu->end() &&
			(!original || muit->mask == upmask))
			continue;

		//mval[level] = mhighlimit;

		HASHINT low0 = HASHCROP( LOWEST0( mhighlimit ) );
		mhighlimit |= low0;

		if(low0)
		{
			muit = start;

			//Log("muit = start");

		}
	}

	return r;
}
#endif

		//so try all sat abv and blw and if not    set min sat abv   that is hghr than    last perm     reset perm if    adv'd blw    when adv'd blw   then   start back frm tp     so just reset all abv mval's to 0
		//when adv'd perm blw    from not finding sat abv and blw     so after set min sat abv reset all abv       

//only 0 if adv'd below
//only inc if no match then go up or else if no inc poss then inc blw and go up

//mus tonly 1 mask going up

#if 0

struct Case;

struct Mask
{
	//HASHINT mask;
	HASHINT ovaldown;
	//HASHINT maskup;
	//HASHINT oval;
	Mask* maskupp;
	Case* casp;
};

//all mval's levels match for c ch's
struct Case
{
	HASHINT mask;
	HASHINT refc;
	std::list<Mask>* chmasks;
};
#endif

bool Down4(
	HASHINT* out,
	HASHINT* in,
	HASHINT* mval,
	HASHINT nlevels,
	HASHINT nmc
	)
{
	Log("start");
	HASHINT* levout = new HASHINT[(nlevels+1)*nmc];
	std::list<Case>* levcase = new std::list<Case> [nlevels+1];
	std::list<Case>::iterator upcit;
	for(HASHINT mci=0; mci<nmc; ++mci)
	{
		levout[0*(nmc)+mci] = in[mci];
		//levout[mci*(nlevels+1)+nlevels] = in[mci];
	}
	HASHINT level=0;
	HASHINT outoff0 = (level-1+1)*nmc;
	HASHINT outoff1 = outoff0+nmc;
	HASHINT uplev;
	HASHINT t2=0;
	HASHINT tt=1;
	std::list<Mask>::iterator* cchcit;
	Case* goodcase;
	HASHINT inclev = (HASHINT)-1;	//must same or inc
	for(; level<nlevels; ++level)
	{
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			//HASHINT oval = levin[mci*(nlevels+1)+level-1];
			//HASHINT oval = *oit;
			//HASHINT oval = levout[mci*(nlevels+1)+level-1+1];
			HASHINT oval = levout[outoff0+mci];
			HASHINT shifti = HASHROTL(oval,1);
			//levin[mci*(nlevels+1)+level] = shifti ^ ~(mval[level] & oval);
			//levin[mci*(nlevels+1)+level].push_back( shifti ^ ~(mval[level] & oval) );
			//levout[outoff1+mci] = HASHCROP( shifti ^ ~(mval[level] & oval) );
			levout[outoff1+mci] = HASHCROP( ~( mval[level] & ~(oval & shifti) ) );
		}
		outoff0 = outoff1;
		outoff1 += nmc;
	}
	level = nlevels-1;
	//std::list<Mask>* masks = new std::list<Mask> [(nlevels+1)*nmc];
	//can only adv below if abv already 0 so know in's up
	//dummy
	{
		Case c;
		c.refc = 0;
		c.mask = 0;
		c.chmasks = new std::list<CaseMask> [nmc];
		c.caspp = NULL;
		levcase[nlevels].push_back(c);
	}
	for(HASHINT mci=0; mci<nmc; ++mci)
	{
		//masks[(nlevels+0)*nmc+mci].clear();
		CaseMask m;
///		m.casp = &levcase[nlevels][0];
		m.ovaldown = out[mci];
	////	m.maskupp = NULL;
		//levcase[nlevels][0].chmasks[mci].push_back(m);
		levcase[nlevels].begin()->chmasks[mci].push_back(m);
		//masks[(nlevels+0)*nmc+mci].push_back(m);
	}
	//check success
check:
	outoff0 = (nlevels-1+1)*nmc;
	for(HASHINT mci=0; mci<nmc; ++mci)
		if(levout[outoff0+mci] != out[mci])
			goto advcur;
	goto good;
	uplev;
	t2=0;
	tt=1;
advcur:
	if(level == (HASHINT)-1)
		goto bad;
	//
	uplev = level+1;
	if(!levcase[uplev].size())
		goto bad;
	Log("uplev%d sz%d,%d,%dEX%d", (int)uplev, (int)levcase[uplev].size(), (int)t2, (int)(t2/levcase[uplev].size()/nmc), (int)(t2/(tt?tt:1)));
	 tt=t2;
	t2=0;
	{
		upcit=levcase[uplev].begin();
again:
		while(upcit!=levcase[uplev].end())
		{
			std::list<Case>::iterator upcit2=upcit;
			++upcit2;
			
			//auto n = upcit2._Mynode;
			//auto upcit2=levcase[uplev].begin();

			while(upcit2!=levcase[uplev].end())
			{
				for(HASHINT mci=0; mci<nmc; ++mci)
				{
					if(!upcit2->chmasks[mci].size())
						goto t;
					for(std::list<CaseMask>::iterator cit2=upcit2->chmasks[mci].begin(); cit2!=upcit2->chmasks[mci].end(); ++cit2)
					{
						if(!upcit->chmasks[mci].size())
							goto go;
						for(std::list<CaseMask>::iterator cit=upcit->chmasks[mci].begin(); cit!=upcit->chmasks[mci].end(); ++cit)
						{
							if(cit->ovaldown == cit2->ovaldown)
								goto go2;
						}
						goto go;
go2:
						;
					}
				}
t:

				//if(true)
				{
					int off = std::distance(upcit,upcit2);
					//auto n = upcit._Mynode;
					//auto n2 = upcit2._Mynode;
					//Log("t");
					//Log("trim %d,%d", (int)levcase[uplev].size()-1, (int)uplev);
					upcit2 = levcase[uplev].erase(upcit2);
					//upcit._Ptr = n;
					//upcit = upcit2 - off;
					upcit = upcit2;
					std::advance(upcit,(int)-off);
					//goto again;
					goto go4;
				}
go:
				++upcit2;
go4:
				;
			}
go3:
			++upcit;
		}
	}
	//1,123,21
	//12,231,21
	//=>
	//1,123,21
	//2,231,21
	/////////////////////////////////
	//1,1234,21
	//12,231,21
	//=>
	//1,123,21
	//1,4,21
	//12,231,21
	//=>
	//1,123,21
	//1,4,21
	//2,231,21
					Log("trim %d,%d", (int)levcase[uplev].size(), (int)uplev);
	upcit=levcase[uplev].begin();
	while(upcit!=levcase[uplev].end())
	{
		//++tt;

		std::list<Mask>* masks = new std::list<Mask> [nmc];
		
		//HASHINT high = 1;
		//HASHINT low = 0;

		//while(true)
		{
			//std::list<Mask>* sub = new std::list<Mask> [nmc];
			std::list<Mask>* sub = masks;

#if 1
			{
				HASHINT mci=0;

				for(std::list<CaseMask>::iterator oit=upcit->chmasks[mci].begin(); oit!=upcit->chmasks[mci].end(); ++oit)
#if 0
					Fill(oit->ovaldown, 0, 0, HASHBITS-1, 0, &sub[mci], 0);

#else
				{
///////////					Fill2(oit->ovaldown, 0, 0, HASHBITS-1, 0, &sub[mci], 0);
				}
#endif
			
				//std::sort(masks[mci].begin(),masks[mci].end(),CompareMask);
				//std::unique(masks[mci].begin(),masks[mci].end(),UniqueMask);
				masks[mci].unique(UniqueMask);
				masks[mci].sort(CompareMask);
			}
#endif

			for(HASHINT mci=1; mci<nmc; ++mci)
			//for(HASHINT mci=0; mci<nmc; ++mci)
			{
				for(std::list<CaseMask>::iterator oit=upcit->chmasks[mci].begin(); oit!=upcit->chmasks[mci].end(); ++oit)
				{
#if 0
					
					Fill(oit->ovaldown, 0, 0, HASHBITS-1, 0, &sub[mci], 0);
#else
					//auto mit=sub[0].begin();
					for(std::list<Mask>::iterator mit=sub[0].begin(); mit!=sub[0].end(); ++mit)
					//while(mit!=sub[0].end())
					{
///////////////						Fill2(oit->ovaldown, 0, 0, HASHBITS-1, mit->mask, &sub[mci], MAXHASH);
					}
#endif
				}

				masks[mci].unique(UniqueMask);
				masks[mci].sort(CompareMask);
				//std::sort(masks[mci].begin(),masks[mci].end(),CompareMask);
				//std::unique(masks[mci].begin(),masks[mci].end(),UniqueMask);

				//if(mci)
				for(HASHINT mci2=1; mci2<=mci; ++mci2)
				{
					if(Narrow(&sub[0], &sub[mci2]))
					{
						mci2=0;
					}
				}
			}
		}

		delete [] upcit->chmasks;
		upcit->chmasks=0;

		if(!masks[0].size())
			goto nomask;

		cchcit = new std::list<Mask>::iterator [nmc];

		for(HASHINT mci=0; mci<nmc; ++mci)
			cchcit[mci] = masks[mci].begin();
		
		goodcase = NULL;

		while(true)
		//if(false)
		{
			bool over = false;
			levcase[level].push_back(Case());
			Case* c = &*levcase[level].rbegin();
			c->chmasks = new std::list<CaseMask> [nmc];
			c->caspp = &*upcit;
			
			bool foundc = true;

			HASHINT cmval = cchcit[0]->mask;
			c->mask = cmval;

			for(HASHINT mci=0; mci<nmc; ++mci)
			{
				//c->mask = cchcit[0]->mask;

				bool foundo = false;
				
			//Log("sz mc%d %d", (int)mci, (int)masks[mci].size());

				//up
				while(cchcit[mci] != masks[mci].end())
				{
					CaseMask m2;
////					m2.casp = c;
					m2.ovaldown = cchcit[mci]->ovaldown[0];
////					m2.maskupp = cchcit[mci]->maskupp;

					if(m2.ovaldown == levout[(level-1+1)*nmc+mci] && foundc)
					{
						foundo = true;
					}

					t2++;

					c->chmasks[mci].push_back(m2);
					c->chmasks[mci].unique(UniqueMask2);
					//std::sort(masks[mci].begin(),masks[mci].end(),CompareMask);
					//std::unique(c->chmasks[mci].begin(),c->chmasks[mci].end(),UniqueMask2);

					++cchcit[mci];

					//Log("asd");

					if(cchcit[mci] == masks[mci].end())
					{
						over = true;
						break;
					}
					if(cchcit[mci]->mask > cmval)
					{
						break;
					}
				}

				foundc = foundc && foundo;

				//if(!foundc)
				//	Log("!f mc%d l%d", (int)mci, (int)level);

			}

			if(foundc)
			{
				//
				goodcase = c;
				//HASHINT startl = level;

				while(goodcase && level != nlevels)
				{

/*					//must be greater total
					if((inclev != (HASHINT)-1 && inclev > level) ||
					mval[level] == goodcase->mask ||
					(inclev == (HASHINT)-1 && mval[level] < goodcase->mask))
					{
						if(inclev == (HASHINT)-1 && mval[level] > goodcase->mask)
							inclev = level;
*/
						mval[level] = goodcase->mask;
						goodcase = goodcase->caspp;
						level++;
/*
					}
					else
					{
						level = startl;
						goodcase = NULL;
						goto lesser;
					}
*/
				}

				delete [] cchcit;

				goto good;

			}

lesser:

			if(c->chmasks[0].size())
			{
				upcit->refc++;
				c->refc = 0;
			}
			else
			{
				delete [] c->chmasks;
				levcase[level].pop_back();
			}

			if(over)
				break;
		}

		delete [] cchcit;

nomask:

		delete [] masks;

		//clean up if no chl

		//Log("c");

#if 1
		if(!upcit->refc)
		{
			Case* pcit=upcit->caspp; 
			HASHINT l2 = uplev+1;

			while(pcit)
			{
				pcit->refc--;

				Case* ppcit = pcit->caspp;

				if(!pcit->refc)
				{
					delete [] pcit->chmasks;

					for(std::list<Case>::iterator cit=levcase[l2].begin(); cit!=levcase[l2].end(); ++cit)
						if(&*cit == pcit)
						{
							levcase[l2].erase(cit);
							break;
						}

					l2++;

					pcit = ppcit;
				}
				else
					break;
			}

			upcit = levcase[uplev].erase(upcit);
			
			Log("uplev%d sz%d,%d", (int)uplev, (int)levcase[uplev].size(), (int)t2);
		}
		else
#endif
			++upcit;
	}
	//f
	{
		upcit=levcase[uplev].begin();
		while(upcit!=levcase[uplev].end())
		{
			delete [] upcit->chmasks;
			upcit->chmasks=0;
			upcit++;// = levcase[uplev].erase(upcit);
		}
	}
	--level;
	goto advcur;
good:
	Log("goo");
	for(HASHINT lev=0; lev<=nlevels; ++lev)
	{
		upcit=levcase[lev].begin();
		while(upcit!=levcase[lev].end())
		{
			delete [] upcit->chmasks;
			upcit = levcase[lev].erase(upcit);
		}
	}
	delete [] levout;
	delete [] levcase;
	return true;
bad:
	Log("goob");
	for(HASHINT lev=0; lev<=nlevels; ++lev)
	{
		upcit=levcase[lev].begin();
		while(upcit!=levcase[lev].end())
		{
			delete [] upcit->chmasks;
			upcit = levcase[lev].erase(upcit);
		}
	}
	delete [] levout;
	delete [] levcase;
	return false;
}






		//so try all sat abv and blw and if not    set min sat abv   that is hghr than    last perm     reset perm if    adv'd blw    when adv'd blw   then   start back frm tp     so just reset all abv mval's to 0
		//when adv'd perm blw    from not finding sat abv and blw     so after set min sat abv reset all abv       

#if 0
//only 0 if adv'd below
//only inc if no match then go up or else if no inc poss then inc blw and go up
bool Down2(
	HASHINT* out,
	HASHINT* in,
	HASHINT mval[HASHLEVELS],
	HASHINT nlevels,
	HASHINT nmc
		  )
{
	Log("start");
	//HASHINT* levin = new HASHINT [nmc*(nlevels+1)];
	std::list<HASHINT>* levin = new std::list<HASHINT> [nmc*(nlevels+1)];
	HASHINT* levout = new HASHINT[nmc*(nlevels+1)];
	for(HASHINT mci=0; mci<nmc; ++mci)
	{
		levin[mci*(nlevels+1)+0].push_back(in[mci]);
		levin[mci*(nlevels+1)+nlevels].push_back(out[mci]);
		levout[mci*(nlevels+1)+0] = in[mci];
	}
	Log("a");
	//bool advd = false;
	for(HASHINT level=0; level<nlevels; ++level)
	{
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			levin[mci*(nlevels+1)+level].clear();
			//for(auto oit=levin[mci*(nlevels+1)+level-1].begin(); oit!=levin[mci*(nlevels+1)+level-1].end(); ++oit)
			{
				//HASHINT oval = levin[mci*(nlevels+1)+level-1];
				//HASHINT oval = *oit;
				HASHINT oval = levout[mci*(nlevels+1)+level-1+1];
				HASHINT shifti = HASHROTL(oval,1);
				//levin[mci*(nlevels+1)+level] = shifti ^ ~(mval[level] & oval);
				//levin[mci*(nlevels+1)+level].push_back( shifti ^ ~(mval[level] & oval) );
				levout[mci*(nlevels+1)+level+1] = ( shifti ^ ~(mval[level] & oval) );
			}
		}
	}
	Log("aa");
	for(HASHINT level=nlevels; level<nlevels+1 && level>=nlevels; --level)
	{
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
	Log("aa2");
			for(auto oit=levin[mci*(nlevels+1)+level].begin(); oit!=levin[mci*(nlevels+1)+level].end(); ++oit)
			{
				//HASHINT ovaldown = levin[mci*(nlevels+1)+level-1];
				//HASHINT ovaldown = *oit;
				//HASHINT shifti = HASHROTL(ovaldown,1);
				//HASHINT oval = shifti ^ ~(mval[level] & ovaldown);
				//if(oval != levin[mci*(nlevels+1)+level].)
				//	goto trymatch;
				//for(auto iit2=levin[mci*(nlevels+1)+level].begin(); iit2!=levin[mci*(nlevels+1)+level].end(); ++iit2)
				//	if(*iit2 == oval)
				//		goto nextmc;
				if(*oit == levout[mci*(nlevels+1)+level-1+1])
					goto nextmc;
	Log("aa4");
				goto trymatch;
			}
			goto trymatch;
nextmc:
	Log("aa3");
			;
		}
		delete [] levin;
		delete [] levout;
		Log("win");
		return true;
	}
	Log("aaa");
trymatch:
	Log("b");
	for(HASHINT level=nlevels-1; level!=0 && level<nlevels; --level)
	{
#if 0
		Log("1down leve %d", (int)level);

		//for(HASHINT level2=level+1; level2<nlevels; level2++)
		{
			//reset in
			for(HASHINT mci=0; mci<nmc; ++mci)
			{
				levin[mci*(nlevels+1)+level].clear();
				levin[mci*(nlevels+1)+level+1].clear();
				for(auto oit=levin[mci*(nlevels+1)+level+1].begin(); oit!=levin[mci*(nlevels+1)+level+1].end(); ++oit)
				{
					HASHINT oval = *oit;
					//HASHINT oval = levout[mci*(nlevels+1)+level2-1+1];
					HASHINT shifti = HASHROTL(oval,1);
					levin[mci*(nlevels+1)+level+1].push_back( shifti ^ ~(mval[level] & oval) );
					//levout[mci*(nlevels+1)+level2+1] = shifti ^ ~(mval[level2-1] & oval);
				}
			}
		}
		Log("1down leve %d", (int)level);
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			//for(auto oit=levin[mci*(nlevels+1)+level-1].begin(); oit!=levin[mci*(nlevels+1)+level-1].end(); ++oit)
			{
	Log("bb");
#if 0
				//HASHINT ovaldown = levin[mci*(nlevels+1)+level];
				HASHINT ovaldown = *oit;
				HASHINT shifti = HASHROTL(ovaldown,1);
				HASHINT oval = shifti ^ ~(mval[level-1] & ovaldown);
				//if(oval != levin[mci*(nlevels+1)+level+1])
				//	goto trymatch2;
#endif
				for(auto iit2=levin[mci*(nlevels+1)+level].begin(); iit2!=levin[mci*(nlevels+1)+level].end(); ++iit2)
					if(*iit2 == levout[mci*(nlevels+1)+level-1+1])
						goto nextmc2;
	Log("b2");
				goto trymatch2;
			}
			goto trymatch2;
nextmc2:
			;
	Log("bbb");
		}
	Log("b3");
		continue;
#endif
incup:
trymatch2:
		Log("down leve %d", (int)level);
		//if(mval[level] == (HASHINT)-1)
		//	goto goup;
		//else
		//	++mval[level];	//always +
		std::list<Mask>* masks = new std::list<Mask> [nmc];
		//HASHINT* ovaldowns = new HASHINT[nmc];
		//memset(ovaldowns, 0, sizeof(HASHINT)*nmc);
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			//HASHINT mlowlimit = mval[level];
			//if(mlowlimit != (HASHINT)-1)
			//	++mlowlimit;
			Log("fill leve %d mc %d", (int)level, (int)0);
#if 0
			Fill2(levin, ovaldowns, 0, HASHBITS-1, HASHBITS-1, mval[level],
				mval[level], masks, 0, true, 0, 0,
				(HASHINT)-1, mval[level+1], true, NULL, NULL, NULL, nmc, nlevels, level);
#endif
			for(auto oit=levin[mci*(nlevels+1)+level+1].begin(); oit!=levin[mci*(nlevels+1)+level+1].end(); ++oit)
				Fill(*oit, 0, 0, HASHBITS-1, HASHBITS-1, &mval[level],
					mval[level], &masks[mci], 0, true, 0, 0,
					(HASHINT)-1, mval[level+1], true, NULL,
					NULL, NULL);
			Log("fill leve %d mc %d sz%d", (int)level, (int)0, (int)masks[0].size());
			masks[mci].sort(CompareMask);
		}
		//delete [] ovaldowns;
		//exit(0);
		//if(!masks[0].size() && level == nlevels-1)
			//InfoMess("","");
			//exit(0);
			//	return false;
		//if(!masks[0].size())
		//	goto goup;
		for(HASHINT mci=1; mci<nmc; ++mci)
		{
			Log("narrow leve %d mci %d,%d sz%d,%d", (int)level, (int)mci, (int)0, (int)masks[mci].size(), (int)masks[0].size());
			if(Narrow(&masks[0], &masks[mci]))
				mci = 0;
		}
		if(!masks[0].size())
			goto goup;
		HASHINT* goodmask = NULL;
		//try sat abv and blw
		//find lowest m that sat's od's below for all ch
		//actualy just sat abv by adv smallest amt
		for(HASHINT mci=0; mci<nmc; ++mci)
		{
			//first try to find a low for the ch that is as low as gm but not lower
			HASHINT *chgoodmask = NULL;
			//all m's same but od's dif
			Log("try sat %d mc %d", (int)level, (int)mci);
			for(auto mit=masks[mci].begin(); mit!=masks[mci].end(); ++mit)
			{
				//if(mit->ovaldown != levout[mci*(nlevels+1)+level-1+1])
				//	continue;
				Log("\t try m 's");
				for(auto oit=levin[mci*(nlevels+1)+level].begin(); oit!=levin[mci*(nlevels+1)+level].end(); ++oit)
					if(mit->ovaldown == *oit &&
						(!chgoodmask || mit->mask < *chgoodmask) &&
						(!goodmask || mit->mask >= *goodmask))
					{
						chgoodmask = &mit->mask;
					}
			}
			if(chgoodmask)
			{
				if(goodmask && *chgoodmask > *goodmask)
				{
					goodmask = chgoodmask;
					mci = -1;
					continue;
				}

				goodmask = chgoodmask;
				continue;
			}
			if(!chgoodmask)
			{
				if(!mci)
					goto failmc;
				mci = -1;
			}
		}
		//if(!goodmask)
		//	goto failmc;
goodm:
		Log("goodm leve %d", (int)level);
		mval[level] = *goodmask;
		delete [] masks;
		delete [] levin;
		delete [] levout;
		return true;
failmc:
		//try sat abv
		Log("failmc leve %d", (int)level);
		//try min sat abv
		goodmask = NULL;
		//if(mval[level]
		//++mval[level];
		for(auto mit=masks[0].begin(); mit!=masks[0].end(); ++mit)
		{
			if((!goodmask || mit->mask < *goodmask) &&
				mit->mask > mval[level])
				goodmask = &mit->mask;
		}
		if(goodmask)
		{
			mval[level] = *goodmask;
			
			for(HASHINT mci=0; mci<nmc; ++mci)
			{
				levin[mci*(nlevels+1)+level].clear();
				for(auto mit=masks[mci].begin(); mit!=masks[mci].end(); ++mit)
				{
					if(mit->mask > *goodmask)
						break;
					if(mit->mask < *goodmask)
						continue;
					levin[mci*(nlevels+1)+level].push_back(mit->ovaldown);
				}
			}
		}
		else
		{
			//delete [] masks;
			//no sat abv?
			//goto incup;
			Log("u");
			goto goup;
		}
		//delete [] masks;
goodm2:
		Log("goodm2 leve %d", (int)level);
		//sat'd abv at lev
		for(HASHINT level2=level+1; level2<nlevels; level2++)
		{
			//reset mvals
			mval[level2] = 0;
			//reset in
			for(HASHINT mci=0; mci<nmc; ++mci)
			{
				levin[mci*(nlevels+1)+level2].clear();
				//for(auto oit=levin[mci*(nlevels+1)+level2-1].begin(); oit!=levin[mci*(nlevels+1)+level2-1].end(); ++oit)
				{
					//HASHINT oval = *oit;
					HASHINT oval = levout[mci*(nlevels+1)+level2-1+1];
					HASHINT shifti = HASHROTL(oval,1);
					//levin[mci*(nlevels+1)+level2].push_back( shifti ^ ~(mval[level2-1] & oval) );
					levout[mci*(nlevels+1)+level2+1] = shifti ^ ~(mval[level2-1] & oval);
				}
			}
		}
		goto goup;
		delete [] masks;

		Log("godowN");
		//if(
		continue;
goup:
		delete [] masks;
		level = nlevels+1;
	}
	delete [] levin;
	delete [] levout;
	return false;
}
#endif

#if 0
//propagate down to gotten input
bool Down(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT mvalnew[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried,
		  HASHADDR downmci,
		  std::list<Tried>* wtried,
		  HASHINT* uppassin,
		  Mask** upmp,
		HASHINT maps_ovals[HASHLEVELS][HASHVALS],
		//std::list<HASHINT> maps_ivals[HASHLEVELS+1][HASHVALS],
		std::list<HASHINT>* maps_ivals,
		HASHINT lowlevel)
{
	Log("lev%u m%u mask%u to%u lowlim%u od%llu", (int)level, (int)masksup->size(), (int)masksup->begin()->mask, (int)masksup->rbegin()->mask, (int)lowlim, masksup->begin()->ovaldown);

	//if(masksup->size() != masksup[1].size())
	//	Log("disc");

	if(level == (HASHADDR)-1)
	{
		//*maskup = masksup[0].begin()->mask;

		//upmp[0] = &*(masksup[0].begin());

		//for(HASHINT mci=0; mci<nmc; ++mci)
		//	upmp[mci] = &*(masksup[mci].begin());

		return true;
	}

	//if(*maskup < mvalnew[level] &&
	//	lowlim)
	//	exit(0);

	//mvalnew[level] = *maskup;

	HASHINT mset = 0;
	HASHINT oval = 0;

	//if(!lowlim)
	//	mval[level] = 0;

	HASHADDR upbin=HASHBITS-1;

	HASHINT* zeromask = new HASHINT[nmc];
	memset(zeromask, 0, sizeof(HASHINT)*nmc);

	HASHINT* inmask = zeromask;

	if(!level)
	{
		//inmask = (HASHINT)-1;
		memset(zeromask, -1, sizeof(HASHINT)*nmc);
	}

	HASHINT* inval = in;
	HASHINT maxhash = MAXHASH;

#if 0
	if(level==1)
	{
		inval = upin;
		inmask = upinmask;
	}
	else if(level > 1)
#else
	if(level > 0)
#endif
	{
		inmask = &maxhash;
		inval = maps_ovals[level-1];
	}

	HASHINT mhighlimit = mval[level];
	HASHINT lowest = mval[level];

	if(!mhighlimit)
		mhighlimit = 1;

	std::list<Mask>* masks = new std::list<Mask>[nmc];
	HASHINT upmask = *maskup;
	//HASHINT upmask = 0;
	HASHINT mci = 0;
	
	std::list<Tried> mtried;
	std::list<Tried> mtried2;

	while(mci<nmc)
	{
		//mtried2.clear();

skipmc:

		if(!mci)mtried.clear();
		//Log("skipmc");

		std::list<Mask>* mu = &masksup[mci];
		std::list<Mask>* m = &masks[mci];

		auto muit = mu->begin();

nextmu:

		while(muit!=mu->end() &&
			muit->mask == upmask)
		{
			//Log("while(muit!=mu->end())");

			//upmask = muit->mask;
			HASHINT oval = muit->ovaldown;

			const HASHADDR setbin=upbin;
			HASHINT ovaldown = 0;
			HASHINT iset = 0;
			
			Log("fill s mci%u sz%u lev%u musz%u  lowest%u mhighlimit%u", (int)mci, (int)m->size(), (int)level, (int)mu->size(), (int)lowest, (int)mhighlimit);

#if 1
			//if(!mci)
				Fill(oval, ovaldown, iset, setbin, setbin, lowest, 0, m, mset, true, inval[mci], inmask[mci], mhighlimit, upmask, lowlim, NULL, &mtried2, &*muit);
			//else
			{
				//for(auto m0it=masks[0].begin(); m0it!=masks[0].end(); ++m0it)
				//	Fill2(oval, &*muit, inval[mci], inmask[mci], m0it->mask, m);
			//	Fill(oval, ovaldown, iset, setbin, setbin, lowest, 0, m, mset, true, inval[mci], inmask[mci], mhighlimit, upmask, lowlim, &mtried, &mtried2, &*muit);
			}
#endif

			Log("fill end mci%u sz%u lev%u musz%u  lowest%u mhighlimit%u", (int)mci, (int)m->size(), (int)level, (int)mu->size(), (int)lowest, (int)mhighlimit);
			
			muit++;
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
		//o fr bl
		//m i fr a
		//prsv u or d?
		//aaa baa caa aba bba cba aca ...
		//aaa baa aba aab bab abb aac ...
		//
		//pv bt prob   aaa baa caa aba bba *bca   i of 1:a->b chng's so ms up 2:b->c    how far?    try all lev combos till 1.) get top's i and 2.) get below's o     if not 2.) then get lowest 1.) and do same for below
		//then when up 1: again? at min ms get all i's correct from tp->down    oh     wave goes down to end bottom then passes thru and leads to perm incr in top then wave starts again    
		//aaa baa caa   aba repeats every b-w layers
		//!!!VVV
//check for co,mbo that stasifes both belw and abv  and if not set lowest sat abv and go to blw    try all sat abv and blw there and if not, then at least set min*?(or abv? or max of both) sat blw  and start from tp again where 
//1.) can inc combo to try sat botm if not already  or and don't go below combo that was alrdy there bfr went back up *? (that is prsv top not bot lowlevel)  and if not sat btm  then go blw once 
		//if can't sat abv there
//only start from lowest abv again if combo below perm advncd forw
		//mpore choicesg go up->down or down->up?
		//increases incrementally exponentially as go up->down
		//but only one always down->up
		//how sat i?  get all sat top's i and check if sat i?    can only limit by i     all m will accept an i   but give dif o/intermdt     msk sat's min blw if takes i given from blw  that is full match then
		//what if no such mask at tp that all mci's sats top abv?  then impossible?     fi can chs any i then always poss     
		//can only inc abv if blw inc'd    then af inc don't chg combo to any prev bef inc blw       as soon as chg abv then all blw fail       so?        
		//get all common bw/ mci's that sat abv   then go dwn   and   same    until   find all match blw i     prfr all lwst but prfbly lwst  bt   but at least as big as val frm blw   prsv bt
		//u
		//use "lowlevel"    try all sat abv and blw  if not   set min sat abv   adv lowlevel down  and   there  try all sat abv and blw   if not  set min sat abv   and   go tp   adv by 1 perm until expand lowlevel
		//and     go dwn    and    new set of combos that sat abv     lowlevel goes back up   how do we know   never going back    
		//if already sat abv   then adv
		//then wave dwn  and if botm   back to tp
		//always min sat tp   except if    alrdy sat tp    nvr know if   higher combo abv won't save lots adv's blw     always know that    min to sat tp    can say that     nothing blw those combos will    sat tp
		//already know nothing hghr combo abv will sat both abv and blw    until adv'd blw    
		//so when perm adv'd blw   try all combos abv too     but dif combo hghr abv might have dif i   and be sat by   little hghr val blw    so
		//when perm adv'd abv     go lv dwn and    adv'ing combo    try sat abv    and sat min sat abv   that is also   hghr than prev perm    so nothing blw perm blw    will    sat    abv?
		//

		//so try all sat abv and blw and if not    set min sat abv   that is hghr than    last perm     reset perm if    adv'd blw    when adv'd blw   then   start back frm tp     so just reset all abv mval's to 0
		//when adv'd perm blw    from not finding sat abv and blw     so after set min sat abv reset all abv       




		if(!m->size())
		{
			if(level >= lowlevel)
			{
				goto up;
			}
		}

		{
			m->sort(CompareMask);

			if(mci && Narrow(m, &masks[0]))
			//if(mci && NarrowHalf(&masks[0], m))
			{
				for(HASHINT mci2=1; mci2<=mci; ++mci2)
				{
					//Log("n%u,%u  sz[%u],[%u]",(int) mci, (int) mci2, (int)masks[mci].size(), (int)masks[mci2].size());

					//if(Narrow(m, &masks[mci2]))
					Narrow(m, &masks[0]);
					//NarrowHalf(m, &masks[0]);
				}
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

			if(mci == nmc-1)
			{

				//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
				//if(m->size() && Down(masks, mval, mvalnew, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried, mci, wtried, uppassin, mp))
				if(m->size())
				//if(submasks->size() && Down(masks, mval, mvalnew, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried, mci, wtried, passin, mp))
				{

					mval[level] = m->begin()->mask;

					return true;
				}
				
up:

				//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
				//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
				//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
				//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
				//up1 m0 i[#-1]0 i[#]0 if left 1 right also
				//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
				//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
				//up1 m1 i[#-1]1 i[#]1if right 0 left
				//then justget o by m below

				//if(gotupo)
				{
					for(HASHINT mci=0; mci<nmc; ++mci)
					{
						masks[mci].clear();
					}

					mval[level] = 0;

					for(HASHINT mci=0; mci<nmc; ++mci)
					{
						Fill(maps_ivals[level+1][mci], maps_ovals[level-1][mci], 0, HASHBITS-1,
							HASHBITS-1, 0, mval[level], &masks[mci], 0, true, 0, 0,
							MAXHASH, mval[level+1], false, NULL, NULL, &*masksup[mci].begin());

						masks[mci].sort(CompareMask);

						if(mci)
							Narrow(&masks[0], &masks[mci]);
					}

					for(HASHINT mci=1; mci<nmc; ++mci)
					{
						Narrow(&masks[0], &masks[mci]);
					}

					if(!masks[0].size())
					{
					}
					else
					{
						mval[level] = masks[0].begin()->mask;

						while(true)
						{
							bool met = true;
next:

							for(HASHINT mci=0; mci<nmc; ++mci)
							{
								maps_ivals[level*nmc+mci].clear();

								bool submet = false;

								for(auto mit=masks[mci].begin(); mit!=masks[mci].end() && mit->mask==mval[level]; ++mit)
								{
									DownFill(mval, maps_ivals, mit->ovaldown, level, level, mci, nmc, HASHLEVELS+1);

									if(maps_ovals[mci][level-1] == mit->ovaldown)
										submet = true;
								}

								met = met & submet;
							}

							if(met)
								return true;

							return false;
						}

#if 0
							if(Down(masks, mval, mvalnew,
								&mval[level], level-1, in, false, upin, upinmask, nmc,
								tried, downmci, wtried, uppassin, mp, 
								maps_ovals, maps_ivals, lowlevel))
							{
								return true;
							}


							for(auto mit=masks[mci].begin(); mit!=masks[mci].end(); ++mit)
								if(mit->mask > mval[level])
								{
									mval[level] = mit->mask;
									goto next;
								}
#endif
						}
					}

					return false;
				}

				//if(!m->size())
			//		Log("!m->size()");
				//else
				//	Log("!Down");

				//delete [] passin;
				//delete [] mp;
				delete [] masks;
				delete [] zeromask;

				return false;
			}
		}
nextmc:
		++mci;
		//Log("nextmc");
	}

fail:

	delete [] masks;
	delete [] zeromask;

	return false;
}
#endif

HASHINT Hash(HASHINT* mval, HASHINT oval, HASHINT oval2, HASHADDR level, bool echo)
{
	//if(level >= HASHLEVELS)
	//	return oval;

	const HASHINT ovalup = HASHCROP( oval2 ^ ~(oval & *mval) );

	if(echo)
		Log("level%u m%u up%u o%u os%u", (int)level, (int)*mval, (int)ovalup, (int)oval, (int)oval2);

	return ovalup;
	//return Hash(mval, ovalup, level+1, echo);
}

#endif

void Hash(HASHINT* ovalup, HASHINT mval, HASHINT oval, HASHINT oval2, HASHADDR level)
{
	//if(level >= HASHLEVELS)
	//	return oval;

	//for(int b=0; b<FITRESX*FITRESY*3; ++b)
	{
		*ovalup = HASHCROP( ~(oval2 & ~(oval & mval)) );

//		if(echo)
//			Log("level%u m%u up%u o%u os%u", (int)level, (int)*mval, (int)ovalup, (int)oval, (int)oval2);
	}

	//return ovalup;
	//return Hash(mval, ovalup, level+1, echo);
}

void Hash(HASHINT* ovalup, HASHINT* mval, HASHINT* oval, HASHINT* oval2, HASHADDR level)
{
	//if(level >= HASHLEVELS)
	//	return oval;

	for(int b=0; b<FITRESX*FITRESY*3; ++b)
	{
		ovalup[b] = HASHCROP( ~(oval2[b] & ~(oval[b] & mval[b])) );

//		if(echo)
//			Log("level%u m%u up%u o%u os%u", (int)level, (int)*mval, (int)ovalup, (int)oval, (int)oval2);
	}

	//return ovalup;
	//return Hash(mval, ovalup, level+1, echo);
}
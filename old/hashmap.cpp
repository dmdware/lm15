

#include "hashmap.h"
#include "../path/pathnode.h"
#include "../utils.h"
#include "../debug.h"
#include "../sim/simflow.h"


HashMap::HashMap()
{
	//memset(values, 0, sizeof(HashVal*) * MAXHASH);
	
    // Initialize regular cells
    m_arraySize = 8;
    //assert((m_arraySize & (m_arraySize - 1)) == 0);   // Must be a power of 2
    m_cells = new void*[m_arraySize];
    memset(m_cells, 0, sizeof(void*) * m_arraySize);
    m_population = 0;

    // Initialize zero cell
    m_zeroUsed = 0;
#if 0
    m_zeroCell.key = 0;
    m_zeroCell.value = 0;
#endif
}

HashMap::~HashMap()
{
	free();
}

void** HashMap::getcell(HashKey key)
{
#if 0
	uint16_t i = key % MAXHASH;

	HashVal* hv = values[i];
	//int32_t li = 0;

	while(hv)
	{
		//++li;

		//todo avoid using hard coded use of PathNode and extraneous key var
		//try returning whole HashVal with key var and sending HashVal's to BinHeap
#ifdef HASHPOOL
		if(((PathNode*)(hv->value))->index == key)
#endif
		{
			//Log("sf%d getf %d\r\n", (int32_t)g_simframe, li);
			return hv->value;
		}

		hv = hv->next;
	}
	
	//Log("sf%d getn %d\r\n", (int32_t)g_simframe, li);

	return NULL;
#endif

    if (key)
    {
        // Check regular cells
        for (void** cell = FIRST_CELL(integerHash(key));; cell = CIRCULAR_NEXT(cell))
        {
			if(!*cell)
				//continue;
                return NULL;
            if (HASHKEY(*cell) == key)
                return cell;
            //if (!HASHKEY(*cell))
            //if (HASHKEY(*cell) == (HashKey)-1)
            //    return NULL;
        }
    }
    else
    {
        // Check zero cell
        if (m_zeroUsed)
            return &m_zeroCell;
        return NULL;
    }
}


void* HashMap::get(HashKey key)
{
#if 0
	uint16_t i = key % MAXHASH;

	HashVal* hv = values[i];
	//int32_t li = 0;

	while(hv)
	{
		//++li;

		//todo avoid using hard coded use of PathNode and extraneous key var
		//try returning whole HashVal with key var and sending HashVal's to BinHeap
#ifdef HASHPOOL
		if(((PathNode*)(hv->value))->index == key)
#endif
		{
			//Log("sf%d getf %d\r\n", (int32_t)g_simframe, li);
			return hv->value;
		}

		hv = hv->next;
	}
	
	//Log("sf%d getn %d\r\n", (int32_t)g_simframe, li);

	return NULL;
#endif

    if (key)
    {
        // Check regular cells
        for (void** cell = FIRST_CELL(integerHash(key));; cell = CIRCULAR_NEXT(cell))
        {
			if(!*cell)
				//continue;
                return NULL;
            if (HASHKEY(*cell) == key)
                return *cell;
            //if (!HASHKEY(*cell))
            //if (HASHKEY(*cell) == (HashKey)-1)
            //    return NULL;
        }
    }
    else
    {
        // Check zero cell
        if (m_zeroUsed)
            return m_zeroCell;
        return NULL;
    }
}

void HashMap::add(HashKey key, void* value)
{
#if 0
	uint16_t i = key % MAXHASH;

	HashVal** hv = &values[i];
	//int32_t li = 0;

	while(*hv)
	{
		//++li;
		hv = &(*hv)->next;
	}

	//Log("sf%d add %d\r\n", (int32_t)g_simframe, li);

	(*hv) = new HashVal;
	(*hv)->next = NULL;
	(*hv)->value = value;
#endif

#if 1
    if (key)
    {
        // Check regular cells
        for (;;)
        {
            for (void** cell = FIRST_CELL(integerHash(key));; cell = CIRCULAR_NEXT(cell))
            {
                //if (HASHKEY(*cell) == (HashKey)0)
               // if (HASHKEY(*cell) == (HashKey)-1)
				if(!*cell)
                {
                    // Insert here
                    if ((m_population + 1) * 4 >= m_arraySize * 3)
                    {
                        // Time to resize
                        Repopulate(m_arraySize * 2);
                        break;
                    }
                    ++m_population;
                    //cell->key = key;
					*cell = value;
                    //return *cell;
                }
				
                if (HASHKEY(*cell) == key)
                    //return *cell;        // Found
					return;
            }
        }
    }
    else
    {
        // Check zero cell
        if (!m_zeroUsed)
        {
            // Insert here
            m_zeroUsed = true;
            if (++m_population * 4 >= m_arraySize * 3)
			{
				// Even though we didn't use a regular slot, let's keep the sizing rules consistent
                Repopulate(m_arraySize * 2);
			}
        }
        //return &m_zeroCell;
		m_zeroCell = value;
    }
#endif
}

void HashMap::free()
{
#if 0
	for(uint16_t i=0; i<MAXHASH; ++i)
	{
		//todo get rid of if statement for NULL check
		if(values[i])
		{
			delete values[i];
			values[i] = NULL;
		}
	}
#else
	
    // (Does not resize the array)
    // Clear regular cells
    memset(m_cells, 0, sizeof(void*) * m_arraySize);
    m_population = 0;
    // Clear zero cell
    m_zeroUsed = false;
    m_zeroCell = NULL;

	delete [] m_cells;
    m_arraySize = 8;
    //assert((m_arraySize & (m_arraySize - 1)) == 0);   // Must be a power of 2
    m_cells = new void*[m_arraySize];
    memset(m_cells, 0, sizeof(void*) * m_arraySize);
#endif
}

void HashMap::Delete(void** cell)
{
    if (cell != &m_zeroCell)
    {
        // Delete from regular cells
        //assert(cell >= m_cells && cell - m_cells < m_arraySize);
        //assert(cell->key);

        // Remove this cell by shuffling neighboring cells so there are no gaps in anyone's probe chain
        for (void** neighbor = CIRCULAR_NEXT(cell);; neighbor = CIRCULAR_NEXT(neighbor))
        {
            //if (!neighbor->key)
			//if(!HASHKEY(*neighbor))
			if(!*neighbor)
            {
                // There's nobody to swap with. Go ahead and clear this cell, then return
                //cell->key = 0;
                //cell->value = 0;
				*cell = NULL;
                m_population--;
                return;
            }
			//todo check does this continue shuffling back unnecessary cells with different keys?
            void** ideal = FIRST_CELL(integerHash( HASHKEY(*neighbor) ));
            if (CIRCULAR_OFFSET(ideal, cell) < CIRCULAR_OFFSET(ideal, neighbor))
            {
                // Swap with neighbor, then make neighbor the new cell to remove.
                //*cell = *neighbor;
                //cell = neighbor;
				//**cell = **neighbor;
				*cell = *neighbor;
				cell = neighbor;
            }
        }
    }
    else
    {
        // Delete zero cell
        //assert(m_zeroUsed);
        m_zeroUsed = false;
        //cell->value = 0;
		*cell = NULL;
        m_population--;
        return;
    }
}

void HashMap::Compact()
{
    Repopulate(upper_power_of_two((m_population * 4 + 3) / 3));
}

void HashMap::Repopulate(size_t desiredSize)
{
    //assert((desiredSize & (desiredSize - 1)) == 0);   // Must be a power of 2
    //assert(m_population * 4  <= desiredSize * 3);

    // Get start/end pointers of old array
    void** oldCells = m_cells;
    void** end = m_cells + m_arraySize;

    // Allocate new array
    m_arraySize = desiredSize;
    m_cells = new void*[m_arraySize];
    memset(m_cells, 0, sizeof(void*) * m_arraySize);

    // Iterate through old array
    for (void** c = oldCells; c != end; c++)
    {
		if(!*c)
			continue;

        //if (HASHKEY(*c))
        {
            // Insert this element into new array
            for (void** cell = FIRST_CELL(integerHash( HASHKEY(*c) ));; cell = CIRCULAR_NEXT(cell))
            {
                //if (!HASHKEY(*cell))
				if(!*cell)
                {
                    // Insert here
                    *cell = *c;
                    break;
                }
            }
        }
    }

    // Delete old array
    delete[] oldCells;
}



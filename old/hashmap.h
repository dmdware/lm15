

#ifndef HASHMAP_H
#define HASHMAP_H

#include "../platform.h"

//intended specifically for PathNode's

//#define MAXHASH			105
#define MAXHASH			512
//#define MAXHASH		1024
//#define MAXHASH		2048

//#define HASHPOOL	//use memory pooling and hash map?

#define HASHFUNC(k)		(k*0x85ebca6b)

#ifndef HASHPOOL
#define HASHKEY(a)		(int)a
#else
#define HASHKEY(a)		(((PathNode*)a)->index)	//note: this hash map makes use of key 0 slot, so path node with index 0 cannot be used. edit: changed to -1, hopefully it works.
#endif

// https://github.com/preshing/CompareIntegerMaps/blob/master/hashtable.h
// https://github.com/preshing/CompareIntegerMaps/blob/master/hashtable.cpp


#define FIRST_CELL(hash) ((m_cells + ((hash) & (m_arraySize - 1))))
#define CIRCULAR_NEXT(c) ((c) + 1 != m_cells + m_arraySize ? (c) + 1 : m_cells)
#define CIRCULAR_OFFSET(a, b) ((b) >= (a) ? (b) - (a) : m_arraySize + (b) - (a))

typedef uint32_t HashKey;	//the path node index PATHNODEINDEX(nx,ny)

class HashMap
{
public:
	void* get(HashKey key);
	void** HashMap::getcell(HashKey key);
	void add(HashKey key, void* value);
	void free();
	
    void Repopulate(uint32_t desiredSize);
	void Delete(void** cell);
    void Clear();
    void Compact();

    void Delete(HashKey key)
    {
        void** value = getcell(key);
        if (value)
            Delete(value);
    }

	//HashVal* values[MAXHASH];
	bool m_zeroUsed;
    void* m_zeroCell;
	void** m_cells;

    uint32_t m_arraySize;
    uint32_t m_population;

	HashMap();
	~HashMap();
};

#endif
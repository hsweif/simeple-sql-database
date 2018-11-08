#ifndef RM_RECORD_H
#define RM_RECORD_H
#include "RID.h"
#include "../utils/pagedef.h"

class RM_Record {
private:
	RID mRid;
	int recordSize;
	BufType mData;
public:
	RM_Record  ();                     // Constructor
	~RM_Record (){};                     // Destructor
	int SetRecord	(BufType pData,int size,RID id);
	BufType GetData () const;   // Set pData to point to
	//   the record's contents
	int GetRid     (RID &rid) const;       // Get the record id
	int GetSize(int &sz) const;
};
#endif
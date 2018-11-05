#ifndef RM_RECORD_H
#define RM_RECORD_H
#include "RID.h"
class RM_Record {
private:
	RID mRid;
	int recordSize;
	char * mData;
public:
	RM_Record  ();                     // Constructor
	~RM_Record ();                     // Destructor
	int SetRecord	(char *pData,int size,RID id);
	int GetData    (char *&pData) const;   // Set pData to point to
	//   the record's contents
	int GetRid     (RID &rid) const;       // Get the record id
};
#endif
#ifndef RID_H
#define RID_H
#include "../utils/pagedef.h"
#include <iostream>
class RID {
private:
	int mPageNum;
	int mSlotNum;
public:
	RID        ();                        // Default constructor
	~RID       ();                        // Destructor
	RID        (int pageNum, int slotNum);
	// Construct RID from page and
	//   slot number
	int GetPageNum (int &pageNum) const;  // Return page number
	int GetSlotNum (int &slotNum) const;  // Return slot number
	BufType Serialize();
	int Deserialize(BufType buf);
	void show();
};
#endif
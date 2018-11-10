#ifndef RM_FILESCAN
#define RM_FILESCAN

class RM_FileScan {
  public:
       RM_FileScan  ();                                // Constructor
       ~RM_FileScan ();                                // Destructor
    RC OpenScan     (const RM_FileHandle &fileHandle,  // Initialize file scan
                     AttrType      attrType,
                     int           attrLength,
                     int           attrOffset,
                     CompOp        compOp,
                     void          *value,
                     ClientHint    pinHint = NO_HINT);
    RC GetNextRec   (RM_Record &rec);                  // Get next matching record
    RC CloseScan    ();                                // Terminate file scan
};
#endif // RM_FILESCAN
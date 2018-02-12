
void debug(const char* message);
void fatal(const char* message);
void fatalExt(const char* first, const char* second);

void initializeApplicationMemory();
void initializeAggregatorMemory();
void freeAggregatorMemory();

appAggregator* memoryGetAggregator(int8_t number);

void heapFreeRest();
void heapMarkAndSweep();
void pushLocalContext(bool restricted);
void popLocalContext();
void memoryAggregatorAdd(int8_t number, OBJ res);

OBJEntry* memoryGetObjectEntry(const char* key);
OBJ memoryGetObject(const char* key);

void newGlobalVariable(const char* name);
OBJEntry* _newLocalVariable(const char* name, OBJ variable);

OBJ newUninitialized();
OBJ newBoolean(bool val);
OBJ newInteger(int32_t val);
OBJ newBigInteger(int64_t val);
OBJ newAutoInteger(int64_t val);
OBJ newString(const char* string);
OBJ newEnvironment(struct objEnvironment* parent, bool restricted);

OBJ cloneObject(const OBJ obj);

void ParseProgram();

void runProgram();
void statementLoop(astNode* node);
OBJ eval(astNode* node);

void printMemoryStatus();
void printObject(OBJ obj);
void printObjectVerbose(OBJ obj);
OBJ getObjectFromBuffer(char* buffer);

OBJ handleFunctionCall(const char* name, int8_t aggr);

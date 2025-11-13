#define FORWARDS = true
#define BACKWARDS = false

int HBA[] = {2, 3, 4, 5};
int HBB[] = {6, 7, 8, 9};
int HBC[] = {10, 11, 12, 13};


void setup()
{
    for (auto &&i : HBA)
    {
        pinMode(i, OUTPUT);
    }
    for (auto &&i : HBB)
    {
        pinMode(i, OUTPUT);
    }
    for (auto &&i : HBC)
    {
        pinMode(i, OUTPUT);
    }
}

void loop()
{
    
}

void switchPair(int HBX[4], bool direction)
{
    digitalWrite()
}
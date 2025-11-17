#define FORWARDS true
#define BACKWARDS false

// { switch A1, A2, B1, B2. interrupt pin }
int HBA[] = {2, 3, 4, 5};
int HBB[] = {6, 7, 8, 9};
int HBC[] = {10, 11, 12, 13};


// switching params
// volatile bool HBX_to_switch[] = { false, false, false };
volatile int HBX_cycle_state = 0;
volatile bool HBX_second_half = false;

// interrupt array
int HBX_interrupt_pins[] = { 18, 19, 20 };

// function prototypes
void switchPair(int HBX[4], bool direction = false, bool off = false); // might comment this out if compiling and uploading with Arduino IDE

void setup()
{
    for (auto &&i : HBA) { pinMode(i, OUTPUT); }
    for (auto &&i : HBB) { pinMode(i, OUTPUT); }
    for (auto &&i : HBC) { pinMode(i, OUTPUT); }

    // initializing interrupts
    attachInterrupt(digitalPinToInterrupt(HBX_interrupt_pins[0]), switch_A, RISING); // A
    attachInterrupt(digitalPinToInterrupt(HBX_interrupt_pins[1]), switch_B, RISING); // B
    attachInterrupt(digitalPinToInterrupt(HBX_interrupt_pins[2]), switch_C, RISING); // C
}

void loop()
{
    switch (HBX_cycle_state)
    {
    case 0:
        switchPair(HBA, !HBX_second_half);
        switchPair(HBB, HBX_second_half);
        switchPair(HBC, false, true);
        break;
    case 1:
        switchPair(HBA, false, true);
        switchPair(HBB, !HBX_second_half);
        switchPair(HBC, HBX_second_half);
        break;
    case 2:
        switchPair(HBA, HBX_second_half);
        switchPair(HBB, false, true);
        switchPair(HBC, !HBX_second_half);
        break;
    default:
        break;
    }
}

void switchPair(int HBX[4], bool direction = false, bool off = false)
{
    auto level = LOW;
    auto level2 = LOW;
    /*if (off)
    {
        level = LOW;
        level2 = LOW;
    }
    else
    {
        level = direction ? HIGH : LOW;
        level2 = direction ? LOW : HIGH;
    }
    
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(HBX[i * 2], level);
    }
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(HBX[(i * 2) + 1], level2);
    }*/
    if (!off)
    {
        level = direction ? HIGH : LOW;   // A1, A2
        level2 = direction ? LOW : HIGH;   // B1, B2
    }
    // A1, A2
    digitalWrite(HBX[0], level);
    digitalWrite(HBX[1], level);

    // B1, B2
    digitalWrite(HBX[2], level2);
    digitalWrite(HBX[3], level2);
}

void switch_A()
{
    HBX_cycle_state = 0;
    HBX_second_half = !HBX_second_half;
}
void switch_B()
{
    HBX_cycle_state = 1;
    HBX_second_half = !HBX_second_half;
}
void switch_C()
{
    HBX_cycle_state = 2;
    HBX_second_half = !HBX_second_half;
}

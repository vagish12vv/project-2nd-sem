#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <windows.h> 

using namespace std;

void typewrite(const string& text, int delayMs = 25) {
    for (size_t i = 0; i < text.length(); i++) {
        cout << text[i] << flush;
        Sleep(delayMs); 
    }
    cout << endl;
}

void slowPrint(const string& text, int delayMs = 10) {
    for (size_t i = 0; i < text.length(); i++) {
        cout << text[i] << flush;
        Sleep(delayMs);
    }
    cout << endl;
}

void pause(int ms = 600) {
    Sleep(ms);
}

void printLine(char c = '=', int len = 55) {
    for (int i = 0; i < len; i++) cout << c;
    cout << endl;
}

int getChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "  >> ";
        if (cin >> choice && choice >= min && choice <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Enter a number between "
             << min << " and " << max << ".\n";
    }
}

class Item {
private:
    string name;        
    string description; 
    bool   usable;      

public:
    Item(string n, string d, bool u = false)
        : name(n), description(d), usable(u) {}

    virtual ~Item() {}

    string getName()        const { return name; }
    string getDescription() const { return description; }
    bool   isUsable()       const { return usable; }

    virtual void displayItem() const {
        cout << "  [Item] " << name << endl;
        cout << "         " << description << endl;
    }
};
class Key : public Item {
private:
    string unlocksDoorName; 

public:
    Key(string n, string d, string doorName)
        : Item(n, d, true), unlocksDoorName(doorName) {}

    string getUnlocksDoorName() const { return unlocksDoorName; }

    void displayItem() const {
        cout << "  [Key]  " << getName() << endl;
        cout << "         " << getDescription() << endl;
        cout << "         Unlocks: " << unlocksDoorName << endl;
    }
};

class Room; 

class Door {
private:
    string doorName;     
    string direction;    
    Room* targetRoom;   
    bool   isLocked;     
    string requiredKey;  

public:
    Door(string name, string dir, bool locked, string keyNeeded)
        : doorName(name), direction(dir), targetRoom(NULL), 
          isLocked(locked), requiredKey(keyNeeded) {}

    void setTargetRoom(Room* r) { targetRoom = r; }

    string getDoorName()   const { return doorName; }
    string getDirection()  const { return direction; }
    Room* getTargetRoom() const { return targetRoom; }
    bool   getIsLocked()   const { return isLocked; }
    string getRequiredKey() const { return requiredKey; }

    void displayDoor() const {
        cout << "  [" << direction << "] " << doorName;
        if (isLocked)
            cout << "  (LOCKED - needs: " << requiredKey << ")";
        else
            cout << "  (OPEN)";
        cout << endl;
    }

    bool unlockDoor(const string& keyName) {
        if (keyName == requiredKey) {
            isLocked = false;
            return true;
        }
        return false;
    }
};

class Room {
private:
    string         name;
    string         description;
    vector<Item*>  items;          
    map<string, Door*> exits;      

public:
    Room(string n, string d) : name(n), description(d) {}

    ~Room() {}

    string getName()        const { return name; }
    string getDescription() const { return description; }
    map<string, Door*>& getExits() { return exits; }
    vector<Item*>& getItems()      { return items; }

    void addExit(string direction, Door* door) {
        exits[direction] = door;
    }

    void addItem(Item* item) {
        items.push_back(item);
    }

    void displayRoom() const {
        printLine();
        cout << "  LOCATION: " << name << endl;
        printLine('-', 55);
        typewrite("  " + description);
        printLine();
    }

    void showItems() const {
        if (items.empty()) {
            cout << "  There are no items in this room.\n";
            return;
        }
        cout << "  Items in " << name << ":\n";
        for (int i = 0; i < (int)items.size(); i++) {
            cout << "  " << (i + 1) << ". ";
            items[i]->displayItem();
        }
    }

    void showExits() const {
        if (exits.empty()) {
            cout << "  No exits visible.\n";
            return;
        }
        cout << "  Exits from " << name << ":\n";
        int i = 1;
        for (map<string, Door*>::const_iterator it = exits.begin(); it != exits.end(); ++it) {
            cout << "  " << i++ << ". ";
            it->second->displayDoor();
        }
    }

    Item* removeItem(int idx) {
        if (idx < 0 || idx >= (int)items.size()) return NULL;
        Item* it = items[idx];
        items.erase(items.begin() + idx);
        return it;
    }
};

class Player {
private:
    string         name;        
    Room* currentRoom; 
    vector<Item*>  inventory;   

public:
    Player(string playerName, Room* startRoom)
        : name(playerName), currentRoom(startRoom) {}

    Room* getCurrentRoom() const { return currentRoom; }
    vector<Item*>& getInventory()         { return inventory; }
    string         getName()        const { return name; }

    void move(Room* newRoom) {
        currentRoom = newRoom;
    }

    void addItem(Item* item) {
        inventory.push_back(item);
    }

    void showInventory() const {
        if (inventory.empty()) {
            cout << "  Your bag is empty.\n";
            return;
        }
        cout << "  == " << name << "'s Inventory ==\n";
        for (int i = 0; i < (int)inventory.size(); i++) {
            cout << "  " << (i + 1) << ". ";
            inventory[i]->displayItem();
        }
    }

    Key* findKey(const string& keyName) const {
        for (size_t i = 0; i < inventory.size(); i++) {
            Key* k = dynamic_cast<Key*>(inventory[i]); 
            if (k && k->getName() == keyName)
                return k;
        }
        return NULL;
    }
};

class GameEngine {
private:
    Player* player;       
    vector<Room*> rooms;        
    vector<Item*> allItems;     
    vector<Door*> allDoors;     
    bool          gameRunning;  
    bool          playerWon;    

    void setupWorld() {
        // --- 1. CREATE ROOMS ---
        Room* hall     = new Room("Main Hall",
            "A grand but decaying hall. The massive front door is heavily locked. "
            "There are passages leading to other parts of the house.");

        Room* bedroom  = new Room("Bedroom",
            "A dusty bedroom with a torn mattress. The walls are covered in scratch marks.");

        Room* library  = new Room("Library", // NEW ROOM
            "Tall bookshelves line the walls. Most books are rotting, but the room feels heavy with secrets. "
            "There is a strange trapdoor on the floor.");

        Room* kitchen  = new Room("Kitchen",
            "A filthy kitchen smelling of rot. Rusty knives are scattered everywhere.");

        Room* basement = new Room("Secret Basement", // NEW ROOM
            "A pitch-black cellar. It's freezing cold. A small pedestal sits in the center.");

        Room* exitRoom = new Room("Outside / Exit",
            "The cool night wind hits you. You are free!");

        rooms.push_back(hall);
        rooms.push_back(bedroom);
        rooms.push_back(library);
        rooms.push_back(kitchen);
        rooms.push_back(basement);
        rooms.push_back(exitRoom);

        // --- 2. CREATE KEYS ---
        Key* brassKey = new Key("Brass Key", "A sturdy brass key.", "Library Door");
        Key* rustyKey = new Key("Rusty Key", "Covered in dried brown stains.", "Kitchen Door");
        Key* ironKey  = new Key("Iron Key", "Heavy and cold to the touch.", "Basement Door");
        Key* silverKey= new Key("Silver Key", "It glows faintly in the dark. Your ticket out.", "Exit Door");

        allItems.push_back(brassKey);
        allItems.push_back(rustyKey);
        allItems.push_back(ironKey);
        allItems.push_back(silverKey);

        // --- 3. CREATE RIDDLES (HINTS) ---
        Item* note1 = new Item("Torn Note", "RIDDLE: 'Where dreams are made and nightmares creep, the path to knowledge lies where you sleep.'");
        Item* note2 = new Item("Dusty Scroll", "RIDDLE: 'Among the dusty tomes and pages old, rests the key to where food grows cold.'");
        Item* note3 = new Item("Burnt Paper", "RIDDLE: 'Where fire burned and water flows, the iron tooth is hidden, I suppose.'");
        Item* note4 = new Item("Bloody Scrawl", "RIDDLE: 'Deep below the written word, in the dark where shadows blur, lies your freedom, cold and pure.'");

        allItems.push_back(note1);
        allItems.push_back(note2);
        allItems.push_back(note3);
        allItems.push_back(note4);

        // --- 4. PLACE ITEMS IN ROOMS ---
        hall->addItem(note1);        // Hint for Bedroom
        
        bedroom->addItem(brassKey);  // Key to Library
        bedroom->addItem(note2);     // Hint for Library
        
        library->addItem(rustyKey);  // Key to Kitchen
        library->addItem(note3);     // Hint for Kitchen
        
        kitchen->addItem(ironKey);   // Key to Basement
        kitchen->addItem(note4);     // Hint for Basement
        
        basement->addItem(silverKey);// Key to Exit (Main Hall)

        // --- 5. CREATE DOORS & EXITS ---
        
        // Hall <-> Bedroom (Always open)
        Door* hallToBed = new Door("Bedroom Door", "North", false, "none");
        hallToBed->setTargetRoom(bedroom);
        hall->addExit("North", hallToBed);

        Door* bedToHall = new Door("Hall Door", "South", false, "none");
        bedToHall->setTargetRoom(hall);
        bedroom->addExit("South", bedToHall);

        // Hall <-> Library (Locked by Brass Key)
        Door* hallToLib = new Door("Library Door", "East", true, "Brass Key");
        hallToLib->setTargetRoom(library);
        hall->addExit("East", hallToLib);

        Door* libToHall = new Door("Hall Door", "West", false, "none");
        libToHall->setTargetRoom(hall);
        library->addExit("West", libToHall);

        // Hall <-> Kitchen (Locked by Rusty Key)
        Door* hallToKit = new Door("Kitchen Door", "West", true, "Rusty Key");
        hallToKit->setTargetRoom(kitchen);
        hall->addExit("West", hallToKit);

        Door* kitToHall = new Door("Hall Door", "East", false, "none");
        kitToHall->setTargetRoom(hall);
        kitchen->addExit("East", kitToHall);

        // Library <-> Basement (Locked by Iron Key)
        Door* libToBase = new Door("Basement Door", "Down", true, "Iron Key");
        libToBase->setTargetRoom(basement);
        library->addExit("Down", libToBase);

        Door* baseToLib = new Door("Library Trapdoor", "Up", false, "none");
        baseToLib->setTargetRoom(library);
        basement->addExit("Up", baseToLib);

        // Hall -> EXIT (Locked by Silver Key)
        Door* hallToExit = new Door("Main Exit Door", "South", true, "Silver Key");
        hallToExit->setTargetRoom(exitRoom);
        hall->addExit("South", hallToExit);

        allDoors.push_back(hallToBed);
        allDoors.push_back(bedToHall);
        allDoors.push_back(hallToLib);
        allDoors.push_back(libToHall);
        allDoors.push_back(hallToKit);
        allDoors.push_back(kitToHall);
        allDoors.push_back(libToBase);
        allDoors.push_back(baseToLib);
        allDoors.push_back(hallToExit);

        player = new Player("Explorer", hall);
    }

    void showMainMenu() {
        cout << endl;
        printLine('*');
        slowPrint("  * ESCAPE ROOM  -  THE FORGOTTEN HOUSE    *");
        printLine('*');
        cout << endl;
        typewrite("  You wake up in the Main Hall. The front door is locked.", 20);
        typewrite("  You must solve the riddles, find the keys, and ESCAPE.", 20);
        cout << endl;
        printLine('-');
        cout << "  1. Start Game\n";
        cout << "  2. Instructions\n";
        cout << "  3. Exit\n";
        printLine('-');
        cout << "  Your choice: ";
    }

    void showInstructions() {
        cout << endl;
        printLine('=');
        cout << "  HOW TO PLAY\n";
        printLine('=');
        cout << "  - 'Look around'     : Read the room description.\n";
        cout << "  - 'Show items'      : Read riddles and find keys.\n";
        cout << "  - 'Pick up item'    : Add an item to your bag.\n";
        cout << "  - 'Show doors'      : See exits and their lock state.\n";
        cout << "  - 'Unlock a door'   : Use a key from your bag.\n";
        cout << "  - 'Move'            : Walk through an unlocked door.\n";
        cout << "  - 'Inventory'       : Check what you are carrying.\n";
        cout << endl;
        cout << "  HINT: Read the note in the Main Hall first!\n";
        printLine('=');
        pause(300);
    }

    void showRoomMenu() {
        cout << endl;
        printLine('-');
        cout << "  You are in: " << player->getCurrentRoom()->getName() << endl;
        printLine('-');
        cout << "  1. Look around\n";
        cout << "  2. Show items in room\n";
        cout << "  3. Pick up an item\n";
        cout << "  4. Show available doors\n";
        cout << "  5. Unlock a door\n";
        cout << "  6. Move to another room\n";
        cout << "  7. Show inventory\n";
        cout << "  8. Quit game\n";
        printLine('-');
    }

    void pickUpItem() {
        Room* room = player->getCurrentRoom();
        if (room->getItems().empty()) {
            typewrite("  There is nothing to pick up here.");
            return;
        }
        room->showItems();
        cout << "  Enter item number to pick up (0 to cancel): ";
        int choice = getChoice(0, (int)room->getItems().size());
        if (choice == 0) { cout << "  Cancelled.\n"; return; }

        Item* picked = room->removeItem(choice - 1);
        if (picked) {
            player->addItem(picked);
            pause(200);
            typewrite("  You picked up: " + picked->getName());
        }
    }

    void unlockDoor() {
        Room* room = player->getCurrentRoom();
        vector<Door*> lockedDoors;
        
        for (map<string, Door*>::iterator it = room->getExits().begin(); it != room->getExits().end(); ++it) {
            if (it->second->getIsLocked())
                lockedDoors.push_back(it->second);
        }

        if (lockedDoors.empty()) {
            typewrite("  All doors here are already unlocked.");
            return;
        }

        cout << "  Locked doors:\n";
        for (int i = 0; i < (int)lockedDoors.size(); i++) {
            cout << "  " << (i + 1) << ". ";
            lockedDoors[i]->displayDoor();
        }
        cout << "  Choose door to unlock (0 to cancel): ";
        int doorChoice = getChoice(0, (int)lockedDoors.size());
        if (doorChoice == 0) { cout << "  Cancelled.\n"; return; }

        Door* chosenDoor = lockedDoors[doorChoice - 1];

        vector<Item*>& inv = player->getInventory();
        if (inv.empty()) {
            typewrite("  You have no keys in your bag!");
            return;
        }

        cout << "  Choose a key from your inventory:\n";
        for (int i = 0; i < (int)inv.size(); i++) {
            cout << "  " << (i + 1) << ". " << inv[i]->getName() << endl;
        }
        cout << "  (0 to cancel): ";
        int keyChoice = getChoice(0, (int)inv.size());
        if (keyChoice == 0) { cout << "  Cancelled.\n"; return; }

        Item* selectedItem = inv[keyChoice - 1];
        Key* selectedKey   = dynamic_cast<Key*>(selectedItem);

        if (!selectedKey) {
            typewrite("  That item is not a key!");
            return;
        }

        pause(300);
        if (chosenDoor->unlockDoor(selectedKey->getName())) {
            typewrite("  *CLICK* The heavy lock turns and falls away!");
        } else {
            typewrite("  The key does not fit this door. Wrong key!");
        }
    }

    void moveRoom() {
        Room* room = player->getCurrentRoom();
        vector<Door*> openDoors;
        
        for (map<string, Door*>::iterator it = room->getExits().begin(); it != room->getExits().end(); ++it) {
            if (!it->second->getIsLocked())
                openDoors.push_back(it->second);
        }

        if (openDoors.empty()) {
            typewrite("  All exits are locked. Find keys first!");
            return;
        }

        cout << "  Choose where to go:\n";
        for (int i = 0; i < (int)openDoors.size(); i++) {
            cout << "  " << (i + 1) << ". [" << openDoors[i]->getDirection()
                 << "] -> " << openDoors[i]->getTargetRoom()->getName() << endl;
        }
        cout << "  (0 to cancel): ";
        int choice = getChoice(0, (int)openDoors.size());
        if (choice == 0) { cout << "  Cancelled.\n"; return; }

        Room* dest = openDoors[choice - 1]->getTargetRoom();
        pause(400);
        typewrite("  You walk through the doorway...");
        pause(500);
        player->move(dest);
        dest->displayRoom();

        if (dest->getName() == "Outside / Exit") {
            pause(500);
            showWinScreen();
            gameRunning = false;
            playerWon   = true;
        }
    }

    void showWinScreen() {
        cout << endl;
        printLine('*');
        slowPrint("  * *", 5);
        slowPrint("  * YOU ESCAPED! CONGRATULATIONS!                       *", 8);
        slowPrint("  * *", 5);
        printLine('*');
        cout << endl;
        typewrite("  The heavy wooden door groans open.", 22);
        typewrite("  You step out into the cold night wind.", 22);
        typewrite("  You survived the Forgotten House.", 22);
        cout << endl;
        typewrite("  Thank you for playing!  -- GAME OVER --", 18);
        cout << endl;
    }

    void showQuitScreen() {
        cout << endl;
        printLine('-');
        typewrite("  You sit down in despair. The house wins this time.");
        printLine('-');
        cout << "  Thanks for playing! Goodbye.\n\n";
    }

public:
    GameEngine() : player(NULL), gameRunning(false), playerWon(false) {} 

    ~GameEngine() {
        delete player;
        for (size_t i = 0; i < rooms.size(); i++)    delete rooms[i];
        for (size_t i = 0; i < allItems.size(); i++) delete allItems[i];
        for (size_t i = 0; i < allDoors.size(); i++) delete allDoors[i];
    }

    void run() {
        bool inMenu = true;
        while (inMenu) {
            showMainMenu();
            int choice = getChoice(1, 3);
            switch (choice) {
                case 1: inMenu = false; break;
                case 2: showInstructions(); break;
                case 3: cout << "\n  Goodbye! Maybe another time.\n\n"; return;
            }
        }

        setupWorld();

        cout << endl;
        printLine('=');
        typewrite("  The last thing you remember is a blinding flash.");
        typewrite("  Now, you stand in a massive, dusty hall.");
        typewrite("  There is a note on the floor. It might help you.");
        printLine('=');
        pause(800);

        player->getCurrentRoom()->displayRoom();

        gameRunning = true;
        while (gameRunning) {
            showRoomMenu();
            int action = getChoice(1, 8);

            switch (action) {
                case 1: player->getCurrentRoom()->displayRoom(); break;
                case 2: player->getCurrentRoom()->showItems(); break;
                case 3: pickUpItem(); break;
                case 4: player->getCurrentRoom()->showExits(); break;
                case 5: unlockDoor(); break;
                case 6: moveRoom(); break;
                case 7: player->showInventory(); break;
                case 8: gameRunning = false; break;
            }
            pause(200);
        }

        if (!playerWon) showQuitScreen();
    }
};

int main() {
    GameEngine engine;
    engine.run();
    return 0;
}
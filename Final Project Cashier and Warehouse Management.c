#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <math.h>

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define BOLD "\x1b[1m"
#define ITALIC "\x1b[3m"
#define CLEAR_LINE "\x1b[2K"
#define TABLESIZE 50

// Struct untuk menyimpan data hashing
struct hashTable {
    char itemID[5];
    char itemName[15];
    int stock;
    long int price;
    char expiryDate[15];
    struct hashTable *next;
};

struct data {
    char itemID[5];
    char itemName[15];
    int stock;
    long int price;
    char expiryDate[15];
};

struct date {
    int day;
    int month;
    int year;
};

struct hashTable *table[TABLESIZE];

// Metode hashing division
// Rumus => Setiap huruf dari id (ASCII) dijumlah lalu dibagi table size
int hashFunction(char id[5]) {
    int index = 0;
    for(int i = 0; i < strlen(id); i++) {
        index += id[i];
    }
    return index % TABLESIZE;
}

struct hashTable *createNode(char id[5], char name[15], int productStock, long int productPrice, char expiry[15]) {
    struct hashTable *newNode = (struct hashTable*)malloc(sizeof(struct hashTable));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    strcpy(newNode->itemID, id);
    strcpy(newNode->itemName, name);
    newNode->stock = productStock;
    newNode->price = productPrice;
    strcpy(newNode->expiryDate, expiry);
    newNode->next = NULL;
    return newNode;
}

void insert(char id[5], char name[15], int productStock, long int productPrice, char expiry[15]) {
    int index = hashFunction(id);
    struct hashTable *newNode = createNode(id, name, productStock, productPrice, expiry);

    newNode->next = table[index];
    table[index] = newNode;
}

void extractFile(int *totalProduct) {
    FILE *file = fopen("Database/database.txt", "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        exit(1);
    }

    char line[150];
    while (fgets(line, sizeof(line), file)) {
        char id[5];
        char name[15];
        int stock;
        long int price;
        char expiry[15];

        if (sscanf(line, "%[^,],%[^,],%d,%ld,%[^\n]\n", id, name, &stock, &price, expiry) == 5) {
            insert(id, name, stock, price, expiry);
            *totalProduct = *totalProduct + 1;
        } else {
            printf("Failed to parse line.\n");
        }
    }

    fclose(file);
}

void saveToFile() {
    FILE *file = fopen("Database/database.txt", "w");
    if (file) {
        for (int i = 0; i < TABLESIZE; i++) {
            struct hashTable *node = table[i];
            while (node) {
                fprintf(file, "%s,%s,%d,%ld,%s\n", node->itemID, node->itemName, node->stock, node->price, node->expiryDate);
                node = node->next;
            }
        }
        fclose(file);
    }
}

void appendToFile(struct data purchase[20], int totalPurchase) {
    FILE *file = fopen("Database/cashflow.txt", "a");
    if (file) {
        for (int i = 0; i < totalPurchase; i++) {
            fprintf(file, "%s,%s,%d,%ld,%s\n", purchase[i].itemID, purchase[i].itemName, purchase[i].stock, purchase[i].price, purchase[i].expiryDate);
        }
        fclose(file);
    }
}

void importCashflow(struct data cashflow[100], int *totalProduct) {
    FILE *file = fopen("Database/cashflow.txt", "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        exit(1);
    }

    char line[150];
    while (fgets(line, sizeof(line), file)) {
        char id[5];
        char name[15];
        int quantity;
        long int price;
        char buyDate[15];

        if (sscanf(line, "%[^,],%[^,],%d,%ld,%[^\n]\n", id, name, &quantity, &price, buyDate) == 5) {
            strcpy(cashflow[*totalProduct].itemID, id);
            strcpy(cashflow[*totalProduct].itemName, name);
            strcpy(cashflow[*totalProduct].expiryDate, buyDate);
            cashflow[*totalProduct].stock = quantity;
            cashflow[*totalProduct].price = price;
            *totalProduct = *totalProduct + 1;
        } else {
            printf("Failed to parse line.\n");
        }
    }

    fclose(file);
}

// Fungsi menentukan position kursor
void cursorPositionCenter(int lineNumber, int textWidth) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    int consoleWidth = csbi.dwSize.X;

    // Calculate the center position
    COORD coord = { (consoleWidth - textWidth) / 2, lineNumber }; 
    SetConsoleCursorPosition(hConsole, coord);
}

void clearHash() {
    for (int i = 0; i < TABLESIZE; i++) {
        table[i] = NULL;
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Fungsi menentukan tanggal saat ini
void today() {
    int day, month, year;
    time_t now;
    time(&now);

    struct tm *local = localtime(&now);

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    printf("Today Date : %02d/%02d/%d\n\n", day, month, year);
}

void todayDate(char dateNow[15]) {
    int day, month, year;
    time_t now;
    time(&now);

    struct tm *local = localtime(&now);

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    sprintf(dateNow, "%02d/%02d/%04d", day, month, year);
}

struct date parseDate(char *dateStr) {
    struct date tanggal;
    sscanf(dateStr, "%d/%d/%d", &tanggal.day, &tanggal.month, &tanggal.year);
    return tanggal;
}

int compareDates(struct date date1, struct date date2) {
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    }
    if (date1.month != date2.month) {
        return date1.month - date2.month;
    }
    return date1.day - date2.day;
}

void lowercase(char name[15]) {
    for(int i = 0; i < strlen(name); i++) {
        name[i] =  tolower(name[i]);
    }
}

int isDigits(const char *str) {
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int checkAvailability(char id[5]) {
    for (int i = 0; i < TABLESIZE; i++) {
        struct hashTable* node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                return 1; 
            }
            node = node->next;
        }
    }
    return 0; 
}

// Rumus membuat ID:
// 2 kata pertama: 1) Jika 2 kata, maka huruf pertama dari setiap kata
//                 2) Jika 1 kata, maka 2 huruf pertama dari kata tersebut
// 2 angka terakhir: Panjang nama produk + 20
// Jika ID sudah terpakai, maka angka + 1 sampai dapat ID yang masih kosong
void generateId(char *input, char *id) {
    id[0] = '\0';

    if (input == NULL || strlen(input) == 0) {
        return;
    }

    char inputCopy[100];
    strncpy(inputCopy, input, 100);
    inputCopy[99] = '\0'; 

    char *firstWord = strtok(inputCopy, " ");
    char *secondWord = strtok(NULL, " ");

    if (secondWord != NULL) {
        id[0] = toupper(firstWord[0]);
        id[1] = toupper(secondWord[0]);
    } else {
        id[0] = toupper(firstWord[0]);
        id[1] = toupper(firstWord[1]);
    }

    int length = 0;
    for (int i = 0; i < strlen(input); i++) {
        length++;
    }

    int lastTwoDigits = length + 20;

    sprintf(id + 2, "%02d", lastTwoDigits);
    id[4] = '\0'; 

    while (checkAvailability(id)) {
        lastTwoDigits++;
        sprintf(id + 2, "%02d", lastTwoDigits);
    }
}

int validateID(const char *id) {
    if (strlen(id) != 4) {
        return 0;
    }

    if (!(id[0] >= 'A' && id[0] <= 'Z' && id[1] >= 'A' && id[1] <= 'Z')) {
        return 0;
    }

    if (!(id[2] >= '0' && id[2] <= '9' && id[3] >= '0' && id[3] <= '9')) {
        return 0;
    }

    return 1;
}

int validateName(char name[15]) {
    if (strlen(name) < 3 || strlen(name) > 15) {
        cursorPositionCenter(25, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(25, 88);
        printf("Status: " RED "Invalid name format!" RESET "\n");
        return 0;
    }

    int found = 0;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *current = table[i];
        while (current) {
            char name1[15], name2[15];
            strcpy(name1, current->itemName);
            strcpy(name2, name);
            lowercase(name1);
            lowercase(name2);

            if (strcmp(name1, name2) == 0) {
                found = 1;
            }
            current = current->next;
        }
        if (found == 1) {
            cursorPositionCenter(25, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(25, 88); 
            printf("Status: " RED "Product already existed!" RESET "\n");
            return 0;
        }
    }

    return 1;
}

int validateStock(int stock) {
    if(stock < 1 || stock > 100) {
        cursorPositionCenter(25, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(25, 88);
        printf("Status: " RED "Invalid stock format!" RESET "\n");
        return 0;
    }
    return 1;
}

int validatePrice(long int price, int status) {
    if(price < 1000) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid price format!" RESET "\n");
        return 0;
    }
    return 1;
}

int validateExpDate(char expDate[15], int status) {
    if (strlen(expDate) != 10) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid date format!" RESET "\n");
        return 0;
    }

    if (expDate[2] != '/' || expDate[5] != '/') {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid date format!" RESET "\n");
        return 0;
    }

    char day[3] = { expDate[0], expDate[1], '\0' };
    char month[3] = { expDate[3], expDate[4], '\0' };
    char year[5] = { expDate[6], expDate[7], expDate[8], expDate[9], '\0' };

    if (!isDigits(day) || !isDigits(month) || !isDigits(year)) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid date format!" RESET "\n");
        return 0;
    }

    int dayInt = atoi(day);
    int monthInt = atoi(month);
    int yearInt = atoi(year);

    if (dayInt < 1 || dayInt > 31 || monthInt < 1 || monthInt > 12 || yearInt < 1) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid date format!" RESET "\n");
        return 0;
    }

    if ((monthInt == 4 || monthInt == 6 || monthInt == 9 || monthInt == 11) && dayInt > 30) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Invalid date format!" RESET"\n");
        return 0;
    }
    if (monthInt == 2) {
        int isLeap = (yearInt % 4 == 0 && (yearInt % 100 != 0 || yearInt % 400 == 0));
        if (dayInt > 29 || (dayInt == 29 && !isLeap)) {
            cursorPositionCenter(status, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(status, 88);
            printf("Status: " RED "Invalid date format!" RESET "\n");
            return 0;
        }
    }

    char dateNow[15];
    todayDate(dateNow);

    struct date now = parseDate(dateNow);
    struct date expiry = parseDate(expDate);

    int comparisonResult = compareDates(now, expiry);

    if (comparisonResult >= 0) {
        cursorPositionCenter(status, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(status, 88);
        printf("Status: " RED "Date must be after %s!" RESET "\n", dateNow);
        return 0;
    }

    return 1;
}

// Mencetak page introduction
void intro() {
    system("cls");

    char welcome[47] = "WELCOME TO CASHIER AND WAREHOUSE MANAGEMENT APP";
    char dev[15] = "Developed By:";
    char team1[46] = "Kevin Joseph Handoyo             - 2702355302";
    char team2[46] = "Muhammad Ibraahiim Putra Wahana  - 2702350106";

    cursorPositionCenter(8, 54);
    for (int i = 0; i < 55; i++) {
        Sleep(10);
        printf("\xb2");
    }

    for(int i = 9; i < 17; i++) {
        cursorPositionCenter(i, 54);
        Sleep(20);
        printf("\xb2");
    }

    for(int i = 9; i < 17; i++) {
        cursorPositionCenter(i, -54);
        Sleep(20);
        printf("\xb2");
    }

    cursorPositionCenter(16, 52);
    for (int i = 0; i < 53; i++) {
        Sleep(10);
        printf("\xb2");
    }

    Sleep(100);
    cursorPositionCenter(10, 46);
    printf(BOLD);
    for(int i = 0; i < strlen(welcome); i++) {
        Sleep(10);
        printf("%c", welcome[i]);
    }
    printf(RESET "\n\n");

    Sleep(100);
    cursorPositionCenter(12, 12);
    printf(BOLD ITALIC);
    for(int i = 0; i < strlen(dev); i++) {
        Sleep(10);
        printf("%c", dev[i]);
    }
    printf(RESET "\n");

    Sleep(100);
    cursorPositionCenter(13, 44);
    printf(BOLD);
    for(int i = 0; i < strlen(team1); i++) {
        Sleep(10);
        printf("%c", team1[i]);
    }
    cursorPositionCenter(14, 44);
    for(int i = 0; i < strlen(team2); i++) {
        Sleep(10);
        printf("%c", team2[i]);
    }
    printf(RESET "\n");

    Sleep(1500);
}

// Print menu login
void printLoginMenu(int option) {
    cursorPositionCenter(8, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(10, 42);
    printf("\xb2        " BOLD "Welcome to " GREEN ITALIC "LengkapMart" RESET "        \xb2\n");
    cursorPositionCenter(11, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(12, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(13, 42);
    printf("\xb2                                      \xb2\n");

    if(option == 1) {
        cursorPositionCenter(14, 42);
        printf("\xb2" BLUE "  -> 1. Login as Cashier          <-  " RESET "\xb2\n");
        cursorPositionCenter(15, 42);
        printf("\xb2 2. Login as Warehouse Staff          \xb2\n");
        cursorPositionCenter(16, 42);
        printf("\xb2 3. Exit                              \xb2\n");
    } else if(option == 2) {
        cursorPositionCenter(14, 42);
        printf("\xb2 1. Login as Cashier                  \xb2\n");
        cursorPositionCenter(15, 42);
        printf("\xb2" BLUE "  -> 2. Login as Warehouse Staff  <-  " RESET "\xb2\n");
        cursorPositionCenter(16, 42);
        printf("\xb2 3. Exit                              \xb2\n");
    } else if(option == 3) {
        cursorPositionCenter(14, 42);
        printf("\xb2 1. Login as Cashier                  \xb2\n");
        cursorPositionCenter(15, 42);
        printf("\xb2 2. Login as Warehouse Staff          \xb2\n");
        cursorPositionCenter(16, 42);
        printf("\xb2" BLUE "  -> 3. Exit                      <-  " RESET "\xb2\n");
    }

    cursorPositionCenter(17, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(18, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(20, 63);
    printf("Use arrow keys to navigate and press Enter to select an option.\n");
}

// Menu awal untuk login
int loginPage() {
    int option = 1;
    int ch;

    while (1) {
        system("cls");
        today();
        printLoginMenu(option);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72: // Up
                    if (option > 1) option--;
                    break;
                case 80: // Down
                    if (option < 3) option++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            // Enter
            return option;
        }
    }
}

// Input password dari user
void getPassword(char* password) {
    char ch;
    int i = 0;

    while ((ch = _getch()) != '\r') { 
        if (ch == '\b') { 
            if (i > 0) {
                printf("\b \b");
                i--;
            }
        } else {
            printf("*");
            password[i++] = ch;
        }
    }
    password[i] = '\0';
    printf("\n");
}

// Fungsi untuk login 
int login(int try) {
    system("cls");
    today();

    char username[15], password[15];

    cursorPositionCenter(8, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(10, 42);
    printf("\xb2              " BOLD GREEN ITALIC "LOGIN PAGE" RESET "              \xb2\n");
    cursorPositionCenter(11, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(12, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    for(int i = 13; i < 18; i++) {
        cursorPositionCenter(i, 42);
        printf("\xb2");
    }
    for(int i = 13; i < 18; i++) {
        cursorPositionCenter(i, -36);
        printf("\xb2");
    }
    cursorPositionCenter(18, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(14, 34);
    printf("Username >> "); 
    cursorPositionCenter(16, 34);
    printf("Password >> "); 

    cursorPositionCenter(14, 10);
    scanf(" %[^\n]", username);
    cursorPositionCenter(16, 10);
    getPassword(password);

    if(strcmp(username, "admin") != 0 || strcmp(password, "admin") != 0) {
        if(try == 0) return 0;
        cursorPositionCenter(20, 66);
        printf(RED "Invalid username or password! Please try again (%d more tries/try)" RESET "\n", try);
        try--;
        cursorPositionCenter(22, 34);
        system("pause");
        login(try);
    } else {
        return 1;
    }
}

void printProductOverview(int product) {
    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);

    struct data data[totalProduct]; // Buat array untuk menyimpan data dari hash table
    int j = 0;

    // Memindahkan data dari hash table ke dalam array struct
    for (int i = 0; i < TABLESIZE; i++) {
        struct hashTable *current = table[i];
        while (current != NULL) {
            strcpy(data[j].itemID, current->itemID);
            strcpy(data[j].itemName, current->itemName);
            strcpy(data[j].expiryDate, current->expiryDate);
            data[j].price = current->price;
            data[j].stock = current->stock;
            j++;
            current = current->next;
        }
    }

    // Menampilkan data dari array struct
    system("cls");
    today();
    cursorPositionCenter(2, 20);
    printf("PRODUCT OVERVIEW\n");
    cursorPositionCenter(3, 90);
    for(int i = 0; i < 88; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(4, 90);
    printf("\xb2 %-4s \xb2 %-5s \xb2 %-20s \xb2 %-10s \xb2 %-15s \xb2 %-15s \xb2\n", "No.", "ID", "Product Name", "Stock", "Price", "Expiry Date");
    cursorPositionCenter(5, 90);
    for(int i = 0; i < 88; i++) {
        printf("\xb2");
    }

    int dataShown = 0;
    int count = 0;

    if(product != 1) {
        dataShown = product * 10 - 10;
    }

    // Menampilkan data dari array struct
    for (int i = dataShown; i < totalProduct; i++) {
        cursorPositionCenter(6 + count, 90);
        printf("\xb2 %-4d \xb2 %-5s \xb2 %-20s \xb2 %-10d \xb2 Rp %-12ld \xb2 %-15s \xb2\n", i+1, data[i].itemID, data[i].itemName, data[i].stock, data[i].price, data[i].expiryDate);
        dataShown++;
        count++;
        if (dataShown % 10 == 0) break;
    }

    cursorPositionCenter(6 + count, 90);
    for(int i = 0; i < 88; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(8 + count , 58);
    printf("Use arrow keys (" GREEN "Left" RESET " / " GREEN "Right" RESET ") to view the products list.\n");
}

// Print menu kasir
void printCashierMenu(int option) {
    cursorPositionCenter(20, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(21, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(22, 42);
    printf("\xb2             " BOLD GREEN ITALIC "CASHIER MENU" RESET "             \xb2\n");
    cursorPositionCenter(23, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(24, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(25, 42);
    printf("\xb2                                      \xb2\n");

    if(option == 1) {
        cursorPositionCenter(26, 42);
        printf("\xb2" BLUE "   -> 1. Purchase                <-   " RESET "\xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Cashflow                        \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Hash-Table View                 \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Check Expiration                \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Logout                          \xb2\n");
    } else if(option == 2) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Purchase                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2" BLUE "   -> 2. Cashflow                <-   " RESET "\xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Hash-Table View                 \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Check Expiration                \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Logout                          \xb2\n");
    } else if(option == 3) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Purchase                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Cashflow                        \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2" BLUE "   -> 3. Hash-Table View         <-   " RESET "\xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Check Expiration                \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Logout                          \xb2\n");
    } else if(option == 4) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Purchase                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Cashflow                        \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Hash-Table View                 \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2" BLUE "   -> 4. Check Expiration        <-   " RESET "\xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Logout                          \xb2\n");
    } else if(option == 5) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Purchase                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Cashflow                        \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Hash-Table View                 \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Check Expiration                \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2" BLUE "   -> 5. Logout                  <-   " RESET "\xb2\n");
    }

    cursorPositionCenter(31, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(32, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(34, 76);
    printf("Use arrow keys (" GREEN "Up" RESET " / " GREEN "Down" RESET ") to navigate and press " GREEN "Enter" RESET " to select an option.\n");
}

// Menu pada bagian kasir
void cashierMenu(int *opt) {
    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    int option = 1;
    int product = 1;
    int ch;

    while (1) {
        system("cls");
        today();
        printProductOverview(product);
        printCashierMenu(option);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72: // Up
                    if(option > 1) option--;
                    break;
                case 80: // Down
                    if(option < 5) option++;
                    break;
                case 75: // Left
                    if(product > 1) product--;
                    break;
                case 77: // Right
                    if((float)product < ((float)totalProduct / 10)) product++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            // Enter
            *opt = option;
            return;
        }
    }
}

void purchaseProduct(struct data purchase[20], int *totalPurchase) {
    while(1) {
        system("cls");
        today();

        cursorPositionCenter(8, 92);
        for (int i = 0; i < 90; i++) {
            printf("\xb2");
        }
        cursorPositionCenter(9, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(10, 92);
        printf("\xb2                                    " BOLD  GREEN ITALIC "PURCHASE PRODUCT" RESET "                                    \xb2\n");
        cursorPositionCenter(11, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(12, 92);
        for (int i = 0; i < 90; i++) {
            printf("\xb2");
        }
        for (int i = 0; i < 14; i++) {
            cursorPositionCenter(13 + i, 92);
            printf("\xb2\n");
        }
        for (int i = 0; i < 14; i++) {
            cursorPositionCenter(13 + i, -86);
            printf("\xb2\n");
        }
        cursorPositionCenter(27, 92);
        for (int i = 0; i < 90; i++) {
            printf("\xb2");
        }

        int totalProduct = 0;
        clearHash();
        extractFile(&totalProduct);
        char id[5];

        cursorPositionCenter(16, 88);
        printf("Input total stock to purchase [1-XX] >> ");

        cursorPositionCenter(18, 88);
        printf("Status: ");

        cursorPositionCenter(20, 88);
        printf("Purchase Info: ");

        while (1) {
            cursorPositionCenter(14, 88);
            printf("Input item ID [A-Z][A-Z][1-9][1-9] >> ");
            cursorPositionCenter(14, 12);
            scanf(" %[^\n]", id);

            clearInputBuffer();

            if (validateID(id)) {
                break;
            }

            cursorPositionCenter(18, 88);
            printf(CLEAR_LINE);
            cursorPositionCenter(18, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(18, 88);
            printf("Status: ");
            cursorPositionCenter(18, 72);
            printf(RED "Invalid ID format! Please try again." RESET "\n");
        }

        int foundAt = -1;

        for(int i = 0; i < TABLESIZE; i++) {
            struct hashTable *node = table[i];
            while (node) {
                if (strcmp(node->itemID, id) == 0) {
                    foundAt = i; // Ditemukan
                }
                node = node->next;
            }
            if (foundAt != -1) break;
        }

        if (foundAt != -1) {
            int deleteStock;
            cursorPositionCenter(18, 72);
            printf("ID %s found in the database at index %d.\n", id, foundAt);
            struct hashTable *current = table[foundAt];
            while(strcmp(current->itemID, id) != 0) {
                current = current->next;
            }

            while(1) {
                cursorPositionCenter(16, 88);
                printf(CLEAR_LINE);
                cursorPositionCenter(16, 92);
                printf("\xb2                                                                                        \xb2\n");

                cursorPositionCenter(16, 88);
                printf("Input total stock to purchase [1-%d] >> ", current->stock);
                scanf(" %d", &deleteStock);

                clearInputBuffer();

                if(deleteStock > 0 && deleteStock <= current->stock) {
                    cursorPositionCenter(18, 88);
                    printf(CLEAR_LINE);
                    cursorPositionCenter(18, 92);
                    printf("\xb2                                                                                        \xb2\n");
                    cursorPositionCenter(18, 88);
                    printf("Status: ");
                    cursorPositionCenter(18, 72);
                    printf("ID %s found in the database at index %d.\n", id, foundAt);
                    break;
                }

                cursorPositionCenter(18, 88);
                printf(CLEAR_LINE);
                cursorPositionCenter(18, 92);
                printf("\xb2                                                                                        \xb2\n");
                cursorPositionCenter(18, 88);
                printf("Status: ");
                cursorPositionCenter(18, 72);
                printf(RED "Invalid stock! Please try again." RESET "\n");
            }

            char date[15];
            todayDate(date);

            current->stock -= deleteStock;

            if(current->stock <= 0) {
                strcpy(purchase[*totalPurchase].itemID, current->itemID);
                strcpy(purchase[*totalPurchase].itemName, current->itemName);
                purchase[*totalPurchase].price = current->price;
                purchase[*totalPurchase].stock = deleteStock;
                char dateNow[15];
                todayDate(dateNow);
                strcpy(purchase[*totalPurchase].expiryDate, dateNow);

                struct hashTable *node = table[foundAt];
                struct hashTable *prev = NULL;

                while (node != NULL && strcmp(node->itemID, id) != 0) {
                    prev = node;
                    node = node->next;
                }

                if (node == NULL) {
                    return; 
                }

                if (prev == NULL) {
                    table[foundAt] = node->next;
                } else {
                    prev->next = node->next;
                }

                free(node);
                cursorPositionCenter(20, 56);
                printf("%s purchased successfully on %s!\n", current->itemName, date);
                cursorPositionCenter(21, 56);
                printf("New stock: Empty! %s deleted from database succesfully.\n", current->itemName);
            } else {
                strcpy(purchase[*totalPurchase].itemID, current->itemID);
                strcpy(purchase[*totalPurchase].itemName, current->itemName);
                purchase[*totalPurchase].price = current->price;
                purchase[*totalPurchase].stock = deleteStock;
                char dateNow[15];
                todayDate(dateNow);
                strcpy(purchase[*totalPurchase].expiryDate, dateNow);

                cursorPositionCenter(20, 56);
                printf("%s purchased successfully on %s!\n", current->itemName, date);
                cursorPositionCenter(21, 56);
                printf("New stock: %d\n", current->stock);
            }

            *totalPurchase = *totalPurchase + 1;
            
            saveToFile();

            int option = 1;
            int ch;

            cursorPositionCenter(23, 88);
            printf("Do you want to purchase more products?\n");
            while (1) {
                if(option == 1) {
                    cursorPositionCenter(24, 88);
                    printf(BOLD BLUE "[ Yes ]" RESET "\n");
                    cursorPositionCenter(25, 88);
                    printf("[ No ]\n");
                } else if(option == 2) {
                    cursorPositionCenter(24, 88);
                    printf("[ Yes ]\n");
                    cursorPositionCenter(25, 88);
                    printf(BOLD BLUE "[ No ]" RESET "\n");
                }

                ch = _getch();
                if (ch == 0 || ch == 224) {
                    ch = _getch();
                    switch (ch) {
                        case 72: // Up
                            if (option > 1) option--;
                            break;
                        case 80: // Down
                            if (option < 2) option++;
                            break;
                        default:
                            break;
                    }
                } else if (ch == 13) {
                    // Enter
                    if(option != 1) {
                        return;
                    } else break;
                }
            }
        } else {
            cursorPositionCenter(18, 72);
            printf("ID %s not found in the database. Please try again.\n", id);
            cursorPositionCenter(29, 34);
            system("pause");
            break;
        }
    }
}

void purchaseSummary(struct data purchase[20], int totalPurchase) {
    system("cls");
    today();

    cursorPositionCenter(6, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(7, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(8, 92);
    printf("\xb2                                    " BOLD GREEN ITALIC "PURCHASE SUMMARY" RESET "                                    \xb2\n");
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(11, 92);
    printf("\xb2   ID   \xb2        Item Name        \xb2  Qty  \xb2       Price       \xb2       Total Price       \xb2\n");
    cursorPositionCenter(13, 92);
    printf("\xb2        \xb2                         \xb2       \xb2                   \xb2                         \xb2\n");

    int grandTotal = 0;

    for(int i = 0; i < totalPurchase; i++) {
        cursorPositionCenter(14 + (i * 2), 92);
        printf("\xb2 [%-4s] \xb2 %-23s \xb2  %-3d  \xb2 Rp %-14ld \xb2 Rp %-20ld \xb2\n", purchase[i].itemID, purchase[i].itemName, purchase[i].stock, purchase[i].price, purchase[i].stock * purchase[i].price);
        cursorPositionCenter(15 + (i * 2), 92);
        printf("\xb2        \xb2                         \xb2       \xb2                   \xb2                         \xb2\n");
        grandTotal += purchase[i].stock * purchase[i].price;
    }

    cursorPositionCenter(14 + (totalPurchase * 2), 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(15 + (totalPurchase * 2), -4);
    printf("\xb2                                        \xb2\n");
    cursorPositionCenter(16 + (totalPurchase * 2), -4);
    printf("\xb2  " BOLD GREEN "Grand Total : Rp %-20d " RESET "\xb2\n", grandTotal);
    cursorPositionCenter(17 + (totalPurchase * 2), -4);
    printf("\xb2                                        \xb2\n");
    cursorPositionCenter(18 + (totalPurchase * 2), -4);
    for (int i = 0; i < 42; i++) {
        printf("\xb2");
    }

    appendToFile(purchase, totalPurchase);

    cursorPositionCenter(20 + (totalPurchase * 2), 34);
    system("pause");
}

void cashflow(int page) {
    cursorPositionCenter(2, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(3, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(4, 92);
    printf("\xb2                                        " BOLD GREEN ITALIC "CASHFLOW" RESET "                                        \xb2\n");
    cursorPositionCenter(5, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(6, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                   \xb2          \xb2                         \xb2            \xb2                  \xb2\n");

    cursorPositionCenter(7, 92);
    printf("\xb2   Purchase Date   \xb2    ID    \xb2        Item Name        \xb2  Quantity  \xb2     Earnings     \xb2\n");

    int dataShown = 0;
    int count = 0;

    if(page != 1) {
        dataShown = page * 10 - 10;
    }

    struct data cashflow[100];
    int totalProduct = 0;
    int totalEarnings = 0;

    importCashflow(cashflow, &totalProduct);

    for(int i = 0; i < totalProduct; i++) {
        totalEarnings += cashflow[i].stock * cashflow[i].price;
    }

    for(int i = dataShown; i < totalProduct; i++) {
        cursorPositionCenter(10 + (count * 2), 92);
        printf("\xb2    %-10s     \xb2  [%-4s]  \xb2  %-20s   \xb2     %-3d    \xb2   Rp %-10d  \xb2\n", cashflow[i].expiryDate, cashflow[i].itemID, cashflow[i].itemName, cashflow[i].stock, cashflow[i].stock * cashflow[i].price);
        cursorPositionCenter(11 + (count * 2), 92);
        printf("\xb2                   \xb2          \xb2                         \xb2            \xb2                  \xb2\n");
        dataShown++;
        count++;
        if (dataShown % 10 == 0) break;
    }

    cursorPositionCenter(10 + (count * 2), 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(11 + (count * 2), -16);
    printf("\xb2                                  \xb2\n");
    cursorPositionCenter(12 + (count * 2), -16);
    printf("\xb2  " BOLD GREEN "Total Earnings : Rp %-11d " RESET "\xb2\n", totalEarnings);
    cursorPositionCenter(13 + (count * 2), -16);
    printf("\xb2                                  \xb2\n");
    cursorPositionCenter(14 + (count * 2), -16);
    for (int i = 0; i < 36; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(11 + (count * 2), 92);
    printf("\xb2 Page %d of %.0f \xb2\n", page, ceil(((float)totalProduct) / 10));
    cursorPositionCenter(12 + (count * 2), 92);
    for(int i = 0; i < 15; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(17 + (count * 2), 96);
    printf("Use arrow keys (" GREEN "Left" RESET " / " GREEN "Right" RESET ") to navigate through the pages and press " GREEN "Enter" RESET " to go back to menu.\n");
}

void cashflowControl() {
    int page = 1;
    int ch;

    struct data check[100];
    int totalProduct = 0;

    importCashflow(check, &totalProduct);

    while (1) {
        system("cls");
        today();
        cashflow(page);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 75: // Left
                    if(page > 1) page--;
                    break;
                case 77: // Right
                    if((float)page < ((float)totalProduct / 10)) page++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            // Enter
            return;
        }
    }
}

void hashTableView(int product) {
    cursorPositionCenter(2, 132);
    for(int i = 0; i < 130; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(3, 132);
    printf("\xb2                                                                                                                                \xb2\n");
    cursorPositionCenter(4, 132);
    printf("\xb2                                                         " BOLD GREEN ITALIC  "Hash-Table" RESET BOLD " View" RESET "                                                        \xb2\n");
    cursorPositionCenter(5, 132);
    printf("\xb2                                                                                                                                \xb2\n");
    cursorPositionCenter(6, 132);
    for(int i = 0; i < 130; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(7, 132);
    printf("\xb2 %-5s \xb2                                                      Products Data                                                     \xb2\n", "Index");
    cursorPositionCenter(8, 132);
    for(int i = 0; i < 130; i++) {
        printf("\xb2");
    }
    printf("\n");

    clearHash();
    int totalProduct = 0;
    extractFile(&totalProduct);

    if(product == 1) {
        for (int i = 0; i < (TABLESIZE / 2); i++) {
            cursorPositionCenter(9 + i, 132);
            printf("\xb2 [%-3d] \xb2 ", i);
            struct hashTable* current = table[i];
            while (current != NULL) {
                printf("(%s)%s -> ", current->itemID, current->itemName);
                current = current->next;
            }
            printf("NULL\n");
            cursorPositionCenter(9 + i, -126);
            printf("\xb2\n", i);
        }
        cursorPositionCenter(35, 132);
        printf("\xb2 Page %d of 2 \xb2\n", product);
    } else if(product == 2) {
        int position = 0;
        for (int i = (TABLESIZE / 2); i < TABLESIZE; i++) {
            cursorPositionCenter(9 + position, 132);
            printf("\xb2 [%-3d] \xb2 ", i);
            struct hashTable* current = table[i];
            while (current != NULL) {
                printf("(%s)%s -> ", current->itemID, current->itemName);
                current = current->next;
            }
            printf("NULL\n");
            cursorPositionCenter(9 + position, -126);
            printf("\xb2\n", i);
            position++;
        }
        cursorPositionCenter(35, 132);
        printf("\xb2 Page %d of 2 \xb2\n", product);
    }

    cursorPositionCenter(34, 132);
    for(int i = 0; i < 130; i++) {
        printf("\xb2");
    }
    printf("\n");

    cursorPositionCenter(36, 132);
    for(int i = 0; i < 15; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(36, 84);
    printf("Use arrow keys (" GREEN "Left" RESET " / " GREEN "Right" RESET ") to navigate through the pages and press " GREEN "Enter" RESET " to go back to menu.\n");
}

void hashTableControl() {
    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    int product = 1;
    int ch;

    while (1) {
        system("cls");
        today();
        hashTableView(product);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 75: // Left
                    if(product > 1) product--;
                    break;
                case 77: // Right
                    if(product < 2) product++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            // Enter
            return;
        }
    }
}

void checkExpiration() {
    system("cls");
    today();

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                    " BOLD GREEN ITALIC  "CHECK EXPIRATION" RESET "                                    \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(21, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    char id[5];

    cursorPositionCenter(16, 88);
    printf("Status: ");

    cursorPositionCenter(18, 88);
    printf("Expiration Info: ");

    while (1) {
        cursorPositionCenter(14, 88);
        printf("Input item ID [A-Z][A-Z][1-9][1-9] >> ");
        cursorPositionCenter(14, 12);
        scanf(" %[^\n]", id);

        clearInputBuffer();

        if (validateID(id)) {
            break;
        }

        cursorPositionCenter(14, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 72);
        printf(RED "Invalid ID format! Please try again." RESET "\n");
    }

    int foundAt = -1;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                foundAt = i; // Ditemukan
            }
            node = node->next;
        }
        if (foundAt != -1) break;
    }

    if (foundAt != -1) {
        char dateNow[15];
        cursorPositionCenter(16, 72);
        printf("ID %s found in the database at index %d.\n", id, foundAt);
        struct hashTable *current = table[foundAt];
        while(strcmp(current->itemID, id) != 0) {
            current = current->next;
        }
        todayDate(dateNow);
        struct date now = parseDate(dateNow);
        struct date product = parseDate(current->expiryDate);

        int comparisonResult = compareDates(now, product);

        if (comparisonResult < 0) {
            cursorPositionCenter(18, 54);
            printf(GREEN "%s is not yet expired." RESET "\n", current->itemName);
        } else if (comparisonResult > 0) {
            cursorPositionCenter(18, 54);
            printf(RED "%s is already expired." RESET "\n", current->itemName);
            cursorPositionCenter(19, 54);
            printf("Delete this product as soon as possible!\n");
        } else {
            cursorPositionCenter(18, 54);
            printf(BLUE "Today is the last day before %s expired." RESET "\n", current->itemName);
        }
    } else {
        cursorPositionCenter(16, 72);
        printf("ID %s not found in the database. Please try again.\n", id);
    }

    cursorPositionCenter(23, 34);
    system("pause");
}

// Fungsi utama untuk kasir
void cashier() {
    system("cls");

    if(login(3) == 1) {
        int opt;
        while(1) {
            cashierMenu(&opt);

            if(opt == 1) {
                struct data purchase[20];
                int totalPurchase = 0;
                purchaseProduct(purchase, &totalPurchase);
                if(totalPurchase != 0) purchaseSummary(purchase, totalPurchase);
            } else if(opt == 2) {
                cashflowControl();
            } else if(opt == 3) {
                hashTableControl();
            } else if(opt == 4) {
                checkExpiration();
            } else if(opt == 5) {
                return;
            }
        }
    } else {
        return;
    }
}

void printStaffMenu(int option) {
    cursorPositionCenter(20, 42);
    for (int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(21, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(22, 42);
    printf("\xb2              " BOLD GREEN ITALIC "STAFF MENU" RESET "              \xb2\n");
    cursorPositionCenter(23, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(24, 42);
    for (int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(25, 42);
    printf("\xb2                                      \xb2\n");

    if (option == 1) {
        cursorPositionCenter(26, 42);
        printf("\xb2" BLUE "   -> 1. Add Item                <-   " RESET "\xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 2) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2" BLUE "   -> 2. Add Stock               <-   " RESET "\xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 3) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2" BLUE "   -> 3. Delete Stock            <-   " RESET "\xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 4) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2" BLUE "   -> 4. Hash-Table View         <-   " RESET "\xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 5) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2" BLUE "   -> 5. Check Expiration        <-   " RESET "\xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 6) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2" BLUE "   -> 6. Update Data             <-   " RESET "\xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2   7. Logout                          \xb2\n");
    } else if (option == 7) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Add Item                        \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Add Stock                       \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Delete Stock                    \xb2\n");
        cursorPositionCenter(29, 42);
        printf("\xb2   4. Hash-Table View                 \xb2\n");
        cursorPositionCenter(30, 42);
        printf("\xb2   5. Check Expiration                \xb2\n");
        cursorPositionCenter(31, 42);
        printf("\xb2   6. Update Data                     \xb2\n");
        cursorPositionCenter(32, 42);
        printf("\xb2" BLUE "   -> 7. Logout                  <-   " RESET "\xb2\n");
    }

    cursorPositionCenter(33, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(34, 42);
    for (int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(36, 76);
    printf("Use arrow keys (" GREEN "Up" RESET " / " GREEN "Down" RESET ") to navigate and press " GREEN "Enter" RESET " to select an option.\n");
}

// Menu pada bagian warehouse staff
void staffMenu(int *opt) {
    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    int option = 1;
    int product = 1;
    int ch;

    while (1) {
        system("cls");
        today();
        printProductOverview(product);
        printStaffMenu(option);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72: // Up
                    if(option > 1) option--;
                    break;
                case 80: // Down
                    if(option < 7) option++;
                    break;
                case 75: // Left
                    if(product > 1) product--;
                    break;
                case 77: // Right
                    if((float)product < ((float)totalProduct / 10)) product++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            // Enter
            *opt = option;
            return;
        }
    }
}

void addItem() {
    system("cls");
    today();

    char dateNow[15];
    todayDate(dateNow);

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                        " BOLD GREEN ITALIC "ADD ITEM" RESET "                                        \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(13, 92);
    printf("\xb2                                                                                        \xb2\n");
    for (int i = 0; i < 12; i++) {
        cursorPositionCenter(14 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 12; i++) {
        cursorPositionCenter(14 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(24, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(25, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(26, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    cursorPositionCenter(14, 88);
    printf("Input new product name (3 - 15 Characters) >> ");
    cursorPositionCenter(16, 88);
    printf("Input starting stocks [1-100] >> ");
    cursorPositionCenter(18, 88);
    printf("Input price (Min. Value: 1000) >> ");
    cursorPositionCenter(20, 88);
    printf("Input expiry date (After %s) [Format: DD/MM/YYYY] >> ", dateNow);
    cursorPositionCenter(22, 88);
    printf("Generated ID:\n");
    cursorPositionCenter(25, 88);
    printf("Status:\n");

    char name[15];
    int stock;
    long int price;
    char expDate[15];

    do {
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(14, 88);
        printf(BLUE "Input new product name (3 - 15 Characters) >> " RESET);
        cursorPositionCenter(14, -4);
        scanf(" %[^\n]", name);
    } while(!validateName(name));

    cursorPositionCenter(14, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(14, 88);
    printf("Input new product name (3 - 15 Characters) >> %s\n", name);

    do {
        cursorPositionCenter(16, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 88);
        printf(BLUE "Input starting stocks [1-100] >> " RESET);
        cursorPositionCenter(16, 22);
        scanf(" %d", &stock);
    } while(!validateStock(stock));

    cursorPositionCenter(16, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(16, 88);
    printf("Input starting stocks [1-100] >> %d\n", stock);

    do {
        cursorPositionCenter(18, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(18, 88);
        printf(BLUE "Input price (Min. Value: 1000) >> " RESET);
        scanf(" %ld", &price);
    } while(!validatePrice(price, 25));

    cursorPositionCenter(18, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(18, 88);
    printf("Input price (Min. Value: 1000) >> %d\n", price);

    do {
        cursorPositionCenter(20, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(20, 88);
        printf(BLUE "Input expiry date (After %s) [Format: DD/MM/YYYY] >> " RESET, dateNow);
        cursorPositionCenter(20, -34);
        scanf(" %[^\n]", expDate);
    } while(!validateExpDate(expDate, 25));

    cursorPositionCenter(20, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(20, 88);
    printf("Input expiry date (After %s) [Format: DD/MM/YYYY] >> ", dateNow);

    char id[5];
    generateId(name, id);

    insert(id, name, stock, price, expDate);

    saveToFile();

    cursorPositionCenter(22, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(22, 88);
    printf(BLUE "Generated ID: %s" RESET "\n", id);

    cursorPositionCenter(25, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(25, 88);
    printf("Status: " GREEN "Product added to database!" RESET "\n");

    cursorPositionCenter(28, 34);
    system("pause");
}

void addStock() {
    system("cls");
    today();

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                        " BOLD GREEN ITALIC "ADD STOCK" RESET "                                       \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(21, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    char id[5];

    cursorPositionCenter(16, 88);
    printf("Status: ");

    cursorPositionCenter(18, 88);
    printf("Stock Info: ");

    while (1) {
        cursorPositionCenter(14, 88);
        printf("Input item ID [A-Z][A-Z][1-9][1-9] >> ");
        cursorPositionCenter(14, 12);
        scanf(" %[^\n]", id);

        clearInputBuffer();

        if (validateID(id)) {
            break;
        }

        cursorPositionCenter(14, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 72);
        printf(RED "Invalid ID format! Please try again." RESET "\n");
    }

    int foundAt = -1;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                foundAt = i; // Ditemukan
            }
            node = node->next;
        }
        if (foundAt != -1) break;
    }

    if (foundAt != -1) {
        int newStock;
        cursorPositionCenter(16, 72);
        printf("ID %s found in the database at index %d.\n", id, foundAt);
        struct hashTable *current = table[foundAt];
        while(strcmp(current->itemID, id) != 0) {
            current = current->next;
        }

        while(1) {
            cursorPositionCenter(14, 88);
            printf(CLEAR_LINE);
            cursorPositionCenter(14, 92);
            printf("\xb2                                                                                        \xb2\n");

            cursorPositionCenter(14, 88);
            printf("Input total stock to add [1-100] >> ");
            scanf(" %d", &newStock);

            clearInputBuffer();

            if(newStock > 0 && newStock < 101) {
                break;
            }

            cursorPositionCenter(16, 72);
            printf(RED "Invalid stock! Please try again." RESET "\n");
        }

        char date[15];
        todayDate(date);

        current->stock += newStock;

        cursorPositionCenter(18, 64);
        printf("%s stock added successfully on %s!\n", current->itemName, date);
        cursorPositionCenter(19, 64);
        printf("New stock: %d\n", current->stock);


        saveToFile();
    } else {
        cursorPositionCenter(16, 72);
        printf("ID %s not found in the database. Please try again.\n", id);
    }
    
    cursorPositionCenter(23, 34);
    system("pause");
}

void deleteStock() {
    system("cls");
    today();

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                      " BOLD GREEN ITALIC "DELETE STOCK" RESET "                                      \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 8; i++) {
        cursorPositionCenter(13 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(21, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    char id[5];

    cursorPositionCenter(16, 88);
    printf("Status: ");

    cursorPositionCenter(18, 88);
    printf("Stock Info: ");

    while (1) {
        cursorPositionCenter(14, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(14, 88);
        printf("Input item ID [A-Z][A-Z][1-9][1-9] >> ");
        cursorPositionCenter(14, 12);
        scanf(" %[^\n]", id);

        clearInputBuffer();

        if (validateID(id)) {
            break;
        }

        cursorPositionCenter(16, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(16, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 88);
        printf("Status: ");
        cursorPositionCenter(16, 72);
        printf(RED "Invalid ID format! Please try again." RESET "\n");
    }

    int foundAt = -1;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                foundAt = i; // Ditemukan
            }
            node = node->next;
        }
        if (foundAt != -1) break;
    }

    if (foundAt != -1) {
        int deleteStock;
        cursorPositionCenter(16, 72);
        printf("ID %s found in the database at index %d.\n", id, foundAt);
        struct hashTable *current = table[foundAt];
        while(strcmp(current->itemID, id) != 0) {
            current = current->next;
        }

        while(1) {
            cursorPositionCenter(14, 88);
            printf(CLEAR_LINE);
            cursorPositionCenter(14, 92);
            printf("\xb2                                                                                        \xb2\n");

            cursorPositionCenter(14, 88);
            printf("Input total stock to delete [1-%d] >> ", current->stock);
            scanf(" %d", &deleteStock);

            clearInputBuffer();

            if(deleteStock > 0 && deleteStock < current->stock + 1) {
                break;
            }

            cursorPositionCenter(16, 88);
            printf(CLEAR_LINE);
            cursorPositionCenter(16, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(16, 88);
            printf("Status: ");
            cursorPositionCenter(16, 72);
            printf(RED "Invalid stock! Please try again." RESET "\n");
        }

        char date[15];
        todayDate(date);

        current->stock -= deleteStock;

        if(current->stock <= 0) {

            struct hashTable *node = table[foundAt];
            struct hashTable *prev = NULL;

            while (node != NULL && strcmp(node->itemID, id) != 0) {
                prev = node;
                node = node->next;
            }

            if (node == NULL) {
                return; 
            }

            if (prev == NULL) {
                table[foundAt] = node->next;
            } else {
                prev->next = node->next;
            }

            free(node);
            cursorPositionCenter(18, 64);
            printf("%s deleted from database successfully on %s!\n", current->itemName, date);
        } else {
            cursorPositionCenter(18, 64);
            printf("%s stock deleted successfully on %s!\n", current->itemName, date);
            cursorPositionCenter(19, 64);
            printf("New stock: %d\n", current->stock);
        }

        saveToFile();
    } else {
        cursorPositionCenter(16, 72);
        printf("ID %s not found in the database. Please try again.\n", id);
    }
    
    cursorPositionCenter(23, 34);
    system("pause");
}

void printUpdateMenu(int option) {
    cursorPositionCenter(20, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(21, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(22, 42);
    printf("\xb2           " BOLD GREEN ITALIC "UPDATE DATA MENU" RESET "           \xb2\n");
    cursorPositionCenter(23, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(24, 42);
    for(int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(25, 42);
    printf("\xb2                                      \xb2\n");

    if(option == 1) {
        cursorPositionCenter(26, 42);
        printf("\xb2" BLUE "   -> 1. Update Price            <-   " RESET "\xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Update Expiry Date              \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Go Back                         \xb2\n");
    } else if(option == 2) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Update Price                    \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2" BLUE "   -> 2. Update Expiry Date      <-   " RESET "\xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2   3. Go Back                         \xb2\n");
    } else if(option == 3) {
        cursorPositionCenter(26, 42);
        printf("\xb2   1. Update Price                    \xb2\n");
        cursorPositionCenter(27, 42);
        printf("\xb2   2. Update Expiry Date              \xb2\n");
        cursorPositionCenter(28, 42);
        printf("\xb2" BLUE "   -> 3. Go Back                 <-   " RESET "\xb2\n");
    }

    cursorPositionCenter(29, 42);
    printf("\xb2                                      \xb2\n");
    cursorPositionCenter(30, 42);
    for (int i = 0; i < 40; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(32, 76);
    printf("Use arrow keys (" GREEN "Up" RESET " / " GREEN "Down" RESET ") to navigate and press " GREEN "Enter" RESET " to select an option.\n");
}

void updatePrice() {
    system("cls");
    today();

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                    " BOLD GREEN ITALIC  "UPDATE DATA PRICE" RESET "                                   \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    for (int i = 0; i < 9; i++) {
        cursorPositionCenter(13 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 9; i++) {
        cursorPositionCenter(13 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(22, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    char id[5];

    cursorPositionCenter(16, 88);
    printf("Input new item price (Min. Value: 1000) >> ");

    cursorPositionCenter(18, 88);
    printf("Status: ");

    cursorPositionCenter(20, 88);
    printf("Update Info: ");

    while (1) {
        cursorPositionCenter(14, 88);
        printf(BLUE "Input item ID [A-Z][A-Z][1-9][1-9] >> " RESET);
        cursorPositionCenter(14, 12);
        scanf(" %[^\n]", id);

        clearInputBuffer();

        if (validateID(id)) {
            break;
        }

        cursorPositionCenter(14, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(18, 72);
        printf(RED "Invalid ID format! Please try again." RESET "\n");
    }

    cursorPositionCenter(14, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(14, 88);
    printf("Input item ID [A-Z][A-Z][1-9][1-9] >> %s", id);

    int foundAt = -1;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                foundAt = i; // Ditemukan
            }
            node = node->next;
        }
        if (foundAt != -1) break;
    }

    if (foundAt != -1) {
        long int newPrice;
        cursorPositionCenter(18, 72);
        printf(GREEN "ID %s found in the database at index %d." RESET "\n", id, foundAt);
        struct hashTable *current = table[foundAt];
        while(strcmp(current->itemID, id) != 0) {
            current = current->next;
        }
        
        cursorPositionCenter(20, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(20, 88);
        printf("Update Info: %s current price is Rp %ld\n", current->itemName, current->price);

        do {
            cursorPositionCenter(16, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(16, 88);
            printf(BLUE "Input new %s price (Min. Value: 1000) >> " RESET, current->itemName);
            scanf(" %ld", &newPrice);
        } while(!validatePrice(newPrice, 18));

        current->price = newPrice;

        saveToFile();

        cursorPositionCenter(16, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 88);
        printf("Input new %s price (Min. Value: 1000) >> %ld\n", current->itemName, newPrice);
        cursorPositionCenter(20, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(20, 88);
        printf("Update Info: %s new price is Rp %ld\n", current->itemName, newPrice);
        cursorPositionCenter(18, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(18, 88);
        printf("Status: " GREEN "%s price updated in the database!" RESET "\n", current->itemName);
    } else {
        cursorPositionCenter(18, 72);
        printf(RED "ID %s not found in the database. Please try again." RESET "\n", id);
    }

    cursorPositionCenter(25, 34);
    system("pause");
}

void updateExpiryDate() {
    system("cls");
    today();

    cursorPositionCenter(8, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    cursorPositionCenter(9, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(10, 92);
    printf("\xb2                                 " BOLD GREEN ITALIC  "UPDATE DATA EXPIRY DATE" RESET "                                \xb2\n");
    cursorPositionCenter(11, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(12, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }
    for (int i = 0; i < 9; i++) {
        cursorPositionCenter(13 + i, 92);
        printf("\xb2\n");
    }
    for (int i = 0; i < 9; i++) {
        cursorPositionCenter(13 + i, -86);
        printf("\xb2\n");
    }
    cursorPositionCenter(22, 92);
    for (int i = 0; i < 90; i++) {
        printf("\xb2");
    }

    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    char id[5];

    char dateNow[15];
    todayDate(dateNow);

    cursorPositionCenter(16, 88);
    printf("Input new item expiry date (After %s) [DD/MM/YYYY] >> ", dateNow);

    cursorPositionCenter(18, 88);
    printf("Status: ");

    cursorPositionCenter(20, 88);
    printf("Update Info: ");

    while (1) {
        cursorPositionCenter(14, 88);
        printf(BLUE "Input item ID [A-Z][A-Z][1-9][1-9] >> " RESET);
        cursorPositionCenter(14, 12);
        scanf(" %[^\n]", id);

        clearInputBuffer();

        if (validateID(id)) {
            break;
        }

        cursorPositionCenter(14, 88);
        printf(CLEAR_LINE);
        cursorPositionCenter(14, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(18, 72);
        printf(RED "Invalid ID format! Please try again." RESET "\n");
    }

    cursorPositionCenter(14, 92);
    printf("\xb2                                                                                        \xb2\n");
    cursorPositionCenter(14, 88);
    printf("Input item ID [A-Z][A-Z][1-9][1-9] >> %s", id);

    int foundAt = -1;

    for(int i = 0; i < TABLESIZE; i++) {
        struct hashTable *node = table[i];
        while (node) {
            if (strcmp(node->itemID, id) == 0) {
                foundAt = i; // Ditemukan
            }
            node = node->next;
        }
        if (foundAt != -1) break;
    }

    if (foundAt != -1) {
        char newDate[15];
        cursorPositionCenter(18, 72);
        printf(GREEN "ID %s found in the database at index %d." RESET "\n", id, foundAt);
        struct hashTable *current = table[foundAt];
        while(strcmp(current->itemID, id) != 0) {
            current = current->next;
        }
        
        cursorPositionCenter(20, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(20, 88);
        printf("Update Info: %s current expiry date is %s\n", current->itemName, current->expiryDate);

        do {
            cursorPositionCenter(16, 92);
            printf("\xb2                                                                                        \xb2\n");
            cursorPositionCenter(16, 88);
            printf(BLUE "Input new %s expiry date (After %s) [DD/MM/YYYY] >> " RESET, current->itemName, dateNow);
            scanf(" %[^\n]", newDate);
        } while(!validateExpDate(newDate, 18));

        strcpy(current->expiryDate, newDate);

        saveToFile();

        cursorPositionCenter(16, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(16, 88);
        printf("Input new %s expiry date (After %s) [DD/MM/YYYY] >> %s", current->itemName, dateNow, newDate);
        cursorPositionCenter(20, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(20, 88);
        printf("Update Info: %s new expiry date is %s\n", current->itemName, newDate);
        cursorPositionCenter(18, 92);
        printf("\xb2                                                                                        \xb2\n");
        cursorPositionCenter(18, 88);
        printf("Status: " GREEN "%s expiry date updated in the database!" RESET "\n", current->itemName);
    } else {
        cursorPositionCenter(18, 72);
        printf(RED "ID %s not found in the database. Please try again." RESET "\n", id);
    }

    cursorPositionCenter(25, 34);
    system("pause");
}

void updateData() {
    int totalProduct = 0;
    clearHash();
    extractFile(&totalProduct);
    int option = 1;
    int product = 1;
    int ch;

    while (1) {
        system("cls");
        today();
        printProductOverview(product);
        printUpdateMenu(option);

        ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72: // Up
                    if(option > 1) option--;
                    break;
                case 80: // Down
                    if(option < 3) option++;
                    break;
                case 75: // Left
                    if(product > 1) product--;
                    break;
                case 77: // Right
                    if((float)product < ((float)totalProduct / 10)) product++;
                    break;
                default:
                    break;
            }
        } else if (ch == 13) {
            if(option == 1) {
                updatePrice();
                break;
            } else if(option == 2) {
                updateExpiryDate();
                break;
            } else if(option == 3) {
                break;
            }
        }
    }
}

// Fungsi utama untuk warehouse staff
void staff() {
    system("cls");

    if(login(3) == 1) {
        int opt;
        while(1) {
            staffMenu(&opt);

            if(opt == 1) {
                addItem();
            } else if(opt == 2) {
                addStock();
            } else if(opt == 3) {
                deleteStock();
            } else if(opt == 4) {
                hashTableControl();
            } else if(opt == 5) {
                checkExpiration();
            } else if(opt == 6) {
                updateData();
            } else if(opt == 7) {
                return;
            }
        }
    } else {
        return;
    }
}

void printExitChoice(int option) {
    if(option == 1) {
        cursorPositionCenter(23, 46);
        printf(BOLD BLUE "[ Yes ]" RESET "\n");
        cursorPositionCenter(24, 46);
        printf("[ No ]\n");
    } else if(option == 2) {
        cursorPositionCenter(23, 46);
        printf("[ Yes ]\n");
        cursorPositionCenter(24, 46);
        printf(BOLD BLUE "[ No ]" RESET "\n");
    }
}

// Opsi untuk menutup aplikasi


int main() {
    clearHash();
    intro();
    do {
        int opt = loginPage();

        if(opt == 1) {
            cashier();
        } else if(opt == 2) {
            staff();
        } else if(opt == 3) {
            if (!exitProgram()) return 0;
        }
    } while (1);
    
    return 0;
}

// halo ini easter egg untuk update file project



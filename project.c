/*
   ST INDOOR ZONE - Gaming Zone Management System
   Language : C (Code::Blocks, Windows)
   Files    : users.dat, stations.dat, customers.dat, sessions.dat
   Default admin account: admin / 1234
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define MIN_MINUTES 15

/* ===================== STRUCTS ===================== */

struct User {
    char username[30];
    char password[30];
    int role;               /* 1 = Admin, 2 = Staff */
};

struct Station {
    int id;
    char name[30];
    char type[15];          /* PC or Console */
    float rate;             /* Tk per hour */
    int busy;               /* 0 = Free, 1 = Busy */
};

struct Customer {
    int id;
    char name[30];
    char phone[15];
};

struct Session {
    int id;                 /* also used as bill number */
    int stationId;
    int customerId;
    time_t startTime;
    time_t endTime;
    int minutes;
    float rate;
    float total;
    int active;             /* 1 = running, 0 = finished */
};

/* ===================== UI HELPERS ===================== */

void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawBox(int x, int y, int w, int h) {
    int i;

    gotoxy(x, y);
    printf("%c", 201);
    i = 0;
    while (i < w - 2) {
        printf("%c", 205);
        i++;
    }
    printf("%c", 187);

    i = 1;
    while (i < h - 1) {
        gotoxy(x, y + i);
        printf("%c", 186);
        gotoxy(x + w - 1, y + i);
        printf("%c", 186);
        i++;
    }

    gotoxy(x, y + h - 1);
    printf("%c", 200);
    i = 0;
    while (i < w - 2) {
        printf("%c", 205);
        i++;
    }
    printf("%c", 188);
}

void showMessage(int x, int y, int color, char msg[]) {
    setColor(color);
    gotoxy(x, y);
    printf("%s", msg);
    setColor(7);
    getch();
}

void pauseMsg(int color, char msg[]) {
    setColor(color);
    printf("\n  %s\n", msg);
    setColor(7);
    printf("  Press any key to continue...");
    getch();
}

void tableHeader(char title[]) {
    system("cls");
    setColor(11);
    gotoxy(28, 0);
    printf("%s\n\n", title);
}

/* ===================== SAFE INPUT ===================== */

void readLine(char s[], int n) {
    char buf[100];
    int len;

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            s[0] = '\0';
            return;
        }
        len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
        if (strlen(buf) > 0) {
            strncpy(s, buf, n - 1);
            s[n - 1] = '\0';
            return;
        }
    }
}

int readInt() {
    char buf[50];
    int v;

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return 0;
        }
        if (sscanf(buf, "%d", &v) == 1) {
            return v;
        }
    }
}

float readFloat() {
    char buf[50];
    float v;

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return 0;
        }
        if (sscanf(buf, "%f", &v) == 1) {
            return v;
        }
    }
}

void getPassword(char pass[]) {
    int i = 0;
    char ch;

    while (1) {
        ch = getch();
        if (ch == 13) {
            break;
        }
        if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < 29) {
            pass[i] = ch;
            i++;
            printf("*");
        }
    }
    pass[i] = '\0';
}

/* ===================== REGISTER AND LOGIN ===================== */

void createDefaultAdmin() {
    FILE *fp;
    struct User u;

    fp = fopen("users.dat", "rb");
    if (fp != NULL) {
        fclose(fp);
        return;
    }

    fp = fopen("users.dat", "wb");
    strcpy(u.username, "admin");
    strcpy(u.password, "1234");
    u.role = 1;
    fwrite(&u, sizeof(struct User), 1, fp);
    fclose(fp);
}

int userExists(char name[]) {
    FILE *fp;
    struct User u;

    fp = fopen("users.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&u, sizeof(struct User), 1, fp) == 1) {
        if (strcmp(u.username, name) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void registerUser() {
    struct User u;
    FILE *fp;

    system("cls");
    setColor(11);
    drawBox(20, 3, 40, 12);
    gotoxy(33, 4);
    printf("REGISTER");
    setColor(7);

    gotoxy(24, 7);
    printf("Username: ");
    readLine(u.username, 30);

    if (userExists(u.username)) {
        showMessage(24, 12, 12, "Username already taken.");
        return;
    }

    gotoxy(24, 9);
    printf("Password: ");
    getPassword(u.password);
    u.role = 2;

    fp = fopen("users.dat", "ab");
    fwrite(&u, sizeof(struct User), 1, fp);
    fclose(fp);

    showMessage(24, 12, 10, "Account created. Press any key.");
}

int loginUser(struct User *current) {
    struct User u;
    char name[30], pass[30];
    FILE *fp;
    int found = 0;

    system("cls");
    setColor(11);
    drawBox(20, 3, 40, 12);
    gotoxy(35, 4);
    printf("LOGIN");
    setColor(7);

    gotoxy(24, 7);
    printf("Username: ");
    readLine(name, 30);
    gotoxy(24, 9);
    printf("Password: ");
    getPassword(pass);

    fp = fopen("users.dat", "rb");
    if (fp != NULL) {
        while (fread(&u, sizeof(struct User), 1, fp) == 1) {
            if (strcmp(u.username, name) == 0 && strcmp(u.password, pass) == 0) {
                *current = u;
                found = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (found) {
        showMessage(24, 12, 10, "Login successful!");
    } else {
        showMessage(24, 12, 12, "Wrong username or password.");
    }
    return found;
}

/* ===================== STATION CRUD ===================== */

int getNextStationId() {
    FILE *fp;
    struct Station s;
    int maxId = 100;

    fp = fopen("stations.dat", "rb");
    if (fp == NULL) {
        return 101;
    }

    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (s.id > maxId) {
            maxId = s.id;
        }
    }
    fclose(fp);
    return maxId + 1;
}

int getStationById(int id, struct Station *out) {
    FILE *fp;
    struct Station s;

    fp = fopen("stations.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (s.id == id) {
            *out = s;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void updateStation(struct Station st) {
    FILE *fp;
    struct Station s;

    fp = fopen("stations.dat", "rb+");
    if (fp == NULL) {
        return;
    }

    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (s.id == st.id) {
            fseek(fp, -(long)sizeof(struct Station), SEEK_CUR);
            fwrite(&st, sizeof(struct Station), 1, fp);
            break;
        }
    }
    fclose(fp);
}

int showStationTable(int onlyFree) {
    FILE *fp;
    struct Station s;
    int count = 0;

    tableHeader("STATION LIST");
    printf("  ID    Name            Type      Rate/hr   Status\n");
    printf("  ---------------------------------------------------\n");
    setColor(7);

    fp = fopen("stations.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (onlyFree == 1 && s.busy == 1) {
            continue;
        }
        printf("  %-6d%-16s%-10s%-10.0f", s.id, s.name, s.type, s.rate);
        if (s.busy == 1) {
            setColor(12);
            printf("Busy");
        } else {
            setColor(10);
            printf("Free");
        }
        setColor(7);
        printf("\n");
        count++;
    }
    fclose(fp);
    return count;
}

void addStation() {
    struct Station s;
    FILE *fp;
    int t = 0;

    system("cls");
    setColor(11);
    drawBox(15, 2, 50, 14);
    gotoxy(30, 3);
    printf("ADD STATION");
    setColor(7);

    s.id = getNextStationId();
    gotoxy(19, 5);
    printf("Station ID : %d", s.id);

    gotoxy(19, 7);
    printf("Name       : ");
    readLine(s.name, 30);

    while (t != 1 && t != 2) {
        gotoxy(19, 8);
        printf("Type (1=PC, 2=Console): ");
        t = readInt();
    }
    if (t == 1) {
        strcpy(s.type, "PC");
    } else {
        strcpy(s.type, "Console");
    }

    s.rate = 0;
    while (s.rate <= 0) {
        gotoxy(19, 9);
        printf("Rate per hour (Tk): ");
        s.rate = readFloat();
    }
    s.busy = 0;

    fp = fopen("stations.dat", "ab");
    fwrite(&s, sizeof(struct Station), 1, fp);
    fclose(fp);

    showMessage(19, 12, 10, "Station added. Press any key.");
}

void viewStations() {
    int count;

    count = showStationTable(0);
    if (count == 0) {
        pauseMsg(14, "No stations yet. Add one first.");
        return;
    }
    printf("\n  Press any key to go back...");
    getch();
}

void editStation() {
    FILE *fp;
    struct Station s;
    int count, id, t, found = 0;

    count = showStationTable(0);
    if (count == 0) {
        pauseMsg(14, "No stations to edit.");
        return;
    }

    printf("\n  Enter station ID to edit: ");
    id = readInt();

    fp = fopen("stations.dat", "rb+");
    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (s.id == id) {
            found = 1;

            printf("  New name: ");
            readLine(s.name, 30);

            t = 0;
            while (t != 1 && t != 2) {
                printf("  New type (1=PC, 2=Console): ");
                t = readInt();
            }
            if (t == 1) {
                strcpy(s.type, "PC");
            } else {
                strcpy(s.type, "Console");
            }

            s.rate = 0;
            while (s.rate <= 0) {
                printf("  New rate (Tk/hr): ");
                s.rate = readFloat();
            }

            fseek(fp, -(long)sizeof(struct Station), SEEK_CUR);
            fwrite(&s, sizeof(struct Station), 1, fp);
            break;
        }
    }
    fclose(fp);

    if (found) {
        pauseMsg(10, "Station updated.");
    } else {
        pauseMsg(12, "Station ID not found.");
    }
}

void deleteStation() {
    FILE *fp, *temp;
    struct Station s;
    int count, id, found = 0, blocked = 0;
    char ans[10];

    count = showStationTable(0);
    if (count == 0) {
        pauseMsg(14, "No stations to delete.");
        return;
    }

    printf("\n  Enter station ID to delete: ");
    id = readInt();

    printf("  Are you sure? (y/n): ");
    readLine(ans, 10);
    if (ans[0] != 'y' && ans[0] != 'Y') {
        pauseMsg(14, "Cancelled.");
        return;
    }

    fp = fopen("stations.dat", "rb");
    temp = fopen("temp.dat", "wb");

    while (fread(&s, sizeof(struct Station), 1, fp) == 1) {
        if (s.id == id) {
            if (s.busy == 1) {
                blocked = 1;
                fwrite(&s, sizeof(struct Station), 1, temp);
            } else {
                found = 1;
            }
        } else {
            fwrite(&s, sizeof(struct Station), 1, temp);
        }
    }
    fclose(fp);
    fclose(temp);

    remove("stations.dat");
    rename("temp.dat", "stations.dat");

    if (blocked) {
        pauseMsg(12, "Station is busy. Cannot delete.");
    } else if (found) {
        pauseMsg(10, "Station deleted.");
    } else {
        pauseMsg(12, "Station ID not found.");
    }
}

void stationMenu(struct User u) {
    int choice;

    while (1) {
        system("cls");
        setColor(11);
        drawBox(20, 3, 40, 13);
        gotoxy(30, 4);
        printf("MANAGE STATIONS");
        setColor(7);

        gotoxy(26, 7);
        printf("1. Add Station");
        gotoxy(26, 8);
        printf("2. View Stations");
        gotoxy(26, 9);
        printf("3. Edit Station");
        gotoxy(26, 10);
        printf("4. Delete Station");
        gotoxy(26, 11);
        printf("5. Back");
        gotoxy(26, 13);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            addStation();
        } else if (choice == 2) {
            viewStations();
        } else if (choice == 3) {
            editStation();
        } else if (choice == 4) {
            if (u.role == 1) {
                deleteStation();
            } else {
                showMessage(22, 14, 12, "Only admin can delete.");
            }
        } else if (choice == 5) {
            break;
        }
    }
}

/* ===================== CUSTOMER CRUD ===================== */

int getNextCustomerId() {
    FILE *fp;
    struct Customer c;
    int maxId = 1000;

    fp = fopen("customers.dat", "rb");
    if (fp == NULL) {
        return 1001;
    }

    while (fread(&c, sizeof(struct Customer), 1, fp) == 1) {
        if (c.id > maxId) {
            maxId = c.id;
        }
    }
    fclose(fp);
    return maxId + 1;
}

int getCustomerById(int id, struct Customer *out) {
    FILE *fp;
    struct Customer c;

    fp = fopen("customers.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&c, sizeof(struct Customer), 1, fp) == 1) {
        if (c.id == id) {
            *out = c;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int customerInActiveSession(int id) {
    FILE *fp;
    struct Session s;

    fp = fopen("sessions.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.active == 1 && s.customerId == id) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int showCustomerTable() {
    FILE *fp;
    struct Customer c;
    int count = 0;

    tableHeader("CUSTOMER LIST");
    printf("  ID      Name                  Phone\n");
    printf("  ---------------------------------------------\n");
    setColor(7);

    fp = fopen("customers.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&c, sizeof(struct Customer), 1, fp) == 1) {
        printf("  %-8d%-22s%-15s\n", c.id, c.name, c.phone);
        count++;
    }
    fclose(fp);
    return count;
}

int addCustomer() {
    struct Customer c;
    FILE *fp;

    system("cls");
    setColor(11);
    drawBox(15, 2, 50, 12);
    gotoxy(30, 3);
    printf("ADD CUSTOMER");
    setColor(7);

    c.id = getNextCustomerId();
    gotoxy(19, 5);
    printf("Customer ID : %d", c.id);
    gotoxy(19, 7);
    printf("Name        : ");
    readLine(c.name, 30);
    gotoxy(19, 8);
    printf("Phone       : ");
    readLine(c.phone, 15);

    fp = fopen("customers.dat", "ab");
    fwrite(&c, sizeof(struct Customer), 1, fp);
    fclose(fp);

    showMessage(19, 11, 10, "Customer added. Press any key.");
    return c.id;
}

void viewCustomers() {
    int count;

    count = showCustomerTable();
    if (count == 0) {
        pauseMsg(14, "No customers yet.");
        return;
    }
    printf("\n  Press any key to go back...");
    getch();
}

void editCustomer() {
    FILE *fp;
    struct Customer c;
    int count, id, found = 0;

    count = showCustomerTable();
    if (count == 0) {
        pauseMsg(14, "No customers to edit.");
        return;
    }

    printf("\n  Enter customer ID to edit: ");
    id = readInt();

    fp = fopen("customers.dat", "rb+");
    while (fread(&c, sizeof(struct Customer), 1, fp) == 1) {
        if (c.id == id) {
            found = 1;
            printf("  New name : ");
            readLine(c.name, 30);
            printf("  New phone: ");
            readLine(c.phone, 15);

            fseek(fp, -(long)sizeof(struct Customer), SEEK_CUR);
            fwrite(&c, sizeof(struct Customer), 1, fp);
            break;
        }
    }
    fclose(fp);

    if (found) {
        pauseMsg(10, "Customer updated.");
    } else {
        pauseMsg(12, "Customer ID not found.");
    }
}

void deleteCustomer() {
    FILE *fp, *temp;
    struct Customer c;
    int count, id, found = 0;
    char ans[10];

    count = showCustomerTable();
    if (count == 0) {
        pauseMsg(14, "No customers to delete.");
        return;
    }

    printf("\n  Enter customer ID to delete: ");
    id = readInt();

    if (customerInActiveSession(id)) {
        pauseMsg(12, "Customer has a running session.");
        return;
    }

    printf("  Are you sure? (y/n): ");
    readLine(ans, 10);
    if (ans[0] != 'y' && ans[0] != 'Y') {
        pauseMsg(14, "Cancelled.");
        return;
    }

    fp = fopen("customers.dat", "rb");
    temp = fopen("temp.dat", "wb");

    while (fread(&c, sizeof(struct Customer), 1, fp) == 1) {
        if (c.id == id) {
            found = 1;
        } else {
            fwrite(&c, sizeof(struct Customer), 1, temp);
        }
    }
    fclose(fp);
    fclose(temp);

    remove("customers.dat");
    rename("temp.dat", "customers.dat");

    if (found) {
        pauseMsg(10, "Customer deleted.");
    } else {
        pauseMsg(12, "Customer ID not found.");
    }
}

void customerMenu(struct User u) {
    int choice;

    while (1) {
        system("cls");
        setColor(11);
        drawBox(20, 3, 40, 13);
        gotoxy(30, 4);
        printf("MANAGE CUSTOMERS");
        setColor(7);

        gotoxy(26, 7);
        printf("1. Add Customer");
        gotoxy(26, 8);
        printf("2. View Customers");
        gotoxy(26, 9);
        printf("3. Edit Customer");
        gotoxy(26, 10);
        printf("4. Delete Customer");
        gotoxy(26, 11);
        printf("5. Back");
        gotoxy(26, 13);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            addCustomer();
        } else if (choice == 2) {
            viewCustomers();
        } else if (choice == 3) {
            editCustomer();
        } else if (choice == 4) {
            if (u.role == 1) {
                deleteCustomer();
            } else {
                showMessage(22, 14, 12, "Only admin can delete.");
            }
        } else if (choice == 5) {
            break;
        }
    }
}

/* ===================== SESSIONS ===================== */

int getNextSessionId() {
    FILE *fp;
    struct Session s;
    int maxId = 0;

    fp = fopen("sessions.dat", "rb");
    if (fp == NULL) {
        return 1;
    }

    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.id > maxId) {
            maxId = s.id;
        }
    }
    fclose(fp);
    return maxId + 1;
}

int showActiveSessions() {
    FILE *fp;
    struct Session s;
    struct Station st;
    struct Customer c;
    char sn[30], cn[30], tbuf[10];
    int count = 0, elapsed;

    tableHeader("ACTIVE SESSIONS");
    printf("  Stn   Station         Customer        Started   Elapsed\n");
    printf("  ------------------------------------------------------\n");
    setColor(7);

    fp = fopen("sessions.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.active == 1) {
            strcpy(sn, "Deleted");
            strcpy(cn, "Deleted");
            if (getStationById(s.stationId, &st)) {
                strcpy(sn, st.name);
            }
            if (getCustomerById(s.customerId, &c)) {
                strcpy(cn, c.name);
            }
            strftime(tbuf, sizeof(tbuf), "%H:%M", localtime(&s.startTime));
            elapsed = (int)(difftime(time(NULL), s.startTime) / 60);
            printf("  %-6d%-16s%-16s%-10s", s.stationId, sn, cn, tbuf);
            setColor(14);
            printf("%d min\n", elapsed);
            setColor(7);
            count++;
        }
    }
    fclose(fp);
    return count;
}

void showBill(struct Session s) {
    struct Station st;
    struct Customer c;
    char sn[30], cn[30], t1[10], t2[10], d[20];

    strcpy(sn, "Deleted");
    strcpy(cn, "Deleted");
    if (getStationById(s.stationId, &st)) {
        strcpy(sn, st.name);
    }
    if (getCustomerById(s.customerId, &c)) {
        strcpy(cn, c.name);
    }
    strftime(t1, sizeof(t1), "%H:%M", localtime(&s.startTime));
    strftime(t2, sizeof(t2), "%H:%M", localtime(&s.endTime));
    strftime(d, sizeof(d), "%d-%m-%Y", localtime(&s.startTime));

    system("cls");
    setColor(14);
    drawBox(18, 2, 44, 15);
    gotoxy(32, 3);
    printf("BILL RECEIPT");
    setColor(7);

    gotoxy(22, 5);
    printf("Bill No   : %d", s.id);
    gotoxy(22, 6);
    printf("Date      : %s", d);
    gotoxy(22, 7);
    printf("Customer  : %s", cn);
    gotoxy(22, 8);
    printf("Station   : %s", sn);
    gotoxy(22, 9);
    printf("Start     : %s", t1);
    gotoxy(22, 10);
    printf("End       : %s", t2);
    gotoxy(22, 11);
    printf("Time      : %dh %dm", s.minutes / 60, s.minutes % 60);
    gotoxy(22, 12);
    printf("Rate      : %.0f Tk/hr", s.rate);
    gotoxy(22, 13);
    printf("--------------------------");
    setColor(14);
    gotoxy(22, 14);
    printf("TOTAL     : %.0f Tk", s.total);
    setColor(7);

    gotoxy(22, 18);
    printf("Press any key...");
    getch();
}

void startSession() {
    struct Station st;
    struct Customer c;
    struct Session s;
    FILE *fp;
    int count, sid, cid;

    count = showStationTable(1);
    if (count == 0) {
        pauseMsg(14, "No free station available.");
        return;
    }

    printf("\n  Enter station ID to start: ");
    sid = readInt();
    if (!getStationById(sid, &st)) {
        pauseMsg(12, "Station ID not found.");
        return;
    }
    if (st.busy == 1) {
        pauseMsg(12, "Station is already busy.");
        return;
    }

    showCustomerTable();
    printf("\n  Enter customer ID (0 = add new customer): ");
    cid = readInt();
    if (cid == 0) {
        cid = addCustomer();
    } else if (!getCustomerById(cid, &c)) {
        pauseMsg(12, "Customer ID not found.");
        return;
    }

    s.id = getNextSessionId();
    s.stationId = sid;
    s.customerId = cid;
    s.startTime = time(NULL);
    s.endTime = 0;
    s.minutes = 0;
    s.rate = st.rate;
    s.total = 0;
    s.active = 1;

    fp = fopen("sessions.dat", "ab");
    fwrite(&s, sizeof(struct Session), 1, fp);
    fclose(fp);

    st.busy = 1;
    updateStation(st);

    tableHeader("SESSION STARTED");
    setColor(7);
    printf("  Station : %s\n", st.name);
    printf("  Rate    : %.0f Tk/hr\n", st.rate);
    pauseMsg(10, "Session started. Timer is running.");
}

void endSession() {
    FILE *fp;
    struct Session s;
    struct Station st;
    int count, sid, found = 0;
    long secs;

    count = showActiveSessions();
    if (count == 0) {
        pauseMsg(14, "No active sessions.");
        return;
    }

    printf("\n  Enter station ID to end: ");
    sid = readInt();

    fp = fopen("sessions.dat", "rb+");
    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.active == 1 && s.stationId == sid) {
            found = 1;
            s.endTime = time(NULL);
            secs = (long)difftime(s.endTime, s.startTime);
            s.minutes = (int)((secs + 59) / 60);
            if (s.minutes < MIN_MINUTES) {
                s.minutes = MIN_MINUTES;
            }
            s.total = (float)(int)(s.rate * s.minutes / 60.0 + 0.5);
            s.active = 0;

            fseek(fp, -(long)sizeof(struct Session), SEEK_CUR);
            fwrite(&s, sizeof(struct Session), 1, fp);

            if (getStationById(sid, &st)) {
                st.busy = 0;
                updateStation(st);
            }
            break;
        }
    }
    fclose(fp);

    if (found) {
        showBill(s);
    } else {
        pauseMsg(12, "No active session on that station.");
    }
}

void viewActiveSessions() {
    int count;

    count = showActiveSessions();
    if (count == 0) {
        pauseMsg(14, "No active sessions.");
        return;
    }
    printf("\n  Press any key to go back...");
    getch();
}

void sessionMenu() {
    int choice;

    while (1) {
        system("cls");
        setColor(11);
        drawBox(20, 3, 40, 12);
        gotoxy(30, 4);
        printf("START / END SESSION");
        setColor(7);

        gotoxy(26, 7);
        printf("1. Start Session");
        gotoxy(26, 8);
        printf("2. End Session and Bill");
        gotoxy(26, 9);
        printf("3. View Active Sessions");
        gotoxy(26, 10);
        printf("4. Back");
        gotoxy(26, 12);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            startSession();
        } else if (choice == 2) {
            endSession();
        } else if (choice == 3) {
            viewActiveSessions();
        } else if (choice == 4) {
            break;
        }
    }
}

/* ===================== BILLING ===================== */

int showBillTable() {
    FILE *fp;
    struct Session s;
    struct Station st;
    struct Customer c;
    char sn[30], cn[30], d[20];
    int count = 0;

    tableHeader("ALL BILLS");
    printf("  Bill  Customer        Station         Date        Min   Total\n");
    printf("  ------------------------------------------------------------\n");
    setColor(7);

    fp = fopen("sessions.dat", "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.active == 0) {
            strcpy(sn, "Deleted");
            strcpy(cn, "Deleted");
            if (getStationById(s.stationId, &st)) {
                strcpy(sn, st.name);
            }
            if (getCustomerById(s.customerId, &c)) {
                strcpy(cn, c.name);
            }
            strftime(d, sizeof(d), "%d-%m-%Y", localtime(&s.startTime));
            printf("  %-6d%-16s%-16s%-12s%-6d%.0f\n", s.id, cn, sn, d, s.minutes, s.total);
            count++;
        }
    }
    fclose(fp);
    return count;
}

void viewBills() {
    int count;

    count = showBillTable();
    if (count == 0) {
        pauseMsg(14, "No bills yet.");
        return;
    }
    printf("\n  Press any key to go back...");
    getch();
}

void reprintBill() {
    FILE *fp;
    struct Session s;
    int count, id, found = 0;

    count = showBillTable();
    if (count == 0) {
        pauseMsg(14, "No bills yet.");
        return;
    }

    printf("\n  Enter bill number to reprint: ");
    id = readInt();

    fp = fopen("sessions.dat", "rb");
    while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
        if (s.id == id && s.active == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found) {
        showBill(s);
    } else {
        pauseMsg(12, "Bill number not found.");
    }
}

void billingMenu() {
    int choice;

    while (1) {
        system("cls");
        setColor(11);
        drawBox(20, 3, 40, 11);
        gotoxy(34, 4);
        printf("BILLING");
        setColor(7);

        gotoxy(26, 7);
        printf("1. View All Bills");
        gotoxy(26, 8);
        printf("2. Reprint a Bill");
        gotoxy(26, 9);
        printf("3. Back");
        gotoxy(26, 11);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            viewBills();
        } else if (choice == 2) {
            reprintBill();
        } else if (choice == 3) {
            break;
        }
    }
}

/* ===================== DAILY SALES REPORT ===================== */

void salesReport() {
    FILE *fp;
    struct Session s;
    struct Station st;
    struct Customer c;
    time_t now;
    struct tm today, e;
    char sn[30], cn[30], tbuf[10], d[20];
    int count = 0, mins = 0;
    float total = 0, allTotal = 0;

    now = time(NULL);
    today = *localtime(&now);
    strftime(d, sizeof(d), "%d-%m-%Y", &today);

    tableHeader("DAILY SALES REPORT");
    setColor(7);
    printf("  Date: %s\n\n", d);
    setColor(11);
    printf("  Bill  Customer        Station         Ended     Min   Total\n");
    printf("  ----------------------------------------------------------\n");
    setColor(7);

    fp = fopen("sessions.dat", "rb");
    if (fp != NULL) {
        while (fread(&s, sizeof(struct Session), 1, fp) == 1) {
            if (s.active == 0) {
                allTotal = allTotal + s.total;
                e = *localtime(&s.endTime);
                if (e.tm_yday == today.tm_yday && e.tm_year == today.tm_year) {
                    strcpy(sn, "Deleted");
                    strcpy(cn, "Deleted");
                    if (getStationById(s.stationId, &st)) {
                        strcpy(sn, st.name);
                    }
                    if (getCustomerById(s.customerId, &c)) {
                        strcpy(cn, c.name);
                    }
                    strftime(tbuf, sizeof(tbuf), "%H:%M", &e);
                    printf("  %-6d%-16s%-16s%-10s%-6d%.0f\n", s.id, cn, sn, tbuf, s.minutes, s.total);
                    count++;
                    mins = mins + s.minutes;
                    total = total + s.total;
                }
            }
        }
        fclose(fp);
    }

    if (count == 0) {
        setColor(14);
        printf("  No sales today.\n");
        setColor(7);
    }

    printf("\n  ----------------------------------------------------------\n");
    printf("  Sessions today    : %d\n", count);
    printf("  Total play time   : %dh %dm\n", mins / 60, mins % 60);
    setColor(10);
    printf("  Today's income    : %.0f Tk\n", total);
    setColor(7);
    printf("  All-time income   : %.0f Tk\n", allTotal);

    printf("\n  Press any key to go back...");
    getch();
}

/* ===================== MAIN MENU ===================== */

void mainMenu(struct User u) {
    int choice;
    char roleName[10];

    if (u.role == 1) {
        strcpy(roleName, "Admin");
    } else {
        strcpy(roleName, "Staff");
    }

    while (1) {
        system("cls");
        setColor(11);
        drawBox(18, 2, 44, 16);
        gotoxy(30, 3);
        printf("ST INDOOR ZONE");
        setColor(10);
        gotoxy(22, 5);
        printf("Welcome, %s (%s)", u.username, roleName);
        setColor(7);

        gotoxy(24, 7);
        printf("1. Manage Stations");
        gotoxy(24, 8);
        printf("2. Manage Customers");
        gotoxy(24, 9);
        printf("3. Start / End Session");
        gotoxy(24, 10);
        printf("4. Billing");
        gotoxy(24, 11);
        printf("5. Daily Sales Report");
        gotoxy(24, 12);
        printf("6. Logout");
        gotoxy(24, 14);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            stationMenu(u);
        } else if (choice == 2) {
            customerMenu(u);
        } else if (choice == 3) {
            sessionMenu();
        } else if (choice == 4) {
            billingMenu();
        } else if (choice == 5) {
            if (u.role == 1) {
                salesReport();
            } else {
                showMessage(24, 16, 12, "Only admin can view reports.");
            }
        } else if (choice == 6) {
            break;
        }
    }
}

int main() {
    struct User current;
    int choice;

    createDefaultAdmin();

    while (1) {
        system("cls");
        setColor(11);
        drawBox(20, 3, 40, 12);
        gotoxy(29, 4);
        printf("ST INDOOR ZONE SYSTEM");
        setColor(7);

        gotoxy(26, 7);
        printf("1. Login");
        gotoxy(26, 8);
        printf("2. Register");
        gotoxy(26, 9);
        printf("3. Exit");
        gotoxy(26, 11);
        printf("Enter choice: ");
        choice = readInt();

        if (choice == 1) {
            if (loginUser(&current)) {
                mainMenu(current);
            }
        } else if (choice == 2) {
            registerUser();
        } else if (choice == 3) {
            break;
        }
    }
    return 0;
}

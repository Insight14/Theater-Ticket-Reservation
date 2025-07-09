/*
    Sri Satya Sourish Reddy - SXR230185
    
    The following program is a Lilo and Stitch Ticket/Seat Reservation system.
    The program begins by getting the file which contains a raw auditorium and
    converts it to a proper one. 
    Then a menu of options is displayed for either Reserving Seats or Exiting the
    program, which also generates a sales report of tickets sold.
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <cctype>

const int MAX_ROWS = 10;
const int MAX_COLS = 26;

using namespace std;

// Node structure for linked list representing a seat
struct Node 
{
    char ticketType;
    int row;
    char seat;
    Node* next;
};

// Function to create a new Node (seat)
Node* createNode(int row, char seat, char ticketType) 
{
    Node* newNode = new Node;
    newNode->row = row;
    newNode->seat = seat;
    newNode->ticketType = ticketType;
    newNode->next = nullptr;
    return newNode;
}

// Function to insert a new Node at the end of a linked list
void insertEnd(Node*& head, int row, char seat, char ticketType) 
{
    Node* newNode = createNode(row, seat, ticketType);
    if (!head) 
    {
        head = newNode;
    } else 
    {
        Node* temp = head;
        while (temp->next) 
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to read the seating chart from a file and create linked lists
void readFile(Node**& rows, const char* filename, int& numRows, int& numCols) 
{
    ifstream file(filename);
    if (!file) 
    {
        cout << "Error: Unable to open file." << endl;
    } 
    else 
    {
        string buffer;
        numRows = 0;
        numCols = 0;

        // First pass: Determine dimensions
        while (getline(file, buffer)) 
        {
            numRows++;
            if (static_cast<int>(buffer.length()) > numCols) 
            {
                numCols = static_cast<int>(buffer.length());
            }
        }

        file.close();
        file.open(filename); // Reopen the file

        if (numRows == 0) 
        {
            cout << "Error: File is empty." << endl;
        } 
        else 
        {
            rows = new Node*[numRows];
            for (int i = 0; i < numRows; i++) 
            {
                rows[i] = nullptr;
            }

            // Second pass: Read data and create linked lists
            int rowNum = 0;
            while (getline(file, buffer)) 
            {
                for (size_t col = 0; col < buffer.length() && col < static_cast<size_t>(numCols); col++) 
                {
                    insertEnd(rows[rowNum], rowNum + 1, static_cast<char>('A' + col), buffer[col]);
                }
                rowNum++;
            }
        }

    }

    file.close();
}

// Function to display the seating chart
void displaySeatingChart(Node** rows, int numRows, int numCols) 
{
    cout << "  ";
    for (char col = 'A'; col < 'A' + numCols; col++) 
    {
        cout << col; // Print column letters
    }
    cout << endl;

    for (int i = 0; i < numRows; i++) 
    {
        cout << i + 1 << " ";
        Node* temp = rows[i];
        while (temp) 
        {
            cout << (temp->ticketType == '.' ? '.' : '#');  // Display seats
            temp = temp->next;
        }
        cout << endl;
    }
}


// Function to find and offer best available seats if no user selected ones are found
void findBestAvailableSeats(Node* rows[], int numRows, int numCols, int groupSize, int numAdults, int numChildren, int numSeniors) 
{
    if (numCols == 0) 
        return;

    double bestDistance = 1e9;
    int bestRow = -1;
    char bestStartSeat = ' ';
    Node* bestStartNode = nullptr;

    double centerRow = (numRows - 1) / 2.0;
    double centerCol = (numCols - 1) / 2.0;

    for (int i = 0; i < numRows; i++) 
    {
        Node* temp = rows[i];
        while (temp) 
        {
            //Node* start = temp;       Unused variable so commented out
            int count = 0;
            Node* check = temp;

            while (check && check->ticketType == '.' && count < groupSize) 
            {
                check = check->next;  // Check available seats
                count++;
            }

            if (count == groupSize) 
            {
                // Found a valid block
                double startCol = temp->seat - 'A';
                double endCol = startCol + groupSize - 1;
                double midCol = (startCol + endCol) / 2.0;
                double distance = sqrt(pow(i - centerRow, 2) + pow(midCol - centerCol, 2));

                if (distance < bestDistance ||
                    (fabs(distance - bestDistance) < 1e-6 && fabs(i - centerRow) < fabs(bestRow - centerRow)) ||
                    (fabs(distance - bestDistance) < 1e-6 && i == bestRow && startCol < (bestStartSeat - 'A'))) 
                {
                    bestDistance = distance;
                    bestRow = i;
                    bestStartSeat = temp->seat;
                    bestStartNode = temp;  // Store best seat info
                }
            }
            temp = temp->next;
        }
    }

    if (bestStartNode) 
    {
        char bestEndSeat = static_cast<char>(bestStartSeat + groupSize - 1); // Calculate ending seat
        cout << "Best available seats found: " << bestRow + 1 << bestStartSeat
             << " - " << bestRow + 1 << bestEndSeat << endl;  // Display in correct format
        cout << "Would you like to reserve these " << groupSize << " seats? (Y/N): ";
        
        char choice;
        cin >> choice;
        if (toupper(choice) == 'Y') 
        {
            Node* current = bestStartNode;
            for (int i = 0; i < numAdults; i++, current = current->next)
                current->ticketType = 'A';
            for (int i = 0; i < numChildren; i++, current = current->next)
                current->ticketType = 'C';
            for (int i = 0; i < numSeniors; i++, current = current->next)
                current->ticketType = 'S';

            cout << "Seats successfully reserved!\n";
        } 
        else 
        {
            cout << "Reservation declined. Returning to menu.\n";
        }
    } 
    else 
    {
        cout << "No seats available.\n";
    }
}

// Function to reserve seats (modified to remove attemptReservation)
void reserveSeats(Node* rows[], int numRows, int numCols) 
{
    int rowNum;
    char seatStart;
    int numAdults, numChildren, numSeniors;
    int count = 0;

    cout << "Enter row number: ";
    cin >> rowNum;
    rowNum--; // Adjust for 0-based indexing

    cout << "Enter starting seat letter: ";
    cin >> seatStart;

    cout << "Enter number of adult tickets: ";
    cin >> numAdults;
    cout << "Enter number of child tickets: ";
    cin >> numChildren;
    cout << "Enter number of senior tickets: ";
    cin >> numSeniors;

    int totalSeats = numAdults + numChildren + numSeniors;
    Node* temp = rows[rowNum];
    Node* check = temp;

    // Find the starting seat
    while (temp && temp->seat != seatStart) 
    {
        temp = temp->next;
    }

    if (!temp) 
    {
        cout << "Starting seat not found. Searching for best available...\n";
        findBestAvailableSeats(rows, numRows, numCols, totalSeats, numAdults, numChildren, numSeniors);
        return;
    }

    // Check availability
    while (check && check->ticketType == '.' && count < totalSeats) 
    {
        count++;
        check = check->next;
    }

    if (count == totalSeats) 
    {
        Node* current = temp;
        for (int i = 0; i < numAdults; i++, current = current->next) 
        {
            current->ticketType = 'A';
        }
        for (int i = 0; i < numChildren; i++, current = current->next) 
        {
            current->ticketType = 'C';
        }
        for (int i = 0; i < numSeniors; i++, current = current->next) 
        {
            current->ticketType = 'S';
        }
        cout << "Seats successfully reserved!" << endl;
    } 
    else 
    {
        cout << "Seats not available at the specified location. Searching for best available seats.\n";
        
        findBestAvailableSeats(rows, numRows, numCols, totalSeats, numAdults, numChildren, numSeniors);
    }
}

// Function to generate a report of ticket sales
void generateReport(Node* rows[], int numRows) 
{
    int totalSeats = 0, totalTickets = 0, adultTickets = 0, childTickets = 0, seniorTickets = 0;
    double totalSales = 0.0;

    for (int i = 0; i < numRows; i++) 
    {
        Node* temp = rows[i];
        while (temp) 
        {
            totalSeats++;
            if (temp->ticketType != '.') 
            {
                totalTickets++;
                if (temp->ticketType == 'A') 
                {
                    adultTickets++;
                    totalSales += 10.00;
                } else if (temp->ticketType == 'C') 
                {
                    childTickets++;
                    totalSales += 5.00;
                } else if (temp->ticketType == 'S') 
                {
                    seniorTickets++;
                    totalSales += 7.50;
                }
            }
            temp = temp->next;
        }
    }

    cout << "Total Seats:\t" << totalSeats << "\nTotal Tickets:\t" << totalTickets
         << "\nAdult Tickets:\t" << adultTickets << "\nChild Tickets:\t" << childTickets
         << "\nSenior Tickets:\t" << seniorTickets << "\nTotal Sales:\t$" << fixed << setprecision(2) << totalSales << endl;
}

// Function to write the updated seating chart to a file
void writeToFile(Node* rows[], int numRows) 
{
    ofstream outFile("A1.txt");
    for (int i = 0; i < numRows; i++) 
    {
        Node* temp = rows[i];
        while (temp) 
        {
            outFile << temp->ticketType;
            temp = temp->next;
        }
        outFile << endl;
    }
    outFile.close();
}


int main() 
{
    const int MAX_FILENAME_LENGTH = 100;
    char filename[MAX_FILENAME_LENGTH];

    int numRows, numCols;
    Node** rows = nullptr;

    cout << "Enter the filename: ";
    cin >> filename;

    readFile(rows, filename, numRows, numCols);

    int choice;
    do 
    {
        cout << "\n===== Theater Ticket Reservation System =====\n";
        cout << "1. Reserve Seats\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) 
        {
            case 1:
                displaySeatingChart(rows, numRows, numCols);
                reserveSeats(rows, numRows, numCols);
                break;
            case 2:
                generateReport(rows, numRows);
                cout << "Thanks for using our Ticket Reservation System!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 2);

    writeToFile(rows, numRows);

    // Cleanup
    for (int i = 0; i < numRows; i++) 
    {
        Node* current = rows[i];
        while (current) 
        {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] rows;

    return 0;
}
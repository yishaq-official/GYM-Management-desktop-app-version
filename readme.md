# Gym Management System

## Description
The Gym Management System is a desktop application designed to manage gym membership data efficiently. Built using Qt and C++, it allows administrators to view, edit, and remove member information stored in a SQL Server database. The system provides a user-friendly interface to handle member details such as ID, name, username, phone number, and payment status, ensuring smooth operation of gym administration tasks.

## How to Run

### Prerequisites
- Ensure you have the following installed:
  - **Qt Framework** (version 6.9.0 or later recommended)
  - **MinGW** (for Windows, 64-bit)
  - **SQL Server** (e.g., SQL Server Express) with ODBC Driver 17 for SQL Server
  - **Qt Creator** (optional, for development and debugging)

### Setup Instructions
1. **Clone the Repository**:
   - Clone this project to your local machine using:
     ```
     git clone <https://github.com/issac-D/Qt-Application-for-GYM-management-large.git>
     ```
   - If not using Git, download the ZIP and extract it to a directory (e.g., `C:\Users\hp\Documents\GymManagementSystem`).

2. **Configure the Database**:
   - Install SQL Server and create a database named `GymManagementSystem`.
   - Set up the following tables:
     - `Users` (columns: `ID`, `Name`, `Role`, `Username`, `Phone`)
     - `Payments` (columns: `UserID`, `Status`)
   - Example SQL to create tables:
     ```sql
     CREATE TABLE Users (
         ID INT PRIMARY KEY,
         Name NVARCHAR(100),
         Role NVARCHAR(50),
         Username NVARCHAR(50),
         Phone NVARCHAR(20)
     );

     CREATE TABLE Payments (
         UserID INT,
         Status NVARCHAR(50)
     );
     ```
   - Insert sample data (optional):
     ```sql
     INSERT INTO Users (ID, Name, Role, Username, Phone) VALUES (1, 'Test User', 'Member', 'testuser', '1234567890');
     INSERT INTO Payments (UserID, Status) VALUES (1, 'Paid');
     ```

3. **Configure Database Connection**:
   - Open `DatabaseManager.cpp` (or equivalent) and update the connection string to match your SQL Server setup. Example:
     ```
     QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
     db.setDatabaseName("DRIVER={ODBC Driver 17 for SQL Server};SERVER=DESKTOP-F7H8IR1\\SQLEXPRESS;DATABASE=GymManagementSystem;Trusted_Connection=Yes;");
     ```
   - Adjust `SERVER`, `DATABASE`, and authentication as needed.

4. **Build the Project**:
   - Open the project in Qt Creator or use the command line.
   - Run `qmake` to generate the Makefile:
     ```
     qmake
     ```
   - Build the project:
     ```
     mingw32-make
     ```
   - This generates the executable in the `build` directory (e.g., `debug/GymManagementSystem.exe`).

5. **Run the Application**:
   - Navigate to the build directory and run the executable:
     ```
     cd build\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\debug
     GymManagementSystem.exe
     ```
   - Alternatively, use Qt Creator’s "Run" button.

## Requirements
- **Software**:
  - Qt 6.9.0 or later
  - MinGW 64-bit compiler
  - SQL Server with ODBC Driver 17
  - Qt Creator (optional)
- **Hardware**: Minimal requirements for a development machine (e.g., 4GB RAM, 2GHz CPU).
- **Database**: SQL Server instance with `GymManagementSystem` database and required tables.

## Logic Overview
- **Main Interface**: The application features a `ViewMembersDialog` window where administrators can view member details.
- **Data Retrieval**: The system connects to the SQL Server database using ODBC and retrieves member data via a `QSqlQuery`. The query joins `Users` and `Payments` tables to fetch ID, name, username, phone, and payment status.
- **Table Display**: Member data is loaded into a `QTableWidget`, with columns for ID, name, username, phone, payment status, and an action button. The table is refreshed when the dialog initializes or the refresh button is clicked.
- **Edit Functionality**: Double-clicking a row opens an `EditMemberDialog` to modify member details, triggering a table refresh upon update.
- **Remove Functionality**: A "Remove" button per row initiates a transaction to delete the member’s payment and user records from the database, followed by a table refresh.
- **Error Handling**: The system logs database errors and displays QMessageBox alerts for critical issues (e.g., connection failures).

## Notes
- Ensure the database connection string is correctly configured before running.
- The application assumes a trusted connection; adjust authentication if using SQL Server login credentials.
- For development, use Qt Creator to debug and modify the UI via `ViewMembersDialog.ui`.

This project is designed for gym administrators to manage membership data efficiently. Contributions and bug reports are welcome!


### Notes
- The README assumes the project structure and files (e.g., `DatabaseManager.cpp`, `ViewMembersDialog.ui`) align with your setup. Adjust paths or details if different.
- The logic overview excludes the filter functionality you removed, focusing on core features.
- If you provide the requested info (UI contents, schema), I can refine this further.

Please let me know if you need adjustments or have the additional details to share!
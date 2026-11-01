# AuraFlex – Gym Management System

<p align="center">
  <strong>A modular, menu-driven Gym Management System developed using C</strong>
</p>

AuraFlex is a terminal-based Gym Management System developed as the Group 11 project for the **ICT1142 – Programming Practicum** module. It helps gym staff manage day-to-day operations through a single application, including member registration, memberships, payments, attendance, trainer management, and reports.

The application uses a modular C architecture and stores records in plain-text files inside the `data/` directory. Users sign in and are presented with dashboard options according to their assigned role.

---

## Table of Contents

- [Project Objectives](#project-objectives)
- [Features](#features)
- [User Roles](#user-roles)
- [Technologies and Concepts](#technologies-and-concepts)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Data Storage](#data-storage)
- [Validation and Business Rules](#validation-and-business-rules)
- [Team Members](#team-members)
- [Documentation](#documentation)
- [Limitations](#limitations)

---

## Project Objectives

- Provide secure user authentication with role-based dashboard access.
- Register, search, update, freeze, and unfreeze gym members.
- Assign memberships and manage membership periods and fees.
- Record payments and provide payment history and totals.
- Track member check-ins and check-outs.
- Manage trainers and trainer–member assignments.
- Generate reports using the records stored by the application.
- Validate user input and protect existing records when updating text files.

## Features

### Authentication and Role-Based Access
- Username and password verification.
- Password masking where supported by the operating environment.
- Maximum of three login attempts.
- Active-account validation.
- Separate dashboards for administrators, receptionists, and trainers.

### Member Management
- Register and view members.
- Search members by member ID.
- Update member details.
- Freeze and unfreeze member accounts.
- Prevent duplicate member IDs.

### Membership Management
- Assign memberships to existing members.
- Store membership plans, start dates, end dates, fees, and statuses.
- Prevent duplicate membership IDs.
- Validate member references and membership fees.
- Connect membership assignment with payment processing.
- Identify expired memberships through reports.

### Payment Management
- Record payments for existing active members.
- Generate unique payment IDs.
- Support `CASH`, `CARD`, and `ONLINE` payment methods.
- Search a member's payment history.
- View payment records and calculate total payments.

### Attendance Management
- Record member check-ins and check-outs.
- Generate attendance IDs automatically.
- Store attendance dates and times.
- Search and view attendance history.
- Display daily attendance summaries, including check-ins, completed check-outs, and members still inside.
- Prevent duplicate open check-ins for the same member on the same day.

### Trainer Management
- Register, view, search, and update trainers.
- Activate or deactivate trainers.
- Assign members to trainers.
- Remove trainer–member assignments.
- View the members assigned to a trainer.
- Validate trainer and member records before creating assignments.

### Reports
- Member reports.
- Membership reports.
- Expired membership reports.
- Payment reports.
- System summary reports.

---

## User Roles

| Role | Main Permissions |
|---|---|
| **Administrator** | Access member, membership, trainer, payment, attendance, and report functions. |
| **Receptionist** | Register and view members, manage memberships, record payments, and handle attendance. |
| **Trainer** | View assigned members and manage trainer–member assignments. |

The available dashboard options depend on the role of the authenticated user.

## Technologies and Concepts

| Category | Technology / Concept |
|---|---|
| Programming Language | C |
| Programming Standard | C11 |
| Interface | Terminal / Command-Line Interface |
| Compiler | GCC or a compatible C compiler |
| Data Storage | Plain-text files |
| Program Design | Modular and procedural programming |
| Data Structures | C structures |
| File Handling | Reading, writing, appending, and temporary-file replacement |
| Other Concepts | Header files, functions, string handling, input validation, date and time handling |

---

## Project Structure

```text
AuraFlex/
│
├── .gitignore
│
├── data/                         # Persistent application records
│   ├── attendance.txt
│   ├── members.txt
│   ├── memberships.txt
│   ├── membership_plans.txt
│   ├── payments.txt
│   ├── trainers.txt
│   ├── trainer_members.txt
│   └── users.txt
│
├── docs/                         # Project documentation
│   └── README.md
│
├── include/                      # Header files and declarations
│   ├── attendance.h
│   ├── auth.h
│   ├── member.h
│   ├── membership.h
│   ├── models.h
│   ├── payment.h
│   ├── platform.h
│   ├── reports.h
│   ├── system.h
│   ├── trainer.h
│   └── ui.h
│
├── src/                          # Application source code
│   ├── main.c
│   ├── attendance/
│   │   └── attendance.c
│   ├── auth/
│   │   └── auth.c
│   ├── member/
│   │   └── member.c
│   ├── membership/
│   │   └── membership.c
│   ├── output/
│   │   └── main.exe
│   ├── payment/
│   │   └── payment.c
│   ├── reports/
│   │   └── reports.c
│   ├── system/
│   │   ├── system.c
│   │   └── ui.c
│   └── trainers/
│       └── trainer.c
│
└── build/                        # Compiled application output
    └── gym.exe
```

### Folder Overview

- **`data/`** – Stores user, member, membership, payment, attendance, trainer, and assignment records.
- **`docs/`** – Contains project documentation.
- **`include/`** – Contains header files with shared structures, declarations, and interfaces.
- **`src/`** – Contains the implementation of the application's modules and the main entry point.
- **`build/`** – Holds the compiled application.

---

## Getting Started

### Prerequisites

- A C compiler such as GCC.
- A terminal or command-line environment.
- The complete AuraFlex project folder.

### 1. Open the Project

Open a terminal in the root `AuraFlex/` directory. The application uses relative paths such as `data/users.txt`, so it should be launched from the project root.

### 2. Create the Build Directory

**Linux / macOS:**

```bash
mkdir -p build
```

**Windows PowerShell:**

```powershell
New-Item -ItemType Directory -Force build
```

### 3. Compile the Application

The following command follows the source-file list in the project User Guide:

**Linux / GCC:**

```bash
gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude \
  src/main.c \
  src/auth/auth.c \
  src/member/member.c \
  src/membership/membership.c \
  src/trainers/trainer.c \
  src/payment/payment.c \
  src/attendance/attendance.c \
  src/reports/reports.c \
  src/system/system.c \
  src/system/ui.c \
  -o build/gym
```

**Windows / MinGW GCC:**

```bash
gcc -Wall -Wextra -std=c11 -Iinclude ^
  src/main.c ^
  src/auth/auth.c ^
  src/member/member.c ^
  src/membership/membership.c ^
  src/trainers/trainer.c ^
  src/payment/payment.c ^
  src/attendance/attendance.c ^
  src/reports/reports.c ^
  src/system/system.c ^
  src/system/ui.c ^
  -o build/gym.exe
```

> **Platform note:** The authentication implementation uses `conio.h` for password input. Password masking and some console features may therefore behave differently across operating systems. The Windows command is intended for a compatible MinGW environment; adjust platform-specific compiler flags or input handling if required.

### 4. Run the Application

Run the program from the project root so that the relative data-file paths resolve correctly.

**Linux / macOS:**

```bash
./build/gym
```

**Windows PowerShell:**

```powershell
.\build\gym.exe
```

### 5. Sign In

1. Enter your username.
2. Enter your password.
3. The system checks the credentials and account status.
4. A successful login opens the dashboard for the user's role.

Users have a maximum of **three login attempts**. Contact the system administrator if access is denied.

> Use the user credentials supplied with the project or by the system administrator. Do not publish real passwords or sensitive member information in the repository.

---

## Data Storage

AuraFlex uses plain-text files in the `data/` directory instead of a database server.

| File | Purpose |
|---|---|
| `users.txt` | User login details, roles, and account statuses |
| `members.txt` | Gym member records |
| `memberships.txt` | Membership assignments and details |
| `membership_plans.txt` | Membership plan records |
| `payments.txt` | Payment records |
| `attendance.txt` | Member attendance records |
| `trainers.txt` | Trainer records |
| `trainer_members.txt` | Trainer–member assignments |

Temporary files are used by relevant operations to rebuild records safely when updating or removing entries.

## Validation and Business Rules

The application includes validation rules such as:

- User credentials must match and the account must be active.
- Login is limited to three attempts.
- Member, trainer, membership, payment, and attendance IDs must be unique where applicable.
- Memberships can only be assigned to existing members.
- Payments require an existing active member and a positive amount.
- Accepted payment methods are `CASH`, `CARD`, and `ONLINE`.
- Membership and payment dates must follow the expected date format.
- A member cannot have more than one open check-in on the same day.
- Check-out requires an existing open attendance record.
- Trainer assignments require an existing member and an active trainer.
- Duplicate trainer–member assignments are rejected.
- Invalid menu selections and file-operation errors are handled with messages.

**Text-file input note:** Stored fields are separated by spaces. Enter single-word values or use underscores for multiple words, for example `John_Silva` or `Personal_Training`.

---

## Team Members

AuraFlex was developed by **Group 11**.

| No. | Team Member | GitHub Username |
|---:|---|---|
| 1 | S.H. Chehan Mendiya | [`ChehanMendiya`](https://github.com/ChehanMendiya) |
| 2 | K.R. Karanayaka | [`kivithkaranayaka-bit`](https://github.com/kivithkaranayaka-bit) |
| 3 | R. Indumin Sasindu | [`Indumin-Sasindu`](https://github.com/Indumin-Sasindu) |
| 4 | T.V.K. Sendiya | [`Sendiya2`](https://github.com/Sendiya2) |
| 5 | Pasindu Madusanka | [`pasindu2988664`](https://github.com/pasindu2988664) |

---

## Documentation

The project documentation includes:

- **Software Requirements Specification (SRS)** – Defines the project scope, user roles, functional requirements, and system constraints.
- **Final Project Report** – Describes the system design, implementation, testing, challenges, and future improvements.
- **User Guide** – Explains compilation, application usage, dashboard operations, input rules, and troubleshooting.
- **Individual Contribution Reports** – Describe the responsibilities and contributions of the group members.

Place the relevant documentation files in the `docs/` directory and link them here when they are included in the repository. For example:

- [Software Requirements Specification](docs/SRS.pdf)
- [Final Project Report](docs/Final_Report.pdf)
- [User Guide](docs/User_Guide.pdf)
- [Individual Contribution Report](docs/Individual_Contribution.pdf)

---

## Limitations

The current project has the following limitations:

- Records are stored in plain-text files and are not encrypted.
- The application does not use a database server.
- The interface is terminal-based rather than graphical or web-based.
- Online payment processing is not supported.
- Email and SMS notifications are not included.
- Biometric attendance devices are not supported.
- Simultaneous access from multiple computers is not supported.
- Password input features may depend on the operating system.

## Future Improvements

Possible future improvements include:

- Database integration.
- Stronger password protection and security.
- A graphical or web-based user interface.
- More advanced reporting and filtering.
- Email and SMS notifications.
- Multi-user and multi-computer support.

---

<p align="center">
  <strong>AuraFlex – Manage Your Gym. Empower Your Members.</strong>
  <br>
  <sub>Group 11 | ICT1142 – Programming Practicum</sub>
</p>

# AuraFlex Gym Management System

AuraFlex is a menu-driven gym management system written in C. It gives
administrators, receptionists, and trainers role-based access to common gym
operations while storing records in text files under `data/`.

## Features

- User authentication with active-account checking and limited login attempts
- Role-based dashboards for administrators, receptionists, and trainers
- Member registration, search, updates, and freeze/unfreeze status control
- Membership plan and membership management
- Payment recording and payment history
- Attendance check-in, check-out, and attendance summaries
- Trainer registration and trainer-member assignments
- Member, membership, payment, attendance, and summary reports
- Input validation and safe text-file updates using temporary files

## User Roles

| Role | Main capabilities |
|---|---|
| Administrator | Full access to members, memberships, trainers, payments, attendance, and reports |
| Receptionist | Register and view members, manage memberships, record payments, and record attendance |
| Trainer | View assigned members and manage trainer-member assignments |

## Project Structure

```text
AuraFlex/
├── data/       Persistent text-file records
├── include/    Header files and shared declarations
├── src/        C source files grouped by feature
├── build/      Local build and test output
└── docs/       Project report, requirements, and user manual
```

Important data files include `users.txt`, `members.txt`, `memberships.txt`,
`payments.txt`, `attendance.txt`, `trainers.txt`, and `trainer_members.txt`.

## Requirements

- A C compiler such as GCC or Clang
- A Windows-compatible console environment for the current password-masking
  implementation, which uses `<conio.h>`
- The `data/` directory, because the application uses paths relative to the
  project root

## Build

Run the following command from the project root:

```bash
gcc -Iinclude \
  src/main.c \
  src/auth/auth.c \
  src/member/member.c \
  src/membership/membership.c \
  src/payment/payment.c \
  src/attendance/attendance.c \
  src/trainers/trainer.c \
  src/reports/reports.c \
  src/system/system.c \
  src/system/ui.c \
  -o build/auraflex
```

On Windows, the output can be named `build/auraflex.exe`.

## Run

Run the executable from the project root so that relative paths to `data/`
resolve correctly:

```bash
build/auraflex
```

The application starts at the login screen. Use an active account from
`data/users.txt`; do not edit production credentials manually.

## Data and Safety Notes

- Data is stored as plain text and is intended for local project use.
- Back up the `data/` directory before testing update or delete operations.
- Keep field values compatible with the whitespace-separated file format.
- Do not commit real passwords or personal member information.
- Do not start the executable from another working directory unless the data
  paths have been configured accordingly.

## Documentation

- [End-User Manual](USER_MANUAL.md) - operating instructions for each role
- [Software Requirements Specification](SRS.md) - system requirements and
  constraints
- [Final Project Report](FINAL_PROJECT_REPORT.md) - design, implementation,
  testing, and future improvements

## Current Limitations

AuraFlex currently uses plain-text storage, a terminal interface, and
platform-specific password input. It does not provide a database server,
networked multi-user access, online payments, or email/SMS notifications.
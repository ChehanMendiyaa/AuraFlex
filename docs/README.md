# AuraFlex Gym Management System

A terminal-based Gym Management System developed in **C**. The project
is designed to compile with GCC on **Windows (MinGW-w64)** and
**Ubuntu/Linux** using one shared codebase.

> This README is based on the project structure and details shared
> during development. Verify the final source-file list, data-file
> names, and implemented features against the latest integrated code
> before submission.

## Overview

AuraFlex provides role-based terminal menus for common gym
administration activities. The known roles are Administrator,
Receptionist and Trainer. The application is divided into C source files
and header files, with local data files used by the application.

## Features and user roles

### Administrator

1.  Member Management
2.  Membership Plans
3.  Trainer Management
4.  Payment Management
5.  Attendance Management
6.  Reports
7.  Logout

### Receptionist

1.  Register Member
2.  Manage Membership
3.  Record Payment
4.  Record Attendance
5.  View Members
6.  Logout

### Trainer

1.  View Assigned Members
2.  Manage Workout Plans
3.  Manage Nutrition Plans
4.  View Fitness Progress
5.  Manage Goals
6.  Logout

**Note:** Confirm that every displayed menu option is implemented and
connected to a working function in the final build. A menu label alone
does not prove that a feature is complete.

## Technology and requirements

-   Language: C
-   Language standard: C11
-   Compiler: GCC
-   Windows compiler: MinGW-w64 GCC
-   Ubuntu/Linux compiler: GCC
-   Interface: Terminal / command line
-   Cross-platform layer: `include/platform.h`

### Ubuntu dependencies

``` bash
sudo apt update
sudo apt install build-essential
```

### Windows dependencies

Install MinGW-w64 GCC and ensure `gcc` is available in the PowerShell
`PATH`.

## Project structure

The source layout used in the build commands is:

``` text
AuraFlex/
├── include/
│   ├── platform.h
│   ├── auth.h
│   ├── member.h
│   ├── membership.h
│   ├── trainer.h
│   ├── reports.h
│   ├── system.h
│   ├── ui.h
│   ├── attendance.h
│   └── payment.h
├── src/
│   ├── main.c
│   ├── auth/auth.c
│   ├── member/member.c
│   ├── membership/membership.c
│   ├── trainers/trainer.c
│   ├── reports/reports.c
│   ├── system/system.c
│   ├── system/ui.c
│   ├── attendance/attendance.c
│   └── payment/payment.c
├── data/       # Required application data files, if used
├── build/      # Compiled executable output
└── README.md
```

### Main components

  -----------------------------------------------------------------------
  File / module                       Responsibility
  ----------------------------------- -----------------------------------
  `main.c`                            Application entry point, login
                                      flow, role dashboards and menu
                                      navigation

  `auth.c`                            User authentication

  `member.c`                          Member operations

  `membership.c`                      Membership operations

  `trainer.c`                         Trainer management and
                                      trainer/member operations

  `payment.c`                         Payment operations

  `attendance.c`                      Attendance operations

  `reports.c`                         Reports

  `system.c`                          System utilities such as screen
                                      clearing and delays

  `ui.c`                              Terminal UI and logo

  `platform.h`                        Windows/Linux-specific terminal
                                      operations
  -----------------------------------------------------------------------

## Cross-platform support

The project uses conditional compilation in `include/platform.h`:

-   `_WIN32` selects Windows-specific implementations.
-   The `#else` branch selects POSIX/Linux implementations.

Shared application code should call wrapper functions rather than
directly calling operating-system-specific functions:

``` c
platform_init_console();
platform_getch();
platform_clear_screen();
platform_sleep_ms(2000);
```

Examples handled by the platform layer:

  Operation               Windows               Ubuntu/Linux
  ----------------------- --------------------- ----------------------
  Clear terminal          `cls`                 `clear`
  Read key without echo   `_getch()`            `termios` / `read()`
  Delay                   `Sleep()`             `nanosleep()`
  UTF-8 console setup     Windows console API   Terminal locale

Keep Windows-only headers and functions inside the `_WIN32` section of
`platform.h`. Do not call `windows.h`, `SetConsoleOutputCP()`,
`CP_UTF8`, `_getch()` or `Sleep()` directly from shared modules.

## Build and run on Ubuntu

Open a terminal in the project root:

``` bash
cd /path/to/AuraFlex
```

Create the build directory:

``` bash
mkdir -p build
```

Compile:

``` bash
gcc -Wall -Wextra -std=c11 \
-D_POSIX_C_SOURCE=200809L -Iinclude \
src/main.c \
src/auth/auth.c \
src/member/member.c \
src/membership/membership.c \
src/reports/reports.c \
src/system/system.c \
src/system/ui.c \
src/attendance/attendance.c \
src/payment/payment.c \
src/trainers/trainer.c \
-o build/gym
```

Run:

``` bash
./build/gym
```

## Build and run on Windows

These commands assume MinGW-w64 GCC is installed and available in
PowerShell.

Create the build directory:

``` powershell
New-Item -ItemType Directory -Force -Path build | Out-Null
```

Compile:

``` powershell
gcc -Wall -Wextra -std=c11 -Iinclude `
src/main.c `
src/auth/auth.c `
src/member/member.c `
src/membership/membership.c `
src/reports/reports.c `
src/system/system.c `
src/system/ui.c `
src/attendance/attendance.c `
src/payment/payment.c `
src/trainers/trainer.c `
-o build/gym.exe
```

Run:

``` powershell
.\build\gym.exe
```

If using Command Prompt rather than PowerShell, use a single-line
command or the appropriate Command Prompt line-continuation syntax.

**Important:** These commands use the source files known from the
project discussions. If your final project has additional or renamed
`.c` files, add the correct files to the command.

## Data files

The application uses local data files according to its implementation.
Confirm the exact filenames, paths, record formats and required initial
content from the final source code and data directory.

Before running:

1.  Include all required data files in the project.
2.  Preserve their expected filenames and folder locations.
3.  Run the application from the project root if relative paths are
    used.
4.  Ensure the program has permission to read and write the data
    directory.
5.  Back up data before testing operations that update or delete
    records.

Project discussions referenced `users.txt` and `data/members.txt`;
verify their final paths and formats. Do not create empty replacement
files unless the application expects them and their format is known.

## User accounts

No default username or password is specified here. Use the approved test
accounts supplied by the project team or lecturer and verify them
against the user data file. Do not commit real or production passwords
to a public repository.

## How to use the application

1.  Start the executable using the relevant command above.
2.  Enter the username requested by the login screen.
3.  Enter the password. If password masking is enabled, asterisks (`*`)
    appear instead of password characters.
4.  After successful login, use the dashboard displayed for your role.
5.  Enter the number of the required menu option.
6.  Follow the prompts from the selected module.
7.  Press Enter when prompted to return to the dashboard.
8.  Select Logout to end the session.

## Troubleshooting

  --------------------------------------------------------------------------------------
  Problem                                            Suggested action
  -------------------------------------------------- -----------------------------------
  `gcc: command not found`                           Install GCC/build tools on Ubuntu
                                                     or configure MinGW-w64 on Windows.

  `windows.h: No such file or directory` on Ubuntu   Move Windows-specific code behind
                                                     `_WIN32` and provide a POSIX
                                                     implementation.

  `conio.h: No such file or directory` on Ubuntu     Use the shared `platform_getch()`
                                                     implementation instead of directly
                                                     calling `_getch()`.

  `Sleep()` is not declared                          Use `platform_sleep_ms()` in shared
                                                     code.

  `SetConsoleOutputCP` or `CP_UTF8` not declared     Use `platform_init_console()` and
                                                     keep Windows console APIs inside
                                                     the `_WIN32` branch.

  `cannot find src/...: No such file or directory`   Check the filename/path and run the
                                                     command from the project root.

  `undefined reference to ...`                       Include the `.c` file containing
                                                     the function and ensure its
                                                     definition matches the header
                                                     declaration.

  Data file cannot be opened                         Check its path, working directory
                                                     and read/write permissions.

  Password is empty immediately                      If username input uses `scanf()`,
                                                     consume the leftover newline before
                                                     reading the password.
  --------------------------------------------------------------------------------------

## Team

  Name      Role
  --------- -------------
  Chehan    Team Leader
  Kiwith    Team Member
  Idumin    Team Member
  Sendiya   Team Member
  Pasindu   Team Member

Add each student's registration/index number to the required project
documents.

## Submission checklist

-   [ ] Complete integrated C source code (`.c` files)
-   [ ] Required header files (`.h` files)
-   [ ] All required sample and application data files
-   [ ] `README.md`
-   [ ] `SRS_G01.pdf`
-   [ ] `Final_Report_G01.pdf`
-   [ ] `Individual_Con_G01.pdf`
-   [ ] `User_Guide_G01.pdf`
-   [ ] Any additional files requested by the lecturer

Before submission, compile and run the application on both Windows and
Ubuntu using GCC. Ensure the documentation describes the same features
as the final integrated implementation.

------------------------------------------------------------------------

**AuraFlex Gym Management System --- Group G01**

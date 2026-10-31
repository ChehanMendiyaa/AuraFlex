# AuraFlex – Gym Management System

AuraFlex is a terminal-based Gym Management System developed in **C**. The project is organised into separate modules for authentication, members, memberships, trainers, payments, attendance, reports, and system utilities.

The project uses text files in the `data/` directory to store application records.

## Features and modules

The repository currently contains the following modules:

| Module | Source file | Header |
|---|---|---|
| Authentication | `src/auth/auth.c` | `include/auth.h` |
| Member management | `src/member/member.c` | `include/member.h` |
| Membership management | `src/membership/membership.c` | `include/membership.h` |
| Trainer management | `src/trainers/trainer.c` | `include/trainer.h` |
| Payment management | `src/payment/payment.c` | `include/payment.h` |
| Attendance management | `src/attendance/attendance.c` | `include/attendance.h` |
| Reports | `src/reports/reports.c` | `include/reports.h` |
| System utilities | `src/system/system.c` | `include/system.h` |
| Terminal user interface | `src/system/ui.c` | `include/ui.h` |
| Shared data structures | — | `include/models.h` |
| Platform-specific helpers | — | `include/platform.h` |

`src/main.c` is the application entry point.

### Data files

The application data directory currently contains:

- `data/users.txt`
- `data/members.txt`
- `data/memberships.txt`
- `data/membership_plans.txt`
- `data/trainers.txt`
- `data/trainer_members.txt`
- `data/payments.txt`
- `data/attendance.txt`

Keep these files in the expected locations when running the application. If the application uses relative paths, start it from the repository root so that the `data/` directory can be found.

## Project structure

```text
AuraFlex/
├── .gitignore
├── data/
│   ├── attendance.txt
│   ├── members.txt
│   ├── memberships.txt
│   ├── membership_plans.txt
│   ├── payments.txt
│   ├── trainers.txt
│   ├── trainer_members.txt
│   └── users.txt
├── docs/
│   └── README.md
├── include/
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
├── src/
│   ├── main.c
│   ├── attendance/attendance.c
│   ├── auth/auth.c
│   ├── member/member.c
│   ├── membership/membership.c
│   ├── output/main.exe
│   ├── payment/payment.c
│   ├── reports/reports.c
│   ├── system/system.c
│   ├── system/ui.c
│   └── trainers/trainer.c
└── build/
    └── gym.exe
```

> The tree above reflects the repository structure supplied for this README. The executable files shown are build outputs, not source modules.

## Requirements

- GCC compiler
- Windows with MinGW-w64, or a Linux distribution with GCC
- A terminal / command prompt

Check that GCC is installed:

```bash
gcc --version
```

## Build and run

Run the commands from the **AuraFlex project root**.

### Windows (PowerShell with MinGW GCC)

```powershell
New-Item -ItemType Directory -Force -Path build | Out-Null

gcc -Wall -Wextra -std=c11 -Iinclude `
  src/main.c `
  src/auth/auth.c `
  src/member/member.c `
  src/membership/membership.c `
  src/trainers/trainer.c `
  src/payment/payment.c `
  src/attendance/attendance.c `
  src/reports/reports.c `
  src/system/system.c `
  src/system/ui.c `
  -o build/gym.exe
```

Run the application:

```powershell
.\build\gym.exe
```

### Ubuntu / Linux

```bash
mkdir -p build

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

Run the application:

```bash
./build/gym
```

If the compiler reports an undefined reference, check that the corresponding `.c` file is included in the build command and that the function declaration and definition match.

## Cross-platform notes

The project includes `include/platform.h` for operating-system-specific terminal operations. Keep Windows-only APIs (for example, `windows.h`, `_getch()`, and `Sleep()`) behind platform-specific branches in that header. Application modules should use the project's platform wrapper functions instead of calling Windows-only APIs directly.

When building on Linux, check that no source file directly depends on Windows-only headers or functions.

## User roles

The application has been described with role-based access for:

- **Admin**
- **Receptionist**
- **Trainer**

The exact menu options and permissions should be kept consistent with the implementation in `src/main.c` and the authentication logic in `src/auth/auth.c`. Do not document a menu action as available until it has been tested in the current build.

## Development guidelines

- Keep function declarations in the matching header under `include/`.
- Keep function implementations in the corresponding module under `src/`.
- Include shared structures from `include/models.h` where appropriate.
- Keep persistent text data in `data/`.
- Use relative file paths consistently and run the application from the repository root.
- Rebuild after changing source files; do not rely on an old executable in `build/` or `src/output/`.

## Troubleshooting

### `conio.h` or `windows.h` not found on Ubuntu

Remove direct Windows-only includes from portable modules. Use the platform abstraction in `include/platform.h` and compile the Linux branch.

### `undefined reference` during linking

Make sure every required implementation file is included in the GCC command. Also check that function names, parameter types, and return types match between the `.h` declaration and `.c` definition.

### Data files cannot be opened

Run the program from the repository root and confirm the required `.txt` files exist in `data/`. Check the file paths used by the relevant source module.

### Terminal clear or keyboard input behaves differently

Use the wrappers provided by `platform.h` for screen clearing, key input, sleeping, and console initialisation. Test these operations in both Windows PowerShell/Windows Terminal and an Ubuntu terminal.

## Team

Add or update each team member's full name, student ID, and GitHub username here before submission. Ensure the details match the team's official records.

| Team member | Responsibility | GitHub username |
|---|---|---|
| Chehan (Team Leader) | Main integration, authentication, reports and system utilities | `ChehanMendiya` |
| Kiwith | Member, membership and trainer modules | `kivithkaranayaka-bit` |
| Idumin | Confirm the final assigned module(s) | `Indumin-Sasindu` |
| Sendiya | Confirm the final assigned module(s) | `Sendiya2` |
| Pasindu | Confirm the final assigned module(s) | `pasindu2080664` |

Update the responsibility column to reflect the final code contributions rather than planned assignments.

## Repository

GitHub: https://github.com/ChehanMendiyaa/AuraFlex

## Before submission

- [ ] Confirm the README is saved as `README.md` in the repository root (GitHub displays the root README on the repository front page).
- [ ] Verify the Windows build using the command above.
- [ ] Verify the Ubuntu/Linux build using the command above.
- [ ] Test login and each role's permitted menu actions.
- [ ] Test create, view, update, and delete operations where supported.
- [ ] Confirm all required data files are present and contain appropriate sample data.
- [ ] Remove accidental generated files such as `.DS_Store` and unnecessary duplicate executables from the source folders.
- [ ] Confirm team names, student IDs, GitHub usernames, and contribution details.
- [ ] Include the required reports and user documentation in the final submission.

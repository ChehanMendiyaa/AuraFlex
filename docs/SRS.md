# Software Requirements Specification (SRS)

## AuraFlex Gym Management System

**Version:** 1.0  
**Date:** 31 October 2026  
**Document status:** Proposed project specification

## 1. Introduction

### 1.1 Purpose

This Software Requirements Specification defines the requirements for AuraFlex, a
menu-driven gym management application written in C. It provides a common
reference for implementation, testing, demonstration, and future maintenance.
The system is intended to replace disconnected manual records with one
validated application for managing members, memberships, payments, attendance,
trainers, and operational reports.

### 1.2 Scope of the System

AuraFlex manages day-to-day administrative records for a small gym. Its users
are administrators, receptionists, and trainers. The application authenticates
users, displays a role-appropriate dashboard, and provides menus for:

- registering, searching, updating, freezing, and unfreezing members;
- assigning memberships and recording membership payments;
- recording and reviewing payments;
- recording check-in and check-out attendance;
- registering trainers and managing trainer-member assignments; and
- producing member, membership, expiry, payment, and summary reports.

The application includes terminal interaction, role-based navigation, input
validation, automatic identifiers where applicable, and persistent text files in
the `data/` directory.

The application excludes online payment processing, a database server,
biometric devices, email/SMS delivery, a graphical or web interface, networked
multi-user access, and third-party integrations. It is not intended to provide
payroll, inventory purchasing, accounting, or medical management. Because the
storage format is whitespace-separated text, names and descriptions are
effectively single-token values unless underscores are used.

### 1.3 Objectives

The system shall:

1. Authenticate a valid active user within a maximum of three login attempts.
2. Provide dashboard options according to the authenticated user role.
3. Prevent duplicate member, trainer, membership, and relationship identifiers.
4. Persist valid operational records in the relevant files under `data/`.
5. Prevent payments for missing or inactive members and reject non-positive
   amounts.
6. Prevent a member from having more than one open attendance record for a day.
7. Produce reports based on the current contents of the persistent data files.
8. Reject malformed menu input, invalid dates, unsupported payment methods, and
   invalid cross-component references with a user-visible message.

### 1.4 Intended Users

- **Administrator:** full access to member, membership, trainer, payment,
  attendance, and reporting functions.
- **Receptionist:** member registration/viewing, membership management, payment
  recording, and attendance operations.
- **Trainer:** viewing assigned members and adding or removing assignments.
- **System maintainer/developer:** manages source code, data-file layout,
  compilation, testing, and controlled maintenance; this is not an end-user
  role.

### 1.5 Development Environment and Constraints

| Area | Requirement |
|---|---|
| Programming language | C using procedural and modular programming |
| Operating environment | Terminal/command-line environment; password masking currently uses `conio.h` and may be platform-dependent |
| User interface | Menu-driven console interface with formatted tables, prompts, and status messages |
| Data storage | Plain-text, whitespace-separated files in `data/`; temporary files are used for safe rewrites |
| Expected concepts | Structures, functions, header/source separation, file I/O, string handling, validation, date/time functions, role-based control flow, and temporary-file replacement |

The application expects the working directory and relative `data/` paths to be
available. It does not provide concurrent write locking or encryption for
plain-text credentials and records.

## 2. Overall System Description

### 2.1 Background / Problem Description

Gym staff must coordinate member registration, membership periods, fee
collection, attendance, trainer assignments, and reports. Paper records or
separate spreadsheets can produce duplicate identifiers, lost payment history,
incorrect attendance state, and missed membership expiry dates. They also make
it difficult to restrict operations according to staff responsibilities.

### 2.2 Proposed Solution

AuraFlex centralizes these activities in a single C application. A user first
authenticates, then uses a role-specific dashboard. Each component validates
its input and references related records before writing changes. Shared text
files provide persistence without requiring a database server, while reporting
functions read those files to produce current summaries.

### 2.3 Main System Components

The application contains six integrated components:

1. **Authentication and role access:** reads users, validates credentials and
   active status, limits attempts, and selects the dashboard.
2. **Member and membership management:** maintains member status and
   membership assignments, including plan dates and fees.
3. **Payment management:** records payment transactions, supports membership
   payment integration, searches history, and calculates totals.
4. **Attendance management:** records check-in/check-out state, history, and
   daily attendance summaries.
5. **Trainer management:** maintains trainer records and trainer-member
   relationships.
6. **Reports and shared user interface:** presents menus and consolidated
   member, membership, expiry, payment, and summary reports using shared data.

### 2.4 Overall System Workflow

```text
+------------------+
| Start application |
+--------+---------+
         |
         v
+------------------+       failure after 3 attempts
| Authenticate user |------------------------------+
+--------+---------+                               |
         | success                                v
         v                               +----------------+
+------------------+                     | Exit with      |
| Select role       |                     | access denied   |
| dashboard         |                     +----------------+
+--------+---------+
         |
         v
+------------------+
| Choose component  |
+--+---+---+---+---+
   |   |   |   |   |
   v   v   v   v   v
Members  Memberships  Payments  Attendance  Trainers  Reports
   \        |            |          |          |        /
    \       |            |          |          |       /
     +------+------------+----------+----------+------+
                            |
                            v
                 +----------------------+
                 | Validate and persist |
                 | related data files   |
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 | Return to dashboard  |
                 | or logout            |
                 +----------------------+
```

## 3. Functional Requirements

### 3.1 Authentication and Role Access

- **FR-AUTH-01:** The system shall read user ID, username, password, role, and
  status from `users.txt`.
- **FR-AUTH-02:** The system shall accept login only when credentials match and
  the user status is `ACTIVE`.
- **FR-AUTH-03:** The system shall mask password entry where supported and
  permit no more than three attempts per launch.
- **FR-AUTH-04:** The system shall display an administrator, receptionist, or
  trainer dashboard based on the authenticated role.
- **FR-AUTH-05:** The system shall allow the authenticated user to log out and
  terminate the current session.

### 3.2 Member and Membership Management

- **FR-MEM-01:** The system shall register a member with a unique member ID,
  name, phone, email, and initial status.
- **FR-MEM-02:** The system shall view all members and search by member ID.
- **FR-MEM-03:** The system shall update member details without corrupting
  unrelated records.
- **FR-MEM-04:** The system shall freeze and unfreeze a member by changing the
  stored status.
- **FR-MEM-05:** The system shall assign a membership only to an existing
  member and store its plan, dates, fee, status, and unique membership ID.
- **FR-MEM-06:** The system shall view membership records and identify expired
  active memberships using the current date.
- **FR-MEM-07:** Assigning a membership shall invoke membership payment
  processing; the assignment shall not be presented as successful when the
  required payment fails.

### 3.3 Payment Management

- **FR-PAY-01:** The system shall record a payment for an existing active
  member.
- **FR-PAY-02:** The system shall generate a unique payment ID using the
  existing payment records where automatic generation is used.
- **FR-PAY-03:** The system shall store member ID, positive amount, payment
  date, and method.
- **FR-PAY-04:** The system shall accept only the supported methods `CASH`,
  `CARD`, or `ONLINE`.
- **FR-PAY-05:** The system shall search payment history by member, display all
  payments, and calculate the total recorded amount.
- **FR-PAY-06:** The system shall expose a payment operation usable by
  membership assignment.

### 3.4 Attendance Management

- **FR-ATT-01:** The system shall record an attendance ID, member ID, date,
  check-in time, and check-out time.
- **FR-ATT-02:** Check-in shall be allowed only for an existing active member.
- **FR-ATT-03:** The system shall reject a second open check-in for the same
  member on the same day.
- **FR-ATT-04:** The system shall check out the member's open record and
  preserve other attendance records.
- **FR-ATT-05:** The system shall search by member, display attendance history,
  and produce a daily count of check-ins, completed check-outs, and members
  still inside.

### 3.5 Trainer Management

- **FR-TRN-01:** The system shall register trainers with unique IDs and
  professional details.
- **FR-TRN-02:** The system shall view, search, update, activate, and
  deactivate trainer records.
- **FR-TRN-03:** The system shall assign a member only when both the trainer
  and member exist and the trainer is active.
- **FR-TRN-04:** The system shall reject a duplicate trainer-member
  relationship.
- **FR-TRN-05:** The system shall remove and view trainer-member assignments.

### 3.6 Reports and User Interface

- **FR-REP-01:** The system shall provide member, membership, expired
  membership, payment, and summary reports.
- **FR-REP-02:** Reports shall read current persisted records rather than
  relying only on in-memory session state.
- **FR-REP-03:** The interface shall display clear menus, prompts, success
  messages, validation messages, and file-operation errors.
- **FR-REP-04:** After a component operation, the system shall allow the user
  to return to the dashboard or log out.

## 4. Data Requirements

All files below are persistent unless explicitly identified as a temporary
working file. Records are stored as whitespace-separated fields.

| File | Purpose and principal data |
|---|---|
| `data/users.txt` | Login credentials, role, and active/inactive user status |
| `data/members.txt` | Member ID, name, phone, email, and member status |
| `data/membership_plans.txt` | Available membership plan definitions and fees |
| `data/memberships.txt` | Membership ID, member ID, plan, start/end dates, fee, and status |
| `data/payments.txt` | Payment ID, member ID, amount, date, and payment method |
| `data/attendance.txt` | Attendance ID, member ID, date, check-in time, and check-out time |
| `data/trainers.txt` | Trainer identity, contact details, specialization, schedule, and status |
| `data/trainer_members.txt` | Trainer ID/member ID assignment pairs |
| `data/exercises.txt` | Exercise definitions available to workout planning or reference |
| `data/workout_plans.txt` | Stored workout-plan records |
| `data/diet_plans.txt` | Stored diet-plan records |
| `data/goals.txt` | Member fitness goals and related progress targets |
| `data/progress.txt` | Member progress measurements or updates |
| `data/classes.txt` | Gym class definitions and scheduling records |
| `data/bookings.txt` | Member bookings for available classes or services |
| `data/equipment.txt` | Equipment inventory/reference records |
| `data/maintenance.txt` | Equipment maintenance records and maintenance status |
| `data/notifications.txt` | Notification records for system or member-related messages |
| `data/audit_log.txt` | Audit/event history where operational logging is enabled |

`data/temp.txt` and component-specific temporary files are working files used
while rewriting records. A successful update or deletion shall replace the
original only after the new file has been written successfully. Temporary
files are not business records and should not be used as report inputs.

## 5. System Functions Summary

| Component | Important functions |
|---|---|
| Authentication | `login`, role dashboard selection, logout |
| Members | `memberMenu`, `registerMember`, `viewMembers`, `searchMember`, `updateMember`, `freezeMember`, `unfreezeMember` |
| Memberships | `membershipMenu`, `assignMembership`, `viewMemberships`, `recordPayment`, `viewPayments` |
| Payments | `paymentMenu`, `paymentRecord`, `processMembershipPayment`, `searchPaymentHistory`, `viewPaymentHistory`, `calculateTotalPayments` |
| Attendance | `attendanceMenu`, `recordCheckIn`, `recordCheckOut`, `searchAttendance`, `viewAttendanceHistory`, `dailyAttendanceSummary` |
| Trainers | `trainerMenu`, `registerTrainer`, `viewTrainers`, `searchTrainer`, `updateTrainer`, `changeTrainerStatus`, `assignMemberToTrainer`, `removeMemberFromTrainer`, `viewTrainerMembers` |
| Reports | `reportsMenu`, `memberReport`, `membershipReport`, `expiredMembershipReport`, `paymentReport`, `summaryReport` |
| Interface/system | `printLogo`, `displayDateTime`, `getDateTime`, `clearScreen`, `clearScreen2s` |

## 6. Input Validation

### 6.1 Component-Level Validation

- **Authentication:** required credentials, matching records, active status,
  and attempt limit.
- **Members:** non-empty fields, valid identifier format, duplicate-ID
  detection, and supported status transitions.
- **Memberships:** existing member, unique membership ID, valid date range,
  valid plan, and non-negative/positive fee according to the operation.
- **Payments:** existing active member, positive numeric amount, valid calendar
  date, and method in the supported set.
- **Attendance:** existing active member, valid current/open-record state,
  duplicate same-day check-in prevention, and valid check-out target.
- **Trainers:** unique trainer ID, valid details, supported status, existing
  active trainer for assignment, existing member, and duplicate assignment
  prevention.
- **Menus:** numeric menu choices, valid range, and recovery from non-numeric
  input without terminating the program unexpectedly.

### 6.2 Cross-Component Validation

- A membership, payment, attendance entry, or trainer assignment must refer to
  an existing member.
- Payments and check-ins must reject frozen/inactive members.
- A membership payment must be associated with the member and amount being
  assigned.
- Trainer assignments must refer to an existing active trainer and an existing
  member.
- Reports must handle missing or empty data files without treating a failed
  file read as a successful report.
- Identifier generation must scan existing records to avoid collisions.
- Updates and deletions must preserve records that do not match the selected
  identifier.

## 7. Non-Functional Requirements

### 7.1 Usability

- Menus shall use consistent numbering and clearly labelled operations.
- Prompts and errors shall explain what input is expected or why an operation
  was rejected.
- The application shall return users to the relevant menu after an operation.
- Role dashboards shall expose only the operations intended for that role.

### 7.2 Reliability

- Valid records shall remain available after the application exits and restarts.
- File updates shall use temporary-file replacement where records are rewritten.
- A missing, unreadable, or unwritable file shall produce an explicit error
  rather than a false success.
- The application shall preserve unrelated records during updates and deletes.

### 7.3 Data Persistence

- Each accepted record shall be written to its designated file.
- Dates, times, identifiers, statuses, and amounts shall be stored in a format
  that can be read by later sessions.
- Reports shall derive results from persisted files.
- Plain-text storage shall be documented as a project constraint; credentials
  and records are not encrypted.

### 7.4 Maintainability

- Public operations shall be declared in headers and implemented in focused
  source modules.
- Shared structures and validation conventions shall be reused rather than
  duplicated.
- File paths and record formats shall remain consistent across modules.
- New components should integrate through explicit functions and headers.

### 7.5 Performance

- Normal menu operations should complete interactively for the expected small
  gym data volume.
- Searches, totals, and reports may scan text files sequentially.
- Identifier generation and reports should avoid unnecessary repeated scans when
  a single read is sufficient.

### 7.6 Error Handling

- Invalid input shall be rejected with a clear message and an opportunity to
  retry.
- File-open, read, write, rename, and record-update failures shall be reported.
- Failed cross-component operations shall not display a success message.
- Login failure shall terminate with an access-denied message after the allowed
  attempts.

## 8. System Integration

`main.c` is the application entry point. It calls `login`, receives the
authenticated user record, and routes the user to the dashboard for the
appropriate role. The dashboard invokes the top-level menu functions declared
in the component headers.

The components share the following operations and data:

- Membership management calls `processMembershipPayment` in payment
  management, so assigning a paid membership creates a corresponding payment.
- Payment and attendance management read `members.txt` to verify member
  existence and active status.
- Trainer management reads member and trainer files before creating an entry in
  `trainer_members.txt`.
- Reports read members, memberships, payments, and attendance-related data to
  calculate current operational results.
- Shared UI and system helpers provide consistent display, date/time, screen
  clearing, and pause behavior.

The integrated flow is therefore authentication -> role control -> component
operation -> validation against shared records -> persistence -> reportable
state -> return to dashboard or logout.

## 9. Appendices

### Appendix A: Source Layout

```text
include/       Public headers and shared declarations
src/           C implementation modules and main.c
data/          Persistent text records and working files
build/         Built executables and test outputs
docs/          Project reports and requirements documentation
```

### Appendix B: Role Access Summary

| Operation | Administrator | Receptionist | Trainer |
|---|:---:|:---:|:---:|
| Member management | Yes | Limited | No |
| Membership management | Yes | Yes | No |
| Payment management | Yes | Record payment | No |
| Attendance management | Yes | Yes | No |
| Trainer management | Yes | No | Assignment operations |
| Reports | Yes | No | No |

### Appendix C: Key Business Rules

1. Only active authenticated users may enter the system.
2. A frozen member cannot make a payment or check in.
3. A member may not have two open check-ins on the same day.
4. A trainer-member assignment may exist only once.
5. Expiry reporting compares membership end dates with the current date.
6. A failed file operation or related-record validation prevents a success
   result from being shown.

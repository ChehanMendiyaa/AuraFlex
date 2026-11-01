# AuraFlex Gym Management System
## End-User Manual

**Version:** 1.0  
**Date:** 31 October 2026

## 1. About AuraFlex

AuraFlex is a terminal-based gym management application. It helps authorized
staff manage members, memberships, payments, attendance, trainers, trainer
assignments, and reports. The system stores records in text files in the
`data/` directory.

This manual is for administrators, receptionists, and trainers who use the
application. It does not describe source-code maintenance or direct editing of
data files.

## 2. Starting the Application

1. Open a terminal in the AuraFlex project directory.
2. Ensure the `data/` directory is present.
3. Run the compiled AuraFlex executable from the project directory.
4. Wait for the login screen to appear.

The application uses relative paths such as `data/users.txt`. Starting it from
another directory may prevent files from being found.

## 3. Signing In

1. Enter your username when prompted.
2. Enter your password.
3. The password is masked where the operating environment supports password
   input.
4. If the credentials match an active user, the appropriate dashboard opens.

You have a maximum of three login attempts. Inactive users and incorrect
credentials are rejected. After the allowed attempts, the application displays
an access-denied message and exits. Contact the system administrator if you
cannot sign in; do not edit `data/users.txt` manually.

## 4. Dashboard Access

### Administrator

The administrator dashboard provides:

1. Member Management
2. Membership Plans / Membership Management
3. Trainer Management
4. Payment Management
5. Attendance Management
6. Reports
7. Logout

### Receptionist

The receptionist dashboard provides:

1. Register Member
2. Manage Membership
3. Record Payment
4. Record Attendance
5. View Members
6. Logout

### Trainer

The trainer dashboard provides:

1. View Assigned Members
2. Assign Member
3. Remove Member
4. Logout

The exact menu label can vary slightly between builds, but the operation and
role permissions remain the same.

## 5. Member Management

Member management is available to administrators and selected receptionist
operations.

### Register a member

1. Open **Member Management** or **Register Member**.
2. Enter a new member ID.
3. Enter the member name, phone number, and email address.
4. Confirm the details when prompted.
5. The member is saved with the initial active status.

Member IDs must be unique. Do not reuse an existing ID.

### View or search members

- Choose **View Members** to display the member list.
- Choose **Search Member** and enter a member ID to find one record.
- Check the displayed status before recording payments or attendance.

### Update member details

1. Choose **Update Member**.
2. Enter the member ID.
3. Enter the replacement details.
4. Confirm the update.

### Freeze or unfreeze a member

Use **Freeze Member** to temporarily change an active member to `FROZEN`.
Use **Unfreeze Member** to restore the member to `ACTIVE`.

A frozen member cannot make a payment or check in for attendance. Existing
records are not deleted when a member is frozen.

## 6. Membership Management

1. Open **Membership Management**.
2. Choose **Assign Membership**.
3. Enter a unique membership ID and an existing member ID.
4. Enter the plan name, start date, end date, and fee.
5. Enter or confirm the payment details when requested.
6. Confirm the operation.

The member must exist. Dates must be valid, and the end date should not be
earlier than the start date. Membership assignment is connected to payment
processing; a failed payment prevents the assignment from being reported as
successful.

Use **View Memberships** to review stored memberships. Use the reports menu to
identify active memberships whose end dates have passed.

## 7. Payment Management

### Record a payment

1. Open **Payment Management**.
2. Choose **Record Payment**.
3. Enter the member ID.
4. Enter a positive payment amount.
5. Enter a valid payment date.
6. Select one supported method: `CASH`, `CARD`, or `ONLINE`.
7. Confirm the payment.

The member must exist and be active. Payment IDs are generated from existing
payment records when automatic ID generation is used.

### Review payments

- **Search Payment History:** find payments associated with a member.
- **View All Payments:** display all recorded payment transactions.
- **Calculate Total Payments:** calculate the total amount stored in the
  payment file.

## 8. Attendance Management

### Record check-in

1. Open **Attendance Management**.
2. Choose **Record Check-In**.
3. Enter the member ID.
4. Confirm the member is active.
5. The system records the current date and time and generates an attendance ID.

A member cannot check in if the member does not exist, is frozen, or already
has an open check-in for the same day.

### Record check-out

1. Choose **Record Check-Out**.
2. Enter the member ID.
3. The system finds the member's open attendance record and records the current
   check-out time.

A check-out cannot be recorded when there is no open check-in.

### Review attendance

- **Search Attendance:** search records by member ID.
- **View Attendance History:** display stored attendance records.
- **Daily Attendance Summary:** view check-ins, completed check-outs, and
  members who are still inside.

## 9. Trainer Management

Trainer management is available to administrators.

### Register or update a trainer

1. Open **Trainer Management**.
2. Choose **Register Trainer**.
3. Enter a unique trainer ID and the trainer's professional details.
4. Save the record.

Use **View Trainers**, **Search Trainer**, or **Update Trainer** to review or
change trainer records. Use **Change Trainer Status** to activate or deactivate
a trainer.

### Assign a member to a trainer

1. Choose **Assign Member to Trainer**.
2. Enter the trainer ID.
3. Enter the member ID.
4. Confirm the assignment.

The trainer must exist and be active, the member must exist, and the same
assignment must not already be present.

### Remove or view assignments

- Choose **View Trainer Members** to display assignments.
- Choose **Remove Member From Trainer** and provide the relevant IDs to remove
  an assignment.

Removing an assignment does not delete the trainer or member.

## 10. Reports

Reports are available to administrators.

- **Member Report:** displays member records.
- **Membership Report:** displays membership records.
- **Expired Membership Report:** identifies expired active memberships.
- **Payment Report:** displays payment information and totals.
- **Summary Report:** combines important system counts and totals.

Reports use the saved files in `data/`. If a recently entered record is not
shown, confirm that the previous operation displayed a success message.

## 11. Input Rules

Follow these rules when entering information:

- Use unique IDs for members, trainers, memberships, and assignments.
- Enter numeric menu choices only.
- Enter positive amounts for payments.
- Enter valid calendar dates in the format requested by the prompt.
- Use only `CASH`, `CARD`, or `ONLINE` for payment methods.
- Use existing member IDs for memberships, payments, attendance, and trainer
  assignments.
- Do not use spaces in fields stored as whitespace-separated text. Use an
  underscore where a multi-word value is required.
- Do not close the terminal while a record is being written.

Invalid input should produce a message and allow you to retry. A failed
operation should not be treated as completed.

## 12. Returning to Menus and Logging Out

After an operation, follow the prompt to press Enter or return to the menu.
Use the **Return** option to leave a submenu. Select **Logout** from the
dashboard when finished.

Always log out before closing the terminal, especially when using a shared
computer.

## 13. Troubleshooting

| Problem | Recommended action |
|---|---|
| Login is rejected | Check username, password, spelling, and account status. Contact an administrator after three failed attempts. |
| A file cannot be opened | Confirm the application was started from the project directory and that `data/` exists. |
| Member ID is rejected | Confirm that the member exists, is active where required, and that the ID is typed exactly. |
| Duplicate ID message | Enter a new identifier that is not already stored. |
| Payment is rejected | Use an active member, a positive amount, a valid date, and `CASH`, `CARD`, or `ONLINE`. |
| Check-in is rejected | Confirm the member is active and does not already have an open check-in today. |
| Check-out is rejected | Confirm that the member checked in and has not already checked out. |
| Assignment is rejected | Confirm that the trainer is active, the member exists, and the assignment is not duplicated. |
| Newly saved data is missing | Return to the menu after the success message and reopen the relevant view or report. |
| Screen formatting looks incorrect | Resize the terminal or use a compatible command-line environment. |

Do not manually modify or delete data files to solve an error. Contact the
system maintainer, because changes can break record formats and cross-component
relationships.

## 14. Data and Security Notes

- Records are stored as plain text and are not encrypted.
- Passwords are stored in the user data file according to the project design.
- Restrict access to the project folder and `data/` directory.
- Keep backups of the `data/` directory before maintenance or testing.
- Do not share passwords or copy real member information into test files.

## 15. Quick Reference

```text
Start application
  -> Sign in
  -> Select the role dashboard
  -> Choose a component
  -> Enter and validate details
  -> Confirm success
  -> Return to dashboard
  -> Log out
```

For technical details, source layout, and implementation information, see
`docs/FINAL_PROJECT_REPORT.md`. For formal system requirements, see
`docs/SRS.md`.

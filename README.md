# Task Management System

## Overview

This project is a **Task Management System** developed in C as part of a university project. The system allows users to manage tasks effectively by providing functionalities such as adding tasks, displaying tasks, modifying details, and more. Tasks are prioritized and assigned to employees, ensuring efficient organization.

## Features

1. **Add Tasks:** Create new tasks with details such as description, assignee, priority, creation date, and deadline.
2. **View Tasks:** Display a list of all tasks currently stored.
3. **Remove Tasks:** Delete tasks by their unique ID.
4. **Modify Task Details:** Edit task description, assignee, and priority.
5. **Sort Tasks:** Arrange tasks by priority or deadline for better visualization.
6. **Save and Load Tasks:** Persist task data across sessions using file storage.

## Installation

To compile and run the Task Management System, follow these steps:

1. Ensure you have a C compiler (e.g., GCC) installed.
2. Clone the repository or download the source code files.
3. Open a terminal and navigate to the project directory.
4. Compile the program:
   ```bash
   gcc -o task_manager task_manager.c
5. Run the program: 
     ```bash
   ./task_manager

## Usage
1. Run the program and follow the on-screen menu options.
2. Use the numerical options to navigate and perform actions:
- 1 to add a task.
- 2 to view all tasks.
- 3 to remove a task by its ID.
- 4 to modify a task's details.
- 5 to sort tasks.
- 6 to save tasks to a file.
- 7 to load tasks from a file.
- 0 to exit the program.

## File Structure
-- task_manager.c: Main source code file.
-- tasks.txt: File used to save and load task data (created automatically if it doesn't exist).


// Traistaru Dragos-Andrei 313CA

# Browser--HW4

## Code Structure and Key Functions

# 1. File: `tema1.c`
This is the main source file that contains the browser implementation. It defines all the commands and operations that can be performed on the browser.

The file `utils.h` is essential for modularity, providing access to all constants and shared structures across various parts of the program.

# 2. Basic Structures
`page_t`
- Represents a web page, having an ID, a URL, and a dynamically allocated description.

`tab_t`
- Represents an open tab in the browser, containing the current page and two stacks for forward/backward navigation.

`tabList_t`
- Implementation of a doubly linked list that links all the existing tabs in the browser.

`browser_t`
- The main structure that represents the browser. It contains the active tab and the complete list of tabs.

`my_stack_t`
- A stack structure used to keep track of the pages visited in tabs.

# 3. Implementation Details
# Reading the page vector (function `read_page`)
- Dynamically allocates memory for the page vector and reads the page data from the file.

# Creating the browser (function `create_browser`)
- Dynamically allocates memory for the browser structure and creates a sentinel for the tab list.

# Reading instructions (function `read_instr`)
- Reads each instruction one by one and applies it to the browser.

# Creating a tab (function `add_tab`)
- Dynamically allocates memory for the respective tab and adds it to the browser, making it the current tab.

# Closing a tab (function `close_tab`)
- Removes the current tab from the tab list and deallocates its memory, as well as the memory of its stacks.
- The browser will set the current tab to the one located to the left of the closed tab.

# Opening a specific tab (function `open_tab`)
- The tab with the specified ID is opened in the browser.

# Opening the next/previous tab (functions `next_tab`, and `prev_tab`)
- Opens the next/previous tab relative to the current tab in the browser.

# Opening a page (function `swap_page`)
- Frees the Forward stack of the current tab and opens the page with the specified ID.
- The previous page is stored in a Backward stack.

# Going back to the previous page (function `back_page`)
- Adds the current page to the Forward stack and sets the current tab to the first element in the Backward stack.

# Moving forward to the next page (function `next_page`)
- Adds the current page to the Backward stack and sets the current tab to the first element in the Forward stack.

# Memory Management
- The program uses dynamic memory allocation for the browser, tabs, page vector, Backward and Forward stacks, and page descriptions.
- Functions like `free_stack`, `free_browser`, `free_page` are used to release resources when they are no longer needed, preventing memory leaks.

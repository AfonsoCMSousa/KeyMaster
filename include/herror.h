
#ifndef HERROR_H
#define HERROR_H

#include <setjmp.h>

// Macro to start a try block (exception handling context)
// @warning Variables `_ex_buf` and `_ex_code` are created in the current scope. Avoid name collisions.
// @warning Ensure every `try` is followed by `catch` and `end_try` to prevent scope issues.
#define try                                                                                     \
    do                                                                                          \
    {                                                                                           \
        jmp_buf _ex_buf;            /* Buffer to store the environment for long jump */         \
        volatile int _ex_code;      /* Variable to store the exception code */                  \
        _ex_code = setjmp(_ex_buf); /* Save the calling environment for later use by longjmp */ \
        if (_ex_code == 0)          /* If no exception has occurred, execute the try block */

// Macro to define a catch block for a specific exception code
// @param exception The exception code to catch (must match the value passed to `throw`)
// @warning Variables declared inside `try` may not be accessible here due to block scope limitations.
#define catch(exception) else if (_ex_code == (exception))

// Macro to throw an exception, unwinding execution to the nearest `try` block
// @param exception The exception code to propagate (non-zero integer)
// @warning Undefined behavior if called outside a `try` block or after `end_try`.
#define throw(exception) longjmp(_ex_buf, (exception))

// Macro to terminate the try-catch block
// @warning Required to close the `do-while` structure. Do not omit.
#define end_try \
    }           \
    while (0)

/**
 * @enum Exception
 * @brief Represents different types of exceptions that can occur in the application.
 *
 * This enumeration defines various exception types that the application can handle.
 * Each enumerator represents a specific kind of exception that might be encountered
 * during the execution of the program.
 */
enum Exception
{
    // General exceptions
    INVALID_ARGUMENT = 1, // Invalid parameter passed to function
    INDEX_OUT_OF_BOUNDS,  // Array/collection access out of valid range
    NULL_POINTER,         // Unexpected null pointer encountered
    DIVISION_BY_ZERO,     // Attempted division by zero
    MEMORY_ALLOC_FAILURE, // Failed to allocate dynamic memory

    // I/O related exceptions
    FILE_NOT_FOUND = 100, // File could not be located
    FILE_READ_ERROR,      // Error occurred while reading from file
    FILE_WRITE_ERROR,     // Error occurred while writing to file
    PERMISSION_DENIED,    // Insufficient permission for operation
    IO_OPERATION_FAILED,  // General I/O operation failure

    // Network related exceptions
    SOCKET_CREATION_FAILED = 200, // Failed to create a network socket
    CONNECTION_FAILED,            // Network connection attempt failed
    CONNECTION_TIMEOUT,           // Connection timed out
    HOST_UNREACHABLE,             // Remote host unreachable
    DATA_TRANSMISSION_ERROR,      // Error transmitting/receiving data

    // Data processing exceptions
    PARSE_ERROR = 300,  // Error parsing data (e.g., JSON, XML, etc.)
    ENCODING_ERROR,     // Error encoding data
    DECODING_ERROR,     // Error decoding data
    UNSUPPORTED_FORMAT, // Unsupported file or data format
    DATA_OVERFLOW,      // Overflow during arithmetic or data operation
    DATA_UNDERFLOW,     // Underflow during arithmetic or data operation
    DATA_CORRUPTION,    // Data integrity check failed

    // Multithreading/concurrency exceptions
    THREAD_CREATION_FAILED = 400, // Failed to create a thread
    THREAD_JOIN_FAILED,           // Failed to join a thread
    MUTEX_LOCK_FAILED,            // Failed to acquire a mutex lock
    DEADLOCK_DETECTED,            // Deadlock condition detected
    THREAD_POOL_EXHAUSTED,        // No available threads in a thread pool

    // Resource management exceptions
    RESOURCE_NOT_AVAILABLE = 500, // Resource not available for use
    RESOURCE_LEAK_DETECTED,       // Resource was not properly released
    HANDLE_INVALID,               // Invalid resource handle
    CONFIGURATION_ERROR,          // Invalid or missing configuration

    // Custom application exceptions
    FEATURE_NOT_IMPLEMENTED = 600, // Feature is not implemented yet
    OPERATION_ABORTED,             // Operation was aborted by the user or system
    TIMEOUT_ERROR,                 // Operation timed out
    INVALID_STATE,                 // Invalid state for operation
    UNSUPPORTED_OPERATION,         // Operation not supported in current context
    VERSION_MISMATCH,              // Version mismatch detected

    // Security-related exceptions
    AUTHENTICATION_FAILED = 700, // User authentication failed
    AUTHORIZATION_FAILED,        // User not authorized for operation
    DATA_ENCRYPTION_FAILED,      // Data encryption failed
    DATA_DECRYPTION_FAILED,      // Data decryption failed
    INSECURE_OPERATION,          // Operation deemed insecure
    TOKEN_EXPIRED,               // Security token expired or invalid
    ACCESS_VIOLATION,            // Unauthorized memory access or system access

    // Reserved for user-defined exceptions
    USER_DEFINED_START = 1000 // Start of user-defined exceptions
};

#endif // HERROR_H
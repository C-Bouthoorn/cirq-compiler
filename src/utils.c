#include "utils.h"

#include "benchmark.h"


int min(int a, int b) {
    if (a < b) return a;
    return b;
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}


#define ABS(x) ( ((x) < 0) ? -(x) : (x) )


char *__LEFT_PAD_SPACE(char *dest, double x, unsigned int width) {
	// Calculate width of number
	unsigned int num_width = 0;

	for (int y = (int) ABS(x); y >= 1; y /= 10) {
		num_width++;
	}

	if (x < 0) {
		num_width++;
	}

	// Add spaces
	if (width > num_width) {
		sprintf(dest, "%*s%.f", width - num_width, " ", x);
	}
	else {
		sprintf(dest, "%.f", x);
	}


	return dest;
}


bool isnumber(char *str) {
    FUNC_START();
    assert(str != NULL);

    size_t len = strlen(str);
    size_t i = 0;

    // Default base is decimal
    uint8_t base = 10;

    // Empty strings are no numbers
    if (len == 0) return false;

    // Check different bases (binary, octal, decimal or hexadecimal)
    if (str[i] == 'b' || str[i] == 'o' || str[i] == 'd' || str[i] == 'h') {
        if (str[i] == 'b') base = 2;
        else if (str[i] == 'o') base = 8;
        else if (str[i] == 'd') base = 10;
        else if (str[i] == 'h') base = 16;

        i++;
    }

    // Check sign
    if (str[i] == '-' || str[i] == '+') {
        i++;
    }

    for (; i < len; i++) {
        char digit = str[i];

        // Make sure everything fits its base

        if (base == 2 && !(digit >= '0' && digit <= '1')) {
            FUNC_END();
            return false;
        }
        else if (base == 8 && !(digit >= '0' && digit <= '7')) {
            FUNC_END();
            return false;
        }
        else if (base == 10 && !(digit >= '0' && digit <= '9')) {
            FUNC_END();
            return false;
        }
        else if (base == 16 && !((digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'f') || (digit >= 'A' && digit <= 'F'))) {
            FUNC_END();
            return false;
        }
    }


    FUNC_END();
    return true;
}


int8_t tonumber(char *str) {
    FUNC_START();

    assert(str != NULL);

    if (! isnumber(str)) {
        // Make sure input is actually a number
        fprintf(stderr, "%s is not a number\n", str);
        assert(false);
    }

    int8_t num = 0;
    uint8_t base = 10;
    bool is_positive = true;

    size_t len = strlen(str), i = 0;

    // Check base (binary, octal, decimal or hexadecimal)
    if (str[i] == 'b' || str[i] == 'o' || str[i] == 'd' || str[i] == 'h') {
        if (str[i] == 'b') base = 2;
        else if (str[i] == 'o') base = 8;
        else if (str[i] == 'd') base = 10;
        else if (str[i] == 'h') base = 16;

        i++;
    }

    // Check sign
    if (str[i] == '-' || str[i] == '+') {
        if (str[i] == '-') {
            is_positive = false;
        }

        i++;
    }


    // Convert all remaining characters
    for (; i < len; i++) {
        char c = str[i];
        int value;

        if (c >= '0' && c <= '9') {
            value = c - '0';
        }
        else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        }
        else {
            fprintf(stderr, "Failed to convert number: invalid base %u for char %c\n", base, c);
            FUNC_END();
            exit(1);
        }

        num *= base;
        num += value;
    }


    // If number is negative, multiply with -1
    if (! is_positive) {
        num *= -1;
    }


    FUNC_END();
    return num;
}


FILE *open_file_or_std(char *filename, char *mode, FILE *std) {
    FUNC_START();

    if (strcmp(filename, "-") == 0) {
        FUNC_END();
        return std;
    }

    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        FUNC_END();
        exit(2);
    }

    FUNC_END();
    return file;
}


#ifdef ENABLE_DEBUG
    /// A wrapper like printf which prints messages to the debugging stream (3)
    int debugf(const char *format, ...) {
        if (debug_stream == (FILE*) -1) {
            return 0;
        }

        // Open debug_stream if it's closed
        if (debug_stream == NULL) {
            debug_stream = fdopen(3, "w");
        }

        // On error: exit
        if (debug_stream == NULL) {
            fprintf(stderr, "fd3 with debugging information closed; continuing without debugging output\n");
            debug_stream = (FILE*) -1;
            return 0;
        }

        // Print everything
        va_list arg;
        int done;

        va_start(arg, format);

        // Ignore format-nonliteral for this line
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-nonliteral"
            done = vfprintf(debug_stream, format, arg);
        #pragma GCC diagnostic pop

        va_end(arg);

        return done;
    }


    FILE *get_debug_stream(void) {
        debugf("");

        if (debug_stream == (FILE*) -1) {
            return NULL;
        }

        return debug_stream;
    }
#endif

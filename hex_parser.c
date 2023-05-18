#include <ADuC7026.H>
#include "Flash.h"
#include "IRQ_FIQ.h"
#include "UART.h"
#include "main.h"


#define MAX_HEX_LINES 128  // Maximum number of lines in the hex file

char hex_lines[MAX_HEX_LINES][40];  // Array to store the hex lines
volatile int num_hex_lines = 0;              // Number of hex lines read
int memory[3000];                  // Array to store the memory contents
int data_buffer[40];               // Buffer to store the data bytes


void parse_hex_file() {
    // Read the hex file from serial
    bool end_of_record_found = false;
    bool start_of_record_found = false;
    static int line_index = 0;
    while (Serial.available() && !end_of_record_found) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
        //if (c == '\r') {
            hex_lines[num_hex_lines][line_index] = '\0'; // Null terminate the line
            num_hex_lines++;                     // Increment the number of lines
            line_index = 0;
            //Serial.print("Data Lines: ");
            //Serial.println(num_hex_lines);
            if(hex_lines[num_hex_lines-1][6] == '0' && hex_lines[num_hex_lines-1][7] == '4'){
                num_hex_lines = 0;
                Serial.println("Start");
            }
            else if(hex_lines[num_hex_lines-1][6] == '0' && hex_lines[num_hex_lines-1][7] == '1'){
                end_of_record_found = true;
                num_hex_lines--;                     // Decrement the number of lines
                Serial.println("End");
            }
        } 
        else {
            // Append the character to the current line
            // Ignore non-hex characters
            if (c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f') {
                hex_lines[num_hex_lines][line_index++] = c;
            }
        }

        // Check if we've reached the maximum number of lines
        if (num_hex_lines >= MAX_HEX_LINES) {
            Serial.println("Error: Maximum number of hex lines reached");
            return;
        }

       /*
        // Check if we've found the end of record
        if(hex_lines[num_hex_lines - 1][6] == '0' && hex_lines[num_hex_lines - 1][7] == '1'){
            end_of_record_found = true;
        } 
        if(hex_lines[num_hex_lines - 1][6] == '0' && hex_lines[num_hex_lines - 1][7] == '4'){
            start_of_record_found = true;
            start_found++;
        }
        else {
            end_of_record_found = false;
            start_of_record_found = false;
        }*/
    }

    /*
    // Display the hex lines
    for (int i = 0; i < num_hex_lines; i++) {
        Serial.print("Line: ");
        Serial.println(i);
        for(int j = 0; j < strlen(hex_lines[i]); j++) {
             Serial.print(hex_lines[i][j]);//, HEX);
             Serial.print(" ");
        }
    }*/
    

    if(end_of_record_found) {
        end_of_record_found = false;
        Serial.print("Data Lines: ");
        Serial.println(num_hex_lines);
        // Parse the hex lines and write the data bytes to memory
        for (int i = 0; i < num_hex_lines; i++) {
            //Serial.println(hex_lines[i], HEX);
            parse_hex_line(hex_lines[i]);
        }
    }
}

void parse_hex_line(char *line) {
    // Ignore non-data records
    if (line[7] != '0' && line[8] != '0') {
        Serial.println("Return");
        return;
    }

    // Get the number of data bytes
    int num_bytes = hex_to_int(line[0]) * 16 + hex_to_int(line[1]);
    //Serial.print("Num_Bytes: ");
    //Serial.println(num_bytes);
    
    // Get the address offset
    int offset = hex_to_int(line[2]) * 4096 + hex_to_int(line[3]) * 256 + hex_to_int(line[4]) * 16 + hex_to_int(line[5]);
    //Serial.print("Offset: ");
    //Serial.println(offset);
    
    // Copy the data bytes to the buffer
    for (int i = 0; i < num_bytes; i++) {
        data_buffer[i] = hex_to_int(line[8 + i * 2]) * 16 + hex_to_int(line[9 + i * 2]);
    }

    // Write the data bytes to memory
    for (int i = 0; i < num_bytes; i++) {
        memory[offset + i] = data_buffer[i];
    }
    
    // Print the data bytes to serial
    for (int i = 0; i < num_bytes; i++) {
        Serial.print(memory[offset + i] < 16 ? "0" : "");
        Serial.print(memory[offset + i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
}

int hex_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return -1;
    }
}


void setup() {
    Serial.begin(600);
    Serial.println("Testing Hex file\n");
}

void loop() {
    /*Serial.print("Start found: ");
    Serial.println(start_found);
    Serial.print("End found: ");
    Serial.println(end_found);*/
     // Wait for input from serial
    while (!Serial.available()) {}

    // Parse the hex file
    parse_hex_file();

    // Clear the hex lines buffer
    //num_hex_lines = 0;
}

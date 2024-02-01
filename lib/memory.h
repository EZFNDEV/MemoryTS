#pragma once
#ifndef MEMORY_H
#define MEMORY_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <TlHelp32.h>

class memory {
public:
  memory();
  ~memory();
  std::vector<MEMORY_BASIC_INFORMATION> getRegions(HANDLE hProcess);

  template <class dataType>
  dataType readMemory(HANDLE hProcess, DWORD64 address) {
    dataType cRead;
    ReadProcessMemory(hProcess, (LPVOID)address, &cRead, sizeof(dataType), NULL);
    return cRead;
  }

  BOOL readBuffer(HANDLE hProcess, DWORD64 address, SIZE_T size, char* dstBuffer) {
    return ReadProcessMemory(hProcess, (LPVOID)address, dstBuffer, size, NULL);
  }

  char readChar(HANDLE hProcess, DWORD64 address) {
    char value;
    ReadProcessMemory(hProcess, (LPVOID)address, &value, sizeof(char), NULL);
    return value;
	}

  BOOL readString(HANDLE hProcess, DWORD64 address, std::string* pString) {
    int length = 0;
    int BATCH_SIZE = 256;
    char* data = (char*) malloc(sizeof(char) * BATCH_SIZE);
    while (length <= BATCH_SIZE * 4096) {
      BOOL success = readBuffer(hProcess, address + length, BATCH_SIZE, data);

      if (success == 0) {
        free(data);
        break;
      }

      for (const char* ptr = data; ptr - data < BATCH_SIZE; ++ptr) {
        if (*ptr == '\0') {
          length += ptr - data + 1;

          char* buffer = (char*) malloc(length);
          readBuffer(hProcess, address, length, buffer);

          *pString = std::string(buffer);

          free(data);
          free(buffer);

          return TRUE;
        }
      }

      length += BATCH_SIZE;
    }

    return FALSE;
  }

  template <class dataType>
  void writeMemory(HANDLE hProcess, DWORD64 address, dataType value) {
    WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(dataType), NULL);
  }

  template <class dataType>
  void writeMemory(HANDLE hProcess, DWORD64 address, dataType value, SIZE_T size) {
	  LPVOID buffer = value;

	  if (typeid(dataType) != typeid(char*)) {
		  buffer = &value;
	  }

	  WriteProcessMemory(hProcess, (LPVOID)address, buffer, size, NULL);
  }

  // Write String, Method 1: Utf8Value is converted to string, get pointer and length from string
  // template <>
  // void writeMemory<std::string>(HANDLE hProcess, DWORD address, std::string value) {
  //  WriteProcessMemory(hProcess, (LPVOID)address, value.c_str(), value.length(), NULL);
  // }

  // Write String, Method 2: get pointer and length from Utf8Value directly
  void writeMemory(HANDLE hProcess, DWORD64 address, char* value, SIZE_T size) {
    WriteProcessMemory(hProcess, (LPVOID)address, value, size, NULL);
  }
};
#endif
#pragma once
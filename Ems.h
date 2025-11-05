#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "employees.txt"
#define MAX_LINE_LENGTH 512

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "123456"

#define INPUT_BUFFER 256

typedef struct {
    char id[32];
    char name[64];
    char gender[16];
    int age;
    char education[64];
    double salary;
    char address[128];
} Employee;

typedef struct {
    Employee* data;
    size_t size;
    size_t capacity;
} EmployeeList;

//函数声明
void init_list(EmployeeList* list);
void free_list(EmployeeList* list);
int ensure_capacity(EmployeeList* list, size_t capacity);
void read_line(const char* prompt, char* buffer, size_t size);
int parse_int(const char* text, int* out_value);
int parse_double(const char* text, double* out_value);
void display_employee(const Employee* emp);
void display_header(void);
int find_employee_by_id(const EmployeeList* list, const char* id);
void load_employees(EmployeeList* list, const char* filename);
int save_employees(const EmployeeList* list, const char* filename);
void browse_employees(const EmployeeList* list);
void add_employee(EmployeeList* list);
void delete_employee(EmployeeList* list);
void modify_employee(EmployeeList* list);
void search_by_id(const EmployeeList* list);
void search_by_name(const EmployeeList* list);
int compare_salary(const void* a, const void* b);
void sort_by_salary(EmployeeList* list);
int admin_login(void);
void admin_menu(EmployeeList* list);
void user_menu(const Employee* emp);
void user_login(const EmployeeList* list);


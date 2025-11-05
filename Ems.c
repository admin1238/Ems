#define _CRT_SECURE_NO_WARNINGS 1
#include "Ems.h"
//初始化员工列表
//size 指存储的员工数量，capacity指数组的总大小
 void init_list(EmployeeList* list) {
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}
 //释放data指向的动态内存，防止内存泄露
 void free_list(EmployeeList* list) {
    free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}
 //动态数组分配内存
int ensure_capacity(EmployeeList* list, size_t capacity) {
    if (list->capacity >= capacity) {
        return 1;
    }
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    
    //确保新容量至少达到要求的容量，如果翻倍后还不够，继续翻倍直到满足要求
    while (new_capacity < capacity) {
        new_capacity *= 2;
    }
    Employee* new_data = realloc(list->data, new_capacity * sizeof(Employee));
    if (!new_data) {
        fprintf(stderr, "内存不足，无法分配更多空间。\n");
        return 0;
    }
    list->data = new_data;
    list->capacity = new_capacity;
    return 1;
}

//标准输入读取一行文本，并去除末尾的换行符。
void read_line(const char* prompt, char* buffer, size_t size) {
    if (prompt) {
        printf("%s", prompt);
    }
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';//strcspn(buffer, "\n") 返回缓冲区中第一个换行符的位置
}

//将字符串转换为整数
int parse_int(const char* text, int* out_value) {
    char* end_ptr;
    long value = strtol(text, &end_ptr, 10);//将字符串转化为整数（十进制）
    if (end_ptr == text || *end_ptr != '\0') {
        return 0;
    }
    *out_value = (int)value;
    return 1;
}

//将字符串转换为双精度浮点数
int parse_double(const char* text, double* out_value) {
    char* end_ptr;
    double value = strtod(text, &end_ptr);
    if (end_ptr == text || *end_ptr != '\0') {
        return 0;
    }
    *out_value = value;
    return 1;
}

void display_employee(const Employee* emp) {
    printf("%-12s %-10s %-6s %-4d %-10s %-10.2f %-20s\n",
        emp->id,
        emp->name,
        emp->gender,
        emp->age,
        emp->education,
        emp->salary,
        emp->address);
}

void display_header(void) {
    printf("%-12s %-10s %-6s %-4s %-10s %-10s %-20s\n",
        "编号", "姓名", "性别", "年龄", "学历", "工资", "地址");
    printf("--------------------------------------------------------------------------------\n");
}

int find_employee_by_id(const EmployeeList* list, const char* id) {
    for (size_t i = 0; i < list->size; ++i) {
        if (strcmp(list->data[i].id, id) == 0) {
            return (int)i;
        }
    }
    return -1;
}

//从文件中加载员工数据到内存中的员工列表。数据持久化读取操作
void load_employees(EmployeeList* list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))   {
        if (line[0] == '\0' || line[0] == '\n') {
            continue;
        }   
        Employee emp;
        char* token = strtok(line, "|\n");
        if (!token) continue;
        strncpy(emp.id, token, sizeof(emp.id) - 1);
        emp.id[sizeof(emp.id) - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(emp.name, token, sizeof(emp.name) - 1);
        emp.name[sizeof(emp.name) - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(emp.gender, token, sizeof(emp.gender) - 1);
        emp.gender[sizeof(emp.gender) - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        emp.age = atoi(token);

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(emp.education, token, sizeof(emp.education) - 1);
        emp.education[sizeof(emp.education) - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        emp.salary = atof(token);

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(emp.address, token, sizeof(emp.address) - 1);
        emp.address[sizeof(emp.address) - 1] = '\0';

        if (!ensure_capacity(list, list->size + 1)) {
            break;
        }
        list->data[list->size++] = emp;
    }
    fclose(file);
}

int save_employees(const EmployeeList* list, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("无法打开文件进行写入");
        return 0;
    }
    for (size_t i = 0; i < list->size; ++i) {
        const Employee* emp = &list->data[i];
        fprintf(file, "%s|%s|%s|%d|%s|%.2f|%s\n",
            emp->id,
            emp->name,
            emp->gender,
            emp->age,
            emp->education,
            emp->salary,
            emp->address);
    }
    fclose(file);
    return 1;
}

void browse_employees(const EmployeeList* list) {
    if (list->size == 0) {
        printf("暂无员工信息。\n");
        return;
    }
    display_header();
    for (size_t i = 0; i < list->size; ++i) {
        display_employee(&list->data[i]);
    }
}

 void add_employee(EmployeeList* list) {
    if (!ensure_capacity(list, list->size + 1)) {
        return;
    }
    Employee emp;
    char buffer[INPUT_BUFFER];

    read_line("请输入员工编号: ", emp.id, sizeof(emp.id));
    if (find_employee_by_id(list, emp.id) >= 0) {
        printf("该编号已存在，无法重复添加。\n");
        return;
    }

    read_line("请输入姓名: ", emp.name, sizeof(emp.name));
    read_line("请输入性别: ", emp.gender, sizeof(emp.gender));

    while (1) {
        read_line("请输入年龄: ", buffer, sizeof(buffer));
        if (parse_int(buffer, &emp.age) && emp.age > 0) {
            break;
        }
        printf("请输入正确的年龄。\n");
    }

    read_line("请输入学历: ", emp.education, sizeof(emp.education));

    while (1) {
        read_line("请输入工资: ", buffer, sizeof(buffer));
        if (parse_double(buffer, &emp.salary) && emp.salary >= 0.0) {
            break;
        }
        printf("请输入有效的工资数字。\n");
    }

    read_line("请输入地址: ", emp.address, sizeof(emp.address));

    list->data[list->size++] = emp;
    if (save_employees(list, DATA_FILE)) {
        printf("员工信息已保存。\n");
    }
}

void delete_employee(EmployeeList* list) {
    char id[INPUT_BUFFER];
    read_line("请输入要删除的员工编号: ", id, sizeof(id));
    int index = find_employee_by_id(list, id);
    if (index < 0) {
        printf("未找到该员工。\n");
        return;
    }
    for (size_t i = index; i + 1 < list->size; ++i) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
    if (save_employees(list, DATA_FILE)) {
        printf("员工信息已删除并保存。\n");
    }
}

void modify_employee(EmployeeList* list) {
    char buffer[INPUT_BUFFER];
    char id[INPUT_BUFFER];
    read_line("请输入要修改的员工编号: ", id, sizeof(id));
    int index = find_employee_by_id(list, id);
    if (index < 0) {
        printf("未找到该员工。\n");
        return;
    }
    Employee* emp = &list->data[index];
    printf("当前信息如下:\n");
    display_header();
    display_employee(emp);

    read_line("新姓名(回车保持不变): ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(emp->name, buffer, sizeof(emp->name) - 1);
        emp->name[sizeof(emp->name) - 1] = '\0';
    }

    read_line("新性别(回车保持不变): ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(emp->gender, buffer, sizeof(emp->gender) - 1);
        emp->gender[sizeof(emp->gender) - 1] = '\0';
    }

    while (1) {
        read_line("新年龄(回车保持不变): ", buffer, sizeof(buffer));
        if (strlen(buffer) == 0) {
            break;
        }
        int new_age;
        if (parse_int(buffer, &new_age) && new_age > 0) {
            emp->age = new_age;
            break;
        }
        printf("请输入正确的年龄或直接回车跳过。\n");
    }

    read_line("新学历(回车保持不变): ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(emp->education, buffer, sizeof(emp->education) - 1);
        emp->education[sizeof(emp->education) - 1] = '\0';
    }

    while (1) {
        read_line("新工资(回车保持不变): ", buffer, sizeof(buffer));
        if (strlen(buffer) == 0) {
            break;
        }
        double new_salary;
        if (parse_double(buffer, &new_salary) && new_salary >= 0.0) {
            emp->salary = new_salary;
            break;
        }
        printf("请输入正确的工资或直接回车跳过。\n");
    }

    read_line("新地址(回车保持不变): ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        strncpy(emp->address, buffer, sizeof(emp->address) - 1);
        emp->address[sizeof(emp->address) - 1] = '\0';
    }

    if (save_employees(list, DATA_FILE)) {
        printf("员工信息已更新。\n");
    }
}

void search_by_id(const EmployeeList* list) {
    char id[INPUT_BUFFER];
    read_line("请输入员工编号: ", id, sizeof(id));
    int index = find_employee_by_id(list, id);
    if (index < 0) {
        printf("未找到该员工。\n");
        return;
    }
    display_header();
    display_employee(&list->data[index]);
}

void search_by_name(const EmployeeList* list) {
    char name[INPUT_BUFFER];
    read_line("请输入姓名或姓名片段: ", name, sizeof(name));
    if (strlen(name) == 0) {
        printf("请输入有效的姓名。\n");
        return;
    }

    int found = 0;
    display_header();
    for (size_t i = 0; i < list->size; ++i) {
        if (strstr(list->data[i].name, name) != NULL) {
            display_employee(&list->data[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("未找到匹配的员工。\n");
    }
}

int compare_salary(const void* a, const void* b) {
    const Employee* emp_a = (const Employee*)a;
    const Employee* emp_b = (const Employee*)b;
    if (emp_a->salary < emp_b->salary) return -1;
    if (emp_a->salary > emp_b->salary) return 1;
    return 0;
}

void sort_by_salary(EmployeeList* list) {
    if (list->size < 2) {
        printf("数据不足，无需排序。\n");
        return;
    }
    qsort(list->data, list->size, sizeof(Employee), compare_salary);
    if (save_employees(list, DATA_FILE)) {
        printf("已按工资从低到高排序并保存。\n");
    }
}

 int admin_login(void) {
    char username[INPUT_BUFFER];
    char password[INPUT_BUFFER];
    read_line("请输入管理员账号: ", username, sizeof(username));
    read_line("请输入管理员密码: ", password, sizeof(password));
    if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        printf("管理员登录成功。\n");
        return 1;
    }
    printf("账号或密码错误。\n");
    return 0;
}

void admin_menu(EmployeeList* list) {
    while (1) {
        printf("\n===== 管理员菜单 =====\n");
        printf("1. 浏览所有员工信息\n");
        printf("2. 添加员工信息\n");
        printf("3. 修改员工信息\n");
        printf("4. 删除员工信息\n");
        printf("5. 按编号查询员工\n");
        printf("6. 按姓名查询员工\n");
        printf("7. 按工资排序\n");
        printf("0. 返回主菜单\n");
        printf("请选择: ");

        char choice[INPUT_BUFFER];
        if (!fgets(choice, sizeof(choice), stdin)) {
            clearerr(stdin);
            continue;
        }
        switch (choice[0]) {
        case '1':
            browse_employees(list);
            break;
        case '2':
            add_employee(list);
            break;
        case '3':
            modify_employee(list);
            break;
        case '4':
            delete_employee(list);
            break;
        case '5':
            search_by_id(list);
            break;
        case '6':
            search_by_name(list);
            break;
        case '7':
            sort_by_salary(list);
            break;
        case '0':
            return;
        default:
            printf("无效的选项，请重新输入。\n");
        }
    }
}

void user_menu(const Employee* emp) {
    while (1) {
        printf("\n===== 用户菜单 =====\n");
        printf("1. 查看个人信息\n");
        printf("0. 退出登录\n");
        printf("请选择: ");

        char choice[INPUT_BUFFER];
        if (!fgets(choice, sizeof(choice), stdin)) {
            clearerr(stdin);
            continue;
        }
        switch (choice[0]) {
        case '1':
            display_header();
            display_employee(emp);
            break;
        case '0':
            return;
        default:
            printf("无效的选项，请重新输入。\n");
        }
    }
}

void user_login(const EmployeeList* list) {
    char id[INPUT_BUFFER];
    char name[INPUT_BUFFER];
    read_line("请输入员工编号: ", id, sizeof(id));
    read_line("请输入姓名: ", name, sizeof(name));

    int index = find_employee_by_id(list, id);
    if (index < 0) {
        printf("未找到该员工。\n");
        return;
    }
    if (strcmp(list->data[index].name, name) != 0) {
        printf("姓名与编号不匹配。\n");
        return;
    }
    printf("登录成功。\n");
    user_menu(&list->data[index]);
}
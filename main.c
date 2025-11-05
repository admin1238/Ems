#define _CRT_SECURE_NO_WARNINGS 1
#include "Ems.h"
int main(void) {
    EmployeeList list;
    init_list(&list);
    load_employees(&list, DATA_FILE);

    while (1) {
        printf("\n===== 员工信息管理系统 =====\n");
        printf("1. 管理员登录\n");
        printf("2. 员工登录\n");
        printf("0. 退出系统\n");
        printf("请选择: ");

        char choice[INPUT_BUFFER];
        if (!fgets(choice, sizeof(choice), stdin)) {
            clearerr(stdin);
            continue;
        }
        switch (choice[0]) {
        case '1':
            if (admin_login()) {
                admin_menu(&list);
            }
            break;
        case '2':
            user_login(&list);
            break;
        case '0':
            save_employees(&list, DATA_FILE);
            free_list(&list);
            printf("感谢使用，再见！\n");
            return 0;
        default:
            printf("无效的选项，请重新输入。\n");
        }
    }
}